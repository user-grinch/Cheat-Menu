#include "pch.h"
#include "topdowncam_sa.h"

TopDownCamera& TopDownCam = TopDownCamera::Get();

TopDownCamera::TopDownCamera()
{
    Events::processScriptsEvent += [this]()
    {
        if (m_bEnabled)
        {
            DrawPages();
        }
    };
}

/*
	Taken from gta chaos mod by Lordmau5
	https://github.com/gta-chaos-mod/Trilogy-ASI-Script
*/
void TopDownCamera::DrawPages()
{
    CPlayerPed *player = FindPlayerPed();
    CVector     pos    = player->GetPosition ();
    float       curOffset = m_nZoom;

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
    CVector cameraPos = CVector (playerOffset.x, playerOffset.y, playerOffset.z + curOffset);

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
