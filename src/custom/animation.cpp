#include "pch.h"
#include "animation.h"
#include "utils/widget.h"

#ifdef GTAVC
#include "kiero/minhook/MinHook.h"
#include <CAnimationStyleDescriptor.h>
#include <CAnimManager.h>
#include "eAnimations.h"
#include <CAnimBlendAssociation.h>
#elif GTA3
#include <RpAnimBlend.h>
#include <CAnimationStyleDescriptor.h>
#include <CAnimManager.h>
#include "eAnimations.h"
#include <CAnimBlendAssociation.h>
#endif

AnimationMgr& Animation = AnimationMgr::Get();

#ifdef GTAVC
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
#elif GTA3
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

AnimationMgr::AnimationMgr()
{
#ifdef GTASA
    Events::processScriptsEvent += [&]
    {
        CPlayerPed* pPlayer = FindPlayerPed();

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

    // Fix crash at 0x4019EA
    static bool hookInjected = false;
    Events::initGameEvent.before += []()
    {
        if (hookInjected)
        {
            MH_DisableHook((void*)0x40D6E0);
        }
    };

    Events::initGameEvent.after += []()
    {
        MH_CreateHook((void*)0x40D6E0, NEW_CStreaming_RemoveModel, (void**)&OLD_CStreaming_RemoveModel);
        MH_EnableHook((void*)0x40D6E0);
        hookInjected = true;
    };
#endif
}

void AnimationMgr::Play(std::string& cat, std::string& anim, std::string& ifp)
{
    CPed *pPed = m_bPedAnim ? m_pTarget : FindPlayerPed();
    if (!pPed)
    {
        return;
    }

#ifdef GTASA
    int hPed = CPools::GetPedRef(pPed);

    if (ifp != "PED")
    {
        Command<Commands::REQUEST_ANIMATION>(ifp.c_str());
        Command<Commands::LOAD_ALL_MODELS_NOW>();
    }

    Command<Commands::CLEAR_CHAR_TASKS>(hPed);
    if (m_bSecondary)
    {
        Command<Commands::TASK_PLAY_ANIM_SECONDARY>(hPed, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
    }
    else
    {
        Command<Commands::TASK_PLAY_ANIM>(hPed, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
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

void AnimationMgr::AddNew()
{
    static char animBuf[INPUT_BUFFER_SIZE];
    static char ifpBuf[INPUT_BUFFER_SIZE];

    ImGui::InputTextWithHint(TEXT("Scene.IFPName"), "ped", ifpBuf, INPUT_BUFFER_SIZE);
    ImGui::InputTextWithHint(TEXT("Scene.AnimName"), "cower", animBuf, INPUT_BUFFER_SIZE);
    ImGui::Spacing();
    if (ImGui::Button(TEXT("Window.AddEntry"), Widget::CalcSize()))
    {
        std::string key = std::string("Custom.") + animBuf;
        m_Data.m_pData->Set(key.c_str(), std::string(ifpBuf));
        m_Data.m_pData->Save();
        Util::SetMessage(TEXT("Window.AddEntryMSG"));
    }
}

bool AnimationMgr::IsTargetFound()
{
    return m_bPedAnim && !m_pTarget;
}

void AnimationMgr::StopImmediately(CPed *ped)
{
    if (!ped)
    {
        return;
    }

#ifdef GTASA
    Command<Commands::CLEAR_CHAR_TASKS>(CPools::GetPedRef(ped));
#else
    _PlayAnim(ped->m_pRwClump, ANIM_GROUP_MAN, ANIM_MAN_IDLE_STANCE, 4.0f, false, false);
#endif
}