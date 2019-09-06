script_name 'Cheat Menu'
script_author("Grinch_")
script_description("Cheat Menu for Grand Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
script_dependencies("ffi","mimgui","memory","MoonAdditions")
script_properties('work-in-pause')
script_version("1.7")
script_version_number(06092019) -- DDMMYYYY

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
-- encoding      = require 'encoding'

-- encoding.default = 'CP1251'
-- u8 = encoding.UTF8

tcheatMenu = 
{
    dir = string.format( "%s%s",getGameDirectory(),"//moonloader//lib//cheat-menu//")
}

-- Loading custom modules
fconfig       = require 'cheat-menu.modules.config'
if not pcall(fconfig.Read) then
    printString(flanguage.GetText("cheatmenu.ConfigLoadFailure"),10000)
end

ftables       = require 'cheat-menu.modules.tables.$index'
fabout        = require 'cheat-menu.modules.about'
fcheats       = require 'cheat-menu.modules.cheats'
fcommon       = require 'cheat-menu.modules.common'
fgame         = require 'cheat-menu.modules.game'
flanguage     = require 'cheat-menu.modules.language'
fmemory       = require 'cheat-menu.modules.memory'
fmenu         = require 'cheat-menu.modules.menu'
fmissions     = require 'cheat-menu.modules.missions'
fpeds         = require 'cheat-menu.modules.peds'
fplayer       = require 'cheat-menu.modules.player'
fteleport     = require 'cheat-menu.modules.teleport'
fvehicles     = require 'cheat-menu.modules.vehicles'
fvisuals      = require 'cheat-menu.modules.visuals'
fweapons      = require 'cheat-menu.modules.weapons'

if fmenu.tmenu.disable_in_samp[0] and isSampLoaded() then
    script.this:unload()
end

tcheatMenu =
{
    window =
    {
        size =
        {
            X = fconfig.get('tcheatMenu.window.size.X',resX/4),
            Y = fconfig.get('tcheatMenu.window.size.Y',resY/1.2),
        },
        main    = imgui.new.bool(false),
        title   = string.format("%s v%s by %s",script.this.name,script.this.version,script.this.authors[1]),
        overlay =
        {
            main     = imgui.new.bool(true),
            distance = fconfig.get('tcheatMenu.window.overlay.distance',10.0),
            corner   = imgui.new.int(fconfig.get('tcheatMenu.window.overlay.corner',0)),
            names    = {"Custom","TopLeft","TopRight","BottomLeft","BottomRight","Close"},
            list     = {},
        },
    },
    menubuttons =
    {
        current =  fconfig.get('tcheatMenu.menubuttons.current',-1),
    },
    font_path  =  tcheatMenu.dir .. "fonts//",
    cursor = 
    {
        state  = nil,
    }
}

tcheatMenu.window.overlay.list = imgui.new['const char*'][#tcheatMenu.window.overlay.names](tcheatMenu.window.overlay.names)

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
    
    -- local mask = tcheatMenu.font_path .. "*.otf"
    -- local handle,file = findFirstFile(mask)
    -- if handle and file then
    --     local glyph_ranges = imgui.GetIO().Fonts:GetGlyphRangesCyrillic()
    --     imgui.GetIO().Fonts:Clear()
    --     while handle and file do 
    --         imgui.GetIO().Fonts:AddFontFromFileTTF(tcheatMenu.font_path .. file, 14, nil, nil)
    --         print(tcheatMenu.font_path .. file)
    --         file = findNextFile(handle)
    --     end
    --     findClose(handle)
    -- end
end)

imgui.OnFrame(

function() -- condition
    if (tcheatMenu.window.main[0] or tcheatMenu.window.overlay.main[0]) then
        return true
    end
    return false
end,
function() -- render frame
    if not isGamePaused() then
        
        if tcheatMenu.window.main[0] then
           
            imgui.SetNextWindowSize(imgui.ImVec2(tcheatMenu.window.size.X,tcheatMenu.window.size.Y), imgui.Cond.Once)

            imgui.Begin(tcheatMenu.window.title, tcheatMenu.window.main,imgui.WindowFlags.NoCollapse + imgui.WindowFlags.NoSavedSettings)

            fcommon.UiCreateButtons({flanguage.GetText("cheatmenu.Teleport"),flanguage.GetText("cheatmenu.Memory"),
            flanguage.GetText("cheatmenu.Player"),flanguage.GetText("cheatmenu.Vehicle"),flanguage.GetText("cheatmenu.Weapon"),
            flanguage.GetText("cheatmenu.Peds"),flanguage.GetText("cheatmenu.Missions"),flanguage.GetText("cheatmenu.Cheats"),
            flanguage.GetText("cheatmenu.Game"),flanguage.GetText("cheatmenu.Visuals"),flanguage.GetText("cheatmenu.Menu"),
            flanguage.GetText("cheatmenu.About")},{fteleport.TeleportMain,fmemory.MemoryMain,fplayer.PlayerMain,fvehicles.VehiclesMain,
            fweapons.WeaponsMain,fpeds.PedsMain,fmissions.MissionsMain,fcheats.CheatsMain,fgame.GameMain,fvisuals.VisualsMain,fmenu.MenuMain,
            fabout.AboutMain})

            imgui.End()
        end

        --Overlay window
        if tcheatMenu.window.overlay.main[0] then
            if fmenu.tmenu.overlay.fps[0] or fmenu.tmenu.overlay.coordinates[0] or (( fmenu.tmenu.overlay.speed[0] or fmenu.tmenu.overlay.health[0]) and isCharInAnyCar(PLAYER_PED)) then
                if (tcheatMenu.window.overlay.corner[0] ~= 0) then
                    window_pos       = imgui.ImVec2(ternary((tcheatMenu.window.overlay.corner[0] == 2 or tcheatMenu.window.overlay.corner[0] == 4),resX - tcheatMenu.window.overlay.distance,tcheatMenu.window.overlay.distance),ternary((tcheatMenu.window.overlay.corner[0] == 3 or tcheatMenu.window.overlay.corner[0] == 4),resY - tcheatMenu.window.overlay.distance,tcheatMenu.window.overlay.distance))
                    window_pos_pivot = imgui.ImVec2(ternary((tcheatMenu.window.overlay.corner[0] == 2 or tcheatMenu.window.overlay.corner[0] == 4),1.0,0.0),ternary((tcheatMenu.window.overlay.corner[0] == 3 or tcheatMenu.window.overlay.corner[0] == 4),1.0,0.0))
                    imgui.SetNextWindowPos(window_pos,0,window_pos_pivot)
                end
                imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,0.65)
                if imgui.Begin('Overlay', tcheatMenu.window.overlay.main,imgui.WindowFlags.NoTitleBar + imgui.WindowFlags.NoResize + imgui.WindowFlags.NoCollapse + imgui.WindowFlags.AlwaysAutoResize + imgui.WindowFlags.NoFocusOnAppearing) then
                    if fmenu.tmenu.overlay.fps[0] == true then
                        imgui.Text(flanguage.GetText("cheatmenu.FPS") .. " :" .. tostring(math.floor(imgui.GetIO().Framerate)))
                    end

                    if isCharInAnyCar(PLAYER_PED) then
                        car = getCarCharIsUsing(PLAYER_PED)
                        if  fmenu.tmenu.overlay.speed[0] == true then
                            speed = getCarSpeed(car)
                            total_gears = getCarNumberOfGears(car)
                            current_gear = getCarCurrentGear(car)
                            imgui.Text(string.format(flanguage.GetText("cheatmenu.Speed") .. "   :%d %d/%d",math.floor(speed),current_gear,total_gears))
                        end

                        if fmenu.tmenu.overlay.health[0] == true then
                            imgui.Text(string.format(flanguage.GetText("cheatmenu.Health") .. "  :%.0f%%",getCarHealth(car)/10))
                        end
                    end

                    if fmenu.tmenu.overlay.coordinates[0] == true then
                        x,y,z = getCharCoordinates(PLAYER_PED)
                        imgui.Text(string.format(flanguage.GetText("cheatmenu.Coordinates") .. ": %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
                    end
                    
                    imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,1.0)
                    if imgui.BeginPopupContextWindow() then
                        imgui.Text(flanguage.GetText("cheatmenu.Position"))
                        imgui.Separator()
                        if (imgui.MenuItemBool(flanguage.GetText("common.Custom"),nil,tcheatMenu.window.overlay.corner[0] == 0)) then tcheatMenu.window.overlay.corner[0] = 0 end
                        if (imgui.MenuItemBool(flanguage.GetText("cheatmenu.TopLeft"),nil,tcheatMenu.window.overlay.corner[0] == 1)) then tcheatMenu.window.overlay.corner[0] = 1 end
                        if (imgui.MenuItemBool(flanguage.GetText("cheatmenu.TopRight"),nil,tcheatMenu.window.overlay.corner[0] == 2)) then tcheatMenu.window.overlay.corner[0] = 2 end
                        if (imgui.MenuItemBool(flanguage.GetText("cheatmenu.BottomLeft"),nil,tcheatMenu.window.overlay.corner[0] == 3)) then tcheatMenu.window.overlay.corner[0] = 3 end
                        if (imgui.MenuItemBool(flanguage.GetText("cheatmenu.BottomRight"),nil,tcheatMenu.window.overlay.corner[0] == 4)) then tcheatMenu.window.overlay.corner[0] = 4 end
                        if  imgui.MenuItemBool(flanguage.GetText("cheatmenu.Close")) then
                            fgame.tgame.fps.bool[0] = false
                            fvehicles.tvehicles.show.speed[0] = false
                            fvehicles.tvehicles.show.health[0] = false
                            fvisuals.tvisuals.show_coordinates[0] = false
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
            tcheatMenu.cursor.state = not tcheatMenu.cursor.state
            showCursor(tcheatMenu.cursor.state)
            wait(250)
        end
    end
end

function main()
    
    flanguage.LoadLanguages()
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
                showCursor(tcheatMenu.cursor.state)
            else
                showCursor(tcheatMenu.window.main[0])
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
            printHelpString(flanguage.GetText("cheatmenu.ScreenShotSuccessMSG"))
            fcommon.KeyWait(tkeys.control_key,tkeys.screenshot_key)
        end

        if isKeyDown(tkeys.control_key) and isKeyDown(tkeys.menu_open_key) then
            fcommon.KeyWait(tkeys.control_key,tkeys.menu_open_key)
            tcheatMenu.window.main[0] = not tcheatMenu.window.main[0]
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
            printHelpString(flanguage.GetText('cheatmenu.CoordSave'))
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
            if (fvehicles.tvehicles.quick_spawn[0] or fweapons.tweapons.quick_spawn[0]) then
                fcommon.QuickSpawner()
            end
        end

        -- This is to fix car colors not being applied using opcode changecarcolours



        if isCharInAnyHeli(PLAYER_PED)
        or isCharInAnyPlane(PLAYER_PED) then
            lua_thread.create(fvehicles.AircraftCamera)
        end

        -- Vehicle related stuff which is required to run every frame
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)

            if fvehicles.tvehicles.color.default ~= -1 then
                local color_id = getCarColours(car)
                if fvehicles.tvehicles.color.default ~= color_id then
                    fvehicles.ForEachCarComponent(function(mat)
                        mat:reset_color()
                    end)
                end
            end
            if fvehicles.tvehicles.lock_health[0] then
                setCarHealth(car,1000)
            end

            if fvehicles.tvehicles.stay_on_bike[0] then
                setCharCanBeKnockedOffBike(PLAYER_PED,true)
            else
                setCharCanBeKnockedOffBike(PLAYER_PED,false)
            end

            if fvehicles.tvehicles.lock_speed[0] then
                if fvehicles.tvehicles.speed[0] > 500 then
                    fvehicles.tvehicles.speed[0] = 500
                end
                setCarForwardSpeed(car,fvehicles.tvehicles.speed[0])
            end

            if fvehicles.tvehicles.heavy[0] then
                setCarHeavy(car,true)
            else
                setCarHeavy(car,false)
            end

            if fvehicles.tvehicles.visual_damage[0] == true then
                setCarCanBeVisiblyDamaged(car,false)
            else
                setCarCanBeVisiblyDamaged(car,true)
            end

            if getCarDoorLockStatus(car) == 4 then
                fvehicles.tvehicles.lock_doors[0] = true
            else
                fvehicles.tvehicles.lock_doors[0] = false
            end
        else
            fvehicles.tvehicles.lock_doors[0] = false
            fvehicles.tvehicles.lights.all[0] = false
        end

        wait(0)
    end
end

function onScriptTerminate(script, quitGame)
    if script == thisScript() then
        showCursor(false,false)
        fconfig.write()
        local crash_text = flanguage.GetText("cheatmenu.MenuCrashMSG")
        if fmenu.tmenu.auto_reload[0] then
            script.this:reload()
            crash_text = flanguage.GetText("cheatmenu.MenuCrash&RelodedMSG")
        end
        if fmenu.tmenu.show_crash_message[0] then
            printHelpString(crash_text)
        end
    end
end
