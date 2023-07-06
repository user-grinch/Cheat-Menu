#include "pch.h"
#include "player.h"
#include "menu.h"
#include "utils/widget.h"
#include "utils/util.h"
#include "kiero/minhook/MinHook.h"

#ifdef GTASA
#include "ped.h"
#include "custom/topdowncam_sa.h"
#include "custom/customskins_sa.h"

static inline const char* clothNameList[18] =
{
    "Shirts", "Heads", "Trousers", "Shoes", "Tattoos left lower arm", "Tattoos left upper arm",
    "Tattoos right upper arm", "Tattoos right lower arm", "Tattoos back", "Tattoos left chest",
    "Tattoos right chest", "Tattoos stomach", "Tattoos lower back", "Necklaces", "Watches",
    "Glasses", "Hats", "Extras"
};

static inline void PlayerModelBrokenFix()
{
    CPlayerPed* pPlayer = FindPlayerPed();

    if (pPlayer->m_nModelIndex == 0)
    {
        Call<0x5A81E0>(0, pPlayer->m_pPlayerData->m_pPedClothesDesc, 0xBC1C78, false);
    }
}
#endif

PlayerPage &playerPage = PlayerPage::Get();
PlayerPage::PlayerPage()
    : IPage<PlayerPage>(ePageID::Player, "Player", true)
{
#ifdef GTASA
//	Fix player model being broken after rebuild
    patch::RedirectCall(0x5A834D, &PlayerModelBrokenFix);
    Events::initGameEvent += [this]()
    {
        m_bAimSkinChanger = gConfig.Get("Features.AimSkinChanger", false);
    };
#endif

    Events::processScriptsEvent += [this]
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* player = FindPlayerPed();
        CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];
        int hplayer = CPools::GetPedRef(player);

        if (m_bPlayerRegen)
        {
            static uint lastDmg = 0;
            static uint lastTimer = 0;
            float maxHealth = BY_GAME(player->m_fMaxHealth, 100, 100);
            float maxArmour = BY_GAME(pInfo->m_nMaxArmour, pInfo->m_nMaxArmour, 100);

            static float prevVal = 0;
            float curVal = player->m_fHealth;

            if (player->m_fHealth >= BY_GAME(player->m_fMaxHealth, 100, 100))
            {
                curVal = player->m_fArmour;
            }

            if (curVal != prevVal)
            {
                lastDmg = timer;
                prevVal = curVal;
            }

            if (timer - lastDmg > 5000 && timer - lastTimer > 1000)
            {
                if (player->m_fHealth < maxHealth || player->m_fArmour < maxArmour)
                {
                    if (player->m_fHealth < maxHealth)
                    {
                        player->m_fHealth += 0.2f;
                        prevVal = player->m_fHealth;
                    }
                    else
                    {
                        // Only give armour if player has some already
                        if (player->m_fArmour >= 1)
                        {
                            player->m_fArmour += 0.2f;
                            prevVal = player->m_fArmour;
                        }
                    }

                    lastTimer = timer;
                }
            }
        }

        if (m_RespawnDieLoc.m_bEnabled)
        {
            if (Command<Commands::IS_CHAR_DEAD>(hplayer))
            {
                m_RespawnDieLoc.m_fPos = player->GetPosition();
            }
            else
            {
                CVector cur_pos = player->GetPosition();

                if (m_RespawnDieLoc.m_fPos.x != 0 && m_RespawnDieLoc.m_fPos.x != cur_pos.x
                        && m_RespawnDieLoc.m_fPos.y != 0 && m_RespawnDieLoc.m_fPos.y != cur_pos.y)
                {
                    BY_GAME(player->Teleport(m_RespawnDieLoc.m_fPos, false)
                            , player->Teleport(m_RespawnDieLoc.m_fPos), player->Teleport(m_RespawnDieLoc.m_fPos));
                    m_RespawnDieLoc.m_fPos = CVector(0, 0, 0);
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
            player->m_nPhysicalFlags.bMeleeProof  = 1;
#elif GTAVC
            player->m_nFlags.bBulletProof = 1;
            player->m_nFlags.bCollisionProof = 1;
            player->m_nFlags.bExplosionProof = 1;
            player->m_nFlags.bFireProof = 1;
            player->m_nFlags.bMeleeProof = 1;
            player->m_nFlags.bImmuneToNonPlayerDamage = 1;
#else
            player->m_nFlags.bBulletProof = 1;
            player->m_nFlags.bCollisionProof = 1;
            player->m_nFlags.bExplosionProof = 1;
            player->m_nFlags.bFireProof = 1;
            player->m_nFlags.bMeleeProof = 1;
#endif
        }

#ifdef GTASA
        if (m_bDrunkEffect && !TopDownCam.GetState())
        {
            Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 100);
        }

        if (m_bAimSkinChanger && aimSkinChanger.Pressed())
        {
            CPed* targetPed = player->m_pPlayerTargettedPed;
            if (targetPed)
            {
                player->SetModelIndex(targetPed->m_nModelIndex);
                Util::ClearCharTasksCarCheck(player);
            }
        }
#endif

        if (godMode.Pressed())
        {
            if (m_bGodMode)
            {
                Util::SetMessage(TEXT("Player.GodDisabled"));
#ifdef GTASA
                patch::Set<bool>(0x96916D, m_bGodMode, false);
                player->m_nPhysicalFlags.bBulletProof = 0;
                player->m_nPhysicalFlags.bCollisionProof = 0;
                player->m_nPhysicalFlags.bExplosionProof = 0;
                player->m_nPhysicalFlags.bFireProof = 0;
                player->m_nPhysicalFlags.bMeleeProof = 0;
#elif GTAVC
                player->m_nFlags.bBulletProof = 0;
                player->m_nFlags.bCollisionProof = 0;
                player->m_nFlags.bExplosionProof = 0;
                player->m_nFlags.bFireProof = 0;
                player->m_nFlags.bMeleeProof = 0;
#else
                player->m_nFlags.bBulletProof = 0;
                player->m_nFlags.bCollisionProof = 0;
                player->m_nFlags.bExplosionProof = 0;
                player->m_nFlags.bFireProof = 0;
                player->m_nFlags.bMeleeProof = 0;
#endif
                m_bGodMode = false;
            }
            else
            {
                Util::SetMessage(TEXT("Player.GodEnabled"));
                m_bGodMode = true;
            }
        }
    };
}

#ifdef GTASA
void PlayerPage::SetCloth(std::string& name)
{
    int bodyPart;
    char model[16], tex[16];

    if (sscanf(name.c_str(), "%d$%[^$]$%s", &bodyPart, &model, &tex) != 3)
    {
        return;
    }

    CPlayerPed* player = FindPlayerPed();
    if (!strcmp(tex, "cutoffchinosblue"))
    {
        player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-697413025, 744365350, bodyPart);
    }
    else
    {
        if (!strcmp(tex, "sneakerbincblue"))
        {
            player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-915574819, 2099005073, bodyPart);
        }
        else
        {
            if (!strcmp(tex, "12myfac"))
            {
                player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(-1750049245, 1393983095, bodyPart);
            }
            else
            {
                player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(tex, model, bodyPart);
            }
        }
    }
    CClothes::RebuildPlayer(player, false);
}

void PlayerPage::RemoveClothesTab()
{
    if (ImGui::BeginTabItem(TEXT( "Player.RemoveClothesTab")))
    {
        ImGui::TextWrapped(TEXT("Player.ClothesTip"));
        ImGui::Spacing();

        ImGui::BeginChild("ClothesRemove");
        if (ImGui::Button(TEXT("Player.RemoveAll"), ImVec2(Widget::CalcSize(3))))
        {
            CPlayerPed* player = FindPlayerPed();
            for (uint i = 0; i < 18; i++)
            {
                player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(0u, 0u, i);
            }
            CClothes::RebuildPlayer(player, false);
        }
        ImGui::SameLine();
        size_t count = 1;
        for (auto [k, v] : m_ClothData.m_pData->Items())
        {
            if (ImGui::Button(std::string(k.str()).c_str(), ImVec2(Widget::CalcSize(3))))
            {
                CPlayerPed* player = FindPlayerPed();
                player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(0u, 0u, std::stoi(v.value_or<std::string>("0")));
                CClothes::RebuildPlayer(player, false);
            }

            if (count % 3 != 0)
            {
                ImGui::SameLine();
            }
            ++count;
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}
#endif

#ifdef GTASA
void PlayerPage::SetModel(std::string& model)
{
    if (pedPage.m_PedData.m_pData->Contains(model.c_str()))
    {
        CPlayerPed* player = FindPlayerPed();
        if (pedPage.m_SpecialPedData.Contains(model.c_str()))
        {
            std::string name = pedPage.m_SpecialPedData.Get(model.c_str(), "Unknown");
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
        Util::ClearCharTasksCarCheck(player);
    }
}
#else
void PlayerPage::ChangePlayerModel(std::string& cat, std::string& key, std::string& val)
{
    CPlayerPed* player = FindPlayerPed();

#ifdef GTAVC
    player->Undress(val.c_str());
    CStreaming::LoadAllRequestedModels(false);
    player->Dress();
#else
    if (cat == "Special")
    {
        // CStreaming::RequestSpecialChar(109, val.c_str(), PRIORITY_REQUEST);
        // CStreaming::LoadAllRequestedModels(true);
        // player->SetModelIndex(109);
        // CStreaming::SetMissionDoesntRequireSpecialChar(109);
        Util::SetMessage(TEXT("Player.SpecialNotImplement"));
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

void PlayerPage::Draw()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);
#ifdef GTASA
    CPad* pad = pPlayer->GetPadFromPlayer();
#endif
    CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];

    ImVec2 btn_sz = ImVec2(Widget::CalcSize(3));
    if (ImGui::Button(TEXT("Player.CopyCoordinates"), btn_sz))
    {
        CVector pos = pPlayer->GetPosition();
        std::string text = std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);

        ImGui::SetClipboardText(text.c_str());
        Util::SetMessage(TEXT("Player.CoordCopied"));
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Player.RefreshHealthArmour"), btn_sz))
    {
        pPlayer->m_fHealth = 150;
        pPlayer->m_fArmour = 100;
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Player.Suicide"), btn_sz))
    {
        pPlayer->m_fHealth = 0.0;
    }

    ImGui::Spacing();

    if (ImGui::BeginTabBar("Player", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT( "Window.ToggleTab")))
        {
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Spacing();
            ImGui::Columns(2, 0, false);
#ifdef GTASA
            if (Widget::Toggle(TEXT("Player.AimSkinChanger"), &m_bAimSkinChanger, (TEXT_S("Player.AimSkinChangerTip") + aimSkinChanger.GetNameString()).c_str()))
            {
                gConfig.Set("Features.AimSkinChanger", m_bAimSkinChanger);
            }
            Widget::ToggleAddr<int8_t>(TEXT("Player.BountyYourself"), 0x96913F);

            ImGui::BeginDisabled(TopDownCam.GetState());
            if (Widget::Toggle(TEXT("Player.DrunkEffect"), &m_bDrunkEffect))
            {
                if (!m_bDrunkEffect)
                {
                    Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 0);
                }
            }
            if (Widget::Toggle(TEXT("Player.FastSprint"), &m_bFastSprint, TEXT("Player.FastSprintTip")))
            {
                patch::Set<float>(0x8D2458, m_bFastSprint ? 0.1f : 5.0f);
            }
            ImGui::EndDisabled();
#endif
            Widget::ToggleAddr<int8_t>(TEXT("Player.FreeHealthcare"), (int)&pInfo->m_bGetOutOfHospitalFree);

            if (Widget::Toggle(TEXT("Player.FreezeWL"), &m_bFreezeWantedLevel))
            {
                static unsigned int chaosLvl;
                if (m_bFreezeWantedLevel)
                {
#ifdef GTASA
                    chaosLvl = pPlayer->GetWanted()->m_nChaosLevel;
#else
                    chaosLvl = pPlayer->m_pWanted->m_nChaosLevel;
#endif
                    patch::SetUChar(BY_GAME(0x561C90, 0x4D2110, 0x4AD900), 0xC3);
                }
                else
                {
#ifdef GTASA
                    pPlayer->GetWanted()->m_nChaosLevel = chaosLvl;
#else
                    pPlayer->m_pWanted->m_nChaosLevel = chaosLvl;
#endif
                    patch::SetUChar(BY_GAME(0x561C90, 0x4D2110, 0x4AD900), BY_GAME(0x51, 0x8B, 0xA1));
                }
            }

            if (Widget::Toggle(TEXT("Player.GodMode"), &m_bGodMode))
            {
#ifdef GTASA
                patch::Set<bool>(0x96916D, m_bGodMode, false);
                pPlayer->m_nPhysicalFlags.bBulletProof = m_bGodMode;
                pPlayer->m_nPhysicalFlags.bCollisionProof = m_bGodMode;
                pPlayer->m_nPhysicalFlags.bExplosionProof = m_bGodMode;
                pPlayer->m_nPhysicalFlags.bFireProof = m_bGodMode;
                pPlayer->m_nPhysicalFlags.bMeleeProof = m_bGodMode;

                // if (m_bGodMode)
                // {
                //     // patch::PutRetn(0x639900);
                //     // patch::PutRetn(0x6398F0);
                //     patch::PutRetn(0x630600);
                //     patch::Nop(0x637590, 0xA);
                //     patch::Nop(0x63070C, 0xA);
                //     //patch::SetRaw(0x639900, (void*)"\xC2\x04\x00", 3);
                // }
                // else
                // {
                //     patch::SetRaw(0x637590, (void*)"\xC7\x87\x40\x05\x00\x00\x00\x00\x00\x00", 0xA);
                //     patch::SetRaw(0x63070C, (void*)"\xC7\x86\x40\x05\x00\x00\x00\x00\x00\x00", 0xA);
                //     patch::SetUChar(0x630600, 0x6A);
                //     // patch::SetUChar(0x6398F0, 0xE9);
                //     // patch::SetUChar(0x6397E0, 0x64);
                //     // patch::SetRaw(0x639900, (void*)"\xC2\x04\x00", 3);
                // }
#elif GTAVC
                pPlayer->m_nFlags.bBulletProof = m_bGodMode;
                pPlayer->m_nFlags.bCollisionProof = m_bGodMode;
                pPlayer->m_nFlags.bExplosionProof = m_bGodMode;
                pPlayer->m_nFlags.bFireProof = m_bGodMode;
                pPlayer->m_nFlags.bMeleeProof = m_bGodMode;
                pPlayer->m_nFlags.bImmuneToNonPlayerDamage = m_bGodMode;
#else
                pPlayer->m_nFlags.bBulletProof = m_bGodMode;
                pPlayer->m_nFlags.bCollisionProof = m_bGodMode;
                pPlayer->m_nFlags.bExplosionProof = m_bGodMode;
                pPlayer->m_nFlags.bFireProof = m_bGodMode;
                pPlayer->m_nFlags.bMeleeProof = m_bGodMode;
#endif
            }
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Player.CycleJump"), 0x969161);
            Widget::ToggleAddr<int8_t>(TEXT("Player.InfO2"), 0x96916E);
            if (Widget::ToggleBits(TEXT("Player.InvisPlayer"), pPlayer->m_nPedFlags.bDontRender))
            {
                pPlayer->m_nPedFlags.bDontRender = !pPlayer->m_nPedFlags.bDontRender;
            }
            Widget::ToggleAddr<int8_t>(TEXT("Player.InfSprint"), 0xB7CEE4);
#else
            Widget::ToggleAddr<int8_t>(TEXT("Player.InfSprint"), (int)&pInfo->m_bInfiniteSprint);
#endif
#ifdef GTAVC
            if (Widget::Toggle(TEXT("Player.NoUndress"), &m_bNoUndress, TEXT("Player.NoUndressTip")))
            {
                if (m_bNoUndress)
                {
                    // pop ecx
                    patch::SetUChar(0x42BDC5, 0x59);
                    patch::SetUChar(0x42C1B0, 0x59);
                    patch::SetUChar(0x42C3B2, 0x59);
                    patch::SetUChar(0x42BDC5, 0x59);
                    patch::SetUChar(0x42C1B0, 0x59);
                    patch::SetUChar(0x42C3B2, 0x59);
                    // nop Undress()
                    patch::RedirectShortJump(0x42BDC6, (void*)0x42BDE1);
                    patch::RedirectShortJump(0x42C1B1, (void*)0x42C1CC);
                    patch::RedirectShortJump(0x42C3B3, (void*)0x42C3CE);
                    // nop Dress()
                    patch::RedirectShortJump(0x42BDC6, (void*)0x42BE05);
                    patch::RedirectShortJump(0x42C1B1, (void*)0x42C1F0);
                    patch::RedirectShortJump(0x42C3B3, (void*)0x42C3F2);
                }
                else
                {
                    // restore
                    patch::SetRaw(0x42BDC5, (void*)"\x0F\xB6\x05\xFB\x0A\xA1", 6);
                    patch::SetRaw(0x42C1B0, (void*)"\x0F\xB6\x05\xFB\x0A\xA1", 6);
                    patch::SetRaw(0x42C3B2, (void*)"\x0F\xB6\x05\xFB\x0A\xA1", 6);
                    patch::SetRaw(0x42BDC5, (void*)"\x0F\xB6\x05\xFB\x0A\xA1", 6);
                    patch::SetRaw(0x42C1B0, (void*)"\x0F\xB6\x05\xFB\x0A\xA1", 6);
                    patch::SetRaw(0x42C3B2, (void*)"\x0F\xB6\x05\xFB\x0A\xA1", 6);
                }
            }
#endif
            ImGui::NextColumn();
            if (Widget::Toggle(TEXT("Game.KeepStuff"), &m_bKeepStuff, TEXT("Game.KeepStuffText")))
            {
#ifdef GTASA
                Command<Commands::SWITCH_ARREST_PENALTIES>(m_bKeepStuff);
                Command<Commands::SWITCH_DEATH_PENALTIES>(m_bKeepStuff);
#elif GTAVC
                if (m_bKeepStuff)
                {
                    patch::Nop(0x42C184, 5);
                    patch::Nop(0x42C068, 5);
                    patch::Nop(0x42BC7B, 5);
                }
                else
                {
                    patch::SetRaw(0x42C184, (void*)"\xE8\xB7\x35\x0D\x00", 5);
                    patch::SetRaw(0x42C068, (void*)"\xE8\xD3\x36\x0D\x00", 5);
                    patch::SetRaw(0x42BC7B, (void*)"\xE8\xC0\x3A\x0D\x00", 5);
                }
#elif GTA3
                if (m_bKeepStuff)
                {
                    patch::Nop(0x421507, 7);
                    patch::Nop(0x421724, 7);
                    patch::Nop(0x4217F8, 8);
                }
                else
                {
                    patch::SetRaw(0x421507, (void*)"\x8B\x0B\xE8\x62\xE6\x0A\x00", 7);
                    patch::SetRaw(0x421724, (void*)"\x8B\x0B\xE8\x45\xE4\x0A\x00", 7);
                    patch::SetRaw(0x4217F8, (void*)"\x83\xC4\x14\xE8\x73\xE3\x0A\x00", 8);
                }
#endif
            }
#ifdef GTASA
            if (Widget::ToggleBits(TEXT("Player.LockControl"), pad->bPlayerSafe))
            {
                pad->bPlayerSafe = !pad->bPlayerSafe;
            }
            Widget::ToggleAddr<int8_t>(TEXT("Player.MaxAppeal"), 0x969180, nullptr,  0x1, 0x0);
            Widget::ToggleAddr<int8_t>(TEXT("Player.MegaJump"), 0x96916C);
            Widget::ToggleAddr<int8_t>(TEXT("Player.MegaPunch"), 0x969173);
            Widget::ToggleAddr<int8_t>(TEXT("Player.NeverGetHungry"), 0x969174);

            bool never_wanted = patch::Get<bool>(0x969171, false);
            if (Widget::Toggle(TEXT("Player.NeverWanted"), &never_wanted))
            {
                CCheat::NotWantedCheat();
            }
#else
            static bool neverWanted = false;
            if (Widget::Toggle(TEXT("Player.NeverWanted"), &neverWanted))
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
            Widget::ToggleAddr<int8_t>(TEXT("Player.NoFee"), (int)&pInfo->m_bGetOutOfJailFree);

            Widget::Toggle(TEXT("Player.RespawnDieLoc"), &m_RespawnDieLoc.m_bEnabled, TEXT("Player.RespawnDieLocTip"));
            Widget::Toggle(TEXT("Player.PlayerRegen"), &m_bPlayerRegen, TEXT("Player.PlayerRegenTip"));
#ifdef GTASA
            static bool sprintInt = false;
            if (Widget::Toggle(TEXT("Player.SprintEverywhere"), &sprintInt, TEXT("Player.SprintEverywhereTip")))
            {
                if (sprintInt)
                {
                    patch::SetRaw(0x688610, (char*)"\x90\x90", 2);
                }
                else
                {
                    patch::SetRaw(0x688610, (char*)"\x75\x40", 2);
                }
            }
#endif
            ImGui::Columns(1);

            ImGui::NewLine();
            ImGui::SeparatorText(TEXT("Player.PlayerFlags"));
            ImGui::Spacing();

            ImGui::Columns(2, 0, false);

            bool state = BY_GAME(pPlayer->m_nPhysicalFlags.bBulletProof, pPlayer->m_nFlags.bBulletProof,
                                 pPlayer->m_nFlags.bBulletProof);
            if (Widget::Toggle(TEXT("Player.BulletProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bBulletProof, pPlayer->m_nFlags.bBulletProof,
                        pPlayer->m_nFlags.bBulletProof) = state;
            }

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bCollisionProof, pPlayer->m_nFlags.bCollisionProof,
                            pPlayer->m_nFlags.bCollisionProof);
            if (Widget::Toggle(TEXT("Player.CollisionProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bCollisionProof, pPlayer->m_nFlags.bCollisionProof,
                        pPlayer->m_nFlags.bCollisionProof) = state;
            }

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bExplosionProof, pPlayer->m_nFlags.bExplosionProof,
                            pPlayer->m_nFlags.bExplosionProof);
            if (Widget::Toggle(TEXT("Player.ExplosionProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bExplosionProof, pPlayer->m_nFlags.bExplosionProof,
                        pPlayer->m_nFlags.bExplosionProof) = state;
            }

            ImGui::NextColumn();

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bFireProof, pPlayer->m_nFlags.bFireProof,
                            pPlayer->m_nFlags.bFireProof);
            if (Widget::Toggle(TEXT("Player.FireProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bFireProof, pPlayer->m_nFlags.bFireProof,
                        pPlayer->m_nFlags.bFireProof) = state;
            }

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bMeleeProof, pPlayer->m_nFlags.bMeleeProof,
                            pPlayer->m_nFlags.bMeleeProof);
            if (Widget::Toggle(TEXT("Vehicle.MeleeProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bMeleeProof, pPlayer->m_nFlags.bMeleeProof,
                        pPlayer->m_nFlags.bMeleeProof) = state;
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(TEXT( "Window.MenusTab")))
        {
            ImGui::BeginChild("PlayerMenus");

            Widget::EditAddr<float>(TEXT("Player.Armour"), reinterpret_cast<uint>(&pPlayer->m_fArmour), 0, 100, BY_GAME(pInfo->m_nMaxArmour, pInfo->m_nMaxArmour, 100));
#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Player.Body")))
            {
                if (pPlayer->m_nModelIndex == 0)
                {
                    ImGui::Columns(3, 0, false);
                    static int bodyState = 0;
                    if (ImGui::RadioButton(TEXT("Player.Fat"), &bodyState, 2))
                    {
                        CCheat::FatCheat();
                    }

                    ImGui::NextColumn();

                    if (ImGui::RadioButton(TEXT("Player.Muscle"), &bodyState, 1))
                    {
                        CCheat::MuscleCheat();
                    }

                    ImGui::NextColumn();

                    if (ImGui::RadioButton(TEXT("Player.Skinny"), &bodyState, 0))
                    {
                        CCheat::SkinnyCheat();
                    }

                    ImGui::Columns(1);
                }
                else
                {
                    ImGui::TextWrapped(TEXT("Player.NeedCJSkin"));
                    ImGui::Spacing();

                    if (ImGui::Button(TEXT("Player.ChangeToCJ"), ImVec2(Widget::CalcSize(1))))
                    {
                        pPlayer->SetModelIndex(0);
                        Util::ClearCharTasksCarCheck(pPlayer);
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
            }

            Widget::EditStat(TEXT("Player.Energy"), STAT_ENERGY);
            Widget::EditStat(TEXT("Player.Fat"), STAT_FAT);
#endif
            Widget::EditAddr<float>(TEXT("Player.Health"), reinterpret_cast<uint>(&pPlayer->m_fHealth), 0, 100, BY_GAME(static_cast<int>(pPlayer->m_fMaxHealth), 100, 100));
#ifdef GTASA
            Widget::EditStat(TEXT("Player.LungCapacity"), STAT_LUNG_CAPACITY);

            Widget::EditAddr<uchar>(TEXT("Player.MaxArmour"), reinterpret_cast<uint>(&pInfo->m_nMaxArmour), 0, 100, 255);
            Widget::EditStat(TEXT("Player.MaxHealth"), STAT_MAX_HEALTH, 0, 569, 1450);
            Widget::EditAddr<int>(TEXT("Player.Money"), 0xB7CE50, -99999999, 0, 99999999);
#else
            int money = pInfo->m_nMoney;
            Widget::EditAddr<int>(TEXT("Player.Money"), (int)&money, -9999999, 0, 99999999);
            pInfo->m_nMoney = money;
            pInfo->m_nDisplayMoney = money;
#endif


#ifdef GTASA
            Widget::EditStat(TEXT("Player.Muscle"), STAT_MUSCLE);
            Widget::EditStat(TEXT("Player.Respect"), STAT_RESPECT);
            Widget::EditStat(TEXT("Player.Stamina"), STAT_STAMINA);
#endif
            if (ImGui::CollapsingHeader(TEXT("Player.WantedLevel")))
            {
#ifdef GTASA
                int val = pPlayer->m_pPlayerData->m_pWanted->m_nWantedLevel;
                int max_wl = pPlayer->m_pPlayerData->m_pWanted->MaximumWantedLevel;
                max_wl = max_wl < 6 ? 6 : max_wl;
#else
                int val = pPlayer->m_pWanted->m_nWantedLevel;
                int max_wl = 6;
#endif

                ImGui::Columns(3, 0, false);
                ImGui::Text("%s: 0", TEXT("Window.Minimum"));
                ImGui::NextColumn();
                ImGui::Text("%s: 0", TEXT("Window.Default"));
                ImGui::NextColumn();
                ImGui::Text("%s: %d", TEXT("Window.Maximum"), max_wl);
                ImGui::Columns(1);

                ImGui::Spacing();

                if (ImGui::InputInt(TEXT("Window.SetValue"), &val))
                {
#ifdef GTASA
                    pPlayer->CheatWantedLevel(val);
#elif GTAVC
                    pPlayer->m_pWanted->CheatWantedLevel(val);
#else
                    pPlayer->m_pWanted->SetWantedLevel(val);
#endif
                }

                ImGui::Spacing();
                if (ImGui::Button(TEXT("Window.Minimum"), Widget::CalcSize(3)))
                {
#ifdef GTASA
                    pPlayer->CheatWantedLevel(0);
#elif GTAVC
                    pPlayer->m_pWanted->CheatWantedLevel(0);
#else
                    pPlayer->m_pWanted->SetWantedLevel(0);
#endif
                }

                ImGui::SameLine();

                if (ImGui::Button(TEXT("Window.Default"), Widget::CalcSize(3)))
                {
#ifdef GTASA
                    pPlayer->CheatWantedLevel(0);
#elif GTAVC
                    pPlayer->m_pWanted->CheatWantedLevel(0);
#else
                    pPlayer->m_pWanted->SetWantedLevel(0);
#endif
                }

                ImGui::SameLine();

                if (ImGui::Button(TEXT("Window.Maximum"), Widget::CalcSize(3)))
                {
#ifdef GTASA
                    pPlayer->CheatWantedLevel(max_wl);
#elif GTAVC
                    pPlayer->m_pWanted->CheatWantedLevel(max_wl);
#else
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
        if (ImGui::BeginTabItem(TEXT( "Player.ClothesTab")))
        {
            if (pPlayer->m_nModelIndex == 0)
            {
                Widget::ImageList(m_ClothData, fArgWrapper(playerPage.SetCloth),
                                    [](std::string& str)
                {
                    std::stringstream ss(str);
                    std::string temp;

                    getline(ss, temp, '$');
                    getline(ss, temp, '$');

                    return temp;
                }, nullptr, nullptr, nullptr, fArgNoneWrapper(playerPage.RemoveClothesTab));
            }
            else
            {
                ImGui::TextWrapped(TEXT("Player.NeedCJSkin"));
                ImGui::Spacing();

                if (ImGui::Button(TEXT("Player.ChangeToCJ"), ImVec2(Widget::CalcSize(1))))
                {
                    pPlayer->SetModelIndex(0);
                    Util::ClearCharTasksCarCheck(pPlayer);
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Player.PedSkinsTab")))
        {
            Widget::ImageList(pedPage.m_PedData, fArgWrapper(playerPage.SetModel),
                                [](std::string& str)
            {
                return pedPage.m_PedData.m_pData->Get(str.c_str(), "Unknown");
            });
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Player.CustomSkinsTab")))
        {
            ImGui::BeginChild("AAA");
            ImGui::Spacing();
            CustomSkins.Draw();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
#else
        if (ImGui::BeginTabItem(TEXT( "Player.SkinsTab")))
        {
            ImGui::Spacing();
#ifdef GTA3
            ImGui::TextWrapped(TEXT("Player.SkinChangeFrozen"));
            CPad::GetPad(0)->DisablePlayerControls = true;
#else
            ImGui::TextWrapped(TEXT("Player.WorkSkinOnly"));
#endif
            Widget::DataList(skinData, fArg3Wrapper(playerPage.ChangePlayerModel));
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
