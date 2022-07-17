#pragma once
#include "pch.h"

class Teleport
{
private:
    static inline bool m_bInsertCoord;
    static inline bool m_bTeleportMarker;
    static inline char m_nInputBuffer[INPUT_BUFFER_SIZE];
    static inline char m_nLocationBuffer[INPUT_BUFFER_SIZE];

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
    static void TeleportPlayer(bool get_marker = false, CVector pos = CVector(0, 0, 0), int interior_id = 0);
    static void ShowPage();
};
