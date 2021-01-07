#include "pch.h"
#include "Weapon.h"
#include "Ui.h"
#include "Util.h"

ImGuiTextFilter Weapon::filter = "";
std::string Weapon::selected_item = "All";
std::vector<std::string> Weapon::search_categories;
std::vector<std::unique_ptr<TextureStructure>> Weapon::weapon_vec;

CJson Weapon::weapon_json = CJson("weapon");
bool Weapon::auto_aim = false;
bool Weapon::fast_reload = false;
bool Weapon::huge_damage = false;
bool Weapon::long_range = false;
bool Weapon::rapid_fire = false;
bool Weapon::dual_weild = false;
bool Weapon::move_aim = false;
bool Weapon::move_fire = false;

uchar Weapon::cur_weapon_slot = -1;
int Weapon::ammo_count = 99999;

int Weapon::selected_gang = 0;
int Weapon::selected_weapon_count = 0;

int Weapon::gang_weapons[10][3] =
{
	{WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED },		// Ballas
	{WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED},		// Grove
	{WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED},		// Vagos
	{WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},		// SF Rifa
	{WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},		// Da Nang Boys
	{WEAPON_DESERT_EAGLE , WEAPON_UNARMED, WEAPON_UNARMED},	// Mafia
	{WEAPON_PISTOL, WEAPON_AK47, WEAPON_UNARMED},			// Triads
	{WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},		// VLA
	{WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},		// Gang 9
	{WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},		// Gang 10
};

Weapon::Weapon()
{
	Events::initGameEvent += []
	{
		Util::LoadTexturesInDirRecursive(PLUGIN_PATH((char*)"CheatMenu\\weapons\\"), ".jpg", Weapon::search_categories, Weapon::weapon_vec);
	};

	Events::processScriptsEvent += []
	{
		CPlayerPed *player = FindPlayerPed();
		if (auto_aim)
		{
			if (CPad::NewMouseControllerState.X == 0 && CPad::NewMouseControllerState.Y == 0)
			{
				if (KeyPressed(2))
					CCamera::m_bUseMouse3rdPerson = false;
			}
			else
				CCamera::m_bUseMouse3rdPerson = true;
		}

		uchar slot = player->m_nActiveWeaponSlot;
		if (cur_weapon_slot != slot)
		{
			auto weapon_type = player->m_aWeapons[slot].m_nType;
			CWeaponInfo *pweapon_info = CWeaponInfo::GetWeaponInfo(weapon_type, player->GetWeaponSkill(weapon_type));

			if (huge_damage)
				pweapon_info->m_nDamage = 1000;

			if (long_range)
			{
				pweapon_info->m_fTargetRange = 1000.0f;
				pweapon_info->m_fWeaponRange = 1000.0f;
				pweapon_info->m_fAccuracy = 1.0f;
				pweapon_info->m_nFlags.bReload2Start = true;
			}

			if (rapid_fire)
				pweapon_info->m_nFlags.bContinuosFire = true;

			if (dual_weild  && (weapon_type == WEAPON_PISTOL || weapon_type == WEAPON_MICRO_UZI || weapon_type == WEAPON_TEC9 || weapon_type == WEAPON_SAWNOFF))
				pweapon_info->m_nFlags.bTwinPistol = true;

			if (move_aim)
				pweapon_info->m_nFlags.bMoveAim = true;
			
			if (move_fire)
				pweapon_info->m_nFlags.bMoveFire = true;

			cur_weapon_slot = slot;
		}
	};
}


Weapon::~Weapon()
{
}

void Weapon::SetGangWeapon(std::string& weapon_type)
{
	gang_weapons[selected_gang][selected_weapon_count] = std::stoi(weapon_type);
	CGangs::SetGangWeapons(selected_gang, gang_weapons[selected_gang][0], gang_weapons[selected_gang][1], gang_weapons[selected_gang][2]);
}

void Weapon::GiveWeaponToPlayer(std::string& weapon_type)
{
	CPlayerPed* player = FindPlayerPed();
	int hplayer = CPools::GetPedRef(player);

	if (weapon_type == "-1") // Jetpack
		Command<Commands::TASK_JETPACK>(hplayer);
	else
	{
		int iweapon_type = std::stoi(weapon_type);

		int model = NULL;
		Command<Commands::GET_WEAPONTYPE_MODEL>(iweapon_type, &model);

		CStreaming::RequestModel(model,PRIORITY_REQUEST);

		if (model == 363) // remote bomb
			CStreaming::RequestModel(364,PRIORITY_REQUEST); // detonator

		CStreaming::LoadAllRequestedModels(false);

		Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, iweapon_type, ammo_count);

		if (model == 363) // remote bomb
			Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(364); // detonator

		Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(model);
	} 
}

void Weapon::Main()
{
	CPlayerPed *player = FindPlayerPed();
	uint hplayer = CPools::GetPedRef(player);

	ImGui::Spacing();
	if (ImGui::Button("Drop weapon", Ui::GetSize(3)))
	{
		float x, y, z;
		Command<Commands::GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS>(hplayer, 0.0, 3.0, 0.0, &x, &y, &z);
		auto type = player->m_aWeapons[player->m_nActiveWeaponSlot].m_nType;
			
		if (type)
		{
			int model =0, pickup = 0;

			Command<Commands::GET_WEAPONTYPE_MODEL>(type, &model);
			Command<Commands::CREATE_PICKUP_WITH_AMMO>(model, 3, 999, x, y, z, &pickup);

			Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, type);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove all", Ui::GetSize(3)))
		Command<Commands::REMOVE_ALL_CHAR_WEAPONS>(hplayer);

	ImGui::SameLine();
	if (ImGui::Button("Remove current", Ui::GetSize(3)))
		Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, player->m_aWeapons[player->m_nActiveWeaponSlot].m_nType);

	ImGui::Spacing();

	if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("CheckboxesChild");
			ImGui::Columns(2, 0, false);

			Ui::CheckboxWithHint("Auto aim", &auto_aim, "Enables aim assist on keyboard\n\nQ = left    E = right");
			if (Ui::CheckboxWithHint("Dual weild", &dual_weild,"Dual weild pistol, shawoff, uzi, tec9\n(Other weapons don't work)"))
			{
				if (!dual_weild)
					CWeaponInfo::LoadWeaponData();
			}
			if (Ui::CheckboxWithHint("Huge damage", &huge_damage))
			{
				if (!huge_damage)
					CWeaponInfo::LoadWeaponData();
			}
			if (Ui::CheckboxWithHint("Fast reload", &fast_reload))
				Command<Commands::SET_PLAYER_FAST_RELOAD>(hplayer, fast_reload);

			Ui::CheckboxAddress("Infinite ammo", 0x969178);

			ImGui::NextColumn();
			
			if (Ui::CheckboxWithHint("Long range", &long_range))
			{
				if (!long_range)
					CWeaponInfo::LoadWeaponData();
			}
			if (Ui::CheckboxWithHint("Move when aiming", &move_aim))
			{
				if (!move_aim)
					CWeaponInfo::LoadWeaponData();
			}
			if (Ui::CheckboxWithHint("Move when firing", &move_fire))
			{
				if (!move_fire)
					CWeaponInfo::LoadWeaponData();
			}
			if (Ui::CheckboxWithHint("Rapid fire", &rapid_fire))
			{
				if (!rapid_fire)
					CWeaponInfo::LoadWeaponData();
			}
			ImGui::Columns(1, 0, false);
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Gang weapon editor"))
		{
			ImGui::Spacing();
			Ui::ListBox("Select gang", Ped::gang_names, selected_gang);

			ImGui::Columns(3, 0, false);
			ImGui::RadioButton("Weap 1", &selected_weapon_count, 0);
			ImGui::NextColumn();
			ImGui::RadioButton("Weap 2", &selected_weapon_count, 1);
			ImGui::NextColumn();
			ImGui::RadioButton("Weap 3", &selected_weapon_count, 2);
			ImGui::Columns(1);

			ImGui::Spacing();
			ImGui::Text((std::string("Current weapon: ") + weapon_json.data[std::to_string(gang_weapons[selected_gang][selected_weapon_count])].get<std::string>()).c_str());
			ImGui::Spacing();
			Ui::DrawImages(weapon_vec, ImVec2(65, 65), search_categories, selected_item, filter, SetGangWeapon, nullptr,
				[](std::string str) {return weapon_json.data[str].get<std::string>(); },
				[](std::string str) {return str != "-1"; /*Jetpack*/ }
			);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Spawn"))
		{
			ImGui::Spacing();
			if (ImGui::InputInt("Ammo", &ammo_count))
			{
				ammo_count = (ammo_count < 0) ? 0 : ammo_count;
				ammo_count = (ammo_count > 99999) ? 99999 : ammo_count;
			}
			Ui::DrawImages(weapon_vec, ImVec2(65, 65), search_categories, selected_item, filter, GiveWeaponToPlayer, nullptr,
				[](std::string str) {return weapon_json.data[str].get<std::string>(); },
				[](std::string str) {return str != "0"; /*Unarmed*/ }
			);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
