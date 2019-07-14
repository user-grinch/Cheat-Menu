-- This module contains all the functions for weapons tab

local module = {}

local tweapons =
{
    images = {},
    path = getGameDirectory() .. "\\moonloader\\lib\\cheat-menu\\weapons\\",
    quick_spawn = imgui.new.bool(false),
    noreload = imgui.new.bool(false),
    fast_reload = imgui.new.bool(false),
}

module.tweapons = tweapons

local flibweapons = require 'lib.game.weapons'

function module.CBaseWeaponInfo(name)
    return callFunction(0x743D10,1,1,name)
end

function module.GiveWeaponToPlayer(weapon)
    model = getWeapontypeModel(weapon)
    if isModelAvailable(model) then
        requestModel(model)
        loadAllModelsNow()
        giveWeaponToChar(PLAYER_PED,weapon,99999)
        markModelAsNoLongerNeeded(model)
        fcommon.CheatActivated()
    end
end

function module.WeaponsMain()
    imgui.Spacing()
    imgui.Text("Weapons")
    imgui.Separator()
    imgui.Spacing()

    if imgui.Button("Remove all weapons",imgui.ImVec2(fcommon.GetSize(2))) then
        removeAllCharWeapons(PLAYER_PED)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Remove current weapon",imgui.ImVec2(fcommon.GetSize(2))) then
        removeWeaponFromChar(PLAYER_PED,getCurrentCharWeapon(PLAYER_PED))
        fcommon.CheatActivated()
    end

    imgui.Spacing()

    if imgui.Button("Weapon Set 1",imgui.ImVec2(fcommon.GetSize(3),25)) then
        callFunction(0x4385B0,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Weapon Set 2",imgui.ImVec2(fcommon.GetSize(3),25)) then
        callFunction(0x438890,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Weapon Set 3",imgui.ImVec2(fcommon.GetSize(3),25)) then
        callFunction(0x438B30,1,1,false)
        fcommon.CheatActivated()
    end

    imgui.Spacing()

    if imgui.BeginTabBar("Weapons") then
        if imgui.BeginTabItem("Spawn") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ name = "Quick weapon",var = tweapons.quick_spawn,help_text = "Weapon can be spawned from quick spawner using (Left Ctrl + Q). \n\nControls:\nEnter = Stop reading key press\nDelete = Erase full string\nBackspace = Erase last character"})
            fcommon.CheckBox({ name = "No reload + Inf. Ammo",var = tweapons.noreload,func = function()
                if tweapons.noreload[0] then
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
            end})
            imgui.NextColumn()
            fcommon.CheckBox({ name = "Fast Reload",var = tweapons.fast_reload,func = function()
                if tweapons.fast_reload[0] then
                    setPlayerFastReload(PLAUER_HANDLE,true)
                    fcommon.CheatActivated()
                else
                    setPlayerFastReload(PLAUER_HANDLE,false)
                    fcommon.CheatDeactivated()
                end
            end})
            imgui.Columns(1)


            imgui.Spacing()

            imgui.Spacing()
            imgui.Text("Weapon list")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Weapon list Window") then
                fcommon.ShowEntries("Melee",{1,2,3,4,5,6,7,8,9},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Handguns",{22,23,24},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Shotguns",{25,26,27},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Sub-Machine Guns",{28,29,32},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Assault Rifles",{30,31},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Rifles",{33,34},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Heavy Weapons",{35,36,37,38},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("Projectiles",{16,17,18,39},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries("More",{41,42,43,44,45,46,14,15,10,11,12},5,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end
return module
