#include "pch.h"
#include "Player.h"
#include "Ped.h"
#include "Menu.h"
#include "Ui.h"
#include "Util.h"

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

inline static void PlayerModelBrokenFix()
{
	CPlayerPed* pPlayer = FindPlayerPed();

	if (pPlayer->m_nModelIndex == 0)
	{
		Call<0x5A81E0>(0, pPlayer->m_pPlayerData->m_pPedClothesDesc, 0xBC1C78, false);
	}
}

Player::Player()
{
	//	Fix player model being broken after rebuild
	patch::RedirectCall(0x5A834D, &PlayerModelBrokenFix);

	m_bAimSkinChanger = config.GetValue("aim_skin_changer", false);

	// Custom skins setup
	if (GetModuleHandle("modloader.asi"))
	{
		if (fs::is_directory(m_CustomSkins::m_Path))
		{
			for (auto& p : fs::recursive_directory_iterator(m_CustomSkins::m_Path))
			{
				if (p.path().extension() == ".dff")
				{
					std::string file_name = p.path().stem().string();

					if (file_name.size() < 9)
					{
						m_CustomSkins::m_List.push_back(file_name);
					}
					else
					{
						flog << "Custom Skin longer than 8 characters " << file_name << std::endl;
					}
				}
			}
		}
		else 
		{
			fs::create_directory(m_CustomSkins::m_Path);
		}

		m_bModloaderInstalled = true;
	}

	Events::processScriptsEvent += []
	{
		uint timer = CTimer::m_snTimeInMilliseconds;
		static CPlayerPed* player = FindPlayerPed();

		if (!m_bImagesLoaded)
		{
			Util::LoadTextureDirectory(m_ClothData, PLUGIN_PATH((char*)"CheatMenu\\clothes.txd"), true);
			m_bImagesLoaded = true;
		}

		if (m_KeepPosition::m_bEnabled)
		{
			if (!player->IsAlive())
			{
				m_KeepPosition::m_fPos = player->GetPosition();
			}
			else
			{
				CVector cur_pos = player->GetPosition();

				if (m_KeepPosition::m_fPos.x != 0 && m_KeepPosition::m_fPos.x != cur_pos.x
					&& m_KeepPosition::m_fPos.y != 0 && m_KeepPosition::m_fPos.y != cur_pos.y)
				{
					player->Teleport(m_KeepPosition::m_fPos, false);
					m_KeepPosition::m_fPos = CVector(0, 0, 0);
				}
			}
		}

		if (m_bGodMode)
		{
			patch::Set<bool>(0x96916D, 1, false);
			player->m_nPhysicalFlags.bBulletProof = 1;
			player->m_nPhysicalFlags.bCollisionProof = 1;
			player->m_nPhysicalFlags.bExplosionProof = 1;
			player->m_nPhysicalFlags.bFireProof = 1;
			player->m_nPhysicalFlags.bMeeleProof = 1;
		}

		if (m_bAimSkinChanger && Ui::HotKeyPressed(Menu::m_HotKeys::aimSkinChanger))
		{
			CPed* target_ped = player->m_pPlayerTargettedPed;
			if (target_ped)
			{
				player->SetModelIndex(target_ped->m_nModelIndex);
				Util::ClearCharTasksVehCheck(player);
			}
		}

		if (Ui::HotKeyPressed(Menu::m_HotKeys::godMode))
		{
			if (m_bGodMode)
			{
				CHud::SetHelpMessage("God mode disabled", false, false, false);

				patch::Set<bool>(0x96916D, m_bGodMode, false);
				player->m_nPhysicalFlags.bBulletProof = false;
				player->m_nPhysicalFlags.bCollisionProof = false;
				player->m_nPhysicalFlags.bExplosionProof = false;
				player->m_nPhysicalFlags.bFireProof = false;
				player->m_nPhysicalFlags.bMeeleProof = false;
				m_bGodMode = false;
			}
			else
			{
				CHud::SetHelpMessage("God mode enabled", false, false, false);
				m_bGodMode = true;
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
			{
				player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-1750049245, 1393983095, body_part);
			}
			else
			{
				player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(texture9.c_str(), model.c_str(), body_part);
			}
		}
	}
	CClothes::RebuildPlayer(player, false);
}

void Player::ChangePlayerModel(std::string& model)
{
	bool custom_skin = std::find(m_CustomSkins::m_List.begin(), m_CustomSkins::m_List.end(), model) !=
		m_CustomSkins::m_List.end();
	if (Ped::m_PedData.m_Json.m_Data.contains(model) || custom_skin)
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
	}
}

void Player::Draw()
{
	CPlayerPed* pPlayer = FindPlayerPed();
	int hplayer = CPools::GetPedRef(pPlayer);
	CPad* pad = pPlayer->GetPadFromPlayer();

	if (ImGui::Button("Copy coordinates", ImVec2(Ui::GetSize(2))))
	{
		CVector pos = pPlayer->GetPosition();
		std::string text = std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);

		ImGui::SetClipboardText(text.c_str());
		CHud::SetHelpMessage("Coordinates copied", false, false, false);
	}
	ImGui::SameLine();
	if (ImGui::Button("Suicide", ImVec2(Ui::GetSize(2))))
		pPlayer->m_fHealth = 0.0;

	ImGui::Spacing();

	if (ImGui::BeginTabBar("Player", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();

			ImGui::BeginChild("CheckboxesChild");

			ImGui::Columns(2, 0, false);

			Ui::CheckboxAddress("Bounty on yourself", 0x96913F);
			CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];

			Ui::CheckboxAddress("Free healthcare", (int)&pInfo->m_bFreeHealthCare);
			if (Ui::CheckboxWithHint("God mode", &m_bGodMode))
			{
				patch::Set<bool>(0x96916D, m_bGodMode, false);
				pPlayer->m_nPhysicalFlags.bBulletProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bCollisionProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bExplosionProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bFireProof = m_bGodMode;
				pPlayer->m_nPhysicalFlags.bMeeleProof = m_bGodMode;
			}

			Ui::CheckboxAddress("Higher cycle jumps", 0x969161);
			Ui::CheckboxAddress("Infinite oxygen", 0x96916E);
			Ui::CheckboxAddress("Infinite run", 0xB7CEE4);
			if (Ui::CheckboxBitFlag("Invisible player", pPlayer->m_nPedFlags.bDontRender))
				pPlayer->m_nPedFlags.bDontRender = (pPlayer->m_nPedFlags.bDontRender == 1) ? 0 : 1;

			ImGui::NextColumn();

			Ui::CheckboxWithHint("Keep position", &m_KeepPosition::m_bEnabled, "Teleport to the position you died from");
			if (Ui::CheckboxBitFlag("Lock control", pad->bPlayerSafe))
			{
				pad->bPlayerSafe = (pad->bPlayerSafe == 1) ? 0 : 1;
			}

			Ui::CheckboxAddress("Mega jump", 0x96916C);
			Ui::CheckboxAddress("Mega punch", 0x969173);
			Ui::CheckboxAddress("Never get hungry", 0x969174);
			Ui::CheckboxAddress("No arrest fee", (int)&pInfo->m_bGetOutOfJailFree);
			
			bool never_wanted = patch::Get<bool>(0x969171, false);
			if (Ui::CheckboxWithHint("Never wanted", &never_wanted))
			{
				CCheat::NotWantedCheat();
			}

			ImGui::Columns(1);

			ImGui::NewLine();
			ImGui::TextWrapped("Player flags,");

			ImGui::Columns(2, 0, false);

			bool state = pPlayer->m_nPhysicalFlags.bBulletProof;
			if (Ui::CheckboxWithHint("Bullet proof", &state, nullptr, m_bGodMode))
			{
				pPlayer->m_nPhysicalFlags.bBulletProof = state;
			}

			state = pPlayer->m_nPhysicalFlags.bCollisionProof;
			if (Ui::CheckboxWithHint("Collision proof", &state, nullptr, m_bGodMode))
			{
				pPlayer->m_nPhysicalFlags.bCollisionProof = state;
			}

			state = pPlayer->m_nPhysicalFlags.bExplosionProof;
			if (Ui::CheckboxWithHint("Explosion proof", &state, nullptr, m_bGodMode))
			{
				pPlayer->m_nPhysicalFlags.bExplosionProof = state;
			}

			ImGui::NextColumn();

			state = pPlayer->m_nPhysicalFlags.bFireProof;
			if (Ui::CheckboxWithHint("Fire proof", &state, nullptr, m_bGodMode))
			{
				pPlayer->m_nPhysicalFlags.bFireProof = state;
			}

			state = pPlayer->m_nPhysicalFlags.bMeeleProof;
			if (Ui::CheckboxWithHint("Meele proof", &state, nullptr, m_bGodMode))
			{
				pPlayer->m_nPhysicalFlags.bMeeleProof = state;
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Menus"))
		{
			ImGui::BeginChild("PlayerMenus");

			Ui::EditReference("Armour", pPlayer->m_fArmour, 0, 100, 150);
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
			Ui::EditReference("Health", pPlayer->m_fHealth, 0, 100, static_cast<int>(pPlayer->m_fMaxHealth));
			Ui::EditStat("Lung capacity", STAT_LUNG_CAPACITY);
			Ui::EditStat("Max health", STAT_MAX_HEALTH, 0, 569, 1450);
			Ui::EditAddress<int>("Money", 0xB7CE50, -99999999, 0, 99999999);
			Ui::EditStat("Muscle", STAT_MUSCLE);
			Ui::EditStat("Respect", STAT_RESPECT);
			Ui::EditStat("Stamina", STAT_STAMINA);
			if (ImGui::CollapsingHeader("Wanted level"))
			{
				int val = pPlayer->m_pPlayerData->m_pWanted->m_nWantedLevel;
				int max_wl = pPlayer->m_pPlayerData->m_pWanted->MaximumWantedLevel;
				max_wl = max_wl < 6 ? 6  : max_wl;

				ImGui::Columns(3, 0, false);
				ImGui::Text("Min: 0");
				ImGui::NextColumn();
				ImGui::Text("Def: 0");
				ImGui::NextColumn();
				ImGui::Text("Max: %d", max_wl);
				ImGui::Columns(1);

				ImGui::Spacing();

				if (ImGui::InputInt("Set value##Wanted level", &val))
					pPlayer->CheatWantedLevel(val);

				ImGui::Spacing();
				if (ImGui::Button("Minimum##Wanted level", Ui::GetSize(3)))
					pPlayer->CheatWantedLevel(0);

				ImGui::SameLine();

				if (ImGui::Button("Default##Wanted level", Ui::GetSize(3)))
					pPlayer->CheatWantedLevel(0);

				ImGui::SameLine();

				if (ImGui::Button("Maximum##Wanted level", Ui::GetSize(3)))
					pPlayer->CheatWantedLevel(max_wl);

				ImGui::Spacing();
				ImGui::Separator();
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Appearance"))
		{
			ImGui::Spacing();
			if (Ui::CheckboxWithHint("Aim skin changer", &m_bAimSkinChanger,
			                         (("Changes to the ped, player is targeting with a weapon.\nTo use aim a ped with a weapon and press ")
										+ Ui::GetHotKeyNameString(Menu::m_HotKeys::aimSkinChanger)).c_str()))
				config.SetValue("aim_skin_changer", m_bAimSkinChanger);

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
							Ui::DrawImages(m_ClothData.m_ImagesList, ImVec2(70, 100), m_ClothData.m_Categories, m_ClothData.m_Selected,
						               m_ClothData.m_Filter, ChangePlayerCloth, nullptr,
						               [](std::string str)
						               {
							               std::stringstream ss(str);
							               std::string temp;

							               getline(ss, temp, '$');
							               getline(ss, temp, '$');

							               return temp;
						               }, nullptr, cloth_category, sizeof(cloth_category)/ sizeof(const char*));
						}
						else
						{
							size_t count = 0;

							if(ImGui::Button("Remove all", ImVec2(Ui::GetSize(2))))
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
								if(ImGui::Button(clothName, ImVec2(Ui::GetSize(2))))
								{
									CPlayerPed* player = FindPlayerPed();
									player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(0u, 0u, count);
									CClothes::RebuildPlayer(player, false);
								}

								if (count %2 != 0)
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
					Ui::DrawImages(Ped::m_PedData.m_ImagesList, ImVec2(65, 110), Ped::m_PedData.m_Categories,
					               Ped::m_PedData.m_Selected, Ped::m_PedData.m_Filter, ChangePlayerModel, nullptr,
					               [](std::string str) { return Ped::m_PedData.m_Json.m_Data[str].get<std::string>(); });
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

		ImGui::EndTabBar();
	}
}
