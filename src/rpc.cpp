#include "pch.h"
#include "rpc.h"
#include "util.h"
#include "menu.h"
#include "vehicle.h"
#include "cheatmenu.h"

char asciitolower(char in) 
{
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

void RPC::Shutdown()
{
}

void RPC::Init()
{
    discord::Result result = discord::Core::Create(951199292981403669, DiscordCreateFlags_Default, &pCore);
    bInit = result == discord::Result::Ok;
}

void RPC::Process()
{
    if (!Menu::m_bDiscordRPC)
    {
        return;
    }

    static std::string detailsText, stateText, smallImg, smallImgText, largeImg, largeImgText;
    static size_t curImage = 1;
    static size_t timer = 0;
    static size_t startTime = CTimer::m_snTimeInMilliseconds;
    
    CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];
    CPlayerPed *pPed = pInfo->m_pPed;

    if (pPed)
    {
        size_t curTimer = CTimer::m_snTimeInMilliseconds;
        size_t wantedLevel = pPed->m_pWanted->m_nWantedLevel;
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

        size_t seconds = curTimer - startTime;
        size_t minutes = (seconds / 60) % 60;
        size_t hours = (minutes / 60) % 60;
        stateText = std::format(TEXT("RPC.PlayingFor"), hours, minutes, seconds);
        
        if (BY_GAME(Util::IsOnMission(), false, false))
        {
            stateText = TEXT("RPC.DoingMission");
        }

        if (CheatMenu::IsMenuShown())
        {
            stateText = TEXT("RPC.BrowsingCheatMenu");
        }

        if (BY_GAME(FrontEndMenuManager.m_bMenuActive, FrontendMenuManager.m_bMenuVisible, FrontEndMenuManager.m_bMenuActive))
        {
            stateText = TEXT("RPC.InsideMenus");
        }

        largeImgText = std::format("{}: {} / {} {}: {} / {}", TEXT("Player.Armour"), pPed->m_fArmour, BY_GAME(pInfo->m_nMaxArmour, pInfo->m_nMaxArmour, 100), TEXT("Player.Health"), pPed->m_fHealth, BY_GAME(pPed->m_fMaxHealth, 100, 100));
        largeImg = std::format("{}{}", BY_GAME("sa", "vc", "3"), curImage);
        
        discord::Activity activity{};
        activity.SetDetails(detailsText.c_str());
        activity.SetState(stateText.c_str());
        activity.GetAssets().SetSmallImage(smallImg.c_str());
        activity.GetAssets().SetSmallText(smallImgText.c_str());
        activity.GetAssets().SetLargeImage(largeImg.c_str());
        activity.GetAssets().SetLargeText(largeImgText.c_str());

        activity.SetType(discord::ActivityType::Playing);
        pCore->ActivityManager().UpdateActivity(activity, 
        [](discord::Result result) 
        {
            if (result != discord::Result::Ok)
            {
                gLog << "Failed to set discord activity!" << std::endl;
            }
        });
        pCore->RunCallbacks();

        if (curTimer - timer > 10000)
        {
            curImage++;

            if (curImage > totalLargeImages)
            {
                curImage = 1;
            }
            timer = curTimer;
        }
    }
}