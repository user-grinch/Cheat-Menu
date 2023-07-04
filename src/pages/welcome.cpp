#include "pch.h"
#include "welcome.h"
#include "utils/widget.h"

WelcomePage& welcomePage = WelcomePage::Get();

void WelcomePage::Draw()
{
    ImGui::NewLine();

    Widget::TextCentered(TEXT("Menu.WelcomeMSG"));
    Widget::TextCentered(std::format("{}: Grinch_",TEXT("Menu.Author")));
    ImGui::Spacing();
    if (ImGui::Button(TEXT("Menu.DiscordServer"), ImVec2(Widget::CalcSize(3))))
    {
        OPEN_LINK(DISCORD_INVITE);
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Menu.GitHubRepo"), ImVec2(Widget::CalcSize(3))))
    {
        OPEN_LINK(GITHUB_LINK);
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Menu.Patreon"), ImVec2(Widget::CalcSize(3))))
    {
        OPEN_LINK(PATREON_LINK);
    }

    ImGui::NewLine();
    ImGui::TextWrapped(TEXT("Menu.BugDisclaimer"));
    ImGui::Dummy(ImVec2(0, 20));
    Widget::TextCentered(TEXT("Menu.PatreonText"));
    Widget::TextCentered(TEXT("Menu.CopyrightDisclaimer"));
}