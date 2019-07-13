script_name 'mimgui Cheat Menu'
script_author("Grinch_")
script_description("Cheat Menu for Grand Theft Auto San Andreas")
script_url("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
script_dependencies("imgui","memory","MoonAdditions")
version = {
    release = "Prerelease",
    number  = 1.5,
}
script_version(version.release)
script_version_number(version.number)


-- All the command keys used throughout the Cheat-Menu
keys =
{
    control_key       = 0xA2, -- LCONTROL
    menu_open_key     = 0x4E, -- N --0x4D, -- M
    screenshot_key    = 0x53, -- S
    quickspawner_key  = 0x51, -- Q
    coords_copy       = 0x43, -- C
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
}

resX,resY = getScreenResolution()

-- Script Dependencies
ffi           = require "ffi"
imgui         = require 'mimgui'
memory        = require 'memory'
glob          = require 'game.globals'
mad           = require 'MoonAdditions'

-- Loading custom modules
fabout        = require 'mcheat-menu.modules.about'
fcheats       = require 'mcheat-menu.modules.cheats'
fcommon       = require 'mcheat-menu.modules.common'
fmemcontrol   = require 'mcheat-menu.modules.memory_control'
fpeds         = require 'mcheat-menu.modules.peds'
fplayer       = require 'mcheat-menu.modules.player'
fteleport     = require 'mcheat-menu.modules.teleportation'
fvehicles     = require 'mcheat-menu.modules.vehicles'


cheatMenu =
{
    window =
    {
        size =
        {
            X = resX/4,
            Y = resY/1.2,
        },
        main    = imgui.new.bool(false),

    },
    menubuttons =
    {
        current = -1,
    },
}

imgui.OnFrame(

function() -- condition
    return cheatMenu.window.main[0]
end,
function() -- render frame
    imgui.SetNextWindowSize(imgui.ImVec2(cheatMenu.window.size.X,cheatMenu.window.size.Y), imgui.Cond.Once)

    -- Styles
    imgui.PushStyleVarFloat(imgui.StyleVar.WindowBorderSize,0)
    imgui.PushStyleVarFloat(imgui.StyleVar.FramePadding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.PopupRounding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.PopupBorderSize,0)
    imgui.PushStyleVarFloat(imgui.StyleVar.ChildBorderSize,0)
    imgui.PushStyleVarFloat(imgui.StyleVar.WindowRounding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.ScrollbarRounding,3)
    imgui.PushStyleVarFloat(imgui.StyleVar.TabRounding,3)
    imgui.PushStyleColor(imgui.Col.Header, imgui.ImVec4(0,0,0,0))

    local header = string.format("Cheat Menu v%.2f by Grinch_",version.number)
    for i=1,(imgui.GetWindowWidth() - #header)/16,1 do
        header = ' ' .. header
    end

    if imgui.Begin(tostring(header), cheatMenu.window.main,imgui.WindowFlags.NoCollapse) then
        fcommon.UiCreateButtons({"Teleport","Memory","Player","Vehicle","Weapon","Peds","Missions","Cheats","Game","Visuals","Menu","About"},{fteleport.TeleportationMain,fmemcontrol.MemoryControlMain,fplayer.PlayerMain,fvehicles.VehiclesMain})
        imgui.End()
    end
end)

function main()
    while true do

        if isKeyDown(keys.control_key) and isKeyDown(keys.menu_open_key) then
            fcommon.KeyWait(keys.control_key,keys.menu_open_key)
            cheatMenu.window.main[0] = not cheatMenu.window.main[0]
        end

        if fteleport.tteleport.shotcut[0] == true
        and isKeyDown(keys.teleport_key1)
        and isKeyDown(keys.teleport_key2) then
            fcommon.KeyWait(keys.teleport_key1,keys.teleport_key2)
            fteleport.Teleport()
        end

        if fplayer.tplayer.god[0] then
            setCharProofs(PLAYER_PED,true,true,true,true,true)
        else
            setCharProofs(PLAYER_PED,false,false,false,false,false)
        end

        if fplayer.tplayer.aimSkinChanger[0] and isKeyDown(keys.asc_key) then
            fcommon.KeyWait(keys.asc_key,keys.asc_key)
            local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
            if bool == true then
                local model = getCharModel(char)
                fplayer.ChangePlayerModel(model)
            end
        end

        if isKeyDown(keys.control_key) and isKeyDown(keys.quickspawner_key) then
            if (fvehicles.tvehicles.quick_spawn[0]) then --or fweapons.tweapons.quick_spawn[0]) then
                fcommon.QuickSpawner()
            end
        end

        -- Vehicle related stuff which is required to run every frame
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)

            --[[if fvehicles.tvehicles.full_health.v then
                setCarHealth(car,1000)
            end

            if fvehicles.tvehicles.stay_on_bike.v then
                setCharCanBeKnockedOffBike(PLAYER_PED,true)
            else
                setCharCanBeKnockedOffBike(PLAYER_PED,false)
            end-]]

            if fvehicles.tvehicles.lock_speed[0] then
                if fvehicles.tvehicles.speed[0] > 500 then
                    fvehicles.tvehicles.speed[0] = 500
                end
                setCarForwardSpeed(car,fvehicles.tvehicles.speed[0])
            end
            --[[
            if fvehicles.tvehicles.heavy.v then
                setCarHeavy(car,true)
            else
                setCarHeavy(car,false)
            end

            if fvehicles.tvehicles.visual_damage.v then
                setCarCanBeVisiblyDamaged(car,false)
            else
                setCarCanBeVisiblyDamaged(car,true)
            end]]--
        end

        wait(0)
    end
end


function onScriptTerminate(script, quitGame)
    if script == thisScript() then
        showCursor(false,false)
        printHelpString("Cheat Menu ~r~Crashed.~w~Please provide moonloader.log in case of debugging.")
    end
end
