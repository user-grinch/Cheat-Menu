#pragma once
#include "pch.h"
#include "interface/ipage.h"

enum class eTeleportType
{
    Marker,
    MapPosition,
    Coordinate,
};

class TeleportPage : public IPage<TeleportPage>
{
private:
    char m_LocBuf[INPUT_BUFFER_SIZE], m_InBuf[INPUT_BUFFER_SIZE];
    bool m_bInsertCoord;
    bool m_bTeleportMarker;
    bool m_bQuickTeleport;
    bool m_bSpawnUnderwater;
    ImVec2 m_fMapSize;
#ifdef GTASA

    DataStore m_SpriteData {"sprites"};

    /*
    	Generates radar sprite coordinates on the fly.
    	Shouldn't get saved in 'teleport.json', needs to be cleared at game shutdown.
    */
    void FetchRadarSpriteData();
#endif

public:
    ResourceStore m_locData{"locations", eResourceType::TYPE_TEXT};

    friend class IFeature;
    TeleportPage();
    TeleportPage(const TeleportPage&);

    void Draw();

    // Returns true if quick teleport feature is active
    bool IsQuickTeleportActive();

    // Callbacks
    void LocationAddNew();
    void LocationClick(str&, str&, str&);

    // Warp player to position, marker, map etc
    template<eTeleportType Type = eTeleportType::Coordinate>
    void WarpPlayer(CVector pos = CVector(0, 0, 0), int interiorID = 0);
};

extern TeleportPage& teleportPage;
