#include "pch.h"
#include "cheatmenu.h"
#include "imgui/imgui_internal.h"
#include "utils/widget.h"
#include "utils/updater.h"
#include "utils/d3dhook.h"
#include "utils/util.h"
#include "utils/overlay.h"
#include "pages/scene.h"
#include "pages/game.h"
#include "pages/menu.h"
#include "pages/ped.h"
#include "pages/player.h"
#include "pages/teleport.h"
#include "pages/vehicle.h"
#include "pages/visual.h"
#include "pages/weapon.h"
#include "interface/ipage.h"

static bool DrawTitleBar()
{
    bool hovered, held;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGuiID id = window->GetPageID("#CLOSE");

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
        if (m_bShowMenu)
        {
            ImGui::SetNextWindowSize(m_fMenuSize);

            if (ImGui::Begin(MENU_TITLE, NULL, ImGuiWindowFlags_NoCollapse || ImGuiWindowFlags_NoTitleBar))
            {
                m_bShowMenu = !DrawTitleBar();
                ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 350));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                    ImVec2(ImGui::GetWindowWidth() / 85, ImGui::GetWindowHeight() / 200));

                PageHandler::DrawPages();

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
    }
    Overlay::Draw();
}

void CheatMenu::Init()
{
    if (!std::filesystem::exists(PLUGIN_PATH((char*)FILE_NAME)))
    {
        Log::Print<eLogLevel::Error>("Failed to find CheatMenu directory!");
        return;
    }

    if (!D3dHook::Init(DrawWindow))
    {
        return;
    }

    // Load menu settings
    // m_nMenuPage = (eMenuPages)gConfig.Get("Window.CurrentPage", (size_t)eMenuPages::WELCOME);
    m_fMenuSize.x = gConfig.Get("Window.SizeX", screen::GetScreenWidth() / 4.0f);
    m_fMenuSize.y = gConfig.Get("Window.SizeY", screen::GetScreenHeight() / 1.2f);
    srand(CTimer::m_snTimeInMilliseconds);

    ApplyStyle();
    Locale::Init(FILE_NAME "/locale/", "English", "English");
    // GenHeaderList();
    
    // Init menu parts
    Player::Init();
    Ped::Init();
    Vehicle::Init();
    Visual::Init();
    WeaponPage::Init();
    Overlay::Init();

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
                Overlay::m_bCmdBar = !Overlay::m_bCmdBar;
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

            if (teleportPage.IsQuickTeleportActive() && quickTeleport.PressedRealtime())
            {
                D3dHook::SetMouseState(true);
            }
        }
    };
}

bool CheatMenu::IsBeingDrawn()
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

void CheatMenu::ResetSize()
{
    m_fMenuSize.x = screen::GetScreenWidth() / 4.0f;
    m_fMenuSize.y = screen::GetScreenHeight() / 1.2f;
    m_bSizeChangedExternal = true;
}
