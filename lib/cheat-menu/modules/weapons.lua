-- This module contains all the functions for weapons tab

local module = {}

local tweapons =
{
    images = {},
    path = getGameDirectory() .. "\\moonloader\\lib\\cheat-menu\\weapons\\",
    quick_spawn = imgui.new.bool(fconfig.get('tweapons.quick_spawn',false)),
    noreload = imgui.new.bool(false),
    fast_reload = imgui.new.bool(false),
    search_text = imgui.new.char[20](fconfig.get('tweapons.search_text',"")),
    models = {},
}

for i = 1,46,1 do
    table.insert(tweapons.models,i)
end

module.tweapons = tweapons

local flibweapons = require 'lib.game.weapons'

function module.CBaseWeaponInfo(name)

    local weapon = callFunction(0x743D10,1,1,name)
    if name ~= "" and getWeapontypeModel(weapon) ~= 0 then
        return weapon
    else
        return 0
    end
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
    imgui.Text(flanguage.GetText("weapons.Weapons"))
    imgui.Separator()
    imgui.Spacing()

    if imgui.Button(flanguage.GetText("weapons.RemoveAllWeapons"),imgui.ImVec2(fcommon.GetSize(2))) then
        removeAllCharWeapons(PLAYER_PED)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button(flanguage.GetText("weapons.RemoveCurrentWeapon"),imgui.ImVec2(fcommon.GetSize(2))) then
        removeWeaponFromChar(PLAYER_PED,getCurrentCharWeapon(PLAYER_PED))
        fcommon.CheatActivated()
    end

    imgui.Spacing()

    if imgui.Button(flanguage.GetText("weapons.WeaponSet1"),imgui.ImVec2(fcommon.GetSize(3),25)) then
        callFunction(0x4385B0,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button(flanguage.GetText("weapons.WeaponSet2"),imgui.ImVec2(fcommon.GetSize(3),25)) then
        callFunction(0x438890,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button(flanguage.GetText("weapons.WeaponSet3"),imgui.ImVec2(fcommon.GetSize(3),25)) then
        callFunction(0x438B30,1,1,false)
        fcommon.CheatActivated()
    end

    imgui.Spacing()

    if imgui.BeginTabBar("Weapons") then
        if imgui.BeginTabItem(flanguage.GetText("weapons.Spawn")) then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ name = flanguage.GetText("weapons.QuickWeapon"),var = tweapons.quick_spawn,help_text = flanguage.GetText("weapons.QuickWeaponToolTip")})
            fcommon.CheckBox({ name = flanguage.GetText("weapons.NoReloadInfAmmo"),var = tweapons.noreload,func = function()
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
            fcommon.CheckBox({ name = flanguage.GetText("weapons.FastReload"),var = tweapons.fast_reload,func = function()
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
            imgui.Text(flanguage.GetText("weapons.WeaponList"))
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Weapon list Window") then
                fcommon.ShowEntries(flanguage.GetText("weapons.Melee"),{1,2,3,4,6,7,8,9},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.Handguns"),{22,23,24},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.Shotguns"),{25,26,27},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.SubMachineGuns"),{28,29,32},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.AssaultRifles"),{30,31},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.Rifles"),{33,34},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.HeavyWeapons"),{35,36,37,38},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.Projectiles"),{16,17,18,39},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                fcommon.ShowEntries(flanguage.GetText("weapons.Miscellaneous"),{10,11,12,14,15,41,42,43,44,45,46},65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("common.Search")) then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText(flanguage.GetText("common.Search"),tweapons.search_text,ffi.sizeof(tweapons.search_text)) then end
            imgui.SameLine()

            imgui.Spacing()
            imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tweapons.search_text) .. ")")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Weapon Entries") then
                fcommon.ShowEntries(nil,tweapons.models,65,65,tweapons.images,tweapons.path,".png",fweapons.GiveWeaponToPlayer,flibweapons.get_name,true,tweapons.search_text)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end
return module
