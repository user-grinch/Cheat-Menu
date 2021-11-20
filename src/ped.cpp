#include "pch.h"
#include "ped.h"
#include "ui.h"
#include "util.h"
#include "weapon.h"
#include <CPopulation.h>

#ifdef GTASA
#include <ePedBones.h>
#endif

Ped::Ped()
{
#ifdef GTASA
	if (GetModuleHandle("ExGangWars.asi"))
	{
		m_bExGangWarsInstalled = true;
	}

	/*
		Taken from gta chaos mod by Lordmau5
		https://github.com/gta-chaos-mod/Trilogy-ASI-Script

		TODO: Implement in VC too
	*/
	Events::pedRenderEvent += [](CPed *ped)
	{

		if (m_bBigHead || m_bThinBody)
		{
			auto animHier = GetAnimHierarchyFromSkinClump (ped->m_pRwClump);
			auto matrices = RpHAnimHierarchyGetMatrixArray (animHier);

			RwV3d scale = {0.7f, 0.7f, 0.7f};
			if (m_bThinBody)
			{
				for (int i = 1; i <= 52; i++)
				{
					RwMatrixScale (&matrices[RpHAnimIDGetIndex (animHier, i)], &scale, rwCOMBINEPRECONCAT);
				}
			}
			scale = {3.0f, 3.0f, 3.0f};
			if (m_bBigHead)
			{
				for (int i = BONE_NECK; i <= BONE_HEAD; i++)
				{
					RwMatrixScale (&matrices[RpHAnimIDGetIndex (animHier, i)], &scale, rwCOMBINEPRECONCAT);
				}
			}
		}
	};
#endif
}

Ped::~Ped()
{
	for (CPed* ped : m_SpawnPed::m_List)
	{
		CWorld::Remove(ped);
		ped->Remove();
	}
}

#ifdef GTASA
void Ped::SpawnPed(std::string& model)
#else // GTA3 & GTAVC
void Ped::SpawnPed(std::string& cat, std::string& name, std::string& model)
#endif
{
	if (m_SpawnPed::m_List.size() == SPAWN_PED_LIMIT)
	{
		SetHelpMessage("Max limit reached", false, false, false);
		return;
	}

	if (BY_GAME(m_PedData.m_pJson->m_Data.contains(model), true, true))
	{
		CPlayerPed* player = FindPlayerPed();
		CVector pos = player->GetPosition();
		pos.y += 1;

		CPed* ped;
		int hplayer;
		static size_t currentSlot = 1;

#ifdef GTASA
		if (m_SpecialPedJson.m_Data.contains(model))
		{
			std::string name;
			if (m_SpecialPedJson.m_Data.contains(model))
				name = m_SpecialPedJson.m_Data[model].get<std::string>().c_str();
			else
				name = model;

			CStreaming::RequestSpecialChar(currentSlot, name.c_str(), PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(true);

			Command<Commands::CREATE_CHAR>(m_SpawnPed::m_nSelectedPedType + 4, 290 + currentSlot, pos.x, pos.y, pos.z + 1, &hplayer);
			CStreaming::SetSpecialCharIsDeletable(290 + currentSlot);

			// SA has 10 slots 
			++currentSlot;
			if (currentSlot > 9)
			{
				currentSlot = 1;
			}
		}
#else // GTA3 & GTAVC
		if (cat == "Special") // Special model
		{
#ifdef GTA3
			SetHelpMessage("Spawning special peds isn't implemented yet.", false, false, false);
			return;
#else // GTAVC
			Command<Commands::LOAD_SPECIAL_CHARACTER>(currentSlot, model.c_str());
			Command<Commands::LOAD_ALL_MODELS_NOW>();
			
			Command<Commands::CREATE_CHAR>(m_SpawnPed::m_nSelectedPedType + 4, 108+currentSlot, pos.x, pos.y, pos.z + 1, &hplayer);
			Command<Commands::UNLOAD_SPECIAL_CHARACTER>(currentSlot);

			++currentSlot;
			if (currentSlot > 21)
			{
				currentSlot = 1;
			}
#endif
		}
#endif
		else
		{	
			int iModel = std::stoi(model);
			CStreaming::RequestModel(iModel, eStreamingFlags::PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);

			Command<Commands::CREATE_CHAR>(m_SpawnPed::m_nSelectedPedType + 4, iModel, pos.x, pos.y, pos.z + 1, &hplayer);
			CStreaming::SetModelIsDeletable(iModel);
		}

		ped = CPools::GetPed(hplayer);

		if (m_SpawnPed::m_bPedMove)
		{
			m_SpawnPed::m_List.push_back(ped);
		}
		else
		{
			Command<Commands::MARK_CHAR_AS_NO_LONGER_NEEDED>(hplayer);
		}
		ped->m_nPedFlags.bPedIsBleeding = m_SpawnPed::m_bPedBleed;
#ifdef GTA3
		ped->m_nWepAccuracy = m_SpawnPed::m_nAccuracy;
#else
		ped->m_nWeaponAccuracy = m_SpawnPed::m_nAccuracy;
#endif
		ped->m_fHealth = m_SpawnPed::m_nPedHealth;
#ifdef GTASA
		if (m_SpawnPed::m_nWeaponId != 0)
		{
			int model = 0;
			Command<Commands::GET_WEAPONTYPE_MODEL>(m_SpawnPed::m_nWeaponId, &model);
			CStreaming::RequestModel(model, PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);
			Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, m_SpawnPed::m_nWeaponId, 999);
		}
#endif
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
#ifdef GTASA
			Ui::CheckboxWithHint("Big head effect", &m_bBigHead);	
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
			Ui::CheckboxWithHint("Thin body effect", &m_bThinBody);	
#elif GTAVC
			Ui::CheckboxAddress("No prostitutes", 0xA10B99);
			Ui::CheckboxAddress("Slut magnet", 0xA10B5F);
			ImGui::NextColumn();
			Ui::CheckboxAddress("Weapons for all", 0xA10AB3);
#else // GTA3
			// Bad idea lol
			static bool pedsMayhem;
			if (Ui::CheckboxWithHint("Peds mayhem", &pedsMayhem))
			{
				Call<0x4911C0>();
			}
			static bool everyoneAttacksPlayer;
			if (Ui::CheckboxWithHint("Everyone attacks players", &everyoneAttacksPlayer))
			{
				Call<0x491270>();
			}
			ImGui::NextColumn();
			Ui::CheckboxAddress("Nasty limbs", 0x95CD44);
			Ui::CheckboxAddress("Weapons for all", 0x95CCF6);
#endif
			ImGui::Columns(1);
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("MenusChild");

#ifdef GTASA
			if (ImGui::CollapsingHeader("Gang wars"))
			{
				if (ImGui::Button("Start gang war", ImVec2(Ui::GetSize(2))))
				{
					if (Util::GetLargestGangInZone() == 1)
					{
						CGangWars::StartDefensiveGangWar();
					}
					else
					{
						CGangWars::StartOffensiveGangWar();
					}
					CGangWars::bGangWarsActive = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("End gang war", ImVec2(Ui::GetSize(2))))
				{
					CGangWars::EndGangWar(true);
				}

				ImGui::Dummy(ImVec2(0, 20));
				ImGui::TextWrapped("Gang zone density:");
				ImGui::Spacing();

				ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);
				for (int i = 0; i != 10; ++i)
				{
					CVector pos = FindPlayerPed()->GetPosition();
					CZone szone = CZone();
					CZone* pZone = &szone;

					CZoneInfo* zoneInfo = CTheZones::GetZoneInfo(&pos, &pZone);
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
					{
						ShellExecute(NULL, "open", "https://gtaforums.com/topic/682194-extended-gang-wars/", NULL, NULL,
						             SW_SHOWNORMAL);
					}
				}

				ImGui::Spacing();
				ImGui::Separator();
			}
#endif
			Ui::EditReference<float>("Pedestrian density multiplier", CPopulation::PedDensityMultiplier, 0, 1, 10);
#ifdef GTASA
			if (ImGui::CollapsingHeader("Recruit anyone"))
			{
				static std::vector<Ui::NamedMemory> selectWeapon{
					{"9mm", 0x96917C}, {"AK47", 0x96917D}, {"Rockets", 0x96917E}
				};
				Ui::RadioButtonAddress("Select weapon", selectWeapon);
				ImGui::Spacing();
				ImGui::Separator();
			}
#endif

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
						{
							Command<Commands::DELETE_CHAR>(CPools::GetPedRef(ped));
						}
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
				for (CPed* ped : m_SpawnPed::m_List)
				{
					CWorld::Remove(ped);
					ped->Remove();
				}
				m_SpawnPed::m_List.clear();
			}
			ImGui::Spacing();
			if (ImGui::BeginTabBar("SpawnPedBar"))
			{
				ImGui::Spacing();

				if (ImGui::BeginTabItem("Spawner"))
				{
					ImGui::Spacing();
#ifdef GTASA
					Ui::DrawImages(m_PedData, SpawnPed, nullptr,
					               [](std::string str) { return m_PedData.m_pJson->m_Data[str].get<std::string>(); });
#else // GTA3 & GTAVC
					Ui::DrawJSON(m_PedData, SpawnPed, nullptr);
#endif
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Config"))
				{
					ImGui::Spacing();
					ImGui::BeginChild("PedCOnfig");
					ImGui::Columns(2, 0, false);
					Ui::CheckboxWithHint("Don't move", &m_SpawnPed::m_bPedMove);
					ImGui::NextColumn();
					Ui::CheckboxWithHint("Ped bleed", &m_SpawnPed::m_bPedBleed);
					ImGui::Columns(1);

					ImGui::Spacing();
					ImGui::SliderInt("Accuracy", &m_SpawnPed::m_nAccuracy, 0.0, 100.0);
					if (ImGui::InputInt("Health", &m_SpawnPed::m_nPedHealth))
					{
						if (m_SpawnPed::m_nPedHealth > 1000)
						{
							m_SpawnPed::m_nPedHealth = 1000;
						}

						if (m_SpawnPed::m_nPedHealth < 0)
						{
							m_SpawnPed::m_nPedHealth = 0;
						}
					}
					Ui::ListBox("Ped type", m_SpawnPed::m_PedTypeList, m_SpawnPed::m_nSelectedPedType);

					ImGui::Spacing();
					ImGui::Text("Selected weapon: %s", m_SpawnPed::m_nWeaponName.c_str());
					ImGui::Spacing();
#ifdef GTASA
					Ui::DrawImages(Weapon::m_WeaponData,
					               [](std::string str) { m_SpawnPed::m_nWeaponId = std::stoi(str); },
					               nullptr,
					               [](std::string str)
					               {
									   m_SpawnPed::m_nWeaponName = Weapon::m_WeaponData.m_pJson->m_Data[str].get<std::string>();
						               return m_SpawnPed::m_nWeaponName;
					               },
					               [](std::string str) { return str != "-1"; /*Jetpack*/ }
					);
#else // GTA3 & GTAVC
					Ui::DrawJSON(Weapon::m_WeaponData, 
					[](std::string& root, std::string& key, std::string& id) 
					{
						m_SpawnPed::m_nWeaponId = std::stoi(id); 
						m_SpawnPed::m_nWeaponName = key;
					}, 
					nullptr);
#endif
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
