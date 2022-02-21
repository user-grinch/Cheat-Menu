#include "pch.h"
#include "weapon.h"
#include "ui.h"
#include "util.h"
#include "CWeaponInfo.h"

void Weapon::Init()
{
    Events::processScriptsEvent += []
    {
        CPlayerPed* player = FindPlayerPed();

#ifdef GTASA
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
#endif


        uchar slot = BY_GAME(player->m_nActiveWeaponSlot, player->m_nActiveWeaponSlot, player->m_nCurrentWeapon);
        if (m_nCurrentWeaponSlot != slot)
        {

#ifdef GTA3
            eWeaponType weaponType = player->m_aWeapons[slot].m_eWeaponType;
#else
            eWeaponType weaponType = player->m_aWeapons[slot].m_nType;
#endif

#ifdef GTASA
            CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(weaponType, player->GetWeaponSkill(weaponType));
#else
            CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(weaponType);

            if(m_bInfiniteAmmo)
            {
                Command<Commands::SET_PLAYER_AMMO>(0, weaponType, 999999);
            }
#endif

            if (m_bHugeDamage)
            {
                pWeaponInfo->m_nDamage = 1000;
            }

            if (m_bLongRange)
            {
#ifdef GTASA
                pWeaponInfo->m_fTargetRange = 1000.0f;
                pWeaponInfo->m_fWeaponRange = 1000.0f;
                pWeaponInfo->m_fAccuracy = 1.0f;
                pWeaponInfo->m_nFlags.bReload2Start = true;
#else
                pWeaponInfo->m_fRange = 1000.0f;
#endif
            }

#ifdef GTASA
            if (m_bRapidFire && weaponType != WEAPON_FTHROWER && weaponType != WEAPON_MINIGUN) // mingun & flamethrower doesn't work with rapidfire
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
#endif
            m_nCurrentWeaponSlot = slot;
        }
    };
}

#ifdef GTASA
void Weapon::SetGangWeapon(std::string& weapon_type)
{
    m_nGangWeaponList[m_nSelectedGang][m_nSelectedWeapon] = std::stoi(weapon_type);
    CGangs::SetGangWeapons(m_nSelectedGang, m_nGangWeaponList[m_nSelectedGang][0], m_nGangWeaponList[m_nSelectedGang][1],
                           m_nGangWeaponList[m_nSelectedGang][2]);
}
#else
// Implementation of SA opcode 0x555
static void ClearPlayerWeapon(eWeaponType weaponType)
{
    CPlayerPed *pPlayer = FindPlayerPed();

#ifdef GTA3
    int weaponSlot = pPlayer->GetWeaponSlot(weaponType);
#else
    int weaponSlot = CWeaponInfo::GetWeaponInfo(weaponType)->m_WeaponSlot;
#endif

    if ( weaponSlot != -1 )
    {
        CWeapon *pWeapon = &pPlayer->m_aWeapons[weaponSlot];

#ifdef GTA3
        if (pWeapon->m_eWeaponType == weaponType)
        {
            if (pPlayer->m_nCurrentWeapon == weaponSlot)
            {
                Command<Commands::SET_CURRENT_PLAYER_WEAPON>(0, WEAPONTYPE_UNARMED);
            }
            // This doesn't work for melee weapons aka bats, chainsaw etc
            pWeapon->m_eWeaponState = WEAPONSTATE_OUT_OF_AMMO;
            pWeapon->m_nAmmoTotal = 0;
            pWeapon->m_nAmmoInClip = 0;
        }
#else
        if (pWeapon->m_nType == weaponType)
        {
            if (pPlayer->m_nActiveWeaponSlot == weaponSlot)
            {
                CWeaponInfo *pWeaponInfo = CWeaponInfo::GetWeaponInfo(WEAPONTYPE_UNARMED);
                pPlayer->SetCurrentWeapon(pWeaponInfo->m_WeaponSlot);
            }
            pWeapon->Shutdown();
        }
#endif
    }
}

// Implementation of opcode 0x605 (CLEO)
static eWeaponType GetWeaponTypeFromModel(int model)
{
    eWeaponType weaponType = WEAPONTYPE_UNARMED;

    for (size_t i = 0; i < 37; i++)
    {

        int temp = CallAndReturn<int, BY_GAME(NULL, 0x4418B0, 0x430690)>(i); // int __cdecl CPickups::ModelForWeapon(int a1)

        if (temp == model)
        {
            weaponType = (eWeaponType)i;
            break;
        }
    }

    return weaponType;
}
#endif

#ifdef GTASA
void Weapon::GiveWeaponToPlayer(std::string& weapon_type)
{
    CPlayerPed* player = FindPlayerPed();
    int hplayer = CPools::GetPedRef(player);

    if (weapon_type == "-1") // Jetpack
    {
        Command<Commands::TASK_JETPACK>(hplayer);
    }
    else if (weapon_type == "-2") // CellPhone
    {
        CStreaming::RequestModel(330, PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(false);
        player->ClearWeaponTarget();
        player->SetCurrentWeapon(WEAPON_UNARMED);
        player->AddWeaponModel(330);
        Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(330);
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
#else
void Weapon::GiveWeaponToPlayer(std::string& rootkey, std::string& name, std::string& model)
{
    CPlayerPed* player = FindPlayerPed();
    int hplayer = CPools::GetPedRef(player);
    int iModel = std::stoi(model);
    CStreaming::RequestModel(iModel, PRIORITY_REQUEST);
    CStreaming::LoadAllRequestedModels(false);

    eWeaponType weaponType = GetWeaponTypeFromModel(iModel);
    Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, weaponType, m_nAmmoCount);
    Command<Commands::MARK_MODEL_AS_NO_LONGER_NEEDED>(iModel);
#ifdef GTA3
    Command<Commands::SET_CURRENT_PLAYER_WEAPON>(0, weaponType);
#endif
}
#endif

void Weapon::ShowPage()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    uint hplayer = CPools::GetPedRef(pPlayer);

    ImGui::Spacing();
    if (ImGui::Button("Drop weapon", Ui::GetSize(3)))
    {
        float x, y, z;
        Command<Commands::GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS>(hplayer, 0.0, 3.0, 0.0, &x, &y, &z);

#ifdef GTA3
        eWeaponType weaponType = pPlayer->m_aWeapons[pPlayer->m_nCurrentWeapon].m_eWeaponType;
#else
        eWeaponType weaponType = pPlayer->m_aWeapons[pPlayer->m_nActiveWeaponSlot].m_nType;
#endif

        if (weaponType)
        {
            int model = 0, pickup = 0;
#ifdef GTASA
            Command<Commands::GET_WEAPONTYPE_MODEL>(weaponType, &model);
#else
            model = CallAndReturn<int, BY_GAME(NULL, 0x4418B0, 0x430690)>(weaponType); // int __cdecl CPickups::ModelForWeapon(int a1)
#endif
            Command<Commands::CREATE_PICKUP_WITH_AMMO>(model, 3, 999, x, y, z, &pickup);

#ifdef GTASA
            Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, weaponType);
#else
            ClearPlayerWeapon(weaponType);
#endif
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove all", Ui::GetSize(3)))
    {
        pPlayer->ClearWeapons();
    }

    ImGui::SameLine();
    if (ImGui::Button("Remove current", Ui::GetSize(3)))
    {
#ifdef GTASA
        Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, pPlayer->m_aWeapons[pPlayer->m_nActiveWeaponSlot].m_nType);
#elif GTAVC
        ClearPlayerWeapon(pPlayer->m_aWeapons[pPlayer->m_nActiveWeaponSlot].m_nType);
#else
        ClearPlayerWeapon(pPlayer->m_aWeapons[pPlayer->m_nCurrentWeapon].m_eWeaponType);
#endif
    }
    ImGui::Spacing();

    if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem("Checkboxes"))
        {
            ImGui::Spacing();
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Spacing();
            ImGui::SameLine();
            ImGui::Text("Info");
            Ui::ShowTooltip("Weapon tweaks apply globally\nto every ped weapon type");
            ImGui::Columns(2, 0, false);
#ifdef GTASA
            Ui::CheckboxWithHint("Fast aim", &m_bAutoAim, "Enables aim assist on keyboard\n\nQ = left    E = right\n\nPress Q and E to switch targets.\nMoving mouse removes the target!");
            if (Ui::CheckboxWithHint("Dual wield", &m_bDualWeild,
                                     "Dual wield pistol, shawoff, uzi, tec9\n(Other weapons don't work)"))
            {
                if (!m_bDualWeild)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
#endif
            if (Ui::CheckboxWithHint("Huge damage", &m_bHugeDamage, "Also enable 'Long range' if weapon range is short"))
            {
                if (!m_bHugeDamage)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
            if (Ui::CheckboxWithHint("Fast reload", &m_bFastReload))
            {
                Command<Commands::SET_PLAYER_FAST_RELOAD>(hplayer, m_bFastReload);
            }

#ifdef GTASA
            Ui::CheckboxAddress("Infinite ammo", 0x969178);
            ImGui::NextColumn();
#else
            ImGui::NextColumn();
            Ui::CheckboxWithHint("Infinite ammo", &m_bInfiniteAmmo);
#endif
            if (Ui::CheckboxWithHint("Long range", &m_bLongRange))
            {
                if (!m_bLongRange)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
#ifdef GTASA
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
#endif
            ImGui::Columns(1, 0, false);
            ImGui::EndChild();
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
#ifdef GTASA
            Ui::DrawImages(m_WeaponData, GiveWeaponToPlayer, nullptr,
                           [](std::string str)
            {
                return m_WeaponData.m_pJson->m_Data[str].get<std::string>();
            },
            [](std::string str)
            {
                return str != "0"; /*Unarmed*/
            }
                          );
#else
            Ui::DrawJSON(m_WeaponData, GiveWeaponToPlayer, nullptr);
#endif
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem("Gang weapon editor"))
        {
            ImGui::Spacing();
            Ui::ListBox("Select gang", m_GangList, m_nSelectedGang);

            ImGui::Columns(3, 0, false);
            ImGui::RadioButton("Weap 1", &m_nSelectedWeapon, 0);
            ImGui::NextColumn();
            ImGui::RadioButton("Weap 2", &m_nSelectedWeapon, 1);
            ImGui::NextColumn();
            ImGui::RadioButton("Weap 3", &m_nSelectedWeapon, 2);
            ImGui::Columns(1);

            ImGui::Spacing();
            ImGui::Text("Current weapon: %s",
                        m_WeaponData.m_pJson->m_Data[std::to_string(m_nGangWeaponList[m_nSelectedGang][m_nSelectedWeapon])].get<
                        std::string>().c_str());
            ImGui::Spacing();
            Ui::DrawImages(m_WeaponData, SetGangWeapon, nullptr,
                           [](std::string str)
            {
                return m_WeaponData.m_pJson->m_Data[str].get<std::string>();
            },
            [](std::string str)
            {
                return str != "-1"; /*Jetpack*/
            }
                          );
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
