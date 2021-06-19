#pragma once
#include <vector>
#include "pch.h"

class Teleport
{
private:
	inline static bool m_bInsertCoord;
	inline static bool m_bQuickTeleport;
	inline static char m_nInputBuffer[INPUT_BUFFER_SIZE];
	inline static SSearchData tp_data{"teleport"};
	inline static char m_nLocationBuffer[INPUT_BUFFER_SIZE];
	inline static uint m_nQuickTeleportTimer;
	inline static CJson m_SpriteJson = CJson("radar sprite");
	struct m_Teleport
	{
		inline static bool m_bEnabled;
		inline static CVector m_fPos = {-1, -1, -1};
		inline static uint m_nTimer;
	};


	/*
		Generates radar sprite coordinates on the fly.
		Shouldn't get saved in 'teleport.json', needs to be cleared at game shutdown.
	*/
	static void FetchRadarSpriteData();

protected:
	Teleport();
public:
	static void Draw();
	static void TeleportPlayer(bool get_marker = false, CVector pos = CVector(0, 0, 0), short interior_id = 0);
	static void TeleportToLocation(std::string& rootkey, std::string& bLocName, std::string& loc);
	static void RemoveTeleportEntry(std::string& rootkey, std::string& key, std::string& val);
};
