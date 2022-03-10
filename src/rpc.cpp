#include "pch.h"
#include "rpc.h"
#include "util.h"
#include "menu.h"
#include "vehicle.h"
#include "cheatmenu.h"

char asciitolower(char in) 
{
    if (in <= 'Z' && in >= 'A')
    {
        return in - ('Z' - 'z');
    }
    
    return in;
}

void RPC::Shutdown()
{
    if (f_ShutDown)
    {
        CallDyn<>(int(f_ShutDown));
    }
}

void RPC::Init()
{
    std::string Id = BY_GAME("951199292981403669", "951448264195059712", "951457540573655080");

    if (!hDll)
    {
        hDll = NULL;
        f_Init = NULL;
        f_Update = NULL;
        f_ShutDown = NULL;

        hDll = LoadLibrary(PLUGIN_PATH((char*)"CheatMenu/dlls/discord-rpc.dll"));

        if (hDll)
        {
            f_Init = GetProcAddress(hDll, "Discord_Initialize");
            f_Update = GetProcAddress(hDll, "Discord_UpdatePresence");
            f_ShutDown = GetProcAddress(hDll, "Discord_Shutdown");
        }
    }

    if (f_Init)
    {
        CallDyn<const char*, int, int, int>((int)f_Init, Id.c_str(), NULL, NULL, NULL);
        drpc.startTimestamp = time(0);
        bInit = true;
    }
    else
    {
        gLog << "Failed to init discord rpc" << std::endl;
    }
}

void RPC::Process()
{
    if (!(Menu::m_bDiscordRPC && bInit))
    {
        return;
    }

    static std::string detailsText, stateText, smallImg, smallImgText, largeImg, largeImgText;
    static size_t curImage = Random(1, 5);
    static size_t timer = CTimer::m_snTimeInMilliseconds;
    
    CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];
    CPlayerPed *pPed = pInfo->m_pPed;

    if (pPed)
    {
        size_t curTimer = CTimer::m_snTimeInMilliseconds;

#ifdef GTASA
        size_t wantedLevel = pPed->GetWantedLevel();
#else
        size_t wantedLevel = pPed->m_pWanted->m_nWantedLevel;
#endif
        if (wantedLevel > 0)
        {
            detailsText = std::format("{}: ", TEXT("Player.WantedLevel"));
            for (size_t i = 0; i < wantedLevel; ++i)
            {
                detailsText += "☆";
            }
        }
        else
        {
            detailsText = std::format("{}: ${}", TEXT("Player.Money"), pInfo->m_nMoney);
        }
        
        if (BY_GAME(pPed->m_nPedFlags.bInVehicle, pPed->m_bInVehicle, pPed->m_bInVehicle))
        {
            std::string name = Vehicle::GetNameFromModel(pPed->m_pVehicle->m_nModelIndex);
            std::transform(name.begin(), name.end(), name.begin(), asciitolower);
            smallImgText = std::format("{} {} {} {}", TEXT("RPC.Driving"), name, TEXT("RPC.In"), Util::GetLocationName(&pPed->GetPosition()));
            smallImg = "drive";
        }
        else
        {
            smallImg = "walk";
            smallImgText = std::format("{} {} {}", TEXT("RPC.Walking"), TEXT("RPC.In"), Util::GetLocationName(&pPed->GetPosition()));
        }

        if (BY_GAME(Util::IsOnMission(), false, false))
        {
            stateText = TEXT("RPC.DoingMission");
        }

        if (CheatMenu::IsMenuShown())
        {
            stateText = TEXT("RPC.BrowsingCheatMenu");
        }
        
        if (Command<Commands::IS_CHAR_DEAD>(CPools::GetPedRef(pPed)))
        {
            stateText = TEXT("RPC.Wasted");
        }
        
        if (BY_GAME(pPed->m_nPedFlags.bIsBeingArrested, false, false))
        {
            stateText = TEXT("RPC.Busted");
        }

        largeImgText = std::format("{}: {} - {}: {}", TEXT("Player.Armour"), pPed->m_fArmour, TEXT("Player.Health"), int(pPed->m_fHealth));
        largeImg = std::format("{}{}", BY_GAME("sa", "vc", "3"), curImage);
        
        drpc.details = detailsText.c_str();
        drpc.state = stateText.c_str();
        drpc.smallImageKey = smallImg.c_str();
        drpc.smallImageText = smallImgText.c_str();
        drpc.largeImageKey = largeImg.c_str();
        drpc.largeImageText = largeImgText.c_str();

        if (f_Update)
        {
            CallDyn<DiscordRichPresence*>(int(f_Update), &drpc);
        }

        if (curTimer - timer > 5*60000)
        {
            curImage++;

            if (curImage > 5) // Must upload images to discord servers
            {
                curImage = 1;
            }
            timer = curTimer;
        }
    }
}