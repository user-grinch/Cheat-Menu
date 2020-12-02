
<p align="center">
  <img width="400" height="250" src="https://i.imgur.com/fZ71SbF.png">
</p>

#
![GitHub repo size](https://img.shields.io/github/repo-size/user-grinch/Cheat-Menu?label=Size&style=for-the-badge)
[![License](https://img.shields.io/github/license/user-grinch/Cheat-Menu?style=for-the-badge)](https://github.com/user-grinch/Cheat-Menu/blob/master/LICENSE)
[![Discord](https://img.shields.io/discord/689515979847237649?label=Discord&style=for-the-badge)](https://discord.gg/ZzW7kmf)

[![MixMods](https://img.shields.io/badge/Topic-Mixmods-%234e4784?style=for-the-badge)](https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu)
[![GTAF](https://img.shields.io/badge/Topic-GTA%20Forums-%23244052?style=for-the-badge)](https://gtaforums.com/topic/961636-moon-cheat-menu/)
## Introduction

Cheat menu for Grand Theft Auto SanAndreas which allows a large set of modifications/cheats to be applied enabling a much more easier & amusing gameplay.

It's based on [Moonloader](https://gtaforums.com/topic/890987-moonloader/) and uses [mimgui](https://github.com/THE-FYP/mimgui) for its interface.

Press <kbd>Left Ctrl</kbd> + <kbd>M</kbd> to open the menu. <b>Doesn't work in SAMP</b>.

## Versions

### Releases
[Releases](https://github.com/inanahammad/Cheat-Menu/releases) are the most up to date & stable versions. If you want a smooth experience with minimal bugs as possible this is the one to get.

### Master Branch
[Master branch](https://github.com/inanahammad/Cheat-Menu) contains all the recent work done to the menu. While this can be the best way to live on the edge & try the latest features out, it could come with a fair number of bugs.

A portuguese translation of the menu is [here](https://github.com/Dowglass/Cheat-Menu)

## Installation

1. Install [DirectX](https://www.microsoft.com/en-us/download/details.aspx?id=35) &  [Visual C++ Redistributable 2017](https://aka.ms/vs/16/release/vc_redist.x86.exe) if not already installed.
2. If your game version isn't v1.0 then you'll need to [downgrade](https://gtaforums.com/topic/927016-san-andreas-downgrader/).
3. Install asi loader from [here](https://www.gtagarage.com/mods/show.php?id=21709)
4. Download [Moonloader](https://gtaforums.com/topic/890987-moonloader/) from [here](https://blast.hk/moonloader/files/moonloader-026.zip).Then extract all these files to your game folder (replace if necessary).
5. Download [Cheat Menu](https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu) from [here](https://github.com/inanahammad/Cheat-Menu/releases).
6. Open up that achieve file and extract everything in game directory (replace if necessary).

Now try it out in-game. If it doesn't work open an issue here or create a post in one of the topics above with 'moonloader.log'.

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

Ped/skin images are loaded from '\moonloader\lib\cheat-menu\peds\' directory. The image name should contain the ped model id. Names for custom peds have to be added inside '\moonloader\lib\cheat-menu\json\ped.json' file.

### Add custom player skins #2

You can also add player skins by using a different method. Place your skin txd & dff files inside 'modloader\Custom Skins\' folder. Note, [Modloader](https://gtaforums.com/topic/669520-mod-loader/) is required & names can't exceed 8 characters. 

### Adding custom vehicle images - jpg

Vehicle images are loaded from '\moonloader\lib\cheat-menu\vehicles\images\' directory. The image name should contain the vehicle model id. You don't need to add custom vehicle names in v2.0-beta & above.

### Adding custom vehicle component images - jpg

Vehicle component images are loaded from '\moonloader\lib\cheat-menu\vehicles\component \' directory. The image name should contain the component model id.

### Adding custom vehicle paint jobs images - png

Paintjob images are loaded from '\moonloader\lib\cheat-menu\vehicles\paintjobs\' directory. Image names can be any string and their name will show up on the menu.

### Adding custom weapons images - jpg

Weapon images are loaded from '\moonloader\lib\cheat-menu\weapons\' directory. The image name should contain the weapon model id. Names for custom weapons have to be added inside '\moonloader\lib\cheat-menu\json\weapon.json' file.

### Adding custom clothes - jpg

Cloth images are loaded from '\moonloader\lib\cheat-menu\clothes\' directory. The image name should in this format 'body_part$model_name$texture_name'
