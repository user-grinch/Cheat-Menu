script_name("Cheat Menu")
script_author("Grinch_")
script_description("Cheat Menu for Grnad Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
script_dependencies("imgui","memory","MoonAdditions")
config = {}
config.version = {
    release = "Public",
    number  = 1.2,
}
script_version(config.version.release)
script_version_number(config.version.number)

-- All the command keys used throughout the Cheat-Menu
config.keys = 
{
    control_key       = 0xA2, -- LCONTROL
    menu_open_key     = 0x4D, -- M
    screenshot_key    = 0x53, -- S
    quickspawner_key  = 0x51, -- Q
    coords_copy       = 0x43, -- C
    akc_return_key    = 0x0D, -- RETURN/Enter - used for aim skin changer
    teleport_key1     = 0x58, -- X - key1 for quick teleport
    teleport_key2     = 0x59, -- Y - key2 for quick teleport
    mc_paste          = 0x56, -- V - memory control paste memory address
    airbreak_forward  = 0x57, -- W - moves player forward in airbreak mode
    airbreak_backward = 0x53, -- S - moves player backward in airbreak mode
    airbreak_left     = 0x41, -- A - rotates player left in airbreak mode
    airbreak_right    = 0x44, -- D - rotates player right in airbreak mode
    airbreak_up       = 0x26, -- Arrow up - moves player up in airbreak mode
    airbreak_down     = 0x28, -- Arrow down - moves player down in airbreak mode
}

------------------------------------------------------------

-- Script Dependencies
imgui = require 'imgui'
memory = require 'memory'
mad = require 'MoonAdditions'
glob = require 'game.globals'         

-- Loading custom modules
fabout = require 'cheat-menu.modules.about'
fcheats = require 'cheat-menu.modules.cheats'
fcommon = require 'cheat-menu.modules.common'
fgame = require 'cheat-menu.modules.game'
fgangs = require 'cheat-menu.modules.gangs'
flip = require 'cheat-menu.modules.lip'
fmain = require 'cheat-menu.modules.main'
fmemcontrol = require 'cheat-menu.modules.memory_control'
fmenu = require 'cheat-menu.modules.menu'
fmisson = require 'cheat-menu.modules.mission'
fpeds = require 'cheat-menu.modules.peds'
fplayer = require 'cheat-menu.modules.player'
fskills = require 'cheat-menu.modules.skills'
fteleport = require 'cheat-menu.modules.teleportation'
fvehicles = require 'cheat-menu.modules.vehicles'
fvisuals = require 'cheat-menu.modules.visuals'
fweapons = require 'cheat-menu.modules.weapons'
------------------------------------------------------------


local main_window_state = imgui.ImBool(false)

cheat_menu = 
{
    auto_reload = imgui.ImBool(false),
    config_path = "moonloader/Cheat Menu.ini",
}

flip.save(cheat_menu.config_path,config)

function imgui.OnDrawFrame()
    if  main_window_state.v then

        -- Setting up Cheat-Menu size
        resX,resY = getScreenResolution()
        imgui.SetNextWindowSize(imgui.ImVec2(resX/4,resY/1.2), imgui.Cond.FirstUseEver)

        imgui.RenderInMenu = fmenu.tmenu.render_in_menu.v
        imgui.LockPlayer   = fmenu.tmenu.lock_player.v 

        imgui.Begin('Cheat Menu by Grinch_', main_window_state)

        if imgui.CollapsingHeader("Teleportation",true) then
           fteleport.teleportation_section()
        end
        if imgui.CollapsingHeader("Memory Control",true) then
            fmemcontrol.memory_control_section()
        end
        if imgui.CollapsingHeader("Player",true) then
            fplayer.player_section()
        end
        if imgui.CollapsingHeader("Weapons",true) then
            fweapons.weapons_section()
        end
        if imgui.CollapsingHeader("Vehicles",true) then
            fvehicles.vehicles_section()
        end
        if imgui.CollapsingHeader("Skills",true) then
            fskills.skills_section()
        end
        if imgui.CollapsingHeader("Missions",true) then
            fmisson.missions_section()
        end
        if imgui.CollapsingHeader("Peds",true) then
            fpeds.peds_section()
        end
        if imgui.CollapsingHeader("Gangs",true) then
            fgangs.gangs_section()
        end
        if imgui.CollapsingHeader("Cheats",true) then
            fcheats.cheats_section()
        end
        if imgui.CollapsingHeader("Game",true) then
           fgame.game_section()
        end
        if imgui.CollapsingHeader("Visuals",true) then
            fvisuals.visuals_section()
        end
        if imgui.CollapsingHeader("Menu Configuration",true) then
            fmenu.menu_configuration_section()
        end
        if imgui.CollapsingHeader("About",true) then
            fabout.about_section()
        end
        imgui.End()
    end
end

function main()

    -- These will load only once
    fmenu.menu_apply_style()
    fcommon.load_textures()

    while true do
       
        -- This part loops every frame
        
        if isKeyDown(config.keys.control_key) and isKeyDown(config.keys.menu_open_key) then
            fcommon.keywait(config.keys.control_key,config.keys.menu_open_key)
            main_window_state.v = not main_window_state.v
        end

        fmain.main_section()
        
        imgui.Process = main_window_state.v
        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() and quitGame == false and cheat_menu.auto_reload.v == true then
        reloadScripts()
    end
end