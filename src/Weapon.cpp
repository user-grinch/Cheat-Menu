#include "pch.h"
#include "Weapon.h"
#include "Ui.h"
#include "Util.h"
#include "Ped.h"

Weapon::Weapon()
{
	Events::processScriptsEvent += []
	{
		if (!m_bImagesLoaded)
		{
			Util::LoadTextureDirectory(m_WeaponData, PLUGIN_PATH((char*)"CheatMenu\\weapons.txd"));
			m_bImagesLoaded = true;
		}

		CPlayerPed* player = FindPlayerPed();
		if (m_bAutoAim)
		{
			if (CPad::NewMouseControllerState.X == 0 && CPad::NewMouseControllerState.Y == 0)
			{
				if (KeyPressed(2))
				{
					CCamera::m_bUseMouse3rdPerson = false;
				}
			}
			else
			{
				CCamera::m_bUseMouse3rdPerson = true;
			}
		}

		uchar slot = player->m_nActiveWeaponSlot;
		if (m_nCurrentWeaponSlot != slot)
		{
			eWeaponType weaponType = player->m_aWeapons[slot].m_nType;
			CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(weaponType, player->GetWeaponSkill(weaponType));

			if (m_bHugeDamage)
			{
				pWeaponInfo->m_nDamage = 1000;
			}

			if (m_bLongRange)
			{
				pWeaponInfo->m_fTargetRange = 1000.0f;
				pWeaponInfo->m_fWeaponRange = 1000.0f;
				pWeaponInfo->m_fAccuracy = 1.0f;
				pWeaponInfo->m_nFlags.bReload2Start = true;
			}

			if (m_bRapidFire && weaponType != WEAPON_MINIGUN) // mingun doesn't work with rapidfire
			{
				pWeaponInfo->m_nFlags.bContinuosFire = true;
			}

			if (m_bDualWeild && (weaponType == WEAPON_PISTOL || weaponType == WEAPON_MICRO_UZI || weaponType ==
				WEAPON_TEC9 || weaponType == WEAPON_SAWNOFF))
			{
				pWeaponInfo->m_nFlags.bTwinPistol = true;
			}

			if (m_bMoveAim)
			{
				pWeaponInfo->m_nFlags.bMoveAim = true;
			}

			if (m_bMoveFire)
			{
				pWeaponInfo->m_nFlags.bMoveFire = true;
			}

			m_nCurrentWeaponSlot = slot;
		}
	};
}


void Weapon::SetGangWeapon(std::string& weapon_type)
{
	m_nGangWeaponList[m_nSelectedGang][m_nSelectedWeapon] = std::stoi(weapon_type);
	CGangs::SetGangWeapons(m_nSelectedGang, m_nGangWeaponList[m_nSelectedGang][0], m_nGangWeaponList[m_nSelectedGang][1],
	                       m_nGangWeaponList[m_nSelectedGang][2]);
}

void Weapon::GiveWeaponToPlayer(std::string& weapon_type)
{
	CPlayerPed* player = FindPlayerPed();
	int hplayer = CPools::GetPedRef(player);

	if (weapon_type == "-1") // Jetpack
	{
		Command<Commands::TASK_JETPACK>(hplayer);
	}
	else
	{
		int iweapon_type = std::stoi(weapon_type);

		int model = NULL;
		Command<Commands::GET_WEAPONTYPE_MODEL>(iweapon_type, &model);

		CStreaming::RequestModel(model, PRIORITY_REQUEST);

		if (model == 363) // remote bomb
		{
			CStreaming::RequestModel(364, PRIORITY_REQUEST); // detonator
		}

		CStreaming::LoadAllRequestedModels(false);
		Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, iweapon_type, m_nAmmoCount);

		if (model == 363) // remote bomb
		{
			Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(364); // detonator
		}

		Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(model);
	}
}

void Weapon::Draw()
{
	CPlayerPed* pPlayer = FindPlayerPed();
	uint hplayer = CPools::GetPedRef(pPlayer);

	ImGui::Spacing();
	if (ImGui::Button("Drop weapon", Ui::GetSize(3)))
	{
		float x, y, z;
		Command<Commands::GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS>(hplayer, 0.0, 3.0, 0.0, &x, &y, &z);
		eWeaponType weaponType = pPlayer->m_aWeapons[pPlayer->m_nActiveWeaponSlot].m_nType;

		if (weaponType)
		{
			int model = 0, pickup = 0;

			Command<Commands::GET_WEAPONTYPE_MODEL>(weaponType, &model);
			Command<Commands::CREATE_PICKUP_WITH_AMMO>(model, 3, 999, x, y, z, &pickup);

			Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, weaponType);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove all", Ui::GetSize(3)))
	{
		Command<Commands::REMOVE_ALL_CHAR_WEAPONS>(hplayer);
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove current", Ui::GetSize(3)))
	{
		Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, pPlayer->m_aWeapons[pPlayer->m_nActiveWeaponSlot].m_nType);
	}

	ImGui::Spacing();
	

	if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("CheckboxesChild");
			ImGui::Columns(2, 0, false);

			Ui::CheckboxWithHint("Fast aim", &m_bAutoAim, "Enables aim assist on keyboard\n\nQ = left    E = right\n\nPress Q and E to switch targets.\nMoving mouse removes the target!");
			if (Ui::CheckboxWithHint("Dual wield", &m_bDualWeild,
			                         "Dual wield pistol, shawoff, uzi, tec9\n(Other weapons don't work)"))
			{
				if (!m_bDualWeild)
				{
					CWeaponInfo::LoadWeaponData();
				}
			}
			if (Ui::CheckboxWithHint("Huge damage", &m_bHugeDamage))
			{
				if (!m_bHugeDamage)
				{
					CWeaponInfo::LoadWeaponData();
				}
			}
			if (Ui::CheckboxWithHint("Fast reload", &m_bFastReload))
				Command<Commands::SET_PLAYER_FAST_RELOAD>(hplayer, m_bFastReload);

			Ui::CheckboxAddress("Infinite ammo", 0x969178);

			ImGui::NextColumn();

			if (Ui::CheckboxWithHint("Long range", &m_bLongRange))
			{
				if (!m_bLongRange)
				{
					CWeaponInfo::LoadWeaponData();
				}
			}
			if (Ui::CheckboxWithHint("Move when aiming", &m_bMoveAim))
			{
				if (!m_bMoveAim)
				{
					CWeaponInfo::LoadWeaponData();
				}
			}
			if (Ui::CheckboxWithHint("Move when firing", &m_bMoveFire))
			{
				if (!m_bMoveFire)
				{
					CWeaponInfo::LoadWeaponData();
				}
			}
			if (Ui::CheckboxWithHint("Rapid fire", &m_bRapidFire))
			{
				if (!m_bRapidFire)
				{
					CWeaponInfo::LoadWeaponData();
				}
			}
			ImGui::Columns(1, 0, false);
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Gang weapon editor"))
		{
			ImGui::Spacing();
			Ui::ListBox("Select gang", Ped::m_GangNames, m_nSelectedGang);

			ImGui::Columns(3, 0, false);
			ImGui::RadioButton("Weap 1", &m_nSelectedWeapon, 0);
			ImGui::NextColumn();
			ImGui::RadioButton("Weap 2", &m_nSelectedWeapon, 1);
			ImGui::NextColumn();
			ImGui::RadioButton("Weap 3", &m_nSelectedWeapon, 2);
			ImGui::Columns(1);

			ImGui::Spacing();
			ImGui::Text("Current weapon: %s",
			            m_WeaponData.m_Json.m_Data[std::to_string(m_nGangWeaponList[m_nSelectedGang][m_nSelectedWeapon])].get<
				            std::string>().c_str());
			ImGui::Spacing();
			Ui::DrawImages(m_WeaponData.m_ImagesList, ImVec2(65, 65), m_WeaponData.m_Categories, m_WeaponData.m_Selected,
			               m_WeaponData.m_Filter, SetGangWeapon, nullptr,
			               [](std::string str) { return m_WeaponData.m_Json.m_Data[str].get<std::string>(); },
			               [](std::string str) { return str != "-1"; /*Jetpack*/ }
			);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Spawn"))
		{
			ImGui::Spacing();
			if (ImGui::InputInt("Ammo", &m_nAmmoCount))
			{
				m_nAmmoCount = (m_nAmmoCount < 0) ? 0 : m_nAmmoCount;
				m_nAmmoCount = (m_nAmmoCount > 99999) ? 99999 : m_nAmmoCount;
			}
			Ui::DrawImages(m_WeaponData.m_ImagesList, ImVec2(65, 65), m_WeaponData.m_Categories, m_WeaponData.m_Selected,
			               m_WeaponData.m_Filter, GiveWeaponToPlayer, nullptr,
			               [](std::string str) { return m_WeaponData.m_Json.m_Data[str].get<std::string>(); },
			               [](std::string str) { return str != "0"; /*Unarmed*/ }
			);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
