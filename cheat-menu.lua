script_name 'Cheat Menu'
script_author("Grinch_")
script_description("Cheat Menu for Grand Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
script_dependencies("imgui","memory","MoonAdditions")
script_version("1.6")
script_version_number(01082019) -- DDMMYYYY

if getMoonloaderVersion() >= 21 then
    script_properties('work-in-pause')
else
    print("Moonloader version doesn't meet requirement(v21).")
end

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
    airbreak_forward  = 0x57, -- W - moves player forward in airbreak mode
    airbreak_backward = 0x53, -- S - moves player backward in airbreak mode
    airbreak_left     = 0x41, -- A - rotates player left in airbreak mode
    airbreak_right    = 0x44, -- D - rotates player right in airbreak mode
    airbreak_up       = 0x26, -- Arrow up - moves player up in airbreak mode
    airbreak_down     = 0x28, -- Arrow down - moves player down in airbreak mode
    aircraft_zoom     = 0x56, -- V - Change aircraft zoom level
}

resX,resY = getScreenResolution()


-- Download dependencies if missing
require 'deps'
{
    'fyp:mimgui',
    'fyp:moonadditions',
}


-- Script Dependencies
ffi           = require "ffi"
imgui         = require 'mimgui'
memory        = require 'memory'
glob          = require 'game.globals'
mad           = require 'MoonAdditions'


-- Loading custom modules
fconfig       = require 'cheat-menu.modules.config'
if not pcall(fconfig.Read) then
    printString("~r~Unable~w~ to load config file.",10000)
end

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
        title   = fconfig.get('tcheatMenu.window.title',string.format("%s v%s by %s",script.this.name,script.this.version,script.this.authors[1])),
        overlay =
        {
            main     = imgui.new.bool(true),
            distance = fconfig.get('tcheatMenu.window.overlay.distance',10.0),
            corner   = fconfig.get('tcheatMenu.window.overlay.corner',0),
        },
    },
    menubuttons =
    {
        current =  fconfig.get('tcheatMenu.menubuttons.current',-1),
    },
}

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

            fcommon.UiCreateButtons({"Teleport","Memory","Player","Vehicle","Weapon","Peds","Missions","Cheats","Game","Visuals","Menu","About"},
            {fteleport.TeleportMain,fmemory.MemoryMain,fplayer.PlayerMain,fvehicles.VehiclesMain,fweapons.WeaponsMain,
            fpeds.PedsMain,fmissions.MissionsMain,fcheats.CheatsMain,fgame.GameMain,fvisuals.VisualsMain,fmenu.MenuMain,fabout.AboutMain})
            imgui.End()

        end

        --Overlay window
        if tcheatMenu.window.overlay.main[0] then
            if fgame.tgame.fps.bool[0] or fvisuals.tvisuals.show_coordinates[0] or (( fvehicles.tvehicles.show.speed[0] or fvehicles.tvehicles.show.health[0]) and isCharInAnyCar(PLAYER_PED)) then
                if (tcheatMenu.window.overlay.corner ~= -1) then
                    window_pos       = imgui.ImVec2(ternary((tcheatMenu.window.overlay.corner == 1 or tcheatMenu.window.overlay.corner == 3),resX - tcheatMenu.window.overlay.distance,tcheatMenu.window.overlay.distance),ternary((tcheatMenu.window.overlay.corner == 2 or tcheatMenu.window.overlay.corner == 3),resY - tcheatMenu.window.overlay.distance,tcheatMenu.window.overlay.distance))
                    window_pos_pivot = imgui.ImVec2(ternary((tcheatMenu.window.overlay.corner == 1 or tcheatMenu.window.overlay.corner == 3),1.0,0.0),ternary((tcheatMenu.window.overlay.corner == 2 or tcheatMenu.window.overlay.corner == 3),1.0,0.0))
                    imgui.SetNextWindowPos(window_pos,0,window_pos_pivot)
                end
                imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,0.65)
                if imgui.Begin('Overlay', tcheatMenu.window.overlay.main,imgui.WindowFlags.NoTitleBar + imgui.WindowFlags.NoResize + imgui.WindowFlags.NoCollapse + imgui.WindowFlags.AlwaysAutoResize + imgui.WindowFlags.NoFocusOnAppearing) then
                    if fgame.tgame.fps.bool[0] == true then
                        imgui.Text("Frames :" .. tostring(math.floor(imgui.GetIO().Framerate)))
                    end

                    if isCharInAnyCar(PLAYER_PED) then
                        car = getCarCharIsUsing(PLAYER_PED)
                        if  fvehicles.tvehicles.show.speed[0] == true then
                            speed = getCarSpeed(car)
                            total_gears = getCarNumberOfGears(car)
                            current_gear = getCarCurrentGear(car)
                            imgui.Text(string.format("Speed   :%d %d/%d",math.floor(speed),current_gear,total_gears))
                        end

                        if fvehicles.tvehicles.show.health[0] == true then
                            imgui.Text(string.format("Health  :%.0f%%",getCarHealth(car)/10))
                        end
                    end

                    if fvisuals.tvisuals.show_coordinates[0] == true then
                        x,y,z = getCharCoordinates(PLAYER_PED)
                        imgui.Text(string.format("Coordinates: %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
                    end

                    imgui.PushStyleVarFloat(imgui.StyleVar.Alpha,1.0)
                    if imgui.BeginPopupContextWindow() then
                        imgui.Text("Position")
                        imgui.Separator()
                        if (imgui.MenuItemBool("Custom",nil,tcheatMenu.window.overlay.corner == -1)) then tcheatMenu.window.overlay.corner = -1 end
                        if (imgui.MenuItemBool("Top-Left",nil,tcheatMenu.window.overlay.corner == 0)) then tcheatMenu.window.overlay.corner = 0 end
                        if (imgui.MenuItemBool("Top-Right",nil,tcheatMenu.window.overlay.corner == 1)) then tcheatMenu.window.overlay.corner = 1 end
                        if (imgui.MenuItemBool("Bottom-Left",nil,tcheatMenu.window.overlay.corner == 2)) then tcheatMenu.window.overlay.corner = 2 end
                        if (imgui.MenuItemBool("Bottom-Right",nil,tcheatMenu.window.overlay.corner == 3)) then tcheatMenu.window.overlay.corner = 3 end
                        if imgui.MenuItemBool("Close") then
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

function main()
    
    flanguage.LoadLanguages()
    if fgame.tgame.disable_help_popups[0] == true then
        setGameGlobal(glob.Help_Wasted_Shown,1)
        setGameGlobal(glob.Help_Busted_Shown,1)
        removePickup(glob.Pickup_Info_Hospital)
        removePickup(glob.Pickup_Info_Hospital_2)
        removePickup(glob.Pickup_Info_Police)
    end

    while true do
        if not isGamePaused() then
            if tcheatMenu.window.main[0] then
                showCursor(true)
            else
                showCursor(false)
            end
        else
            showCursor(false)
        end
        if fgame.tgame.ss_shortcut[0]
        and isKeyDown(tkeys.control_key) and isKeyDown(tkeys.screenshot_key) then
            takePhoto(true)
            printHelpString("Screenshot taken ~g~successfully")
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
        if fvehicles.tvehicles.color.default ~= -1 then
            if isCharInAnyCar(PLAYER_PED) then
                local color_id = getCarColours(car)
                if fvehicles.tvehicles.color.default ~= color_id then
                    fvehicles.ForEachCarComponent(function(mat)
                        mat:reset_color()
                    end)
                end
            end
        end



        if isCharInAnyHeli(PLAYER_PED)
        or isCharInAnyPlane(PLAYER_PED) then
            lua_thread.create(fvehicles.AircraftCamera)
        end

        -- Vehicle related stuff which is required to run every frame
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)

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
        end

        wait(0)
    end
end


function onScriptTerminate(script, quitGame)
    if script == thisScript() then
        showCursor(false,false)
        fconfig.write()
        local crash_text = "Cheat Menu ~r~crashed ~w~.Provide moonloader.log in case of debugging."
        if fmenu.tmenu.auto_reload[0] then
            script.this:reload()
            crash_text = "Cheat Menu ~r~crashed ~w~& ~g~reloaded~w~.Provide moonloader.log in case of debugging."
        end
        if fmenu.tmenu.show_crash_message[0] then
            printHelpString(crash_text)
        end
    end
end
