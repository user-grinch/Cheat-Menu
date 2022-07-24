#include "pch.h"
#include "player.h"
#include "menu.h"
#include "widget.h"
#include "util.h"

#ifdef GTASA
#include "ped.h"

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

/*
	Taken from gta chaos mod by Lordmau5
	https://github.com/gta-chaos-mod/Trilogy-ASI-Script
*/
void Player::TopDownCamera::Process()
{
    CPlayerPed *player = FindPlayerPed ();
    CVector     pos    = player->GetPosition ();
    float       curOffset = m_fOffset;

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

void Player::Init()
{
#ifdef GTASA
//	Fix player model being broken after rebuild
    patch::RedirectCall(0x5A834D, &PlayerModelBrokenFix);
    m_bAimSkinChanger = gConfig.Get("Features.AimSkinChanger", false);
#endif

    // Custom skins setup
    std::string path = GAME_PATH((char*)"modloader/");
    if (GetModuleHandle("modloader.asi") && std::filesystem::is_directory(path))
    {
#ifdef GTASA
        path += "CustomSkins/";
        if (std::filesystem::is_directory(path))
        {
            for (auto& p : std::filesystem::recursive_directory_iterator(path))
            {
                if (p.path().extension() == ".dff")
                {
                    std::string file_name = p.path().stem().string();

                    if (file_name.size() < 9)
                    {
                        CustomSkins::m_List.push_back(file_name);
                    }
                    else
                    {
                        Log::Print<eLogLevel::Error>("Custom Skin longer than 8 characters {}", file_name);
                    }
                }
            }
        }
        else
        {
            std::filesystem::create_directory(path);
        }
#endif

        m_bModloaderInstalled = true;
    }

    Events::processScriptsEvent += []
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
            float curVal = (player->m_fHealth == player->m_fMaxHealth) ? player->m_fHealth : player->m_fArmour;
            if (curVal != prevVal)
            {
                lastDmg = timer;
                curVal = prevVal;
            }

            if (timer - lastDmg > 5000 && timer - lastTimer > 1000)
            {
                if (player->m_fHealth != maxHealth || player->m_fArmour != maxArmour)
                {
                    if (player->m_fHealth != maxHealth)
                    {
                        player->m_fHealth += 0.2f;
                        prevVal = player->m_fHealth;
                    }
                    else
                    {
                        player->m_fArmour += 0.2f;
                        prevVal = player->m_fArmour;
                    }
                  
                    lastTimer = timer;
                }
            }
        }

        if (KeepPosition::m_bEnabled)
        {
            if (Command<Commands::IS_CHAR_DEAD>(hplayer))
            {
                KeepPosition::m_fPos = player->GetPosition();
            }
            else
            {
                CVector cur_pos = player->GetPosition();

                if (KeepPosition::m_fPos.x != 0 && KeepPosition::m_fPos.x != cur_pos.x
                        && KeepPosition::m_fPos.y != 0 && KeepPosition::m_fPos.y != cur_pos.y)
                {
                    BY_GAME(player->Teleport(KeepPosition::m_fPos, false)
                            , player->Teleport(KeepPosition::m_fPos), player->Teleport(KeepPosition::m_fPos));
                    KeepPosition::m_fPos = CVector(0, 0, 0);
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
#else
            player->m_nFlags.bBulletProof = m_bGodMode;
            player->m_nFlags.bCollisionProof = m_bGodMode;
            player->m_nFlags.bExplosionProof = m_bGodMode;
            player->m_nFlags.bFireProof = m_bGodMode;
            player->m_nFlags.bMeleeProof = m_bGodMode;
#endif
        }

#ifdef GTASA
        if (m_bDrunkEffect && !TopDownCamera::m_bEnabled)
        {
            Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 100);
        }

        if (TopDownCamera::m_bEnabled)
        {
            TopDownCamera::Process();
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
                SetHelpMessage(TEXT("Player.GodDisabled"));
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
                player->m_nFlags.bBulletProof = m_bGodMode;
                player->m_nFlags.bCollisionProof = m_bGodMode;
                player->m_nFlags.bExplosionProof = m_bGodMode;
                player->m_nFlags.bFireProof = m_bGodMode;
                player->m_nFlags.bMeleeProof = m_bGodMode;
#endif
                m_bGodMode = false;
            }
            else
            {
                SetHelpMessage(TEXT("Player.GodEnabled"));
                m_bGodMode = true;
            }
        }
    };
}

#ifdef GTASA
void Player::ChangePlayerCloth(std::string& name)
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
#endif

#ifdef GTASA
void Player::ChangePlayerModel(std::string& model)
{
    bool custom_skin = std::find(CustomSkins::m_List.begin(), CustomSkins::m_List.end(), model) !=
                       CustomSkins::m_List.end();

    if (Ped::m_PedData.m_pData->Contains(model.c_str()) || custom_skin)
    {
        CPlayerPed* player = FindPlayerPed();
        if (Ped::m_SpecialPedData.Contains(model.c_str()) || custom_skin)
        {
            std::string name;
            if (Ped::m_SpecialPedData.Contains(model.c_str()))
            {
                name = Ped::m_SpecialPedData.Get(model.c_str(), "Unknown");
            }
            else
            {
                name = model;
            }

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
#else
void Player::ChangePlayerModel(std::string& cat, std::string& key, std::string& val)
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
        SetHelpMessage(TEXT("Player.SpecialNotImplement"));
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

void Player::ShowPage()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);
#ifdef GTASA
    CPad* pad = pPlayer->GetPadFromPlayer();
#endif
    CPlayerInfo *pInfo = &CWorld::Players[CWorld::PlayerInFocus];

    if (ImGui::Button(TEXT("Player.CopyCoordinates"), ImVec2(Widget::CalcSize(2))))
    {
        CVector pos = pPlayer->GetPosition();
        std::string text = std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);

        ImGui::SetClipboardText(text.c_str());
        SetHelpMessage(TEXT("Player.CoordCopied"));
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Player.Suicide"), ImVec2(Widget::CalcSize(2))))
    {
        pPlayer->m_fHealth = 0.0;
    }

    ImGui::Spacing();

    if (ImGui::BeginTabBar("Player", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::Spacing();

            ImGui::BeginChild("CheckboxesChild");

            ImGui::Columns(2, 0, false);

#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Player.BountyYourself"), 0x96913F);

            ImGui::BeginDisabled(TopDownCamera::m_bEnabled);
            if (Widget::Checkbox(TEXT("Player.DrunkEffect"), &m_bDrunkEffect))
            {
                if (!m_bDrunkEffect)
                {
                    Command<eScriptCommands::COMMAND_SET_PLAYER_DRUNKENNESS> (0, 0);
                }
            }
            if (Widget::Checkbox(TEXT("Player.FastSprint"), &m_bFastSprint, TEXT("Player.FastSprintTip")))
            {
                patch::Set<float>(0x8D2458, m_bFastSprint ? 0.1f : 5.0f);
            }
            ImGui::EndDisabled();
#endif
            Widget::CheckboxAddr(TEXT("Player.FreeHealthcare"), (int)&pInfo->m_bGetOutOfHospitalFree);

            if (Widget::Checkbox(TEXT("Player.FreezeWL"), &m_bFreezeWantedLevel))
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

            if (Widget::Checkbox(TEXT("Player.GodMode"), &m_bGodMode))
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
#else
                pPlayer->m_nFlags.bBulletProof = m_bGodMode;
                pPlayer->m_nFlags.bCollisionProof = m_bGodMode;
                pPlayer->m_nFlags.bExplosionProof = m_bGodMode;
                pPlayer->m_nFlags.bFireProof = m_bGodMode;
                pPlayer->m_nFlags.bMeleeProof = m_bGodMode;
#endif
            }
#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Player.CycleJump"), 0x969161);
            Widget::CheckboxAddr(TEXT("Player.InfO2"), 0x96916E);
            if (Widget::CheckboxBits(TEXT("Player.InvisPlayer"), pPlayer->m_nPedFlags.bDontRender))
            {
                pPlayer->m_nPedFlags.bDontRender = !pPlayer->m_nPedFlags.bDontRender;
            }
            Widget::CheckboxAddr(TEXT("Player.InfSprint"), 0xB7CEE4);
#else
            Widget::CheckboxAddr(TEXT("Player.InfSprint"), (int)&pInfo->m_bInfiniteSprint);
#endif

            ImGui::NextColumn();

#ifdef GTASA
            if (Widget::CheckboxBits(TEXT("Player.LockControl"), pad->bPlayerSafe))
            {
                pad->bPlayerSafe = !pad->bPlayerSafe;
            }
            Widget::CheckboxAddrRaw(TEXT("Player.MaxAppeal"), 0x969180, 1, "\x01", "\x00");
            Widget::CheckboxAddr(TEXT("Player.MegaJump"), 0x96916C);
            Widget::CheckboxAddr(TEXT("Player.MegaPunch"), 0x969173);
            Widget::CheckboxAddr(TEXT("Player.NeverGetHungry"), 0x969174);

            bool never_wanted = patch::Get<bool>(0x969171, false);
            if (Widget::Checkbox(TEXT("Player.NeverWanted"), &never_wanted))
            {
                CCheat::NotWantedCheat();
            }
#else
            static bool neverWanted = false;
            if (Widget::Checkbox(TEXT("Player.NeverWanted"), &neverWanted))
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
            Widget::CheckboxAddr(TEXT("Player.NoFee"), (int)&pInfo->m_bGetOutOfJailFree);
            Widget::Checkbox(TEXT("Player.RespawnDieLoc"), &KeepPosition::m_bEnabled, TEXT("Player.RespawnDieLocTip"));
            Widget::Checkbox(TEXT("Player.PlayerRegen"), &m_bPlayerRegen, TEXT("Player.PlayerRegenTip"));
#ifdef GTASA
            static bool sprintInt = false;
            if (Widget::Checkbox(TEXT("Player.SprintEverywhere"), &sprintInt, TEXT("Player.SprintEverywhereTip")))
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
            ImGui::TextWrapped(TEXT("Player.PlayerFlags"));

            ImGui::Columns(2, 0, false);

            bool state = BY_GAME(pPlayer->m_nPhysicalFlags.bBulletProof, pPlayer->m_nFlags.bBulletProof,
                                 pPlayer->m_nFlags.bBulletProof);
            if (Widget::Checkbox(TEXT("Player.BulletProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bBulletProof, pPlayer->m_nFlags.bBulletProof,
                        pPlayer->m_nFlags.bBulletProof) = state;
            }

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bCollisionProof, pPlayer->m_nFlags.bCollisionProof,
                            pPlayer->m_nFlags.bCollisionProof);
            if (Widget::Checkbox(TEXT("Player.CollisionProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bCollisionProof, pPlayer->m_nFlags.bCollisionProof,
                        pPlayer->m_nFlags.bCollisionProof) = state;
            }

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bExplosionProof, pPlayer->m_nFlags.bExplosionProof,
                            pPlayer->m_nFlags.bExplosionProof);
            if (Widget::Checkbox(TEXT("Player.ExplosionProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bExplosionProof, pPlayer->m_nFlags.bExplosionProof,
                        pPlayer->m_nFlags.bExplosionProof) = state;
            }

            ImGui::NextColumn();

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bFireProof, pPlayer->m_nFlags.bFireProof,
                            pPlayer->m_nFlags.bFireProof);
            if (Widget::Checkbox(TEXT("Player.FireProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bFireProof, pPlayer->m_nFlags.bFireProof,
                        pPlayer->m_nFlags.bFireProof) = state;
            }

            state = BY_GAME(pPlayer->m_nPhysicalFlags.bMeleeProof, pPlayer->m_nFlags.bMeleeProof,
                            pPlayer->m_nFlags.bMeleeProof);
            if (Widget::Checkbox(TEXT("Player.MeeleProof"), &state, nullptr, m_bGodMode))
            {
                BY_GAME(pPlayer->m_nPhysicalFlags.bMeleeProof, pPlayer->m_nFlags.bMeleeProof,
                        pPlayer->m_nFlags.bMeleeProof) = state;
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
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
                        Util::ClearCharTasksVehCheck(pPlayer);
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
            if (ImGui::CollapsingHeader(TEXT("Player.TopDownCamera")))
            {
                if (ImGui::Checkbox(TEXT("Window.Enabled"), &TopDownCamera::m_bEnabled))
                {
                    Command<Commands::RESTORE_CAMERA_JUMPCUT>();
                }
                ImGui::Spacing();
                ImGui::SliderFloat(TEXT("Player.CameraZoom"), &TopDownCamera::m_fOffset, 20.0f, 60.0f);
                ImGui::Spacing();
                ImGui::Separator();
            }
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
        if (ImGui::BeginTabItem(TEXT("Player.AppearanceTab")))
        {
            ImGui::Spacing();

            if (Widget::Checkbox(TEXT("Player.AimSkinChanger"), &m_bAimSkinChanger, TEXT("Player.AimSkinChangerTip") + aimSkinChanger.Pressed()))
            {
                gConfig.Set("Features.AimSkinChanger", m_bAimSkinChanger);
            }
            if (ImGui::BeginTabBar("AppearanceTabBar"))
            {
                if (ImGui::BeginTabItem(TEXT("Player.ClothesTab")))
                {
                    if (pPlayer->m_nModelIndex == 0)
                    {
                        Widget::ImageList(m_ClothData, ChangePlayerCloth, [](std::string& str)
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
                        ImGui::TextWrapped(TEXT("Player.NeedCJSkin"));
                        ImGui::Spacing();

                        if (ImGui::Button(TEXT("Player.ChangeToCJ"), ImVec2(Widget::CalcSize(1))))
                        {
                            pPlayer->SetModelIndex(0);
                            Util::ClearCharTasksVehCheck(pPlayer);
                        }
                    }
                    ImGui::EndTabItem();
                }
                if (pPlayer->m_nModelIndex == 0
                && ImGui::BeginTabItem(TEXT("Player.RemoveClothesTab")))
                {
                    ImGui::TextWrapped(TEXT("Player.ClothesTip"));
                    ImGui::Spacing();

                    ImGui::BeginChild("ClothesRemove");
                    size_t count = 0;
                    if (ImGui::Button(TEXT("Player.RemoveAll"), ImVec2(Widget::CalcSize(2))))
                    {
                        CPlayerPed* player = FindPlayerPed();
                        for (uint i = 0; i < 18; i++)
                        {
                            player->m_pPlayerData->m_pPedClothesDesc->SetTextureAndModel(0u, 0u, i);
                        }
                        CClothes::RebuildPlayer(player, false);
                    }
                    ImGui::SameLine();
                    for (auto [k, v] : m_ClothData.m_pData->Items())
                    {
                        if (ImGui::Button(std::string(k.str()).c_str(), ImVec2(Widget::CalcSize(2))))
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

                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(TEXT("Player.PedSkinsTab")))
                {
                    Widget::ImageList(Ped::m_PedData, ChangePlayerModel,
                    [](std::string& str)
                    {
                        return Ped::m_PedData.m_pData->Get(str.c_str(), "Unknown");
                    });
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(TEXT("Player.CustomSkinsTab")))
                {
                    ImGui::Spacing();

                    if (m_bModloaderInstalled)
                    {
                        Widget::Filter(TEXT("Window.Search"), m_ClothData.m_Filter,
                                           std::string(TEXT("Player.TotalSkins") + std::to_string(CustomSkins::m_List.size()))
                                           .c_str());
                        Widget::Tooltip(TEXT("Player.CustomSkinsDirTip"));
                        ImGui::Spacing();
                        ImGui::TextWrapped(TEXT("Player.CustomSkinsTip"));
                        ImGui::Spacing();
                        for (std::string name : CustomSkins::m_List)
                        {
                            if (CustomSkins::m_Filter.PassFilter(name.c_str()))
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
                        ImGui::TextWrapped(TEXT("Player.CustomSkinTutorial"));
                        ImGui::Spacing();
                        if (ImGui::Button(TEXT("Player.DownloadModloader"), ImVec2(Widget::CalcSize(1))))
                            ShellExecute(NULL, "open", "https://gtaforums.com/topic/669520-mod-loader/", NULL, NULL,
                                         SW_SHOWNORMAL);
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
#else
        if (ImGui::BeginTabItem(TEXT("Player.SkinsTab")))
        {
            ImGui::Spacing();
#ifdef GTA3
            ImGui::TextWrapped(TEXT("Player.SkinChangeFrozen"));
            CPad::GetPad(0)->DisablePlayerControls = true;
#else
            ImGui::TextWrapped(TEXT("Player.WorkSkinOnly"));
#endif
            Widget::DataList(skinData, ChangePlayerModel, nullptr);
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
