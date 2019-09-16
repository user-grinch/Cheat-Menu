-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019 Grinch_

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

script_name 'Cheat Menu'
script_author("Grinch_")
script_description("Cheat Menu for Grand Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
script_dependencies("ffi","mimgui","memory","MoonAdditions")
script_properties('work-in-pause')
script_version("1.7-staging")
script_version_number(16092019) -- DDMMYYYY

-- All the command keys used throughout the Cheat-Menu
tkeys =
{
    control_key       = 0xA2, -- LCONTROL
    menu_open_key     = 0x4D, -- M
    screenshot_key    = 0x53, -- S
    quickspawner_key  = 0x51, -- Q
    asc_key           = 0x0D, -- RETURN/Enter - used for aim skin changer
    teleport_key1     = 0x58, -- X - key1 for quick teleport
    teleport_key2     = 0x59, -- Y - key2 for quick teleport
    mc_paste          = 0x56, -- V - memory control paste memory address
    airbreak_forward  = 0x57, -- W
    airbreak_backward = 0x53, -- S 
    airbreak_left     = 0x41, -- A
    airbreak_right    = 0x44, -- D 
    airbreak_up       = 0x26, -- Arrow up 
    airbreak_down     = 0x28, -- Arrow down 
    aircraft_zoom     = 0x56, -- V
    coord_copy_key    = 0x43, -- C 
}

resX,resY = getScreenResolution()

-- Script Dependencies
ffi           = require "ffi"
imgui         = require 'mimgui'
memory        = require 'memory'
glob          = require 'game.globals'
mad           = require 'MoonAdditions'


tcheatmenu = 
{
    dir = string.format( "%s%s",getWorkingDirectory(),"//lib//cheat-menu//")
}

-- Loading custom modules
fconfig       = require 'cheat-menu.modules.config'
if not pcall(fconfig.Read) then
    printString("~r~Failed~w~ to load config file.",10000)
end

ftable        = require 'cheat-menu.modules.tables.$index'
fabout        = require 'cheat-menu.modules.about'
fanimation    = require 'cheat-menu.modules.animation'
fcommon       = require 'cheat-menu.modules.common'
fgame         = require 'cheat-menu.modules.game'
fmemory       = require 'cheat-menu.modules.memory'
fmenu         = require 'cheat-menu.modules.menu'
fmission      = require 'cheat-menu.modules.mission'
fped          = require 'cheat-menu.modules.ped'
fplayer       = require 'cheat-menu.modules.player'
fteleport     = require 'cheat-menu.modules.teleport'
fupdate       = require 'cheat-menu.modules.update'
fvehicle      = require 'cheat-menu.modules.vehicle'
fvisual       = require 'cheat-menu.modules.visual'
fweapon       = require 'cheat-menu.modules.weapon'

if fmenu.tmenu.disable_in_samp[0] and isSampLoaded() then
    script.this:unload()
end

tcheatmenu =
{
    window =
    {
        size =
        {
            X = resX/4,
            Y = resY/1.2,
        },
        main    = imgui.new.bool(false),
        title   = string.format("%s v%s by %s",script.this.name,script.this.version,script.this.authors[1]),
        overlay =
        {
            main     = imgui.new.bool(true),
            distance = fconfig.get('tcheatmenu.window.overlay.distance',10.0),
            corner   = imgui.new.int(fconfig.get('tcheatmenu.window.overlay.corner',0)),
            names    = {"Custom","TopLeft","TopRight","BottomLeft","BottomRight","Close"},
            list     = {},
        },
    },
    menubuttons =
    {
        current =  fconfig.get('tcheatmenu.menubuttons.current',-1),
    },
    font_path  =  tcheatmenu.dir .. "fonts//",
    cursor = 
    {
        state  = nil,
    },
    update =
    {
        available = false,
        version_number = 0
    },
}

tcheatmenu.window.overlay.list = imgui.new['const char*'][#tcheatmenu.window.overlay.names](tcheatmenu.window.overlay.names)

function ternary ( cond , T , F )
    if cond then return T else return F end
end

imgui.OnInitialize(function() -- Called once
    -- Styles
    imgui.PushStyleVarFloat(imgui.StyleVar.WindowBorderSize,0)
    imgui.PushStyleVarFloat(imgui.StyleVar.FramePadding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.PopupRounding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.PopupBorderSize,0)
    imgui.PushStyleVarFloat(imgui.StyleVar.ChildBorderSize,0)
    imgui.PushStyleVarFloat(imgui.StyleVar.WindowRounding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.ScrollbarRounding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.TabRounding,3)
    imgui.PushStyleVarVec2(imgui.StyleVar.WindowTitleAlign,imgui.ImVec2(0.5,0.5))
    imgui.PushStyleColor(imgui.Col.Header, imgui.ImVec4(0,0,0,0))
end)

imgui.OnFrame(

function() -- condition
    if (tcheatmenu.window.main[0] or tcheatmenu.window.overlay.main[0]) then
        return true
    end
    return false
end,
function() -- render frame
    if not isGamePaused() then
        
        if tcheatmenu.window.main[0] then
           
            imgui.SetNextWindowSize(imgui.ImVec2(tcheatmenu.window.size.X,tcheatmenu.window.size.Y), imgui.Cond.FirstUseEver)

            imgui.Begin(tcheatmenu.window.title, tcheatmenu.window.main,imgui.WindowFlags.NoCollapse + imgui.WindowFlags.NoSavedSettings)

            if tcheatmenu.update.available then
                imgui.Spacing()
                if imgui.Button(string.format( "New version available %d | Click to hide",tcheatmenu.update.version_number), imgui.ImVec2(fcommon.GetSize(1))) then
                    --lua_thread.create(fupdate.DownloadUpdates)
                    tcheatmenu.update.available = false
                end
                imgui.Spacing()            
            end
            fcommon.UiCreateButtons({"Teleport","Memory","Player","Animation","Vehicle","Weapon","Peds","Mission","Game","Visual","Menu",
            "About"},{fteleport.TeleportMain,fmemory.MemoryMain,fplayer.PlayerMain,fanimation.AnimationMain,fvehicle.VehicleMain,
            fweapon.WeaponMain,fped.PedMain,fmission.MissionMain,fgame.GameMain,fvisual.VisualMain,fmenu.MenuMain,
            fabout.AboutMain})

            imgui.End()
        end

        --Overlay window
        if tcheatmenu.window.overlay.main[0] then
            if fmenu.tmenu.overlay.fps[0] or fmenu.tmenu.overlay.coordinates[0] or (( fmenu.tmenu.overlay.speed[0] or fmenu.tmenu.overlay.health[0]) and isCharInAnyCar(PLAYER_PED)) then
                if (tcheatmenu.window.overlay.corner[0] ~= 0) then
                    window_pos       = imgui.ImVec2(ternary((tcheatmenu.window.overlay.corner[0] == 2 or tcheatmenu.window.overlay.corner[0] == 4),resX - tcheatmenu.window.overlay.distance,tcheatmenu.window.overlay.distance),ternary((tcheatmenu.window.overlay.corner[0] == 3 or tcheatmenu.window.overlay.corner[0] == 4),resY - tcheatmenu.window.overlay.distance,tcheatmenu.window.overlay.distance))
                    window_pos_pivot = imgui.ImVec2(ternary((tcheatmenu.window.overlay.corner[0] == 2 or tcheatmenu.window.overlay.corner[0] == 4),1.0,0.0),ternary((tcheatmenu.window.overlay.corner[0] == 3 or tcheatmenu.window.overlay.corner[0] == 4),1.0,0.0))
                    imgui.SetNextWindowPos(window_pos,0,window_pos_pivot)
                end
                imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,0.65)
                if imgui.Begin('Overlay', tcheatmenu.window.overlay.main,imgui.WindowFlags.NoTitleBar + imgui.WindowFlags.NoResize + imgui.WindowFlags.NoCollapse + imgui.WindowFlags.AlwaysAutoResize + imgui.WindowFlags.NoFocusOnAppearing) then
                    if fmenu.tmenu.overlay.fps[0] == true then
                        imgui.Text("Frames :" .. tostring(math.floor(imgui.GetIO().Framerate)))
                    end

                    if isCharInAnyCar(PLAYER_PED) then
                        car = getCarCharIsUsing(PLAYER_PED)
                        if  fmenu.tmenu.overlay.speed[0] == true then
                            speed = getCarSpeed(car)
                            total_gears = getCarNumberOfGears(car)
                            current_gear = getCarCurrentGear(car)
                            imgui.Text(string.format("Speed   :%d %d/%d",math.floor(speed),current_gear,total_gears))
                        end

                        if fmenu.tmenu.overlay.health[0] == true then
                            imgui.Text(string.format("Health  :%.0f%%",getCarHealth(car)/10))
                        end
                    end

                    if fmenu.tmenu.overlay.coordinates[0] == true then
                        x,y,z = getCharCoordinates(PLAYER_PED)
                        imgui.Text(string.format("Coordinates : %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
                    end
                    
                    imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,1.0)
                    if imgui.BeginPopupContextWindow() then
                        imgui.Text("Position")
                        imgui.Separator()
                        if (imgui.MenuItemBool("Custom",nil,tcheatmenu.window.overlay.corner[0] == 0)) then tcheatmenu.window.overlay.corner[0] = 0 end
                        if (imgui.MenuItemBool("TopLeft",nil,tcheatmenu.window.overlay.corner[0] == 1)) then tcheatmenu.window.overlay.corner[0] = 1 end
                        if (imgui.MenuItemBool("TopRight",nil,tcheatmenu.window.overlay.corner[0] == 2)) then tcheatmenu.window.overlay.corner[0] = 2 end
                        if (imgui.MenuItemBool("BottomLeft",nil,tcheatmenu.window.overlay.corner[0] == 3)) then tcheatmenu.window.overlay.corner[0] = 3 end
                        if (imgui.MenuItemBool("BottomRight",nil,tcheatmenu.window.overlay.corner[0] == 4)) then tcheatmenu.window.overlay.corner[0] = 4 end
                        if  imgui.MenuItemBool("Close") then
                            fgame.tgame.fps.bool[0] = false
                            fvehicle.tvehicles.show.speed[0] = false
                            fvehicle.tvehicles.show.health[0] = false
                            fvisual.tvisuals.show_coordinates[0] = false
                        end
                        imgui.EndPopup()
                    end
                    imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,0.65)
                    imgui.PopStyleVar()
                    imgui.End()
                end
            end
        end   
    end  
end).HideCursor = true

function ShowHideCursor()
    while true do
        wait(0)
        if fmenu.tmenu.manual_mouse[0] == true and imgui.IsMouseDoubleClicked(1) then
            tcheatmenu.cursor.state = not tcheatmenu.cursor.state
            showCursor(tcheatmenu.cursor.state)
            wait(250)
        end
    end
end

function main()

    if fmenu.tmenu.auto_update_check[0] then
        if not pcall(fupdate.CheckUpdates) then
            printHelpString("~r~Failed~w~ to check for update")
        end
    end

    lua_thread.create(ShowHideCursor)

    if fgame.tgame.disable_help_popups[0] == true then
        setGameGlobal(glob.Help_Wasted_Shown,1)
        setGameGlobal(glob.Help_Busted_Shown,1)
        removePickup(glob.Pickup_Info_Hospital)
        removePickup(glob.Pickup_Info_Hospital_2)
        removePickup(glob.Pickup_Info_Police)
    end

    if fgame.tgame.disable_cheats[0] == true then
        writeMemory(0x004384D0 ,1,0xE9 ,false)
        writeMemory(0x004384D1 ,4,0x000000D0 ,false)
        writeMemory(0x004384D5 ,4,0x90909090 ,false)
    end
    
    while true do

        if not isGamePaused() then
            if fmenu.tmenu.manual_mouse[0] then
                showCursor(tcheatmenu.cursor.state)
            else
                showCursor(tcheatmenu.window.main[0])
            end
        else
            showCursor(false)
        end

        if fgame.tgame.airbreak[0] then
            fgame.AirbreakMode()
        end

        if fgame.tgame.ss_shortcut[0]
        and isKeyDown(tkeys.control_key) and isKeyDown(tkeys.screenshot_key) then
            takePhoto(true)
            printHelpString("Screenshot ~g~taken")
            fcommon.KeyWait(tkeys.control_key,tkeys.screenshot_key)
        end

        if isKeyDown(tkeys.control_key) and isKeyDown(tkeys.menu_open_key) then
            fcommon.KeyWait(tkeys.control_key,tkeys.menu_open_key)
            tcheatmenu.window.main[0] = not tcheatmenu.window.main[0]
        end

        if fteleport.tteleport.shortcut[0]
        and isKeyDown(tkeys.teleport_key1)
        and isKeyDown(tkeys.teleport_key2) then
            fcommon.KeyWait(tkeys.teleport_key1,tkeys.teleport_key2)
            fteleport.Teleport()
        end

        if isKeyDown(tkeys.control_key)
        and isKeyDown(tkeys.coord_copy_key) then
            fcommon.KeyWait(tkeys.control_key,tkeys.ss_shortcut)
            setClipboardText(ffi.string(fteleport.tteleport.coords))
            printHelpString("~g~Saved~w~ coordinates to clipboard")
        end

        if fplayer.tplayer.god[0] then
            setCharProofs(PLAYER_PED,true,true,true,true,true)
        else
            setCharProofs(PLAYER_PED,false,false,false,false,false)
        end

        if fplayer.tplayer.aimSkinChanger[0] and isKeyDown(tkeys.asc_key) then
            fcommon.KeyWait(tkeys.asc_key,tkeys.asc_key)
            local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
            if bool == true then
                local model = getCharModel(char)
                fplayer.ChangePlayerModel(model)
            end
        end

        if isKeyDown(tkeys.control_key) and isKeyDown(tkeys.quickspawner_key) then
            if (fvehicle.tvehicles.quick_spawn[0] or fweapon.tweapons.quick_spawn[0]) then
                fcommon.QuickSpawner()
            end
        end

        -- This is to fix car colors not being applied using opcode changecarcolours



        if isCharInAnyHeli(PLAYER_PED)
        or isCharInAnyPlane(PLAYER_PED) then
            lua_thread.create(fvehicle.AircraftCamera)
        end

        -- Vehicle related stuff which is required to run every frame
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)

            if fvehicle.tvehicles.color.default ~= -1 then
                local color_id = getCarColours(car)
                if fvehicle.tvehicles.color.default ~= color_id then
                    fvehicle.ForEachCarComponent(function(mat)
                        mat:reset_color()
                    end)
                end
            end
            if fvehicle.tvehicles.lock_health[0] then
                setCarHealth(car,1000)
            end

            if fvehicle.tvehicles.stay_on_bike[0] then
                setCharCanBeKnockedOffBike(PLAYER_PED,true)
            else
                setCharCanBeKnockedOffBike(PLAYER_PED,false)
            end

            if fvehicle.tvehicles.lock_speed[0] then
                if fvehicle.tvehicles.speed[0] > 500 then
                    fvehicle.tvehicles.speed[0] = 500
                end
                setCarForwardSpeed(car,fvehicle.tvehicles.speed[0])
            end

            if fvehicle.tvehicles.heavy[0] then
                setCarHeavy(car,true)
            else
                setCarHeavy(car,false)
            end

            if fvehicle.tvehicles.visual_damage[0] == true then
                setCarCanBeVisiblyDamaged(car,false)
            else
                setCarCanBeVisiblyDamaged(car,true)
            end

            if getCarDoorLockStatus(car) == 4 then
                fvehicle.tvehicles.lock_doors[0] = true
            else
                fvehicle.tvehicles.lock_doors[0] = false
            end
        else
            fvehicle.tvehicles.lock_doors[0] = false
            fvehicle.tvehicles.lights.all[0] = false
        end

        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() then
        showCursor(false,false)
        fconfig.write()
        local crash_text = "Cheat menu crashed unexpectedly.Sent moonloader.log for debugging"
        if fmenu.tmenu.auto_reload[0] then
            script.this:reload()
            crash_text = "Cheat menu crashed unexpectedly & reloaded.Sent moonloader.log for debugging"
        end
        if fmenu.tmenu.show_crash_message[0] then
            printHelpString(crash_text)
        end
    end
end
