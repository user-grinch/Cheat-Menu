#pragma once
#include <vector>
#include "pch.h"

class Teleport
{
private:
    static inline bool m_bInsertCoord;
    static inline bool m_bQuickTeleport;
    static inline char m_nInputBuffer[INPUT_BUFFER_SIZE];
    static inline ResourceStore m_tpData{ "teleport", eResourceType::TYPE_TEXT };
    static inline char m_nLocationBuffer[INPUT_BUFFER_SIZE];
    static inline uint m_nQuickTeleportTimer;

#ifdef GTASA
    static inline CJson m_SpriteJson = CJson("radar sprite");
#endif

    struct m_Teleport
    {
        static inline bool m_bEnabled;
        static inline CVector m_fPos = { -1, -1, -1 };
        static inline uint m_nTimer;
    };


#ifdef GTASA
    /*
    	Generates radar sprite coordinates on the fly.
    	Shouldn't get saved in 'teleport.json', needs to be cleared at game shutdown.
    */
    static void FetchRadarSpriteData();
#endif

    static void TeleportToLocation(std::string& rootkey, std::string& bLocName, std::string& loc);
    static void RemoveTeleportEntry(std::string& rootkey, std::string& key, std::string& val);

public:
    Teleport();

    static void TeleportPlayer(bool get_marker = false, CVector pos = CVector(0, 0, 0), int interior_id = 0);
    static void ShowPage();
};
