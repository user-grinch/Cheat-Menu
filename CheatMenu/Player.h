#pragma once
class Player
{
private:
	struct keep_position
	{
		inline static bool state = false;
		inline static CVector pos;
	};
	inline static bool god_mode = false;
	inline static int body = 0;
	inline static bool aim_skin_changer = false;
	inline static SearchData cloth_data;
	inline static bool images_loaded = false;

	struct custom_skins
	{
		inline static std::string dir = paths::GetGameDirPathA() + std::string("\\modloader\\Custom Skins\\");;
		inline static ImGuiTextFilter filter = "";
		inline static std::vector<std::string> store_vec;
	};
	inline static bool modloader_installed = false;

public:
	Player();
	~Player();
	static void ChangePlayerCloth(std::string& model);
	static void ChangePlayerModel(std::string& model);
	static void Draw();
};

