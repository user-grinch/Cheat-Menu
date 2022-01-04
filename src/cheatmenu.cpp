#include "pch.h"
#include "cheatMenu.h"
#include "menuinfo.h"
#include "ui.h"
#include "updater.h"

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
			m_bShowMouse = false;
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

					if (Updater::IsUpdateAvailable())
					{
						ShowUpdateScreen();
					}
					else 
					{
						Ui::DrawHeaders(header);
						
						if (Ui::m_HeaderId == -1)
						{
							ShowWelcomeScreen();
						}
					}

					if (m_bSizeChangedExternal)
					{
						m_bSizeChangedExternal = false;
					}
					else
					{
						m_fMenuSize = ImGui::GetWindowSize();
					}
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

CheatMenu::CheatMenu()
{
	ImGui::CreateContext();
	ApplyStyle();
	pCallbackFunc = std::bind(&DrawWindow);

	// Load menu settings
	Ui::m_HeaderId = gConfig.GetValue("window.idnum", -1);
	m_fMenuSize.x = gConfig.GetValue("window.sizeX", screen::GetScreenWidth() / 4.0f);
	m_fMenuSize.y = gConfig.GetValue("window.sizeY", screen::GetScreenHeight() / 1.2f);
	srand(CTimer::m_snTimeInMilliseconds);

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

			if (m_bShowMouse != m_bShowMenu)
			{
				if (m_bShowMouse) // Only write when the menu closes
				{
					gConfig.WriteToDisk();
				}

				m_bShowMouse = m_bShowMenu;
			}
		}
	};
}

void CheatMenu::ShowWelcomeScreen()
{
	ImGui::BeginChild("WelcomeScreen");
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
	ImGui::EndChild();
}

void CheatMenu::ShowUpdateScreen()
{
	ImGui::BeginChild("UPdateScreen");
	std::string ver = Updater::GetUpdateVersion();
	ImGui::Dummy(ImVec2(0, 20));
	Ui::CenterdText("A new version of the mod is available.");
	Ui::CenterdText(std::string("Current version: ") + MENU_VERSION);
	Ui::CenterdText("Latest version: " + ver);
	ImGui::Dummy(ImVec2(0, 10));
	ImGui::TextWrapped("In order to keep using the menu, you need to update to the latest version."
		" This is to ensure everything is using the most up-to-date version.");
	ImGui::Dummy(ImVec2(0, 10));
	ImGui::TextWrapped("To know what changes are made or to download, click on the \"Download page\" button."
		" Follow the instructions there. If you're still having issues, let me know on discord.");

	ImGui::Dummy(ImVec2(0, 5));
	if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(3))))
	{
		ShellExecute(NULL, "open", DISCORD_INVITE, NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::SameLine();

	if (ImGui::Button("Download page", Ui::GetSize(3)))
	{
		ShellExecute(NULL, "open", std::string("https://github.com/user-grinch/Cheat-Menu/releases/tag/" +
			ver).c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::SameLine();

	if (ImGui::Button("Hide page", Ui::GetSize(3)))
	{
		Updater::ResetUpdaterState();
	}
	ImGui::EndChild();
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
