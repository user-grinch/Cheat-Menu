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

module.tplayer =
{
    aimSkinChanger      = imgui.new.bool(fconfig.Get('tplayer.aimSkinChanger',false)),
    cjBody              = imgui.new.int(fconfig.Get('tplayer.cjBody',0)),
    clothes             = 
    { 
        images          = {},
        path            = tcheatmenu.dir .. "clothes\\",
    },
    god                 = imgui.new.bool(fconfig.Get('tplayer.god',false)),
    invisible           = imgui.new.bool(fconfig.Get('tplayer.invisible',false)),
    keep_position       = imgui.new.bool(fconfig.Get('tplayer.keep_stuff',false)),
    model_val           = nil,
    never_wanted        = imgui.new.bool(false),
    skins =
    {
        search_text    = imgui.new.char[20](),
    },
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
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)
            speed = getCarSpeed(car)
        end
        clearCharTasksImmediately(PLAYER_PED)
        if car ~= nil then
            taskWarpCharIntoCarAsDriver(PLAYER_PED,car)
            setCarForwardSpeed(car,speed)
        end
        printHelpString("~g~Skin~w~ changed")
    end
end

function HealthArmour()
    fcommon.DropDownMenu("Health",function()
        local health = imgui.new.int(getCharHealth(PLAYER_PED))

        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. tostring(255))
        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-70)
        if imgui.InputInt("Set ##Health",health) then
            if health[0] > 100 then
                setFloatStat(24,health[0]*5.686)
                
            else
                setFloatStat(24,569.0)
            end

            setCharHealth(PLAYER_PED,health[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Minimum ##Health",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,0)
        end
        imgui.SameLine()
        if imgui.Button("Default ##Health",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,100)
        end
        imgui.SameLine()
        if imgui.Button("Maximum ##Health",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,1450.0)
            setCharHealth(PLAYER_PED,255)
        end

        if health[0] < 0 then
            setCharHealth(PLAYER_PED,0)
        end

        if health[0] >  255 then
            setCharHealth(PLAYER_PED, 255)
        end
    end)

    fcommon.DropDownMenu("Armour",function()
        local armour = imgui.new.int()
        local max_armour = 100
        local min_armour = 0
        armour[0] = getCharArmour(PLAYER_PED)

        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. min_armour)
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. max_armour)
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-70)
        if imgui.InputInt("Set ##Armour",armour) then

            if armour[0] < 0 then
                armour[0] = 0
            end
            if getCharArmour(PLAYER_PED) < armour[0] then
                addArmourToChar(PLAYER_PED,(armour[0]-getCharArmour(PLAYER_PED)))
            end
            if getCharArmour(PLAYER_PED) > armour[0] then
                damageChar(PLAYER_PED,getCharArmour(PLAYER_PED)-armour[0],true)
            end
        end
        imgui.PopItemWidth()
        imgui.Spacing()
        if imgui.Button("Minimum ##Armour",imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button("Default ##Armour",imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button("Maximum ##Armour",imgui.ImVec2(fcommon.GetSize(3))) then
            addArmourToChar(PLAYER_PED, max_armour)
        end
    end)
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

function SkinChangerMenu()
    imgui.Spacing()
    fcommon.CheckBoxVar("Aim skin changer", module.tplayer.aimSkinChanger)
    fcommon.InformationTooltip("Activate using, Aim ped +".. fcommon.GetHotKeyNames(tcheatmenu.hot_keys.asc_key))

    imgui.Spacing()
    if imgui.BeginTabBar("Skins") then
        if imgui.BeginTabItem("List") then
            fcommon.DrawImages(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.LIST,fped.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH,module.ChangePlayerModel,nil,fped.GetModelName,ffi.string(module.tplayer.skins.search_text))
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Search") then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",module.tplayer.skins.search_text,ffi.sizeof(module.tplayer.skins.search_text)) then end
            imgui.SameLine()

            imgui.Spacing()
            imgui.Text("Skins found :(" .. ffi.string(module.tplayer.skins.search_text) .. ")")
            imgui.Separator()
            imgui.Spacing()
            fcommon.DrawImages(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.SEARCH,fped.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH,module.ChangePlayerModel,nil,fped.GetModelName,ffi.string(module.tplayer.skins.search_text))
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

--------------------------------------------------
-- Cloth functions

function module.GetClothName(name)
    local body_part, model, texture = name:match("([^$]+)$([^$]+)$([^$]+)")
    return texture
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
--------------------------------------------------

-- Main function
function module.PlayerMain()
    if imgui.Button("Suicide",imgui.ImVec2(fcommon.GetSize(1))) then
        setCharHealth(PLAYER_PED,0)
    end
    imgui.Spacing()

    if imgui.BeginTabBar('Player') then

        if imgui.BeginTabItem("Checkbox") then
            imgui.Spacing()
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
            end)
           
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            imgui.Text("Body")
            if imgui.RadioButtonIntPtr("Fat",module.tplayer.cjBody,1) then
                callFunction(0x439110,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr("Muscle",module.tplayer.cjBody,2) then
                -- body not changing to muscular after changing to fat fix
                callFunction(0x439190,1,1,false)
                callFunction(0x439150,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr("Skinny",module.tplayer.cjBody,3) then
                callFunction(0x439190,1,1,false)
                fcommon.CheatActivated()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Menu") then
            imgui.Spacing()
            
            fcommon.UpdateStat({ name = "Energy",stat = 165})
            fcommon.UpdateStat({ name = "Fat",stat = 21})
            HealthArmour()
            fcommon.UpdateStat({ name = "Lung capacity",stat = 225})
            fcommon.UpdateAddress({name = "Money",address = 0xB7CE50,size = 4,min = -9999999,max = 9999999})
            fcommon.UpdateStat({ name = "Muscle",stat = 23})
            fcommon.UpdateStat({ name = "Respect",stat = 68,max = 2450}) 
            fcommon.UpdateStat({ name = "Stamina",stat = 22})
            WantedLevelMenu()

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Skin") then
            SkinChangerMenu()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Clothes") then
            imgui.Spacing()
            if imgui.Button("Remove clothes",imgui.ImVec2(fcommon.GetSize(1))) then
                for i=0, 17 do givePlayerClothes(PLAYER_HANDLE,0,0,i) end
                buildPlayerModel(PLAYER_HANDLE)
                printHelpString("Clothes ~r~removed")
            end
            imgui.Spacing()

            if imgui.BeginTabBar("Clothes") then
                imgui.Spacing()
                
                if imgui.BeginTabItem("List") then
                    fcommon.DrawImages(fconst.IDENTIFIER.CLOTH,fconst.DRAW_TYPE.LIST,module.tplayer.clothes.images,fconst.CLOTH.IMAGE_HEIGHT,fconst.CLOTH.IMAGE_WIDTH,module.ChangePlayerCloth,nil,module.GetClothName)
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
