
<p align="center">
  <img width="400" height="250" src="https://i.imgur.com/fZ71SbF.png">
  </p>
<p align="center">  
  Press <kbd>Left Ctrl</kbd> + <kbd>M</kbd> to open the menu. Doesn't work in multiplayer
</p>

#
[![License](https://img.shields.io/github/license/user-grinch/Cheat-Menu?style=for-the-badge)](https://github.com/user-grinch/Cheat-Menu/blob/master/LICENSE)
[![Discord](https://img.shields.io/discord/689515979847237649?label=Discord&style=for-the-badge)](https://discord.gg/ZzW7kmf)
[![MixMods](https://img.shields.io/badge/Topic-Mixmods-%234e4784?style=for-the-badge)](https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu)
[![GTAF](https://img.shields.io/badge/Topic-GTA%20Forums-%23244052?style=for-the-badge)](https://gtaforums.com/topic/961636-moon-cheat-menu/)
## Introduction

Cheat menu for Grand Theft Auto SanAndreas which allows a large set of modifications/cheats to be applied enabling a much more easier & amusing gameplay.

Get stable binaries from [here](https://github.com/user-grinch/Cheat-Menu/releases). Beta binaries are available at the discord server.

A portuguese translation of the menu is [here](https://github.com/Dowglass/Cheat-Menu)

## Installation

1. Install [DirectX9](https://www.microsoft.com/en-us/download/details.aspx?id=35) &  [Visual C++ Redistributable 2015 x86](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe) if not already installed.
2. If your game version isn't v1.0 then you'll need to [downgrade](https://gtaforums.com/topic/927016-san-andreas-downgrader/).
3. Install asi loader from [here](https://www.gtagarage.com/mods/show.php?id=21709)
4. Download CheatMenu and extract everything in game directory (replace if necessary).

## Building
You'll need to install Visual Studio 2015 (v140), [DirectX9 SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812) & [Plugin SDK](https://github.com/DK22Pac/plugin-sdk).

## Projects used
1. [ImGui](https://github.com/ocornut/imgui)
2. [Kiero](https://github.com/Rebzzel/kiero)
3. [MinHook](https://github.com/TsudaKageyu/minhook)
4. [Plugin SDK](https://github.com/DK22Pac/plugin-sdk)

## Images
![1](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/teleport.gif)
![2](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/player.gif)
![3](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/ped.gif)
![4](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/animation.gif)
![5](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/vehicle.gif)
![6](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/weapon.gif)
![7](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/game.gif)
![8](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/visual.gif)
![9](https://raw.githubusercontent.com/user-grinch/user-grinch.github.io/master/assets/img/mods/cheat-menu/menu.gif)

## Docs 

### Adding custom peds/skin images - jpg

Ped/skin images are loaded from 'CheatMenu\peds\' directory. The image name should contain the ped model id. Names for custom peds have to be added inside 'CheatMenu\json\ped.json' file.

### Add custom player skins #2

You can also add player skins by using a different method. Place your skin txd & dff files inside 'modloader\Custom Skins\' folder. Note, [Modloader](https://gtaforums.com/topic/669520-mod-loader/) is required & names can't exceed 8 characters. 

### Adding custom vehicle images - jpg

Vehicle images are loaded from 'CheatMenu\vehicles\images\' directory. The image name should contain the vehicle model id. You don't need to add custom vehicle names in v2.0-beta & above.

### Adding custom vehicle component images - jpg

Vehicle component images are loaded from 'CheatMenu\vehicles\component\' directory. The image name should contain the component model id.

### Adding custom vehicle paint jobs images - png

Paintjob images are loaded from 'CheatMenu\vehicles\paintjobs\' directory. Image names can be any string and their name will show up on the menu.

### Adding custom weapons images - jpg

Weapon images are loaded from 'CheatMenu\weapons\' directory. The image name should contain the weapon model id. Names for custom weapons have to be added inside 'CheatMenu\json\weapon.json' file.

### Adding custom clothes - jpg

Cloth images are loaded from 'CheatMenu\clothes\' directory. The image name should in this format 'body_part$model_name$texture_name'
