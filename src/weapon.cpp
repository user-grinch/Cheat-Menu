#include "pch.h"
#include "weapon.h"
#include "widget.h"
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
            if (CPad::NewMouseControllerState.x == 0 && CPad::NewMouseControllerState.y == 0)
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

        static uint8_t curWeaponSlot;
        uint8_t slot = BY_GAME(player->m_nActiveWeaponSlot, player->m_nSelectedWepSlot, player->m_nSelectedWepSlot);
        if (curWeaponSlot != slot)
        {
            eWeaponType weaponType = player->m_aWeapons[slot].m_eWeaponType;

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

            if (m_bNoSpread)
            {
                pWeaponInfo->m_fAccuracy = 100.0f;
            }

            if (m_bMoveFire)
            {
                pWeaponInfo->m_nFlags.bMoveFire = true;
            }
#endif
            curWeaponSlot = slot;
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


        if (pWeapon->m_eWeaponType == weaponType)
        {
            if (pPlayer->m_nCurrentWeapon == weaponSlot)
            {
#ifdef GTA3
                Command<Commands::SET_CURRENT_PLAYER_WEAPON>(0, WEAPONTYPE_UNARMED);
#else
                CWeaponInfo *pWeaponInfo = CWeaponInfo::GetWeaponInfo(WEAPONTYPE_UNARMED);
                pPlayer->SetCurrentWeapon(pWeaponInfo->m_WeaponSlot);
#endif
            }
#ifdef GTA3
                // This doesn't work for melee weapons aka bats, chainsaw etc
                pWeapon->m_eWeaponState = WEAPONSTATE_OUT_OF_AMMO;
                pWeapon->m_nAmmoTotal = 0;
                pWeapon->m_nAmmoInClip = 0;
#else
                pWeapon->Shutdown();
#endif
        }
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

void Weapon::AddWeapon()
{
    static char name[INPUT_BUFFER_SIZE];
    static int model = 0;
    ImGui::InputTextWithHint(TEXT("Menu.Name"), "Minigun", name, INPUT_BUFFER_SIZE);
    Widget::InputInt(TEXT("Weapon.WeaponType"), &model, 0, 999999);
    ImGui::Spacing();
    ImVec2 sz = Widget::CalcSize(1);
    if (ImGui::Button(TEXT("Window.AddEntry"), sz))
    {
        std::string key = std::format("Custom.{} (Added)", name);
        m_WeaponData.m_pData->Set(key.c_str(), std::to_string(model));
        m_WeaponData.m_pData->Save();
        Util::SetMessage(TEXT("Window.AddEntryMSG"));
    }
}

void Weapon::ShowPage()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    uint hplayer = CPools::GetPedRef(pPlayer);

    ImGui::Spacing();
    if (ImGui::Button(TEXT("Weapon.DropWeapon"), Widget::CalcSize(3)))
    {
        float x, y, z;
        Command<Commands::GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS>(hplayer, 0.0, 3.0, 0.0, &x, &y, &z);

        eWeaponType weaponType = pPlayer->m_aWeapons[BY_GAME(pPlayer->m_nActiveWeaponSlot, pPlayer->m_nSelectedWepSlot, pPlayer->m_nSelectedWepSlot)].m_eWeaponType;
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
    if (ImGui::Button(TEXT("Weapon.DropAll"), Widget::CalcSize(3)))
    {
        pPlayer->ClearWeapons();
    }

    ImGui::SameLine();
    if (ImGui::Button(TEXT("Weapon.DropCurrent"), Widget::CalcSize(3)))
    {
#ifdef GTASA
        Command<Commands::REMOVE_WEAPON_FROM_CHAR>(hplayer, pPlayer->m_aWeapons[pPlayer->m_nActiveWeaponSlot].m_eWeaponType);
#else
        ClearPlayerWeapon(pPlayer->m_aWeapons[pPlayer->m_nCurrentWeapon].m_eWeaponType);
#endif
    }
    ImGui::Spacing();

    if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Spacing();
            ImGui::SameLine();
            ImGui::Text(TEXT("Window.Info"));
            Widget::Tooltip(TEXT("Weapon.WeaponTweaksText"));
            ImGui::Columns(2, 0, false);
#ifdef GTASA
            Widget::Checkbox(TEXT("Weapon.FastAim"), &m_bAutoAim, TEXT("Weapon.FastAimText"));
            if (Widget::Checkbox(TEXT("Weapon.DualWeild"), &m_bDualWeild,TEXT("Weapon.DualWeildText")))
            {
                if (!m_bDualWeild)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
#endif
            if (Widget::Checkbox(TEXT("Weapon.HugeDamage"), &m_bHugeDamage, TEXT("Weapon.HugeDamageText")))
            {
                if (!m_bHugeDamage)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
            if (Widget::Checkbox(TEXT("Weapon.FastReload"), &m_bFastReload))
            {
                Command<Commands::SET_PLAYER_FAST_RELOAD>(hplayer, m_bFastReload);
            }

#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Weapon.InfiniteAmmo"), 0x969178);
            ImGui::NextColumn();
#else
            ImGui::NextColumn();
            Widget::Checkbox(TEXT("Weapon.InfiniteAmmo"), &m_bInfiniteAmmo);
#endif
            if (Widget::Checkbox(TEXT("Weapon.LongRange"), &m_bLongRange))
            {
                if (!m_bLongRange)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
#ifdef GTASA
            if (Widget::Checkbox(TEXT("Weapon.MoveWhenAiming"), &m_bMoveAim))
            {
                if (!m_bMoveAim)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
            if (Widget::Checkbox(TEXT("Weapon.MoveWhenFiring"), &m_bMoveFire))
            {
                if (!m_bMoveFire)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
            if (Widget::Checkbox(TEXT("Weapon.NoSpread"), &m_bNoSpread))
            {
                if (!m_bNoSpread)
                {
                    CWeaponInfo::LoadWeaponData();
                }
            }
            if (Widget::Checkbox(TEXT("Weapon.RapidFire"), &m_bRapidFire))
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
        if (ImGui::BeginTabItem(TEXT("Window.SpawnTab")))
        {
            ImGui::Spacing();
            if (ImGui::InputInt(TEXT("Weapon.Ammo"), &m_nAmmoCount))
            {
                m_nAmmoCount = (m_nAmmoCount < 0) ? 0 : m_nAmmoCount;
                m_nAmmoCount = (m_nAmmoCount > 99999) ? 99999 : m_nAmmoCount;
            }
#ifdef GTASA
            Widget::ImageList(m_WeaponData, GiveWeaponToPlayer,
            [](std::string& str)
            {
                return m_WeaponData.m_pData->Get(str.c_str(), "Unknown");
            },
            [](std::string& str)
            {
                return str != "0"; /*Unarmed*/
            }, AddWeapon);
#else
            Widget::DataList(m_WeaponData, GiveWeaponToPlayer, AddWeapon);
#endif
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT("Weapon.GangWeaponEditor")))
        {
            ImGui::Spacing();
            static const char* gangList =
            {
                "Ballas\0Grove street families\0Los santos vagos\0San fierro rifa\0Da nang boys\0"
                "Mafia\0Mountain cloud triad\0Varrio los aztecas\0Gang9\0Gang10\0"
            };
            ImGui::Combo(TEXT("Weapon.SelectGang"), &m_nSelectedGang, gangList);
            ImGui::Combo(TEXT("Ped.SelectWeapon"), &m_nSelectedWeapon, "Weapon 1\0Weapon 2\0Weapon 3\0");
            ImGui::Spacing();

            std::string key = std::to_string(m_nGangWeaponList[m_nSelectedGang][m_nSelectedWeapon]);
            ImGui::Text(TEXT("Weapon.CurrentWeapon"), m_WeaponData.m_pData->Get(key.c_str(), "Unknown").c_str());
            ImGui::Spacing();
            Widget::ImageList(m_WeaponData, SetGangWeapon,
            [](std::string& str){
                return m_WeaponData.m_pData->Get(str.c_str(), "Unknown");
            },
            [](std::string& str){
                return str != "-1"; /*Jetpack*/
            });
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
