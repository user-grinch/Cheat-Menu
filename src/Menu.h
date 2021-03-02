#pragma once

class Menu
{
private:
	struct overlay
	{
		inline static bool coord = false;
		inline static bool fps = false;
		inline static bool loc_name = false;
		inline static bool transparent = false;
		inline static bool veh_health = false;
		inline static bool veh_speed = false;
		inline static std::vector<std::string> pos_names = { "Custom", "Top left", "Top right", "Bottom left", "Bottom right" };
		inline static int selected_pos = 4; // TODO: Create Enum
		inline static float posX = 0.0f;
		inline static float posY = 0.0f;
	};
public:
	struct hotkeys
	{
		inline static HotKeyData aim_skin_changer;
		inline static HotKeyData freecam;
		inline static HotKeyData command_window;
		inline static HotKeyData fix_veh;
		inline static HotKeyData flip_veh;
		inline static HotKeyData free_cam_tp_player{VK_RETURN,VK_RETURN};
		inline static HotKeyData god_mode;
		inline static HotKeyData menu_open;
		inline static HotKeyData quick_ss;
		inline static HotKeyData quick_tp;
		inline static HotKeyData veh_engine;
		inline static HotKeyData veh_instant_start;
		inline static HotKeyData veh_instant_stop;
	};
	struct commands
	{
		inline static bool show_menu = false;
		inline static char input_buffer[INPUT_BUFFER_SIZE] = "";
	};

	Menu();
	static void Draw();
	static void DrawOverlay();
	static void DrawShortcutsWindow();
	static void ProcessCommands();
};

