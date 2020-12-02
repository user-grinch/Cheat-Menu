/*
	Required:
		Visual Studio 2015.3 (v140)
		Plugin SDK
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
	static void ProcessMenu();
	static void ProcessWindow();
	static void ApplyImGuiStyle();

public:
	CheatMenu();
	~CheatMenu();
};

class Launcher
{
public:
	Launcher()
	{
		bool launch = true;

		uint gameVersion = GetGameVersion();
		if (gameVersion != GAME_10US_HOODLUM && gameVersion != GAME_10US_COMPACT) {
			MessageBox(HWND_DESKTOP, "CheatMenu requires v1.0 US of the game.", "CheatMenu", MB_ICONERROR);
			flog << "Game version isn't 1.0" << std::endl;
			launch = false;
		}
		else
		{
			Events::initRwEvent += [&launch]()
			{
				if (GetModuleHandleA("SAMP.dll")) {
					MessageBox(HWND_DESKTOP, "SAMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
					flog << "SAMP detected. Exiting..." << std::endl;
					launch = false;
				}

				if (launch)
					static CheatMenu cheatmenu;
			};
		}
	}
} launcher;

