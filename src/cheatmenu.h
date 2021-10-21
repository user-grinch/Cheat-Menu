/*
	Author: Grinch_
	Copyright Grinch_ 2019-2022. All rights reserved.
	Required:
		DirectX 9 SDK
		Plugin SDK
		Build Tools 2019 (v142)
		Windows SDK
*/

#pragma once
#ifndef GTA3
#include "animation.h"
#include "player.h"
#include "ped.h"
#include "vehicle.h"
#include "weapon.h"
#include "game.h"
#include "visual.h"
#endif
#include "teleport.h"
#include "menu.h"
#include "hook.h"

#ifndef GTA3
class CheatMenu : Hook, Animation, Game, Menu, Ped, Player, Teleport, Vehicle, Visual, Weapon
#else
class CheatMenu : Hook, Menu, Teleport
#endif
{
private:
	static inline bool m_bShowMenu = false;
	static inline ImVec2 m_fMenuSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);

#ifdef GTA3
	static inline CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Menu", &Menu::Draw}
	};
#else
	static inline CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Player", &Player::Draw}, {"Ped", &Ped::Draw},
		{"Animation", &Animation::Draw},
		{"Dummy", nullptr},
		{"Vehicle", &Vehicle::Draw}, {"Weapon", &Weapon::Draw},
		{"Game", &Game::Draw}, {"Visual", &Visual::Draw}, {"Menu", &Menu::Draw}
	};
#endif

	static void ApplyStyle();
	static void DrawWindow();

public:
	CheatMenu();
};
