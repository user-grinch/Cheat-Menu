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

class CheatMenu : Hook, Animation, Game, Menu, Ped, Player, Teleport, Vehicle, Visual, Weapon
{
private:
	static unsortedMap header;
	static void DrawMenu();
	static void ProcessWindow();
	static void ApplyStyle();

public:
	CheatMenu();
};

