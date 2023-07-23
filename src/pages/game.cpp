#include "pch.h"
#include "menu.h"
#include "game.h"
#include "utils/widget.h"
#include "utils/util.h"

#ifdef GTASA
#include <CMessages.h>
#include <CSprite2d.h>
#include <CAERadioTrackManager.h>
#include "custom/freecam_sa.h"
#include "custom/randomcheats_sa.h"
#include "custom/topdowncam_sa.h"

static bool bSaveGameFlag = false;
#endif

static void RealTimeClock()
{
    time_t tmp = time(nullptr);
    struct tm* now = localtime(&tmp);

#ifdef GTASA
    static int lastday;
    if (now->tm_yday != lastday)
    {
        CStats::SetStatValue(0x86, CStats::GetStatValue(0x86) + 1.0f);
        lastday = now->tm_yday;
    }

    CClock::ms_nGameClockMonth = now->tm_mon + 1;
    CClock::ms_nGameClockDays = now->tm_mday;
    CClock::CurrentDay = now->tm_wday + 1;
#endif

    CClock::ms_nGameClockHours = now->tm_hour;
    CClock::ms_nGameClockMinutes = now->tm_min;
    CClock::ms_nGameClockSeconds = now->tm_sec;
}

GamePage& gamePage = GamePage::Get();

GamePage::GamePage()
    : IPage<GamePage>(ePageID::Game, "Game", true)
{
#ifdef GTASA

    Events::drawMenuBackgroundEvent += []()
    {
        if (bSaveGameFlag)
        {
            FrontEndMenuManager.m_nCurrentMenuPage = MENUPAGE_CHOOSE_SAVE_SLOT;
            bSaveGameFlag = false;
        }
    };
#endif
    
    Events::processScriptsEvent += [this]
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* pPlayer = FindPlayerPed();
        int hplayer = CPools::GetPedRef(pPlayer);

        if (m_HardMode.m_bEnabled)
        {
            if (pPlayer->m_fHealth > 50.0f)
            {
                pPlayer->m_fHealth = 50.0f;
            }

            pPlayer->m_fArmour = 0.0f;

#ifdef GTASA
            CStats::SetStatValue(STAT_MAX_HEALTH, 350.0f);
            CStats::SetStatValue(STAT_STAMINA, 0.0f);
#endif
        }

#ifdef GTASA
        if (m_bScreenShot)
        {
            if (quickSceenShot.Pressed())
            {
                injector::WriteMemory<BYTE>(0xBA6830, 1, true);
                Command<Commands::TAKE_PHOTO>();
                Util::SetMessage(TEXT("Game.ScreenshotTaken"));
            }
        }

        static int m_nSolidWaterObj;
        if (m_bSolidWater)
        {
            CVector pos = pPlayer->GetPosition();

            float waterHeight = 0;
            Command<Commands::GET_WATER_HEIGHT_AT_COORDS>(pos.x, pos.y, false, &waterHeight);

            if (!Command<Commands::IS_CHAR_IN_ANY_BOAT>(hplayer) && waterHeight != -1000.0f && pos.z > (waterHeight))
            {
                if (m_nSolidWaterObj == 0)
                {
                    Command<Commands::CREATE_OBJECT>(3095, pos.x, pos.y, waterHeight, &m_nSolidWaterObj);
                    Command<Commands::SET_OBJECT_VISIBLE>(m_nSolidWaterObj, false);
                    if (pos.z < (waterHeight + 1))
                    {
                        pPlayer->SetPosn(pos.x, pos.y, waterHeight + 1);
                    }
                }
                else
                {
                    Command<Commands::SET_OBJECT_COORDINATES>(m_nSolidWaterObj, pos.x, pos.y, waterHeight);
                }
            }
        }
        else
        {
            if (m_nSolidWaterObj)
            {
                Command<Commands::DELETE_OBJECT>(m_nSolidWaterObj);
                m_nSolidWaterObj = 0;
            }
        }

        if (freeCam.Pressed())
        {
            if (Freecam.Toggle())
            {
                // restore lock camera zoom here
                if (m_bLockCameraZoom)
                {
                    TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_nCameraZoom, 250, true);
                }
                else
                {
                    Command<Commands::CAMERA_PERSIST_FOV>(false);
                }
            }
        }
#endif

        // improve this later
        static uint syncTimer;
        if (m_bSyncTime && timer - syncTimer > 50)
        {
            std::time_t t = std::time(nullptr);
            std::tm* now = std::localtime(&t);

            CClock::ms_nGameClockHours = now->tm_hour;
            CClock::ms_nGameClockMinutes = now->tm_min;

            syncTimer = timer;
        }
    };
}

void SetPlayerMission(std::string& rootkey, std::string& name, std::string& id)
{
    CPlayerPed* player = FindPlayerPed();
    uint hplayer = CPools::GetPedRef(player);
    int interior = 0;

#ifndef GTA3
    Command<0x09E8>(hplayer, &interior);
#endif

    if (BY_GAME(Util::IsOnMission(), true, true) && interior == 0)
    {
        player->SetWantedLevel(0);
        Command<Commands::LOAD_AND_LAUNCH_MISSION_INTERNAL>(std::stoi(id));
    }
    else
    {
        Util::SetMessage(TEXT("Game.MissionStartFailed"));
    }
}

void GamePage::Draw()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);

#ifdef GTASA
    if (ImGui::Button(TEXT("Game.SaveGame"), Widget::CalcSize()))
    {
        FrontEndMenuManager.m_bStartUpFrontEndRequested = true;
        bSaveGameFlag = true;
    }
    ImGui::Spacing();
#endif

    if (ImGui::BeginTabBar("Game", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT( "Window.ToggleTab")))
        {
            ImGui::BeginChild("##Checkbox");
            ImGui::Spacing();
            ImGui::Columns(2, nullptr, false);
            if (Widget::Toggle(TEXT("Game.DisableCheats"), &m_bDisableCheats))
            {
                if (m_bDisableCheats)
                {
#ifdef GTASA
                    patch::Set<BYTE>(0x4384D0, 0xE9, false);
                    patch::SetInt(0x4384D1, 0xD0, false);
                    patch::Nop(0x4384D5, 4, false);
#elif GTAVC
                    patch::Nop(0x602BD8, 5);
                    patch::Nop(0x602BE7, 5);
#else
                    patch::Nop(0x5841B8, 5);
                    patch::Nop(0x5841C7, 5);
#endif
                }
                else
                {
#ifdef GTASA
                    patch::Set<BYTE>(0x4384D0, 0x83, false);
                    patch::SetInt(0x4384D1, -0x7DF0F908, false); // correct?
                    patch::SetInt(0x4384D5, 0xCC, false);
#elif GTAVC
                    patch::SetRaw(0x602BD8, (char*)"\x88\xD8\x89\xF1\x50", 5);
                    patch::SetRaw(0x602BE7, (char*)"\xE8\x34\x91\xEA\xFF", 5);
#else
                    patch::SetRaw(0x5841B8, (char*)"\x88\xD8\x89\xF1\x50", 5);
                    patch::SetRaw(0x5841C7, (char*)"\xE8\x84\xE2\xF0\xFF", 5);
#endif
                }
            }
            if (Widget::Toggle(TEXT("Game.DisableReplay"), &m_bDisableReplay))
            {
                if (m_bDisableReplay)
                {
                    patch::SetUChar(BY_GAME(0x460500, 0x624EC0, 0x593170), 0xC3);
                }
                else
                {
                    patch::SetUChar(BY_GAME(0x460500, 0x624EC0, 0x593170), BY_GAME(0xA0, 0x80, 0x80));
                }
            }

            Widget::ToggleAddr<int8_t>(TEXT("Game.FasterClock"), BY_GAME(0x96913B, 0xA10B87, 0x95CDBB));
#ifdef GTASA
            if (Widget::Toggle(TEXT("Game.ForbiddenWantedLevel"), &m_bForbiddenArea, TEXT("Game.ForbiddenWantedLevelText")))
            {
                patch::Set<BYTE>(0x441770, m_bForbiddenArea ? 0x83 : 0xC3);
            }
            Widget::ToggleAddr<int8_t>(TEXT("Game.FreePNS"), 0x96C009);
#endif

#ifdef GTAVC
            ImGui::NextColumn();
#endif
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Game.FreezeGame"), 0xA10B48);
#endif
            if (Widget::Toggle(TEXT("Game.FreezeGameTime"), &m_bFreezeTime))
            {
                if (m_bFreezeTime)
                {
                    patch::SetRaw(BY_GAME(0x52CF10, 0x487010, 0x473460), (char*)"\xEB\xEF", 2);
                }
                else
                {
                    patch::SetRaw(BY_GAME(0x52CF10, 0x487010, 0x473460), (char*)BY_GAME("\x56\x8B", "\x6A\x01", "\x6A\x01"), 2);
                }
            }

#ifdef GTASA
            ImGui::NextColumn();
#endif
            if (Widget::Toggle("Freeze misson timer", &m_bMissionTimer))
            {
                Command<Commands::FREEZE_ONSCREEN_TIMER>(m_bMissionTimer);
            }
            if (Widget::Toggle(TEXT("Game.HardMode"), &m_HardMode.m_bEnabled, TEXT("Game.HardModeText")))
            {
                CPlayerPed* player = FindPlayerPed();

                if (m_HardMode.m_bEnabled)
                {
                    m_HardMode.m_fBacArmour = player->m_fArmour;
                    m_HardMode.m_fBacHealth = player->m_fHealth;

#ifdef GTASA
                    m_HardMode.m_fBacMaxHealth = CStats::GetStatValue(STAT_MAX_HEALTH);
                    m_HardMode.m_fBacStamina = CStats::GetStatValue(STAT_STAMINA);
#else
                    m_HardMode.m_fBacMaxHealth = 100.0f;
#endif
                    player->m_fHealth = 50.0f;
                }
                else
                {
                    player->m_fArmour = m_HardMode.m_fBacArmour;

#ifdef GTASA
                    CStats::SetStatValue(STAT_STAMINA, m_HardMode.m_fBacStamina);
                    CStats::SetStatValue(STAT_MAX_HEALTH, m_HardMode.m_fBacMaxHealth);
#endif
                    player->m_fHealth = m_HardMode.m_fBacHealth;
                }
            }
#ifdef GTASA
            if (Widget::Toggle(TEXT("Game.MobileRadio"), &m_bMobileRadio))
            {
                // AERadioTrackManager.StartRadio(5, 0, 0, 0);
                CallMethodAndReturn<int, 0x4EB3C0, int, int, int, int, int>((int)&AERadioTrackManager, 5, 0, 0, 0);
                patch::Nop(0x576C4E, 30);
                // CVehicle *pVeh = FindPlayerVehicle(-1, false);
                // static tVehicleAudioSettings settings;
                // if (m_bMobileRadio)
                // {
                //     patch::PutRetn(0x4F5BA0);
                //     patch::PutRetn(0x4FCF40);
                //     patch::PutRetn(0x4F5B60);
                //     patch::PutRetn(0x4F5700);

                //     patch::PutRetn(0x4E9820);

                //     if (!pVeh)
                //     {
                //         settings.m_nRadioType = RADIO_CIVILIAN;
                //         settings.m_nRadioID = RADIO_K_DST;
                //         settings.m_fBassEq = 1.0;
                //         settings.m_nBassSetting = 1;
                //         CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio = &settings;
                //         // CAEVehicleAudioEntity::s_pPlayerDriver = FindPlayerPed();
                //         CallMethod<0x4EB550, int, int>(0x8CB6F8, (int)&settings);
                //     }
                // }
                // else
                // {
                //     patch::Set<uint8_t>(0x4F5BA0, 0x90);
                //     patch::Set<uint8_t>(0x4FCF40, 0x51);
                //     patch::Set<uint8_t>(0x4F5B60, 0x0F);
                //     patch::Set<uint8_t>(0x4F5700, 0x51);
                //     CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio = nullptr;
                //     // CAEVehicleAudioEntity::s_pPlayerDriver = nullptr;
                //     // AERadioTrackManager.StopRadio(&settings, 0);
                //     Call<0x4E9820, int, int, int>((int)&AERadioTrackManager, 0, 0);
                // }
            }
            if (Widget::Toggle(TEXT("Game.NoWaterPhysics"), &m_bNoWaterPhysics))
            {
                if (m_bNoWaterPhysics)
                {
                    patch::Set<uint8_t>(0x6C2759, 1, true);
                }
                else
                {
                    patch::Set<uint8_t>(0x6C2759, 0, true);
                }
            }
            Widget::Toggle(TEXT("Game.Screenshot"), &m_bScreenShot,
                             std::format("{} {}", TEXT("Game.ScreenshotTip"),
                                         quickSceenShot.GetNameString()).c_str());
            Widget::Toggle(TEXT("Game.SolidWater"), &m_bSolidWater, TEXT("Game.SolidWaterText"));
#endif
            if (Widget::Toggle(TEXT("Game.SyncSystemTime"), &m_bSyncTime))
            {
                if (m_bSyncTime)
                {
                    patch::RedirectCall(BY_GAME(0x53BFBD, 0x4A44F7, 0x48C8EB), &RealTimeClock);
                }
                else
                {
                    patch::RedirectCall(BY_GAME(0x53BFBD, 0x4A44F7, 0x48C8EB), &CClock::Update);
                }
            }

            ImGui::Columns(1);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Window.MenusTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("##Menus");
#ifdef GTASA
            if (ImGui::CollapsingHeader((TEXT_S("Game.CameraZoom") + "##HEADER").c_str()))
            {
                ImGui::Spacing();
                if (Freecam.GetState())
                {
                    ImGui::TextWrapped(TEXT("Game.CameraZoomLockFreecam"));
                }
                else
                {
                    if (Widget::Toggle(TEXT("Game.CameraZoomLock"), &m_bLockCameraZoom))
                    {
                        if (!m_bLockCameraZoom)
                        {
                            Command<Commands::CAMERA_PERSIST_FOV>(false);
                        }
                    }
                    ImGui::Spacing();
                    if (!m_bLockCameraZoom)
                    {
                        ImGui::BeginDisabled();
                    }
                    if (ImGui::SliderInt(TEXT("Game.CameraZoom"), &m_nCameraZoom, 5, 120))
                    {
                        TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_nCameraZoom, 250, true);
                        Command<Commands::CAMERA_PERSIST_FOV>(true);
                    }
                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Game.ResetDefault"), Widget::CalcSize()))
                    {
                        m_nCameraZoom = 70.0f;
                        TheCamera.LerpFOV(TheCamera.FindCamFOV(), 70.0f, 250, true);
                        Command<Commands::CAMERA_PERSIST_FOV>(true);
                    }

                    if (m_bLockCameraZoom)
                    {
                        ImGui::TextWrapped(TEXT("Game.CameraZoomLockInfo"));
                    }
                    else
                    {
                        ImGui::EndDisabled();
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader(TEXT("Game.ChangeRadioStation")))
            {
                static const char* channels =
                    "Playback FM\0KRose\0KDST\0Bounce FM\0SFUR\0Radio Los Santos\0Radio X\0CSR\0KJah West\0"
                    "Master Sounds\0WCTR\0User Tracks\0None\0";

                int curStation = Command<Commands::GET_RADIO_CHANNEL>();
                if (ImGui::Combo(TEXT("Game.CurrentStation"), &curStation, channels))
                {
                    Command<Commands::SET_RADIO_CHANNEL>(curStation);
                }
                ImGui::Spacing();
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader(TEXT("Game.CurrentDay")))
            {
                int day = CClock::CurrentDay - 1;
                if (ImGui::Combo(TEXT("Game.SelectDay"), &day, "Sunday\0Monday\0Tuesday\0Wednesday\0Thursday\0Friday\0Saturday\0"))
                {
                    CClock::CurrentDay = day + 1;
                }

                ImGui::Spacing();
                ImGui::Separator();
            }
#endif
            Widget::EditAddr<int>(TEXT("Game.DaysPassed"), BY_GAME(0xB79038, 0x97F1F4, 0x8F2BB8), 0, 9999);
            Widget::EditAddr<int>(TEXT("Game.FPSLimit"), (uint)&(RsGlobal.maxFPS), 1, 30, 999);
            Widget::EditAddr<float>(TEXT("Game.GameSpeed"), reinterpret_cast<uint>(&CTimer::ms_fTimeScale), 1, 1, 10);
            Widget::EditAddr(TEXT("Game.Gravity"), BY_GAME(0x863984, 0x68F5F0, 0x5F68D4), -1.0f, 0.008f, 1.0f, 1.0f, 0.01f);

            if (ImGui::CollapsingHeader(TEXT("Game.SetTime")))
            {
                int hour = CClock::ms_nGameClockHours;
                int minute = CClock::ms_nGameClockMinutes;

                ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth()/2);
                if (ImGui::InputInt(TEXT("Game.Hour"), &hour))
                {
                    if (hour < 0) hour = 23;
                    if (hour > 23) hour = 0;
                    CClock::ms_nGameClockHours = hour;
                }

                if (ImGui::InputInt(TEXT("Game.Minute"), &minute))
                {
                    if (minute < 0) minute = 59;
                    if (minute > 59) minute = 0;
                    CClock::ms_nGameClockMinutes = minute;
                }
#ifdef GTASA
                static int min = 24;
                if (ImGui::InputInt(TEXT("Game.TotalMinutesDay"), &min))
                {
                    int val = min * 41.666666667f;
                    patch::Set<uint32_t>(0x5BA35F, val, true);
                    patch::Set<uint32_t>(0x53BDEC, val, true);
                }
#endif
                ImGui::PopItemWidth();
                ImGui::Spacing();
                ImGui::Separator();
            }
#ifdef GTASA
            static std::vector<Widget::BindInfo> themes
            {
                {TEXT("Game.Beach"), 0x969159}, {TEXT("Game.Country"), 0x96917D}, {TEXT("Game.FunHouse"), 0x969176}, {TEXT("Game.Ninja"), 0x96915C}
            };
            Widget::EditRadioBtnAddr(TEXT("Game.Themes"), themes);
            if (ImGui::CollapsingHeader(TEXT("Player.TopDownCamera")))
            {
                bool state = TopDownCam.GetState();
                if (Widget::Toggle(TEXT("Window.Enabled"), &state))
                {
                    Command<Commands::RESTORE_CAMERA_JUMPCUT>();
                    TopDownCam.Toggle();
                }
                ImGui::Spacing();
                ImGui::SliderInt(TEXT("Player.CameraZoom"), &TopDownCam.m_nZoom, 20, 60);
                ImGui::Spacing();
                ImGui::Separator();
            }
#endif
            if (ImGui::CollapsingHeader(TEXT("Game.Weather")))
            {
#ifdef GTASA
                if (ImGui::Button(TEXT("Game.Foggy"), Widget::CalcSize(3)))
                {
                    Call<0x438F80>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Overcast"), Widget::CalcSize(3)))
                {
                    Call<0x438F60>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Rainy"), Widget::CalcSize(3)))
                {
                    Call<0x438F70>();
                }

                if (ImGui::Button(TEXT("Game.Sandstorm"), Widget::CalcSize(3)))
                {
                    Call<0x439590>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Thunderstorm"), Widget::CalcSize(3)))
                {
                    Call<0x439570>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.VerySunny"), Widget::CalcSize(3)))
                {
                    Call<0x438F50>();
                }

                ImGui::Spacing();
                static int weatherID = 0;
                if (ImGui::InputInt(TEXT("Game.WeatherID"), &weatherID))
                {
                    if (weatherID < 0)
                    {
                        weatherID = 255;
                    }

                    if (weatherID > 255)
                    {
                        weatherID = 0;
                    }
                    CWeather::OldWeatherType = weatherID;
                    CWeather::NewWeatherType = weatherID;
                }
                Widget::Tooltip(TEXT("Game.WeatherIDText"));
#else
                if (ImGui::Button(TEXT("Game.Sunny"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(0);
                }
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Cloudy"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(1);
                }
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Rainy"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(2);
                }

                if (ImGui::Button(TEXT("Game.Foggy"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(3);
                }
#ifdef GTAVC
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.ExtraSunny"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(4);
                }
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Hurricane"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(5);
                }

                if (ImGui::Button(TEXT("Game.ExtraColors"), Widget::CalcSize(3)))
                {
                    CWeather::ForceWeatherNow(6);
                }
#endif
#endif
                ImGui::Spacing();
                ImGui::Separator();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT( "Game.Freecam")))
        {
            ImGui::Spacing();
            bool state = Freecam.GetState();
            if (Widget::Toggle(TEXT("Game.Enable"), &state))
            {
                if (Freecam.Toggle())
                {
                    // restore lock camera zoom here
                    if (m_bLockCameraZoom)
                    {
                        TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_nCameraZoom, 250, true);
                    }
                    else
                    {
                        Command<Commands::CAMERA_PERSIST_FOV>(false);
                    }
                }
            }
            ImGui::Spacing();

            if (ImGui::SliderFloat(TEXT("Game.FieldOfView"), &Freecam.m_fFOV, 5.0f, 120.0f) && Freecam.GetState())
            {
                TheCamera.LerpFOV(TheCamera.FindCamFOV(), Freecam.m_fFOV, 250, true);
            }
            ImGui::SliderInt(TEXT("Game.MovementSpeed"), &Freecam.m_nMul, 1, 10);
            ImGui::Dummy(ImVec2(0.0f, 20.0f));

            ImGui::BeginChild("Conrtls");
            if (ImGui::BeginTable("FreecamCOntorls", 2, ImGuiTableFlags_ScrollY))
            {
                ImGui::TableSetupColumn(TEXT("Game.KeyAction"));
                ImGui::TableSetupColumn(TEXT("Game.KeyCombo"));
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Forward");
                ImGui::TableNextColumn();
                ImGui::Text(freeCamForward.GetNameString().c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Backward");
                ImGui::TableNextColumn();
                ImGui::Text(freeCamBackward.GetNameString().c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("");
                ImGui::TableNextColumn();
                ImGui::Text("");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Forward slide");
                ImGui::TableNextColumn();
                ImGui::Text((freeCamForward.GetNameString() + " + Space").c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Backward slide");
                ImGui::TableNextColumn();
                ImGui::Text((freeCamForward.GetNameString() + " + Space").c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Left slide");
                ImGui::TableNextColumn();
                ImGui::Text(freeCamLeft.GetNameString().c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Right slide");
                ImGui::TableNextColumn();
                ImGui::Text(freeCamRight.GetNameString().c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("");
                ImGui::TableNextColumn();
                ImGui::Text("");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Up");
                ImGui::TableNextColumn();
                ImGui::Text((freeCamForward.GetNameString() + " + Ctrl").c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Down");
                ImGui::TableNextColumn();
                ImGui::Text((freeCamBackward.GetNameString() + " + Ctrl").c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("");
                ImGui::TableNextColumn();
                ImGui::Text("");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("2x faster");
                ImGui::TableNextColumn();
                ImGui::Text("Shift");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("2x slower");
                ImGui::TableNextColumn();
                ImGui::Text("Alt");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Change movespeed");
                ImGui::TableNextColumn();
                ImGui::Text("Mouse wheel");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Change zoom");
                ImGui::TableNextColumn();
                ImGui::Text("Ctrl + Mouse wheel");

                ImGui::EndTable();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
#endif
        if (ImGui::BeginTabItem(TEXT( "Game.Missions")))
        {
            ImGui::Spacing();

            static bool bMissionLoaderWarningShown;
            if (!bMissionLoaderWarningShown)
            {
                ImGui::TextWrapped(TEXT("Game.MissionLoaderTip"));
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Game.ShowLoader"), ImVec2(Widget::CalcSize())))
                {
                    bMissionLoaderWarningShown = true;
                }
            }
            else
            {
                if (ImGui::Button(TEXT("Game.FailMission"), ImVec2(Widget::CalcSize())))
                {
                    if (!Util::IsOnCutscene())
                    {
                        Command<Commands::FAIL_CURRENT_MISSION>();
                    }
                }

                ImGui::Spacing();

                Widget::DataList(m_MissionData, SetPlayerMission);
            }
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT( "Game.Stats")))
        {
            ImGui::Spacing();

            if (ImGui::Button(TEXT("Game.MaxWepSkills"), Widget::CalcSize(2)))
            {
                for (size_t i = 69; i != 80; ++i)
                {
                    gConfig.Set(std::format("Stats.{}", i).c_str(), 1000);
                    CStats::SetStatValue((unsigned short)i, 1000);
                }
                CHud::GetRidOfAllHudMessages(true);
                Util::SetMessage(TEXT("Game.MaxWepSkillsText"));
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Game.MaxVehSkills"), Widget::CalcSize(2)))
            {
                CStats::SetStatValue(160, 1000);
                gConfig.Set("Stats.160", 1000);
                CStats::SetStatValue(223, 1000);
                gConfig.Set("Stats.223", 1000);
                CStats::SetStatValue(229, 1000);
                gConfig.Set("Stats.229", 1000);
                CStats::SetStatValue(230, 1000);
                gConfig.Set("Stats.230", 1000);
                CHud::GetRidOfAllHudMessages(true);
                Util::SetMessage(TEXT("Game.MaxVehSkillsText"));
            }

            ImGui::Spacing();
            Widget::DataList(m_StatData, nullptr, nullptr, true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Game.RandomCheats")))
        {
            ImGui::Spacing();
            ImGui::Columns(2, NULL, false);

            bool state = RandomCheats.GetState();
            if (Widget::Toggle(TEXT("Game.Enable"), &state))
            {
                RandomCheats.Toggle();
            }
            ImGui::NextColumn();
            Widget::Toggle(TEXT("Game.ProgressBar"), &RandomCheats.m_bProgressBar);
            ImGui::Columns(1);
            ImGui::Spacing();

            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);

            ImGui::SliderInt(TEXT("Game.ActivateTimer"), &RandomCheats.m_nInterval, 5, 60);
            Widget::Tooltip(TEXT("Game.ActivateTimerText"));

            ImGui::PopItemWidth();

            ImGui::TextWrapped(TEXT("Game.SelectCheats"));
            ImGui::Separator();
            if (ImGui::BeginChild("Cheats list"))
            {
                RandomCheats.DrawList();
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
