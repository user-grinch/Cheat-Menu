# Cheat Menu      [![GNU v3.0](https://img.shields.io/badge/license-GNU-blue.svg?style=flat)](https://github.com/inanahammad/Cheat-Menu/blob/master/LICENSE)

## Introduction

Cheat menu for Grand Theft Auto SanAndreas which allows a large set of modifications/cheats to be applied enabling a much more easier & amusing gameplay.

It's based on [Moonloader](https://gtaforums.com/topic/890987-moonloader/) and uses [mimgui](https://github.com/THE-FYP/mimgui) for its interface.

For installation take a look [here](https://github.com/inanahammad/Cheat-Menu/wiki/Installation).
## Official Topics

- [Mixmods](https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu)
- [GTA Forums](https://gtaforums.com/topic/930023-mooncheat-menu/)


## Versions

### Releases
[Releases](https://github.com/inanahammad/Cheat-Menu/releases) are the most up to date & stable versions. If you want a smooth experience with minimal bugs as possible this is the one to get.

### Master Branch
[Master branch](https://github.com/inanahammad/Cheat-Menu) contains all the recent work done to the menu. While this can be the best way to live on the edge & try the latest features out, it could come with a fair number of bugs.


## Installation

1. Install [DirectX](https://www.microsoft.com/en-us/download/details.aspx?id=35) &  [Visual C++ Redistributable 2017](https://aka.ms/vs/16/release/vc_redist.x86.exe) if not already installed.
2. If your game version isn't v1.0 then you'll need to [downgrade](https://gtaforums.com/topic/927016-san-andreas-downgrader/).
3. Download [Moonloader](https://gtaforums.com/topic/890987-moonloader/) from [here](https://blast.hk/moonloader/files/moonloader-027.0-preview2.zip).Then extract all these files to your game folder (replace if necessary).
4. Download [Cheat Menu](https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu) from [here](https://github.com/inanahammad/Cheat-Menu/releases).
5. Open up that achieve file and extract 'lib' folder & 'cheat-menu.lua' into moonloader folder (replace if necessary).

Now try it out in-game. If it doesn't work open an issue here or create a post in one of the topics above with 'moonloader.log'.


## Images
![Image](https://i.imgur.com/4H0xAff.jpg)
![Image](https://i.imgur.com/wVUqbYC.jpg)
![Image](https://i.imgur.com/0C0FQU3.jpg)
![Image](https://i.imgur.com/Fjqd2hi.jpg)


## Youtube
[![Youtue Video](https://img.youtube.com/vi/XF1bhn74s2M/0.jpg)](https://www.youtube.com/watch?v=XF1bhn74s2M)


## Docs

### Adding custom peds/skins images
- Version required 1.8-wip and above


Ped/skin images (only jpg) are loaded from '\moonloader\lib\cheat-menu\peds\' directory. There are more folders inside that directory. Those folders are menus that appear on the lists tab. Images under those directories will appear both on the 'search' tab and on the 'list' tab. The image name should contain the ped model id. Names for custom peds have to be added inside '\moonloader\lib\cheat-menu\json\ped.json' file.

### Adding custom vehicle images 
- Version required 1.8-wip and above

Vehicle images (only jpg) are loaded from '\moonloader\lib\cheat-menu\vehicles\images\' directory. There are more folders inside that directory. Those folders are menus that appear on the lists tab. Images under those directories will appear both on the 'search' tab and on the 'list' tab. The image name should contain the vehicle model id. Names for custom vehicles have to be added inside '\moonloader\lib\cheat-menu\json\vehicle.json' file.

### Adding custom vehicle component images 
- Version required 1.8-wip and above

Vehicle component images (only jpg) are loaded from '\moonloader\lib\cheat-menu\vehicles\component \' directory. There are more folders inside that directory. Those folders are menus that appear on the lists tab. Images under those directories will appear on the 'Tune' tab. The image name should contain the component model id.

### Adding custom vehicle paint jobs images
- Version required 1.8-wip and above

Paintjob images (only png) are loaded from '\moonloader\lib\cheat-menu\vehicles\paintjobs\images\' directory. Images are directly loaded from that directory and appear on the paint jobs tab. Image names can be any string and their name will show up on the menu.

### Adding custom weapons images
- Version required 1.8-wip and above

Weapon images (only jpg) are loaded from '\moonloader\lib\cheat-menu\weapons\' directory. There are more folders inside that directory. Those folders are menus that appear on the lists tab. Images under those directories will appear both on the 'search' tab and on the 'list' tab. The image name should contain the weapon model id. Names for custom vehicles have to be added inside '\moonloader\lib\cheat-menu\json\weapon.json' file.

### Adding custom clothes
- Version required 1.8-wip and above

Cloth images (only jpg) are loaded from '\moonloader\lib\cheat-menu\clothes\' directory. There are more folders inside that directory. Those folders are menus that appear on the lists tab. Images under those directories will appear both on the 'search' tab and on the 'list' tab. The image name should in this format 'body_part$model_name$texture_name'
