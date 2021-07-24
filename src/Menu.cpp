#include "pch.h"
#include "MenuInfo.h"
#include "Menu.h"
#include "Teleport.h"
#include "Weapon.h"
#include "Vehicle.h"
#include "Ui.h"
#include "Util.h"
#include "Updater.h"

Menu::Menu()
{
	// TODO: use structs
	// Load config data
	m_Overlay::bCoord = config.GetValue("overlay.coord", false);
	m_Overlay::bCpuUsage = config.GetValue("overlay.cpu_usage", false);
	m_Overlay::bFPS = config.GetValue("overlay.fps", false);
	m_Overlay::bLocName = config.GetValue("overlay.loc_name", false);
	m_Overlay::bTransparent = config.GetValue("overlay.transparent", false);
	m_Overlay::bMemUsage = config.GetValue("overlay.mem_usage", false);
	m_Overlay::bVehHealth = config.GetValue("overlay.veh_health", false);
	m_Overlay::bVehSpeed = config.GetValue("overlay.veh_speed", false);
	m_Overlay::mSelectedPos = (DISPLAY_POS)config.GetValue("overlay.selected_pos", (int)DISPLAY_POS::BOTTOM_RIGHT);
	m_Overlay::fPosX = config.GetValue("overlay.pox", 0);
	m_Overlay::fPosY = config.GetValue("overlay.posy", 0);

	// Hotkeys
	m_HotKeys::aimSkinChanger.m_key1 = config.GetValue("hotkey.aim_skin_changer.key1", VK_RETURN);
	m_HotKeys::aimSkinChanger.m_key2 = config.GetValue("hotkey.aim_skin_changer.key2", VK_RETURN);

	m_HotKeys::freeCam.m_key1 = config.GetValue("hotkey.freecam.key1", VK_F6);
	m_HotKeys::freeCam.m_key2 = config.GetValue("hotkey.freecam.key2", VK_F6);

	m_HotKeys::quickSceenShot.m_key1 = config.GetValue("hotkey.quick_screenshot.key1", VK_F5);
	m_HotKeys::quickSceenShot.m_key2 = config.GetValue("hotkey.quick_screenshot.key2", VK_F5);

	m_HotKeys::quickTeleport.m_key1 = config.GetValue("hotkey.quick_tp.key1", VK_KEY_X);
	m_HotKeys::quickTeleport.m_key2 = config.GetValue("hotkey.quick_tp.key2", VK_KEY_Y);

	m_HotKeys::menuOpen.m_key1 = VK_LCONTROL; config.GetValue("hotkey.menu_open.key1", VK_LCONTROL);
	m_HotKeys::menuOpen.m_key2 = VK_KEY_M; config.GetValue("hotkey.menu_open.key2", VK_KEY_M);

	m_HotKeys::commandWindow.m_key1 = config.GetValue("hotkey.command_window.key1", VK_LMENU);
	m_HotKeys::commandWindow.m_key2 = config.GetValue("hotkey.command_window.key2", VK_KEY_C);

	m_HotKeys::flipVeh.m_key1 = config.GetValue("hotkey.flip_veh.key1", VK_NONE);
	m_HotKeys::flipVeh.m_key2 = config.GetValue("hotkey.flip_veh.key2", VK_NONE);

	m_HotKeys::fixVeh.m_key1 = config.GetValue("hotkey.fix_veh.key1", VK_NONE);
	m_HotKeys::fixVeh.m_key2 = config.GetValue("hotkey.fix_veh.key2", VK_NONE);

	m_HotKeys::godMode.m_key1 = config.GetValue("hotkey.god_mode.key1", VK_NONE);
	m_HotKeys::godMode.m_key2 = config.GetValue("hotkey.god_mode.key2", VK_NONE);

	m_HotKeys::vehEngine.m_key1 = config.GetValue("hotkey.veh_engine.key1", VK_NONE);
	m_HotKeys::vehEngine.m_key2 = config.GetValue("hotkey.veh_engine.key2", VK_NONE);

	m_HotKeys::vehInstantStart.m_key1 = config.GetValue("hotkey.veh_instant_start.key1", VK_NONE);
	m_HotKeys::vehInstantStart.m_key2 = config.GetValue("hotkey.veh_instant_start.key2", VK_NONE);

	m_HotKeys::vehInstantStop.m_key1 = config.GetValue("hotkey.veh_instant_stop.key1", VK_NONE);
	m_HotKeys::vehInstantStop.m_key2 = config.GetValue("hotkey.veh_instant_stop.key2", VK_NONE);

	Util::GetCPUUsageInit();
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	m_Overlay::mTotalRam = static_cast<int>(memInfo.ullTotalPhys * 1e-6); // Bytes -> MegaBytes
}

void Menu::DrawOverlay()
{
	CPlayerPed* player = FindPlayerPed();
	bool m_bShowMenu = m_Overlay::bCoord || m_Overlay::bFPS || m_Overlay::bLocName || m_Overlay::bCpuUsage || m_Overlay::bMemUsage ||
		((m_Overlay::bVehHealth || m_Overlay::bVehSpeed) && player && player->m_pVehicle && player->m_pVehicle->IsDriver(player));

	const float offset = 10.0f;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	if (m_Overlay::mSelectedPos == DISPLAY_POS::CUSTOM)
	{
		if (m_Overlay::fPosX != NULL && m_Overlay::fPosY != NULL)
		{
			config.SetValue("overlay.posx", m_Overlay::fPosX);
			config.SetValue("overlay.posy", m_Overlay::fPosY);
			ImGui::SetNextWindowPos(ImVec2(m_Overlay::fPosX, m_Overlay::fPosY), ImGuiCond_Once);
		}
	}
	else
	{
		window_flags |= ImGuiWindowFlags_NoMove;
		ImVec2 pos, pivot;

		if (m_Overlay::mSelectedPos == DISPLAY_POS::TOP_LEFT)
		{
			pos = ImVec2(offset, offset);
			pivot = ImVec2(0.0f, 0.0f);
		}

		if (m_Overlay::mSelectedPos == DISPLAY_POS::TOP_RIGHT)
		{
			pos = ImVec2(io.DisplaySize.x - offset, offset);
			pivot = ImVec2(1.0f, 0.0f);
		}

		if (m_Overlay::mSelectedPos == DISPLAY_POS::BOTTOM_LEFT)
		{
			pos = ImVec2(offset, io.DisplaySize.y - offset);
			pivot = ImVec2(0.0f, 1.0f);
		}

		if (m_Overlay::mSelectedPos == DISPLAY_POS::BOTTOM_RIGHT)
		{
			pos = ImVec2(io.DisplaySize.x - offset, io.DisplaySize.y - offset);
			pivot = ImVec2(1.0f, 1.0f);
		}

		ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
	}

	ImGui::SetNextWindowBgAlpha(m_Overlay::bTransparent ? 0.0f : 0.5f);

	if (m_bShowMenu && ImGui::Begin("Overlay", nullptr, window_flags))
	{
		CVector pos{0,0,0};
		if (player)
		{
			pos = player->GetPosition();
		}

		size_t game_ms = CTimer::m_snTimeInMilliseconds;

		if (game_ms - m_Overlay::mLastInterval > m_Overlay::mInterval)
		{
			m_Overlay::fCpuUsage = static_cast<float>(Util::GetCurrentCPUUsage());

			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);
			int mUsedRam = static_cast<int>((memInfo.ullTotalPhys - memInfo.ullAvailPhys) * 1e-6);
			m_Overlay::fMemUsage = 100.0f * (static_cast<float>(mUsedRam) / static_cast<float>(m_Overlay::mTotalRam));
			m_Overlay::mFPS = static_cast<size_t>(CTimer::game_FPS);

			m_Overlay::mLastInterval = game_ms;
		}

		if (m_Overlay::bCoord)
			ImGui::Text("Coord: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);

		if (m_Overlay::bCpuUsage)
			ImGui::Text("CPU usage: %.2f%%", m_Overlay::fCpuUsage);

		if (m_Overlay::bFPS)
			ImGui::Text("Frames: %d", m_Overlay::mFPS);

		if (m_Overlay::bLocName)
			ImGui::Text("Location: %s", Util::GetLocationName(&pos).c_str());

		if (m_Overlay::bMemUsage)
			ImGui::Text("RAM usage: %.2f%%", m_Overlay::fMemUsage);

		if (player && player->m_pVehicle && player->m_pVehicle->IsDriver(player))
		{
			if (m_Overlay::bVehHealth)
				ImGui::Text("Veh Health: %.f", player->m_pVehicle->m_fHealth);

			if (m_Overlay::bVehSpeed)
			{
				int speed = player->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f; // 02E3 - GET_CAR_SPEED
				ImGui::Text("Veh Speed: %d", speed); 
			}
		}

		ImVec2 windowPos = ImGui::GetWindowPos();
		m_Overlay::fPosX = windowPos.x;
		m_Overlay::fPosY = windowPos.y;

		ImGui::End();
	}
}

void Menu::DrawShortcutsWindow()
{
	int resX = static_cast<int>(screen::GetScreenWidth());
	int resY = static_cast<int>(screen::GetScreenHeight());

	ImGui::SetNextWindowPos(ImVec2(0, resY - 40), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(resX, 40));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration + ImGuiWindowFlags_AlwaysAutoResize +
		ImGuiWindowFlags_NoSavedSettings
		+ ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Shortcuts window", nullptr, flags))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, resY / 130));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));

		ImGui::SetNextItemWidth(resX);
		ImGui::SetKeyboardFocusHere(-1);

		if (ImGui::InputTextWithHint("##TEXTFIELD", "Enter command", m_Commands::m_nInputBuffer, INPUT_BUFFER_SIZE,
		                             ImGuiInputTextFlags_EnterReturnsTrue))
		{
			ProcessCommands();
			m_Commands::m_bShowMenu = false;
			strcpy(m_Commands::m_nInputBuffer, "");
		}

		ImGui::PopStyleVar(2);
		ImGui::End();
	}
}

void Menu::ProcessCommands()
{
	std::stringstream ss(m_Commands::m_nInputBuffer);

	std::string command;
	ss >> command;

	if (command == "hp")
	{
		try
		{
			std::string temp;
			ss >> temp;
			FindPlayerPed()->m_fHealth = std::stof(temp);
		}
		catch (...)
		{
			CHud::SetHelpMessage("Invalid value", false, false, false);
		}
	}

	if (command == "time")
	{
		try
		{
			std::string temp;
			ss >> temp;
			CClock::ms_nGameClockHours = std::stoi(temp);

			ss >> temp;
			CClock::ms_nGameClockMinutes = std::stoi(temp);
		}
		catch (...)
		{
			CHud::SetHelpMessage("Invalid value", false, false, false);
		}
	}

	if (command == "tp")
	{
		try
		{
			CVector pos;
			std::string temp;

			ss >> temp;
			pos.x = std::stof(temp);

			ss >> temp;
			pos.y = std::stof(temp);

			ss >> temp;
			pos.z = std::stof(temp);

			Teleport::TeleportPlayer(false, pos, 0);
		}
		catch (...)
		{
			CHud::SetHelpMessage("Invalid location", false, false, false);
		}
	}

	if (command == "wep")
	{
		std::string wep_name;
		ss >> wep_name;

		if (wep_name == "jetpack")
		{
			std::string weapon = "-1";
			Weapon::GiveWeaponToPlayer(weapon);
			CHud::SetHelpMessage("Weapon given", false, false, false);
		}
		else
		{
			eWeaponType weapon = CWeaponInfo::FindWeaponType((char*)wep_name.c_str());
			std::string weapon_name = std::to_string(weapon);
			CWeaponInfo* pweaponinfo = CWeaponInfo::GetWeaponInfo(weapon, 1);

			if (wep_name != "" && pweaponinfo->m_nModelId1 != -1)
			{
				Weapon::GiveWeaponToPlayer(weapon_name);
				CHud::SetHelpMessage("Weapon given", false, false, false);
			}
			else
				CHud::SetHelpMessage("Invalid command", false, false, false);
		}

		return;
	}
	if (command == "veh")
	{
		std::string veh_name;
		ss >> veh_name;

		int model = Vehicle::GetModelFromName(veh_name.c_str());
		if (model != 0)
		{
			std::string smodel = std::to_string(model);
			Vehicle::SpawnVehicle(smodel);
			CHud::SetHelpMessage("Vehicle spawned", false, false, false);
		}
		else
			CHud::SetHelpMessage("Invalid command", false, false, false);
	}
}

void Menu::Draw()
{
	if (ImGui::BeginTabBar("Menu", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Overlay"))
		{
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::SameLine();
			if (Ui::ListBox("Overlay", m_Overlay::posNames, (int&)m_Overlay::mSelectedPos))
				config.SetValue("overlay.selected_pos", m_Overlay::mSelectedPos);

			ImGui::Spacing();

			ImGui::Columns(2, nullptr, false);
			if (ImGui::Checkbox("No background", &m_Overlay::bTransparent))
				config.SetValue("overlay.transparent", m_Overlay::bTransparent);

			if (ImGui::Checkbox("Show coordinates", &m_Overlay::bCoord))
				config.SetValue("overlay.coord", m_Overlay::bCoord);

			if (ImGui::Checkbox("Show CPU usage", &m_Overlay::bCpuUsage))
				config.SetValue("overlay.cpu_usage", m_Overlay::bCpuUsage);

			if (ImGui::Checkbox("Show FPS", &m_Overlay::bFPS))
				config.SetValue("overlay.fps", m_Overlay::bFPS);

			ImGui::NextColumn();

			if (ImGui::Checkbox("Show location", &m_Overlay::bLocName))
				config.SetValue("overlay.loc_name", m_Overlay::bLocName);

			if (ImGui::Checkbox("Show RAM usage", &m_Overlay::bMemUsage))
				config.SetValue("overlay.mem_usage", m_Overlay::bMemUsage);

			if (ImGui::Checkbox("Show veh health", &m_Overlay::bVehHealth))
				config.SetValue("overlay.veh_health", m_Overlay::bVehHealth);

			if (ImGui::Checkbox("Show veh speed", &m_Overlay::bVehSpeed))
				config.SetValue("overlay.veh_speed", m_Overlay::bVehSpeed);

			ImGui::Columns(1);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Hotkeys"))
		{
			ImGui::Spacing();
			ImGui::Text("Usage");
			Ui::ShowTooltip("Left-click selects hotkey.\nLeft clicking outside deselects."
				"\nRight click disables hotkey.");
			ImGui::Spacing();
			ImGui::BeginChild("Hotkeys");
			if (Ui::HotKey("Open/ close cheat menu", m_HotKeys::menuOpen))
			{
				config.SetValue("hotkey.menu_open.key1", m_HotKeys::menuOpen.m_key1);
				config.SetValue("hotkey.menu_open.key2", m_HotKeys::menuOpen.m_key2);
			}
			if (Ui::HotKey("Open/ close command window", m_HotKeys::commandWindow))
			{
				config.SetValue("hotkey.command_window.key1", m_HotKeys::commandWindow.m_key1);
				config.SetValue("hotkey.command_window.key2", m_HotKeys::commandWindow.m_key2);
			}

			ImGui::Dummy(ImVec2(0, 10));

			if (Ui::HotKey("Activate aim skin changer", m_HotKeys::aimSkinChanger))
			{
				config.SetValue("hotkey.aim_skin_changer.key1", m_HotKeys::aimSkinChanger.m_key1);
				config.SetValue("hotkey.aim_skin_changer.key2", m_HotKeys::aimSkinChanger.m_key2);
			}
			if (Ui::HotKey("Freecam", m_HotKeys::freeCam))
			{
				config.SetValue("hotkey.freecam.key1", m_HotKeys::freeCam.m_key1);
				config.SetValue("hotkey.freecam.key2", m_HotKeys::freeCam.m_key2);
			}
			if (Ui::HotKey("Take quick screenshot", m_HotKeys::quickSceenShot))
			{
				config.SetValue("hotkey.quick_screenshot.key1", m_HotKeys::quickSceenShot.m_key1);
				config.SetValue("hotkey.quick_screenshot.key2", m_HotKeys::quickSceenShot.m_key2);
			}
			if (Ui::HotKey("Toggle quick teleport", m_HotKeys::quickTeleport))
			{
				config.SetValue("hotkey.quick_tp.key1", m_HotKeys::quickTeleport.m_key1);
				config.SetValue("hotkey.quick_tp.key2", m_HotKeys::quickTeleport.m_key2);
			}

			ImGui::Dummy(ImVec2(0, 10));

			if (Ui::HotKey("Fix current vehicle", m_HotKeys::fixVeh))
			{
				config.SetValue("hotkey.fix_veh.key1", m_HotKeys::fixVeh.m_key1);
				config.SetValue("hotkey.fix_veh.key2", m_HotKeys::fixVeh.m_key2);
			}

			if (Ui::HotKey("Flip current vehicle", m_HotKeys::flipVeh))
			{
				config.SetValue("hotkey.flip_veh.key1", m_HotKeys::flipVeh.m_key1);
				config.SetValue("hotkey.flip_veh.key2", m_HotKeys::flipVeh.m_key2);
			}

			if (Ui::HotKey("Toggle god mode", m_HotKeys::godMode))
			{
				config.SetValue("hotkey.god_mode.key1", m_HotKeys::godMode.m_key1);
				config.SetValue("hotkey.god_mode.key2", m_HotKeys::godMode.m_key2);
			}

			if (Ui::HotKey("Toggle veh engine", m_HotKeys::vehEngine))
			{
				config.SetValue("hotkey.veh_engine.key1", m_HotKeys::vehEngine.m_key1);
				config.SetValue("hotkey.veh_engine.key2", m_HotKeys::vehEngine.m_key2);
			}

			if (Ui::HotKey("Vehicle instant start", m_HotKeys::vehInstantStart))
			{
				config.SetValue("hotkey.veh_instant_start.key1", m_HotKeys::vehInstantStart.m_key1);
				config.SetValue("hotkey.veh_instant_start.key2", m_HotKeys::vehInstantStart.m_key2);
			}

			if (Ui::HotKey("Vehicle instant stop", m_HotKeys::vehInstantStop))
			{
				config.SetValue("hotkey.veh_instant_stop.key1", m_HotKeys::vehInstantStop.m_key1);
				config.SetValue("hotkey.veh_instant_stop.key2", m_HotKeys::vehInstantStop.m_key2);
			}

			ImGui::Dummy(ImVec2(0, 10));

			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Commands"))
		{
			if (ImGui::BeginChild("CommandsChild"))
			{
				ImGui::TextWrapped(
					std::string(
						"Open or close command window using " + Ui::GetHotKeyNameString(m_HotKeys::commandWindow)).
					c_str());
				ImGui::Spacing();
				if (ImGui::CollapsingHeader("Set health"))
				{
					ImGui::Spacing();
					ImGui::TextWrapped("Set player health.\nExample: hp (health).");
					ImGui::Spacing();
					ImGui::Separator();
				}
				if (ImGui::CollapsingHeader("Set time"))
				{
					ImGui::Spacing();
					ImGui::TextWrapped("Set current game time.\nExample: time (hour) (minute).\n");
					ImGui::TextWrapped("Writing something like 'time 12' would be interpreted as 'time 12 12'");
					ImGui::Spacing();
					ImGui::Separator();
				}
				if (ImGui::CollapsingHeader("Quick vehicle spawner"))
				{
					ImGui::Spacing();
					ImGui::TextWrapped("Spawn vehicles by typing their model names.\nExample: veh (veh_name)");
					ImGui::Spacing();
					ImGui::Separator();
				}
				if (ImGui::CollapsingHeader("Quick weapon spawner"))
				{
					ImGui::Spacing();
					ImGui::TextWrapped("Spawn weapons by typing their model names.\nExample: wep (wep_name)");
					ImGui::Spacing();
					ImGui::Separator();
				}

				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("About"))
		{
			ImGui::Spacing();

			if (ImGui::Button("Check update", ImVec2(Ui::GetSize(3))))
			{
				if (Updater::m_State == UPDATER_IDLE)
					Updater::m_State = UPDATER_CHECKING;
			}

			ImGui::SameLine();

			if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(3))))
				ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);

			ImGui::SameLine();

			if (ImGui::Button("GitHub repo", ImVec2(Ui::GetSize(3))))
				ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);

			ImGui::Spacing();

			if (ImGui::BeginChild("AboutChild"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::Text("Author: Grinch_");

				ImGui::Text("Version: %s",MENU_VERSION);

				ImGui::NextColumn();
				ImGui::Text("ImGui: %s", ImGui::GetVersion());
				ImGui::Text("Build: %s", BUILD_NUMBER);

				ImGui::Columns(1);

				ImGui::Dummy(ImVec2(0, 10));
				ImGui::TextWrapped("If you find bugs or have suggestions, let me know on discord.");
				ImGui::Dummy(ImVec2(0, 10));
				ImGui::TextWrapped("Thanks to Junior-Djjr");
				ImGui::Dummy(ImVec2(0, 10));
				Ui::CenterdText("Copyright GPLv3 2019-2021 Grinch_");

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
