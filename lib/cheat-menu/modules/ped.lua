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

module.tped =
{

    gang =
    {
        array = {},
        index = imgui.new.int(0),
        list = 
        {
            "Ballas",
            "Grove street families",
            "Los santos vagos",
            "San fierro rifa",
            "Da nang boys",
            "Mafia",
            "Mountain cloud triad",
            "Varrio los aztecas",
            "Gang9",
            "Gang10",
        },
        names = 
        {   
            ["Ballas"] = 0,
            ["Da nang boys"] = 4,
            ["Gang9"] = 8,
            ["Gang10"] = 9,
            ["Grove street families"] = 1,
            ["Los santos vagos"] = 2,
            ["Mafia"] = 5,
            ["Mountain cloud triad"] = 6,
            ["San fierro rifa"] = 3,
            ["Varrio los aztecas"] = 7,
        },  
        wars = imgui.new.bool(fconfig.Get('tped.gang.wars',false)),
    },
    images      = {},
    names       = fcommon.LoadJson("ped"),
    path        = tcheatmenu.dir .. "peds\\",
    search_text = imgui.new.char[20](""),  
    selected    = nil,
    special     = fcommon.LoadJson("ped special"),
    type        =
    {
        array   = {},
        names   = 
        {
            "CIVMALE",
            "CIVFEMALE",
            "COP",
            "Ballas",
            "Grove Street Families",
            "Los Santos Vagos",
            "San Fierro Rifa",
            "Da Nang Boys",
            "Mafia",
            "Mountain Cloud Triads",
            "Varrio Los Aztecas",
            "GANG9",
            "GANG10",
            "DEALER",
            "EMERGENCY",
            "FIREMAN",
            "CRIMINAL",
            "BUM",
            "SPECIAL",
            "PROSTITUTE"
        },
        index   = imgui.new.int(fconfig.Get('tped.type.index',0)),
    },
}

module.tped.type.array = imgui.new['const char*'][#module.tped.type.names](module.tped.type.names)
module.tped.gang.array = imgui.new['const char*'][#module.tped.gang.list](module.tped.gang.list)

-- Returns ped name
function module.GetModelName(model)
    if module.tped.names[model] then return module.tped.names[model] else return "" end
end

function module.SpawnPed(model)  
    model = tonumber(model)
    if  module.tped.names[tostring(model)] ~= nil then
        if module.tped.special[tostring(model)] == nil then
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.type.index[0]+5,model,x,y,z) -- CIVMALE = PLAYER + 5 
            markModelAsNoLongerNeeded(model)
            markCharAsNoLongerNeeded(ped)
        else
            if hasSpecialCharacterLoaded(model) then
                unloadSpecialCharacter(model)
            end
            loadSpecialCharacter(module.tped.special[tostring(model)],1)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.type.index[0]+5,290,x,y,z) -- CIVMALE = PLAYER + 5
            markModelAsNoLongerNeeded(module.tped.special[tostring(model)])
            markCharAsNoLongerNeeded(ped)
        end
        printHelpString("Ped ~g~Spawned")
    end
end

function SetGangZoneDensity(title,id)
    local x,y,z = getCharCoordinates(PLAYER_PED)
        
    local density = imgui.new.int(getZoneGangStrength(getNameOfInfoZone(x,y,z),id))
    if imgui.SliderInt(title,density,0,255) then
        setZoneGangStrength(getNameOfInfoZone(x,y,z),id,density[0])
        clearSpecificZonesToTriggerGangWar()
        setGangWarsActive(fped.tped.gang.wars[0])
    end
end

function module.PedMain()

        if imgui.BeginTabBar("Ped") then
            imgui.Spacing()
            if imgui.BeginTabItem("Checkbox") then
                imgui.Columns(2,nil,false)
                fcommon.CheckBoxValue("Elvis everywhere",0x969157)
                fcommon.CheckBoxValue("Everyone is armed",0x969140)
                fcommon.CheckBoxValue("Gangs control the streets",0x96915B)
                fcommon.CheckBoxValue("Gang members everywhere",0x96915A)
                fcommon.CheckBoxFunc("Gang wars",module.tped.gang.wars,
                function()
                    setGangWarsActive(module.tped.gang.wars[0])
                    if module.tped.gang.wars[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
                end)
                
                imgui.NextColumn()
                
                fcommon.CheckBoxValue("No road peds",0x8D2538,nil,0,25)
                fcommon.CheckBoxValue("Peds attack with golfclub",0x96913E)
                fcommon.CheckBoxValue("Peds attack with rockets",0x969158)
                fcommon.CheckBoxValue("Peds riot",0x969175)
                fcommon.CheckBoxValue("Slut magnet",0x96915D)

                imgui.Columns(1)
                imgui.Spacing()
                imgui.Separator()
                imgui.Spacing()
                fcommon.RadioButton("Recruit anyone",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E})

                imgui.EndTabItem()
            end
            if imgui.BeginTabItem("Menu") then
                fcommon.UpdateAddress({name = 'Pedestrian density multiplier',address = 0x8D2530,size = 4,min = 0,max = 10, default = 1,is_float = true})
                fcommon.DropDownMenu("Gang zone density",function()
                    imgui.PushItemWidth(imgui.GetWindowWidth() - 200)

                    for k,v in pairs(module.tped.gang.names) do
                        SetGangZoneDensity(k,v)
                    end
                    imgui.PopItemWidth()
                    imgui.Spacing()
                    imgui.Text("You'll need ExGangWars plugin to display some turf colors")
                end)
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem("Spawn") then
                imgui.Spacing()
                if imgui.Combo("Ped type", module.tped.type.index,module.tped.type.array,#module.tped.type.names) then end
                imgui.Spacing()
                if imgui.BeginTabBar("Peds list") then   
                    if imgui.BeginTabItem("List") then
                        fcommon.DrawImages(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.LIST,module.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH,module.SpawnPed,nil,module.GetModelName)         
                        imgui.EndTabItem()
                    end    
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",module.tped.search_text,ffi.sizeof(module.tped.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text("Peds found :(" .. ffi.string(module.tped.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    fcommon.DrawImages(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.SEARCH,module.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH,module.SpawnPed,nil,module.GetModelName,ffi.string(module.tped.search_text))

                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
        end
    end
end
return module
