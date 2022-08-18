#pragma once
#include "interface/ipage.h"

class MenuPage : public IPage<MenuPage>
{
public:
    bool m_bAutoCheckUpdate;  // Should updates be checked automatically
    bool m_bDiscordRPC;       // Is the discord rich presence enabled
    bool m_bTextOnlyMode;     // Hide all menu images mode

    friend IPage;
    MenuPage();
    MenuPage(const MenuPage&);

    void Draw();
};

extern MenuPage& menuPage;
