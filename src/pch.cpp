#include "pch.h"

std::string Globals::header_id = "";
ImVec2 Globals::menu_size = ImVec2(screen::GetScreenWidth()/4, screen::GetScreenHeight()/1.2);
ImVec2 Globals::screen_size = ImVec2(-1, -1);
bool Globals::show_menu = false;
bool Globals::init_done = false;
Renderer Globals::renderer = Render_Unknown;
void *Globals::device = nullptr;

std::ofstream flog = std::ofstream("CheatMenu.log");
CJson config = CJson("config");
