#pragma once
#include "pch.h"

class Menu
{
public:
    static inline bool m_bAutoCheckUpdate;
    static inline bool m_bDiscordRPC;
    static inline bool m_bTextOnlyMode; 
    
    Menu() = delete;
    Menu(const Menu&) = delete;

    static void Init();
    static void ShowPage();
};
