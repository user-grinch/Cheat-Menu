#include "pch.h"
#include "Ped.h"
#include "Ui.h"
#include "Util.h"

Ped::Ped()
{
	if (GetModuleHandle("ExGangWars.asi"))
		m_bExGangWarsInstalled = true;

	Events::processScriptsEvent += []
	{
		if (!m_bImagesLoaded)
		{
			Util::LoadTexturesInDirRecursive(
				PLUGIN_PATH((char*)"CheatMenu\\peds\\"), ".jpg", m_PedData.m_Categories, m_PedData.m_ImagesList);
			m_bImagesLoaded = true;
		}
	};
}

Ped::~Ped()
{
	Util::ReleaseTextures(m_PedData.m_ImagesList);
	for (CPed* ped : m_SpawnPed.m_List)
	{
		CWorld::Remove(ped);
		ped->Remove();
	}
}

void Ped::SpawnPed(std::string& model)
{
	if (m_SpawnPed.m_List.size() == SPAWN_PED_LIMIT)
	{
		CHud::SetHelpMessage("Max limit reached", false, false, false);
		return;
	}

	if (m_PedData.m_Json.m_Data.contains(model))
	{
		CPlayerPed* player = FindPlayerPed();
		CVector pos = player->GetPosition();
		pos.y += 1;

		CPed* ped;
		int hplayer;

		if (m_SpecialPedJson.m_Data.contains(model))
		{
			std::string name;
			if (m_SpecialPedJson.m_Data.contains(model))
				name = m_SpecialPedJson.m_Data[model].get<std::string>().c_str();
			else
				name = model;

			CStreaming::RequestSpecialChar(1, name.c_str(), PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(true);

			Command<Commands::CREATE_CHAR>(m_SpawnPed.m_nSelectedPedType + 4, 291, pos.x, pos.y, pos.z + 1, &hplayer);
			CStreaming::SetSpecialCharIsDeletable(291);
		}
		else
		{
			int iModel = std::stoi(model);
			CStreaming::RequestModel(iModel, eStreamingFlags::PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);

			Command<Commands::CREATE_CHAR>(m_SpawnPed.m_nSelectedPedType + 4, iModel, pos.x, pos.y, pos.z + 1, &hplayer);
			CStreaming::SetModelIsDeletable(iModel);
		}

		ped = CPools::GetPed(hplayer);

		if (m_SpawnPed.m_bPedMove)
			m_SpawnPed.m_List.push_back(ped);
		else
		{
			Command<Commands::MARK_CHAR_AS_NO_LONGER_NEEDED>(hplayer);
		}
		ped->m_nPedFlags.bPedIsBleeding = m_SpawnPed.m_bPedBleed;
		ped->m_nWeaponAccuracy = m_SpawnPed.m_nAccuracy;
		ped->m_fHealth = m_SpawnPed.m_nPedHealth;

		if (m_SpawnPed.m_nWeaponId != 0)
		{
			int model = 0;
			Command<Commands::GET_WEAPONTYPE_MODEL>(m_SpawnPed.m_nWeaponId, &model);
			CStreaming::RequestModel(model, PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);
			Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, m_SpawnPed.m_nWeaponId, 999);
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

					CZoneExtraInfo* zoneInfo = CTheZones::GetZoneInfo(&pos, &pZone);
					int density = zoneInfo->m_nGangDensity[i];

					if (ImGui::SliderInt(m_GangNames[i].c_str(), &density, 0, 127))
					{
						zoneInfo->m_nGangDensity[i] = static_cast<char>(density);
						Command<Commands::CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR>();
						CGangWars::bGangWarsActive = true;
					}
				}
				ImGui::PopItemWidth();
				ImGui::Spacing();

				if (!m_bExGangWarsInstalled)
				{
					ImGui::TextWrapped("You'll need ExGangWars plugin to display some turf colors");
					ImGui::Spacing();
					if (ImGui::Button("Download ExGangWars", Ui::GetSize(1)))
						ShellExecute(NULL, "open", "https://gtaforums.com/topic/682194-extended-gang-wars/", NULL, NULL,
						             SW_SHOWNORMAL);
				}

				ImGui::Spacing();
				ImGui::Separator();
			}

			Ui::EditAddress<float>("Pedestrian density multiplier", 0x8D2530, 0, 1, 10);

			if (ImGui::CollapsingHeader("Recruit anyone"))
			{
				static std::vector<Ui::NamedMemory> selectWeapon{
					{"9mm", 0x96917C}, {"AK47", 0x96917D}, {"Rockets", 0x96917E}
				};
				Ui::RadioButtonAddress("Select weapon", selectWeapon);
				ImGui::Spacing();
				ImGui::Separator();
			}
			if (ImGui::CollapsingHeader("Remove peds in radius"))
			{
				ImGui::InputInt("Radius", &m_nPedRemoveRadius);
				ImGui::Spacing();
				if (ImGui::Button("Remove peds", Ui::GetSize(1)))
				{
					CPlayerPed* player = FindPlayerPed();
					for (CPed* ped : CPools::ms_pPedPool)
					{
						if (DistanceBetweenPoints(ped->GetPosition(), player->GetPosition()) < m_nPedRemoveRadius
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
				for (CPed* ped : m_SpawnPed.m_List)
				{
					CWorld::Remove(ped);
					ped->Remove();
				}
				m_SpawnPed.m_List.clear();
			}
			ImGui::Spacing();
			if (ImGui::BeginTabBar("SpawnPedBar"))
			{
				ImGui::Spacing();

				if (ImGui::BeginTabItem("Spawner"))
				{
					ImGui::Spacing();

					Ui::DrawImages(m_PedData.m_ImagesList, ImVec2(65, 110), m_PedData.m_Categories, m_PedData.m_Selected,
					               m_PedData.m_Filter, SpawnPed, nullptr,
					               [](std::string str) { return m_PedData.m_Json.m_Data[str].get<std::string>(); });
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Config"))
				{
					ImGui::Spacing();
					ImGui::BeginChild("PedCOnfig");
					ImGui::Columns(2, 0, false);
					Ui::CheckboxWithHint("Don't move", &m_SpawnPed.m_bPedMove);
					ImGui::NextColumn();
					Ui::CheckboxWithHint("Ped bleed", &m_SpawnPed.m_bPedBleed);
					ImGui::Columns(1);

					ImGui::Spacing();
					ImGui::SliderInt("Accuracy", &m_SpawnPed.m_nAccuracy, 0.0, 100.0);
					if (ImGui::InputInt("Health", &m_SpawnPed.m_nPedHealth))
					{
						if (m_SpawnPed.m_nPedHealth > 1000)
							m_SpawnPed.m_nPedHealth = 1000;

						if (m_SpawnPed.m_nPedHealth < 0)
							m_SpawnPed.m_nPedHealth = 0;
					}
					Ui::ListBox("Ped type", m_SpawnPed.m_PedTypeList, m_SpawnPed.m_nSelectedPedType);

					ImGui::Spacing();
					ImGui::Text("Selected weapon: %s",
					            Weapon::m_WeaponData.m_Json.m_Data[std::to_string(m_SpawnPed.m_nWeaponId)].get<std::string>());
					ImGui::Spacing();
					Ui::DrawImages(Weapon::m_WeaponData.m_ImagesList, ImVec2(65, 65), Weapon::m_WeaponData.m_Categories,
					               Weapon::m_WeaponData.m_Selected, Weapon::m_WeaponData.m_Filter,
					               [](std::string str) { m_SpawnPed.m_nWeaponId = std::stoi(str); },
					               nullptr,
					               [](std::string str)
					               {
						               return Weapon::m_WeaponData.m_Json.m_Data[str].get<std::string>();
					               },
					               [](std::string str) { return str != "-1"; /*Jetpack*/ }
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
