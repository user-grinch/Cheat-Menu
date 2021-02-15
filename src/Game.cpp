#include "pch.h"
#include "Menu.h"
#include "Game.h"
#include "Ui.h"
#include "Util.h"
#include "CIplStore.h"

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

bool Game::freecam::init_done = false;
bool Game::freecam::enable = false;
float Game::freecam::speed = 0.08f;
float Game::freecam::tmouseX = 0;
float Game::freecam::tmouseY = 0;
float Game::freecam::mouseX = 0;
float Game::freecam::mouseY = 0;
int Game::freecam::hped = -1;
float Game::freecam::fov = -1;
CPed *Game::freecam::ped = nullptr;

bool Game::hard_mode::state = false;
float Game::hard_mode::prev_armour = 0.0f;
float Game::hard_mode::prev_health = 0.0f;
float Game::hard_mode::prev_max_health = 0.0f;
float Game::hard_mode::prev_stamina = 0.0f;

bool Game::disable_cheats = false;
bool Game::disable_replay = false;
bool Game::forbidden_area_wl = true;
bool Game::freeze_mission_timer = false;
bool Game::freeze_time = false;
bool Game::keep_stuff = false;
bool Game::solid_water = false;
bool Game::ss_shortcut = false;
bool Game::sync_time = false;

uint Game::sync_time_timer = 0;

uint Game::solid_water_object = 0;

CJson Game::random_cheats::name_json = CJson("cheat name");

static bool mission_warning_shown = false;

// Thanks to aap
void RealTimeClock(void)
{	
	static int lastday;
	time_t tmp = time(NULL);
	struct tm *now = localtime(&tmp);
	
	if(now->tm_yday != lastday)
		CStats::SetStatValue(0x86, CStats::GetStatValue(0x86) + 1.0f);

	lastday = now->tm_yday;
	CClock::ms_nGameClockMonth = now->tm_mon+1;
	CClock::ms_nGameClockDays = now->tm_mday;
	CClock::CurrentDay = now->tm_wday+1;
	CClock::ms_nGameClockHours = now->tm_hour;
	CClock::ms_nGameClockMinutes = now->tm_min;
	CClock::ms_nGameClockSeconds = now->tm_sec;
}

Game::Game()
{
	Events::initGameEvent += []
	{
		json.LoadData(search_categories, selected_item);
		stat::json.LoadData(stat::search_categories, stat::selected_item);
		freecam::fov = TheCamera.FindCamFOV();

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
			if (Ui::HotKeyPressed(Menu::hotkeys::quick_ss))
			{
				Command<Commands::TAKE_PHOTO>();
				CHud::SetHelpMessage("Screenshot taken", false, false, false);
			}
		}

		if (hard_mode::state)
		{
			if (player->m_fHealth > 50.0f)
				player->m_fHealth = 50.0f;

			player->m_fArmour = 0.0f;
			CStats::SetStatValue(STAT_MAX_HEALTH, 350.0f);
			CStats::SetStatValue(STAT_STAMINA, 0.0f);
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
			int id = Random(0,91);

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
		
		if (Ui::HotKeyPressed(Menu::hotkeys::freecam))
		{
			if (freecam::enable)
			{
				freecam::enable = false;
				ClearFreecamStuff();
			}
			else freecam::enable = true;
		}
		if (freecam::enable)
			FreeCam();
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

void Game::FreeCam()
{
	int delta_speed = freecam::speed * (CTimer::m_snTimeInMillisecondsNonClipped - CTimer::m_snPreviousTimeInMillisecondsNonClipped);

	if (!freecam::init_done)
	{
		CPlayerPed *player = FindPlayerPed(-1);
		Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, true);
		CHud::bScriptDontDisplayRadar = true;
		CHud::m_Wants_To_Draw_Hud = false;
		CVector player_pos = player->GetPosition();
		CPad::GetPad(0)->DisablePlayerControls = true;

		Command<Commands::CREATE_RANDOM_CHAR>(player_pos.x,player_pos.y,player_pos.z, &freecam::hped);
		freecam::ped = CPools::GetPed(freecam::hped);
		freecam::ped->m_bIsVisible = false;
		
		Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(freecam::hped,true);
		Command<Commands::SET_CHAR_COLLISION>(freecam::hped, false);
		Command<Commands::SET_LOAD_COLLISION_FOR_CHAR_FLAG>(freecam::hped, false);

		freecam::tmouseX = player->GetHeading() + 89.6f;
		freecam::tmouseY = 0;

		freecam::init_done = true;
		player_pos.z -= 20;
		freecam::ped->SetPosn(player_pos);

		TheCamera.LerpFOV(TheCamera.FindCamFOV(),freecam::fov,1000,true);
		Command<Commands::CAMERA_PERSIST_FOV>(true);
	}

	CVector pos = freecam::ped->GetPosition();

	Command<Commands::GET_PC_MOUSE_MOVEMENT>(&freecam::mouseX, &freecam::mouseY);
	freecam::tmouseX = freecam::tmouseX - freecam::mouseX/250;
	freecam::tmouseY = freecam::tmouseY + freecam::mouseY/3;
	
	if (freecam::tmouseY > 150)
		freecam::tmouseY = 150;

	if (freecam::tmouseY < -150)
		freecam::tmouseY = -150;

	if (Ui::HotKeyPressed(Menu::hotkeys::free_cam_tp_player))
	{
		CPlayerPed *player = FindPlayerPed(-1);
		CVector pos = freecam::ped->GetPosition();
		CEntity* player_entity = FindPlayerEntity(-1);
		pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, 0, &player_entity) + 0.5f;
		Command<Commands::SET_CHAR_COORDINATES>(CPools::GetPedRef(player),pos.x,pos.y,pos.z);

		// disble them again cause they get enabled
		CHud::bScriptDontDisplayRadar = true;
		CHud::m_Wants_To_Draw_Hud = false;
		CHud::SetHelpMessage((char*)"Player telported",false,false,false);
	}
	
	if (KeyPressed(VK_RCONTROL))
		delta_speed /= 2;

	if (KeyPressed(VK_RSHIFT))
		delta_speed *= 2;

	if (KeyPressed(VK_KEY_I) || KeyPressed(VK_KEY_K))
	{
		if (KeyPressed(VK_KEY_K))
			delta_speed *= -1;

		float angle;
		Command<Commands::GET_CHAR_HEADING>(freecam::hped,&angle);
		pos.x += delta_speed * cos(angle * 3.14159f/180.0f);
		pos.y += delta_speed * sin(angle * 3.14159f/180.0f);
		pos.z += delta_speed* 2 * sin(freecam::tmouseY/3* 3.14159f/180.0f);
	}

	if (KeyPressed(VK_KEY_J) || KeyPressed(VK_KEY_L))
	{
		if (KeyPressed(VK_KEY_J))
			delta_speed *= -1;

		float angle;
		Command<Commands::GET_CHAR_HEADING>(freecam::hped,&angle);
		angle -= 90;
		
		pos.x += delta_speed * cos(angle * 3.14159f/180.0f);
		pos.y += delta_speed * sin(angle * 3.14159f/180.0f);
	}

	if ( CPad::NewMouseControllerState.wheelUp)
	{
		if (freecam::fov > 10.0f)
			freecam::fov -= 2.0f * delta_speed;

		TheCamera.LerpFOV(TheCamera.FindCamFOV(),freecam::fov,250,true);
		Command<Commands::CAMERA_PERSIST_FOV>(true);
	}

	if ( CPad::NewMouseControllerState.wheelDown)
	{
		if (freecam::fov < 115.0f)
			freecam::fov += 2.0f * delta_speed;
		
		TheCamera.LerpFOV(TheCamera.FindCamFOV(),freecam::fov,250,true);
		Command<Commands::CAMERA_PERSIST_FOV>(true);
	}

	freecam::ped->SetHeading(freecam::tmouseX);
	Command<Commands::ATTACH_CAMERA_TO_CHAR>(freecam::hped,0.0,0.0,20.0,90.0,180,freecam::tmouseY,0.0,2);
	freecam::ped->SetPosn(pos);
	CIplStore::AddIplsNeededAtPosn(pos);
}

void Game::ClearFreecamStuff()
{
	freecam::init_done = false;
	Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, false);
	CHud::bScriptDontDisplayRadar = false;
	CHud::m_Wants_To_Draw_Hud = true;
	CPad::GetPad(0)->DisablePlayerControls = false;

	Command<Commands::DELETE_CHAR>(freecam::hped);
	freecam::ped = nullptr;
	Command<Commands::CAMERA_PERSIST_FOV>(false);
	Command<Commands::RESTORE_CAMERA_JUMPCUT>();
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

			if (Ui::CheckboxWithHint("Forbidden area wl", &forbidden_area_wl, "Wanted levels that appears outside \
of LS without completing missions"))
			{
				if (forbidden_area_wl)
					patch::Set<BYTE>(0x441770, 0x83, false);
				else
					patch::Set<BYTE>(0x441770, 0xC3, false);
			}

			Ui::CheckboxAddress("Free pay n spray", 0x96C009);

			if (ImGui::Checkbox("Freeze misson timer", &freeze_mission_timer))
				Command<Commands::FREEZE_ONSCREEN_TIMER>(freeze_mission_timer);

			ImGui::NextColumn();

			if (Ui::CheckboxWithHint("Hard mode", &hard_mode::state, "Makes the game more challanging to play. \n\
Lowers armour, health, stamina etc."))
			{
				CPlayerPed *player = FindPlayerPed();

				if (hard_mode::state)
				{
					hard_mode::prev_armour = player->m_fArmour;
					hard_mode::prev_health = player->m_fHealth;
					hard_mode::prev_max_health = CStats::GetStatValue(STAT_MAX_HEALTH);
					hard_mode::prev_stamina = CStats::GetStatValue(STAT_STAMINA);
					player->m_fHealth = 50.0f;
				}
				else
				{
					player->m_fArmour = hard_mode::prev_armour;
					CStats::SetStatValue(STAT_STAMINA,hard_mode::prev_stamina);
					CStats::SetStatValue(STAT_MAX_HEALTH, hard_mode::prev_max_health);
					player->m_fHealth = hard_mode::prev_health;
					CWeaponInfo::LoadWeaponData();
				}
			}

			if (Ui::CheckboxWithHint("Keep stuff", &keep_stuff, "Keep stuff after arrest/death"))
			{
				Command<Commands::SWITCH_ARREST_PENALTIES>(keep_stuff);
				Command<Commands::SWITCH_DEATH_PENALTIES>(keep_stuff);
			}
			Ui::CheckboxWithHint("Screenshot shortcut", &ss_shortcut, (("Take screenshot using ") + Ui::GetHotKeyNameString(Menu::hotkeys::quick_ss)
			+ "\nSaved inside 'GTA San Andreas User Files\\Gallery'").c_str());
			if (Ui::CheckboxWithHint("Solid water", &solid_water, "Player can walk on water"))
			{
				if (!solid_water && solid_water_object != 0)
				{
					Command<Commands::DELETE_OBJECT>(solid_water_object);
					solid_water_object = 0;
				}
			}
			if (ImGui::Checkbox("Sync system time", &sync_time))
			{
				if (sync_time)
					patch::RedirectCall(0x53BFBD, &RealTimeClock);
				else
					patch::RedirectCall(0x53BFBD, &CClock::Update);
			}

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{
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
			if (ImGui::CollapsingHeader("Free cam"))
			{
				if (Ui::CheckboxWithHint("Enable", &freecam::enable, "Forward: I\tBackward: K\
\nLeft: J\t\t  Right: L\n\nSlower: RCtrl\tFaster: RShift\n\nZoom: Mouse wheel"))
				{
					if (!freecam::enable)
						ClearFreecamStuff();
				}
				ImGui::Spacing();
					
				if (ImGui::SliderFloat("Field of view", &freecam::fov, 5.0f, 120.0f))
				{
					TheCamera.LerpFOV(TheCamera.FindCamFOV(),freecam::fov,250.0f,true);
					Command<Commands::CAMERA_PERSIST_FOV>(true);
				}
				ImGui::SliderFloat("Movement Speed", &freecam::speed, 0.0f, 0.5f);
				ImGui::Spacing();
				ImGui::TextWrapped("Press Enter to teleport player to camera location");
				ImGui::Spacing();
				ImGui::Separator();
			}
			Ui::EditReference("Game speed", CTimer::ms_fTimeScale,1, 1, 10);
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

			if (!mission_warning_shown)
			{
				ImGui::TextWrapped("Mission selector might cause unintended changes to your game. \
It's recommanded not to save your game after using this. Use it at your own risk!");
				ImGui::Spacing();
				if (ImGui::Button("Show mission selector", ImVec2(Ui::GetSize())))\
					mission_warning_shown = true;
			}
			else
			{
				if (ImGui::Button("Fail current mission", ImVec2(Ui::GetSize())))
				{
					if (!CCutsceneMgr::ms_running)
						Command<Commands::FAIL_CURRENT_MISSION>();
				}

				ImGui::Spacing();

				Ui::DrawJSON(json, search_categories, selected_item, filter, SetPlayerMission, nullptr);
			}
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
