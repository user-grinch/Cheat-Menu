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

local module = {}

module.tweapon =
{
    fast_reload = imgui.new.bool(fconfig.Get('tweapon.fast_reload',false)),
    images      = {},
    names       = fcommon.LoadJson("weapon"),
    no_reload   = imgui.new.bool(fconfig.Get('tweapon.no_reload',false)),
    path        = tcheatmenu.dir .. "weapons",
    ped         = imgui.new.bool(fconfig.Get('tweapon.ped',false)),
    quick_spawn = imgui.new.bool(fconfig.Get('tweapon.quick_spawn',false)),
    search_text = imgui.new.char[20](""),   
}

-- Returns weapon name
function module.GetModelName(id)
    return module.tweapon.names[id] or ""
end

-- Used in quick spawner (fcommon)
function module.CBaseWeaponInfo(name)
    local weapon = callFunction(0x743D10,1,1,name)
    if name ~= "" and getWeapontypeModel(weapon) ~= 0 then
        return weapon
    else
        return 0
    end
end

-- Gives weapon to player or ped
function module.GiveWeapon(weapon)
    weapon = tonumber(weapon)
    model = getWeapontypeModel(weapon)
    if isModelAvailable(model) then
        requestModel(model)
        loadAllModelsNow()

        if module.tweapon.ped[0] == true then
            if fped.tped.selected ~=  nil then
                giveWeaponToChar(fped.tped.selected,weapon,99999)
                fcommon.CheatActivated()
            else
                printHelpString("~r~No~w~ ped selected")
            end
        else
            giveWeaponToChar(PLAYER_PED,weapon,99999)
            fcommon.CheatActivated()
        end          
        markModelAsNoLongerNeeded(model)
    end
end

-- Main function
function module.WeaponMain()
    imgui.Spacing()
    if imgui.Button("Remove current weapon",imgui.ImVec2(fcommon.GetSize(2))) then
        
            if module.tweapon.ped[0] == true then
                if fped.tped.selected ~=  nil then
                    removeWeaponFromChar(fped.tped.selected,getCurrentCharWeapon(fped.tped.selected))
                    fcommon.CheatActivated()
                else
                    printHelpString("~r~No~w~ ped selected")
                end
            else
                removeWeaponFromChar(PLAYER_PED,getCurrentCharWeapon(PLAYER_PED))
                fcommon.CheatActivated()
            end
            
    end
    imgui.SameLine()
    if imgui.Button("Remove all weapons",imgui.ImVec2(fcommon.GetSize(2))) then         
        if module.tweapon.ped[0] == true then
            if fped.tped.selected ~=  nil then
                removeAllCharWeapons(fped.tped.selected)
                fcommon.CheatActivated()
            else
                printHelpString("~r~No~w~ ped selected")
            end
        else
            removeAllCharWeapons(PLAYER_PED)
            fcommon.CheatActivated()
        end
    end
    imgui.Spacing()
    if imgui.BeginTabBar("Weapon") then
        if imgui.BeginTabItem("Checkbox") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ name = "Fast reload",var = module.tweapon.fast_reload,func = function()
                setPlayerFastReload(PLAYER_HANDLE,module.tweapon.fast_reload[0])
                if module.tweapon.fast_reload[0] then                  
                    fcommon.CheatActivated()
                else
                    fcommon.CheatDeactivated()
                end
            end})
            imgui.NextColumn()
            fcommon.CheckBox({ name = "No reload + Inf ammo",var = module.tweapon.no_reload,func = function()
                if module.tweapon.no_reload[0] then
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
            imgui.Columns(1)
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Spawn") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ name = "Quick Weapon",var = module.tweapon.quick_spawn,help_text = "Weapon can be spawned from quick spawner using (Left Ctrl + Q). \n\nControls:\nEnter : Stop reading key press\nDelete : Erase full string\nBackspace : Erase last character"})
            imgui.NextColumn()
            fcommon.CheckBox({name = "Ped",var = module.tweapon.ped,help_text = "Give weapon to ped.Aim with a gun to select."})
            imgui.Columns(1)
            imgui.Spacing()
            if imgui.BeginTabBar("Spawn") then
                
                if imgui.BeginTabItem("List") then
                    imgui.Spacing()
                    fcommon.DrawImages(fconst.IDENTIFIER.WEAPON,fconst.DRAW_TYPE.LIST,module.tweapon.images,fconst.WEAPON.IMAGE_HEIGHT,fconst.WEAPON.IMAGE_WIDTH,module.GiveWeapon,nil,module.GetModelName)
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    if imgui.InputText("Search",module.tweapon.search_text,ffi.sizeof(module.tweapon.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text("Weapons found :(" .. ffi.string(module.tweapon.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    fcommon.DrawImages(fconst.IDENTIFIER.WEAPON,fconst.DRAW_TYPE.SEARCH,module.tweapon.images,fconst.WEAPON.IMAGE_HEIGHT,fconst.WEAPON.IMAGE_WIDTH,module.GiveWeapon,nil,module.GetModelName,ffi.string(module.tweapon.search_text))
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end
return module
