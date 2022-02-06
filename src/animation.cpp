#include "pch.h"
#include "animation.h"
#include "ui.h"
#include "util.h"

#ifndef GTASA
#include <CAnimationStyleDescriptor.h>
#include <CAnimManager.h>
#include "eAnimations.h"
#include <CAnimBlendAssociation.h>
#endif

#ifdef GTA3
#include <RpAnimBlend.h>
#endif

#ifdef GTAVC
#include "../depend/kiero/minhook/MinHook.h"
#endif

#ifdef GTASA
void Animation::PlayCutscene(std::string& rootKey, std::string& cutsceneId, std::string& interior)
{
    if (Util::IsOnCutscene())
    {
        SetHelpMessage("Another cutscene is running", false, false, false);
        return;
    }

    CPlayerPed* pPlayer = FindPlayerPed();
    if (!pPlayer)
    {
        return;
    }

    m_Cutscene::m_SceneName = cutsceneId;
    Command<Commands::LOAD_CUTSCENE>(cutsceneId.c_str());
    m_Cutscene::m_nInterior = pPlayer->m_nAreaCode;
    pPlayer->m_nAreaCode = std::stoi(interior);
    Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
}

void Animation::PlayParticle(std::string& rootKey, std::string& particle, std::string& dummy)
{
    CPlayerPed* pPlayer = FindPlayerPed();
    if (!pPlayer)
    {
        return;
    }
    CVector pos = pPlayer->GetPosition();

    int handle;
    Command<Commands::CREATE_FX_SYSTEM>(particle.c_str(), pos.x, pos.y, pos.z, 1, &handle);
    Command<Commands::PLAY_FX_SYSTEM>(handle);
    m_Particle::m_nParticleList.push_back(handle);
}


void Animation::RemoveParticle(std::string& ifp, std::string& particle, std::string& dummy)
{
    if (ifp == "Custom")
    {
        m_Particle::m_Data.m_pJson->m_Data["Custom"].erase(particle);
        m_Particle::m_Data.m_pJson->WriteToDisk();
        SetHelpMessage("Particle removed", false, false, false);
    }
    else
    {
        SetHelpMessage("You can only remove custom particles", false, false, false);
    }
}

#elif GTAVC
// Thanks to codenulls(https://github.com/codenulls/)

static auto OLD_CStreaming_RemoveModel = (bool(__cdecl*)(int))0x40D6E0;
static bool NEW_CStreaming_RemoveModel(int modelID)
{
    // Check if it's IFP animation block
    if (modelID >= 7916 && modelID <= 7950)
    {
        // Do not unload the animation block
        return true;
    }
    return OLD_CStreaming_RemoveModel(modelID);
}

void Animation::_PlayAnimation(RpClump* pClump, int animGroup, int animID, float blend)
{
    if (animGroup < CAnimManager::ms_numAnimAssocDefinitions)
    {
        CAnimationStyleDescriptor* pAnimDef = &CAnimManager::ms_aAnimAssocDefinitions[animGroup];
        if (pAnimDef)
        {
            if (!_LoadAnimationBlock(pAnimDef->blockName))
            {
                return;
            }
        }
    }

    CAnimBlendAssociation* pAnimAssoc = RpAnimBlendClumpGetFirstAssociation(pClump);
    while (pAnimAssoc)
    {
        if (pAnimAssoc->m_nAnimId == animID && pAnimAssoc->m_nAnimGroup == animGroup)
        {
            // Destroy the animation
            pAnimAssoc->~CAnimBlendAssociation();
            break;
        }
        pAnimAssoc = RpAnimBlendGetNextAssociation(pAnimAssoc);
    }
    pAnimAssoc = CAnimManager::BlendAnimation(pClump, animGroup, animID, blend);
    pAnimAssoc->m_nFlags = ANIMATION_STARTED | ANIMATION_MOVEMENT;

    if (m_Loop)
    {
        pAnimAssoc->m_nFlags |= ANIMATION_LOOPED;
    }

    if (m_bSecondary)
    {
        pAnimAssoc->m_nFlags |= ANIMATION_PARTIAL;
    }
}

bool Animation::_LoadAnimationBlock(const char* szBlockName)
{
    CAnimBlock* pAnimBlock = CAnimManager::GetAnimationBlock(szBlockName);
    if (pAnimBlock)
    {
        if (!pAnimBlock->bLoaded)
        {
            int animIndex = ((unsigned char*)pAnimBlock - (unsigned char*)CAnimManager::ms_aAnimBlocks) / 32;
            CStreaming::RequestModel(7916 + animIndex, 0x20 | MISSION_REQUIRED | PRIORITY_REQUEST);
            CStreaming::LoadAllRequestedModels(true);
            if (pAnimBlock->bLoaded)
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    return false;
}
#else

void Animation::_PlayAnimation(RpClump* pClump, int animGroup, int animID, float blend)
{
    CAnimBlendAssociation* pAnimStaticAssoc = CAnimManager::GetAnimAssociation((AssocGroupId)animGroup, (AnimationId)animID);
    CAnimBlendAssociation* pAnimAssoc = RpAnimBlendClumpGetFirstAssociation(pClump);
    while (pAnimAssoc)
    {
        if (pAnimAssoc->m_nAnimID == pAnimStaticAssoc->m_nAnimID && pAnimAssoc->m_pAnimBlendHierarchy == pAnimStaticAssoc->m_pAnimBlendHierarchy)
        {
            // Destroy the animation
            pAnimAssoc->FreeAnimBlendNodeArray();
            break;
        }
        pAnimAssoc = RpAnimBlendGetNextAssociation(pAnimAssoc);
    }
    pAnimAssoc = CAnimManager::BlendAnimation(pClump, (AssocGroupId)animGroup, (AnimationId)animID, blend);
    pAnimAssoc->m_nFlags = 0x1 | 0x20;

    if (m_Loop)
    {
        pAnimAssoc->m_nFlags |= 0x2;
    }

    if (m_bSecondary)
    {
        pAnimAssoc->m_nFlags |= 0x10;
    }
}
#endif


void Animation::PlayAnimation(std::string& ifp, std::string& anim, std::string& value)
{
    CPed *pPed = m_PedAnim ? m_pTarget : FindPlayerPed();

    if (!pPed)
    {
        return;
    }

#ifdef GTASA
    int hped = CPools::GetPedRef(pPed);

    if (ifp != "PED")
    {
        Command<Commands::REQUEST_ANIMATION>(ifp.c_str());
        Command<Commands::LOAD_ALL_MODELS_NOW>();
    }

    Command<Commands::CLEAR_CHAR_TASKS>(hped);
    if (m_bSecondary)
    {
        Command<Commands::TASK_PLAY_ANIM_SECONDARY>(hped, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
    }
    else
    {
        Command<Commands::TASK_PLAY_ANIM>(hped, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
    }

    if (ifp != "PED")
    {
        Command<Commands::REMOVE_ANIMATION>(ifp.c_str());
    }

#else
    if (pPed)
    {
        int groupID, animID;
        sscanf(value.c_str(), "%d$%d,", &groupID, &animID);
        _PlayAnimation(pPed->m_pRwClump, groupID, animID, 4.0f);
    }
#endif
}

Animation::Animation()
{
#ifdef GTASA
    Events::processScriptsEvent += [this]
    {
        CPlayerPed* pPlayer = FindPlayerPed();
        if (m_Cutscene::m_bRunning)
        {
            if (Command<Commands::HAS_CUTSCENE_FINISHED>())
            {
                if (!pPlayer)
                {
                    return;
                }

                pPlayer->m_nAreaCode = m_Cutscene::m_nInterior;
                Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
                m_Cutscene::m_nInterior = 0;
                TheCamera.Fade(0, 1);
            }
        }
        else
        {
            if (m_Cutscene::m_SceneName != "" && Command<Commands::HAS_CUTSCENE_LOADED>())
            {
                Command<Commands::START_CUTSCENE>();
                m_Cutscene::m_bRunning = true;
            }
        }

        if (pPlayer && pPlayer->m_pPlayerTargettedPed)
        {
            m_pTarget = pPlayer->m_pPlayerTargettedPed;
        }

        if (m_pTarget && !m_pTarget->IsAlive())
        {
            m_pTarget = nullptr;
        }
    };
#elif GTAVC
    // mov al, 01
    // ret
    // nop (2x)
    patch::SetRaw(0x40C9C0, (void*)"\xB0\x01\xC3\x90\x90", 5);
    // // ret
    // // nop (3x)
    patch::SetRaw(0x404950, (void*)"\xC3\x90\x90\x90", 4);

    MH_CreateHook((void*)0x40D6E0, NEW_CStreaming_RemoveModel, (void**)&OLD_CStreaming_RemoveModel);
    MH_EnableHook((void*)0x40D6E0);
#endif
}

void Animation::ShowPage()
{
    if (ImGui::BeginTabBar("Animation", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        ImGui::Spacing();
        CPlayerPed* pPlayer = FindPlayerPed();
        int hPlayer = CPools::GetPedRef(pPlayer);

        ImGui::Spacing();

        if (ImGui::BeginTabItem("Animation##TABBAR"))
        {
            ImGui::Spacing();
            if (ImGui::Button("Stop animation", Ui::GetSize()))
            {
                if (hPlayer)
                {
#ifdef GTASA
                    Command<Commands::CLEAR_CHAR_TASKS>(hPlayer);
#else
                    _PlayAnimation(pPlayer->m_pRwClump, ANIM_GROUP_MAN, ANIM_MAN_IDLE_STANCE, 4.0f);
#endif
                }
            }
            ImGui::Spacing();

            ImGui::Columns(2, nullptr, false);
            Ui::CheckboxWithHint("Loop", &m_Loop, "Keep playing the animation on repeat");
            Ui::CheckboxWithHint("Secondary", &m_bSecondary, "Player can move while playing the animation");
            ImGui::NextColumn();
#ifdef GTASA
            Ui::CheckboxWithHint("Ped anim", &m_PedAnim, "Play animation on other peds.\nSelect with weapon target.");
#endif
            ImGui::Columns(1);
            ImGui::Spacing();

            if (m_PedAnim && !m_pTarget)
            {
                ImGui::TextWrapped("No player target found. Aim a ped with a weapon to select it for animation player.");
            }
            else
            {
                if (ImGui::CollapsingHeader("Add new"))
                {
                    ImGui::InputTextWithHint("IFP name", "ped", m_nIfpBuffer, INPUT_BUFFER_SIZE);
                    ImGui::InputTextWithHint("Anim name", "cower", m_nAnimBuffer, INPUT_BUFFER_SIZE);
                    ImGui::Spacing();
                    if (ImGui::Button("Add animation", Ui::GetSize()))
                    {
                        m_AnimData.m_pJson->m_Data["Custom"][m_nAnimBuffer] = ("0, " + std::string(m_nIfpBuffer));
                        m_AnimData.m_pJson->WriteToDisk();
                    }
                }
                ImGui::Spacing();

                if (ImGui::BeginChild("Anims Child"))
                {
                    ImGui::Spacing();
                    Ui::DrawJSON(m_AnimData, PlayAnimation, RemoveAnimation);
                    ImGui::EndChild();
                }
            }
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem("Cutscene"))
        {
            ImGui::Spacing();
            if (ImGui::Button("Stop cutscene", Ui::GetSize()))
            {
                if (m_Cutscene::m_bRunning)
                {
                    Command<Commands::CLEAR_CUTSCENE>();
                    m_Cutscene::m_bRunning = false;
                    m_Cutscene::m_SceneName = "";
                    CPlayerPed* player = FindPlayerPed();
                    player->m_nAreaCode = m_Cutscene::m_nInterior;
                    Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
                    m_Cutscene::m_nInterior = 0;
                    TheCamera.Fade(0, 1);
                }
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("Cutscene Child"))
            {
                ImGui::Spacing();
                Ui::DrawJSON(m_Cutscene::m_Data, PlayCutscene, nullptr);
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Particle##TABBAR"))
        {
            ImGui::Spacing();
            if (ImGui::Button("Remove all", Ui::GetSize(2)))
            {
                for (int& p : m_Particle::m_nParticleList)
                {
                    Command<Commands::KILL_FX_SYSTEM>(p);
                }
                m_Particle::m_nParticleList.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove latest", Ui::GetSize(2)))
            {
                Command<Commands::KILL_FX_SYSTEM>(m_Particle::m_nParticleList.back()); // stop if anything is running
                m_Particle::m_nParticleList.pop_back();
            }
            ImGui::Spacing();
            if (Ui::CheckboxBitFlag("Invisible player", pPlayer->m_nPedFlags.bDontRender))
            {
                pPlayer->m_nPedFlags.bDontRender = (pPlayer->m_nPedFlags.bDontRender == 1) ? 0 : 1;
            }
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Add new"))
            {
                ImGui::InputTextWithHint("Particle name", "kkjj_on_fire", m_Particle::m_NameBuffer, INPUT_BUFFER_SIZE);
                ImGui::Spacing();
                if (ImGui::Button("Add animation", Ui::GetSize()))
                {
                    m_Particle::m_Data.m_pJson->m_Data["Custom"][m_Particle::m_NameBuffer] = "Dummy";
                    m_Particle::m_Data.m_pJson->WriteToDisk();
                }
            }
            ImGui::Spacing();
            if (ImGui::BeginChild("Anims Child"))
            {
                ImGui::Spacing();
                Ui::DrawJSON(m_Particle::m_Data, PlayParticle, RemoveParticle);
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Style"))
        {
             ImGui::Spacing();
            if (Ui::ListBox("Fighting style", m_FightingStyleList, m_nFightingStyle))
            {
                Command<Commands::GIVE_MELEE_ATTACK_TO_CHAR>(hPlayer, m_nFightingStyle + 4, 6);
                SetHelpMessage("Fighting anim set", false, false, false);
            }
            if (Ui::ListBoxStr("Walking style", m_WalkingStyleList, m_nWalkingStyle))
            {
                if (m_nWalkingStyle == "default")
                {
                    patch::Set<DWORD>(0x609A4E, 0x4D48689);
                    patch::Set<WORD>(0x609A52, 0);
                }
                else
                {
                    patch::Nop(0x609A4E, 6);
                    Command<Commands::REQUEST_ANIMATION>(m_nWalkingStyle.c_str());
                    Command<Commands::LOAD_ALL_MODELS_NOW>();
                    Command<Commands::SET_ANIM_GROUP_FOR_CHAR>(hPlayer, m_nWalkingStyle.c_str());
                    Command<Commands::REMOVE_ANIMATION>(m_nWalkingStyle.c_str());
                }
                SetHelpMessage("Walking anim set", false, false, false);
            }
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}

void Animation::RemoveAnimation(std::string& ifp, std::string& anim, std::string& ifpRepeat)
{
    if (ifp == "Custom")
    {
        m_AnimData.m_pJson->m_Data["Custom"].erase(anim);
        m_AnimData.m_pJson->WriteToDisk();
        SetHelpMessage("Animation removed", false, false, false);
    }
    else
    {
        SetHelpMessage("You can only remove custom anims", false, false, false);
    }
}