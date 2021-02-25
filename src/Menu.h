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
		static HotKeyData aim_skin_changer;
		static HotKeyData freecam;
		static HotKeyData command_window;
		static HotKeyData fix_veh;
		static HotKeyData flip_veh;
		static HotKeyData free_cam_tp_player;
		static HotKeyData god_mode;
		static HotKeyData menu_open;
		static HotKeyData quick_ss;
		static HotKeyData quick_tp;
		static HotKeyData veh_engine;
		static HotKeyData veh_instant_start;
		static HotKeyData veh_instant_stop;
	};
	struct commands
	{
		static bool show_menu;
		static char input_buffer[INPUT_BUFFER_SIZE];
	};

	Menu();
	static void Draw();
	static void DrawOverlay();
	static void DrawShortcutsWindow();
	static void ProcessCommands();
};

