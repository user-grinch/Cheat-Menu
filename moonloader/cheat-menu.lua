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

script_name('Cheat Menu')
script_author("Grinch_")
script_description("Cheat Menu for Grand Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-moon-cheat-menu")
script_dependencies("ffi","lfs","memory","mimgui","MoonAdditions")
script_properties('work-in-pause')
script_version("2.2-beta")
script_version_number(2020102601) -- YYYYMMDDNN

print(string.format("Loading v%s (%d)",script.this.version,script.this.version_num))

resX, resY = getScreenResolution()

tcheatmenu =
{   
    dir                 = string.format( "%s%s",getWorkingDirectory(),"/lib/cheat-menu/"),
    fail_loading_json   = false,
}

--------------------------------------------------
-- Libraries
casts         = require 'cheat-menu.libraries.casts'
ffi           = require 'ffi'
glob          = require 'game.globals'
imgui         = require 'mimgui'
lfs           = require 'lfs'
mad           = require 'MoonAdditions'
memory        = require 'cheat-menu.libraries.memory'
os            = require 'os'
vkeys         = require 'vkeys'
ziplib        = ffi.load(string.format( "%s/lib/ziplib.dll",getWorkingDirectory()))

--------------------------------------------------
-- Menu modules
fcommon       = require 'cheat-menu.modules.common'
fconfig       = require 'cheat-menu.modules.config'
fconst        = require 'cheat-menu.modules.const'

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

tcheatmenu   =
{   
    coord    = 
    {
        X    = fconfig.Get('tcheatmenu.coord.X',50),
        Y    = fconfig.Get('tcheatmenu.coord.Y',50),
    },
    dir          = tcheatmenu.dir,
    current_menu = fconfig.Get('tcheatmenu.current_menu',0),
    fail_loading_json = tcheatmenu.fail_loading_json,
    restart_required = false,
    show     = imgui.new.bool(fmenu.tmenu.fast_load_images[0]),
    size     =
    {
        X    = fconfig.Get('tcheatmenu.size.X',resX/4),
        Y    = fconfig.Get('tcheatmenu.size.Y',resY/1.2),
    },
    title    = string.format("%s v%s",script.this.name,script.this.version),
}

imgui.OnInitialize(function() -- Called once
    
    local io = imgui.GetIO()

    -- Load fonts
    if fmenu.tmenu.font.size[0] < 12 then fmenu.tmenu.font.size[0] = 12 end
    local mask = tcheatmenu.dir .. "fonts//*.ttf"

    local handle, name = findFirstFile(mask)
    while handle and name do
        fmenu.tmenu.font.list[name] = io.Fonts:AddFontFromFileTTF(string.format( "%sfonts//%s",tcheatmenu.dir,name), fmenu.tmenu.font.size[0])
        name = findNextFile(handle)
    end

    io.FontDefault = fmenu.tmenu.font.list[fmenu.tmenu.font.selected]
    io.IniFilename = nil
    io.WantSaveIniSettings = false

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
        print("Failed loading styles")
    end

    if fmenu.tmenu.fast_load_images[0] then
        showCursor(false)
        tcheatmenu.show[0] = false
    end

    -- Indexing images
    lua_thread.create(
    function() 
        fcommon.IndexFiles(fvehicle.tvehicle.path,fvehicle.tvehicle.images,"jpg")
        fcommon.IndexFiles(fweapon.tweapon.path,fweapon.tweapon.images,"jpg")
        fcommon.IndexFiles(fped.tped.path,fped.tped.images,"jpg")
        fcommon.IndexFiles(fplayer.tplayer.clothes.path,fplayer.tplayer.clothes.images,"jpg")
        fcommon.IndexFiles(fvehicle.tvehicle.components.path,fvehicle.tvehicle.components.images,"jpg")
        fcommon.IndexFiles(fvehicle.tvehicle.paintjobs.path,fvehicle.tvehicle.paintjobs.images,"png")
    end)

end)

imgui.OnFrame(
function() -- condition
    return tcheatmenu.show[0] and not isGamePaused() 
end,
function(self) -- render frame
    
    self.LockPlayer = fmenu.tmenu.lock_player[0] 
    imgui.SetNextWindowSize(imgui.ImVec2(tcheatmenu.size.X,tcheatmenu.size.Y),imgui.Cond.Once)
    imgui.SetNextWindowPos(imgui.ImVec2(tcheatmenu.coord.X,tcheatmenu.coord.Y),imgui.Cond.Once)

    imgui.PushStyleVarVec2(imgui.StyleVar.WindowMinSize,imgui.ImVec2(250,350))
    imgui.PushStyleVarVec2(imgui.StyleVar.FramePadding,imgui.ImVec2(math.floor(tcheatmenu.size.X/85),math.floor(tcheatmenu.size.Y/200)))

    imgui.Begin(tcheatmenu.title, tcheatmenu.show,imgui.WindowFlags.NoCollapse + imgui.WindowFlags.NoSavedSettings)

    --------------------------------------------------
    -- Warnings 

    if tcheatmenu.fail_loading_json then
        imgui.Button("Failed to load some json files!",imgui.ImVec2(fcommon.GetSize(1)))
        if imgui.Button("Reinstall latest",imgui.ImVec2(fcommon.GetSize(2))) then
            fmenu.DownloadUpdate()
            tcheatmenu.fail_loading_json = false
        end
        imgui.SameLine()
        if imgui.Button("Hide message",imgui.ImVec2(fcommon.GetSize(2))) then
            tcheatmenu.fail_loading_json = false
        end
        imgui.Spacing()
    end

    if tcheatmenu.restart_required then
        imgui.Button("Restart is required to apply some changes",imgui.ImVec2(fcommon.GetSize(1)))
        if imgui.Button("Restart menu",imgui.ImVec2(fcommon.GetSize(2))) then
            fmenu.tmenu.crash_text = "Cheat Menu ~g~reloaded"
			thisScript():reload()
        end
        imgui.SameLine()
        if imgui.Button("Hide message",imgui.ImVec2(fcommon.GetSize(2))) then
            tcheatmenu.restart_required = false
        end
        imgui.Spacing()
    end

    --------------------------------------------------
    -- Updater code
    if fmenu.tmenu.update_status == fconst.UPDATE_STATUS.NEW_UPDATE then
        imgui.Button("An updated version is available",imgui.ImVec2(fcommon.GetSize(1)))
        if imgui.Button("Download now",imgui.ImVec2(fcommon.GetSize(3))) then
            fmenu.DownloadUpdate()
        end
        imgui.SameLine()
        if imgui.Button("Hide message",imgui.ImVec2(fcommon.GetSize(3))) then
            fmenu.tmenu.update_status =fconst.UPDATE_STATUS.HIDE_MSG
        end
        imgui.SameLine()
        if imgui.Button("View changelog",imgui.ImVec2(fcommon.GetSize(3))) then
            if fmenu.tmenu.get_beta_updates[0] then
                os.execute('explorer "https://github.com/user-grinch/Cheat-Menu/commits/master"')
            else
                os.execute('explorer "https://github.com/user-grinch/Cheat-Menu/releases/tag/' .. fmenu.tmenu.repo_version ..'"')
            end
        end
        imgui.Spacing()
    end

    if fmenu.tmenu.update_status == fconst.UPDATE_STATUS.DOWNLOADING then
        imgui.Button("Downloading update...",imgui.ImVec2(fcommon.GetSize(1)))
        imgui.Spacing()
    end

    if fmenu.tmenu.update_status == fconst.UPDATE_STATUS.DOWNLOADED then
        if imgui.Button("Install update. This might take a while.",imgui.ImVec2(fcommon.GetSize(1))) then
           fmenu.InstallUpdate()
        end
        imgui.Spacing()
    end
    --------------------------------------------------
    -- Main code
    fcommon.CreateMenus({"Teleport","Memory","Player","Ped","Animation","Vehicle","Weapon","Mission","Stat","Game","Visual","Menu"},
    {fteleport.TeleportMain,fmemory.MemoryMain,fplayer.PlayerMain,fped.PedMain,fanimation.AnimationMain,fvehicle.VehicleMain,
    fweapon.WeaponMain,fmission.MissionMain,fstat.StatMain,fgame.GameMain,fvisual.VisualMain,fmenu.MenuMain})

    --------------------------------------------------
    -- Welcome page
    if tcheatmenu.current_menu == 0 then

        if imgui.BeginChild("Welcome") then
            imgui.Dummy(imgui.ImVec2(0,10))
            imgui.TextWrapped("Welcome to " .. tcheatmenu.title .. " by Grinch_")
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
            fcommon.HotKey("Cheat Menu open/close hotkey",fmenu.tmenu.hot_keys.menu_open)

            imgui.Dummy(imgui.ImVec2(0,10))

            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("Auto reload",fmenu.tmenu.auto_reload,"Reload cheat menu automatically\nin case of a crash.\n\nMight cause crash loop sometimes.")
            fcommon.CheckBoxVar("Check for updates",fmenu.tmenu.auto_update_check,"Cheat Menu will automatically check for updates\nonline. This requires an internet connection and\
will download files from github repository.")
            imgui.NextColumn()
            fcommon.CheckBoxVar("Fast load images",fmenu.tmenu.fast_load_images,"Loads images at the menu startup. Enabling this may\ndecrease fps loss when opening the image tabs but\
can freeze the game at startup for a few seconds.\n\nBest to enable if you won't reload the menu frequently.")
            fcommon.CheckBoxVar("Show tooltips",fmenu.tmenu.show_tooltips,"Shows usage tips beside options.")
            imgui.Columns(1)
            imgui.Spacing()
            imgui.TextWrapped("You can configure everything here anytime from the 'Menu' section.")
            imgui.Spacing()
            imgui.TextWrapped("This modification is licensed under the terms of GPLv3. For more details see <http://www.gnu.org/licenses/>")
            imgui.EndChild()
        end
    end
    
    --------------------------------------------------
    -- Update the menu size & positon variables so they can be saved later
    tcheatmenu.size.X  = imgui.GetWindowWidth()
    tcheatmenu.size.Y  = imgui.GetWindowHeight()
    tcheatmenu.coord.X = imgui.GetWindowPos().x 
    tcheatmenu.coord.Y = imgui.GetWindowPos().y

    --------------------------------------------------
    imgui.End()
    imgui.PopStyleVar(2)
end)

--------------------------------------------------
-- Overlay window
imgui.OnFrame(function() 

    -- return true if any overlay element needs to be displayed
    return not isGamePaused() and fmenu.tmenu.overlay.show[0] and (fmenu.tmenu.overlay.fps[0] or fmenu.tmenu.overlay.coordinates[0] or fmenu.tmenu.overlay.location[0]
    or ((fmenu.tmenu.overlay.speed[0] or fmenu.tmenu.overlay.health[0]) and isCharInAnyCar(PLAYER_PED)))
end,
function()
    local io = imgui.GetIO()
    local overlay = fmenu.tmenu.overlay
    local pos = overlay.position_index[0]

    if pos > 0 then
        x = (pos == 1 or pos == 3) and 10 or io.DisplaySize.x - 10
        y = (pos == 1 or pos == 2) and 10 or io.DisplaySize.y - 10
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
    
    if fmenu.tmenu.overlay.transparent_bg[0] then 
        imgui.PushStyleColor(imgui.Col.WindowBg,imgui.ImVec4(0,0,0,0))
    end

    imgui.Begin("Overlay", nil, flags)
    
    if fmenu.tmenu.overlay.fps[0] then
        imgui.Text("Frames: " .. tostring(math.floor(imgui.GetIO().Framerate)))
    end
    if isCharInAnyCar(PLAYER_PED) then
        local hveh = getCarCharIsUsing(PLAYER_PED)
        if  fmenu.tmenu.overlay.speed[0] then
            imgui.Text(string.format("Speed: %d %d/%d",math.floor(getCarSpeed(hveh)),getCarCurrentGear(hveh)
            ,getCarNumberOfGears(hveh)))
        end

        if fmenu.tmenu.overlay.health[0] then
            imgui.Text(string.format("Health: %.0f%%",getCarHealth(hveh)/10))
        end
    end

    if fmenu.tmenu.overlay.location[0] then
        imgui.Text("Location: " .. fcommon.GetLocationInfo(getCharCoordinates(PLAYER_PED)))
    end

    if fmenu.tmenu.overlay.coordinates[0] then
        local x,y,z = getCharCoordinates(PLAYER_PED)
        imgui.Text(string.format("Coordinates: %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
    end

    if fmenu.tmenu.overlay.transparent_bg[0] then 
        imgui.PopStyleColor()
    end

    imgui.PopStyleVar(2)

    --------------------------------------------------
    -- Overlay right click context menu
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
        imgui.Separator()
        if imgui.MenuItemBool("No background",nil,fmenu.tmenu.overlay.transparent_bg[0]) then 
            fmenu.tmenu.overlay.transparent_bg[0] = not fmenu.tmenu.overlay.transparent_bg[0]
        end
        if imgui.MenuItemBool("Close") then
            fmenu.tmenu.overlay.fps[0] = false
            fmenu.tmenu.overlay.speed[0] = false
            fmenu.tmenu.overlay.health[0] = false
            fmenu.tmenu.overlay.coordinates[0] = false
            fmenu.tmenu.overlay.location[0] = false
        end
        imgui.EndPopup()        
    end

    --------------------------------------------------
    -- Update overlay position variables
    if pos == 0 then
        fmenu.tmenu.overlay.pos_x[0] = imgui.GetWindowPos().x
        fmenu.tmenu.overlay.pos_y[0] = imgui.GetWindowPos().y
    end
    --------------------------------------------------

    imgui.End()

end).HideCursor = true

--------------------------------------------------
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
    if imgui.InputTextWithHint("##TEXTFIELD","Enter command",fmenu.tmenu.command.input_field,ffi.sizeof(fmenu.tmenu.command.input_field),imgui.InputTextFlags.EnterReturnsTrue 
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
    -- Functions that need to lunch only once at startup
    
    --writeMemory(0x4b331f,6,0xE9,false)
    if isSampLoaded() then
        fgame.tgame.script_manager.skip_auto_reload = true
        print("SAMP detected, unloading script.")
        thisScript():unload()
    end

    if tcheatmenu.current_menu == 0 and not string.find(script.this.version,"beta") then -- first startup
        fmenu.tmenu.get_beta_updates[0] = false
    end

    math.randomseed(getGameTimer())

    if fmenu.tmenu.auto_update_check[0] then
        fmenu.CheckUpdates()
    end

    fplayer.CustomSkinsSetup()

    if fplayer.tplayer.enable_saving[0] then
        if fplayer.tplayer.save_data["player_skin"] ~= nil then
            fplayer.ChangePlayerModel(tostring(fplayer.tplayer.save_data["player_skin"]),true)
        else
            for i=0, 18 do
                local name = fplayer.tplayer.save_data["clothes"][tostring(i)] 
                if name ~= nil then
                    if name == "none" then
                        givePlayerClothes(PLAYER_HANDLE,0,0,i) 
                        buildPlayerModel(PLAYER_HANDLE)
                    else
                        fplayer.ChangePlayerCloth(name,true)
                    end
                end
            end
        end
    end

    if fweapon.tweapon.gang.enable_weapon_editor[0] then
        local weapons = fweapon.tweapon.gang.used_weapons
        for x=1,10,1 do          
            setGangWeapons(x-1,weapons[x][1],weapons[x][2],weapons[x][3])
        end
    end

    fplayer.SetPlayerInvisible(fplayer.tplayer.invisible[0])
    
    if fgame.tgame.freeze_mission_timer[0] then
        freezeOnscreenTimer(true)
    end

    if fvehicle.tvehicle.no_vehicles[0] then
        writeMemory(0x434237,1,0x73,false) 
        writeMemory(0x434224,1,0,false)
        writeMemory(0x484D19,1,0x83,false) 
        writeMemory(0x484D17,1,0,false)
    end

    if fgame.tgame.disable_help_popups[0] == true then
        setGameGlobal(glob.Help_Wasted_Shown,1)
        setGameGlobal(glob.Help_Busted_Shown,1)
        removePickup(glob.Pickup_Info_Hospital)
        removePickup(glob.Pickup_Info_Hospital_2)
        removePickup(glob.Pickup_Info_Police)
    end

    setGameGlobal(glob.STAT_Unlocked_Cities_Number,4)

    if fgame.tgame.disable_cheats[0] == true then
        writeMemory(0x4384D0,1,0xE9,false)
        writeMemory(0x4384D1,4,0xD0,false)
        writeMemory(0x4384D5,4,0x90909090,false)
    end

    if not fgame.tgame.forbidden_area_wanted_level[0] then
        writeMemory(0x441770,1,0xC3,false)
    end

    switchArrestPenalties(not(fgame.tgame.keep_stuff[0]))
    switchDeathPenalties(not(fgame.tgame.keep_stuff[0]))

    if fped.tped.gang.wars[0] then
        setGangWarsActive(fped.tped.gang.wars[0])
    end
    
    setPlayerFastReload(PLAYER_HANDLE,fweapon.tweapon.fast_reload[0])

    if fgame.tgame.disable_replay[0] then
        writeMemory(0x460500,4,0xC3,false)
    end

    -- Vehicle gxt names
    for gxt_name,veh_name in pairs(fvehicle.tvehicle.gxt_name_table) do
        setGxtEntry(gxt_name,veh_name)
    end

    -- Command window
    fmenu.RegisterAllCommands()

    -- Set saved values of addresses
    fconfig.SetConfigData()

    if fvisual.tvisual.disable_motion_blur[0] then
        writeMemory(0x7030A0,4,0xC3,false)
    end

    if not fvisual.tvisual.radio_channel_names[0] then
       writeMemory(0x507035,4,0x90,false)
    end

    fvehicle.ParseCarcols()
    fvehicle.ParseVehiclesIDE()

    fcommon.CreateThread(fplayer.KeepPosition)
    fcommon.CreateThread(fplayer.RegenerateHealth)
    fcommon.CreateThread(fped.PedHealthDisplay)
    fcommon.CreateThread(fgame.FreezeTime)
    fcommon.CreateThread(fgame.LoadScriptsOnKeyPress)
    fcommon.CreateThread(fgame.RandomCheatsActivate)
    fcommon.CreateThread(fgame.RandomCheatsDeactivate)
    fcommon.CreateThread(fgame.SolidWater)
    fcommon.CreateThread(fgame.SyncSystemTime)
    fcommon.CreateThread(fvehicle.OnEnterVehicle)
    fcommon.CreateThread(fvehicle.RainbowColors)
    fcommon.CreateThread(fvehicle.RainbowNeons)
    fcommon.CreateThread(fvehicle.TrafficNeons)
    fcommon.CreateThread(fvisual.LockWeather)
    fcommon.CreateThread(fweapon.AutoAim)

    ------------------------------------------------

    while true do

        --------------------------------------------------
        -- Functions that neeed to run constantly

        --------------------------------------------------
        -- Weapons
        local CurWeapon = getCurrentCharWeapon(PLAYER_PED)

        local pPed = getCharPointer(PLAYER_PED)

        local skill = callMethod(0x5E3B60,pPed,1,0,CurWeapon)
        local pWeaponInfo = callFunction(0x743C60,2,2,CurWeapon,skill)

        if fweapon.tweapon.huge_damage[0] then
            writeMemory(pWeaponInfo+0x22,2,1000,false)
        end
        if fweapon.tweapon.long_range[0] then
            memory.setfloat(pWeaponInfo+0x04,1000.0)
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
        --------------------------------------------------

        if fanimation.tanimation.ped[0] == true or fweapon.tweapon.ped[0] == true then
            local bool, ped = getCharPlayerIsTargeting(PLAYER_HANDLE)
            if bool == true then
                fped.tped.selected = ped
            end
        end

        if fped.tped.selected ~= nil then
            if not doesCharExist(fped.tped.selected) or isCharDead(fped.tped.selected) then
                fped.tped.selected = nil
            end
        end

        -- Camera mode
        fcommon.OnHotKeyPress(fmenu.tmenu.hot_keys.camera_mode,function()
            fgame.tgame.camera.bool[0] = not fgame.tgame.camera.bool[0]
            if fgame.tgame.camera.bool[0] then
                fcommon.CreateThread(fgame.CameraMode,"CameraMode")
                printHelpString("Camera mode enabled")
            else
                printHelpString("Camera mode disabled")
            end
        end)

        -- Quick screenshot
        if fgame.tgame.ss_shortcut[0] then
            fcommon.OnHotKeyPress(fmenu.tmenu.hot_keys.quick_screenshot,function()
                takePhoto(true)
                printHelpString("Screenshot ~g~taken")
            end)
        end

        -- Qucik teleport
        if fteleport.tteleport.shortcut[0] then
            fcommon.OnHotKeyPress(fmenu.tmenu.hot_keys.quick_teleport,function()
                fteleport.Teleport()
            end)
        end
    
        if fplayer.tplayer.god[0] then
            writeMemory(0x96916D,1,1,false)
            setCharProofs(PLAYER_PED,true,true,true,true,true)
        end

        -- Aim skin changer
        if fplayer.tplayer.aimSkinChanger[0] then
            fcommon.OnHotKeyPress(fmenu.tmenu.hot_keys.asc_key,function()
                local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
                if bool == true then
                    local model = getCharModel(char)
                    fplayer.ChangePlayerModel(tostring(model))
                end
            end)
        end

        --  Vehicle functions
        if isCharInAnyCar(PLAYER_PED) then
            local car = getCarCharIsUsing(PLAYER_PED)

            if fvehicle.tvehicle.car_engine[0] == fconst.CHECKBOX_STATE.ON then
                setCarEngineOn(car,true)
            end
            if fvehicle.tvehicle.car_engine[0] == fconst.CHECKBOX_STATE.OFF then
                setCarEngineOn(car,false)
            end

            -- Reset car colors if player changed color in tune shop
            if fvehicle.tvehicle.color.default ~= -1 then
                if fvehicle.tvehicle.color.default ~= getCarColours(car) then
                    fvehicle.ForEachCarComponent(function(mat,comp,car)
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

        ------------------------------------------------
        -- Menu open close
        fcommon.OnHotKeyPress(fmenu.tmenu.hot_keys.menu_open,function()
            tcheatmenu.show[0] = not tcheatmenu.show[0]
        end)

        fcommon.OnHotKeyPress(fmenu.tmenu.hot_keys.command_window,function()
            fmenu.tmenu.command.show[0] = not fmenu.tmenu.command.show[0]
        end)

        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() then

        fconfig.Write()
        fcommon.SaveJson("animation",fanimation.tanimation.list)
        fteleport.tteleport.coordinates["Radar"] = nil
        fcommon.SaveJson("coordinate",fteleport.tteleport.coordinates)
        fcommon.SaveJson("memory",fmemory.tmemory.list)

        if isCharInAnyCar(PLAYER_PED) then
            local model = getCarModel(getCarCharIsUsing(PLAYER_PED))
            fvehicle.tvehicle.first_person_camera.offsets[tostring(model)].x = fvehicle.tvehicle.first_person_camera.offset_x_var[0]
            fvehicle.tvehicle.first_person_camera.offsets[tostring(model)].y = fvehicle.tvehicle.first_person_camera.offset_y_var[0] 
            fvehicle.tvehicle.first_person_camera.offsets[tostring(model)].z = fvehicle.tvehicle.first_person_camera.offset_z_var[0] 
            fcommon.SaveJson("first person camera offsets",fvehicle.tvehicle.first_person_camera.offsets)
        end

        if fgame.tgame.camera.bool[0] then
            displayRadar(true)
            displayHud(true)
        end

        restoreCameraJumpcut()

        if doesObjectExist(fgame.tgame.solid_water_object) then
            deleteObject(fgame.tgame.solid_water_object)
        end

        fgame.RemoveAllObjects()
        fped.RemoveAllSpawnedPeds()   
        fweapon.RemoveAllWeaponDrops()
         
        fcommon.ReleaseImages(fvehicle.tvehicle.images)
        fcommon.ReleaseImages(fweapon.tweapon.images)
        fcommon.ReleaseImages(fvehicle.tvehicle.paintjobs.images)
        fcommon.ReleaseImages(fvehicle.tvehicle.components.images)
        fcommon.ReleaseImages(fped.tped.images)
        fcommon.ReleaseImages(fplayer.tplayer.clothes.images)

        if fconfig.tconfig.reset == false then
            if fmenu.tmenu.crash_text == "" then
                fmenu.tmenu.crash_text = "Cheat menu crashed unexpectedly"

                if fmenu.tmenu.auto_reload[0] and not fgame.tgame.script_manager.skip_auto_reload then
                    script:reload()
                    fmenu.tmenu.crash_text =  fmenu.tmenu.crash_text .. " but reloaded"
                end
            end
        end
        if fmenu.tmenu.show_crash_message[0] and not fgame.tgame.script_manager.skip_auto_reload then
            printHelpString(fmenu.tmenu.crash_text)
        end
    end
end

function onSaveGame()
    fgame.RemoveAllObjects()
    fped.RemoveAllSpawnedPeds()   
    fweapon.RemoveAllWeaponDrops()  
end