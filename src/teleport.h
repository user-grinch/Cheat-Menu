#pragma once
#include "pch.h"

enum class eTeleportType
{
    Marker,
    MapPosition,
    Coordinate,
};

class Teleport
{
private:
    static inline bool m_bInsertCoord;
    static inline bool m_bTeleportMarker;
    static inline char m_nInputBuffer[INPUT_BUFFER_SIZE];
    static inline char m_nLocationBuffer[INPUT_BUFFER_SIZE];
    static inline ImVec2 m_fMapSize;

    struct TPMarker
    {
        static inline bool m_bEnabled;
        static inline CVector m_fPos = { -1, -1, -1 };
        static inline uint m_nTimer;
        static inline bool m_bJetpack;
    };

#ifdef GTASA
    static inline DataStore m_SpriteData {"sprites"};

    /*
    	Generates radar sprite coordinates on the fly.
    	Shouldn't get saved in 'teleport.json', needs to be cleared at game shutdown.
    */
    static void FetchRadarSpriteData();
#endif

    static void TeleportToLocation(std::string& rootkey, std::string& bLocName, std::string& loc);
    static void RemoveTeleportEntry(std::string& rootkey, std::string& key, std::string& val);

public:
    Teleport() = delete;
    Teleport(const Teleport&) = delete;
    static inline ResourceStore m_locData{ "locations", eResourceType::TYPE_TEXT };
    static inline bool m_bQuickTeleport;

    static void Init();
    template<eTeleportType Type = eTeleportType::Coordinate>
    static void WarpPlayer(CVector pos = CVector(0, 0, 0), int interiorID = 0);
    static void ShowPage();
};
