#pragma once
class Player
{
private:
	struct keep_position
	{
		static bool state;
		static CVector pos;
	};
	static bool god_mode;
	static int body;
	static bool aim_skin_changer;
	static ImGuiTextFilter filter;
	static std::string selected_item;
	static std::vector<std::string> search_categories;
	static std::vector<std::unique_ptr<TextureStructure>> clothes_vec;

	struct custom_skins
	{
		static std::string dir;
		static ImGuiTextFilter filter;
		static std::vector<std::string> store_vec;
	};
	static bool modloader_installed;

public:
	static void ChangePlayerCloth(std::string& model);
	static void ChangePlayerModel(std::string& model);
	Player();
	~Player();
	static void Main();
};

