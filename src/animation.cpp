#include "pch.h"
#include "animation.h"
#include "widget.h"
#include "util.h"

#ifdef GTA3
#include <RpAnimBlend.h>
#include <CAnimationStyleDescriptor.h>
#include <CAnimManager.h>
#include "eAnimations.h"
#include <CAnimBlendAssociation.h>

#elif GTAVC

#include "../depend/kiero/minhook/MinHook.h"
#include <CAnimationStyleDescriptor.h>
#include <CAnimManager.h>
#include "eAnimations.h"
#include <CAnimBlendAssociation.h>
#endif

#ifdef GTASA
#include "overlay.h"

void Cutscene::Play(std::string& rootKey, std::string& cutsceneId, std::string& interior)
{
    if (Util::IsOnCutscene())
    {
        Util::SetMessage(TEXT("Animation.CutsceneRunning"));
        return;
    }

    CPlayerPed* pPlayer = FindPlayerPed();
    if (pPlayer)
    {
        m_SceneName = cutsceneId;
        m_pLastVeh =  pPlayer->m_nPedFlags.bInVehicle ? pPlayer->m_pVehicle : nullptr;
        m_nVehSeat = -1;

        if (m_pLastVeh->m_pDriver != pPlayer)
        {
            for (size_t i = 0; i != 8; ++i)
            {
                if (m_pLastVeh->m_apPassengers[i] == pPlayer)
                {
                    m_nVehSeat = i;
                    break;
                }
            }
        }
        Command<Commands::LOAD_CUTSCENE>(cutsceneId.c_str());
        m_nInterior = pPlayer->m_nAreaCode;
        pPlayer->m_nAreaCode = std::stoi(interior);
        Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
    }
}

void Particle::Play(std::string& cat, std::string& name, std::string& particle)
{
    CPlayerPed* pPlayer = FindPlayerPed();
    if (pPlayer)
    {
        CVector pos = pPlayer->GetPosition();
        int handle;
        Command<Commands::CREATE_FX_SYSTEM>(particle.c_str(), pos.x, pos.y, pos.z, 1, &handle);
        Command<Commands::PLAY_FX_SYSTEM>(handle);
        m_nParticleList.push_back(handle);
    }
}


void Particle::Remove(std::string& cat, std::string& name, std::string& particle)
{
    if (cat == "Custom")
    {
        Particle::m_Data.m_pData->RemoveKey("Custom", particle.c_str());
        Particle::m_Data.m_pData->Save();
        Util::SetMessage(TEXT("Animation.ParticleRemoved"));
    }
    else
    {
        Util::SetMessage(TEXT("Animation.CustomParticlesOnly"));
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

bool _LoadAnimationBlock(const char* szBlockName)
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

void _PlayAnim(RpClump* pClump, int animGroup, int animID, float blend, bool loop, bool secondary)
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

    if (loop)
    {
        pAnimAssoc->m_nFlags |= ANIMATION_LOOPED;
    }

    if (secondary)
    {
        pAnimAssoc->m_nFlags |= ANIMATION_PARTIAL;
    }
}

#else

void _PlayAnim(RpClump* pClump, int animGroup, int animID, float blend, bool loop, bool secondary)
{
    CAnimBlendAssociation* pAnimStaticAssoc = CAnimManager::GetAnimAssociation(animGroup, animID);
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
    pAnimAssoc = CAnimManager::BlendAnimation(pClump, animGroup, animID, blend);
    pAnimAssoc->m_nFlags = 0x1 | 0x20;

    if (loop)
    {
        pAnimAssoc->m_nFlags |= 0x2;
    }

    if (secondary)
    {
        pAnimAssoc->m_nFlags |= 0x10;
    }
}
#endif

void Animation::Play(std::string& ifp, std::string& anim, std::string& _)
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
    int groupID, animID;
    sscanf(ifp.c_str(), "%d$%d,", &groupID, &animID);
    _PlayAnim(pPed->m_pRwClump, groupID, animID, 4.0f, m_Loop, m_bSecondary);
#endif
}

void Animation::Remove(std::string& ifp, std::string& anim, std::string& ifpRepeat)
{
    if (ifp == "Custom")
    {
        m_AnimData.m_pData->RemoveKey("Custom", anim.c_str());
        m_AnimData.m_pData->Save();
        Util::SetMessage(TEXT("Animation.AnimationRemoved"));
    }
    else
    {
        Util::SetMessage(TEXT("Animation.CustomAnimsOnly"));
    }
}

void Animation::Init()
{
#ifdef GTASA
    Events::processScriptsEvent += []
    {
        CPlayerPed* pPlayer = FindPlayerPed();
        if (Cutscene::m_bRunning)
        {
            if (Command<Commands::HAS_CUTSCENE_FINISHED>())
            {
                if (!pPlayer)
                {
                    return;
                }

                pPlayer->m_nAreaCode = Cutscene::m_nInterior;
                Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
                Cutscene::m_nInterior = 0;
                TheCamera.Fade(0, 1);
            }
        }
        else
        {
            if (Cutscene::m_SceneName != "" && Command<Commands::HAS_CUTSCENE_LOADED>())
            {
                Command<Commands::START_CUTSCENE>();
                Cutscene::m_bRunning = true;
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

        if (ImGui::BeginTabItem(TEXT("Animation.AnimationTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Animation.StopAnimation"), Widget::CalcSize()))
            {
                if (hPlayer)
                {
#ifdef GTASA
                    Command<Commands::CLEAR_CHAR_TASKS>(hPlayer);
#else
                    _PlayAnim(pPlayer->m_pRwClump, ANIM_GROUP_MAN, ANIM_MAN_IDLE_STANCE, 4.0f, m_Loop, m_bSecondary);
#endif
                }
            }
            ImGui::Spacing();

            ImGui::Columns(2, nullptr, false);
            Widget::Checkbox(TEXT("Animation.LoopCheckbox"), &m_Loop, TEXT("Animation.LoopCheckboxText"));
            Widget::Checkbox(TEXT("Animation.SecondaryCheckbox"), &m_bSecondary, TEXT("Animation.SecondaryCheckboxText"));
            ImGui::NextColumn();
#ifdef GTASA
            Widget::Checkbox(TEXT("Animation.PedAnim"), &m_PedAnim, TEXT("Animation.PedAnimText"));
#endif
            ImGui::Columns(1);
            ImGui::Spacing();

            if (m_PedAnim && !m_pTarget)
            {
                ImGui::TextWrapped(TEXT("Animation.NoTarget"));
            }
            else
            {
                ImGui::Spacing();

                if (ImGui::BeginChild("Anims Child"))
                {
                    ImGui::Spacing();
                    Widget::DataList(m_AnimData, Play, Remove, 
                    [](){
                        static char animBuf[INPUT_BUFFER_SIZE];
                        static char ifpBuf[INPUT_BUFFER_SIZE];
                        
                        ImGui::InputTextWithHint(TEXT("Animation.IFPName"), "ped", ifpBuf, INPUT_BUFFER_SIZE);
                        ImGui::InputTextWithHint(TEXT("Animation.AnimName"), "cower", animBuf, INPUT_BUFFER_SIZE);
                        ImGui::Spacing();
                        if (ImGui::Button(TEXT("Window.AddEntry"), Widget::CalcSize()))
                        {
                            std::string key = std::string("Custom.") + animBuf;
                            m_AnimData.m_pData->Set(key.c_str(), std::string(ifpBuf));
                            m_AnimData.m_pData->Save();
                            Util::SetMessage("Window.AddEntryMSG");
                        }
                    });
                    ImGui::EndChild();
                }
            }
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT("Animation.Tasks")))
        {
            ImGui::Spacing();
            Widget::Checkbox(TEXT("Menu.ShowPedTasks"), &Overlay::m_bPedTasks);
            ImGui::Spacing();
            CPlayerPed* player = FindPlayerPed();
            if (player)
            {   
                ImGui::BeginChild("TasksList");
                ImGui::Text(TEXT("Animation.PrimaryTasks"));
                ImGui::Separator();
                for (size_t i = 0; i != TASK_PRIMARY_MAX; ++i)
                {
                    CTask *pTask = player->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[i];
                    if (pTask)
                    {
                        const char *name = taskNames[pTask->GetId()];
                        if (ImGui::MenuItem(name))
                        {
                            ImGui::SetClipboardText(name);
                            Util::SetMessage(TEXT("Window.CopiedToClipboard"));
                        }
                    }
                }

                ImGui::Dummy(ImVec2(0, 25));

                ImGui::Text(TEXT("Animation.SecondaryTasks"));
                ImGui::Separator();
                for (size_t i = 0; i != TASK_SECONDARY_MAX; ++i)
                {
                    CTask *pTask = player->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[i];
                    if (pTask)
                    {
                        const char *name = taskNames[pTask->GetId()];
                        if (ImGui::MenuItem(name))
                        {
                            ImGui::SetClipboardText(name);
                            Util::SetMessage(TEXT("Window.CopiedToClipboard"));
                        }
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Animation.CutsceneTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Animation.StopCutscene"), Widget::CalcSize()))
            {
                if (Cutscene::m_bRunning)
                {
                    CPlayerPed* player = FindPlayerPed();
                    int hPlayer = CPools::GetPedRef(player);
                    int hVeh = CPools::GetVehicleRef(Cutscene::m_pLastVeh);
                    Command<Commands::CLEAR_CUTSCENE>();
                    Cutscene::m_bRunning = false;
                    Cutscene::m_SceneName = "";
                    player->m_nAreaCode = Cutscene::m_nInterior;
                    Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);

                    if (Cutscene::m_nVehSeat == -1)
                    {
                        Command<Commands::WARP_CHAR_INTO_CAR>(hPlayer, hVeh);
                    }
                    else
                    {
                        Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(hPlayer, hVeh, Cutscene::m_nVehSeat);
                    }
                    Cutscene::m_nInterior = 0;
                    TheCamera.Fade(0, 1);
                }
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("Cutscene Child"))
            {
                ImGui::Spacing();
                Widget::DataList(Cutscene::m_Data, Cutscene::Play, nullptr);
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Animation.ParticleTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Animation.RemoveAll"), Widget::CalcSize(2)))
            {
                for (int& p : Particle::m_nParticleList)
                {
                    Command<Commands::KILL_FX_SYSTEM>(p);
                }
                Particle::m_nParticleList.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Animation.RemoveLatest"), Widget::CalcSize(2)))
            {
                Command<Commands::KILL_FX_SYSTEM>(Particle::m_nParticleList.back()); // stop if anything is running
                Particle::m_nParticleList.pop_back();
            }
            ImGui::Spacing();
            if (Widget::CheckboxBits(TEXT("Animation.InvisiblePlayer"), pPlayer->m_nPedFlags.bDontRender))
            {
                pPlayer->m_nPedFlags.bDontRender = (pPlayer->m_nPedFlags.bDontRender == 1) ? 0 : 1;
            }
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::BeginChild("Anims Child"))
            {
                ImGui::Spacing();
                Widget::DataList(Particle::m_Data, Particle::Play, Particle::Remove,
                [](){
                    static char name[INPUT_BUFFER_SIZE], particle[INPUT_BUFFER_SIZE];
                    ImGui::InputTextWithHint(TEXT("Animation.ParticleName"), "KKJJ fire particle", name, INPUT_BUFFER_SIZE);
                    ImGui::InputTextWithHint(TEXT("Animation.ParticleID"), "kkjj_on_fire", particle, INPUT_BUFFER_SIZE);
                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Window.AddEntry"), Widget::CalcSize()))
                    {
                        std::string key = std::string("Custom.") + name;
                        Particle::m_Data.m_pData->Set(key.c_str(), std::string(particle));
                        Particle::m_Data.m_pData->Save();
                        Util::SetMessage(TEXT("Window.AddEntryMSG"));
                    }
                });
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Animation.Styles")))
        {
            ImGui::Spacing();

            static int fightStyle;
            static const char* fightStyles = "Default\0Boxing\0Kung Fu\0Kick Boxing\0Punch Kick\0";
            static std::string walkStyle = "default";
            static std::vector<std::string> walkStyles =
            {
                "default", "man", "shuffle", "oldman", "gang1", "gang2", "oldfatman",
                "fatman", "jogger", "drunkman", "blindman", "swat", "woman", "shopping", "busywoman",
                "sexywoman", "pro", "oldwoman", "fatwoman", "jogwoman", "oldfatwoman", "skate"
            };

            if (ImGui::Combo(TEXT("Animation.FightingStyle"), &fightStyle, fightStyles))
            {
                Command<Commands::GIVE_MELEE_ATTACK_TO_CHAR>(hPlayer, fightStyle + 4, 6);
                Util::SetMessage(TEXT("Animation.FightingStyleSet"));
            }
            if (Widget::ListBox(TEXT("Animation.WalkingStyle"), walkStyles, walkStyle))
            {
                if (walkStyle == "default")
                {
                    patch::Set<DWORD>(0x609A4E, 0x4D48689);
                    patch::Set<WORD>(0x609A52, 0);
                }
                else
                {
                    patch::Nop(0x609A4E, 6);
                    Command<Commands::REQUEST_ANIMATION>(walkStyle.c_str());
                    Command<Commands::LOAD_ALL_MODELS_NOW>();
                    Command<Commands::SET_ANIM_GROUP_FOR_CHAR>(hPlayer, walkStyle.c_str());
                    Command<Commands::REMOVE_ANIMATION>(walkStyle.c_str());
                }
                Util::SetMessage(TEXT("Animation.WalkingStyleSet"));
            }
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}