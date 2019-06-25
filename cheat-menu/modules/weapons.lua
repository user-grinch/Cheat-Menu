-- This module contains all the functions for weapons tab

local module = {}

local tweapons =
{
    image_size =
    {
        x = resX/21.3,
        y = resY/12,
    },
    list = {},
    path = getGameDirectory() .. "\\moonloader\\cheat-menu\\weapons\\",
    quick_spawn = imgui.ImBool(false),
    noreload = imgui.ImBool(false),
    fast_reload = imgui.ImBool(false),
}

module.tweapons = tweapons

local flibweapons = require 'lib.game.weapons'

-- Creates weapon entry
weapon_entry = function (weapon)
    model = getWeapontypeModel(weapon)
    if imgui.ImageButton(tweapons.list[tostring(model)],imgui.ImVec2(tweapons.image_size.x,tweapons.image_size.y)) then 
        module.give_weapon_to_player(weapon,tweapons.quick_spawn.v)
    end
    if imgui.IsItemHovered() then
        imgui.BeginTooltip() 
        imgui.SetTooltip(flibweapons.get_name(weapon))
        imgui.EndTooltip()
    end
end

function module.CBaseWeaponInfo(name)
    return callFunction(0x743D10,1,1,name)
end
 
function module.give_weapon_to_player(weapon)
    model = getWeapontypeModel(weapon)
    if isModelAvailable(model) then 
        requestModel(model)
        loadAllModelsNow()
        giveWeaponToChar(PLAYER_PED,weapon,99999)
        markModelAsNoLongerNeeded(model)
        fcommon.CheatActivated()
    end
end

function module.weapons_section()
    imgui.Spacing()
    imgui.Text("Weapons")
    imgui.Separator()
    imgui.Spacing()

    if imgui.Button("Remove all weapons",imgui.ImVec2(fcommon.getsize(2),50)) then
        removeAllCharWeapons(PLAYER_PED)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Remove current weapon",imgui.ImVec2(fcommon.getsize(2),50)) then
        removeWeaponFromChar(PLAYER_PED,getCurrentCharWeapon(PLAYER_PED))
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    imgui.Spacing()
    imgui.Text("Spawn Options")
    imgui.Separator()
    imgui.Spacing()
    fcommon.ValueSwitch({ name = "Quick weapon",var = tweapons.quick_spawn,help_text = "Weapon can be spawned from quick spawner using (Left Ctrl + Q). \n\nControls:\nEnter = Stop reading key press\nDelete = Erase full string\nBackspace = Erase last character"}) 
    if imgui.Checkbox("No reload + Infinite Ammo",tweapons.noreload) then 
        if tweapons.noreload.v then
            writeMemory( 7600773,1,144,1)
            writeMemory( 7600815,1,144,1)
            writeMemory( 7600816,2,37008,1)
            writeMemory( 7612591,1,144,1)
            writeMemory( 7612646,1,144,1)
            writeMemory( 7612647,2,37008,1)
            fcommon.CheatActivated()
        else
            writeMemory( 7600773,1,72,1)
            writeMemory( 7600815,1,255,1)
            writeMemory( 7600816,2,3150,1)
            writeMemory( 7612591,1,72,1)
            writeMemory( 7612646,1,255,1)
            writeMemory( 7612647,2,3150,1)
            fcommon.CheatDeactivated()
        end
    end
    if imgui.Checkbox("Fast Reload",tweapons.fast_reload) then 
        if tweapons.fast_reload.v then
            setPlayerFastReload(PLAUER_HANDLE,true)
            fcommon.CheatActivated()
        else
            setPlayerFastReload(PLAUER_HANDLE,false)
            fcommon.CheatDeactivated()
        end
    end
    imgui.Spacing()
    imgui.Text("Weapon Sets")
    imgui.Separator()
    imgui.Spacing()
    if imgui.Button("Weapon Set 1",imgui.ImVec2(fcommon.getsize(3),25)) then
        callFunction(0x4385B0,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Weapon Set 2",imgui.ImVec2(fcommon.getsize(3),25)) then
        callFunction(0x438890,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Weapon Set 3",imgui.ImVec2(fcommon.getsize(3),25)) then
        callFunction(0x438B30,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.Spacing()
    
    imgui.Spacing()
    imgui.Text("Weapon list")
    imgui.Separator()
    imgui.Spacing()
    fcommon.entries("Melee",weapon_entry,{1,2,3,4,5,6,7,8,9})
    fcommon.entries("Handguns",weapon_entry,{22,23,24})
    fcommon.entries("Shotguns",weapon_entry,{25,26,27})
    fcommon.entries("Sub-Machine Guns",weapon_entry,{28,29,32})
    fcommon.entries("Assault Rifles",weapon_entry,{30,31})
    fcommon.entries("Rifles",weapon_entry,{33,34})
    fcommon.entries("Heavy Weapons",weapon_entry,{35,36,37,38})
    fcommon.entries("Projectiles",weapon_entry,{16,17,18,39})
    fcommon.entries("More",weapon_entry,{41,42,43,44,45,46,14,15,10,11,12})
end
return module