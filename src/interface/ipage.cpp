#include "pch.h"
#include "ipage.h"
#include "utils/updater.h"
#include "utils/widget.h"

static void DrawAnniversaryPage()
{
    Widget::TextCentered("Happy Anniversary!");
    ImGui::NewLine();

    ImGui::TextWrapped("On this day, in 2019, the first public version of menu was released in MixMods Forum." 
    " It's been a blast working on it and I've learned a lot in the process.\n\nThanks to you and everyone who used or" 
    " contributed to the modification in any form or shape.");

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
}   

static void DrawWelcomePage()
{
    ImGui::NewLine();

    Widget::TextCentered(TEXT("Menu.WelcomeMSG"));
    Widget::TextCentered(std::format("{}: Grinch_",TEXT("Menu.Author")));

    ImGui::NewLine();
    ImGui::TextWrapped(TEXT("Menu.EnsureLatest"));
    if (ImGui::Button(TEXT("Menu.DiscordServer"), ImVec2(Widget::CalcSize(2))))
    {
        OPEN_LINK(DISCORD_INVITE);
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Menu.GitHubRepo"), ImVec2(Widget::CalcSize(2))))
    {
        OPEN_LINK(GITHUB_LINK);
    }
    ImGui::NewLine();
    ImGui::TextWrapped(TEXT("Menu.BugDisclaimer"));
    ImGui::Dummy(ImVec2(0, 20));
    Widget::TextCentered(TEXT("Menu.PatreonText"));
    if (ImGui::Button(TEXT("Menu.Patreon"), ImVec2(Widget::CalcSize(1))))
    {
        OPEN_LINK("https://www.patreon.com/grinch_");
    }
    ImGui::Dummy(ImVec2(0, 30));
    Widget::TextCentered(TEXT("Menu.CopyrightDisclaimer"));
}

static void DrawUpdatePage()
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
        ShellExecute(NULL, "open", std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" +
                                               ver).c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

template <typename T>
void IPage<T>::Process()
{
    ImVec2 size = Widget::CalcSize(3, false);
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    if (Updater::IsUpdateAvailable())
    {
        m_nCurrentPage = eMenuPage::Update;
    }
    
    // Check once if it's anniversary day
    static bool aniCheckDone;
    if (!aniCheckDone)
    {
        SYSTEMTIME st;
        GetSystemTime(&st);

        if (st.wMonth == 3 && st.wDay == 28)
        {
            /*
            *   We don't want to be annoying and
            *   show anniversary screen on every game start
            */
            bool flag = gConfig.Get("Window.AnniversaryShown", false);

            if (!flag)
            {
                gConfig.Set("Window.AnniversaryShown", true);
                m_nMenuPage = eMenuPages::ANNIVERSARY;
            }
        }
        aniCheckDone = true;
    }

    ImDrawList *pDrawList = ImGui::GetWindowDrawList();
    for (size_t i = 0; i < m_headerList.size(); ++i)
    {
        /*
        * For welcome & update pages
        * They don't need to add item in the header list
        */
        if (m_headerList[i].skipHeader)
        {
            if (m_nMenuPage == m_headerList[i].page)
            {
                pCallback = m_headerList[i].pFunc;
            }

            continue;
        }

        const char* text = m_headerList[i].name.c_str();

        ImVec4 color;
        if (m_headerList[i].page == m_nMenuPage)
        {
            color = style.Colors[ImGuiCol_ButtonActive];
            pCallback = m_headerList[i].pFunc;
        }
        else
        {
            color = style.Colors[ImGuiCol_Button];
        }

        if (ImGui::InvisibleButton(text, size))
        {
            m_nMenuPage = m_headerList[i].page;
            size_t curPage = static_cast<size_t>(m_headerList[i].page);
            gConfig.Set("Window.CurrentPage", curPage);
            pCallback = m_headerList[i].pFunc;
            Updater::ResetUpdaterState();
        }

        if (ImGui::IsItemHovered())
        {
            color = style.Colors[ImGuiCol_ButtonHovered];
        }

        /*
        * Window rounding flags
        * TODO: hardcoded atm
        */
        ImDrawFlags flags = ImDrawFlags_RoundCornersNone;
        if (i == 0) flags = ImDrawFlags_RoundCornersTopLeft;
        if (i == 2) flags = ImDrawFlags_RoundCornersTopRight;
        if (i == 6) flags = ImDrawFlags_RoundCornersBottomLeft;
        if (i == 8) flags = ImDrawFlags_RoundCornersBottomRight;

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImVec2 size = ImGui::CalcTextSize(text);
        pDrawList->AddRectFilled(min, max, ImGui::GetColorU32(color), style.FrameRounding, flags);
        ImGui::RenderTextClipped(min + style.FramePadding, max - style.FramePadding, text, NULL, &size, style.ButtonTextAlign);

        if (i % 3 != 2)
        {
            ImGui::SameLine();
        }
    }
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0, 10));

    if (m_pCurrentPage != nullptr && ImGui::BeginChild("HEADERCONTENT"))
    {
        m_pCurrentPage->Draw();
        ImGui::EndChild();
    }
}