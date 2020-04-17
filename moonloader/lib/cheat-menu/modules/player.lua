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

local module = {}

module.tplayer =
{
    aimSkinChanger      = imgui.new.bool(fconfig.Get('tplayer.aimSkinChanger',false)),
    cjBody              = imgui.new.int(fconfig.Get('tplayer.cjBody',0)),
    clothes             = 
    { 
        images          = {},
        path            = tcheatmenu.dir .. "clothes\\",
    },
    filter              = imgui.ImGuiTextFilter(),
    god                 = imgui.new.bool(fconfig.Get('tplayer.god',false)),
    invisible           = imgui.new.bool(fconfig.Get('tplayer.invisible',false)),
    keep_position       = imgui.new.bool(fconfig.Get('tplayer.keep_position',false)),
    model_val           = nil,
    never_wanted        = imgui.new.bool(false),
}

function module.KeepPosition()
    while true do
        if isPlayerDead(PLAYER_HANDLE) and module.tplayer.keep_position[0] then
            local x,y,z = getCharCoordinates(PLAYER_PED)
            wait(1000)
            setCharCoordinates(PLAYER_PED,x,y,z)
        end
        wait(0)
    end
end

function module.ChangePlayerModel(model)
    model = tonumber(model)
    if fped.tped.names[tostring(model)] ~= nil then
        if fped.tped.special[tostring(model)] == nil then
            if isModelAvailable(model) then
                requestModel(model)
                loadAllModelsNow()
                setPlayerModel(PLAYER_HANDLE,model)
                markModelAsNoLongerNeeded(model)
            end
        else
            if not hasSpecialCharacterLoaded(290) then
                unloadSpecialCharacter(model)
            end
            loadSpecialCharacter(fped.tped.special[tostring(model)],1)
            loadAllModelsNow()
            setPlayerModel(PLAYER_HANDLE,290)
        end
        
        local veh = nil
        if isCharInAnyCar(PLAYER_PED) then
            veh = getCarCharIsUsing(PLAYER_PED)
            speed = getCarSpeed(veh)
        end
        clearCharTasksImmediately(PLAYER_PED)
        if veh ~= nil then
            taskWarpCharIntoCarAsDriver(PLAYER_PED,veh)
            setCarForwardSpeed(veh,speed)
        end
        printHelpString("~g~Skin~w~ changed")
    end
end

function WantedLevelMenu()
    
    fcommon.DropDownMenu("Wanted level",function()
        local  _,wl = storeWantedLevel(PLAYER_HANDLE)
        local wanted_level = imgui.new.int(wl)
        local max_wanted_level = imgui.new.int(readMemory(0x58DFE4,1,false))
        
        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. tostring(6))
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)

        if imgui.InputInt("Set",wanted_level) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,wanted_level[0])
        end
        imgui.PopItemWidth()
   
        imgui.Spacing()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,max_wanted_level[0])
        end
    end)
end

--------------------------------------------------
-- Cloth functions

function module.GetClothName(name,separated)
    local body_part, model, texture = name:match("([^$]+)$([^$]+)$([^$]+)")

    if separated == nil then
        return string.format( "%s\n%s\n%s",body_part,model,texture)
    else
        return body_part,model,texture
    end

end

function module.ChangePlayerCloth(name)
    local body_part, model, texture = name:match("([^$]+)$([^$]+)$([^$]+)")
    
    setPlayerModel(PLAYER_HANDLE,0)
    givePlayerClothesOutsideShop(PLAYER_HANDLE,0,0,body_part)
    givePlayerClothesOutsideShop(PLAYER_HANDLE,texture,model,body_part)
    buildPlayerModel(PLAYER_HANDLE)
    printHelpString("Clothes changed")
    clearCharTasksImmediately(PLAYER_PED)
end

function module.RemoveThisCloth(name)
    local body_part, model, texture = name:match("([^$]+)$([^$]+)$([^$]+)")
    givePlayerClothes(PLAYER_HANDLE,0,0,body_part)
    buildPlayerModel(PLAYER_HANDLE)
end
--------------------------------------------------

-- Main function
function module.PlayerMain()
    if imgui.Button("Suicide",imgui.ImVec2(fcommon.GetSize(1))) then
        setCharHealth(PLAYER_PED,0)
    end
    imgui.Spacing()

    fcommon.Tabs("Player",{"Checkboxes","Menus","Skins","Clothes"},{
        function()
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxValue("Aim while driving",0x969179)
            fcommon.CheckBoxVar("God mode",module.tplayer.god)
            fcommon.CheckBoxValue("Have bounty on head",0x96913F)
            fcommon.CheckBoxValue("Higher cycle jumps",0x969161)
            fcommon.CheckBoxValue("Infinite ammo",0x969178)
            fcommon.CheckBoxValue("Infinite oxygen",0x96916E)
            fcommon.CheckBoxValue("Infinite run",0xB7CEE4)
        
            imgui.NextColumn()

            fcommon.CheckBoxFunc("Invisible player",module.tplayer.invisible,
            function()
                if module.tplayer.invisible[0] then
                    module.tplayer.model_val = readMemory((getCharPointer(PLAYER_PED)+1140),4,false)
                    writeMemory(getCharPointer(PLAYER_PED)+1140,4,2,false)
                    fcommon.CheatActivated()
                else
                    writeMemory((getCharPointer(PLAYER_PED)+1140),4,fplayer.tplayer.model_val,false)
                    fcommon.CheatDeactivated()
                end
            end,"Player can't enter/exit vehicle")
            fcommon.CheckBoxVar("Keep position",module.tplayer.keep_position,"Auto teleport to the position you died from")
            fcommon.CheckBoxValue("Lock player control",getCharPointer(PLAYER_PED)+0x598)
            fcommon.CheckBoxValue("Mega jump",0x96916C)
            fcommon.CheckBoxValue("Mega punch",0x969173)
            fcommon.CheckBoxValue("Never get hungry",0x969174)

            module.tplayer.never_wanted[0] = readMemory(0x969171 ,1,false)
            fcommon.CheckBoxFunc("Never wanted",module.tplayer.never_wanted,
            function()
                callFunction(0x4396C0,1,0,false)
                if module.tplayer.never_wanted[0] then
                    fcommon.CheatActivated()
                else
                    fcommon.CheatDeactivated()
                end
                fconfig.Set(fconfig.tconfig.misc_data,"Never Wanted",module.tplayer.never_wanted[0])
            end)
           
            imgui.Columns(1)
        end,
        function()
            fcommon.UpdateAddress({name = "Armour",address = getCharPointer(PLAYER_PED)+0x548,size = 4,min = 0,default =0,max = 100, is_float = true})
            fcommon.DropDownMenu("Body",function()
                if imgui.RadioButtonIntPtr("Fat",module.tplayer.cjBody,1) then
                    callFunction(0x439110,1,1,false)
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",1)
                    fcommon.CheatActivated()
                end
                if imgui.RadioButtonIntPtr("Muscle",module.tplayer.cjBody,2) then
                    -- body not changing to muscular after changing to fat fix
                    callFunction(0x439190,1,1,false)
                    callFunction(0x439150,1,1,false)
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",2)
                    fcommon.CheatActivated()
                end
                if imgui.RadioButtonIntPtr("Skinny",module.tplayer.cjBody,3) then
                    callFunction(0x439190,1,1,false)
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",3)
                    fcommon.CheatActivated()
                end
            end)
            fcommon.UpdateStat({ name = "Energy",stat = 165})
            fcommon.UpdateStat({ name = "Fat",stat = 21})
            fcommon.UpdateAddress({name = "Health",address = getCharPointer(PLAYER_PED)+0x540,size = 4,min = 0,default =100,max = 255, is_float = true})
            fcommon.UpdateStat({ name = "Lung capacity",stat = 225})
            fcommon.UpdateStat({ name = "Max health",stat = 24,min = 0,default = 569,max = 1450})
            fcommon.UpdateAddress({name = "Money",address = 0xB7CE50,size = 4,min = -9999999,max = 9999999})
            fcommon.UpdateStat({ name = "Muscle",stat = 23})
            fcommon.UpdateStat({ name = "Respect",stat = 68,max = 2450}) 
            fcommon.UpdateStat({ name = "Stamina",stat = 22})
            
            WantedLevelMenu()
        end,
        function()
            fcommon.CheckBoxVar("Aim skin changer", module.tplayer.aimSkinChanger)
            fcommon.InformationTooltip("Activate using, Aim ped +".. fcommon.GetHotKeyNames(tcheatmenu.hot_keys.asc_key))

            imgui.Spacing()
            fcommon.Tabs("Skins",{"List","Search"},{
                function()
                    fcommon.DrawImages(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.LIST,fped.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH,module.ChangePlayerModel,nil,fped.GetModelName,module.tplayer.filter)
                end,
                function()
                    fcommon.DrawImages(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.SEARCH,fped.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH,module.ChangePlayerModel,nil,fped.GetModelName,module.tplayer.filter)
                end
            })
        end,
        function()
            if imgui.Button("Remove clothes",imgui.ImVec2(fcommon.GetSize(1))) then
                for i=0, 17 do givePlayerClothes(PLAYER_HANDLE,0,0,i) end
                buildPlayerModel(PLAYER_HANDLE)
                printHelpString("Clothes ~r~removed")
            end
            imgui.Text("Info")
            fcommon.InformationTooltip("Right click to add clothes\nLeft click to remove clothes")
            imgui.Spacing()          
            fcommon.DrawImages(fconst.IDENTIFIER.CLOTHES,fconst.DRAW_TYPE.LIST,module.tplayer.clothes.images,fconst.CLOTH.IMAGE_HEIGHT,fconst.CLOTH.IMAGE_WIDTH,module.ChangePlayerCloth,module.RemoveThisCloth,module.GetClothName)
        end
    })
end

return module
