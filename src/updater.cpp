#include "pch.h"
#include "updater.h"
#include "version.h"

bool Updater::IsUpdateAvailable()
{
    return Updater::curState == States::FOUND;
}

void Updater::ResetUpdaterState()
{
    Updater::curState = States::IDLE;
}

std::string Updater::GetUpdateVersion()
{
    return Updater::latestVer;
}

void Updater::CheckUpdate()
{
    if (Updater::curState == States::IDLE)
    {
        Updater::curState = States::CHECKING;
    }
}

void Updater::IncrementDailyUsageCounter()
{
    URLDownloadToFile(NULL, "https://github.com/user-grinch/Cheat-Menu/releases/download/3.2/counter.info", "", 0, NULL);
}

void Updater::Process()
{
    if (Updater::curState != States::CHECKING)
    {
        return;
    }

    const char* link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
    char* path = PLUGIN_PATH((char*)"CheatMenu/json/versioninfo.json");
    HRESULT res = URLDownloadToFile(NULL, link, path, 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        SetHelpMessage(TEXT("Updater.Failed"));
        return;
    }

    CJson verinfo = CJson("versioninfo");

    // fetch the version number
    if (verinfo.m_Data.empty())
    {
        latestVer = MENU_VERSION_NUMBER;
    }
    else
    {
        latestVer = verinfo.m_Data.items().begin().value()["name"].get<std::string>();
    }

    if (latestVer > MENU_VERSION_NUMBER)
    {
        SetHelpMessage(TEXT("Updater.Found"));
        curState = States::FOUND;
    }
    else
    {
        SetHelpMessage(TEXT("Updater.NotFound"));
        Updater::curState = States::IDLE;
    }
}
