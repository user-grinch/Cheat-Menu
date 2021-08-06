#pragma once
#include "pch.h"

class Player
{
private:
	inline static bool m_bGodMode;
	inline static bool m_bAimSkinChanger;
	inline static bool m_bImagesLoaded;
	inline static bool m_bModloaderInstalled;
	struct m_KeepPosition
	{
		inline static bool m_bEnabled = false;
		inline static CVector m_fPos;
	};

	#ifdef GTASA
	inline static int m_nUiBodyState;
	inline static SSearchData m_ClothData;
	struct m_CustomSkins
	{
		inline static std::string m_Path = paths::GetGameDirPathA() + std::string("\\modloader\\Custom Skins\\");;
		inline static ImGuiTextFilter m_Filter;
		inline static std::vector<std::string> m_List;
	};
	#endif

public:
	Player();

#ifdef GTASA
	static void ChangePlayerModel(std::string& model);
#elif GTAVC
	static void ChangePlayerModel(std::string& cat, std::string& name, std::string& id);
#endif
	static void Draw();

	#ifdef GTASA
	static void ChangePlayerCloth(std::string& model);
	#endif
};
