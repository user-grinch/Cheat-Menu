#include "pch.h"
#include "cheatmenu.h"
#include "imgui/imgui_internal.h"
#include "utils/widget.h"
#include "utils/updater.h"
#include "utils/d3dhook.h"
#include "utils/overlay.h"
#include "interface/ipage.h"
#include "pages/teleport.h"
#include "pages/menu.h"

static bool DrawTitleBar()
{
    bool hovered, held;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGuiID id = window->GetID("#CLOSE");
    ImGui::KeepAliveID(id);

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

void CheatMenuMgr::Draw()
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
        if (m_bVisible)
        {
            ImGui::SetNextWindowSize(m_fSize, ImGuiCond_Once);
            ImGui::SetNextWindowPos(m_fPos, ImGuiCond_Once);

            if (m_bWindowParamUpdated)
            {
                ImGui::SetNextWindowSize(m_fSize);
                ImGui::SetNextWindowPos(m_fPos);
                m_bWindowParamUpdated = false;
            }

            if (ImGui::Begin(MENU_TITLE, NULL, ImGuiWindowFlags_NoCollapse || ImGuiWindowFlags_NoTitleBar))
            {
                m_bVisible = !DrawTitleBar();
                ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 350));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                    ImVec2(ImGui::GetWindowWidth() / 85, ImGui::GetWindowHeight() / 200));
                PageHandler::DrawPages();

                if (ImGui::IsWindowHovered())
                {
                    m_fSize = ImGui::GetWindowSize();
                    m_fPos = ImGui::GetWindowPos();
                    gConfig.Set("Window.PosX", m_fPos.x);
                    gConfig.Set("Window.PosY", m_fPos.y);
                    gConfig.Set("Window.SizeX", m_fSize.x);
                    gConfig.Set("Window.SizeY", m_fSize.y);
                }
                ImGui::PopStyleVar(2);
                ImGui::End();
            }
        }
    }
    Overlay::Draw();
}

CheatMenuMgr& CheatMenu = CheatMenuMgr::Get();
CheatMenuMgr::CheatMenuMgr()
{
    Events::initRwEvent += [this]()
    {
        /*
            Had to put this in place since some people put the folder in root
            directory and the asi in modloader. Why??
        */
        if (!std::filesystem::is_directory(PLUGIN_PATH((char*)FILE_NAME)))
        {
            std::string msg = std::format("{} folder not found. You need to put both '{}.asi' & '{}' folder in the same directory", FILE_NAME, FILE_NAME, FILE_NAME);
            Log::Print<eLogLevel::Error>(msg.c_str());
            MessageBox(NULL, msg.c_str(), FILE_NAME, MB_ICONERROR);
            return;
        }

        /*
            Need SilentPatch since all gta games have issues with mouse input
            Implementing mouse fix is a headache anyway
        */
        if (!GetModuleHandle(BY_GAME("SilentPatchSA.asi","SilentPatchVC.asi","SilentPatchIII.asi")))
        {
            Log::Print<eLogLevel::Error>("SilentPatch not found. Please install it from here https://gtaforums.com/topic/669045-silentpatch/");
            int msgID = MessageBox(NULL, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", FILE_NAME, MB_OKCANCEL | MB_DEFBUTTON1);

            if (msgID == IDOK)
            {
                OPEN_LINK("https://gtaforums.com/topic/669045-silentpatch/");
            };
            return;
        }

        Log::Print<eLogLevel::None>("Starting " MENU_TITLE " (" __DATE__ ")\nAuthor: Grinch_\nDiscord: "
                                DISCORD_INVITE "\nMore Info: " GITHUB_LINK);

        // date time
        SYSTEMTIME st;
        GetSystemTime(&st);
        Log::Print<eLogLevel::None>("Date: {}-{}-{} Time: {}:{}\n", st.wYear, st.wMonth, st.wDay,
                                    st.wHour, st.wMinute);

        /*
            TODO: Find a better way
            Since you could still name it something else
        */
#ifdef GTASA
        if (GetModuleHandle("SAMP.dll") || GetModuleHandle("SAMP.asi"))
        {
            Log::Print<eLogLevel::Error>(FILE_NAME " doesn't support SAMP");
            MessageBox(RsGlobal.ps->window, "SAMP detected. Exiting...", FILE_NAME, MB_ICONERROR);
            exit(EXIT_FAILURE);
        }
        CFastman92limitAdjuster::Init();
        Log::Print<eLogLevel::None>("Game detected: GTA San Andreas 1.0 US");
#elif GTAVC
        if (GetModuleHandle("vcmp-proxy.dll") || GetModuleHandle("vcmp-proxy.asi"))
        {
            Log::Print<eLogLevel::Error>(FILE_NAME " doesn't support VCMP");
            MessageBox(RsGlobal.ps->window, "VCMP detected. Exiting...", FILE_NAME, MB_ICONERROR);
            exit(EXIT_FAILURE);
        }
        Log::Print<eLogLevel::None>("Game detected: GTA Vice City 1.0 EN");
#else
        Log::Print<eLogLevel::None>("Game detected: GTA III 1.0 EN");
#endif

        bool modloader = GetModuleHandle("modloader.asi");
        const char *path = PLUGIN_PATH((char*)"");
        Log::Print<eLogLevel::None>("Install location: {}", modloader && strstr(path, "modloader") ? "Modloader" : "Game directory");
        Log::Print<eLogLevel::None>("Font support package: {}", FontMgr::IsSupportPackageInstalled() ? "True" : "False");
        Log::Print<eLogLevel::None>("\nCLEO installed: {}", GetModuleHandle("cleo.asi") || GetModuleHandle("cleo_redux.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("FLA installed: {}", GetModuleHandle("$fastman92limitAdjuster.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("Mixsets installed: {}", GetModuleHandle("MixSets.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("Modloader installed: {}", modloader ? "True" : "False");
        Log::Print<eLogLevel::None>("OLA installed: {}", GetModuleHandle("III.VC.SA.LimitAdjuster.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("ProjectProps installed: {}", GetModuleHandle("ProjectProps.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("Renderhook installed: {}", GetModuleHandle("_gtaRenderHook.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("Widescreen Fix installed: {}\n", GetModuleHandle("GTASA.WidescreenFix.asi") ? "True" : "False");

        // Checking for updates once a day
        if (menuPage.m_bAutoCheckUpdate && gConfig.Get("Menu.LastUpdateChecked", 0) != st.wDay)
        {
            Updater::CheckUpdate();
            gConfig.Set("Menu.LastUpdateChecked", st.wDay);
        }
        
        if (Updater::IsUpdateAvailable())
        {
            Log::Print<eLogLevel::Info>("New update available: %s", Updater::GetUpdateVersion().c_str());
        }

        m_fSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);
        if (!std::filesystem::exists(PLUGIN_PATH((char*)FILE_NAME)))
        {
            Log::Print<eLogLevel::Error>("Failed to find CheatMenu directory!");
            return;
        }

        Locale::Init(FILE_NAME "/locale/", "English", "English");
        Locale::SetLocaleByName(gConfig.Get("Menu.Language", ""));
        Overlay::Init();

        // Load menu settings
        m_fSize.x = gConfig.Get("Window.SizeX", screen::GetScreenWidth() / 4.0f);
        m_fSize.y = gConfig.Get("Window.SizeY", screen::GetScreenHeight() / 1.2f);
        m_fPos.x = gConfig.Get("Window.PosX", 50.0f);
        m_fPos.y = gConfig.Get("Window.PosY", 50.0f);

        srand(CTimer::m_snTimeInMilliseconds);
    };

    // Doesn't work with ThirteenAG's windowed mode while inside initRwEvent
    Events::initGameEvent += [this]() 
    {
        if (!D3dHook::Init(fArgNoneWrapper(CheatMenu.Draw)))
        {
            return;
        }

        ApplyStyle();
    };

    Events::processScriptsEvent += [this]()
    {
        if (!FrontEndMenuManager.m_bMenuActive)
        {
            if (menuOpen.Pressed())
            {
                m_bVisible = !m_bVisible;
            }

            if (commandWindow.Pressed())
            {
                Overlay::m_bCmdBar = !Overlay::m_bCmdBar;
            }

            bool mouseState = D3dHook::GetMouseState();
            if (mouseState != m_bVisible)
            {
                if (mouseState) // Only write when the menu closes
                {
                    gConfig.Save();
                }

                D3dHook::SetMouseState(m_bVisible);
            }

            if (teleportPage.IsQuickTeleportActive() && quickTeleport.PressedRealtime())
            {
                D3dHook::SetMouseState(true);
            }
        }
    };
}

bool CheatMenuMgr::IsVisible()
{
    return m_bVisible;
}

void CheatMenuMgr::ApplyStyle()
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

void CheatMenuMgr::ResetParams()
{
    m_fSize.x = screen::GetScreenWidth() / 4.0f;
    m_fSize.y = screen::GetScreenHeight() / 1.2f;
    m_fPos = {50.0f, 50.0f};
    m_bWindowParamUpdated = true;
}
