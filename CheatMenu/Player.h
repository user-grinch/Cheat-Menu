#pragma once
class Player
{
private:
	inline static bool m_bGodMode;
	inline static int m_nUiBodyState;
	inline static bool m_bAimSkinChanger;
	inline static SSearchData m_ClothData;
	inline static bool m_bImagesLoaded;
	inline static bool m_bModloaderInstalled;
	static struct
	{
		inline static bool m_bEnabled = false;
		inline static CVector m_fPos;
	} m_KeepPosition;
	static struct
	{
		inline static std::string m_Path = paths::GetGameDirPathA() + std::string("\\modloader\\Custom Skins\\");;
		inline static ImGuiTextFilter m_Filter;
		inline static std::vector<std::string> m_List;
	} m_CustomSkins;

public:
	Player();
	~Player();
	static void ChangePlayerCloth(std::string& model);
	static void ChangePlayerModel(std::string& model);
	static void Draw();
};
