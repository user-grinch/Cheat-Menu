#include "pch.h"
#include "anniversary.h"
#include "utils/widget.h"

AnniversaryPage& anniversaryPage = AnniversaryPage::Get();

void AnniversaryPage::Draw()
{
    Widget::TextCentered("Happy Anniversary!");
    ImGui::NewLine();

    ImGui::TextWrapped("On this day, in 2019, the first public version of menu was released."
                       " It's been a blast working on it and I've learned a lot in the process.\n\nThanks to you and everyone who used or"
                       " contributed to the modification in any shape or form.");

    ImGui::NewLine();
    ImGui::TextWrapped("Feel free to star the GitHub repo or join the discord server and provide feedback, ideas, or suggestions.");
    ImGui::NewLine();

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
}