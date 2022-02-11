#include "pch.h"
#include "cheatMenu.h"
#include "version.h"
#include "ui.h"
#include "updater.h"
#include "d3dhook.h"
#include "../depend/imgui/imgui_internal.h"
#include "util.h"

void CheatMenu::DrawWindow()
{
    ImGuiIO& io = ImGui::GetIO();
    static bool bRunning = true;

    if (BY_GAME(FrontEndMenuManager.m_bMenuActive, FrontendMenuManager.m_bMenuVisible, FrontEndMenuManager.m_bMenuActive))
    {
        if (bRunning)
        {
            gConfig.WriteToDisk();
            bRunning = false;
            D3dHook::SetMouseState(false);
        }
    }
    else
    {
        bRunning = true;
        if (m_bShowMenu || BY_GAME(m_Commands::m_bShowMenu, true, true))
        {
            if (m_bShowMenu)
            {
                ImGui::SetNextWindowSize(m_fMenuSize);

                if (ImGui::Begin(MENU_TITLE, NULL, ImGuiWindowFlags_NoCollapse || ImGuiWindowFlags_NoTitleBar))
                {
                    m_bShowMenu = !Ui::DrawTitleBar();
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 350));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                        ImVec2(ImGui::GetWindowWidth() / 85, ImGui::GetWindowHeight() / 200));

                    ProcessPages();

                    if (m_bSizeChangedExternal)
                        m_bSizeChangedExternal = false;
                    else
                        m_fMenuSize = ImGui::GetWindowSize();

                    gConfig.SetValue("window.sizeX", m_fMenuSize.x);
                    gConfig.SetValue("window.sizeY", m_fMenuSize.y);

                    ImGui::PopStyleVar(2);
                    ImGui::End();
                }
            }
#ifdef GTASA
            else
            {
                DrawShortcutsWindow();
            }
#endif
        }
    }
    DrawOverlay();
}

void CheatMenu::ProcessPages()
{
    static void* pCallback;
    ImVec2 size = Ui::GetSize(3, false);
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushFont(FontMgr::GetFont("header"));
    m_nMenuPage = Updater::IsUpdateAvailable() ? eMenuPages::UPDATE : m_nMenuPage;
    
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
            bool flag = gConfig.GetValue("window.anniversaryShown", false);

            if (!flag)
            {
                gConfig.SetValue("window.anniversaryShown", true);
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
                pCallback = m_headerList[i].pFunc;

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
            color = style.Colors[ImGuiCol_Button];

        if (ImGui::InvisibleButton(text, size))
        {
            m_nMenuPage = m_headerList[i].page;
            size_t curPage = static_cast<size_t>(m_headerList[i].page);
            gConfig.SetValue("window.page", curPage);
            pCallback = m_headerList[i].pFunc;
            Updater::ResetUpdaterState();
        }

        if (ImGui::IsItemHovered())
            color = style.Colors[ImGuiCol_ButtonHovered];

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
            ImGui::SameLine();
    }
    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0, 10));

    if (pCallback != nullptr && ImGui::BeginChild("HEADERCONTENT"))
    {
        static_cast<void(*)()>(pCallback)();
        ImGui::EndChild();
    }
}

CheatMenu::CheatMenu()
{
    if (!D3dHook::InjectHook(DrawWindow))
    {
        return;
    }

    ApplyStyle();

    // Load menu settings
    m_nMenuPage = (eMenuPages)gConfig.GetValue("window.page", (size_t)eMenuPages::WELCOME);
    m_fMenuSize.x = gConfig.GetValue("window.sizeX", screen::GetScreenWidth() / 4.0f);
    m_fMenuSize.y = gConfig.GetValue("window.sizeY", screen::GetScreenHeight() / 1.2f);
    srand(CTimer::m_snTimeInMilliseconds);

    Locale::Init("CheatMenu/locale/");
    Events::processScriptsEvent += []()
    {
        if (!BY_GAME(FrontEndMenuManager.m_bMenuActive, FrontendMenuManager.m_bMenuVisible, FrontEndMenuManager.m_bMenuActive))
        {
            if (menuOpen.Pressed())
            {
                m_bShowMenu = !m_bShowMenu;
            }

            if (commandWindow.Pressed())
            {
                if (m_Commands::m_bShowMenu)
                {
                    ProcessCommands();
                    strcpy(m_Commands::m_nInputBuffer, "");
                }
                m_Commands::m_bShowMenu = !m_Commands::m_bShowMenu;
            }

            bool mouseState = D3dHook::GetMouseState();
            if (mouseState != m_bShowMenu)
            {
                if (mouseState) // Only write when the menu closes
                {
                    gConfig.WriteToDisk();
                }

                D3dHook::SetMouseState(m_bShowMenu);
            }
        }
    };
}

CheatMenu::~CheatMenu()
{
    D3dHook::RemoveHook();
}

/*
* YIKES YOU AREN"T SUPPOSED TO FIND THIS YOU KNOW!!!
* Probably a good easter egg for the upcoming anniversary ;)
*/
void CheatMenu::ShowAnniversaryPage()
{
    Ui::CenterdText("Happy Anniversary!");
    ImGui::NewLine();

    static bool showHistory;
    if (showHistory)
    {
        ImGui::Checkbox("Show backstory", &showHistory);
        ImGui::BeginChild("BACKSTORY");
        ImGui::TextWrapped("I wanted to share the backstory behind the initial idea or plan behind the menu."
" This is gonna be long so feel free to skip it if you're not interested.");
        ImGui::NewLine();

        ImGui::TextWrapped("The original idea of the menu comes way back from 2016! The inspiration for the menu"
" is from the 'CheatMenu by UNRATED69'. I wanted something that had some more features and worked with SAxVCxLC."
" But there not being any other CheatMenu's back then, I wanted to make something myself but lacked the knowledge to do so.");
        ImGui::NewLine();

        ImGui::TextWrapped("In 2018, I finally got an opportunity to learn CLEO or GTA3Script after Junior released"
" his tutorial. I started from basics but it soon became apparent that due to the limitations of CLEO, creating menus were"
" really tedious.");

        ImGui::NewLine();
        ImGui::TextWrapped("Later that year I found Moonloader, which had ImGui support. Meaning I could make menus"
" without brain fucking myself (kudos to everyone who writes 100s of lines in CLEO). I recall starting working on"
" the menu in October/November that same year.");

        ImGui::NewLine();
        ImGui::TextWrapped("I had high hopes the mod would succeed and the menu was nowhere near the state I wanted"
" it to be. But over a hot conversation with KKJJ, I finally decided to add the absolute bare minimum of features and"
" see what happens. And to my surprise, it even surpassed all of my expectations and became my most popular mod to this day.");

        ImGui::NewLine();
        ImGui::TextWrapped("A part of me is already cringing telling the story but it is what it is. I've learned"
" a lot working on this mod and I'm grateful. If you made it through all this way, kudos, you're awesome.");

        ImGui::NewLine();
        ImGui::TextWrapped("Again, thanks to you and everyone who used or helped me along the way. Enjoy ;)");

        ImGui::EndChild();
    }
    else
    {
        ImGui::TextWrapped("On this day, in 2019, the first public version of menu was released in MixMods Forum." 
    " It's been a blast working on it and I've learned a lot in the process.\n\nThanks to you and everyone who used or" 
    " contributed to the modification in any form or shape.");

        ImGui::NewLine();
        ImGui::TextWrapped("Feel free to star the GitHub repo or join the discord server and provide feedback, ideas, or suggestions.");
        ImGui::NewLine();

        if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(2))))
        {
            ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
        }

        ImGui::SameLine();

        if (ImGui::Button("GitHub repo", ImVec2(Ui::GetSize(2))))
        {
            ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
        }

        ImGui::NewLine();

        ImGui::Checkbox("Show backstory", &showHistory);
    }
}   

void CheatMenu::ShowWelcomePage()
{
    ImGui::NewLine();

    Ui::CenterdText("Welcome to Cheat Menu");
    Ui::CenterdText("Author: Grinch_");

    ImGui::NewLine();
    ImGui::TextWrapped("Please ensure you have the latest version from GitHub.");
    ImGui::NewLine();
    if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(2))))
    {
        ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
    }

    ImGui::SameLine();

    if (ImGui::Button("GitHub repo", ImVec2(Ui::GetSize(2))))
    {
        ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
    }

    ImGui::NewLine();
    ImGui::TextWrapped("If you find bugs or have suggestions, you can let me know on discord :)");
    ImGui::Dummy(ImVec2(0, 30));
    Ui::CenterdText("Copyright Grinch_ 2019-2022. All rights reserved.");
}

void CheatMenu::ShowUpdatePage()
{
    std::string ver = Updater::GetUpdateVersion();
    ImGui::Dummy(ImVec2(0, 20));
    Ui::CenterdText("A new version of the menu is available.");
    Ui::CenterdText(std::string("Current version: ") + MENU_VERSION);
    Ui::CenterdText("Latest version: " + ver);
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::TextWrapped("It's highly recommanded to update to the latest version."
                       " Newer versions may contain new features and bug fixes.");
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::TextWrapped("To know what changes are made or to download, click on the 'Download page' button."
                       " Follow the instructions there. If you're still having issues, let me know on discord.");

    ImGui::Dummy(ImVec2(0, 5));
    if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(2))))
        ShellExecute(NULL, "open", DISCORD_INVITE, NULL, NULL, SW_SHOWNORMAL);

    ImGui::SameLine();

    if (ImGui::Button("Download page", Ui::GetSize(2)))
    {
        ShellExecute(NULL, "open", std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" +
                                               ver).c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

void CheatMenu::ApplyStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    style->WindowPadding = ImVec2(8, 8);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(8, 8);
    style->FrameRounding = 5.0f;
    style->PopupRounding = 5.0f;
    style->ItemSpacing = ImVec2(7, 7);
    style->ItemInnerSpacing = ImVec2(7, 7);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 12.0f;
    style->ScrollbarRounding = 10.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->ChildBorderSize = 0;
    style->WindowBorderSize = 0;
    style->FrameBorderSize = 0;
    style->TabBorderSize = 0;
    style->PopupBorderSize = 0;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.33f, 0.3f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.06f, 0.95f);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.05f, 0.06f, 0.95f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.5f, 0.5f, 0.3f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.3f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 0.3f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.12f, 0.12f, 0.12f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.06f, 0.05f, 0.06f, 0.95f);
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.6f);
}

void CheatMenu::ResetMenuSize()
{
    m_fMenuSize.x = screen::GetScreenWidth() / 4.0f;
    m_fMenuSize.y = screen::GetScreenHeight() / 1.2f;
    m_bSizeChangedExternal = true;
}
