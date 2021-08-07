#pragma once
#include "pch.h"

class Player
{
private:
	inline static bool m_bGodMode;
	inline static bool m_bImagesLoaded;
	inline static bool m_bModloaderInstalled;
	struct m_KeepPosition
	{
		inline static bool m_bEnabled = false;
		inline static CVector m_fPos;
	};

#ifdef GTASA
	inline static bool m_bAimSkinChanger;
	inline static int m_nUiBodyState;
	inline static SSearchData m_ClothData;
	struct m_CustomSkins
	{
		inline static std::string m_Path = paths::GetGameDirPathA() + std::string("\\modloader\\Custom Skins\\");;
		inline static ImGuiTextFilter m_Filter;
		inline static std::vector<std::string> m_List;
	};
#elif GTAVC
	inline static SSearchData skinData{ "skin" };
#endif

public:
	Player();
	static void Draw();

#ifdef GTASA
	static void ChangePlayerModel(std::string& model);
	static void ChangePlayerCloth(std::string& model);
#elif GTAVC
	static void ChangePlayerModel(std::string& cat, std::string& name, std::string& id);
#endif
};
