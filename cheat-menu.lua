script_name("Cheat Menu")
script_author("Grinch_")
script_description("Cheat Menu for Grnad Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
script_dependencies("imgui","memory","MoonAdditions")
version = {
    release = "Prerelease",
    number  = 1.25,
}
script_version(version.release)
script_version_number(version.number)

-- All the command keys used throughout the Cheat-Menu
keys = 
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

resX,resY = getScreenResolution()
------------------------------------------------------------

-- Script Dependencies
imgui         = require 'imgui'
memory        = require 'memory'
glob          = require 'game.globals'      
mad           = require 'MoonAdditions'

-- Loading custom modules
fabout        = require 'cheat-menu.modules.about'
fcheats       = require 'cheat-menu.modules.cheats'
fcommon       = require 'cheat-menu.modules.common'
fgame         = require 'cheat-menu.modules.game'
fgangs        = require 'cheat-menu.modules.gangs'
fmain         = require 'cheat-menu.modules.main'
fmemcontrol   = require 'cheat-menu.modules.memory_control'
fmenu         = require 'cheat-menu.modules.menu'
fmisson       = require 'cheat-menu.modules.mission'
fpeds         = require 'cheat-menu.modules.peds'
fplayer       = require 'cheat-menu.modules.player'
fskills       = require 'cheat-menu.modules.skills'
fteleport     = require 'cheat-menu.modules.teleportation'
fvehicles     = require 'cheat-menu.modules.vehicles'
fvisuals      = require 'cheat-menu.modules.visuals'
fweapons      = require 'cheat-menu.modules.weapons'
flibweapons   = require 'lib.game.weapons'
imgui.Spinner = require('cheat-menu.modules.imgui_addons').Spinner
------------------------------------------------------------




cheat_menu = 
{
    auto_reload = imgui.ImBool(false)
}


local DISTANCE = 10.0
local corner = 0
window = {
    main    = imgui.ImBool(false),
    overlay = imgui.ImBool(true),
}


function ternary ( cond , T , F )
    if cond then return T else return F end
end

function imgui.OnDrawFrame()
    if  window.main.v then

        imgui.SetNextWindowSize(imgui.ImVec2(resX/4,resY/1.2), imgui.Cond.FirstUseEver)

        imgui.RenderInMenu = fmenu.tmenu.render_in_menu.v
        imgui.LockPlayer   = fmenu.tmenu.lock_player.v  

        imgui.Begin(string.format("Cheat Menu v%.2f by Grinch_",version.number), window.main)

        if resX < 1280 or resY < 720 then
            imgui.Text("Cheat Menu isn't optimized for your current screen resolution.")
            imgui.Spacing()
        end
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

    -- Overlay window
    if window.overlay.v then
        imgui.ShowCursor   = window.main.v

        if (corner ~= -1) then
            window_pos       = imgui.ImVec2(ternary((corner == 1 or corner == 3),resX - DISTANCE,DISTANCE),ternary((corner == 2 or corner == 3),resY - DISTANCE,DISTANCE))
            window_pos_pivot = imgui.ImVec2(ternary((corner == 1 or corner == 3),1.0,0.0),ternary((corner == 2 or corner == 3),1.0,0.0))
            imgui.SetNextWindowPos(window_pos,0,window_pos_pivot)
        end
        
        if fgame.tfps.bool.v or fvehicles.tvehicles.show.speed.v or fvehicles.tvehicles.show.health.v or fvisuals.show_coordinates.v then
            imgui.PushStyleVar(imgui.StyleVar.Alpha,0.65)
            if imgui.Begin('Overlay', window.overlay,imgui.WindowFlags.NoTitleBar + imgui.WindowFlags.NoResize + imgui.WindowFlags.NoCollapse + imgui.WindowFlags.AlwaysAutoResize + imgui.WindowFlags.NoFocusOnAppearing) then
                if fgame.tfps.bool.v == true then
                    imgui.Text("Frames :" .. tostring(math.floor(imgui.GetIO().Framerate))) 
                end
                
                if isCharInAnyCar(PLAYER_PED) then
                    car = getCarCharIsUsing(PLAYER_PED)
                    if  fvehicles.tvehicles.show.speed.v == true then
                        speed = getCarSpeed(car)
                        total_gears = getCarNumberOfGears(car)
                        current_gear = getCarCurrentGear(car)
                        imgui.Text(string.format("Speed   :%d %d/%d",math.floor(speed),current_gear,total_gears))
                    end
            
                    if fvehicles.tvehicles.show.health.v == true then
                        imgui.Text(string.format("Health  :%.0f%%",getCarHealth(car)/10))
                    end
                end
                
                if fvisuals.show_coordinates.v == true then
                    x,y,z = getCharCoordinates(PLAYER_PED)
                    imgui.Text(string.format("Coordinates: %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
                    if isKeyDown(keys.control_key)
                    and isKeyDown(keys.coords_copy) then
                        fcommon.keywait(keys.control_key,keys.coords_copy)
                        setClipboardText(string.format("%d %d %d", x,y,z))
                        printHelpString("Coordinates saved to clipboard")
                    end
                end

                if imgui.BeginPopupContextWindow() then
                    imgui.Text("Position")
                    imgui.Separator()
                    if (imgui.MenuItem("Custom",nil,corner == -1)) then corner = -1 end
                    if (imgui.MenuItem("Top-Left",nil,corner == 0)) then corner = 0 end
                    if (imgui.MenuItem("Top-Right",nil,corner == 1)) then corner = 1 end
                    if (imgui.MenuItem("Bottom-Left",nil,corner == 2)) then corner = 2 end
                    if (imgui.MenuItem("Bottom-Right",nil,corner == 3)) then corner = 3 end
                    if imgui.MenuItem("Close") then 
                        fgame.tfps.bool.v = false
                        fvehicles.tvehicles.show.speed.v = false 
                        fvehicles.tvehicles.show.health.v = false
                    end
                    imgui.EndPopup()
                end
            end
            imgui.End()
            imgui.PopStyleVar()
        end
    end
end

function main()
    fmenu.menu_apply_style()
  
    while true do

        if isKeyDown(keys.control_key) and isKeyDown(keys.menu_open_key) then
            fcommon.keywait(keys.control_key,keys.menu_open_key)
            window.main.v = not window.main.v
        end

        fmain.main_section()
        imgui.Process = window.main.v or window.overlay.v
        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() and quitGame == false and cheat_menu.auto_reload.v == true then
        script:reload()
    end
end