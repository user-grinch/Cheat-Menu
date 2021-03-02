#include "pch.h"
#include "Player.h"
#include "Ped.h"
#include "Menu.h"
#include "Ui.h"
#include "Util.h"

inline static void PlayerModelBrokenFix()
{
	CPlayerPed* player = FindPlayerPed();

	if (player->m_nModelIndex == 0)
		Call<0x5A81E0>(0, player->m_pPlayerData->m_pPedClothesDesc, 0xBC1C78, false);
}

Player::Player()
{
	//	Fix player model being broken after rebuild
	patch::RedirectCall(0x5A834D, &PlayerModelBrokenFix);

	aim_skin_changer = config.GetValue("aim_skin_changer", false);

	// Custom skins setup
	if (GetModuleHandle("modloader.asi"))
	{
		if (fs::is_directory(custom_skins::dir))
		{
			for (auto& p : fs::recursive_directory_iterator(custom_skins::dir))
			{
				if (p.path().extension() == ".dff")
				{
					std::string file_name = p.path().stem().string();

					if (file_name.size() < 9)
						custom_skins::store_vec.push_back(file_name);
					else
						flog << "Custom Skin longer than 8 characters " << file_name << std::endl;
				}
			}
		}
		else fs::create_directory(custom_skins::dir);

		modloader_installed = true;
	}

	Events::processScriptsEvent += []
	{
		uint timer = CTimer::m_snTimeInMilliseconds;
		static CPlayerPed* player = FindPlayerPed();

		if (!images_loaded)
		{
			Util::LoadTexturesInDirRecursive(PLUGIN_PATH((char*)"CheatMenu\\clothes\\"), ".jpg", search_categories, clothes_vec);
			images_loaded = true;
		}

		if (keep_position::state)
		{
			if (!player->IsAlive())
			{
				keep_position::pos = player->GetPosition();
			}
			else
			{
				CVector cur_pos = player->GetPosition();

				if (keep_position::pos.x != 0 && keep_position::pos.x != cur_pos.x
				 && keep_position::pos.y != 0 && keep_position::pos.y != cur_pos.y)
				{
					player->Teleport(keep_position::pos, false);
					keep_position::pos = CVector(0, 0, 0);
				}
			}
		}

		if (god_mode)
		{
			patch::Set<bool>(0x96916D, 1, false);
			player->m_nPhysicalFlags.bBulletProof = 1;
			player->m_nPhysicalFlags.bCollisionProof = 1;
			player->m_nPhysicalFlags.bExplosionProof = 1;
			player->m_nPhysicalFlags.bFireProof = 1;
			player->m_nPhysicalFlags.bMeeleProof = 1;
		}

		if (aim_skin_changer && Ui::HotKeyPressed(Menu::hotkeys::aim_skin_changer))
		{
			CPed* target_ped = player->m_pPlayerTargettedPed;
			if (target_ped)
			{
				player->SetModelIndex(target_ped->m_nModelIndex);
				Util::ClearCharTasksVehCheck(player);
			}
		}

		if (Ui::HotKeyPressed(Menu::hotkeys::god_mode))
		{
			if (god_mode)
			{
				CHud::SetHelpMessage("God mode disabled", false, false, false);

				patch::Set<bool>(0x96916D, god_mode, false);
				player->m_nPhysicalFlags.bBulletProof = false;
				player->m_nPhysicalFlags.bCollisionProof = false;
				player->m_nPhysicalFlags.bExplosionProof = false;
				player->m_nPhysicalFlags.bFireProof = false;
				player->m_nPhysicalFlags.bMeeleProof = false;
				god_mode = false;
			}
			else
			{
				CHud::SetHelpMessage("God mode enabled", false, false, false);
				god_mode = true;
			}
		}
	};
}

void Player::ChangePlayerCloth(std::string& name)
{
	std::stringstream ss(name);
	std::string temp;

	getline(ss, temp, '$');
	int body_part = std::stoi(temp);

	getline(ss, temp, '$');
	std::string model = temp.c_str();

	getline(ss, temp, '$');
	std::string texture9 = temp.c_str();

	CPlayerPed* player = FindPlayerPed();

	if (texture9 == "cutoffchinosblue")
	{
		player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-697413025, 744365350, body_part);
	}
	else
	{
		if (texture9 == "sneakerbincblue")
		{
			player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-915574819, 2099005073, body_part);
		}
		else
		{
			if (texture9 == "12myfac")
				player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-1750049245, 1393983095, body_part);
			else
				player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(texture9.c_str(), model.c_str(), body_part);
		}
	}
	CClothes::RebuildPlayer(player, false);
}

void Player::ChangePlayerModel(std::string& model)
{
	bool custom_skin = std::find(custom_skins::store_vec.begin(), custom_skins::store_vec.end(), model) != custom_skins::store_vec.end();
	if (Ped::ped_json.data.contains(model) || custom_skin)
	{
		CPlayerPed* player = FindPlayerPed();
		if (Ped::pedspecial_json.data.contains(model) || custom_skin)
		{
			std::string name;
			if (Ped::pedspecial_json.data.contains(model))
				name = Ped::pedspecial_json.data[model].get<std::string>().c_str();
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
	}
}

void Player::Draw()
{
	static CPlayerPed* player = FindPlayerPed();
	static int hplayer = CPools::GetPedRef(player);
	static CPad* pad = player->GetPadFromPlayer();

	if (ImGui::Button("Copy coordinates", ImVec2(Ui::GetSize(2))))
	{
		CVector pos = player->GetPosition();
		std::string text = std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);

		ImGui::SetClipboardText(text.c_str());
		CHud::SetHelpMessage("Coordinates copied", false, false, false);
	}
	ImGui::SameLine();
	if (ImGui::Button("Suicide", ImVec2(Ui::GetSize(2))))
		player->m_fHealth = 0.0;

	ImGui::Spacing();

	if (ImGui::BeginTabBar("Player", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();

			ImGui::BeginChild("CheckboxesChild");

			ImGui::Columns(2, 0, false);

			Ui::CheckboxAddress("Bounty on yourself", 0x96913F);
			if (Ui::CheckboxWithHint("God mode", &god_mode))
			{
				patch::Set<bool>(0x96916D, god_mode, false);
				player->m_nPhysicalFlags.bBulletProof = god_mode;
				player->m_nPhysicalFlags.bCollisionProof = god_mode;
				player->m_nPhysicalFlags.bExplosionProof = god_mode;
				player->m_nPhysicalFlags.bFireProof = god_mode;
				player->m_nPhysicalFlags.bMeeleProof = god_mode;
			}

			Ui::CheckboxAddress("Higher cycle jumps", 0x969161);
			Ui::CheckboxAddress("Infinite oxygen", 0x96916E);
			Ui::CheckboxAddress("Infinite run", 0xB7CEE4);
			if (Ui::CheckboxBitFlag("Invisible player", player->m_nPedFlags.bDontRender))
				player->m_nPedFlags.bDontRender = (player->m_nPedFlags.bDontRender == 1) ? 0 : 1;

			ImGui::NextColumn();

			Ui::CheckboxWithHint("Keep position", &keep_position::state, "Teleport to the position you died from");
			if (Ui::CheckboxBitFlag("Lock control", pad->bPlayerSafe))
				pad->bPlayerSafe = (pad->bPlayerSafe == 1) ? 0 : 1;

			Ui::CheckboxAddress("Mega jump", 0x96916C);
			Ui::CheckboxAddress("Mega punch", 0x969173);
			Ui::CheckboxAddress("Never get hungry", 0x969174);

			bool never_wanted = patch::Get<bool>(0x969171, false);
			if (Ui::CheckboxWithHint("Never wanted", &never_wanted))
				CCheat::NotWantedCheat();

			ImGui::Columns(1);

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Menus"))
		{
			ImGui::BeginChild("PlayerMenus");

			Ui::EditReference("Armour", player->m_fArmour, 0, 100, 150);
			if (ImGui::CollapsingHeader("Body"))
			{
				if (player->m_nModelIndex == 0)
				{
					ImGui::Columns(3, 0, false);
					if (ImGui::RadioButton("Fat", &body, 2))
						CCheat::FatCheat();

					ImGui::NextColumn();

					if (ImGui::RadioButton("Muscle", &body, 1))
						CCheat::MuscleCheat();

					ImGui::NextColumn();

					if (ImGui::RadioButton("Skinny", &body, 0))
						CCheat::SkinnyCheat();

					ImGui::Columns(1);
				}
				else
				{
					ImGui::TextWrapped("You need to be in CJ skin.");
					ImGui::Spacing();

					if (ImGui::Button("Change to CJ skin", ImVec2(Ui::GetSize(1))))
					{
						player->SetModelIndex(0);
						Util::ClearCharTasksVehCheck(player);
					}
				}
				ImGui::Spacing();
				ImGui::Separator();
			}

			Ui::EditStat("Energy", STAT_ENERGY);
			Ui::EditStat("Fat", STAT_FAT);
			Ui::EditReference("Health", player->m_fHealth, 0, 100, static_cast<int>(player->m_fMaxHealth));
			Ui::EditStat("Lung capacity", STAT_LUNG_CAPACITY);
			Ui::EditStat("Max health", STAT_MAX_HEALTH, 0, 569, 1450);
			Ui::EditAddress<int>("Money", 0xB7CE50, -99999999, 0, 99999999);
			Ui::EditStat("Muscle", STAT_MUSCLE);
			Ui::EditStat("Respect", STAT_RESPECT);
			Ui::EditStat("Stamina", STAT_STAMINA);
			if (ImGui::CollapsingHeader("Wanted level"))
			{
				int val = player->m_pPlayerData->m_pWanted->m_nWantedLevel;
				int max_wl = player->m_pPlayerData->m_pWanted->MaximumWantedLevel;

				ImGui::Columns(3, 0, false);
				ImGui::Text("Min: 0");
				ImGui::NextColumn();
				ImGui::Text("Def: 0");
				ImGui::NextColumn();
				ImGui::Text("Max: %d", max_wl);
				ImGui::Columns(1);

				ImGui::Spacing();

				if (ImGui::InputInt("Set value##Wanted level", &val))
					player->CheatWantedLevel(val);

				ImGui::Spacing();
				if (ImGui::Button("Minimum##Wanted level", Ui::GetSize(3)))
					player->CheatWantedLevel(0);

				ImGui::SameLine();

				if (ImGui::Button("Default##Wanted level", Ui::GetSize(3)))
					player->CheatWantedLevel(0);

				ImGui::SameLine();

				if (ImGui::Button("Maximum##Wanted level", Ui::GetSize(3)))
					player->CheatWantedLevel(max_wl);

				ImGui::Spacing();
				ImGui::Separator();
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Appearance"))
		{
			ImGui::Spacing();
			if (Ui::CheckboxWithHint("Aim skin changer", &aim_skin_changer, (("Activate using Aim ped + ") + Ui::GetHotKeyNameString(Menu::hotkeys::aim_skin_changer)).c_str()))
				config.SetValue("aim_skin_changer", aim_skin_changer);

			if (ImGui::BeginTabBar("AppearanceTabBar"))
			{
				if (ImGui::BeginTabItem("Clothes"))
				{
					ImGui::Spacing();

					if (player->m_nModelIndex == 0)
					{
						if (ImGui::Button("Remove clothes", ImVec2(Ui::GetSize())))
						{
							CPlayerPed* player = FindPlayerPed();

							int temp = 0;
							for (uint i = 0; i < 18; i++)
								player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(temp, temp, i);

							CClothes::RebuildPlayer(player, false);
						}
						ImGui::Spacing();

						Ui::DrawImages(clothes_vec, ImVec2(70, 100), search_categories, selected_item, filter, ChangePlayerCloth, nullptr,
							[](std::string str)
						{
							std::stringstream ss(str);
							std::string temp;

							getline(ss, temp, '$');
							getline(ss, temp, '$');

							return temp;
						});
					}
					else
					{
						ImGui::TextWrapped("You need to be in CJ skin.");
						ImGui::Spacing();

						if (ImGui::Button("Change to CJ skin", ImVec2(Ui::GetSize(1))))
						{
							player->SetModelIndex(0);
							Util::ClearCharTasksVehCheck(player);
						}
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Ped skins"))
				{
					Ui::DrawImages(Ped::peds_vec, ImVec2(65, 110), Ped::search_categories, Ped::selected_item, Ped::filter, ChangePlayerModel, nullptr,
						[](std::string str) {return Ped::ped_json.data[str].get<std::string>(); });
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Custom skins"))
				{
					ImGui::Spacing();

					if (modloader_installed)
					{
						Ui::FilterWithHint("Search", filter, std::string("Total skins: " + std::to_string(custom_skins::store_vec.size())).c_str());
						Ui::ShowTooltip("Place your dff & txd files inside 'modloader/Custom Skins'");
						ImGui::Spacing();
						ImGui::TextWrapped("Note: Your txd & dff names can't exceed 8 characters. Don't change names while the game is running.");
						ImGui::Spacing();
						for (std::string name : custom_skins::store_vec)
						{
							if (custom_skins::filter.PassFilter(name.c_str()))
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
						ImGui::TextWrapped("Modloader is not installed. Please install modloader.");
						ImGui::Spacing();
						if (ImGui::Button("Download Modloader", ImVec2(Ui::GetSize(1))))
							ShellExecute(NULL, "open", "https://gtaforums.com/topic/669520-mod-loader/", NULL, NULL, SW_SHOWNORMAL);
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
