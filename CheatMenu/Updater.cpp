#include "pch.h"
#include "Updater.h"
#include "Ui.h"
#include "MenuInfo.h"
#include "Util.h"

void Updater::CheckForUpdate()
{
	const char* link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags";
	char* path = PLUGIN_PATH((char*)"CheatMenu/json/versioninfo.json");
	HRESULT res = URLDownloadToFile(NULL, link, path, 0, NULL);

	if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
	{
		CHud::SetHelpMessage("Failed to check for updates", false, false, false);
		return;
	}
	CJson verinfo = CJson("versioninfo");

	// fetch the version number
	m_LatestVersion = verinfo.m_Data.items().begin().value()["name"].get<std::string>();

	if (m_LatestVersion > MENU_VERSION_NUMBER)
	{
		CHud::SetHelpMessage("Update found", false, false, false);
		m_State = UPDATER_UPDATE_FOUND;
	}
	else
	{
		CHud::SetHelpMessage("No update found.", false, false, false);
		Updater::m_State = UPDATER_IDLE;
	}
}

void Updater::ShowUpdateScreen()
{
	ImGui::Dummy(ImVec2(0, 20));
	Ui::CenterdText("A new version of the mod is available.");
	Ui::CenterdText(std::string("Current version: ") + MENU_VERSION);
	Ui::CenterdText("Latest version: " + Updater::m_LatestVersion);
	ImGui::Dummy(ImVec2(0, 10));
	ImGui::TextWrapped("In order to keep using the menu, you need to update to the latest version."
		" This is to ensure everything is using the most up-to-date version.");
	ImGui::Dummy(ImVec2(0, 10));
	ImGui::TextWrapped("To know what changes are made or to download, click on the \"Download page\" button."
		" Follow the instructions there. If you're still having issues, let me know on discord.");

	ImGui::Dummy(ImVec2(0, 5));
	if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(3))))
	{
		ShellExecute(NULL, "open", DISCORD_INVITE, NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::SameLine();

	if (ImGui::Button("Download page", Ui::GetSize(3)))
	{
		ShellExecute(NULL, "open", std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" +
			Updater::m_LatestVersion).c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::SameLine();

	if (ImGui::Button("Hide page", Ui::GetSize(3)))
	{
		Updater::m_State = UPDATER_IDLE;
	}
}
