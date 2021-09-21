#include "pch.h"
#include "hook.h"
#include "cheatmenu.h"
#include "updater.h"
#include "menuinfo.h"

void MenuThread(void* param)
{
	/*
		Had to put this in place since some people put the folder in root 
		directory and the asi in modloader. Why??

		TODO: Unlikely they'd even read the log so have to do something else
	*/
	if (!std::filesystem::is_directory(PLUGIN_PATH((char*)"CheatMenu")))
	{
		gLog << "CheatMenu folder not found. You need to put both \"CheatMenu.asi\" & \"CheatMenu\" folder in the same directory" << std::endl;
		return;
	}

#ifdef GTASA
	Hook::ApplyMouseFix();
#endif

	static bool bGameInit = false;

	// Wait till game init
	Events::initRwEvent += []
	{
		bGameInit = true;
	};

	while (!bGameInit)
	{
		Sleep(1000);
	}

    /*
        TODO: Find a better way
        Since you could still name it something else
    */
	if (GetModuleHandle("SAMP.dll") || GetModuleHandle("SAMP.asi"))
	{
		MessageBox(RsGlobal.ps->window, "SAMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
		return;
	}

	gLog << "Starting...\nVersion: " MENU_TITLE "\nAuthor: Grinch_\nDiscord: " DISCORD_INVITE "\nMore Info: "
		GITHUB_LINK "\n" << std::endl;
	CFastman92limitAdjuster::Init();
	CheatMenu menu;


	// Checking for updates once a day
	time_t now = time(0);
	struct tm  tstruct = *localtime(&now);
	int lastCheckDate = gConfig.GetValue("config.last_update_checked", 0);

	if (lastCheckDate != tstruct.tm_mday)
	{
		Updater::CheckForUpdate();
		gConfig.SetValue("config.last_update_checked", tstruct.tm_mday);
	}

	while (true)
	{
		Sleep(5000);

		if (Updater::m_State == UPDATER_CHECKING)
		{
			Updater::CheckForUpdate();
		}
	}
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		uint gameVersion = GetGameVersion();
#ifdef GTASA
		if (gameVersion == GAME_10US_HOODLUM)
		{
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
		}
		else
		{
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA SA v1.0 US is required.", "CheatMenu", MB_ICONERROR);
		}
#elif GTAVC
		if (gameVersion == GAME_10EN)
		{
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
		}
		else
		{
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA VC v1.0 EN is required.", "CheatMenu", MB_ICONERROR);
		}
#endif
	}

	return TRUE;
}