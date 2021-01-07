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
		Events::initRwEvent += []()
		{
			bool launch = true;

			uint gameVersion = GetGameVersion();
			if (gameVersion != GAME_10US_HOODLUM && gameVersion != GAME_10US_COMPACT) {
				MessageBox(HWND_DESKTOP, "CheatMenu requires v1.0 US of the game.", "CheatMenu", MB_ICONERROR);
				launch = false;
			}

			if (GetModuleHandleA("SAMP.dll")) {
				MessageBox(HWND_DESKTOP, "SAMP detected. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
				launch = false;
			}

			/*
				Mouse is extremely buggy without SilentPatch
				Should have a better fix for this but everyone should have 
				SilentPatch installed so mehh...
			*/
			if (!GetModuleHandleA("SilentPatchSA.asi")) {
				MessageBox(HWND_DESKTOP, "SilentPatch isn't installed. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
				launch = false;
			}

			if (launch)
			{
				flog << "Loading CheatMenu" << std::endl;
				static CheatMenu cheatmenu;
			}
		};
	}
} launcher;

