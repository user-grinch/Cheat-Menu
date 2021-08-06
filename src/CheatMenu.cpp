#include "pch.h"
#include "CheatMenu.h"
#include "MenuInfo.h"
#include "Ui.h"
#include "Updater.h"

void CheatMenu::DrawWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	static bool bRunning = true;

#ifdef GTASA
	if (FrontEndMenuManager.m_bMenuActive)
#elif GTAVC
	if (FrontendMenuManager.m_bMenuVisible)
#endif
	{
		if (bRunning)
		{
			config.WriteToDisk();
			bRunning = false;
			m_bShowMouse = false;
		}
	}
	else
	{
		bRunning = true;
#ifdef GTASA
		if (Globals::m_bShowMenu || m_Commands::m_bShowMenu)
#elif GTAVC
		if (Globals::m_bShowMenu)
#endif
		{
			if (Globals::m_bShowMenu)
			{
				ImGui::SetNextWindowSize(Globals::m_fMenuSize);
				if (ImGui::Begin(MENU_TITLE, &Globals::m_bShowMenu, ImGuiWindowFlags_NoCollapse))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 350));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
										ImVec2(ImGui::GetWindowWidth() / 85, ImGui::GetWindowHeight() / 200));

					if (Updater::m_State == UPDATER_UPDATE_FOUND)
						Updater::ShowUpdateScreen();
					else
						Ui::DrawHeaders(header);

					Globals::m_fMenuSize = ImGui::GetWindowSize();
					config.SetValue("window.sizeX", Globals::m_fMenuSize.x);
					config.SetValue("window.sizeY", Globals::m_fMenuSize.y);

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

CheatMenu::CheatMenu()
{
	ApplyStyle();
	windowCallback = std::bind(&DrawWindow);

	// Load menu settings
	Globals::m_HeaderId = config.GetValue("window.id", std::string(""));
	Globals::m_fMenuSize.x = config.GetValue("window.sizeX", screen::GetScreenWidth() / 4.0f);
	Globals::m_fMenuSize.y = config.GetValue("window.sizeY", screen::GetScreenHeight() / 1.2f);
	srand(CTimer::m_snTimeInMilliseconds);

	Events::processScriptsEvent += []()
	{
		if (Globals::m_bInit &&
#ifdef GTASA
		!FrontEndMenuManager.m_bMenuActive
#elif GTAVC
		!FrontendMenuManager.m_bMenuVisible
#endif
		)
		{
			if (Ui::HotKeyPressed(Menu::m_HotKeys::menuOpen))
			{
				Globals::m_bShowMenu = !Globals::m_bShowMenu;
			}

			if (Ui::HotKeyPressed(Menu::m_HotKeys::commandWindow))
			{
				if (m_Commands::m_bShowMenu)
				{
					ProcessCommands();
					strcpy(m_Commands::m_nInputBuffer, "");
				}
				m_Commands::m_bShowMenu = !m_Commands::m_bShowMenu;
			}

			if (m_bShowMouse != Globals::m_bShowMenu)
			{
				if (m_bShowMouse) // Only write when the menu closes
				{
					config.WriteToDisk();
				}

				m_bShowMouse = Globals::m_bShowMenu;
			}
		}
	};
}

void CheatMenu::ApplyStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	style->WindowRounding = 1;
	style->ScrollbarRounding = 1;
	style->GrabRounding = 1;
	style->WindowRounding = 1;
	style->ChildRounding = 1;
	style->ScrollbarRounding = 1;
	style->GrabRounding = 1;
	style->FrameRounding = 0;
	style->TabRounding = 1.0;
	style->AntiAliasedLines = true;
	style->AntiAliasedFill = true;
	style->Alpha = 1;

	style->FrameBorderSize = 0;
	style->ChildBorderSize = 0;
	style->TabBorderSize = 0;
	style->WindowBorderSize = 0;
	style->PopupBorderSize = 0;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.0f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
}

void MenuThread(void* param)
{
	if (!fs::is_directory(PLUGIN_PATH((char*)"CheatMenu")))
	{
		flog << "CheatMenu folder not found. You need to put both \"CheatMenu.asi\" & \"CheatMenu\" folder in the same directory" << std::endl;
		return;
	}

	static bool bGameInit = false;
#ifdef GTASA
	Hook::ApplyMouseFix();
#endif

// Wait till the game is initialized
	Events::initRwEvent += []
	{
		bGameInit = true;
	};

	while (!bGameInit)
		Sleep(1000);

	if (GetModuleHandle("SAMP.dll"))
	{
		MessageBox(RsGlobal.ps->window, "SAMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
		return;
	}

	flog << "Starting...\nVersion: " MENU_TITLE "\nAuthor: Grinch_\nDiscord: " DISCORD_INVITE "\nMore Info: "
		GITHUB_LINK "\n" << std::endl;
	CFastman92limitAdjuster::Init();
	CheatMenu menu;

	time_t     now = time(0);
	struct tm  tstruct = *localtime(&now);
	int last_check_date = config.GetValue("config.last_update_checked", 0);

	if (last_check_date != tstruct.tm_mday)
	{
		Updater::CheckForUpdate();
		config.SetValue("config.last_update_checked", tstruct.tm_mday);
	}

	while (true)
	{
		Sleep(5000);

		if (Updater::m_State == UPDATER_CHECKING)
			Updater::CheckForUpdate();
	}
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		uint gameVersion = GetGameVersion();
#ifdef GTASA
		if (gameVersion == GAME_10US_HOODLUM || gameVersion == GAME_10US_COMPACT)
		{
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
		}
		else
		{
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA SA v1.0 US is required.", "CheatMenu", MB_ICONERROR);
		}
#elif GTAVC
		if (gameVersion == GAME_10EN)
		{
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
		}
		else
		{
			MessageBox(HWND_DESKTOP, "Unknown game version. GTA VC v1.0 EN is required.", "CheatMenu", MB_ICONERROR);
		}
#endif
	}

	return TRUE;
}
