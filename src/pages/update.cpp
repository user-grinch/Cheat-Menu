#include "pch.h"
#include "update.h"
#include "utils/widget.h"
#include "utils/updater.h"

UpdatePage& updatePage = UpdatePage::Get();

void UpdatePage::Draw()
{
    std::string ver = Updater::GetUpdateVersion();
    ImGui::Dummy(ImVec2(0, 20));
    Widget::TextCentered(TEXT("Menu.NewVersion"));
    Widget::TextCentered(std::format("{}: {}", TEXT("Menu.CurrentVersion"), MENU_VERSION));
    Widget::TextCentered(TEXT("Menu.LatestVersion") + ver);
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::TextWrapped(TEXT("Menu.UpdaterInfo1"));
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::TextWrapped(TEXT("Menu.UpdaterInfo2"));

    ImGui::Dummy(ImVec2(0, 5));
    if (ImGui::Button(TEXT("Menu.DiscordServer"), ImVec2(Widget::CalcSize(2))))
    {
        OPEN_LINK(DISCORD_INVITE);
    }

    ImGui::SameLine();

    if (ImGui::Button(TEXT("Menu.DownloadPage"), Widget::CalcSize(2)))
    {
        OPEN_LINK(std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" + ver).c_str());
    }
}