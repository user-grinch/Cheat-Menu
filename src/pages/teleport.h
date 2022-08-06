#pragma once
#include "pch.h"

enum class eTeleportType
{
    Marker,
    MapPosition,
    Coordinate,
};

/*
    Teleport Class
    Contains code for the Teleport menu page
*/
class Teleport
{
private:
    static inline bool m_bInsertCoord;
    static inline bool m_bTeleportMarker;
    static inline bool m_bQuickTeleport;
    static inline ImVec2 m_fMapSize;
#ifdef GTASA
    static inline DataStore m_SpriteData {"sprites"};

    /*
    	Generates radar sprite coordinates on the fly.
    	Shouldn't get saved in 'teleport.json', needs to be cleared at game shutdown.
    */
    static void FetchRadarSpriteData();
#endif

public:
    static inline ResourceStore m_locData{"locations", eResourceType::TYPE_TEXT};

    Teleport() = delete;
    Teleport(const Teleport&) = delete;

    // Initialized the class, hooks etc.
    static void Init();

    // Warp player to position, marker, map etc
    template<eTeleportType Type = eTeleportType::Coordinate>
    static void WarpPlayer(CVector pos = CVector(0, 0, 0), int interiorID = 0);

    // Returns true if quick teleport feature is active
    static bool IsQuickTeleportActive();

    // Draws the teleport menu page
    static void ShowPage();
};
