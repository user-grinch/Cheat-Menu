/*
	Author: Grinch_
	Copyright GPLv3 2019-2021
	Required:
		DirectX 9 SDK
		Plugin SDK
		Build Tools 2019 (v142)
		Windows SDK
*/

#pragma once
#include "Hook.h"
#include "Menu.h"
#include "Teleport.h"
#include "Player.h"

#ifdef GTASA
#include "Animation.h"
#include "Game.h"
#include "Ped.h"
#include "Vehicle.h"
#include "Visual.h"
#include "Weapon.h"
class CheatMenu : Hook, Animation, Game, Menu, Ped, Player, Teleport, Vehicle, Visual, Weapon
#elif defined(GTAVC)
class CheatMenu : Hook, Menu, Player, Teleport
#endif

{
private:
#ifdef GTASA
	inline static CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Player", &Player::Draw}, {"Ped", &Ped::Draw},
		{"Animation", &Animation::Draw}, {"Vehicle", &Vehicle::Draw}, {"Weapon", &Weapon::Draw},
		{"Game", &Game::Draw}, {"Visual", &Visual::Draw}, {"Menu", &Menu::Draw}
	};
#elif GTAVC
	inline static CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Player", &Player::Draw}, {"Menu", &Menu::Draw}
	};
#endif

	static void ApplyStyle();
	static void DrawWindow();

public:
	CheatMenu();
};
