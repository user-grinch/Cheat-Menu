
<p align="center">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/logo.png">
  </p>
<p align="center">  
  Press <kbd>Left Ctrl</kbd> + <kbd>M</kbd> to open the menu. Doesn't work in multiplayer
</p>

## Introduction

A mod menu for Grand Theft Auto III, Vice City & San Andreas which allows a large set of modifications/cheats to be applied enabling a much more easier & amusing gameplay. This is **NOT** intended for use in multiplayer, I won't be responsible for any form of bans. Use it at your own risk!

Get stable binaries from [here](https://github.com/user-grinch/Cheat-Menu/releases). Beta binaries are available at the discord server.

[![Discord Banner 2](https://discordapp.com/api/guilds/689515979847237649/widget.png?style=banner2)](https://discord.com/invite/ZzW7kmf)

## Installation
Only these versions of the game is supported. I highly recommand installing the menu on a fresh, non-modded game first.
- GTA SA v1.0 US Hoodlum 
- GTA VC v1.0 EN
- GTA III v1.0 EN

If you have those mentioned versions installed follow the steps below,
1. Install [DirectX9](https://www.microsoft.com/en-us/download/details.aspx?id=35), [Visual C++ Redistributable 2022 x86](https://aka.ms/vs/17/release/vc_redist.x86.exe) & [Ultimate asi loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases) (Necessary).
2. Download [**CheatMenuSA/VC/III.7z**](https://github.com/user-grinch/Cheat-Menu/releases) and extract everything in game root directory. The **CheatMenu** folder & **.asi** file must be placed in the same place.
3. Install [SilentPatch](https://gtaforums.com/topic/669045-silentpatch/) (For GTA III don't install the ddraw.dll component)

### For III & VC
3. Install [D3D8to9 wrapper](https://github.com/crosire/d3d8to9/releases). Sometimes the asi loader blocks **d3d8.dll** file. Just delete the **Globals.ini** file and you should be good to go.

Be sure to check [here](https://github.com/user-grinch/Cheat-Menu/issues/48) to know about incompatibilities with other mods.

## Incompatible Mods
### GTA SA
1. **ENB/ SA_DirectX 2.0**  **FIX:** Disable Effect/ Enable water
2. **MSI Afterburner**  **FIX:**  Enable Stealth mode in rivatuner
3. **GTA Underground**

### GTA III
1. SilentPatch **ddraw.dll** component.  **FIX:**  Delete ddraw.dll file for now

Let me know if you mind more.

## Things to do on crash
1. Delete the `CheatMenu/json/config.json` file and try again.
2. If it's happening after installing a recent mod try removing it. A list of incompatibile mods are [here](https://github.com/user-grinch/Cheat-Menu/issues/48).
3. Try on a fresh installation of the game.
4. If still doesn't fix your issue, go ahead and report it.

### Crash reports/ Bug reports
Install [Crash info](https://www.mixmods.com.br/2021/06/crashinfo.html) & reproduce your crash. When reporting the crash upload the crashinfo logfile with it. Explain how to reproduce the crash in details. Also include,
- Game version
- Menu version
- List of mods installed

You can either open a issue here or directly contact me on discord (Recommended).


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

## Images
<details>
  <summary>Show Images</summary>
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/1.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/2.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/3.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/4.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/5.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/6.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/7.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/8.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/9.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/10.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/11.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/12.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/13.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/14.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/15.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/16.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/17.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/18.png">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/19.png">
</details>

## How to add ___ to CheatMenu (SA Only)

*Be careful about editing 'json' files, especially commas! Check [here](https://en.wikipedia.org/wiki/JSON)*

### Player Skins (Recommended)
*If you just want more player skins without much hassel use this*

1. Install [Modloader](https://github.com/thelink2012/modloader/releases/tag/v0.3.7).
2. Get your skin **.dff** & **.txd** files and put them inside *modloader\Custom Skins\* directory.
3. Rename your **dff** & **txd** filenames to less than **8** characters. Having more than **8** characters will **crash** your game.
**Note:** After installing, don't rename or delete files while your game is running. It'll crash!

### Peds + Player Skins
*If you want to add both peds & skin then follow this*

1. Add peds to your game following a tutorial.
2. Then create a image of your ped and put it in *CheatMenu\peds.txd*.
3. The image name should be the model name of the ped.
4. Open this file *CheatMenu\json\ped.json* and add an entry for your ped there. <br/>
   **Format:  "PedModel" : "PedName"**
   
### Vehicles
1. Add vehicles to your game following [this](https://www.mixmods.com.br/2015/12/tutorial-adicionar-carros-sem-substituir.html) tutorial first. (Use translator)
2. Then create an image of your vehicle in and put it in *CheatMenu\vehicles.txd*.
3. The image name should be the model id of the vehicle.

### Vehicle Tunes
1. Add tunes to your game following a [tutorial](https://gtaforums.com/topic/481926-tutorial-how-to-add-tuning-parts/).
2. Then create a image of your tune part and put it in *CheatMenu\components.txd*.

### Vehicle Paintjobs
1. Put your paintjob images *CheatMenu\textures.txd.
2. You can use any name for these images.

### Weapons
1. Add weapons to your game following [this](https://www.mixmods.com.br/2016/05/tutorial-adicionar-armas-sem-substituir.html) tutorial first. (Use translator)
2. Then create an image of your vehicle and put it *CheatMenu\weapons.txd*.
3. The image name should be the weapon id (not model!) of the weapon.
4. Open this file *CheatMenu\json\weapon.json* and add an entry for your weapon there.<br/>
   **Format:  "WeaponID" : "WeaponName"**
