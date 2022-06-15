#include "pch.h"
#include "menu.h"
#include "game.h"
#include "ui.h"
#include "util.h"
#ifdef GTASA
#include <CIplStore.h>
#include <CMessages.h>
#include <CSprite2d.h>
#endif

static bool bSaveGameFlag = false;

void Game::RealTimeClock()
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
    // Generate enabled cheats vector
    for (auto [k, v] : m_RandomCheats::m_pData.Items())
    {
        /*
        [
        	cheat_id = [ cheat_name, state (true/false) ]
        ]
        */
        std::string key { k.str() };
        m_RandomCheats::m_EnabledCheats[std::stoi(key)][0] = v.value_or<std::string>("Unknown");
        m_RandomCheats::m_EnabledCheats[std::stoi(key)][1] = "true";
    }

    Events::drawMenuBackgroundEvent += []()
    {
        if (bSaveGameFlag)
        {
            FrontEndMenuManager.m_nCurrentMenuPage = MENUPAGE_GAME_SAVE;
            bSaveGameFlag = false;
        }
    };

    Events::drawingEvent += []()
    {
        if (m_RandomCheats::m_bEnabled && m_RandomCheats::m_bProgressBar)
        {
            // Next cheat timer bar
            uint screenWidth = screen::GetScreenWidth();
            uint screenHeight = screen::GetScreenHeight();
            uint timer = CTimer::m_snTimeInMilliseconds;
            uint totalTime = m_RandomCheats::m_nInterval;
            float progress = (totalTime - (timer - m_RandomCheats::m_nTimer) / 1000.0f) / totalTime;

            CRect sizeBox = CRect(0,0, screenWidth, screenHeight/50);
            CRect sizeProgress = CRect(0,0, screenWidth*progress, screenHeight/50);
            CRGBA colorBG = CRGBA(24, 99, 44, 255);
            CRGBA colorProgress = CRGBA(33, 145, 63, 255);

            CSprite2d::DrawRect(sizeBox, colorBG);
            CSprite2d::DrawRect(sizeProgress, colorProgress);
        }
    };
#endif

    Events::processScriptsEvent += []
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* pPlayer = FindPlayerPed();
        int hplayer = CPools::GetPedRef(pPlayer);

        if (m_HardMode::m_bEnabled)
        {
            if (pPlayer->m_fHealth > 50.0f)
                pPlayer->m_fHealth = 50.0f;

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

#endif

        if (freeCam.Pressed())
        {
            if (m_Freecam::m_bEnabled)
            {
                m_Freecam::m_bEnabled = false;
                ClearFreecamStuff();
            }
            else
            {
                m_Freecam::m_bEnabled = true;
            }
        }

        if (m_Freecam::m_bEnabled)
        {
            FreeCam();
        }

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

#ifdef GTASA
        if (m_RandomCheats::m_bEnabled)
        {
            if ((timer - m_RandomCheats::m_nTimer) > (static_cast<uint>(m_RandomCheats::m_nInterval) * 1000))
            {
                int id = Random(0, 91);

                for (int i = 0; i < 92; i++)
                {
                    if (i == id)
                    {
                        if (m_RandomCheats::m_EnabledCheats[i][1] == "true")
                        {
                            Call<0x00438370>(id); // cheatEnableLegimate(int CheatID)
                            CMessages::AddMessage((char*)m_RandomCheats::m_EnabledCheats[i][0].c_str(), 2000, 0, false);
                            m_RandomCheats::m_nTimer = timer;
                        }
                        break;
                    }
                }
            }
        }
#endif
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

void Game::FreeCam()
{
    int delta = (CTimer::m_snTimeInMilliseconds - CTimer::m_snPreviousTimeInMilliseconds);

    int ratio = 1 / (1 + (delta * m_Freecam::m_nMul));
    int speed = m_Freecam::m_nMul + m_Freecam::m_nMul * ratio * delta;

    if (!m_Freecam::m_bInitDone)
    {
        CPlayerPed* player = FindPlayerPed();
        Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, true);

        m_Freecam::m_bHudState = patch::Get<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL)); // hud
        patch::Set<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL), 0); // hud
        m_Freecam::m_bRadarState = patch::Get<BYTE>(BY_GAME(0xBA676C, 0xA10AB6, NULL)); // radar

        CVector playerPos = player->GetPosition();
        CPad::GetPad(0)->DisablePlayerControls = true;

        Command<Commands::CREATE_RANDOM_CHAR>(playerPos.x, playerPos.y, playerPos.z, &m_Freecam::m_nPed);
        m_Freecam::m_pPed = CPools::GetPed(m_Freecam::m_nPed);

        m_Freecam::m_fTotalMouse.x = player->GetHeading() + 89.6f;
        m_Freecam::m_fTotalMouse.y = 0;
        playerPos.z -= 20;

        Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(m_Freecam::m_nPed, true);
        Command<Commands::SET_LOAD_COLLISION_FOR_CHAR_FLAG>(m_Freecam::m_nPed, false);

#ifdef GTASA
        m_Freecam::m_pPed->m_bIsVisible = false;
        Command<Commands::SET_CHAR_COLLISION>(m_Freecam::m_nPed, false);
        m_Freecam::m_pPed->SetPosn(playerPos);
        TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_Freecam::m_fFOV, 1000, true);
        Command<Commands::CAMERA_PERSIST_FOV>(true);
        patch::Set<BYTE>(0xBA676C, 2); // disable radar
#elif GTAVC
        m_Freecam::m_pPed->m_nFlags.bIsVisible = false;
        m_Freecam::m_pPed->m_nFlags.bUseCollision = false;
        m_Freecam::m_pPed->SetPosition(playerPos);
        patch::Set<BYTE>(0xA10AB6, 1); // disable radar
#else
        m_Freecam::m_pPed->m_nFlags.bIsVisible = false;
        m_Freecam::m_pPed->m_nFlags.bUsesCollision = false;
        m_Freecam::m_pPed->SetPosition(playerPos.x, playerPos.y, playerPos.z);
#endif

        m_Freecam::m_bInitDone = true;
    }

    CVector pos = m_Freecam::m_pPed->GetPosition();

    m_Freecam::m_fMouse.x = CPad::NewMouseControllerState.x;
    m_Freecam::m_fMouse.y = CPad::NewMouseControllerState.y;
    m_Freecam::m_fTotalMouse.x = m_Freecam::m_fTotalMouse.x - m_Freecam::m_fMouse.x / 250;
    m_Freecam::m_fTotalMouse.y = m_Freecam::m_fTotalMouse.y + m_Freecam::m_fMouse.y / 3;

    if (m_Freecam::m_fTotalMouse.x > 150)
    {
        m_Freecam::m_fTotalMouse.y = 150;
    }

    if (m_Freecam::m_fTotalMouse.y < -150)
    {
        m_Freecam::m_fTotalMouse.y = -150;
    }

    if (freeCamTeleport.Pressed())
    {
        CPlayerPed* player = FindPlayerPed();
        CVector pos = m_Freecam::m_pPed->GetPosition();

#ifdef GTASA
        CEntity* playerEntity = FindPlayerEntity(-1);
        pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &playerEntity) + 0.5f;
#else
        pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr) + 0.5f;
#endif
        Command<Commands::SET_CHAR_COORDINATES>(CPools::GetPedRef(player), pos.x, pos.y, pos.z);

        // disble them again cause they get enabled
#ifndef GTA3
        CHud::bScriptDontDisplayRadar = true;
#endif
        CHud::m_Wants_To_Draw_Hud = false;
        SetHelpMessage(TEXT("Game.PlayerTeleported"));
    }

    if (KeyPressed(VK_RCONTROL))
    {
        speed /= 2;
    }

    if (KeyPressed(VK_RSHIFT))
    {
        speed *= 2;
    }

    if (freeCamForward.PressedBasic() || freeCamBackward.PressedBasic())
    {
        if (freeCamBackward.PressedBasic())
        {
            speed *= -1;
        }

        float angle;
        Command<Commands::GET_CHAR_HEADING>(m_Freecam::m_nPed, &angle);
        pos.x += speed * cos(angle * 3.14159f / 180.0f);
        pos.y += speed * sin(angle * 3.14159f / 180.0f);
        pos.z += speed * 2 * sin(m_Freecam::m_fTotalMouse.y / 3 * 3.14159f / 180.0f);
    }

    if (freeCamLeft.PressedBasic() || freeCamRight.PressedBasic())
    {
        if (freeCamLeft.PressedBasic())
        {
            speed *= -1;
        }

        float angle;
        Command<Commands::GET_CHAR_HEADING>(m_Freecam::m_nPed, &angle);
        angle -= 90;

        pos.x += speed * cos(angle * 3.14159f / 180.0f);
        pos.y += speed * sin(angle * 3.14159f / 180.0f);
    }

#ifdef GTASA
    if (CPad::NewMouseControllerState.wheelUp)
    {
        if (m_Freecam::m_fFOV > 10.0f)
        {
            m_Freecam::m_fFOV -= 2.0f * speed;
        }

        TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_Freecam::m_fFOV, 250, true);
        Command<Commands::CAMERA_PERSIST_FOV>(true);
    }

    if (CPad::NewMouseControllerState.wheelDown)
    {
        if (m_Freecam::m_fFOV < 115.0f)
        {
            m_Freecam::m_fFOV += 2.0f * speed;
        }

        TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_Freecam::m_fFOV, 250, true);
        Command<Commands::CAMERA_PERSIST_FOV>(true);
    }

    m_Freecam::m_pPed->SetHeading(m_Freecam::m_fTotalMouse.x);
    Command<Commands::ATTACH_CAMERA_TO_CHAR>(m_Freecam::m_nPed, 0.0, 0.0, 20.0, 90.0, 180, m_Freecam::m_fTotalMouse.y, 0.0, 2);
    m_Freecam::m_pPed->SetPosn(pos);
    CIplStore::AddIplsNeededAtPosn(pos);
#elif GTAVC
    m_Freecam::m_pPed->m_placement.SetHeading(m_Freecam::m_fTotalMouse.x);
    m_Freecam::m_pPed->SetPosition(pos);
#else
    m_Freecam::m_pPed->SetHeading(m_Freecam::m_fTotalMouse.x);
    m_Freecam::m_pPed->SetPosition(pos.x, pos.y, pos.z);
#endif
}

void Game::ClearFreecamStuff()
{
    m_Freecam::m_bInitDone = false;
    Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, false);
    patch::Set<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL), m_Freecam::m_bHudState); // hud
    patch::Set<BYTE>(BY_GAME(0xBA676C, 0xA10AB6, NULL), m_Freecam::m_bRadarState); // radar

    CPad::GetPad(0)->DisablePlayerControls = false;

    Command<Commands::DELETE_CHAR>(m_Freecam::m_nPed);
    m_Freecam::m_pPed = nullptr;

#ifdef GTASA
    Command<Commands::CAMERA_PERSIST_FOV>(false);
#endif
    Command<Commands::RESTORE_CAMERA_JUMPCUT>();
}

void Game::ShowPage()
{
    ImGui::Spacing();
    CPlayerPed* pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);

#ifdef GTASA
    if (ImGui::Button(TEXT("Game.SaveGame"), Ui::GetSize()))
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

            Ui::CheckboxAddress(TEXT("Game.FasterClock"), BY_GAME(0x96913B, 0xA10B87, 0x95CDBB));
#ifdef GTASA
            if (Ui::CheckboxWithHint(TEXT("Game.ForbiddenWantedLevel"), &m_bForbiddenArea, TEXT("Game.ForbiddenWantedLevelText")))
            {
                patch::Set<BYTE>(0x441770, m_bForbiddenArea ? 0x83 : 0xC3, false);
            }
            Ui::CheckboxAddress(TEXT("Game.FreePNS"), 0x96C009);
#endif

#ifdef GTAVC
            ImGui::NextColumn();
#endif
#ifdef GTASA
            Ui::CheckboxAddress(TEXT("Game.FreezeGame"), 0xA10B48);
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
            if (Ui::CheckboxWithHint(TEXT("Game.HardMode"), &m_HardMode::m_bEnabled, TEXT("Game.HardModeText")))
            {
                CPlayerPed* player = FindPlayerPed();

                if (m_HardMode::m_bEnabled)
                {
                    m_HardMode::m_fBacArmour = player->m_fArmour;
                    m_HardMode::m_fBacHealth = player->m_fHealth;

#ifdef GTASA
                    m_HardMode::m_fBacMaxHealth = CStats::GetStatValue(STAT_MAX_HEALTH);
                    m_HardMode::m_fBacStamina = CStats::GetStatValue(STAT_STAMINA);
#else
                    m_HardMode::m_fBacMaxHealth = 100.0f;
#endif
                    player->m_fHealth = 50.0f;
                }
                else
                {
                    player->m_fArmour = m_HardMode::m_fBacArmour;

#ifdef GTASA
                    CStats::SetStatValue(STAT_STAMINA, m_HardMode::m_fBacStamina);
                    CStats::SetStatValue(STAT_MAX_HEALTH, m_HardMode::m_fBacMaxHealth);
#endif
                    player->m_fHealth = m_HardMode::m_fBacHealth;
                }
            }
#ifdef GTASA

            if (Ui::CheckboxWithHint(TEXT("Game.KeepStuff"), &m_bKeepStuff, TEXT("Game.KeepStuffText")))
            {
                Command<Commands::SWITCH_ARREST_PENALTIES>(m_bKeepStuff);
                Command<Commands::SWITCH_DEATH_PENALTIES>(m_bKeepStuff);
            }
            Ui::CheckboxWithHint(TEXT("Game.Screenshot"), &m_bScreenShot,
                                 (("Take screenshot using ") + quickSceenShot.GetNameString()
                                  + "\nSaved inside 'GTA San Andreas User Files\\Gallery'").c_str());
            Ui::CheckboxWithHint(TEXT("Game.SolidWater"), &m_bSolidWater, TEXT("Game.SolidWaterText"));
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
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
        {
#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Game.CurrentDay")))
            {
                int day = CClock::CurrentDay - 1;
                if (Ui::ListBox(TEXT("Game.SelectDay"), m_DayNames, day))
                {
                    CClock::CurrentDay = day + 1;
                }

                ImGui::Spacing();
                ImGui::Separator();
            }
#endif
            Ui::EditAddress<int>(TEXT("Game.DaysPassed"), BY_GAME(0xB79038, 0x97F1F4, 0x8F2BB8), 0, 9999);
            Ui::EditReference(TEXT("Game.FPSLimit"), RsGlobal.maxFPS, 1, 30, 60);
#ifdef GTASA
            if (ImGui::CollapsingHeader(TEXT("Game.Freecam")))
            {
                if (Ui::CheckboxWithHint(TEXT("Game.Enable"), &m_Freecam::m_bEnabled, TEXT("Game.EnableText")))
                {
                    if (!m_Freecam::m_bEnabled)
                    {
                        ClearFreecamStuff();
                    }
                }
                ImGui::Spacing();

                ImGui::SliderFloat(TEXT("Game.FieldOfView"), &m_Freecam::m_fFOV, 5.0f, 120.0f);
                ImGui::SliderInt(TEXT("Game.Movement Speed"), &m_Freecam::m_nMul, 1, 10);
                ImGui::Spacing();
                ImGui::TextWrapped(TEXT("Game.FreecamTip"));
                ImGui::Spacing();
                ImGui::Separator();
            }
#endif
            Ui::EditReference(TEXT("Game.GameSpeed"), CTimer::ms_fTimeScale, 1, 1, 10);
            Ui::EditFloat(TEXT("Game.Gravity"), BY_GAME(0x863984, 0x68F5F0, 0x5F68D4), -1.0f, 0.008f, 1.0f, 1.0f, 0.01f);

            if (ImGui::CollapsingHeader(TEXT("Game.SetTime")))
            {
                int hour = CClock::ms_nGameClockHours;
                int minute = CClock::ms_nGameClockMinutes;

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

                ImGui::Spacing();
                ImGui::Separator();
            }
#ifdef GTASA
            static std::vector<Ui::NamedMemory> themes
            {
                {TEXT("Game.Beach"), 0x969159}, {TEXT("Game.Country"), 0x96917D}, {TEXT("Game.FunHouse"), 0x969176}, {TEXT("Game.Ninja"), 0x96915C}
            };
            Ui::EditRadioButtonAddress(TEXT("Game.Themes"), themes);
#endif
            if (ImGui::CollapsingHeader(TEXT("Game.Weather")))
            {
#ifdef GTASA
                if (ImGui::Button(TEXT("Game.Foggy"), Ui::GetSize(3)))
                {
                    Call<0x438F80>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Overcast"), Ui::GetSize(3)))
                {
                    Call<0x438F60>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Rainy"), Ui::GetSize(3)))
                {
                    Call<0x438F70>();
                }

                if (ImGui::Button(TEXT("Game.Sandstorm"), Ui::GetSize(3)))
                {
                    Call<0x439590>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Thunderstorm"), Ui::GetSize(3)))
                {
                    Call<0x439570>();
                }

                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.VerySunny"), Ui::GetSize(3)))
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
                Ui::ShowTooltip(TEXT("Game.WeatherIDText"));
#else
                if (ImGui::Button(TEXT("Game.Sunny"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(0);
                }
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Cloudy"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(1);
                }
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Rainy"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(2);
                }

                if (ImGui::Button(TEXT("Game.Foggy"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(3);
                }
#ifdef GTAVC
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.ExtraSunny"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(4);
                }
                ImGui::SameLine();
                if (ImGui::Button(TEXT("Game.Hurricane"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(5);
                }

                if (ImGui::Button(TEXT("Game.ExtraColors"), Ui::GetSize(3)))
                {
                    CWeather::ForceWeatherNow(6);
                }
#endif
#endif
                ImGui::Spacing();
                ImGui::Separator();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Game.Missions")))
        {
            ImGui::Spacing();

            static bool bMissionLoaderWarningShown;
            if (!bMissionLoaderWarningShown)
            {
                ImGui::TextWrapped(TEXT("Game.MissionLoaderTip"));
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Game.ShowLoader"), ImVec2(Ui::GetSize())))
                {
                    bMissionLoaderWarningShown = true;
                }
            }
            else
            {
                if (ImGui::Button(TEXT("Game.FailMission"), ImVec2(Ui::GetSize())))
                {
                    if (!Util::IsOnCutscene())
                    {
                        Command<Commands::FAIL_CURRENT_MISSION>();
                    }
                }

                ImGui::Spacing();

                Ui::DrawJSON(m_MissionData, SetPlayerMission, nullptr);
            }
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT("Game.Stats")))
        {
            // similar to Ui::DrawJSON()
            ImGui::Spacing();

            if (ImGui::Button(TEXT("Game.MaxWepSkills"), Ui::GetSize(2)))
            {
                for (size_t i = 69; i != 80; ++i)
                {
                    CStats::SetStatValue((unsigned short)i, 1000);
                }
                CHud::GetRidOfAllHudMessages(true);
                SetHelpMessage(TEXT("Game.MaxWepSkillsText"));
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Game.MaxVehSkills"), Ui::GetSize(2)))
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
            Ui::ListBoxStr("##Categories", m_StatData.m_Categories, m_StatData.m_Selected);
            ImGui::SameLine();
            Ui::FilterWithHint("##Filter", m_StatData.m_Filter, TEXT("Window.Search"));
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
                            Ui::EditStat(name.c_str(), std::stoi(std::string(k2.str())));
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
            ImGui::Checkbox(TEXT("Game.Enable"), &m_RandomCheats::m_bEnabled);
            ImGui::NextColumn();
            ImGui::Checkbox(TEXT("Game.ProgressBar"), &m_RandomCheats::m_bProgressBar);
            ImGui::Columns(1);
            ImGui::Spacing();

            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);

            ImGui::SliderInt(TEXT("Game.ActivateTimer"), &m_RandomCheats::m_nInterval, 5, 60);
            Ui::ShowTooltip(TEXT("Game.ActivateTimerText"));

            ImGui::PopItemWidth();

            ImGui::TextWrapped(TEXT("Game.SelectCheats"));
            ImGui::Separator();
            if (ImGui::BeginChild("Cheats list"))
            {
                for (std::string* element : m_RandomCheats::m_EnabledCheats)
                {
                    bool selected = (element[1] == "true") ? true : false;

                    if (ImGui::MenuItem(element[0].c_str(), nullptr, selected))
                    {
                        element[1] = selected ? "false" : "true";
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}
