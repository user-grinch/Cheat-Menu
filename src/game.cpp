#include "pch.h"
#include "menu.h"
#include "game.h"
#include "widget.h"
#include "util.h"
#ifdef GTASA
#include <CIplStore.h>
#include <CMessages.h>
#include <CSprite2d.h>
#endif

#ifdef GTASA
static bool bSaveGameFlag = false;
void Freecam::Process()
{
    static CVector m_fTotalMouse;

    int delta = (CTimer::m_snTimeInMilliseconds - CTimer::m_snPreviousTimeInMilliseconds);

    int ratio = 1 / (1 + (delta * m_nMul));
    int speed = m_nMul + m_nMul * ratio * delta;

    if (!m_bInitDone)
    {
        CPlayerPed* player = FindPlayerPed();
        Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, true);

        m_bHudState = patch::Get<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL)); // hud
        patch::Set<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL), 0); // hud
        m_bRadarState = patch::Get<BYTE>(BY_GAME(0xBA676C, 0xA10AB6, NULL)); // radar

        CVector playerPos = player->GetPosition();
        CPad::GetPad(0)->DisablePlayerControls = true;

        Command<Commands::CREATE_RANDOM_CHAR>(playerPos.x, playerPos.y, playerPos.z, &m_nPed);
        m_pPed = CPools::GetPed(m_nPed);

        m_fTotalMouse.x = player->GetHeading() + 89.6f;
        m_fTotalMouse.y = 0;
        playerPos.z -= 20;

        Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(m_nPed, true);
        Command<Commands::SET_LOAD_COLLISION_FOR_CHAR_FLAG>(m_nPed, false);

        m_pPed->m_bIsVisible = false;
        Command<Commands::SET_CHAR_COLLISION>(m_nPed, false);
        m_pPed->SetPosn(playerPos);
        TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_fFOV, 1000, true);
        Command<Commands::CAMERA_PERSIST_FOV>(true);
        patch::Set<BYTE>(0xBA676C, 2); // disable radar
        m_bInitDone = true;
    }

    CVector pos = m_pPed->GetPosition();

    m_fTotalMouse.x = m_fTotalMouse.x - CPad::NewMouseControllerState.x / 250;
    m_fTotalMouse.y = m_fTotalMouse.y + CPad::NewMouseControllerState.y / 3;

    if (m_fTotalMouse.x > 150)
    {
        m_fTotalMouse.y = 150;
    }

    if (m_fTotalMouse.y < -150)
    {
        m_fTotalMouse.y = -150;
    }

    if (freeCamTeleport.Pressed())
    {
        CPlayerPed* player = FindPlayerPed();
        CVector pos = m_pPed->GetPosition();

        CEntity* playerEntity = FindPlayerEntity(-1);
        pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &playerEntity) + 0.5f;
        Command<Commands::SET_CHAR_COORDINATES>(CPools::GetPedRef(player), pos.x, pos.y, pos.z);

        // disble them again cause they get enabled
        CHud::bScriptDontDisplayRadar = true;
        CHud::m_Wants_To_Draw_Hud = false;
        SetHelpMessage(TEXT("Game.PlayerTeleported"));
    }

    if (KeyPressed(VK_MENU))
    {
        speed /= 2;
    }

    if (KeyPressed(VK_SHIFT))
    {
        speed *= 2;
    }
        
    if (freeCamForward.PressedRealtime() || freeCamBackward.PressedRealtime())
    {
        if (freeCamBackward.PressedRealtime())
        {
            speed *= -1;
        }

        float angle;
        Command<Commands::GET_CHAR_HEADING>(m_nPed, &angle);
        pos.x += speed * cos(angle * 3.14159f / 180.0f);
        pos.y += speed * sin(angle * 3.14159f / 180.0f);
        
        if (!KeyPressed(VK_SPACE))
        {
            pos.z += speed * 2 * sin(m_fTotalMouse.y / 3 * 3.14159f / 180.0f);
        }
    }

    if (freeCamLeft.PressedRealtime() || freeCamRight.PressedRealtime())
    {
        if (freeCamLeft.PressedRealtime())
        {
            speed *= -1;
        }

        float angle;
        Command<Commands::GET_CHAR_HEADING>(m_nPed, &angle);
        angle -= 90;

        pos.x += speed * cos(angle * 3.14159f / 180.0f);
        pos.y += speed * sin(angle * 3.14159f / 180.0f);
    }

    if (CPad::NewMouseControllerState.wheelUp)
    {
        if (KeyPressed(VK_CONTROL) && m_nMul != 10)
        {
            if (m_fFOV > 10.0f)
            {
                m_fFOV -= 2.0f * speed;
            }

            TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_fFOV, 250, true);
            Command<Commands::CAMERA_PERSIST_FOV>(true);
        }
        else
        {
            if (m_nMul < 10)
            {
                ++m_nMul;
                SetHelpMessage(std::format("Speed: {}", m_nMul).c_str());
            }
        }
    }

    if (CPad::NewMouseControllerState.wheelDown)
    {
        if (KeyPressed(VK_CONTROL) && m_nMul != 1)
        {
            if (m_fFOV < 115.0f)
            {
                m_fFOV += 2.0f * speed;
            }

            TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_fFOV, 250, true);
            Command<Commands::CAMERA_PERSIST_FOV>(true);
        }
        else
        {
            if (m_nMul > 1)
            {
                --m_nMul;
                SetHelpMessage(std::to_string(m_nMul).c_str());
                SetHelpMessage(std::format("Speed: {}", m_nMul).c_str());
            }
        }
    }

    m_pPed->SetHeading(m_fTotalMouse.x);
    Command<Commands::ATTACH_CAMERA_TO_CHAR>(m_nPed, 0.0, 0.0, 20.0, 90.0, 180, m_fTotalMouse.y, 0.0, 2);
    m_pPed->SetPosn(pos);
    CIplStore::AddIplsNeededAtPosn(pos);
}

void Freecam::Clear()
{
    m_bInitDone = false;
    Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, false);
    patch::Set<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL), m_bHudState); // hud
    patch::Set<BYTE>(BY_GAME(0xBA676C, 0xA10AB6, NULL), m_bRadarState); // radar

    CPad::GetPad(0)->DisablePlayerControls = false;

    Command<Commands::DELETE_CHAR>(m_nPed);
    m_pPed = nullptr;

    Command<Commands::CAMERA_PERSIST_FOV>(false);
    Command<Commands::RESTORE_CAMERA_JUMPCUT>();
}

void RandomCheats::Process()
{
    static bool genCheats = false;
    if (!genCheats)
    {
            // Generate enabled cheats vector
        for (auto [k, v] : m_pData.Items())
        {
            /*
            [
                cheat_id = [ cheat_name, state (true/false) ]
            ]
            */
            std::string key { k.str() };
            m_EnabledCheats[std::stoi(key)][0] = v.value_or<std::string>("Unknown");
            m_EnabledCheats[std::stoi(key)][1] = "true";
        }
        genCheats = true;
    }
    
    if (m_bEnabled)
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        if ((timer - m_nTimer) > (static_cast<uint>(m_nInterval) * 1000))
        {
            int id = Random(0, 91);

            for (int i = 0; i < 92; i++)
            {
                if (i == id)
                {
                    if (m_EnabledCheats[i][1] == "true")
                    {
                        Call<0x438370>(id); // cheatEnableLegimate(int CheatID)
                        CMessages::AddMessage((char*)m_EnabledCheats[i][0].c_str(), 2000, 0, false);
                        m_nTimer = timer;
                    }
                    break;
                }
            }
        }
    }
}

void RandomCheats::DrawBar()
{
    if (m_bEnabled && m_bProgressBar)
    {
        // Next cheat timer bar
        uint screenWidth = screen::GetScreenWidth();
        uint screenHeight = screen::GetScreenHeight();
        uint timer = CTimer::m_snTimeInMilliseconds;
        uint totalTime = m_nInterval;
        float progress = (totalTime - (timer - m_nTimer) / 1000.0f) / totalTime;

        CRect sizeBox = CRect(0,0, screenWidth, screenHeight/50);
        CRect sizeProgress = CRect(0,0, screenWidth*progress, screenHeight/50);
        CRGBA colorBG = CRGBA(24, 99, 44, 255);
        CRGBA colorProgress = CRGBA(33, 145, 63, 255);

        CSprite2d::DrawRect(sizeBox, colorBG);
        CSprite2d::DrawRect(sizeProgress, colorProgress);
    }
}

void RandomCheats::DrawList()
{
    for (std::string* element : m_EnabledCheats)
    {
        bool selected = (element[1] == "true") ? true : false;

        if (ImGui::MenuItem(element[0].c_str(), nullptr, selected))
        {
            element[1] = selected ? "false" : "true";
        }
    }
}
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

void Game::Init()
{
#ifdef GTASA
    Events::drawMenuBackgroundEvent += []()
    {
        if (bSaveGameFlag)
        {
            FrontEndMenuManager.m_nCurrentMenuPage = MENUPAGE_GAME_SAVE;
            bSaveGameFlag = false;
        }
    };

    Events::drawingEvent.Add(RandomCheats::DrawBar);
#endif

    Events::processScriptsEvent += []
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* pPlayer = FindPlayerPed();
        int hplayer = CPools::GetPedRef(pPlayer);

        if (HardMode::m_bEnabled)
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
                Command<Commands::TAKE_PHOTO>();
                SetHelpMessage(TEXT("Game.ScreenshotTaken"));
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

        RandomCheats::Process();

        if (freeCam.Pressed())
        {
            if (Freecam::m_bEnabled)
            {
                Freecam::m_bEnabled = false;
                Freecam::Clear();
            }
            else
            {
                Freecam::m_bEnabled = true;
            }
        }

        if (Freecam::m_bEnabled)
        {
            Freecam::Process();
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
        SetHelpMessage(TEXT("Game.MissionStartFailed"));
    }
}

void Game::ShowPage()
{
    ImGui::Spacing();
    CPlayerPed* pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);

#ifdef GTASA
    if (ImGui::Button(TEXT("Game.SaveGame"), Widget::CalcSize()))
    {
        FrontEndMenuManager.m_bActivateMenuNextFrame = true;
        bSaveGameFlag = true;
    }
    ImGui::Spacing();
#endif

    if (ImGui::BeginTabBar("Game", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::BeginChild("##Checkbox");
            ImGui::Spacing();
            ImGui::Columns(2, nullptr, false);
            if (ImGui::Checkbox(TEXT("Game.DisableCheats"), &m_bDisableCheats))
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
            if (ImGui::Checkbox(TEXT("Game.DisableReplay"), &m_bDisableReplay))
            {
                if (m_bDisableReplay)
                {
                    patch::SetUChar(BY_GAME(0x460500, 0x624EC0, 0x593170), 0xC3);
                }
                else
                {
                    patch::SetUChar(BY_GAME(0x460500, 0x624EC0, 0x593170), 0x80);
                }
            }

            Widget::CheckboxAddr(TEXT("Game.FasterClock"), BY_GAME(0x96913B, 0xA10B87, 0x95CDBB));
#ifdef GTASA
            if (Widget::Checkbox(TEXT("Game.ForbiddenWantedLevel"), &m_bForbiddenArea, TEXT("Game.ForbiddenWantedLevelText")))
            {
                patch::Set<BYTE>(0x441770, m_bForbiddenArea ? 0x83 : 0xC3);
            }
            Widget::CheckboxAddr(TEXT("Game.FreePNS"), 0x96C009);
#endif

#ifdef GTAVC
            ImGui::NextColumn();
#endif
#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Game.FreezeGame"), 0xA10B48);
#endif
            if (ImGui::Checkbox(TEXT("Game.FreezeGameTime"), &m_bFreezeTime))
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
            if (ImGui::Checkbox("Freeze misson timer", &m_bMissionTimer))
            {
                Command<Commands::FREEZE_ONSCREEN_TIMER>(m_bMissionTimer);
            }
            if (Widget::Checkbox(TEXT("Game.HardMode"), &HardMode::m_bEnabled, TEXT("Game.HardModeText")))
            {
                CPlayerPed* player = FindPlayerPed();

                if (HardMode::m_bEnabled)
                {
                    HardMode::m_fBacArmour = player->m_fArmour;
                    HardMode::m_fBacHealth = player->m_fHealth;

#ifdef GTASA
                    HardMode::m_fBacMaxHealth = CStats::GetStatValue(STAT_MAX_HEALTH);
                    HardMode::m_fBacStamina = CStats::GetStatValue(STAT_STAMINA);
#else
                    HardMode::m_fBacMaxHealth = 100.0f;
#endif
                    player->m_fHealth = 50.0f;
                }
                else
                {
                    player->m_fArmour = HardMode::m_fBacArmour;

#ifdef GTASA
                    CStats::SetStatValue(STAT_STAMINA, HardMode::m_fBacStamina);
                    CStats::SetStatValue(STAT_MAX_HEALTH, HardMode::m_fBacMaxHealth);
#endif
                    player->m_fHealth = HardMode::m_fBacHealth;
                }
            }
#ifdef GTASA
            if (Widget::Checkbox(TEXT("Game.NoWaterPhysics"), &m_bNoWaterPhysics))
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
            if (Widget::Checkbox(TEXT("Game.KeepStuff"), &m_bKeepStuff, TEXT("Game.KeepStuffText")))
            {
                Command<Commands::SWITCH_ARREST_PENALTIES>(m_bKeepStuff);
                Command<Commands::SWITCH_DEATH_PENALTIES>(m_bKeepStuff);
            }
            Widget::Checkbox(TEXT("Game.Screenshot"), &m_bScreenShot, 
                                std::format("{} {}", TEXT("Game.ScreenshotTip"), 
                                quickSceenShot.GetNameString()).c_str());
            Widget::Checkbox(TEXT("Game.SolidWater"), &m_bSolidWater, TEXT("Game.SolidWaterText"));
#endif
            if (ImGui::Checkbox(TEXT("Game.SyncSystemTime"), &m_bSyncTime))
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
        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
        {
            ImGui::BeginChild("##Menus");
#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Game.CurrentDay")))
            {
                int day = CClock::CurrentDay - 1;
                if (ImGui::Combo(TEXT("Game.SelectDay"), &day, "Sunday\0Monday\0Tuesday\0Wednesday\0Thursday\0Friday\0Saturday\0"))
                {
                    CClock::CurrentDay = day + 1;
                }

                ImGui::Spacing();
                ImGui::Separator();
            }//0xC17040
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
        if (ImGui::BeginTabItem(TEXT("Game.Freecam")))
        {
            ImGui::Spacing();
            if (Widget::Checkbox(TEXT("Game.Enable"), &Freecam::m_bEnabled, TEXT("Game.EnableText")))
            {
                if (!Freecam::m_bEnabled)
                {
                    Freecam::Clear();
                }
            }
            ImGui::Spacing();

            ImGui::SliderFloat(TEXT("Game.FieldOfView"), &Freecam::m_fFOV, 5.0f, 120.0f);
            ImGui::SliderInt(TEXT("Game.MovementSpeed"), &Freecam::m_nMul, 1, 10);
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
        if (ImGui::BeginTabItem(TEXT("Game.Missions")))
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

                Widget::DataList(m_MissionData, SetPlayerMission, nullptr);
            }
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT("Game.Stats")))
        {
            // similar to Widget::DataList()
            ImGui::Spacing();

            if (ImGui::Button(TEXT("Game.MaxWepSkills"), Widget::CalcSize(2)))
            {
                for (size_t i = 69; i != 80; ++i)
                {
                    CStats::SetStatValue((unsigned short)i, 1000);
                }
                CHud::GetRidOfAllHudMessages(true);
                SetHelpMessage(TEXT("Game.MaxWepSkillsText"));
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Game.MaxVehSkills"), Widget::CalcSize(2)))
            {
                CStats::SetStatValue(160, 1000);
                CStats::SetStatValue(223, 1000);
                CStats::SetStatValue(229, 1000);
                CStats::SetStatValue(230, 1000);
                CHud::GetRidOfAllHudMessages(true);
                SetHelpMessage(TEXT("Game.MaxVehSkillsText"));
            }

            ImGui::Spacing();
            ImGui::PushItemWidth((ImGui::GetWindowContentRegionWidth() - ImGui::GetStyle().ItemSpacing.x)/2);
            Widget::ListBox("##Categories", m_StatData.m_Categories, m_StatData.m_Selected);
            ImGui::SameLine();
            Widget::Filter("##Filter", m_StatData.m_Filter, TEXT("Window.Search"));
            ImGui::PopItemWidth();

            ImGui::Spacing();
            ImGui::BeginChild("STATCHILD");
            for (auto [k, v] : m_StatData.m_pData->Items())
            {
                if (k.str() == m_StatData.m_Selected || m_StatData.m_Selected == "All")
                {
                    for (auto [k2, v2] : v.as_table()->ref<DataStore::Table>())
                    {
                        std::string name = v2.value_or<std::string>("Unknown");
                        if (m_StatData.m_Filter.PassFilter(name.c_str()))
                        {
                            Widget::EditStat(name.c_str(), std::stoi(std::string(k2.str())));
                        }
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Game.RandomCheats")))
        {
            ImGui::Spacing();
            ImGui::Columns(2, NULL, false);
            ImGui::Checkbox(TEXT("Game.Enable"), &RandomCheats::m_bEnabled);
            ImGui::NextColumn();
            ImGui::Checkbox(TEXT("Game.ProgressBar"), &RandomCheats::m_bProgressBar);
            ImGui::Columns(1);
            ImGui::Spacing();

            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);

            ImGui::SliderInt(TEXT("Game.ActivateTimer"), &RandomCheats::m_nInterval, 5, 60);
            Widget::Tooltip(TEXT("Game.ActivateTimerText"));

            ImGui::PopItemWidth();

            ImGui::TextWrapped(TEXT("Game.SelectCheats"));
            ImGui::Separator();
            if (ImGui::BeginChild("Cheats list"))
            {
                RandomCheats::DrawList();
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
