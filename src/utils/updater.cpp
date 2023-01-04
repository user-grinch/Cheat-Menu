#include "pch.h"
#include "updater.h"

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

void Updater::Process()
{
    if (Updater::curState != States::CHECKING)
    {
        return;
    }

    const char* link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
    char* path = PLUGIN_PATH((char*)FILE_NAME "/data/versioninfo.json");
    HRESULT res = URLDownloadToFile(NULL, link, path, 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        Util::SetMessage(TEXT("Updater.Failed"));
        return;
    }

    // Extract the version number
    FILE *pFile= fopen(path, "r");
    if (pFile != NULL)
    {
        char buf[64];
        float version = 0.0f;
        while (fgets(buf, 64, pFile) != NULL)
        {
            sscanf(buf, "[{\"name\": \"%f\",", &version);
            if (version != 0.0f)
            {
                latestVer = std::format("{}", version);
                break;
            }
        }
        fclose(pFile);
    }
    remove(path);

    if (latestVer > MENU_VERSION_NUMBER)
    {
        Util::SetMessage(TEXT("Updater.Found"));
        curState = States::FOUND;
    }
    else
    {
        Util::SetMessage(TEXT("Updater.NotFound"));
        Updater::curState = States::IDLE;
    }
}
