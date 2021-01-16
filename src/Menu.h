#pragma once

class Menu
{
private:
	struct overlay
	{
		static bool coord;
		static bool fps;
		static bool loc_name;
		static bool transparent;
		static bool veh_health;
		static bool veh_speed;
		static std::vector<std::string> pos_names;
		static int selected_pos;
		static float posX;
		static float posY;
	};
public:
	struct hotkeys
	{
		static HotKeyData command_window;
		static HotKeyData menu_open;
		static HotKeyData aim_skin_changer;
		static HotKeyData quick_ss;
		static HotKeyData quick_tp;
		static HotKeyData airbreak;
	};
	struct commands
	{
		static bool show_menu;
		static char input_buffer[INPUT_BUFFER_SIZE];
	};

	Menu();
	~Menu();
	static void Main();
	static void ProcessOverlay();
	static void ProcessShortcutsWindow();
	static void ProcessCommands();

};

