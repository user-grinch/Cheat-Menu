#include "pch.h"
#include "autodrive.h"
#include "utils/widget.h"
#include "pages/teleport.h"

AutoDriveMgr& AutoDrive = AutoDriveMgr::Get();

void AutoDriveMgr::StartDrive(CVehicle *pVeh, const char *buf)
{
    int hVeh = CPools::GetVehicleRef(pVeh);
    CVector pos;

    if (buf ==  nullptr)
    {
#ifdef GTASA
        tRadarTrace targetBlip = CRadar::ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];
        pos = targetBlip.m_vecPos;
        if (targetBlip.m_nRadarSprite != RADAR_SPRITE_WAYPOINT)
        {
            Util::SetMessage(TEXT("Teleport.TargetBlipText"));
            return;
        }
#else
        return;
#endif
    }
    else
    {
        if (sscanf(buf, "%f,%f,%f", &pos.x, &pos.y, &pos.z) != 3)
        {
            int dim;
            sscanf(buf, "%d,%f,%f,%f", &dim, &pos.x, &pos.y, &pos.z);
        }
    }

    int model = pVeh->m_nModelIndex;
    if (CModelInfo::IsBoatModel(model))
    {
        Command<Commands::BOAT_GOTO_COORDS>(hVeh, pos.x, pos.y, pos.z);
    }
    else if (CModelInfo::IsPlaneModel(model))
    {
        CVector p = pVeh->GetPosition();
        p.z = 300.0f;
#ifdef GTASA
        pVeh->SetPosn(p);
#elif GTAVC
        pVeh->SetPosition(p);
#else
        pVeh->SetPos(p);
#endif
        Command<Commands::PLANE_GOTO_COORDS>(hVeh, pos.x, pos.y, 300.0f, 30, 200);
    }
    else if (CModelInfo::IsHeliModel(model))
    {
        CVector p = pVeh->GetPosition();
        p.z = 300.0f;
#ifdef GTASA
        pVeh->SetPosn(p);
#elif GTAVC
        pVeh->SetPosition(p);
#else
        pVeh->SetPos(p);
#endif
        Command<Commands::HELI_GOTO_COORDS>(hVeh, pos.x, pos.y, 300.0f, 30, 200);
    }
#ifdef GTASA
    else if (CModelInfo::IsTrainModel(model))
    {
        return;
    }
#endif
    else
    {
        Command<Commands::CAR_GOTO_COORDINATES>(hVeh, pos.x, pos.y, pos.z);
    }
}

void AutoDriveMgr::Draw()
{
    CPlayerPed* pPlay = FindPlayerPed();
    CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());;
    int hPlayer = CPools::GetPedRef(pPlay);
    int hVeh = CPools::GetVehicleRef(pVeh);

    if (ImGui::Button(TEXT("Vehicle.AutoDriveStop"), Widget::CalcSize(1)))
    {
        Command<Commands::WARP_CHAR_INTO_CAR>(hPlayer, hVeh);
    }
    ImGui::Spacing();
    if (ImGui::BeginTabBar("PassTabaBar"))
    {
        if (ImGui::BeginTabItem(TEXT( "Teleport.Coordinates")))
        {
            static char buf[INPUT_BUFFER_SIZE];
            ImGui::Spacing();
            ImGui::TextWrapped(TEXT("Vehicle.AutoDriveInfo"));
            ImGui::Spacing();
            ImGui::InputTextWithHint(TEXT("Teleport.Coordinates"), "x, y, z", buf, IM_ARRAYSIZE(buf));
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Vehicle.AutoDriveCoord"), Widget::CalcSize(BY_GAME(2,1,1))))
            {
                StartDrive(pVeh, buf);
            }
#ifdef GTASA
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Vehicle.AutoDriveMarker"), Widget::CalcSize(2)))
            {
                StartDrive(pVeh);
            }
#endif
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Teleport.Location")))
        {
            ImGui::Spacing();
            Widget::DataList(teleportPage.m_locData,
                             [this, pVeh](std::string& rootkey, std::string& bLocName, std::string& loc)
            {
                StartDrive(pVeh, loc.c_str());
            });
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}