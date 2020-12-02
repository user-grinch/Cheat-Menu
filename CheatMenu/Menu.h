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
	struct hotkey
	{
		static int command_window[2];
		static int menu_open[2];
		static int aim_skin_changer[2];
		static int quick_ss[2];
		static int quick_tp[2];
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

