#include "pch.h"
#include "Game.h"
#include "Menu.h"

ImGuiTextFilter Game::filter = "";
std::vector<std::string> Game::search_categories;
std::string Game::selected_item = "All";
std::vector<std::string> Game::day_names{ "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };
char Game::save_game_name[22] = "";

CJson Game::json = CJson("mission");

CJson Game::stat::json = CJson("stat");
std::vector<std::string>  Game::stat::search_categories;
std::string Game::stat::selected_item = "All";
ImGuiTextFilter Game::stat::filter = "";

bool Game::random_cheats::enable = false;
int Game::random_cheats::enable_wait_time = 10;
uint Game::random_cheats::timer = 0;
std::string Game::random_cheats::enabled_cheats[92][2];

bool Game::airbreak::init_done = false;
bool Game::airbreak::enable = false;
float Game::airbreak::speed = 0.4f;
float Game::airbreak::tmouseX = 0;
float Game::airbreak::tmouseY = 0;

bool Game::disable_cheats = false;
bool Game::disable_replay = false;
bool Game::forbidden_area_wl = true;
bool Game::freeze_mission_timer = false;
bool Game::freeze_time = false;
bool Game::keep_stuff = false;
bool Game::solid_water = false;
bool Game::ss_shortcut = false;
bool Game::sync_time = false;

uint Game::ss_shotcut_timer = 0;
uint Game::sync_time_timer = 0;

uint Game::solid_water_object = 0;

CJson Game::random_cheats::name_json = CJson("cheat name");

Game::Game()
{
	Events::initGameEvent += []
	{
		json.LoadJsonData(search_categories, selected_item);
		stat::json.LoadJsonData(stat::search_categories, stat::selected_item);

		// Generate enabled cheats vector
		for (auto element : random_cheats::name_json.data.items())
		{
			/*
			[
				cheat_id = [ cheat_name, state (true/false) ]
			]
			*/
			random_cheats::enabled_cheats[std::stoi(element.key())][0] = element.value().get<std::string>();
			random_cheats::enabled_cheats[std::stoi(element.key())][1] = "true";
		}	
	};

	Events::processScriptsEvent += []
	{
		uint timer = CTimer::m_snTimeInMilliseconds;
		static CPlayerPed *player = FindPlayerPed();
		static int hplayer = CPools::GetPedRef(player);

		if (ss_shortcut)
		{
			if (Ui::HotKeyPressed(Menu::hotkey::quick_ss) && timer - ss_shotcut_timer > 1000)
			{
				Command<Commands::TAKE_PHOTO>();
				CHud::SetHelpMessage("Screenshot taken", false, false, false);
				ss_shotcut_timer = timer;
			}
		}

		if (solid_water)
		{
			CVector pos = player->GetPosition();

			float water_height = 0;
			Command<Commands::GET_WATER_HEIGHT_AT_COORDS>(pos.x, pos.y, false, &water_height);

			if (!Command<Commands::IS_CHAR_IN_ANY_BOAT>(hplayer) && water_height != -1000.0f && pos.z > (water_height+1))
			{
				if (solid_water_object == 0)
				{
					Command<Commands::CREATE_OBJECT>(3095, pos.x, pos.y, water_height, &solid_water_object);
					Command<Commands::SET_OBJECT_VISIBLE>(solid_water_object, false);
				}
				else
					Command<Commands::SET_OBJECT_COORDINATES>(solid_water_object, pos.x, pos.y, water_height);
			}
			else
			{
				if (solid_water_object != 0)
				{
					Command<Commands::DELETE_OBJECT>(solid_water_object);
					solid_water_object = 0;
				}
			}
		}

		// improve this later
		if (sync_time && timer - sync_time_timer > 50)
		{
			std::time_t t = std::time(0); 
			std::tm* now = std::localtime(&t);

			CClock::ms_nGameClockHours = now->tm_hour;
			CClock::ms_nGameClockMinutes = now->tm_min;
			
			sync_time_timer = timer;
		}

		if (random_cheats::enable 
		&& (timer - random_cheats::timer) >  (uint(random_cheats::enable_wait_time)*1000))
		{
			int id = cheat_id(gen);

			for (int i = 0; i < 92; i++)
			{
				if (i == id)
				{
					if (random_cheats::enabled_cheats[i][1] == "true")
					{
						((void(*)(int))0x00438370)(id); // cheatEnableLegimate(int CheatID)
						CHud::SetHelpMessage(random_cheats::enabled_cheats[i][0].c_str(), false, false, false);
						random_cheats::timer = timer;
					}
					break;
				}
			}
		}

		if (airbreak::enable)
		{
			AirbreakMode(player,hplayer);
		}
	};
}

Game::~Game()
{
}

void SetPlayerMission(std::string& rootkey, std::string& name, std::string& id)
{
	CPlayerPed *player = FindPlayerPed();
	uint hplayer = CPools::GetPedRef(player);
	int interior = 0;

	Command<0x09E8>(hplayer,&interior);
	if ( Util::IsOnMission() && interior == 0)
	{
		player->SetWantedLevel(0);
		Command<Commands::LOAD_AND_LAUNCH_MISSION_INTERNAL>(std::stoi(id));
	}
	else CHud::SetHelpMessage("Can't start mission now", false, false, false);
		
}

void Game::AirbreakMode(CPlayerPed* player, int hplayer)
{
	CVector pos = player->GetPosition();

	if (!airbreak::init_done)
	{
		Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, true);
		Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(hplayer,true);
		Command<Commands::SET_CHAR_COLLISION>(hplayer, false);
		Command<Commands::SET_LOAD_COLLISION_FOR_CHAR_FLAG>(hplayer, false);
		player->m_nPedFlags.bDontRender = true;

		CHud::bScriptDontDisplayRadar = true;
		CHud::m_Wants_To_Draw_Hud = false;

		airbreak::tmouseX = TheCamera.GetHeading() + (90.0f * 3.1416f / 180.0f);
		airbreak::tmouseY = 0;
		airbreak::init_done = true;
	}

	float mul = 1.0f;
	
	airbreak::tmouseX -= (CPad::NewMouseControllerState.X / 6.0f);
	airbreak::tmouseY += (CPad::NewMouseControllerState.Y / 3.0f);

	airbreak::tmouseY = (airbreak::tmouseY > 17.1887f) ? 17.1887f : airbreak::tmouseY;
	airbreak::tmouseY = (airbreak::tmouseY < -17.1887f) ? -17.1887f : airbreak::tmouseY;

	TheCamera.m_fOrientation = airbreak::tmouseY * (3.1416 / 180);

	if (KeyPressed(VK_RCONTROL))
		mul /= 2;

	if (KeyPressed(VK_RSHIFT))
		mul *= 2;

	if (KeyPressed(VK_KEY_I) || KeyPressed(VK_KEY_K))
	{
		if (KeyPressed(VK_KEY_K))
			mul *= -1;

		float angle = TheCamera.GetHeading() + (90.0f * 3.1416f / 180.0f);

		pos.x += airbreak::speed * cos(angle) * mul;
		pos.y += airbreak::speed * sin(angle) * mul;
		pos.z += airbreak::speed * sin(TheCamera.m_fOrientation*2) * mul;
	}

	if (KeyPressed(VK_KEY_J) || KeyPressed(VK_KEY_L))
	{
		if (KeyPressed(VK_KEY_J))
			mul *= -1;
		
		float angle = TheCamera.GetHeading() + (90.0f * 3.1416f / 180.0f);

		pos.x += airbreak::speed * cos(angle) * mul;
		pos.y += airbreak::speed * sin(angle) * mul;
	}

	player->SetPosn(pos);
}

void Game::Main()
{
	ImGui::Spacing();
	static CPlayerPed *player = FindPlayerPed();
	static int hplayer = CPools::GetPedRef(player);

	if (ImGui::BeginTabBar("Game", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::Columns(2, 0, false);
			if (ImGui::Checkbox("Disable cheats", &disable_cheats))
			{
				if (disable_cheats)
				{
					patch::Set<BYTE>(0x4384D0, 0xE9, false);
					patch::SetInt(0x4384D1, 0xD0, false);
					patch::Nop(0x4384D5, 4, false);
				}
				else
				{
					patch::Set<BYTE>(0x4384D0, 0x83, false);
					patch::SetInt(0x4384D1, -0x7DF0F908, false);
					patch::SetInt(0x4384D5, 0xCC, false);
				}
			}
			if (ImGui::Checkbox("Disable F1 & F3 replay", &disable_replay))
			{
				if (disable_replay)
					patch::SetInt(0x460500, 0xC3, false);
				else
					patch::SetInt(0x460500, 0xBD844BB, false);
			}

			Ui::CheckboxAddress("Faster clock", 0x96913B);

			if (Ui::CheckboxWithHint("Forbidden area wl", &forbidden_area_wl, "Wanted levels that appears outside\
of LS without completing missions"))
			{
				if (forbidden_area_wl)
					patch::Set<BYTE>(0x441770, 0x83, false);
				else
					patch::Set<BYTE>(0x441770, 0xC3, false);
			}

			Ui::CheckboxAddress("Free pay n spray", 0x96C009);

			ImGui::NextColumn();

			if (ImGui::Checkbox("Freeze misson timer", &freeze_mission_timer))
				Command<Commands::FREEZE_ONSCREEN_TIMER>(freeze_mission_timer);

			if (Ui::CheckboxWithHint("Keep stuff", &keep_stuff, "Keep stuff after arrest/death"))
			{
				Command<Commands::SWITCH_ARREST_PENALTIES>(keep_stuff);
				Command<Commands::SWITCH_DEATH_PENALTIES>(keep_stuff);
			}
			Ui::CheckboxWithHint("Screenshot shortcut", &ss_shortcut, (("Take screenshot using ") + Ui::GetHotKeyNameString(Menu::hotkey::quick_ss)).c_str());
			if (Ui::CheckboxWithHint("Solid water", &solid_water, "Player can walk on water"))
			{
				if (!solid_water && solid_water_object != 0)
				{
					Command<Commands::DELETE_OBJECT>(solid_water_object);
					solid_water_object = 0;
				}
			}
			ImGui::Checkbox("Sync system time", &sync_time);

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{
			if (ImGui::CollapsingHeader("Airbreak mode"))
			{
				if (Ui::CheckboxWithHint("Enable", &airbreak::enable, "Forward: I\tBackward: K\
\nLeft: J\t\t  Right: L\n\nSlower: RCtrl\tFaster: RShift"))
				{
					if (!airbreak::enable)
					{
						airbreak::init_done = false;
						Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, false);
						Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(hplayer, false);
						Command<Commands::SET_CHAR_COLLISION>(hplayer, true);
						Command<Commands::SET_LOAD_COLLISION_FOR_CHAR_FLAG>(hplayer, true);
						player->m_nPedFlags.bDontRender = false;

						CHud::bScriptDontDisplayRadar = false;
						CHud::m_Wants_To_Draw_Hud = true;

						CVector pos = player->GetPosition();
						CEntity* player_entity = FindPlayerEntity(-1);
						pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, 0, &player_entity) + 0.5f;
						player->SetPosn(pos);
						Command<Commands::RESTORE_CAMERA_JUMPCUT>();
					}
				}
				ImGui::Spacing();

				ImGui::SliderFloat("Movement Speed", &airbreak::speed, 0.0, 5.0);

				ImGui::Spacing();
				ImGui::Separator();
			}
			if (ImGui::CollapsingHeader("Current day"))
			{
				int day = CClock::CurrentDay-1;
				if (Ui::ListBox("Select day", day_names, day))
					CClock::CurrentDay = day+1;
				ImGui::Spacing();
				ImGui::Separator();
			}
			Ui::EditAddress<int>("Days passed", 0xB79038, 0, 9999);
			Ui::EditReference("FPS limit", RsGlobal.frameLimit, 1, 30, 60);
			Ui::EditReference("Game speed", CTimer::ms_fTimeScale,0, 1, 10);
			Ui::EditFloat("Gravity", 0x863984, -1.0f, 0.008f, 1.0f);

			if (ImGui::CollapsingHeader("Set time"))
			{
				int hour = CClock::ms_nGameClockHours;
				int minute = CClock::ms_nGameClockMinutes;

				if (ImGui::InputInt("Hour", &hour))
				{
					if (hour < 0) hour = 23;
					if (hour > 23) hour = 0;
					CClock::ms_nGameClockHours = hour;
				}

				if (ImGui::InputInt("Minute", &minute))
				{
					if (minute < 0) minute = 59;
					if (minute > 59) minute = 0;
					CClock::ms_nGameClockMinutes = minute;
				}

				ImGui::Spacing();
				ImGui::Separator();
			}

			static std::vector<Ui::NamedMemory> themes { { "Beach", 0x969159 }, { "Country" ,0x96917D }, { "Fun house" ,0x969176 }, { "Ninja" ,0x96915C }};
			Ui::EditRadioButtonAddress("Themes", themes);
			
			if (ImGui::CollapsingHeader("Weather"))
			{
				typedef void func(void);
				if (ImGui::Button("Foggy",Ui::GetSize(3)))
					((func*)0x438F80)();

				ImGui::SameLine();
				if (ImGui::Button("Overcast", Ui::GetSize(3)))
					((func*)0x438F60)();

				ImGui::SameLine();
				if (ImGui::Button("Rainy", Ui::GetSize(3)))
					((func*)0x438F70)();

				if (ImGui::Button("Sandstorm", Ui::GetSize(3)))
					((func*)0x439590)();

				ImGui::SameLine();
				if (ImGui::Button("Thunderstorm", Ui::GetSize(3)))
					((func*)0x439570)();

				ImGui::SameLine();
				if (ImGui::Button("Very sunny", Ui::GetSize(3)))
					((func*)0x438F50)();

				ImGui::Spacing();
				ImGui::Separator();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Missions"))
		{
			ImGui::Spacing();

			if (ImGui::Button("Fail current mission", ImVec2(Ui::GetSize())))
			{
				if (!CCutsceneMgr::ms_running)
					Command<Commands::FAIL_CURRENT_MISSION>();
			}

			ImGui::Spacing();

			Ui::DrawJSON(json, search_categories, selected_item, filter, SetPlayerMission, nullptr);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Stats"))
		{
			// similat to Ui::DrawJSON()
			ImGui::Spacing();
			
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2 - 5);
			Ui::ListBoxStr("##Categories", stat::search_categories, stat::selected_item);
			ImGui::SameLine();
			Ui::FilterWithHint("##Filter", stat::filter, "Search");
			ImGui::PopItemWidth();

			ImGui::Spacing();

			ImGui::BeginChild("STATCHILD");
			for (auto root : stat::json.data.items())
			{
				if (root.key() == stat::selected_item || stat::selected_item == "All")
				{
					for (auto _data : root.value().items())
					{
						std::string name = _data.value().get<std::string>();
						if (stat::filter.PassFilter(name.c_str()))
							Ui::EditStat(name.c_str(), std::stoi(_data.key()));
					}
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Random cheats"))
		{
			ImGui::Spacing();
			ImGui::Checkbox("Enable", &random_cheats::enable);
			ImGui::Spacing();

			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);

			ImGui::SliderInt("Activate cheat timer", &random_cheats::enable_wait_time, 5, 60);
			Ui::ShowTooltip("Wait time after a cheat is activated.");

			ImGui::PopItemWidth();

			ImGui::TextWrapped("Select cheats");
			ImGui::Separator();
			if (ImGui::BeginChild("Cheats list"))
			{
				for (auto element : random_cheats::enabled_cheats)
				{
					bool selected = (element[1] == "true") ? true : false;

					if (ImGui::MenuItem(element[0].c_str(), 0, selected))
						element[1] = selected ? "false" : "true";
				}
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
