#include "pch.h"
#include "cheatmenu.h"
#include "updater.h"
#include "version.h"

void MenuThread(void* param)
{
    // wait for game init
    Sleep(3000);

    /*
    	Had to put this in place since some people put the folder in root
    	directory and the asi in modloader. Why??
    */
    if (!std::filesystem::is_directory(PLUGIN_PATH((char*)"CheatMenu")))
    {
        gLog << "Error: CheatMenu folder not found. You need to put both \"CheatMenu.asi\" & \"CheatMenu\" folder in the same directory" << std::endl;
        MessageBox(RsGlobal.ps->window, "CheatMenu folder not found. You need to put both \"CheatMenu.asi\" & \"CheatMenu\" folder in the same directory", "CheatMenu", MB_ICONERROR);
        return;
    }

    /*
    	Need SilentPatch since all gta games have issues with mouse input
    	Implementing mouse fix is a headache anyway
    */
    if (!GetModuleHandle(BY_GAME("SilentPatchSA.asi","SilentPatchVC.asi","SilentPatchIII.asi")))
    {
        gLog << "Error: SilentPatch not found. Please install it from here https://gtaforums.com/topic/669045-silentpatch/" << std::endl;
        int msgID = MessageBox(RsGlobal.ps->window, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", "CheatMenu", MB_OKCANCEL | MB_DEFBUTTON1);

        if (msgID == IDOK)
        {
            ShellExecute(nullptr, "open", "https://gtaforums.com/topic/669045-silentpatch/", nullptr, nullptr, SW_SHOWNORMAL);
        };
        return;
    }

    /*
        TODO: Find a better way
        Since you could still name it something else
    */
#ifdef GTASA
    if (GetModuleHandle("SAMP.dll") || GetModuleHandle("SAMP.asi"))
    {
        gLog << "Error: CheatMenu doesn't support SAMP" << std::endl;
        MessageBox(RsGlobal.ps->window, "SAMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
        return;
    }
    CFastman92limitAdjuster::Init();
#elif GTAVC
    if (GetModuleHandle("vcmp-proxy.dll") || GetModuleHandle("vcmp-proxy.asi"))
    {
        gLog << "Error: CheatMenu doesn't support VCMP" << std::endl;
        MessageBox(RsGlobal.ps->window, "VCMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
        return;
    }
#endif

    gLog << "Starting...\nVersion: " MENU_TITLE "\nAuthor: Grinch_\nDiscord: " DISCORD_INVITE "\nMore Info: "
         GITHUB_LINK "\n" << std::endl;

    CheatMenu menu;

    // Checking for updates once a day
    SYSTEMTIME st;
    GetSystemTime(&st);
    if (gConfig.GetValue("config.update_date", 0) != st.wDay)
    {
        Updater::CheckUpdate();
        gConfig.SetValue("config.update_date", st.wDay);
    }

    while (true)
    {
        Updater::Process();
        Sleep(5000);
    }
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        uint gameVer = GetGameVersion();

#ifdef GTASA
        if (gameVer == GAME_10US_HOODLUM || gameVer == GAME_10US_COMPACT)
#else
        if (gameVer == BY_GAME(NULL, GAME_10EN, GAME_10EN))
#endif
        {
            CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
        }
        else
        {
            gLog << "Error: Unknown game version. GTA " <<  BY_GAME("SA v1.0 US Hoodlum", "VC v1.0 EN", "III v1.0 EN") << " is required." << std::endl;
            MessageBox(HWND_DESKTOP, "Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum", "VC v1.0 EN", "III v1.0 EN") " is required.", "CheatMenu", MB_ICONERROR);
        }
    }

    return TRUE;
}