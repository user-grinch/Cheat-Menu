
<p align="center">
  <img src="https://raw.githubusercontent.com/user-grinch/Cheat-Menu/rewrite/images/logo.png">
  </p>
<p align="center">  
  Press <kbd>Left Ctrl</kbd> + <kbd>M</kbd> to open the menu. Doesn't work in multiplayer
</p>

## Introduction

Cheat menu for Grand Theft Auto San Andreas & Vice City which allows a large set of modifications/cheats to be applied enabling a much more easier & amusing gameplay.

Get stable binaries from [here](https://github.com/user-grinch/Cheat-Menu/releases). Beta binaries are available at the discord server.

[![Discord Banner 2](https://discordapp.com/api/guilds/689515979847237649/widget.png?style=banner2)](https://discord.com/invite/ZzW7kmf)

## Installation

1. Install [DirectX9](https://www.microsoft.com/en-us/download/details.aspx?id=35) &  [Visual C++ Redistributable 2019 x86](hhttps://aka.ms/vs/16/release/vc_redist.x86.exe) if not already installed.
2. If your game version isn't v1.0 then you'll need to [downgrade](https://gtaforums.com/topic/927016-san-andreas-downgrader/).
3. Install [asi loader](https://www.gtagarage.com/mods/show.php?id=21709)
4. Download **CheatMenu.7z** and extract everything in game directory (replace if necessary).
5. (Only VC) Download and install [d3d8to9 wrapper](https://github.com/crosire/d3d8to9/releases). Sometimes the asi loader may come with Globals.ini. Remove the d3d8.dll from there.

(Vice City version is available at the discord server)

## Required to build
1. [Build Tools 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)
2. [DirectX9 SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
3. [Plugin SDK](https://github.com/DK22Pac/plugin-sdk)

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

## How to add ___ to CheatMenu

*Be careful about editing 'json' files, especially commas! Check [here](https://en.wikipedia.org/wiki/JSON)*

### Clothes
Haven't found any tutorials for this yet. But CheatMenu loads cloths from *CheatMenu\clothes.txd*<br/>
**Name format: body_part$model_name$texture_name**.<br/>

### Player Skins (Recommended)
*If you just want more player skins without much hassel use this*

1. For this you need [modloader](https://github.com/thelink2012/modloader/releases/tag/v0.3.7) installed.
2. Get your skin **dff** & **txd** file and put them inside *modloader\Custom Skins\* directory.
3. Rename your **dff** & **txd** filenames into 8 characters. Having more than 8 characters will crash your game.
4. Important thing to remember is that you can't rename these files while the game is running! If you do, your game will crash.

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
