#include "pch.h"
#include "player.h"
#include "menu.h"
#include "ui.h"
#include "util.h"
#ifdef GTASA
#include "ped.h"

// hardcoded cloth category names
const char* cloth_category[18] =
{
	"Shirts",
	"Heads",
	"Trousers",
	"Shoes",
	"Tattoos left lower arm",
	"Tattoos left upper arm",
	"Tattoos right upper arm",
	"Tattoos right lower arm",
	"Tattoos back",
	"Tattoos left chest",
	"Tattoos right chest",
	"Tattoos stomach",
	"Tattoos lower back",
	"Necklaces",
	"Watches",
	"Glasses",
	"Hats",
	"Extras"
};

static inline void PlayerModelBrokenFix()
{
	CPlayerPed* pPlayer = FindPlayerPed();

	if (pPlayer->m_nModelIndex == 0)
		Call<0x5A81E0>(0, pPlayer->m_pPlayerData->m_pPedClothesDesc, 0xBC1C78, false);
}


/*
	Taken from gta chaos mod by Lordmau5
	https://github.com/gta-chaos-mod/Trilogy-ASI-Script
*/
void Player::TopDownCameraView()
{
	CPlayerPed *player = FindPlayerPed ();
	CVector     pos    = player->GetPosition ();
	float       curOffset = m_TopDownCamera::m_fOffset;

	// drunk effect causes issues
	Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 0);

	CVehicle *vehicle = FindPlayerVehicle(-1, false);
	
	// TODO: implement smooth transition
	if (vehicle)
	{
		float speed = vehicle->m_vecMoveSpeed.Magnitude();
		if (speed > 1.2f)
		{
			speed = 1.2f;
		}
		if (speed * 40.0f > 40.0f)
		{
			speed = 40.0f;
		}

		if (speed < 0.0f)
		{
			speed = 0.0f;
		}
		curOffset += speed; 
	}

	CVector playerOffset = CVector (pos.x, pos.y, pos.z + 2.0f);
	CVector cameraPos
		= CVector (playerOffset.x, playerOffset.y, playerOffset.z + curOffset);

	CColPoint outColPoint;
	CEntity * outEntity;

	// TODO: Which variable? X, Y or Z for the look direction?

	if (CWorld::ProcessLineOfSight (playerOffset, cameraPos, outColPoint,
									outEntity, true, true, true, true, true,
									true, true, true))
	{
		Command<eScriptCommands::COMMAND_SET_FIXED_CAMERA_POSITION> (
			outColPoint.m_vecPoint.x, outColPoint.m_vecPoint.y,
			outColPoint.m_vecPoint.z, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		Command<eScriptCommands::COMMAND_SET_FIXED_CAMERA_POSITION> (
			cameraPos.x, cameraPos.y, cameraPos.z, 0.0f, 0.0f, 0.0f);
	}

	Command<eScriptCommands::COMMAND_POINT_CAMERA_AT_POINT> (pos.x, pos.y,
															pos.z, 2);

	TheCamera.m_fGenerationDistMultiplier = 10.0f;
	TheCamera.m_fLODDistMultiplier        = 10.0f;
}
#endif

Player::Player()
{
#ifdef GTASA
//	Fix player model being broken after rebuild
	patch::RedirectCall(0x5A834D, &PlayerModelBrokenFix);
	m_bAimSkinChanger = gConfig.GetValue("aim_skin_changer", false);
#endif

	// Custom skins setup
	if (GetModuleHandle("modloader.asi"))
	{
#ifdef GTASA
		if (std::filesystem::is_directory(m_CustomSkins::m_Path))
		{
			for (auto& p : std::filesystem::recursive_directory_iterator(m_CustomSkins::m_Path))
			{
				if (p.path().extension() == ".dff")
				{
					std::string file_name = p.path().stem().string();

					if (file_name.size() < 9)
						m_CustomSkins::m_List.push_back(file_name);
					else
						gLog << "Custom Skin longer than 8 characters " << file_name << std::endl;
				}
			}
		}
		else
		{
			std::filesystem::create_directory(m_CustomSkins::m_Path);
		}
#endif

		m_bModloaderInstalled = true;
	}

	Events::processScriptsEvent += []
	{
		uint timer = CTimer::m_snTimeInMilliseconds;
		CPlayerPed* player = FindPlayerPed();
		int hplayer = CPools::GetPedRef(player);

		if (m_KeepPosition::m_bEnabled)
		{
			if (Command<Commands::IS_CHAR_DEAD>(hplayer))
			{
				m_KeepPosition::m_fPos = player->GetPosition();
			}
			else
			{
				CVector cur_pos = player->GetPosition();

				if (m_KeepPosition::m_fPos.x != 0 && m_KeepPosition::m_fPos.x != cur_pos.x
					&& m_KeepPosition::m_fPos.y != 0 && m_KeepPosition::m_fPos.y != cur_pos.y)
				{
					BY_GAME(player->Teleport(m_KeepPosition::m_fPos, false)
					, player->Teleport(m_KeepPosition::m_fPos), player->Teleport(m_KeepPosition::m_fPos));
					m_KeepPosition::m_fPos = CVector(0, 0, 0);
				}
			}
		}

		if (m_bGodMode)
		{
#ifdef GTASA
			patch::Set<bool>(0x96916D, 1, false);
			player->m_nPhysicalFlags.bBulletProof = 1;
			player->m_nPhysicalFlags.bCollisionProof = 1;
			player->m_nPhysicalFlags.bExplosionProof = 1;
			player->m_nPhysicalFlags.bFireProof = 1;
			player->m_nPhysicalFlags.bMeeleProof  = 1;
#elif GTAVC
			player->m_nFlags.bBulletProof = 1;
			player->m_nFlags.bCollisionProof = 1;
			player->m_nFlags.bExplosionProof = 1;
			player->m_nFlags.bFireProof = 1;
			player->m_nFlags.bMeleeProof = 1;
#else // GTA3
			player->m_nEntityFlags.bBulletProof = m_bGodMode;
			player->m_nEntityFlags.bCollisionProof = m_bGodMode;
			player->m_nEntityFlags.bExplosionProof = m_bGodMode;
			player->m_nEntityFlags.bFireProof = m_bGodMode;
			player->m_nEntityFlags.bMeleeProof = m_bGodMode;
#endif
		}

#ifdef GTASA
		if (m_bDrunkEffect && !m_TopDownCamera::m_bEnabled)
		{
			Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 100);
		}

		if (m_TopDownCamera::m_bEnabled)
		{
			TopDownCameraView();
		}

		if (m_bAimSkinChanger && aimSkinChanger.Pressed())
		{
			CPed* targetPed = player->m_pPlayerTargettedPed;
			if (targetPed)
			{
				player->SetModelIndex(targetPed->m_nModelIndex);
				Util::ClearCharTasksVehCheck(player);
			}
		}
#endif

		if (godMode.Pressed())
		{
			if (m_bGodMode)
			{
				SetHelpMessage("God mode disabled", false, false, false);
#ifdef GTASA
				patch::Set<bool>(0x96916D, m_bGodMode, false);
				player->m_nPhysicalFlags.bBulletProof = 0;
				player->m_nPhysicalFlags.bCollisionProof = 0;
				player->m_nPhysicalFlags.bExplosionProof = 0;
				player->m_nPhysicalFlags.bFireProof = 0;
				player->m_nPhysicalFlags.bMeeleProof = 0;
#elif GTAVC
				player->m_nFlags.bBulletProof = 0;
				player->m_nFlags.bCollisionProof = 0;
				player->m_nFlags.bExplosionProof = 0;
				player->m_nFlags.bFireProof = 0;
				player->m_nFlags.bMeleeProof = 0;
#else // GTA3
				player->m_nEntityFlags.bBulletProof = m_bGodMode;
				player->m_nEntityFlags.bCollisionProof = m_bGodMode;
				player->m_nEntityFlags.bExplosionProof = m_bGodMode;
				player->m_nEntityFlags.bFireProof = m_bGodMode;
				player->m_nEntityFlags.bMeleeProof = m_bGodMode;
#endif
				m_bGodMode = false;
			}
			else
			{
				SetHelpMessage("God mode enabled", false, false, false);
				m_bGodMode = true;
			}
		}
	};
}

#ifdef GTASA
void Player::ChangePlayerCloth(std::string& name)
{
	std::stringstream ss(name);
	std::string temp;

	getline(ss, temp, '$');
	int body_part = std::stoi(temp);

	getline(ss, temp, '$');
	std::string model = temp.c_str();

	getline(ss, temp, '$');
	std::string texName = temp.c_str();

	CPlayerPed* player = FindPlayerPed();

	if (texName == "cutoffchinosblue")
	{
		player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-697413025, 744365350, body_part);
	}
	else
	{
		if (texName == "sneakerbincblue")
		{
			player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-915574819, 2099005073, body_part);
		}
		else
		{
			if (texName == "12myfac")
			{
				player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-1750049245, 1393983095, body_part);
			}
			else
			{
				player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(texName.c_str(), model.c_str(), body_part);
			}
		}
	}
	CClothes::RebuildPlayer(player, false);
}
#endif

#ifdef GTASA
void Player::ChangePlayerModel(std::string& model)
{
	bool custom_skin = std::find(m_CustomSkins::m_List.begin(), m_CustomSkins::m_List.end(), model) !=
		m_CustomSkins::m_List.end();

	if (Ped::m_PedData.m_pJson->m_Data.contains(model) || custom_skin)
	{
		CPlayerPed* player = FindPlayerPed();
		if (Ped::m_SpecialPedJson.m_Data.contains(model) || custom_skin)
		{
			std::string name;
			if (Ped::m_SpecialPedJson.m_Data.contains(model))
				name = Ped::m_SpecialPedJson.m_Data[model].get<std::string>().c_str();
			else
				name = model;

			CStreaming::RequestSpecialChar(1, name.c_str(), PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(true);

			player->SetModelIndex(291);
			
			CStreaming::SetSpecialCharIsDeletable(291);
		}
		else
		{
			int imodel = std::stoi(model);

			CStreaming::RequestModel(imodel, eStreamingFlags::PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);
			player->SetModelIndex(imodel);
			CStreaming::SetModelIsDeletable(imodel);
		}
		Util::ClearCharTasksVehCheck(player);
	}
}
#else // GTA3 & GTAVC
void Player::ChangePlayerModel(std::string& cat, std::string& key, std::string& val)
{
	CPlayerPed* player = FindPlayerPed();

#ifdef GTAVC
	player->Undress(val.c_str());
	CStreaming::LoadAllRequestedModels(false);
	player->Dress();
#else // GTA3
	if (cat == "Special")
	{
		// CStreaming::RequestSpecialChar(109, val.c_str(), PRIORITY_REQUEST);
		// CStreaming::LoadAllRequestedModels(true);
		// player->SetModelIndex(109);
		// CStreaming::SetMissionDoesntRequireSpecialChar(109);
		SetHelpMessage("Spawning special peds isn't implemented yet.", false, false, false);
	}
	else
	{
		int imodel = std::stoi(val);
		CStreaming::RequestModel(imodel, eStreamingFlags::PRIORITY_REQUEST);
		CStreaming::LoadAllRequestedModels(true);
		player->DeleteRwObject();
		player->SetModelIndex(imodel);
		CStreaming::SetModelIsDeletable(imodel);
	}
#endif
}
#endif

void Player::Draw()
{
	CPlayerPed* pPlayer = FindPlayerPed();
	int hplayer = CPools::GetPedRef(pPlayer);
#ifdef GTASA
	CPad* pad = pPlayer->GetPadFromPlayer();
#endif
	CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];

	if (ImGui::Button("Copy coordinates", ImVec2(Ui::GetSize(2))))
	{
		CVector pos = pPlayer->GetPosition();
		std::string text = std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);

		ImGui::SetClipboardText(text.c_str());
		SetHelpMessage("Coordinates copied", false, false, false);
	}
	ImGui::SameLine();
	if (ImGui::Button("Suicide", ImVec2(Ui::GetSize(2))))
	{
		pPlayer->m_fHealth = 0.0;
	}

	ImGui::Spacing();

	if (ImGui::BeginTabBar("Player", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();

			ImGui::BeginChild("CheckboxesChild");

			ImGui::Columns(2, 0, false);

#ifdef GTASA
			Ui::CheckboxAddress("Bounty on yourself", 0x96913F);	

			ImGui::BeginDisabled(m_TopDownCamera::m_bEnabled);
			if (Ui::CheckboxWithHint("Drunk effect", &m_bDrunkEffect))
			{
				if (!m_bDrunkEffect)
				{
					Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 0);
				}
			}
			if (Ui::CheckboxWithHint("Fast Sprint", &m_bFastSprint, "Best to enable God Mode & Infinite sprint too"))
			{
				if(m_bFastSprint)
				{
					patch::Set<float>(0x8D2458, 0.1f);
				}
				else
				{
					patch::Set<float>(0x8D2458, 5.0f);
				}
			}
			ImGui::EndDisabled();
#endif
			Ui::CheckboxAddress("Free healthcare", BY_GAME((int)&pInfo->m_bFreeHealthCare, 
							(int)&pInfo->m_bFreeHealthCare, (int)&pInfo->m_bGetOutOfHospitalFree));

			if (Ui::CheckboxWithHint("God mode", &m_bGodMode))
			{
#ifdef GTASA
				patch::Set<bool>(0x96916D, m_bGodMode, false);
				pPlayer->m_nPhysicalFlags.bBulletProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bCollisionProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bExplosionProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bFireProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bMeeleProof = m_bGodMode;
#elif GTAVC
				pPlayer->m_nFlags.bBulletProof = m_bGodMode;
				pPlayer->m_nFlags.bCollisionProof = m_bGodMode;
				pPlayer->m_nFlags.bExplosionProof = m_bGodMode;
				pPlayer->m_nFlags.bFireProof = m_bGodMode;
				pPlayer->m_nFlags.bMeleeProof = m_bGodMode;
#else // GTA3
				pPlayer->m_nEntityFlags.bBulletProof = m_bGodMode;
				pPlayer->m_nEntityFlags.bCollisionProof = m_bGodMode;
				pPlayer->m_nEntityFlags.bExplosionProof = m_bGodMode;
				pPlayer->m_nEntityFlags.bFireProof = m_bGodMode;
				pPlayer->m_nEntityFlags.bMeleeProof = m_bGodMode;
#endif
			}
#ifdef GTASA
			Ui::CheckboxAddress("Higher cycle jumps", 0x969161);
			Ui::CheckboxAddress("Infinite oxygen", 0x96916E);
			if (Ui::CheckboxBitFlag("Invisible player", pPlayer->m_nPedFlags.bDontRender))
			{
				pPlayer->m_nPedFlags.bDontRender = (pPlayer->m_nPedFlags.bDontRender == 1) ? 0 : 1;
			}
			Ui::CheckboxAddress("Infinite sprint", 0xB7CEE4);
#else // GTA3 & GTAVC
			Ui::CheckboxAddress("Infinite sprint", BY_GAME(NULL, (int)&pInfo->m_bNeverGetsTired, (int)&pInfo->m_bInfiniteSprint));
#endif

			ImGui::NextColumn();

#ifdef GTASA
			if (Ui::CheckboxBitFlag("Lock control", pad->bPlayerSafe))
			{
				pad->bPlayerSafe = (pad->bPlayerSafe == 1) ? 0 : 1;
			}
			Ui::CheckboxAddressEx("Max sex appeal", 0x969180, 1, 0);
			Ui::CheckboxAddress("Mega jump", 0x96916C);
			Ui::CheckboxAddress("Mega punch", 0x969173);
			Ui::CheckboxAddress("Never get hungry", 0x969174);

			bool never_wanted = patch::Get<bool>(0x969171, false);
			if (Ui::CheckboxWithHint("Never wanted", &never_wanted))
			{
				CCheat::NotWantedCheat();
			}
#else // GTA3 & GTAVC
			static bool neverWanted = false;
			if (Ui::CheckboxWithHint("Never wanted", &neverWanted))
			{
				if (neverWanted)
				{
#ifdef GTA3
					pPlayer->m_pWanted->SetWantedLevel(0);
#else
					pPlayer->m_pWanted->CheatWantedLevel(0);
#endif
					pPlayer->m_pWanted->Update();
					patch::SetRaw(BY_GAME(NULL, 0x4D2110, 0x4AD900), (char*)"\xC3\x90\x90\x90\x90\x90", 6); // CWanted::UpdateWantedLevel()
					patch::Nop(BY_GAME(NULL, 0x5373D0, 0x4EFE73), 5); // CWanted::Update();
				}
				else
				{
					pPlayer->m_pWanted->ClearQdCrimes();
#ifdef GTA3
					pPlayer->m_pWanted->SetWantedLevel(0);
					patch::SetRaw(0x4AD900, (char*)"\xA1\x18\x77\x5F\x00", 6);
					patch::SetRaw(0x4EFE73, (char*)"\xE8\x38\xD9\xFB\xFF", 5);
#else
					pPlayer->m_pWanted->CheatWantedLevel(0);
					patch::SetRaw(0x4D2110, (char*)"\x8B\x15\xDC\x10\x69\x00", 6);
					patch::SetRaw(0x5373D0, (char*)"\xE8\x8B\xAE\xF9\xFF", 5);
#endif
				}
			}
#endif
			Ui::CheckboxAddress("No arrest fee", (int)&pInfo->m_bGetOutOfJailFree);
			Ui::CheckboxWithHint("Respawn die location", &m_KeepPosition::m_bEnabled, "Respawn to the location you died from");

			ImGui::Columns(1);

			ImGui::NewLine();
			ImGui::TextWrapped("Player flags,");

			ImGui::Columns(2, 0, false);

			bool state = BY_GAME(pPlayer->m_nPhysicalFlags.bBulletProof, pPlayer->m_nFlags.bBulletProof,
							pPlayer->m_nEntityFlags.bBulletProof);
			if (Ui::CheckboxWithHint("Bullet proof", &state, nullptr, m_bGodMode))
			{
				BY_GAME(pPlayer->m_nPhysicalFlags.bBulletProof, pPlayer->m_nFlags.bBulletProof,
							pPlayer->m_nEntityFlags.bBulletProof) = state;
			}

			state = BY_GAME(pPlayer->m_nPhysicalFlags.bCollisionProof, pPlayer->m_nFlags.bCollisionProof,
							pPlayer->m_nEntityFlags.bCollisionProof);
			if (Ui::CheckboxWithHint("Collision proof", &state, nullptr, m_bGodMode))
			{
				BY_GAME(pPlayer->m_nPhysicalFlags.bCollisionProof, pPlayer->m_nFlags.bCollisionProof,
							pPlayer->m_nEntityFlags.bCollisionProof) = state;
			}

			state = BY_GAME(pPlayer->m_nPhysicalFlags.bExplosionProof, pPlayer->m_nFlags.bExplosionProof,
							pPlayer->m_nEntityFlags.bExplosionProof);
			if (Ui::CheckboxWithHint("Explosion proof", &state, nullptr, m_bGodMode))
			{
				BY_GAME(pPlayer->m_nPhysicalFlags.bExplosionProof, pPlayer->m_nFlags.bExplosionProof,
							pPlayer->m_nEntityFlags.bExplosionProof) = state;
			}

			ImGui::NextColumn();

			state = BY_GAME(pPlayer->m_nPhysicalFlags.bFireProof, pPlayer->m_nFlags.bFireProof,
							pPlayer->m_nEntityFlags.bFireProof);
			if (Ui::CheckboxWithHint("Fire proof", &state, nullptr, m_bGodMode))
			{
				BY_GAME(pPlayer->m_nPhysicalFlags.bFireProof, pPlayer->m_nFlags.bFireProof,
							pPlayer->m_nEntityFlags.bFireProof) = state;
			}

			state = BY_GAME(pPlayer->m_nPhysicalFlags.bMeeleProof, pPlayer->m_nFlags.bMeleeProof,
							pPlayer->m_nEntityFlags.bMeleeProof);
			if (Ui::CheckboxWithHint("Meele proof", &state, nullptr, m_bGodMode))
			{
				BY_GAME(pPlayer->m_nPhysicalFlags.bMeeleProof, pPlayer->m_nFlags.bMeleeProof,
							pPlayer->m_nEntityFlags.bMeleeProof) = state;
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Menus"))
		{
			ImGui::BeginChild("PlayerMenus");

			Ui::EditReference("Armour", pPlayer->m_fArmour, 0, 100, 150);
#ifdef GTASA
			if (ImGui::CollapsingHeader("Body"))
			{
				if (pPlayer->m_nModelIndex == 0)
				{
					ImGui::Columns(3, 0, false);
					if (ImGui::RadioButton("Fat", &m_nUiBodyState, 2))
						CCheat::FatCheat();

					ImGui::NextColumn();

					if (ImGui::RadioButton("Muscle", &m_nUiBodyState, 1))
						CCheat::MuscleCheat();

					ImGui::NextColumn();

					if (ImGui::RadioButton("Skinny", &m_nUiBodyState, 0))
						CCheat::SkinnyCheat();

					ImGui::Columns(1);
				}
				else
				{
					ImGui::TextWrapped("You need to be in CJ skin.");
					ImGui::Spacing();

					if (ImGui::Button("Change to CJ skin", ImVec2(Ui::GetSize(1))))
					{
						pPlayer->SetModelIndex(0);
						Util::ClearCharTasksVehCheck(pPlayer);
					}
				}
				ImGui::Spacing();
				ImGui::Separator();
			}

			Ui::EditStat("Energy", STAT_ENERGY);
			Ui::EditStat("Fat", STAT_FAT);
#endif
			Ui::EditReference("Health", pPlayer->m_fHealth, 0, 100, BY_GAME(static_cast<int>(pPlayer->m_fMaxHealth), 100, 100));
#ifdef GTASA
			Ui::EditStat("Lung capacity", STAT_LUNG_CAPACITY);
			Ui::EditStat("Max health", STAT_MAX_HEALTH, 0, 569, 1450);
			Ui::EditAddress<int>("Money", 0xB7CE50, -99999999, 0, 99999999);
#else // GTA3 & GTAVC
			int money = pInfo->m_nMoney;
			Ui::EditAddress<int>("Money", (int)&money, -9999999, 0, 99999999);
			pInfo->m_nMoney = money;
			pInfo->m_nDisplayMoney = money;
#endif

			
#ifdef GTASA
			Ui::EditStat("Muscle", STAT_MUSCLE);
			Ui::EditStat("Respect", STAT_RESPECT);
			Ui::EditStat("Stamina", STAT_STAMINA);
			if (ImGui::CollapsingHeader("Top down camera"))
			{
				if (ImGui::Checkbox("Enabled", &m_TopDownCamera::m_bEnabled))
				{
					Command<Commands::RESTORE_CAMERA_JUMPCUT>();
				}
				ImGui::Spacing();
				ImGui::SliderFloat("Camera zoom", &m_TopDownCamera::m_fOffset, 20.0f, 60.0f);
				ImGui::Spacing();
				ImGui::Separator();
			}
#endif
			if (ImGui::CollapsingHeader("Wanted level"))
			{
#ifdef GTASA
				int val = pPlayer->m_pPlayerData->m_pWanted->m_nWantedLevel;
				int max_wl = pPlayer->m_pPlayerData->m_pWanted->MaximumWantedLevel;
				max_wl = max_wl < 6 ? 6 : max_wl;
#else // GTA3 & GTAVC
				int val = pPlayer->m_pWanted->m_nWantedLevel;
				int max_wl = 6;
#endif

				ImGui::Columns(3, 0, false);
				ImGui::Text("Min: 0");
				ImGui::NextColumn();
				ImGui::Text("Def: 0");
				ImGui::NextColumn();
				ImGui::Text("Max: %d", max_wl);
				ImGui::Columns(1);

				ImGui::Spacing();

				if (ImGui::InputInt("Set value##Wanted level", &val))
				{
#ifdef GTASA
					pPlayer->CheatWantedLevel(val);
#elif GTAVC
					pPlayer->m_pWanted->CheatWantedLevel(val);
#else // GTA3
					pPlayer->m_pWanted->SetWantedLevel(val);
#endif
				}

				ImGui::Spacing();
				if (ImGui::Button("Minimum##Wanted level", Ui::GetSize(3)))
				{
#ifdef GTASA
					pPlayer->CheatWantedLevel(0);
#elif GTAVC
					pPlayer->m_pWanted->CheatWantedLevel(0);
#else // GTA3
					pPlayer->m_pWanted->SetWantedLevel(0);
#endif
				}

				ImGui::SameLine();

				if (ImGui::Button("Default##Wanted level", Ui::GetSize(3)))
				{
#ifdef GTASA
					pPlayer->CheatWantedLevel(0);
#elif GTAVC
					pPlayer->m_pWanted->CheatWantedLevel(0);
#else // GTA3
					pPlayer->m_pWanted->SetWantedLevel(0);
#endif
				}

				ImGui::SameLine();

				if (ImGui::Button("Maximum##Wanted level", Ui::GetSize(3)))
				{
#ifdef GTASA
					pPlayer->CheatWantedLevel(max_wl);
#elif GTAVC
					pPlayer->m_pWanted->CheatWantedLevel(max_wl);
#else // GTA3
					pPlayer->m_pWanted->SetWantedLevel(max_wl);
#endif
				}

				ImGui::Spacing();
				ImGui::Separator();
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}

#ifdef GTASA
		if (ImGui::BeginTabItem("Appearance"))
		{
			ImGui::Spacing();

			if (Ui::CheckboxWithHint("Aim skin changer", &m_bAimSkinChanger,
				(("Changes to the ped, player is targeting with a weapon.\nTo use aim a ped with a weapon and press ")
					+ aimSkinChanger.Pressed())))
				gConfig.SetValue("aim_skin_changer", m_bAimSkinChanger);
			if (ImGui::BeginTabBar("AppearanceTabBar"))
			{
				if (ImGui::BeginTabItem("Clothes"))
				{
					static int bClothOption = 0;
					ImGui::RadioButton("Add", &bClothOption, 0);
					ImGui::SameLine();
					ImGui::RadioButton("Remove", &bClothOption, 1);
					ImGui::Spacing();

					if (pPlayer->m_nModelIndex == 0)
					{
						if (bClothOption == 0)
						{
							Ui::DrawImages(m_ClothData, ChangePlayerCloth, nullptr,
									   [](std::string str)
									   {
										   std::stringstream ss(str);
										   std::string temp;

										   getline(ss, temp, '$');
										   getline(ss, temp, '$');

										   return temp;
									   }, nullptr, cloth_category, sizeof(cloth_category) / sizeof(const char*));
						}
						else
						{
							size_t count = 0;

							if (ImGui::Button("Remove all", ImVec2(Ui::GetSize(2))))
							{
								CPlayerPed* player = FindPlayerPed();
								for (uint i = 0; i < 18; i++)
								{
									player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(0u, 0u, i);
								}
								CClothes::RebuildPlayer(player, false);
							}
							ImGui::SameLine();
							for (const char* clothName : cloth_category)
							{
								if (ImGui::Button(clothName, ImVec2(Ui::GetSize(2))))
								{
									CPlayerPed* player = FindPlayerPed();
									player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(0u, 0u, count);
									CClothes::RebuildPlayer(player, false);
								}

								if (count % 2 != 0)
								{
									ImGui::SameLine();
								}
								++count;
							}

							ImGui::Spacing();
							ImGui::TextWrapped("If CJ is wearing a full suit, click 'Extras/ Remove all' to remove it.");
						}
					}
					else
					{
						ImGui::TextWrapped("You need to be in CJ skin.");
						ImGui::Spacing();

						if (ImGui::Button("Change to CJ skin", ImVec2(Ui::GetSize(1))))
						{
							pPlayer->SetModelIndex(0);
							Util::ClearCharTasksVehCheck(pPlayer);
						}
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Ped skins"))
				{
					Ui::DrawImages(Ped::m_PedData, ChangePlayerModel, nullptr,
								   [](std::string str) { return Ped::m_PedData.m_pJson->m_Data[str].get<std::string>(); });
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Custom skins"))
				{
					ImGui::Spacing();

					if (m_bModloaderInstalled)
					{
						Ui::FilterWithHint("Search", m_ClothData.m_Filter,
										   std::string("Total skins: " + std::to_string(m_CustomSkins::m_List.size()))
										   .c_str());
						Ui::ShowTooltip("Place your dff & txd files inside 'modloader/Custom Skins'");
						ImGui::Spacing();
						ImGui::TextWrapped(
							"Note: Your txd & dff names can't exceed 8 characters. Don't change names while the game is running.");
						ImGui::Spacing();
						for (std::string name : m_CustomSkins::m_List)
						{
							if (m_CustomSkins::m_Filter.PassFilter(name.c_str()))
							{
								if (ImGui::MenuItem(name.c_str()))
								{
									ChangePlayerModel(name);
								}
							}
						}
					}
					else
					{
						ImGui::TextWrapped(
							"Custom skin allows to change player skins without replacing any existing game ped skins.\n\
Steps to enable 'Custom Skins',\n\n\
1. Download & install modloader\n\
2. Create a folder inside 'modloader' folder with the name 'Custom Skins'\n\
3. Download ped skins online ( .dff & .txd files) and put them inside.\n\
4. Restart your game.\n\n\n\
Limitations:\n\
1. Your .dff & .txd file names must not exceed 8 characters.\n\
2. Do not rename them while the game is running\n\
\nDoing so will crash your game.");
						ImGui::Spacing();
						if (ImGui::Button("Download Modloader", ImVec2(Ui::GetSize(1))))
							ShellExecute(NULL, "open", "https://gtaforums.com/topic/669520-mod-loader/", NULL, NULL,
										 SW_SHOWNORMAL);
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}
#else // GTA3 & GTA
		if (ImGui::BeginTabItem("Skins"))
		{
			ImGui::Spacing();
#ifdef GTA3
			ImGui::TextWrapped("Player must be frozen to change skins.");
			CPad::GetPad(0)->m_bDisablePlayerControls = true;
#else
			ImGui::TextWrapped("Only contains the skins that works.");
#endif
			Ui::DrawJSON(skinData, ChangePlayerModel, nullptr);
			ImGui::EndTabItem();
		}
#endif
		ImGui::EndTabBar();
	}
}
