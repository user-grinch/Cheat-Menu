#pragma once
#define _GTA_
#define INPUT_BUFFER_SIZE 64
#define SPAWN_PED_LIMIT 20
#define DISCORD_INVITE "https://discord.gg/ZzW7kmf"
#define GITHUB_LINK "https://github.com/user-grinch/Cheat-Menu"
#define PATREON_LINK "https://www.patreon.com/grinch_"
#define IMGUI_DEFINE_MATH_OPERATORS

#define MENU_NAME "Cheat Menu"
#define MENU_VERSION_NUMBER "3.6"
#define MENU_VERSION MENU_VERSION_NUMBER"-beta"
#define MENU_TITLE MENU_NAME " v" MENU_VERSION

#ifdef GTASA
#define BY_GAME(sa, vc, iii) sa
#elif GTAVC
#define BY_GAME(sa, vc, iii) vc
#elif GTA3
#define BY_GAME(sa, vc, iii) iii
#endif

#define FILE_NAME BY_GAME("CheatMenuSA" , "CheatMenuVC", "CheatMenuIII")
#define MENU_DATA_PATH(x) (PLUGIN_PATH((char*)FILE_NAME##"/"##x))
#define MENU_DATA_EXISTS(x) (std::filesystem::exists(MENU_DATA_PATH(x)))
#define OPEN_LINK(x) ShellExecute(nullptr, "open", x, nullptr, nullptr, SW_SHOWNORMAL)



