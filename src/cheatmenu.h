/*
    Author: Grinch_
    Copyright Grinch_ 2019-2022. All rights reserved.
    Required:
        DirectX 9 SDK
        Plugin SDK
        Build Tools 2022 (v143)
        Windows SDK
*/

#pragma once
#include "animation.h"
#include "game.h"
#include "menu.h"
#include "ped.h"
#include "player.h"
#include "teleport.h"
#include "vehicle.h"
#include "visual.h"
#include "weapon.h"

class CheatMenu : Animation, Game, Menu, Ped, Player, Teleport, Vehicle, Visual, Weapon
{
private:
    enum class eMenuPages
    {
        ANIMATION, ANNIVERSARY, GAME, MENU, NONE, PED, PLAYER, TELEPORT, UPDATE, VEHICLE, VISUAL, WEAPON, WELCOME
    };
    struct HeaderData
    {
        std::string name;
        void *pFunc;
        eMenuPages page;
        bool skipHeader = false;
    };
    
    static inline eMenuPages m_nMenuPage = eMenuPages::WELCOME;
    static inline bool m_bShowMenu = false;
    static inline ImVec2 m_fMenuSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);
    static inline bool m_bSizeChangedExternal = false;

    static void ApplyStyle();
    static void DrawWindow();
    static void ShowAnniversaryPage();
    static void ShowUpdatePage();
    static void ShowWelcomePage();
    static void ProcessMenuPages();

    static inline std::vector<HeaderData> m_headerList
    {
        {"Teleport", &Teleport::ShowPage, eMenuPages::TELEPORT}, 
        {"Player", &Player::ShowPage, eMenuPages::PLAYER}, 
        {"Ped", &Ped::ShowPage, eMenuPages::PED}, 
        {"Animation", &Animation::ShowPage, eMenuPages::ANIMATION}, 
        {"Vehicle", &Vehicle::ShowPage, eMenuPages::VEHICLE},
        {"Weapon", &Weapon::ShowPage, eMenuPages::WEAPON},
        {"Game", &Game::ShowPage, eMenuPages::GAME}, 
        {"Visual", &Visual::ShowPage, eMenuPages::VISUAL}, 
        {"Menu", &Menu::ShowPage, eMenuPages::MENU}, 
        {"Welcome", &ShowWelcomePage, eMenuPages::WELCOME, true},
        {"Update", &ShowUpdatePage, eMenuPages::UPDATE, true},
        {"Anniversary", &ShowAnniversaryPage, eMenuPages::ANNIVERSARY, true}
    };

public:
    CheatMenu();
    ~CheatMenu();

    static void ResetMenuSize();
};
