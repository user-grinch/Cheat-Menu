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
#include "utils/savemgr.h"

static bool gameStartFlag = true;

void CheatMenuMgr::Draw()
{
    ImGuiIO& io = ImGui::GetIO();
    static bool bRunning = true;

    if (m_bUpdateColors)
    {
        ApplyStyle(m_Color);
        m_bUpdateColors = false;
    }
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
                m_bVisible = !PageHandler::DrawPages();
                if (ImGui::IsWindowHovered())
                {
                    m_fSize = ImGui::GetWindowSize();
                    m_fPos = ImGui::GetWindowPos();
                    gConfig.Set("Window.Pos.X", m_fPos.x);
                    gConfig.Set("Window.Pos.Y", m_fPos.y);
                    gConfig.Set("Window.Size.X", m_fSize.x);
                    gConfig.Set("Window.Size.Y", m_fSize.y);
                }
                ImGui::End();
            }
        }
    }
    Overlay::Draw();
}

void CheatMenuMgr::SetVisibility(bool flag) 
{
    m_bVisible = flag;
}

void CheatMenuMgr::UpdateAccentColor(float *col) 
{
    m_Color = {int(col[0] * 255), int(col[1] * 255), int(col[2] * 255), 255};
    m_bUpdateColors = true;
}

CheatMenuMgr& CheatMenu = CheatMenuMgr::Get();
CheatMenuMgr::CheatMenuMgr()
{
    // Needed to be init here
    ImGui::CreateContext();

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

        Log::Print<eLogLevel::None>("Starting " MENU_TITLE " (" __DATE__ ")\nAuthor: Grinch_\nDiscord: "
                                    DISCORD_INVITE "\nPatreon: " PATREON_LINK "\nMore Info: " GITHUB_LINK "\n");

        // date time
        SYSTEMTIME st;
        GetSystemTime(&st);
        Log::Print<eLogLevel::None>("Date: {}-{}-{} Time: {}:{}\n", st.wYear, st.wMonth, st.wDay,
                                    st.wHour, st.wMinute);

        bool modloader = GetModuleHandle("modloader.asi");
        const char *path = PLUGIN_PATH((char*)"");
        Log::Print<eLogLevel::None>("Install location: {}", modloader && strstr(path, "modloader") ? "Modloader" : "Game directory");
        Log::Print<eLogLevel::None>("FLA installed: {}", GetModuleHandle("$fastman92limitAdjuster.asi") ? "True" : "False");
        Log::Print<eLogLevel::None>("Modloader installed: {}", modloader ? "True" : "False");
        Log::Print<eLogLevel::None>("OLA installed: {}", GetModuleHandle("III.VC.SA.LimitAdjuster.asi") ? "True" : "False");
#ifdef GTASA
        Log::Print<eLogLevel::None>("Renderhook installed: {}", GetModuleHandle("_gtaRenderHook.asi") ? "True" : "False");
#endif
        Log::Print<eLogLevel::None>("");
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
        m_fSize.x = gConfig.Get("Window.Size.X", screen::GetScreenWidth() / 2.0f);
        m_fSize.y = gConfig.Get("Window.Size.Y", screen::GetScreenHeight() / 1.7f);
        m_fPos.x = gConfig.Get("Window.Pos.X", 50.0f);
        m_fPos.y = gConfig.Get("Window.Pos.Y", 50.0f);

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


    Events::initScriptsEvent.before += [this]() 
    {
        gameStartFlag = true;


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
    };

    Events::processScriptsEvent += [this]()
    {
        // run this once every new game/ load game
        if (gameStartFlag)
        {
            SaveMgr::InitAndLoad();
            gameStartFlag = false;
        }

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

void CheatMenuMgr::ApplyStyle(ImColor accent_col)
{
    ImGuiStyle *style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    
    style->FrameBorderSize = 0.0f;
    style->WindowBorderSize = 0.0f;
    style->PopupBorderSize = 0.0f;
    style->ScrollbarRounding = 3.0f;
    style->TabRounding = 0.0f;
    style->TabBorderSize = 0.0f;
    style->WindowRounding = 0.0f;
    style->ChildRounding = 0.0f;
    style->FrameRounding = 0.0f;
    style->GrabRounding = 0.0f;
    style->PopupRounding = 0.0f; 
    style->ScrollbarSize = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->WindowPadding = ImVec2(5, 5);
    style->ItemSpacing = ImVec2(5, 5);

    style->Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 240);
    style->Colors[ImGuiCol_Border] = ImColor(33, 34, 45, 255);
    style->Colors[ImGuiCol_Button] = ImColor(33, 34, 45, 255);
    style->Colors[ImGuiCol_ButtonActive] = accent_col;
    style->Colors[ImGuiCol_ButtonHovered] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(33, 34, 45, 255);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = accent_col;
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_FrameBg] = ImColor(33, 34, 45, 255);
    style->Colors[ImGuiCol_FrameBgActive] = accent_col;
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
    style->Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);
    style->Colors[ImGuiCol_CheckMark] = accent_col;
    style->Colors[ImGuiCol_SliderGrab] = accent_col;
    style->Colors[ImGuiCol_SliderGrabActive] = accent_col;
    style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
    style->Colors[ImGuiCol_HeaderHovered] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_HeaderActive] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_ResizeGrip] = ImColor(220, 50, 66, 0);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_ResizeGripActive] = accent_col;
    style->Colors[ImGuiCol_SeparatorHovered] = ImColor(30, 30, 40, 255);
    style->Colors[ImGuiCol_SeparatorActive] = accent_col;
    style->Colors[ImGuiCol_TitleBgActive] = accent_col;
    style->Colors[ImGuiCol_Tab] = ImColor(0, 0, 0, 0);
    style->Colors[ImGuiCol_TabActive] = ImColor(33, 34, 45, 255);
    style->Colors[ImGuiCol_TabHovered] = ImColor(30, 31, 42, 255);
}

void CheatMenuMgr::ResetParams()
{
    m_fSize.x = screen::GetScreenWidth() / 2.0f;
    m_fSize.y = screen::GetScreenHeight() / 1.7f;
    m_fPos = {50.0f, 50.0f};
    m_bWindowParamUpdated = true;
}
