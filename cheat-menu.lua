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
script_dependencies("ffi","lfs","memory","mimgui","MoonAdditions")
script_properties('work-in-pause')
script_version("1.8-wip")
script_version_number(20191021) -- YYYYMMDD

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
}

ffi           = require "ffi"
imgui         = require 'mimgui'
memory        = require 'memory'
glob          = require 'game.globals'
mad           = require 'MoonAdditions'
lfs           = require 'lfs'

Dir = string.format( "%s%s",getWorkingDirectory(),"//lib//cheat-menu//")

tcheatmenu =
{   
    dir = Dir,
}

-- Loading custom modules
fconfig       = require 'cheat-menu.modules.config'
if not pcall(fconfig.Read) then
    printString("~r~Failed~w~ to load config file.",10000)
end

fcommon       = require 'cheat-menu.modules.common'
fanimation    = require 'cheat-menu.modules.animation'
fgame         = require 'cheat-menu.modules.game'
fmemory       = require 'cheat-menu.modules.memory'
fmenu         = require 'cheat-menu.modules.menu'
fmission      = require 'cheat-menu.modules.mission'
fped          = require 'cheat-menu.modules.ped'
fplayer       = require 'cheat-menu.modules.player'
fstat         = require 'cheat-menu.modules.stat'
fteleport     = require 'cheat-menu.modules.teleport'
fvehicle      = require 'cheat-menu.modules.vehicle'
fvisual       = require 'cheat-menu.modules.visual'
fweapon       = require 'cheat-menu.modules.weapon'

if fmenu.tmenu.disable_in_samp[0] and isSampLoaded() then
    thisScript():reload()
end

resX,resY = getScreenResolution()

tcheatmenu =
{   
    dir = Dir,
    window =
    {
        size =
        {
            X = fconfig.get('tcheatmenu.window.size.X',resX/4),
            Y = fconfig.get('tcheatmenu.window.size.Y',resY/1.2),
        },
        show    = imgui.new.bool(false),
        title   = string.format("%s v%s by %s",script.this.name,script.this.version,script.this.authors[1]),
    },
    current_menu = fconfig.get('tcheatmenu.current_menu',1),
}

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
    return tcheatmenu.window.show[0] and not isGamePaused() 
end,
function(self) -- render frame

    self.LockPlayer = fmenu.tmenu.lock_player[0] 
    imgui.SetNextWindowSize(imgui.ImVec2(tcheatmenu.window.size.X,tcheatmenu.window.size.Y),imgui.Cond.Once)
    imgui.PushStyleVarVec2(imgui.StyleVar.WindowMinSize,imgui.ImVec2(250,500))
    imgui.Begin(tcheatmenu.window.title, tcheatmenu.window.show,imgui.WindowFlags.NoCollapse + imgui.WindowFlags.NoSavedSettings)
    imgui.PopStyleVar()

    if fmenu.tmenu.update_available then
        imgui.Spacing()
        if imgui.Button(string.format( "New version available | Click to hide"), imgui.ImVec2(fcommon.GetSize(1))) then
            fmenu.tmenu.update_available = false
        end
        imgui.Spacing()            
    end

    fcommon.CreateMenus({"Teleport","Memory","Player","Ped","Animation","Vehicle","Weapon","Mission","Stat","Game","Visual","Menu"},
    {fteleport.TeleportMain,fmemory.MemoryMain,fplayer.PlayerMain,fped.PedMain,fanimation.AnimationMain,fvehicle.VehicleMain,
    fweapon.WeaponMain,fmission.MissionMain,fstat.StatMain,fgame.GameMain,fvisual.VisualMain,fmenu.MenuMain})

    tcheatmenu.window.size.X = imgui.GetWindowWidth()
    tcheatmenu.window.size.Y = imgui.GetWindowHeight()
    imgui.End()
end)


imgui.OnFrame(function() 

    return not isGamePaused() and fmenu.tmenu.overlay.show[0] and (fmenu.tmenu.overlay.fps[0] or fmenu.tmenu.overlay.coordinates[0]
    or ((fmenu.tmenu.overlay.speed[0] or fmenu.tmenu.overlay.health[0]) and isCharInAnyCar(PLAYER_PED)))
end,
function()
    local io = imgui.GetIO()
    local overlay = fmenu.tmenu.overlay
    local pos = overlay.position_index[0]

    if pos > 0 then
        x = (pos == 1 or pos == 3) and overlay.offset[0] or io.DisplaySize.x - overlay.offset[0]
        y = (pos == 1 or pos == 2) and overlay.offset[0] or io.DisplaySize.y - overlay.offset[0]
        local window_pos_pivot = imgui.ImVec2((pos == 1 or pos == 3) and 0 or 1, (pos == 1 or pos == 2) and 0 or 1)
        imgui.SetNextWindowPos(imgui.ImVec2(x, y), imgui.Cond.Always, window_pos_pivot)
    end

    local flags = imgui.WindowFlags.NoDecoration + imgui.WindowFlags.AlwaysAutoResize + imgui.WindowFlags.NoSavedSettings
    if pos ~= 0 then
        flags = flags + imgui.WindowFlags.NoMove
    end

    imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,0.65)
    imgui.Begin("", nil, flags)
    
        if fmenu.tmenu.overlay.fps[0] then
            imgui.Text("Frames :" .. tostring(math.floor(imgui.GetIO().Framerate)))
        end
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)
            if  fmenu.tmenu.overlay.speed[0] then
                speed = getCarSpeed(car)
                total_gears = getCarNumberOfGears(car)
                current_gear = getCarCurrentGear(car)
                imgui.Text(string.format("Speed   :%d %d/%d",math.floor(speed),current_gear,total_gears))
            end

            if fmenu.tmenu.overlay.health[0] then
                imgui.Text(string.format("Health  :%.0f%%",getCarHealth(car)/10))
            end
        end

        if fmenu.tmenu.overlay.coordinates[0] then
            x,y,z = getCharCoordinates(PLAYER_PED)
            imgui.Text(string.format("Coordinates : %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
        end
        imgui.PopStyleVar()
        if imgui.BeginPopupContextWindow() then
            imgui.Text("Position")
            imgui.Separator()

            if imgui.MenuItemBool("Custom",nil,fmenu.tmenu.overlay.position_index[0] == 0) then 
                fmenu.tmenu.overlay.position_index[0] = 0 
            end
            if imgui.MenuItemBool("Top Left",nil,fmenu.tmenu.overlay.position_index[0] == 1) then 
                fmenu.tmenu.overlay.position_index[0] = 1 
            end
            if imgui.MenuItemBool("Top Right",nil,fmenu.tmenu.overlay.position_index[0] == 2) then 
                fmenu.tmenu.overlay.position_index[0] = 2 
            end
            if imgui.MenuItemBool("Bottom Left",nil,fmenu.tmenu.overlay.position_index[0] == 3) then 
                fmenu.tmenu.overlay.position_index[0] = 3 
            end
            if imgui.MenuItemBool("Bottom Right",nil,fmenu.tmenu.overlay.position_index[0] == 4) then 
                fmenu.tmenu.overlay.position_index[0] = 4 
            end
            if  imgui.MenuItemBool("Close") then
                fgame.tgame.fps.bool[0] = false
                fvehicle.tvehicle.show.speed[0] = false
                fvehicle.tvehicle.show.health[0] = false
                fvisual.tvisuals.show_coordinates[0] = false
            end
            imgui.EndPopup()        
        end
    imgui.End()
end).HideCursor = true


function main()

    math.randomseed(getGameTimer())

    if fmenu.tmenu.auto_update_check[0] then
        fmenu.CheckUpdates()
    end

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

    switchArrestPenalties(fgame.tgame.keep_stuff[0])
    switchDeathPenalties(fgame.tgame.keep_stuff[0])
    setGangWarsActive(fped.tped.gang_wars[0])
    setPlayerFastReload(PLAYER_HANDLE,fweapon.tweapon.fast_reload[0])
    
    if fweapon.tweapon.no_reload[0] then
        writeMemory( 7600773,1,144,1)
        writeMemory( 7600815,1,144,1)
        writeMemory( 7600816,2,37008,1)
        writeMemory( 7612591,1,144,1)
        writeMemory( 7612646,1,144,1)
        writeMemory( 7612647,2,37008,1)
    else
        writeMemory( 7600773,1,72,1)
        writeMemory( 7600815,1,255,1)
        writeMemory( 7600816,2,3150,1)
        writeMemory( 7612591,1,72,1)
        writeMemory( 7612646,1,255,1)
        writeMemory( 7612647,2,3150,1)
    end

    lua_thread.create(fgame.RandomCheats)

    for key,value in pairs(fgame.tgame.cop) do
        if  fgame.tgame.ghost_cop_cars[0] then
            writeMemory(tonumber(key),4,math.random(400,611),false)
        else
            writeMemory(tonumber(key),4,value,false)
        end
    end

    while true do

        if fanimation.tanimation.ped[0] == true or fweapon.tweapon.ped[0] == true then
            bool, ped = getCharPlayerIsTargeting(PLAYER_HANDLE)
            if bool == true then
                fped.tped.selected = ped
            end
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
            tcheatmenu.window.show[0] = not tcheatmenu.window.show[0]
        end

        if fteleport.tteleport.shortcut[0]
        and isKeyDown(tkeys.teleport_key1)
        and isKeyDown(tkeys.teleport_key2) then
            fcommon.KeyWait(tkeys.teleport_key1,tkeys.teleport_key2)
            fteleport.Teleport()
        end
        
        setCharProofs(PLAYER_PED,fplayer.tplayer.god[0],fplayer.tplayer.god[0],fplayer.tplayer.god[0],fplayer.tplayer.god[0],fplayer.tplayer.god[0])

        if fplayer.tplayer.aimSkinChanger[0] and isKeyDown(tkeys.asc_key) then
            fcommon.KeyWait(tkeys.asc_key,tkeys.asc_key)
            local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
            if bool == true then
                local model = getCharModel(char)
                fplayer.ChangePlayerModel(model)
            end
        end

        if isKeyDown(tkeys.control_key) and isKeyDown(tkeys.quickspawner_key) then
            if (fvehicle.tvehicle.quick_spawn[0] or fweapon.tweapon.quick_spawn[0]) then
                fcommon.QuickSpawner()
            end
        end

        if isCharInAnyHeli(PLAYER_PED)
        or isCharInAnyPlane(PLAYER_PED) then
            lua_thread.create(fvehicle.AircraftCamera)
        end

        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)

            if fvehicle.tvehicle.color.default ~= -1 then
                local color_id = getCarColours(car)
                if fvehicle.tvehicle.color.default ~= color_id then
                    fvehicle.ForEachCarComponent(function(mat)
                        mat:reset_color()
                    end)
                end
            end

            setCarCanBeDamaged(car,not(fvehicle.tvehicle.no_damage[0]))
            setCharCanBeKnockedOffBike(PLAYER_PED,fvehicle.tvehicle.stay_on_bike[0])
            setCarHeavy(car,fvehicle.tvehicle.heavy[0])

            if fvehicle.tvehicle.lock_speed[0] then
                if fvehicle.tvehicle.speed[0] > 500 then
                    fvehicle.tvehicle.speed[0] = 500
                end
                setCarForwardSpeed(car,fvehicle.tvehicle.speed[0])
            end

            setCarCanBeVisiblyDamaged(car,not(fvehicle.tvehicle.visual_damage[0]))


            if getCarDoorLockStatus(car) == 4 then
                fvehicle.tvehicle.lock_doors[0] = true
            else
                fvehicle.tvehicle.lock_doors[0] = false
            end
        else
            fvehicle.tvehicle.lock_doors[0] = false
            fvehicle.tvehicle.lights[0] = false
        end

        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() then
        fconfig.write()
        if fconfig.tconfig.reset == false then
            local crash_text = "Cheat menu crashed unexpectedly"
            if fmenu.tmenu.auto_reload[0] then
                script.this:reload()
                crash_text =  crash_text .. " and reloaded"
            end
            if fmenu.tmenu.show_crash_message[0] then
                printHelpString(crash_text)
            end
        end
    end
end
