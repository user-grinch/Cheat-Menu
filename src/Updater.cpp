#include "Updater.h"
#include "pch.h"
#include "MenuInfo.h"

void Updater::CheckForUpdates()
{
    LPCSTR link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
    LPCSTR path = PLUGIN_PATH((char*)"CheatMenu\\json\\versioninfo.json");
    HRESULT res = URLDownloadToFile(NULL, link, path, 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        CHud::SetHelpMessage("Failed to check for updates",false,false,false);
        return;
    }  
    CJson verinfo = CJson("versioninfo");

    // fetch the version number
    std::string latest_version = verinfo.data.items().begin().value()["name"].get<std::string>();

    if (latest_version > MENU_VERSION_NUMBER)
        CHud::SetHelpMessage("Update found",false,false,false);
    else
        CHud::SetHelpMessage("No update found.",false,false,false);
}