/*
	Required:
		DirectX 9 SDK
		Plugin SDK
		Visual Studio 2019 (v142)
		Windows SDK
*/

#pragma once
#include "Animation.h"
#include "Game.h"
#include "Hook.h"
#include "Menu.h"
#include "Ped.h"
#include "Player.h"
#include "Teleport.h"
#include "Vehicle.h"
#include "Visual.h"
#include "Weapon.h"

class CheatMenu : Hook, CAnimation, CGame, Menu, Ped, Player, Teleport, Vehicle, Visual, Weapon
{
private:
	inline static CallbackTable header
	{
		{"Teleport", &Teleport::Draw}, {"Player", &Player::Draw}, {"Ped", &Ped::Draw},
		{"Animation", &CAnimation::Draw}, {"Vehicle", &Vehicle::Draw}, {"Weapon", &Weapon::Draw},
		{"Game", &CGame::Draw}, {"Visual", &Visual::Draw}, {"Menu", &Menu::Draw}
	};

	static void ApplyStyle();
	static void DrawWindow();

public:
	CheatMenu();
};
