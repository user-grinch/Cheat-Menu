#include "pch.h"
#include "ped.h"
#include "ui.h"
#include "util.h"
#include "weapon.h"
#include <CPopulation.h>

#ifdef GTASA
#include <ePedBones.h>
#endif

void Ped::Init()
{
#ifdef GTASA
    if (GetModuleHandle("ExGangWars.asi"))
    {
        m_bExGangWarsInstalled = true;
    }
#endif

    /*
    	Taken from gta chaos mod by Lordmau5 & _AG
    	TODO: Implement in VC too
    */
#ifdef GTASA
    Events::pedRenderEvent += [](CPed *ped)
    {

        if (m_bBigHead)// || m_bThinBody)
        {
            auto animHier = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
            auto matrices = RpHAnimHierarchyGetMatrixArray (animHier);

            RwV3d scale = {0.7f, 0.7f, 0.7f};
            if (m_bThinBody)
            {
                for (int i = 1; i <= 54; i++)
                {
                    RwMatrixScale (&matrices[RpHAnimIDGetIndex (animHier, i)], &scale, rwCOMBINEPRECONCAT);
                }
            }
            scale = {3.0f, 3.0f, 3.0f};
            if (m_bBigHead)
            {
                for (int i = 5; i <= 8; i++)
                {
                    RwMatrixScale (&matrices[RpHAnimIDGetIndex (animHier, i)], &scale, rwCOMBINEPRECONCAT);
                }
            }
        }
    };
#elif GTA3
    CdeclEvent <AddressList<0x4CFE12, H_CALL>, PRIORITY_AFTER, ArgPickN<CPed*, 0>, void(CPed*)> onPreRender;
    
    onPreRender += [](CPed* ped) 
    {
        if (!m_bBigHead)
        {
            return;
        }
        
        RwFrame* frame = ped->m_apFrames[2]->m_pFrame;

        if (frame) 
        {
            RwMatrix* headMatrix = RwFrameGetMatrix(frame);

            if (headMatrix) 
            {
                CMatrix mat;
                mat.m_pAttachMatrix = NULL;
                mat.Attach(headMatrix, false);
                mat.SetScale(3.0f);
                mat.SetTranslateOnly(0.4f, 0.0f, 0.0f);
                mat.UpdateRW();
            }
        }
    };
#endif
}

#ifdef GTASA
void Ped::SpawnPed(std::string& model)
#else
void Ped::SpawnPed(std::string& cat, std::string& name, std::string& model)
#endif
{
    if (SpawnPed::m_List.size() == SPAWN_PED_LIMIT)
    {
        SetHelpMessage(TEXT("Ped.MaxLimit"));
        return;
    }

    if (BY_GAME(m_PedData.m_pData->Contains(model.c_str()), true, true))
    {
        CPlayerPed* player = FindPlayerPed();
        CVector pos = player->GetPosition();
        pos.y += 1;

        CPed* ped;
        int hplayer;
        static size_t currentSlot = 1;

#ifdef GTASA
        if (m_SpecialPedData.Contains(model.c_str()))
        {
            std::string name;
            if (m_SpecialPedData.Contains(model.c_str()))
            {
                name = m_SpecialPedData.Get(model.c_str(), "Unknown");
            }
            else
            {
                name = model;
            }

            CStreaming::RequestSpecialChar(currentSlot, name.c_str(), PRIORITY_REQUEST);
            CStreaming::LoadAllRequestedModels(true);

            Command<Commands::CREATE_CHAR>(SpawnPed::m_nSelectedPedType + 4, 290 + currentSlot, pos.x, pos.y, pos.z + 1, &hplayer);
            CStreaming::SetSpecialCharIsDeletable(290 + currentSlot);

            // SA has 10 slots
            ++currentSlot;
            if (currentSlot > 9)
            {
                currentSlot = 1;
            }
        }
#else
        if (cat == "Special") // Special model
        {
#ifdef GTA3
            SetHelpMessage(TEXT("Player.SpecialNotImplement"));
            return;
#else
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

            Command<Commands::CREATE_CHAR>(SpawnPed::m_nSelectedPedType + 4, iModel, pos.x, pos.y, pos.z + 1, &hplayer);
            CStreaming::SetModelIsDeletable(iModel);
        }

        ped = CPools::GetPed(hplayer);

        if (SpawnPed::m_bPedMove)
        {
            SpawnPed::m_List.push_back(ped);
        }
        else
        {
            Command<Commands::MARK_CHAR_AS_NO_LONGER_NEEDED>(hplayer);
        }
        ped->m_nPedFlags.bPedIsBleeding = SpawnPed::m_bPedBleed;
        ped->m_nWeaponAccuracy = SpawnPed::m_nAccuracy;
        ped->m_fHealth = SpawnPed::m_nPedHealth;
#ifdef GTASA
        if (SpawnPed::m_nWeaponId != 0)
        {
            int model = 0;
            Command<Commands::GET_WEAPONTYPE_MODEL>(SpawnPed::m_nWeaponId, &model);
            CStreaming::RequestModel(model, PRIORITY_REQUEST);
            CStreaming::LoadAllRequestedModels(false);
            Command<Commands::GIVE_WEAPON_TO_CHAR>(hplayer, SpawnPed::m_nWeaponId, 999);
        }
#endif
    }
}

void Ped::ShowPage()
{
    if (ImGui::BeginTabBar("Ped", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Columns(2, 0, false);
#ifndef GTAVC
            Ui::CheckboxWithHint(TEXT("Ped.BigHead"), &m_bBigHead);
#endif
#ifdef GTASA
            Ui::CheckboxAddress(TEXT("Ped.ElvisEverywhere"), 0x969157);
            Ui::CheckboxAddress(TEXT("Ped.EveryoneArmed"), 0x969140);
            Ui::CheckboxAddress(TEXT("Ped.GangsControl"), 0x96915B);
            Ui::CheckboxAddress(TEXT("Ped.GangsEverywhere"), 0x96915A);
            Ui::CheckboxWithHint(TEXT("Ped.GangWars"), &CGangWars::bGangWarsActive);

            ImGui::NextColumn();

            Ui::CheckboxAddress(TEXT("Ped.PedsMayhem"), 0x96913E);
            Ui::CheckboxAddress(TEXT("Ped.PedsAtkRocket"), 0x969158);
            Ui::CheckboxAddress(TEXT("Ped.PedsRiot"), 0x969175);
            Ui::CheckboxAddress(TEXT("Ped.SlutMagnet"), 0x96915D);
            Ui::CheckboxWithHint(TEXT("Ped.ThinBody"), &m_bThinBody);
#elif GTAVC
            Ui::CheckboxAddress(TEXT("Ped.NoProstitutes"), 0xA10B99);
            Ui::CheckboxAddress(TEXT("Ped.SlutMagnet"), 0xA10B5F);
            ImGui::NextColumn();
            Ui::CheckboxAddress(TEXT("Ped.WeaponAll"), 0xA10AB3);
#else
            // Bad idea lol
            static bool pedsMayhem;
            if (Ui::CheckboxWithHint(TEXT("Ped.PedsMayhem"), &pedsMayhem))
            {
                Call<0x4911C0>();
            }
            static bool everyoneAttacksPlayer;
            if (Ui::CheckboxWithHint(TEXT("Ped.EveryoneAtk"), &everyoneAttacksPlayer))
            {
                Call<0x491270>();
            }
            ImGui::NextColumn();
            Ui::CheckboxAddress(TEXT("Ped.NastyLimbs"), 0x95CD44);
            Ui::CheckboxAddress(TEXT("Ped.WeaponAll"), 0x95CCF6);
#endif
            ImGui::Columns(1);
            ImGui::EndChild();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("MenusChild");

#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Ped.GangWars")))
            {
                if (ImGui::Button(TEXT("Ped.StartWar"), ImVec2(Ui::GetSize(2))))
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
                if (ImGui::Button(TEXT("Ped.EndWar"), ImVec2(Ui::GetSize(2))))
                {
                    CGangWars::EndGangWar(true);
                }

                ImGui::Dummy(ImVec2(0, 20));
                ImGui::TextWrapped(TEXT("Ped.ZoneDensity"));
                ImGui::Spacing();

                ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);
                for (int i = 0; i != 10; ++i)
                {
                    CVector pos = FindPlayerPed()->GetPosition();
                    CZone szone = CZone();
                    CZone* pZone = &szone;

                    CZoneInfo* zoneInfo = CTheZones::GetZoneInfo(&pos, &pZone);
                    int density = zoneInfo->m_nGangDensity[i];

                    if (ImGui::SliderInt(m_GangList[i].c_str(), &density, 0, 127))
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
                    ImGui::TextWrapped(TEXT("Ped.ExGangWarsTip"));
                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Ped.DownloadExGangWars"), Ui::GetSize(1)))
                    {
                        ShellExecute(NULL, "open", "https://gtaforums.com/topic/682194-extended-gang-wars/", NULL, NULL,
                                     SW_SHOWNORMAL);
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
            }
#endif
            Ui::EditReference<float>(TEXT("Ped.PedDensityMul"), CPopulation::PedDensityMultiplier, 0, 1, 10);
#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Ped.RecruitAnyone")))
            {
                static std::vector<Ui::NamedMemory> selectWeapon
                {
                    {"9mm", 0x96917C}, {"AK47", 0x96917D}, {"Rockets", 0x96917E}
                };
                Ui::RadioButtonAddress(TEXT("Ped.SelectWeapon"), selectWeapon);
                ImGui::Spacing();
                ImGui::Separator();
            }
#endif

            if (ImGui::CollapsingHeader(TEXT("Ped.RemovePedsRadius")))
            {
                static int removeRadius = 5;
                ImGui::InputInt(TEXT("Ped.Radius"), &removeRadius);
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Ped.RemovePeds"), Ui::GetSize(1)))
                {
                    CPlayerPed* player = FindPlayerPed();
                    for (CPed* ped : CPools::ms_pPedPool)
                    {
                        if (DistanceBetweenPoints(ped->GetPosition(), player->GetPosition()) < removeRadius
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
        if (ImGui::BeginTabItem(TEXT("Window.SpawnTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Ped.RemoveFrozen"), Ui::GetSize(1)))
            {
                for (CPed* ped : SpawnPed::m_List)
                {
                    CWorld::Remove(ped);
                    ped->Remove();
                }
                SpawnPed::m_List.clear();
            }
            ImGui::Spacing();
            if (ImGui::BeginTabBar("SpawnPedBar"))
            {
                ImGui::Spacing();

                if (ImGui::BeginTabItem(TEXT("Ped.SpawnerTab")))
                {
                    ImGui::Spacing();
#ifdef GTASA
                    Ui::DrawImages(m_PedData, SpawnPed, nullptr,
                                   [](std::string str)
                    {
                            return m_PedData.m_pData->Get(str.c_str(), "Unknown");
                    });
#else
                    Ui::DrawJSON(m_PedData, SpawnPed, nullptr);
#endif
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(TEXT("Ped.ConfigTab")))
                {
                    ImGui::Spacing();
                    ImGui::BeginChild("PedCOnfig");
                    ImGui::Columns(2, 0, false);
                    Ui::CheckboxWithHint(TEXT("Ped.NoMove"), &SpawnPed::m_bPedMove);
                    ImGui::NextColumn();
                    Ui::CheckboxWithHint(TEXT("Ped.PedBleed"), &SpawnPed::m_bPedBleed);
                    ImGui::Columns(1);

                    ImGui::Spacing();
                    ImGui::SliderInt(TEXT("Ped.Accuracy"), &SpawnPed::m_nAccuracy, 0.0, 100.0);
                    if (ImGui::InputInt(TEXT("Ped.Health"), &SpawnPed::m_nPedHealth))
                    {
                        if (SpawnPed::m_nPedHealth > 1000)
                        {
                            SpawnPed::m_nPedHealth = 1000;
                        }

                        if (SpawnPed::m_nPedHealth < 0)
                        {
                            SpawnPed::m_nPedHealth = 0;
                        }
                    }
                    Ui::ListBox(TEXT("Ped.PedType"), SpawnPed::m_PedTypeList, SpawnPed::m_nSelectedPedType);

                    ImGui::Spacing();
                    ImGui::Text(TEXT("Ped.SelectedWeapon"), SpawnPed::m_nWeaponName.c_str());
                    ImGui::Spacing();
#ifdef GTASA
                    Ui::DrawImages(Weapon::m_WeaponData,
                                   [](std::string str)
                    {
                        SpawnPed::m_nWeaponId = std::stoi(str);
                    },
                    nullptr,
                    [](std::string str)
                    {
                        SpawnPed::m_nWeaponName = Weapon::m_WeaponData.m_pData->Get(str.c_str(), "Unknown");
                        return SpawnPed::m_nWeaponName;
                    },
                    [](std::string str)
                    {
                        return str != "-1"; /*Jetpack*/
                    }
                                  );
#else
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
