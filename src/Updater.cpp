#include "Updater.h"
#include "pch.h"
#include "MenuInfo.h"

void Updater::CheckForUpdates()
{
    LPCSTR link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
    LPCSTR path = PLUGIN_PATH((char*)"CheatMenu/json/versioninfo.json");
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
    std::string link = "https://github.com/user-grinch/Cheat-Menu/releases/download/" + latest_version + "/CheatMenu.7z";
    LPCSTR path = PLUGIN_PATH((char*)"update.7z");
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