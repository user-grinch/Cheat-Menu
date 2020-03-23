-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019-2020 Grinch_

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
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-moon-cheat-menu")
script_dependencies("ffi","lfs","memory","mimgui","MoonAdditions")
script_properties('work-in-pause')
script_version("2.0-beta")
script_version_number(20200324) -- YYYYMMDD

print(string.format("Loading v%s (%d)",script.this.version,script.this.version_num)) -- For debugging purposes

tcheatmenu =
{   
    dir = string.format( "%s%s",getWorkingDirectory(),"/lib/cheat-menu/"),
}

--------------------------------------------------
-- Libraries
copas         = require 'copas'
ffi           = require 'ffi'
glob          = require 'game.globals'
http          = require 'copas.http'
imgui         = require 'mimgui'
lfs           = require 'lfs'
mad           = require 'MoonAdditions'
memory        = require 'memory'
os            = require 'os'
vkeys         = require 'vkeys'
ziplib        = ffi.load(string.format( "%s/lib/ziplib.dll",getWorkingDirectory()))

-- External scripts
gsx           = import 'gsx-data.lua'

-- Menu modules
fcommon       = require 'cheat-menu.modules.common'
fconfig       = require 'cheat-menu.modules.config'
fconst        = require 'cheat-menu.modules.const'

if not fconfig.Get("tmenu.debug.read_config",true) then
    local debug = fconfig.Get("tmenu.debug",{})
    fconfig.tconfig.read = {}
    fconfig.Set(fconfig.tconfig.read,"tmenu.debug",debug)
end

fanimation    = require 'cheat-menu.modules.animation'
fgame         = require 'cheat-menu.modules.game'
fmemory       = require 'cheat-menu.modules.memory'
fmenu         = require 'cheat-menu.modules.menu'
fmission      = require 'cheat-menu.modules.mission'
fped          = require 'cheat-menu.modules.ped'
fplayer       = require 'cheat-menu.modules.player'
fstat         = require 'cheat-menu.modules.stat'
fstyle        = require 'cheat-menu.modules.style'
fteleport     = require 'cheat-menu.modules.teleport'
fvehicle      = require 'cheat-menu.modules.vehicle'
fvisual       = require 'cheat-menu.modules.visual'
fweapon       = require 'cheat-menu.modules.weapon'

--------------------------------------------------

ffi.cdef[[
    int zip_extract(const char *zipname, const char *dir,int *func, void *arg);
]]


-- Unload cheat-menu if disable_in_samp == true
if isSampLoaded() then
    print("SAMP is loaded.")
    if fmenu.tmenu.disable_in_samp[0] then
        print("Disable in samp enabled, unloading script.")
        thisScript():unload()
    end
end

resX, resY = getScreenResolution()
tcheatmenu       =
{   
    dir          = tcheatmenu.dir,
    current_menu = fconfig.Get('tcheatmenu.current_menu',0),
    hot_keys     =
    {
        asc_key              = fconfig.Get('tcheatmenu.hot_keys.asc',{vkeys.VK_RETURN,vkeys.VK_RETURN}),
        camera_mode_forward  = fconfig.Get('tcheatmenu.hot_keys.camera_mode_forward',{vkeys.VK_W,vkeys.VK_W}),
        camera_mode_flip     = fconfig.Get('tcheatmenu.hot_keys.camera_mode_flip',{vkeys.VK_LCONTROL,vkeys.VK_LCONTROL}),
        camera_mode_backward = fconfig.Get('tcheatmenu.hot_keys.camera_mode_backward',{vkeys.VK_S,vkeys.VK_S}),
        camera_mode_x_axis   = fconfig.Get('tcheatmenu.hot_keys.camera_mode_x_axis',{vkeys.VK_X,vkeys.VK_X}),
        camera_mode_y_axis   = fconfig.Get('tcheatmenu.hot_keys.camera_mode_y_axis',{vkeys.VK_Y,vkeys.VK_Y}),
        camera_mode_z_axis   = fconfig.Get('tcheatmenu.hot_keys.camera_mode_z_axis',{vkeys.VK_Z,vkeys.VK_Z}),
        command_window       = fconfig.Get('tcheatmenu.hot_keys.command_window',{vkeys.VK_LMENU,vkeys.VK_M}),
        currently_active     = nil,
        mc_paste             = fconfig.Get('tcheatmenu.hot_keys.mc_paste',{vkeys.VK_LCONTROL,vkeys.VK_V}),
        menu_open            = fconfig.Get('tcheatmenu.hot_keys.menu_open',{vkeys.VK_LCONTROL,vkeys.VK_M}),
        quick_screenshot     = fconfig.Get('tcheatmenu.hot_keys.quick_screenshot',{vkeys.VK_LCONTROL,vkeys.VK_S}),
        quick_teleport       = fconfig.Get('tcheatmenu.hot_keys.quick_teleport',{vkeys.VK_X,vkeys.VK_Y}),
        script_manager_temp  = {vkeys.VK_LCONTROL,vkeys.VK_1}
    },
    read_key_press = false,
    tab_data     = {},
    window       =
    {
        coord    = 
        {
            X    = fconfig.Get('tcheatmenu.window.coord.X',50),
            Y    = fconfig.Get('tcheatmenu.window.coord.Y',50),
        },
        show     = imgui.new.bool(false),
        size     =
        {
            X    = fconfig.Get('tcheatmenu.window.size.X',resX/4),
            Y    = fconfig.Get('tcheatmenu.window.size.Y',resY/1.2),
        },
        title    = string.format("%s v%s",script.this.name,script.this.version),
    },
}

imgui.OnInitialize(function() -- Called once
    
    -- Loading fonts
   fstyle.LoadFonts()

    if not doesFileExist(tcheatmenu.dir .. "json//styles.json") then 
        fstyle.saveStyles(imgui.GetStyle(), "Default") 
    end
    
    fstyle.tstyle.status = fstyle.loadStyles() 

    if fstyle.tstyle.status then
        fstyle.tstyle.list  = fstyle.getStyles()
        fstyle.tstyle.array = imgui.new['const char*'][#fstyle.tstyle.list](fstyle.tstyle.list)

        for i=1,#fstyle.tstyle.list,1 do
            if fstyle.tstyle.list[i] == fstyle.tstyle.selected_name then
                fstyle.tstyle.selected[0] = i - 1
            end
        end

        fstyle.applyStyle(imgui.GetStyle(), fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1])
    else 
        print("Can't load styles")
    end

    -- Indexing images
    lua_thread.create(
    function() 
        fcommon.IndexImages(fvehicle.tvehicle.path,fvehicle.tvehicle.images,fconst.VEHICLE.IMAGE_EXT)
        fcommon.IndexImages(fweapon.tweapon.path,fweapon.tweapon.images,fconst.WEAPON.IMAGE_EXT)
        fcommon.IndexImages(fvehicle.tvehicle.paintjobs.path,fvehicle.tvehicle.paintjobs.images,fconst.PAINTJOB.IMAGE_EXT)
        fcommon.IndexImages(fvehicle.tvehicle.components.path,fvehicle.tvehicle.components.images,fconst.COMPONENT.IMAGE_EXT)
        fcommon.IndexImages(fped.tped.path,fped.tped.images,fconst.PED.IMAGE_EXT)
        fcommon.IndexImages(fplayer.tplayer.clothes.path,fplayer.tplayer.clothes.images,fconst.CLOTH.IMAGE_EXT)
    end)
end)

imgui.OnFrame(
function() -- condition
    return tcheatmenu.window.show[0] and not isGamePaused() 
end,
function(self) -- render frame

    self.LockPlayer = fmenu.tmenu.lock_player[0] 
    imgui.SetNextWindowSize(imgui.ImVec2(tcheatmenu.window.size.X,tcheatmenu.window.size.Y),imgui.Cond.Once)
    imgui.SetNextWindowPos(imgui.ImVec2(tcheatmenu.window.coord.X,tcheatmenu.window.coord.Y),imgui.Cond.Once)
    imgui.PushStyleVarVec2(imgui.StyleVar.WindowMinSize,imgui.ImVec2(250,350))

    local pop = 1
    if fmenu.tmenu.auto_scale[0] then
       imgui.PushStyleVarVec2(imgui.StyleVar.FramePadding,imgui.ImVec2(math.floor(tcheatmenu.window.size.X/85),math.floor(tcheatmenu.window.size.Y/200)))
       pop = pop + 1
    end
    imgui.Begin(tcheatmenu.window.title, tcheatmenu.window.show,imgui.WindowFlags.NoCollapse + imgui.WindowFlags.NoSavedSettings )

    if fmenu.tmenu.update_status == fconst.UPDATE_STATUS.NEW_UPDATE then
        imgui.Button("A new update is available",imgui.ImVec2(fcommon.GetSize(1)))
        if imgui.Button("Download now",imgui.ImVec2(fcommon.GetSize(2))) then

            if string.find( script.this.version,"beta") then
                fmenu.httpRequest("https://github.com/user-grinch/Cheat-Menu/archive/master.zip", nil, function(body, code, headers, status)  
                    print(link, 'OK', status)
                    downloadUrlToFile("https://github.com/user-grinch/Cheat-Menu/archive/master.zip",string.format("%supdate.zip",tcheatmenu.dir),fmenu.DownloadHandler)
                end)
            else
                fmenu.httpRequest("https://api.github.com/repos/user-grinch/Cheat-Menu/tags", nil, function(body, code, headers, status)  
                    print(link, 'OK', status)
                    fmenu.tmenu.repo_version = tostring(decodeJson(body)[1].name)
                    downloadUrlToFile("https://github.com/user-grinch/Cheat-Menu/archive/".. fmenu.tmenu.repo_version .. ".zip",string.format("%supdate.zip",tcheatmenu.dir),fmenu.DownloadHandler)
                end)
            end
            
            printHelpString("Download has started. You'll get notified when the download completes.")
            fmenu.tmenu.update_status = fconst.UPDATE_STATUS.DOWNLOADING
        end
        imgui.SameLine()
        if imgui.Button("Hide message",imgui.ImVec2(fcommon.GetSize(2))) then
            fmenu.tmenu.update_status =fconst.UPDATE_STATUS.HIDE_MSG
        end
        imgui.Spacing()
    end

    if fmenu.tmenu.update_status == fconst.UPDATE_STATUS.DOWNLOADING then
        imgui.Button("Downloading update...",imgui.ImVec2(fcommon.GetSize(1)))
        imgui.Spacing()
    end

    if fmenu.tmenu.update_status == fconst.UPDATE_STATUS.INSTALL then
        if imgui.Button("Install update. This might take a while.",imgui.ImVec2(fcommon.GetSize(1))) then
            fmenu.tmenu.update_status = fconst.UPDATE_STATUS.HIDE_MSG
            fgame.tgame.script_manager.skip_auto_reload = true
            ziplib.zip_extract(tcheatmenu.dir .. "update.zip",tcheatmenu.dir,nil,nil)
            
            if string.find( script.this.version,"beta") then
                fcommon.MoveFiles(getWorkingDirectory() .. "\\lib\\cheat-menu\\Cheat-Menu-master\\",getGameDirectory())
            else
                print(fmenu.tmenu.repo_version)
                fcommon.MoveFiles(getWorkingDirectory() .. "\\lib\\cheat-menu\\Cheat-Menu-" .. fmenu.tmenu.repo_version .. "\\",getGameDirectory())
            end
            
            os.remove(tcheatmenu.dir .. "update.zip")
            printHelpString("Update ~g~Installed")
            print("Update installed. Reloading script.")
            thisScript():reload()
        end
        imgui.Spacing()
    end

    fcommon.CreateMenus({"Teleport","Memory","Player","Ped","Animation","Vehicle","Weapon","Mission","Stat","Game","Visual","Menu"},
    {fteleport.TeleportMain,fmemory.MemoryMain,fplayer.PlayerMain,fped.PedMain,fanimation.AnimationMain,fvehicle.VehicleMain,
    fweapon.WeaponMain,fmission.MissionMain,fstat.StatMain,fgame.GameMain,fvisual.VisualMain,fmenu.MenuMain})
    
    -- Welcome page
    if tcheatmenu.current_menu == 0 then

        if imgui.BeginChild("Welcome") then
            imgui.Dummy(imgui.ImVec2(0,10))
            imgui.TextWrapped("Welcome to " .. tcheatmenu.window.title)
            imgui.Dummy(imgui.ImVec2(0,10))
            imgui.TextWrapped("Please configure the settings below,\n(Recommanded settings are already applied)")
            imgui.Dummy(imgui.ImVec2(0,20))

            if fstyle.tstyle.status then	
                if imgui.Combo('Select style', fstyle.tstyle.selected, fstyle.tstyle.array, #fstyle.tstyle.list) then
                    fstyle.applyStyle(imgui.GetStyle(), fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1])
                    fstyle.tstyle.selected_name = fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]
                end
            end
            imgui.Spacing()
            fcommon.HotKey(tcheatmenu.hot_keys.menu_open,"Cheat Menu open/close hotkey")

            imgui.Dummy(imgui.ImVec2(0,10))

            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("Auto reload",fmenu.tmenu.auto_reload,"Reload cheat menu automatically\nin case of a crash.\n\nMight cause crash loop sometimes.")
            fcommon.CheckBoxVar("Check for updates",fmenu.tmenu.auto_update_check)
            fcommon.InformationTooltip("Cheat Menu will automatically check for updates\nonline. This requires an internet connection and\
will download files from github repository.")
            imgui.NextColumn()
            fcommon.CheckBoxVar("Fast load images",fmenu.tmenu.fast_load_images,"Loads vehicles, weapons, peds etc. images\nat menu startup.\n \
This may increase game startup time or\nfreeze it for few seconds.")
            fcommon.CheckBoxVar("Show tooltips",fmenu.tmenu.show_tooltips)
            fcommon.InformationTooltip("Shows usage tips beside options.")
            imgui.Columns(1)
            imgui.Spacing()
            imgui.TextWrapped("You can configure everything here anytime from the 'Menu' section.")
            if getMoonloaderVersion() < 27 then
                imgui.Dummy(imgui.ImVec2(0,20))
                imgui.TextWrapped("You're using an older version of moonloader. Some features may not work properly. Please update to the lastet version if possible.")
            end
            imgui.EndChild()
        end
    end


    tcheatmenu.window.size.X  = imgui.GetWindowWidth()
    tcheatmenu.window.size.Y  = imgui.GetWindowHeight()
    tcheatmenu.window.coord.X = imgui.GetWindowPos().x
    tcheatmenu.window.coord.Y = imgui.GetWindowPos().y
    
    imgui.End()
    imgui.PopStyleVar(pop)
end)

-- Overlay window
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

    if pos == 0 then
        imgui.SetNextWindowPos(imgui.ImVec2(fmenu.tmenu.overlay.pos_x[0], fmenu.tmenu.overlay.pos_y[0]), imgui.Cond.Once , window_pos_pivot)
    end

    imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,0.65)
    imgui.PushStyleVarVec2(imgui.StyleVar.WindowMinSize,imgui.ImVec2(0,0))
    imgui.Begin("Overlay", nil, flags)
    
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
            imgui.Text(string.format("Coordinates :%d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
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
            if imgui.MenuItemBool("Close") then
                fmenu.tmenu.overlay.fps[0] = false
                fmenu.tmenu.overlay.speed[0] = false
                fmenu.tmenu.overlay.health[0] = false
                fmenu.tmenu.overlay.coordinates[0] = false
            end
            imgui.Separator()
            if imgui.MenuItemBool("Copy coordinates") then 
                local x,y,z = getCharCoordinates(PLAYER_PED)
                setClipboardText(string.format( "%d,%d,%d",x,y,z))
                printHelpString("Coordinates copied")
            end
            imgui.EndPopup()        
        end
        if pos == 0 then
            fmenu.tmenu.overlay.pos_x[0] = imgui.GetWindowPos().x
            fmenu.tmenu.overlay.pos_y[0] = imgui.GetWindowPos().y
        end
    imgui.End()
    imgui.PopStyleVar()

end).HideCursor = true

-- Command window
imgui.OnFrame(function() 
    return not isGamePaused() and fmenu.tmenu.command.show[0]
end,
function()


    imgui.SetNextWindowPos(imgui.ImVec2(0, resY-fmenu.tmenu.command.height), imgui.Cond.Always)
    imgui.SetNextWindowSize(imgui.ImVec2(resX,fmenu.tmenu.command.height))

    local frame_padding  = {imgui.GetStyle().FramePadding.x,imgui.GetStyle().FramePadding.y}
    local window_padding = {imgui.GetStyle().WindowPadding.x,imgui.GetStyle().WindowPadding.y}
    imgui.PushStyleVarVec2(imgui.StyleVar.FramePadding, imgui.ImVec2(frame_padding[1],resY/130))
    imgui.PushStyleVarVec2(imgui.StyleVar.WindowPadding, imgui.ImVec2(3,3))

    imgui.Begin("Command Window", nil, imgui.WindowFlags.NoDecoration + imgui.WindowFlags.AlwaysAutoResize 
    + imgui.WindowFlags.NoSavedSettings + imgui.WindowFlags.NoMove)
    imgui.SetNextItemWidth(resX)
    imgui.SetKeyboardFocusHere(-1)
    if imgui.InputText("##TEXTFIELD",fmenu.tmenu.command.input_field,ffi.sizeof(fmenu.tmenu.command.input_field),imgui.InputTextFlags.EnterReturnsTrue 
    or imgui.InputTextFlags.CallbackCompletion or imgui.InputTextFlags.CallbackHistory) then
        if imgui.IsKeyPressed(vkeys.VK_RETURN,false) then
            fmenu.tmenu.command.show[0] = not fmenu.tmenu.command.show[0]
            fmenu.ExecuteCommand()
            imgui.StrCopy(fmenu.tmenu.command.input_field, "", 1)
        end
    end
    imgui.End()

    imgui.PushStyleVarVec2(imgui.StyleVar.FramePadding, imgui.ImVec2(frame_padding[1],frame_padding[2]))
    imgui.PushStyleVarVec2(imgui.StyleVar.WindowPadding, imgui.ImVec2(window_padding[1],window_padding[2]))

end).HideCursor = true

function main()

    
    --------------------------------------------------
    -- Functions that need to lunch only once on startup

    -- Gang weapons
    for x=1,10,1 do          
        setGangWeapons(x-1,fweapon.tweapon.gang.used_weapons[x][1],fweapon.tweapon.gang.used_weapons[x][2],fweapon.tweapon.gang.used_weapons[x][3])
    end

    -- Invisible player
    if fplayer.tplayer.invisible[0] then
        fplayer.tplayer.model_val = readMemory((getCharPointer(PLAYER_PED)+1140),4,false)
        writeMemory(getCharPointer(PLAYER_PED)+1140,4,2,false)
    end

    -- Mission timer
    if fgame.tgame.freeze_mission_timer[0] then
        freezeOnscreenTimer(true)
    else
        freezeOnscreenTimer(false)
    end

    -- No traffic vehicles 
    if fvehicle.tvehicle.no_vehicles[0] then
        writeMemory(0x434237,1,0x73,false) -- change condition to unsigned (0-255)
        writeMemory(0x434224,1,0,false)
        writeMemory(0x484D19,1,0x83,false) -- change condition to unsigned (0-255)
        writeMemory(0x484D17,1,0,false)
    else
        writeMemory(0x434237,1,-1063242627,false) -- change condition to unsigned (0-255)
        writeMemory(0x434224,1,940431405,false)
        writeMemory(0x484D19,1,292493,false) -- change condition to unsigned (0-255)
        writeMemory(0x484D17,1,1988955949,false)
    end

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

    switchArrestPenalties(not(fgame.tgame.keep_stuff[0]))
    switchDeathPenalties(not(fgame.tgame.keep_stuff[0]))

    if fped.tped.gang.wars[0] then
        setGangWarsActive(fped.tped.gang.wars[0])
    end
    
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

    -- Ghost cop cars
    for key,value in pairs(fgame.tgame.cop) do
        if  fgame.tgame.ghost_cop_cars[0] then
            writeMemory(tonumber(key),4,math.random(400,611),false)
        else
            writeMemory(tonumber(key),4,value,false)
        end
    end

    -- Disable Replay
    if fgame.tgame.disable_replay[0] then
        writeMemory(4588800,1,195,false)
    else
        writeMemory(4588800,1,160,false)
    end

    -- Vehicle gxt names
    for gxt_name,veh_name in pairs(fvehicle.tvehicle.gxt_name_table) do
        setGxtEntry(gxt_name,veh_name)
    end

    -- Command window
    fmenu.RegisterAllCommands()

    -- Set saved values of addresses
    fconfig.SetConfigData()

    -- Camera fov
    cameraSetLerpFov(getCameraFov(),fgame.tgame.camera.fov[0],1000,true)
    cameraPersistFov(true) 

    -- Money text
    -- ffi.copy(ffi.cast("char(*)", fvisual.tvisual.money.negative_memory), ffi.string(fvisual.tvisual.money.negative))
    -- writeMemory(0x58F50A,4,fvisual.tvisual.money.negative_memory,false)
    -- ffi.copy(ffi.cast("char(*)", fvisual.tvisual.money.positive_memory), ffi.string(fvisual.tvisual.money.positive))
    -- writeMemory(0x58F4C8,4,fvisual.tvisual.money.positive_memory,false)

    lua_thread.create(fcommon.ReadKeyPress)
    lua_thread.create(fgame.CameraMode)
    lua_thread.create(fplayer.KeepPosition)
    lua_thread.create(fped.PedHealthDisplay)
    lua_thread.create(fgame.FreezeTime)
    lua_thread.create(fgame.LoadScriptsOnKeyPress)
    lua_thread.create(fgame.SolidWater)
    lua_thread.create(fgame.SyncSystemTime)
    lua_thread.create(fvehicle.AircraftCamera)
    lua_thread.create(fvehicle.FirstPersonCamera)
    lua_thread.create(fweapon.AutoAim)
    lua_thread.create(fvehicle.OnEnterVehicle)
    lua_thread.create(fvehicle.RandomColors)
    lua_thread.create(fgame.RandomCheats)
    lua_thread.create(fvehicle.RandomTrafficColors)
    lua_thread.create(fvehicle.UnlimitedNitro)

    --------------------------------------------------
   
    while true do
        --------------------------------------------------
        -- Functions that neeed to run constantly

        --------------------------------------------------
        
  
        -- Weapons
        local pPed = getCharPointer(PLAYER_PED)
        local CurWeapon = getCurrentCharWeapon(PLAYER_PED)
        local skill = callMethod(0x5E3B60,pPed,1,0,CurWeapon)
        local pWeaponInfo = callFunction(0x743C60,2,2,CurWeapon,skill)

        if fweapon.tweapon.huge_damage[0] then
            writeMemory(pWeaponInfo+0x22,2,1000,false)
        end
        if fweapon.tweapon.long_target_range[0] then
            memory.setfloat(pWeaponInfo+0x04,1000.0)
        end
        if fweapon.tweapon.long_weapon_range[0] then
            memory.setfloat(pWeaponInfo+0x08,1000.0)
        end
        if fweapon.tweapon.max_accuracy[0] then
            memory.setfloat(pWeaponInfo+0x38,1.0)
        end
        if fweapon.tweapon.max_ammo_clip[0] then
            writeMemory(pWeaponInfo+0x20,2,9999,false)
        end
        if fweapon.tweapon.max_move_speed[0] then
            memory.setfloat(pWeaponInfo+0x3C,1.0)
        end
        if fweapon.tweapon.max_skills[0] then
            callFunction(0x4399D0,1,1)
        end
        --------------------------------------------------

        if fanimation.tanimation.ped[0] == true or fweapon.tweapon.ped[0] == true then
            bool, ped = getCharPlayerIsTargeting(PLAYER_HANDLE)
            if bool == true then
                fped.tped.selected = ped
            end
        end


        -- Quick screenshot
        fcommon.OnHotKeyPress(tcheatmenu.hot_keys.quick_screenshot,function()
            if fgame.tgame.ss_shortcut[0] then
                takePhoto(true)
                printHelpString("Screenshot ~g~taken")
            end
        end)

        -- Qucik teleport
        fcommon.OnHotKeyPress(tcheatmenu.hot_keys.quick_teleport,function()
            if fteleport.tteleport.shortcut[0] then
                fteleport.Teleport()
            end
        end)
        
        -- God mode
        setCharProofs(PLAYER_PED,fplayer.tplayer.god[0],fplayer.tplayer.god[0],fplayer.tplayer.god[0],fplayer.tplayer.god[0],fplayer.tplayer.god[0])

        -- Aim skin changer
        fcommon.OnHotKeyPress(tcheatmenu.hot_keys.asc_key,function()
            if fplayer.tplayer.aimSkinChanger[0] then
                local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
                if bool == true then
                    local model = getCharModel(char)
                    fplayer.ChangePlayerModel(model)
                end
            end
        end)

         -- Vehicle functions
        if isCharInAnyCar(PLAYER_PED) then
            local car = getCarCharIsUsing(PLAYER_PED)

            setCarEngineOn(car,not (fvehicle.tvehicle.disable_car_engine[0]))

            -- Reset car colors if player changed color in tune shop
            if fvehicle.tvehicle.color.default ~= -1 then
                local color_id = getCarColours(car)
                if fvehicle.tvehicle.color.default ~= color_id then
                    fvehicle.ForEachCarComponent(function(mat)
                        mat:reset_color()
                    end)
                end
            end

            if fvehicle.tvehicle.lock_speed[0] then
                if fvehicle.tvehicle.speed[0] > 500 then
                    fvehicle.tvehicle.speed[0] = 500
                end
                setCarForwardSpeed(car,fvehicle.tvehicle.speed[0])
            end

            if getCarDoorLockStatus(car) == 4 then
                fvehicle.tvehicle.lock_doors[0] = true
            else
                fvehicle.tvehicle.lock_doors[0] = false
            end

            setCarVisible(car,not(fvehicle.tvehicle.invisible_car[0]))
            setCarWatertight(car,fvehicle.tvehicle.watertight_car[0])
            setCarCanBeDamaged(car,not(fvehicle.tvehicle.no_damage[0]))
            setCarCanBeVisiblyDamaged(car,not(fvehicle.tvehicle.visual_damage[0]))
            setCharCanBeKnockedOffBike(PLAYER_PED,fvehicle.tvehicle.stay_on_bike[0])
            setCarHeavy(car,fvehicle.tvehicle.heavy[0])

        else
            fvehicle.tvehicle.lock_doors[0] = false
            fvehicle.tvehicle.lights[0] = false
        end

        --------------------------------------------------

        -- Menu open close
        fcommon.OnHotKeyPress(tcheatmenu.hot_keys.menu_open,function()
            tcheatmenu.window.show[0] = not tcheatmenu.window.show[0]
        end)
        fcommon.OnHotKeyPress(tcheatmenu.hot_keys.command_window,function()
            fmenu.tmenu.command.show[0] = not fmenu.tmenu.command.show[0]
        end)

        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() then

        fconfig.Write()
        if fconfig.tconfig.reset == false then
            if fmenu.tmenu.crash_text == "" then
                fmenu.tmenu.crash_text = "Cheat menu crashed unexpectedly"

                if fmenu.tmenu.auto_reload[0] and not fgame.tgame.script_manager.skip_auto_reload then
                    for index, script in ipairs(script.list()) do
                        if script.name ~= thisScript().name then
                            script.this:reload()
                        end
                    end
                    fmenu.tmenu.crash_text =  fmenu.tmenu.crash_text .. " but reloaded"
                end
            end
        end

        if fgame.tgame.camera.bool[0] then
            x,y,z = getCharCoordinates(PLAYER_PED)
            z = getGroundZFor3dCoord(x,y,z+fgame.tgame.camera.z_offset)
            
            setCharCoordinates(PLAYER_PED,x,y,z)

            freezeCharPositionAndDontLoadCollision(PLAYER_PED,false)
            setCharCollision(PLAYER_PED,true)
            setLoadCollisionForCharFlag(PLAYER_PED,true)
            setEveryoneIgnorePlayer(0,false)

            writeMemory(0xBA676C,1,0,false) -- Radar
            writeMemory(0xBA6769,1,1,false) -- Hud

            restoreCameraJumpcut()
            writeMemory((getCharPointer(PLAYER_PED)+1140),4,fgame.tgame.camera.model_val,false)
        end
        
        if fmenu.tmenu.show_crash_message[0] and not fgame.tgame.script_manager.skip_auto_reload then
            printHelpString(fmenu.tmenu.crash_text)
        end

        fmenu.tmenu.crash_text = ""

        --Releasing Images
        fcommon.ReleaseImages(fvehicle.tvehicle.images)
        fcommon.ReleaseImages(fweapon.tweapon.images)
        fcommon.ReleaseImages(fvehicle.tvehicle.paintjobs.images)
        fcommon.ReleaseImages(fvehicle.tvehicle.components.images)
        fcommon.ReleaseImages(fped.tped.images)
        fcommon.ReleaseImages(fplayer.tplayer.clothes.images)
    end
end
