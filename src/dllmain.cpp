#include "pch.h"
#include "cheatmenu.h"
#include "utils/updater.h"
#include "utils/rpc.h"
#include "pages/menu.h"

void MenuThread(void* param)
{
    /* 
        Wait for game init
        // Sleep(3000);
        Doing it like this doesn't prevent from attaching a debugger
    */ 
    static bool gameStarted = false;
    Events::processScriptsEvent +=[]
    {
        gameStarted = true;
    };

    while (!gameStarted)
    {
        Sleep(500);
    }
    
    /*
    	Had to put this in place since some people put the folder in root
    	directory and the asi in modloader. Why??
    */
    if (!std::filesystem::is_directory(PLUGIN_PATH((char*)FILE_NAME)))
    {
        std::string msg = std::format("{} folder not found. You need to put both '{}.asi' & '{}' folder in the same directory", FILE_NAME, FILE_NAME, FILE_NAME);
        Log::Print<eLogLevel::Error>(msg.c_str());
        MessageBox(NULL, msg.c_str(), FILE_NAME, MB_ICONERROR);
        return;
    }

    /*
    	Need SilentPatch since all gta games have issues with mouse input
    	Implementing mouse fix is a headache anyway
    */
    if (!GetModuleHandle(BY_GAME("SilentPatchSA.asi","SilentPatchVC.asi","SilentPatchIII.asi")))
    {
        Log::Print<eLogLevel::Error>("SilentPatch not found. Please install it from here https://gtaforums.com/topic/669045-silentpatch/");
        int msgID = MessageBox(NULL, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", FILE_NAME, MB_OKCANCEL | MB_DEFBUTTON1);

        if (msgID == IDOK)
        {
            OPEN_LINK("https://gtaforums.com/topic/669045-silentpatch/");
        };
        return;
    }

    Log::Print<eLogLevel::None>("Starting " MENU_TITLE " (" BUILD_NUMBER ")\nAuthor: Grinch_\nDiscord: "
                            DISCORD_INVITE "\nMore Info: " GITHUB_LINK);

    // date time
    SYSTEMTIME st;
    GetSystemTime(&st);
    Log::Print<eLogLevel::None>("Date: {}-{}-{} Time: {}:{}\n", st.wYear, st.wMonth, st.wDay,
                                st.wHour, st.wMinute);

    /*
        TODO: Find a better way
        Since you could still name it something else
    */
#ifdef GTASA
    if (GetModuleHandle("SAMP.dll") || GetModuleHandle("SAMP.asi"))
    {
        Log::Print<eLogLevel::Error>(FILE_NAME " doesn't support SAMP");
        MessageBox(NULL, "SAMP detected. Exiting " FILE_NAME, FILE_NAME, MB_ICONERROR);
        return;
    }
    CFastman92limitAdjuster::Init();
    Log::Print<eLogLevel::None>("Game detected: GTA San Andreas 1.0 US");
#elif GTAVC
    if (GetModuleHandle("vcmp-proxy.dll") || GetModuleHandle("vcmp-proxy.asi"))
    {
        Log::Print<eLogLevel::Error>(FILE_NAME " doesn't support VCMP");
        MessageBox(NULL, "VCMP detected. Exiting " FILE_NAME, FILE_NAME, MB_ICONERROR);
        return;
    }
    Log::Print<eLogLevel::None>("Game detected: GTA Vice City 1.0 EN");
#else
    Log::Print<eLogLevel::None>("Game detected: GTA III 1.0 EN");
#endif

    bool modloader = GetModuleHandle("modloader.asi");
    const char *path = PLUGIN_PATH((char*)"");
    Log::Print<eLogLevel::None>("Install location: {}", modloader && strstr(path, "modloader") ? "Modloader" : "Game directory");
    Log::Print<eLogLevel::None>("Font support package: {}", FontMgr::IsSupportPackageInstalled() ? "True" : "False");
    Log::Print<eLogLevel::None>("\nCLEO installed: {}", GetModuleHandle("cleo.asi") || GetModuleHandle("cleo_redux.asi") ? "True" : "False");
    Log::Print<eLogLevel::None>("FLA installed: {}", GetModuleHandle("$fastman92limitAdjuster.asi") ? "True" : "False");
    Log::Print<eLogLevel::None>("Mixsets installed: {}", GetModuleHandle("MixSets.asi") ? "True" : "False");
    Log::Print<eLogLevel::None>("Modloader installed: {}", modloader ? "True" : "False");
    Log::Print<eLogLevel::None>("OLA installed: {}", GetModuleHandle("III.VC.SA.LimitAdjuster.asi") ? "True" : "False");
    Log::Print<eLogLevel::None>("ProjectProps installed: {}", GetModuleHandle("ProjectProps.asi") ? "True" : "False");
    Log::Print<eLogLevel::None>("Renderhook installed: {}", GetModuleHandle("_gtaRenderHook.asi") ? "True" : "False");
    Log::Print<eLogLevel::None>("Widescreen Fix installed: {}\n", GetModuleHandle("GTASA.WidescreenFix.asi") ? "True" : "False");

    CheatMenu::Init();

    // Checking for updates once a day
    if (menuPage.m_bAutoCheckUpdate && gConfig.Get("Menu.LastUpdateChecked", 0) != st.wDay)
    {
        Updater::CheckUpdate();
        gConfig.Set("Menu.LastUpdateChecked", st.wDay);
    }
    
    if (Updater::IsUpdateAvailable())
    {
        Log::Print<eLogLevel::Info>("New update available: %s", Updater::GetUpdateVersion().c_str());
    }

    while (true)
    {
        FontMgr::DrawPages();
        RPC::DrawPages();
        Updater::DrawPages();
        Sleep(1000);
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
            Log::Print<eLogLevel::Error>("Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum or Compact", "VC v1.0 EN", "III v1.0 EN") " is required.");
            MessageBox(HWND_DESKTOP, "Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum or Compact", "VC v1.0 EN", "III v1.0 EN") " is required.", FILE_NAME, MB_ICONERROR);
        }
    }

    return TRUE;
}