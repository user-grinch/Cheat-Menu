#include "pch.h"
#include "Ped.h"
#include "Ui.h"
#include "Util.h"

ImGuiTextFilter Ped::filter = "";
std::string Ped::selected_item = "All";
std::vector<std::string> Ped::search_categories;
std::vector<std::unique_ptr<TextureStructure>> Ped::peds_vec;
bool Ped::images_loaded = false;

CJson Ped::ped_json = CJson("ped");
CJson Ped::pedspecial_json = CJson("ped special");

std::vector<std::string> Ped::gang_names = { "Ballas", "Grove street families", "Los santos vagos", "San fierro rifa",
"Da nang boys", "Mafia", "Mountain cloud triad", "Varrio los aztecas", "Gang9", "Gang10" };

bool Ped::exgangwars_installed = false;
int Ped::ped_remove_radius = 5;

std::vector<CPed*> Ped::spawn_ped::list;
int Ped::spawn_ped::accuracy = 50;
int Ped::spawn_ped::health = 100;
int Ped::spawn_ped::selected_ped_type = 0;
bool Ped::spawn_ped::dont_move = false;
bool Ped::spawn_ped::ped_bleed = false;
int Ped::spawn_ped::weapon_id = 0;

std::vector<std::string> Ped::spawn_ped::ped_type = { "Civ male","Civ female","Cop","Ballas","Grove Street Families","Los Santos Vagos",
											"San Fierro Rifa","Da Nang Boys","Mafia","Mountain Cloud Triads","Varrio Los Aztecas",
											"Gang 9","Medic","Dealer","Criminal","Fireman","Prostitute" };
Ped::Ped()
{
	if (GetModuleHandle("ExGangWars.asi"))
		exgangwars_installed = true;

	Events::processScriptsEvent += []
	{
		if (!images_loaded)
		{
			Util::LoadTexturesInDirRecursive(PLUGIN_PATH((char*)"CheatMenu\\peds\\"), ".jpg", search_categories, peds_vec);
			images_loaded = true;
		}
	};
}

Ped::~Ped()
{
	for (CPed* ped : spawn_ped::list)
	{
		CWorld::Remove(ped);
		ped->Remove();
	}
}

void Ped::SpawnPed(std::string& model)
{
	if (spawn_ped::list.size() == SPAWN_PED_LIMIT)
	{
		CHud::SetHelpMessage("Max limit reached", false, false, false);
		return;
	}

	if (Ped::ped_json.data.contains(model))
	{
		CPlayerPed* player = FindPlayerPed();
		CVector pos = player->GetPosition();
		pos.y += 1;

		CPed* ped;
		int hplayer;

		if (Ped::pedspecial_json.data.contains(model))
		{
			std::string name;
			if (Ped::pedspecial_json.data.contains(model))
				name = Ped::pedspecial_json.data[model].get<std::string>().c_str();
			else
				name = model;

			CStreaming::RequestSpecialChar(1, name.c_str(), PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(true);

			Command <Commands::CREATE_CHAR>(spawn_ped::selected_ped_type + 4, 291, pos.x, pos.y, pos.z + 1, &hplayer);
			CStreaming::SetSpecialCharIsDeletable(291);
		}
		else
		{
			int imodel = std::stoi(model);
			CStreaming::RequestModel(imodel, eStreamingFlags::PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);

			Command <Commands::CREATE_CHAR>(spawn_ped::selected_ped_type + 4, imodel, pos.x, pos.y, pos.z + 1, &hplayer);
			CStreaming::SetModelIsDeletable(imodel);
		}

		ped = CPools::GetPed(hplayer);

		if (spawn_ped::dont_move)
			spawn_ped::list.push_back(ped);
		else
		{
			Command<Commands::MARK_CHAR_AS_NO_LONGER_NEEDED>(hplayer);
		}
		ped->m_nPedFlags.bPedIsBleeding = spawn_ped::ped_bleed;
		ped->m_nWeaponAccuracy = spawn_ped::accuracy;
		ped->m_fHealth = spawn_ped::health;

		if (spawn_ped::weapon_id != 0)
		{
			int model = 0;
			Command<Commands::GET_WEAPONTYPE_MODEL>(spawn_ped::weapon_id, &model);
			CStreaming::RequestModel(model, PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);
			Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, spawn_ped::weapon_id, 999);
		}
	}
}

void Ped::Draw()
{
	if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("CheckboxesChild");
			ImGui::Columns(2, 0, false);

			Ui::CheckboxAddress("Elvis everywhere", 0x969157);
			Ui::CheckboxAddress("Everyone is armed", 0x969140);
			Ui::CheckboxAddress("Gangs control streets", 0x96915B);
			Ui::CheckboxAddress("Gangs everywhere", 0x96915A);
			Ui::CheckboxWithHint("Gang wars", &CGangWars::bGangWarsActive);

			ImGui::NextColumn();

			Ui::CheckboxAddress("Peds mayhem", 0x96913E);
			Ui::CheckboxAddress("Peds attack with rockets", 0x969158);
			Ui::CheckboxAddress("Peds riot", 0x969175);
			Ui::CheckboxAddress("Slut magnet", 0x96915D);

			ImGui::Columns(1);
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("MenusChild");

			if (ImGui::CollapsingHeader("Gang wars"))
			{
				if (ImGui::Button("Start gang war", ImVec2(Ui::GetSize(2))))
				{
					if (Util::GetLargestGangInZone() == 1)
						CGangWars::StartDefensiveGangWar();
					else
						CGangWars::StartOffensiveGangWar();

					CGangWars::bGangWarsActive = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("End gang war", ImVec2(Ui::GetSize(2))))
					CGangWars::EndGangWar(true);

				ImGui::Dummy(ImVec2(0, 20));
				ImGui::TextWrapped("Gang zone density:");
				ImGui::Spacing();

				ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);
				for (int i = 0; i != 10; ++i)
				{
					CVector pos = FindPlayerPed()->GetPosition();
					CZone szone = CZone();
					CZone* pZone = &szone;

					CZoneExtraInfo* zone_info = CTheZones::GetZoneInfo(&pos, &pZone);
					int density = zone_info->m_nGangDensity[i];

					if (ImGui::SliderInt(Ped::gang_names[i].c_str(), &density, 0, 127))
					{
						zone_info->m_nGangDensity[i] = static_cast<char>(density);
						Command<Commands::CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR>();
						CGangWars::bGangWarsActive = true;
					}
				}
				ImGui::PopItemWidth();
				ImGui::Spacing();

				if (!exgangwars_installed)
				{
					ImGui::TextWrapped("You'll need ExGangWars plugin to display some turf colors");
					ImGui::Spacing();
					if (ImGui::Button("Download ExGangWars", Ui::GetSize(1)))
						ShellExecute(NULL, "open", "https://gtaforums.com/topic/682194-extended-gang-wars/", NULL, NULL, SW_SHOWNORMAL);
				}

				ImGui::Spacing();
				ImGui::Separator();
			}

			Ui::EditAddress<float>("Pedestrian density multiplier", 0x8D2530, 0, 1, 10);

			if (ImGui::CollapsingHeader("Recruit anyone"))
			{
				static std::vector<Ui::NamedMemory> select_weapon{ {"9mm", 0x96917C}, { "AK47", 0x96917D }, { "Rockets", 0x96917E } };
				Ui::RadioButtonAddress("Select weapon", select_weapon);
				ImGui::Spacing();
				ImGui::Separator();
			}
			if (ImGui::CollapsingHeader("Remove peds in radius"))
			{
				ImGui::InputInt("Radius", &ped_remove_radius);
				ImGui::Spacing();
				if (ImGui::Button("Remove peds", Ui::GetSize(1)))
				{
					CPlayerPed* player = FindPlayerPed();
					for (CPed* ped : CPools::ms_pPedPool)
					{
						if (DistanceBetweenPoints(ped->GetPosition(), player->GetPosition()) < ped_remove_radius
						&& ped->m_pVehicle == nullptr && ped != player)
							Command<Commands::DELETE_CHAR>(CPools::GetPedRef(ped));
					}
				}
				ImGui::Spacing();
				ImGui::Separator();
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Spawn"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Remove frozen peds", Ui::GetSize(1)))
			{
				for (CPed* ped : spawn_ped::list)
				{
					CWorld::Remove(ped);
					ped->Remove();
				}
				spawn_ped::list.clear();
			}
			ImGui::Spacing();
			if (ImGui::BeginTabBar("SpawnPedBar"))
			{
				ImGui::Spacing();

				if (ImGui::BeginTabItem("Spawner"))
				{
					ImGui::Spacing();

					Ui::DrawImages(peds_vec, ImVec2(65, 110), search_categories, selected_item, filter, SpawnPed, nullptr,
						[](std::string str) {return ped_json.data[str].get<std::string>(); });
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Config"))
				{
					ImGui::Spacing();
					ImGui::BeginChild("PedCOnfig");
					ImGui::Columns(2, 0, false);
					Ui::CheckboxWithHint("Don't move", &spawn_ped::dont_move);
					ImGui::NextColumn();
					Ui::CheckboxWithHint("Ped bleed", &spawn_ped::ped_bleed);
					ImGui::Columns(1);

					ImGui::Spacing();
					ImGui::SliderInt("Accuracy", &spawn_ped::accuracy, 0.0, 100.0);
					if (ImGui::InputInt("Health", &spawn_ped::health))
					{
						if (spawn_ped::health > 1000)
							spawn_ped::health = 1000;

						if (spawn_ped::health < 0)
							spawn_ped::health = 0;
					}
					Ui::ListBox("Ped type", spawn_ped::ped_type, spawn_ped::selected_ped_type);

					ImGui::Spacing();
					ImGui::Text((std::string("Selected weapon: ") + Weapon::weapon_json.data[std::to_string(spawn_ped::weapon_id)].get<std::string>()).c_str());
					ImGui::Spacing();
					Ui::DrawImages(Weapon::weapon_vec, ImVec2(65, 65), Weapon::search_categories, Weapon::selected_item, Weapon::filter,
						[](std::string str) { spawn_ped::weapon_id = std::stoi(str); },
						nullptr,
						[](std::string str) {return Weapon::weapon_json.data[str].get<std::string>(); },
						[](std::string str) {return str != "-1"; /*Jetpack*/ }
					);

					ImGui::Spacing();
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

