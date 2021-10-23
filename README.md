
<p align="center">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/logo.png">
  </p>
<p align="center">  
  Press <kbd>Left Ctrl</kbd> + <kbd>M</kbd> to open the menu. Doesn't work in multiplayer
</p>

## Introduction

A mod menu for Grand Theft Auto III, Vice City & San Andreas which allows a large set of modifications/cheats to be applied enabling a much more easier & amusing gameplay. This is **NOT** intended for use in multiplayer, I won't be responsible for any form of bans. Use it at your own risk!

Get stable binaries from [here](https://github.com/user-grinch/Cheat-Menu/releases). Beta binaries are available at the discord server. Binaries for GTA III are available at the discord server too.

[![Discord Banner 2](https://discordapp.com/api/guilds/689515979847237649/widget.png?style=banner2)](https://discord.com/invite/ZzW7kmf)

## Installation
Only these versions of the game is supported. I highly recommand installing the menu on a fresh, non-modded game first.
- GTA SA v1.0 US Hoodlum 
- GTA VC v1.0 EN
- GTA III v1.0 EN

If you have those mentioned versions installed follow the steps below,
1. Install [DirectX9](https://www.microsoft.com/en-us/download/details.aspx?id=35), [Visual C++ Redistributable 2019 x86](https://aka.ms/vs/16/release/vc_redist.x86.exe) & [Asi loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases).
2. Download [**CheatMenuSA/VC/III.7z**](https://github.com/user-grinch/Cheat-Menu/releases) and extract everything in game root directory. The **CheatMenu** folder & **.asi** file must be placed in the same place.

### For III & VC
3. Install [D3D8to9 wrapper](https://github.com/crosire/d3d8to9/releases). Sometimes the asi loader blocks **d3d8.dll** file. Just delete the **Globals.ini** file and you should be good to go.

Be sure to check [here](https://github.com/user-grinch/Cheat-Menu/issues/48) to know about incompatibilities with other mods.

## Required to build
1. [Build Tools 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)
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
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/teleport.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/player.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/ped.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/animation.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/vehicle.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/weapon.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/game.gif">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/visual.gif">
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
