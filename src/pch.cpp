#include "pch.h"

std::string Globals::header_id = "";
int Globals::last_key_timer = 0;
ImVec2 Globals::menu_size = ImVec2(screen::GetScreenWidth()/4, screen::GetScreenHeight()/1.2);
std::string Globals::menu_title = std::string("Cheat Menu v") + std::string(MENU_VERSION) + " (" BUILD_NUMBER + ")";
ImVec2 Globals::font_screen_size = ImVec2(-1, -1);
bool Globals::show_menu = false;
bool Globals::init_done = false;
Renderer Globals::renderer = Render_Unknown;
ID3D11Device *Globals::device11 = nullptr;
std::string Globals::menu_path = paths::GetPluginDirPathA();

std::ofstream flog = std::ofstream("CheatMenu.log");
CJson config = CJson("config");
