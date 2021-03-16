#include "Updater.h"
#include "pch.h"
#include "Ui.h"
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

// void Updater::DownloadUpdate()
// {
//     std::string link = "https://github.com/user-grinch/Cheat-Menu/releases/download/" + latest_version + "/CheatMenu.zip";
//     char* path = PLUGIN_PATH((char*)"update.zip");
//     HRESULT res = S_OK;//URLDownloadToFile(NULL, link.c_str(), path, 0, NULL);

//     if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
//     {
//         CHud::SetHelpMessage("Failed download update",false,false,false);
//         state = UPDATER_IDLE;
//         return;
//     }  
//     CHud::SetHelpMessage("Update downloaded successfully.",false,false,false);
//     state = UPDATER_DOWNLOADED;
// }

// void Updater::InstallUpdate()
// {   
//     CHud::SetHelpMessage("Update in progress. Do not pause/close the game.",false,false,false);
//     Sleep(100);
//     // delete the old menu
// 	std::remove(PLUGIN_PATH((char*)"CheatMenu.asi.old"));
//     std::string new_name = PLUGIN_PATH((char*)"CheatMenu.asi.old");
//     std::string old_name = PLUGIN_PATH((char*)"CheatMenu.asi");
//     std::rename(old_name.c_str(),new_name.c_str());
//     fs::remove_all(PLUGIN_PATH((char*)"CheatMenu"));

//     std::string dir = PLUGIN_PATH((char*)"");
//     std::string file = PLUGIN_PATH((char*)"update.zip");
//     zip_extract(file.c_str(),dir.c_str(),NULL,NULL);
// }

// static bool menu_loaded = false;
// static void LoadUpdatedMenu()
// {
//     std::string new_name = PLUGIN_PATH((char*)"CheatMenuNew.asi");
//     LoadLibrary(new_name.c_str());
// 	menu_loaded = true;
// }

// void Updater::FinishUpdate()
// {
//     // kinda hacky, can't update twice on same instance ( should be unlikely anyways?)
//     std::string new_name = PLUGIN_PATH((char*)"CheatMenuNew.asi");
//     std::string old_name = PLUGIN_PATH((char*)"CheatMenu.asi");
//     std::rename(old_name.c_str(),new_name.c_str());
    
//     Events::processScriptsEvent += LoadUpdatedMenu;

//     while (!menu_loaded)
// 		Sleep(1000);

// 	Events::processScriptsEvent -= LoadUpdatedMenu;
    
//     std::rename(new_name.c_str(),old_name.c_str());
// }

void Updater::ShowUpdateScreen()
{
    ImGui::Dummy(ImVec2(0,20));
    Ui::CenterdText("A new version of the mod is available.");
    Ui::CenterdText(std::string("Current version: ") + MENU_VERSION);
    Ui::CenterdText("Latest version: " + Updater::latest_version);
    ImGui::Dummy(ImVec2(0,10));
    ImGui::TextWrapped("In order to keep using the menu, you need to update to the latest version." 
    " This is to ensure everything is using the most up-to-date version."); 
    ImGui::Dummy(ImVec2(0,10));
    ImGui::TextWrapped("To know what changes are made or to download, click on the \"Download page\" button."
    " Follow the instructions there. If you're still having issues, let me know on discord.");

    ImGui::Dummy(ImVec2(0,5));
    if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(2))))
		ShellExecute(NULL, "open", DISCORD_INVITE, NULL, NULL, SW_SHOWNORMAL);

    ImGui::SameLine();
    
    if (ImGui::Button("Download page",Ui::GetSize(2)))
        ShellExecute(NULL, "open", std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" + 
        Updater::latest_version).c_str(), NULL, NULL, SW_SHOWNORMAL);
    
    // if (Updater::state == UPDATER_DOWNLOADING)
    // 	ImGui::Button("Downloading update...",Ui::GetSize());

    // if (Updater::state == UPDATER_DOWNLOADED)
    // {
    // 	if (ImGui::Button("Update downloaded. Click to install.",Ui::GetSize()))
    // 		Updater::state = UPDATER_INSTALLING;
    // }
}