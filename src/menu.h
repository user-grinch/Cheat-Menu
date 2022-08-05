#pragma once
#include "pch.h"

/*
    Menu Class
    Handles code for the Menu page
*/
class Menu
{
public:
    static inline bool m_bAutoCheckUpdate;  // Should updates be checked automatically
    static inline bool m_bDiscordRPC;       // Is the discord rich presence enabled
    static inline bool m_bTextOnlyMode;     // Hide all menu images mode
    
    Menu() = delete;
    Menu(const Menu&) = delete;

    // Inits the class
    static void Init();

    // Displays the menu page
    static void ShowPage();
};
