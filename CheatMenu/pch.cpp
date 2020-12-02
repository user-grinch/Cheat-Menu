#include "pch.h"

std::string Globals::header_id = "";
int Globals::last_key_timer = 0;
ImVec2 Globals::menu_size = ImVec2(screen::GetScreenWidth()/4, screen::GetScreenHeight()/1.2);
std::string Globals::menu_title = std::string("Cheat Menu v") + std::string(MENU_VERSION);
ImVec2 Globals::font_screen_size = ImVec2(-1, -1);
bool Globals::show_menu = false;
bool Globals::init_done = false;
Renderer Globals::renderer = Render_Unknown;
ID3D11Device *Globals::device11 = nullptr;

CJson config = CJson("config", true);
std::ofstream flog = std::ofstream("CheatMenu.log");

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> cheat_id(0, 91);
