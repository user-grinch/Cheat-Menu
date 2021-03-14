#include "Updater.h"
#include "pch.h"
#include "MenuInfo.h"
#include "..\deps\zip\zip.h"

void Updater::CheckForUpdate()
{
    const char* link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
    char* path = PLUGIN_PATH((char*)"CheatMenu/json/versioninfo.json");
    HRESULT res = URLDownloadToFile(NULL, link, path, 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        CHud::SetHelpMessage("Failed to check for updates",false,false,false);
        return;
    }  
    CJson verinfo = CJson("versioninfo");

    // fetch the version number
    latest_version = verinfo.data.items().begin().value()["name"].get<std::string>();

    if (latest_version > MENU_VERSION_NUMBER)
    {
        CHud::SetHelpMessage("Update found",false,false,false);
        state = UPDATER_UPDATE_FOUND;
    }
    else
    {
        CHud::SetHelpMessage("No update found.",false,false,false);
        Updater::state = UPDATER_IDLE;
    }
}

void Updater::DownloadUpdate()
{
    std::string link = "https://github.com/user-grinch/Cheat-Menu/releases/download/" + latest_version + "/CheatMenu.zip";
    char* path = PLUGIN_PATH((char*)"update.zip");
    HRESULT res = URLDownloadToFile(NULL, link.c_str(), path, 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        CHud::SetHelpMessage("Failed download update",false,false,false);
        state = UPDATER_IDLE;
        return;
    }  
    CHud::SetHelpMessage("Update downloaded successfully.",false,false,false);
    state = UPDATER_DOWNLOADED;
}

void Updater::InstallUpdate()
{   
    CHud::SetHelpMessage("Updating in progress. Do not pause/close the game.",false,false,false);
    Sleep(100);
    // delete the old menu
	std::remove(PLUGIN_PATH((char*)"CheatMenu.asi.old"));
    std::string new_name = PLUGIN_PATH((char*)"CheatMenu.asi.old");
    std::string old_name = PLUGIN_PATH((char*)"CheatMenu.asi");
    std::rename(old_name.c_str(),new_name.c_str());
    fs::remove_all(PLUGIN_PATH((char*)"CheatMenu"));

    std::string dir = PLUGIN_PATH((char*)"");
    std::string file = PLUGIN_PATH((char*)"update.zip");
    zip_extract(file.c_str(),dir.c_str(),NULL,NULL);
}

static bool menu_loaded = false;
static void LoadUpdatedMenu()
{
    std::string new_name = PLUGIN_PATH((char*)"CheatMenuNew.asi");
    LoadLibrary(new_name.c_str());
	menu_loaded = true;
}

void Updater::FinishUpdate()
{
    // kinda hacky
    std::string new_name = PLUGIN_PATH((char*)"CheatMenuNew.asi");
    std::string old_name = PLUGIN_PATH((char*)"CheatMenu.asi");
    std::rename(old_name.c_str(),new_name.c_str());
    
    Events::processScriptsEvent += LoadUpdatedMenu;

    while (!menu_loaded)
		Sleep(1000);

	Events::processScriptsEvent -= LoadUpdatedMenu;
    
    std::rename(new_name.c_str(),old_name.c_str());
}