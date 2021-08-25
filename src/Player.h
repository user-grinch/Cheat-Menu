#pragma once
#include "pch.h"

class Player
{
private:
	inline static bool m_bGodMode;
	inline static bool m_bModloaderInstalled;
	struct m_KeepPosition
	{
		inline static bool m_bEnabled = false;
		inline static CVector m_fPos;
	};

#ifdef GTASA
	inline static bool m_bAimSkinChanger;
	inline static bool m_bDrunkEffect;
	inline static bool m_bFastSprint;
	inline static int m_nUiBodyState;
	inline static ResourceStore m_ClothData { "clothes" , eResourceType::TYPE_IMAGE, ImVec2(70, 100)};
	struct m_CustomSkins
	{
		inline static std::string m_Path = paths::GetGameDirPathA() + std::string("\\modloader\\Custom Skins\\");;
		inline static ImGuiTextFilter m_Filter;
		inline static std::vector<std::string> m_List;
	};

	struct m_TopDownCamera 
	{
		inline static bool m_bEnabled = false;
		inline static float m_fOffset = 40.0f;
	};
#elif GTAVC
	inline static ResourceStore skinData{ "skin", eResourceType::TYPE_TEXT };
#endif

public:
	Player();
	static void Draw();

#ifdef GTASA
	static void ChangePlayerModel(std::string& model);
	static void ChangePlayerCloth(std::string& model);
	static void TopDownCameraView();
#elif GTAVC
	static void ChangePlayerModel(std::string& cat, std::string& name, std::string& id);
#endif
};
