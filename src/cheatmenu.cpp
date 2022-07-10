#include "pch.h"
#include "widget.h"
#include "updater.h"
#include "d3dhook.h"
#include "../depend/imgui/imgui_internal.h"
#include "util.h"

#include "animation.h"
#include "cheatmenu.h"
#include "game.h"
#include "menu.h"
#include "ped.h"
#include "player.h"
#include "teleport.h"
#include "vehicle.h"
#include "visual.h"
#include "weapon.h"

static bool DrawTitleBar()
{
    bool hovered, held;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGuiID id = window->GetID("#CLOSE");

    ImGui::PushFont(FontMgr::Get("title"));
    Widget::TextCentered(MENU_TITLE);

    // Return now, skip drawing close btn
    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows
                                | ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        ImGui::PopFont();
        return false;
    }

    // init draw stuff
    ImVec2 rectMin = ImGui::GetItemRectMin();
    float fontSize = ImGui::GetFontSize();
    ImRect titleRect = window->TitleBarRect();
    float framePadding = Style.FramePadding.x;
    ImVec2 pos = ImVec2(titleRect.Max.x - framePadding*2 - fontSize, titleRect.Min.y);
    const ImRect bb(pos, pos + ImVec2(fontSize, fontSize) + Style.FramePadding * 2.0f);
    ImRect bbInteract = bb;
    const float areaVisibleRatio = window->OuterRectClipped.GetArea() / bb.GetArea();
    if (areaVisibleRatio < 1.5f)
    {
        bbInteract.Expand(ImFloor(bbInteract.GetSize() * -0.25f));
    }
    bool pressed = ImGui::ButtonBehavior(bbInteract, id, &hovered, &held);

    // drawing close button here
    float cross_extent = (fontSize * 0.3f) - 1.0f;
    ImVec2 closePos = ImVec2(bb.GetCenter().x - cross_extent, rectMin.y);
    ImU32 closeCol = ImGui::GetColorU32(held || hovered ? ImVec4(0.80f, 0.0f, 0.0f, 1.0f) : ImVec4(0.80f, 0.80f, 0.80f, 1.00f));
    window->DrawList->AddText(closePos, closeCol, "X");
    ImGui::PopFont();

    return pressed;
}

void CheatMenu::DrawWindow()
{
    ImGuiIO& io = ImGui::GetIO();
    static bool bRunning = true;

    if (FrontEndMenuManager.m_bMenuActive)
    {
        if (bRunning)
        {
            gConfig.Save();
            bRunning = false;
            D3dHook::SetMouseState(false);
        }
    }
    else
    {
        bRunning = true;
        if (m_bShowMenu || BY_GAME(Menu::Commands::m_bShowMenu, true, true))
        {
            if (m_bShowMenu)
            {
                ImGui::SetNextWindowSize(m_fMenuSize);

                if (ImGui::Begin(MENU_TITLE, NULL, ImGuiWindowFlags_NoCollapse || ImGuiWindowFlags_NoTitleBar))
                {
                    m_bShowMenu = !DrawTitleBar();
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 350));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                        ImVec2(ImGui::GetWindowWidth() / 85, ImGui::GetWindowHeight() / 200));

                    ProcessPages();

                    if (m_bSizeChangedExternal)
                        m_bSizeChangedExternal = false;
                    else
                        m_fMenuSize = ImGui::GetWindowSize();

                    gConfig.Set("Window.SizeX", m_fMenuSize.x);
                    gConfig.Set("Window.SizeY", m_fMenuSize.y);

                    ImGui::PopStyleVar(2);
                    ImGui::End();
                }
            }
#ifdef GTASA
            else
            {
                Menu::DrawCommandWindow();
            }
#endif
        }
    }
    Menu::DrawOverlay();
    ShowModelInfo::Draw();
}

void CheatMenu::ProcessPages()
{
    static void* pCallback;
    ImVec2 size = Widget::CalcSize(3, false);
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
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
            color = style.Colors[ImGuiCol_Button];

        if (ImGui::InvisibleButton(text, size))
        {
            m_nMenuPage = m_headerList[i].page;
            size_t curPage = static_cast<size_t>(m_headerList[i].page);
            gConfig.Set("Window.CurrentPage", curPage);
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
        {
            ImGui::SameLine();
        }
    }
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0, 10));

    if (pCallback != nullptr && ImGui::BeginChild("HEADERCONTENT"))
    {
        static_cast<void(*)()>(pCallback)();
        ImGui::EndChild();
    }
}

void CheatMenu::GenHeaderList()
{
    CheatMenu::m_headerList = 
    {
        {TEXT_S("Window.TeleportPage"), &Teleport::ShowPage, eMenuPages::TELEPORT}, 
        {TEXT_S("Window.PlayerPage"), &Player::ShowPage, eMenuPages::PLAYER}, 
        {TEXT_S("Window.PedPage"), &Ped::ShowPage, eMenuPages::PED}, 
        {TEXT_S("Window.AnimationPage"), &Animation::ShowPage, eMenuPages::ANIMATION}, 
        {TEXT_S("Window.VehiclePage"), &Vehicle::ShowPage, eMenuPages::VEHICLE},
        {TEXT_S("Window.WeaponPage"), &Weapon::ShowPage, eMenuPages::WEAPON},
        {TEXT_S("Window.GamePage"), &Game::ShowPage, eMenuPages::GAME}, 
        {TEXT_S("Window.VisualPage"), &Visual::ShowPage, eMenuPages::VISUAL}, 
        {TEXT_S("Window.MenuPage"), &Menu::ShowPage, eMenuPages::MENU}, 
        {"Welcome", &ShowWelcomePage, eMenuPages::WELCOME, true},
        {"Update", &ShowUpdatePage, eMenuPages::UPDATE, true},
        {"Anniversary", &ShowAnniversaryPage, eMenuPages::ANNIVERSARY, true}
    };
}

void CheatMenu::Init()
{
    if (!std::filesystem::exists(PLUGIN_PATH((char*)FILE_NAME)))
    {
        Log::Print<eLogLevel::Error>(TEXT("Menu.CheatMenuNoDir"));
        return;
    }

    if (!D3dHook::InjectHook(DrawWindow))
    {
        return;
    }

    // Load menu settings
    m_nMenuPage = (eMenuPages)gConfig.Get("Window.CurrentPage", (size_t)eMenuPages::WELCOME);
    m_fMenuSize.x = gConfig.Get("Window.SizeX", screen::GetScreenWidth() / 4.0f);
    m_fMenuSize.y = gConfig.Get("Window.SizeY", screen::GetScreenHeight() / 1.2f);
    srand(CTimer::m_snTimeInMilliseconds);

    ApplyStyle();
    Locale::Init(FILE_NAME "/locale/", "English", "English");
    GenHeaderList();
    
    // Init menu parts
    Animation::Init();
    Game::Init();
    Menu::Init();
    Player::Init();
    Ped::Init();
    Teleport::Init();
    Vehicle::Init();
    Visual::Init();
    Weapon::Init();

    Events::processScriptsEvent += []()
    {
        if (!FrontEndMenuManager.m_bMenuActive)
        {
            if (menuOpen.Pressed())
            {
                m_bShowMenu = !m_bShowMenu;
            }

            if (commandWindow.Pressed())
            {
                if (Menu::Commands::m_bShowMenu)
                {
                    Menu::ProcessCommands();
                    strcpy(Menu::Commands::m_nInputBuffer, "");
                }
                Menu::Commands::m_bShowMenu = !Menu::Commands::m_bShowMenu;
            }

            bool mouseState = D3dHook::GetMouseState();
            if (mouseState != m_bShowMenu)
            {
                if (mouseState) // Only write when the menu closes
                {
                    gConfig.Save();
                }

                D3dHook::SetMouseState(m_bShowMenu);
            }

            if (quickTeleport.PressedRealtime())
            {
                D3dHook::SetMouseState(true);
            }
        }
    };
}

void CheatMenu::ShowAnniversaryPage()
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
        ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Menu.GitHubRepo"), ImVec2(Widget::CalcSize(3))))
    {
        ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
    }
}   

void CheatMenu::ShowWelcomePage()
{
    ImGui::NewLine();

    Widget::TextCentered(TEXT("Menu.WelcomeMSG"));
    Widget::TextCentered(std::format("{}: Grinch_",TEXT("Menu.Author")));

    ImGui::NewLine();
    ImGui::TextWrapped(TEXT("Menu.EnsureLatest"));
    ImGui::NewLine();
    if (ImGui::Button(TEXT("Menu.DiscordServer"), ImVec2(Widget::CalcSize(2))))
    {
        ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Menu.GitHubRepo"), ImVec2(Widget::CalcSize(2))))
    {
        ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::NewLine();
    ImGui::TextWrapped(TEXT("Menu.BugDisclaimer"));
    ImGui::Dummy(ImVec2(0, 30));
    Widget::TextCentered(TEXT("Menu.CopyrightDisclaimer"));
}

void CheatMenu::ShowUpdatePage()
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
        ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
    }

    ImGui::SameLine();

    if (ImGui::Button(TEXT("Menu.DownloadPage"), Widget::CalcSize(2)))
    {
        ShellExecute(NULL, "open", std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" +
                                               ver).c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

bool CheatMenu::IsMenuShown()
{
    return m_bShowMenu;
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
