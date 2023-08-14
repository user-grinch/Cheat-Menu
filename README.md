
<p align="center">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/logo.png" width="700" height="180">
  </p>
<p align="center">  
  Press <kbd>Left Ctrl</kbd> + <kbd>M</kbd> to open the menu, Doesn't work in multiplayer
</p>
<p align="center">  
  READ THE INSTRUCTIONS. EVERYTHING YOU NEED TO KNOW IS ALREADY HERE.
</p>

## Introduction

A mod menu for Grand Theft Auto III, Vice City & San Andreas allows a wide range of modifications/cheats to be applied, enabling much easier and more amusing gameplay. Please note that this mod is **NOT** intended for use in multiplayer, and I will not be held responsible for any bans that may occur. Use it at your own risk!

Get stable binaries from [here](https://github.com/user-grinch/Cheat-Menu/releases). Beta binaries are available on the Discord server.

[![Discord Banner 2](https://discordapp.com/api/guilds/689515979847237649/widget.png?style=banner2)](https://discord.com/invite/ZzW7kmf)

## Installation
Only these versions of the game are supported. I highly recommend installing the menu on a fresh, non-modded game first.
- GTA SA v1.0 US Hoodlum 
- GTA VC v1.0 EN
- GTA III v1.0 EN

If you have those mentioned versions installed, use the installer [here](https://github.com/user-grinch/Cheat-Menu/raw/master/installer/Installer.exe).

<details>
  <summary>Manual ( Not recommended, use the installer instead! )</summary>

1. Install [DirectX9](https://www.microsoft.com/en-us/download/details.aspx?id=35), [Visual C++ Redistributable 2022 x86](https://aka.ms/vs/17/release/vc_redist.x86.exe) & [Ultimate asi loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases).
2. Download [**CheatMenuSA/VC/III.7z**](https://github.com/user-grinch/Cheat-Menu/releases) and extract everything in the game's root directory (NOT MODLOADER). The **CheatMenu** folder & **.asi** file must be placed in the same place.
3. Install [SilentPatch](https://gtaforums.com/topic/669045-silentpatch/) (For GTA III don't install the ddraw.dll component)
4. Install [D3D8to9 wrapper](https://github.com/crosire/d3d8to9/releases) (For III & VC) 
</details>

## Incompatible Mods
### GTA SA
1. **ENB/ SA_DirectX 2.0** 
2. **MSI Afterburner**  **FIX:**  Enable Stealth mode in rivatuner
3. **GTA Underground**

### GTA III
1. SilentPatch **ddraw.dll** component.  **FIX:**  Delete the ddraw.dll file for now

Let me know if you mind more.

## Things to do on crash
1. Delete the `CheatMenuXX.toml` config file and try again.
2. If it's happening after installing a recent mod try removing it.
3. Try on a fresh installation of the game.
4. If still doesn't fix your issue, please report it.

### Crash reports/ Bug reports
Install [Crash info](https://www.mixmods.com.br/2021/06/crashinfo.html) & reproduce your crash. When reporting the crash upload the crashinfo.log logfile with it. Explain how to reproduce the crash in detail. Also, include:
- Game version
- Menu version
- List of mods installed

You can either open an issue here or directly contact me on Discord (Recommended).

## Images
<details>
  <summary>Show Images</summary>
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/1.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/2.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/3.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/4.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/5.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/6.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/7.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/8.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/9.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/10.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/11.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/12.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/13.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/14.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/15.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/16.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/17.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/18.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/images/19.png">
</details>

## Submit Translations
*How to translate Cheat Menu to another language*

The translations are loaded from `CheatMenuXX/locale/*.toml` files. Make a copy of the `English.toml` file and translate the texts there to your language. Rename the file to `your_language.toml` and make a pull request. 

If your language isn't supported ( Appears `???` in the menu. Check [here](https://github.com/user-grinch/Cheat-Menu/tree/master/tools/subset/in.txt) for supported languages), open a [issue](https://github.com/user-grinch/Cheat-Menu/issues) asking for support.


## Required to build
1. [Build Tools 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
2. [DirectX9 SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
4. [Plugin SDK](https://github.com/DK22Pac/plugin-sdk)
5. [Premake5](https://premake.github.io/) - Comes with already.

## Projects used
1. [ImGui](https://github.com/ocornut/imgui)
2. [Kiero](https://github.com/Rebzzel/kiero)
3. [MinHook](https://github.com/TsudaKageyu/minhook)
4. [Plugin SDK](https://github.com/DK22Pac/plugin-sdk)

All credits are given in Menu/About tab.
