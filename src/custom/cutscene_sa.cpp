#include "pch.h"
#include "cutscene_sa.h"

CutsceneMgr& Cutscene = *CutsceneMgr::Get();
CutsceneMgr::CutsceneMgr()
{
    static CdeclEvent <AddressList<0x5B195F, H_JUMP>, PRIORITY_AFTER,  ArgPickNone, void()> skipCutsceneEvent;
    skipCutsceneEvent += [this]()
    {
        Stop();
    };
}

void CutsceneMgr::Play(std::string& rootKey, std::string& cutsceneId, std::string& interior)
{
    if (CCutsceneMgr::ms_running)
    {
        Util::SetMessage(TEXT("Animation.CutsceneRunning"));
        return;
    }

    CPlayerPed* pPlayer = FindPlayerPed();
    if (pPlayer)
    {
        m_pLastVeh =  pPlayer->m_nPedFlags.bInVehicle ? pPlayer->m_pVehicle : nullptr;
        m_nVehSeat = -1;

        if (m_pLastVeh && m_pLastVeh->m_pDriver != pPlayer)
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
        CCutsceneMgr::LoadCutsceneData(cutsceneId.c_str());
        CCutsceneMgr::Update();

        m_nInterior = pPlayer->m_nAreaCode;
        pPlayer->m_nAreaCode = std::stoi(interior);
        Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
        CutsceneMgr::m_bRunning = true;
        CCutsceneMgr::StartCutscene();
    }
}

void CutsceneMgr::Stop()
{
    if (CutsceneMgr::m_bRunning)
    {
        CPlayerPed *pPlayer = FindPlayerPed();
        int hPlayer = CPools::GetPedRef(pPlayer);

        CCutsceneMgr::DeleteCutsceneData();
        CutsceneMgr::m_bRunning = false;
        pPlayer->m_nAreaCode = CutsceneMgr::m_nInterior;
        CutsceneMgr::m_nInterior = 0;
        Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);

        // handle vehicle
        if (CutsceneMgr::m_pLastVeh)
        {
            int hVeh = CPools::GetVehicleRef(CutsceneMgr::m_pLastVeh);
            if (CutsceneMgr::m_nVehSeat == -1)
            {
                Command<Commands::WARP_CHAR_INTO_CAR>(hPlayer, hVeh);
            }
            else
            {
                Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(hPlayer, hVeh, CutsceneMgr::m_nVehSeat);
            }
        }
        TheCamera.Fade(0, 1);
    }
}