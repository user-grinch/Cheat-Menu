#include "pch.h"
#include "freecam_sa.h"

static CVector gTotalMouse;
FreecamMgr& Freecam = FreecamMgr::Get();

void FreecamMgr::Enable()
{
    CPlayerPed* player = FindPlayerPed();
    Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, true);

    // set hud & radar states
    m_bHudState = patch::Get<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL)); // hud
    m_bRadarState = patch::Get<BYTE>(BY_GAME(0xBA676C, 0xA10AB6, NULL)); // radar
    patch::Set<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL), 0); // disable hud
    patch::Set<BYTE>(0xBA676C, 2); // disable radar

    // create our dummy character
    CVector pos = player->GetPosition();
    Command<Commands::CREATE_RANDOM_CHAR>(pos.x, pos.y, pos.z, &m_nPed);
    m_pPed = CPools::GetPed(m_nPed);
    m_pPed->m_bIsVisible = false;

    gTotalMouse.x = player->GetHeading() + 89.6f;
    gTotalMouse.y = 0;
    pos.z -= 20;
    m_pPed->SetPosn(pos);

    Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(m_nPed, true);
    Command<Commands::SET_LOAD_COLLISION_FOR_CHAR_FLAG>(m_nPed, false);
    Command<Commands::SET_CHAR_COLLISION>(m_nPed, false);

    // set camera fov
    TheCamera.LerpFOV(TheCamera.FindCamFOV(), m_fFOV, 1000, true);
    Command<Commands::CAMERA_PERSIST_FOV>(true);
}

void FreecamMgr::DrawPages()
{
    int delta = (CTimer::m_snTimeInMilliseconds - CTimer::m_snPreviousTimeInMilliseconds);
    int ratio = 1 / (1 + (delta * m_nMul));
    int speed = m_nMul + m_nMul * ratio * delta;
    CVector pos = m_pPed->GetPosition();

    gTotalMouse.x -= CPad::NewMouseControllerState.x / 250;
    gTotalMouse.y += CPad::NewMouseControllerState.y / 3;

    gTotalMouse.x = gTotalMouse.x > 150 ? 150 : gTotalMouse.x;
    gTotalMouse.x = gTotalMouse.x < -150 ? -150 : gTotalMouse.x;

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
        Util::SetMessage(TEXT("Game.PlayerTeleported"));
    }

    if (KeyPressed(VK_MENU) && m_nMul > 1)
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

        if (KeyPressed(VK_CONTROL))
        {
            pos.z += speed * sin(90.0f / 3 * 3.14159f / 180.0f);
        }
        else
        {
            pos.x += speed * cos(angle * 3.14159f / 180.0f);
            pos.y += speed * sin(angle * 3.14159f / 180.0f);

            if (!KeyPressed(VK_SPACE))
            {
                pos.z += speed * 2 * sin(gTotalMouse.y / 3 * 3.14159f / 180.0f);
            }
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
        if (KeyPressed(VK_CONTROL))
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
                Util::SetMessage(std::format("Speed: {}", m_nMul).c_str());
            }
        }
    }

    if (CPad::NewMouseControllerState.wheelDown)
    {
        if (KeyPressed(VK_CONTROL))
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
                Util::SetMessage(std::to_string(m_nMul).c_str());
                Util::SetMessage(std::format("Speed: {}", m_nMul).c_str());
            }
        }
    }

    m_pPed->SetHeading(gTotalMouse.x);
    Command<Commands::ATTACH_CAMERA_TO_CHAR>(m_nPed, 0.0, 0.0, 20.0, 90.0, 180, gTotalMouse.y, 0.0, 2);
    m_pPed->SetPosn(pos);
    plugin::Call<0x4045B0>(&pos); // CIPLStore::AddIplsNeededAtPosn(CVector)
}

void FreecamMgr::Disable()
{
    Command<Commands::SET_EVERYONE_IGNORE_PLAYER>(0, false);
    patch::Set<BYTE>(BY_GAME(0xBA6769, 0x86963A, NULL), m_bHudState); // hud
    patch::Set<BYTE>(BY_GAME(0xBA676C, 0xA10AB6, NULL), m_bRadarState); // radar

    Command<Commands::DELETE_CHAR>(m_nPed);
    m_pPed = nullptr;

    Command<Commands::RESTORE_CAMERA_JUMPCUT>();
}

FreecamMgr::FreecamMgr()
{
    Events::processScriptsEvent += [this]()
    {
        if (m_bEnabled)
        {
            DrawPages();
        }
    };
}