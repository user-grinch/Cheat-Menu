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
    gang_wars   = imgui.new.bool(fconfig.get('tped.gang_wars',false)),
    images      = {},
    models      = {},
    model_list  = {},
    names       = ftable.peds.list,
    path        = tcheatmenu.dir .. "peds\\",
    search_text = imgui.new.char[20](""),  
    selected    = nil,
    special     = ftable.peds.special,
    type        =
    {
        array   = {},
        names   = ftable.peds.types,
        index   = imgui.new.int(fconfig.get('tped.type.index',0)),
    },
}



module.tped.type.array = imgui.new['const char*'][#module.tped.type.names](module.tped.type.names)

function module.GetName(model)
    if module.tped.names[model] then return module.tped.names[model] else return "" end
end

function module.SpawnPed(model)  
    if  module.tped.names[model] ~= nil then
        if module.tped.special[model] == nil then
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.type.index[0]+2,model,x,y,z)
            markModelAsNoLongerNeeded(model)
            markCharAsNoLongerNeeded(ped)
        else
            if hasSpecialCharacterLoaded(model) then
                unloadSpecialCharacter(model)
            end
            loadSpecialCharacter(module.tped.special[model],1)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.type.index[0]+2,290,x,y,z)
            markModelAsNoLongerNeeded(module.tped.special[model])
            markCharAsNoLongerNeeded(ped)
        end
        printHelpString("Ped ~g~Spawned")
    end
end

function SetDensity(title,id)
    local x,y,z = getCharCoordinates(PLAYER_PED)
        
    local density = imgui.new.int(getZoneGangStrength(getNameOfInfoZone(x,y,z),id))
    if imgui.SliderInt(title,density,0,255) then
        setZoneGangStrength(getNameOfInfoZone(x,y,z),id,density[0])
        clearSpecificZonesToTriggerGangWar()
        setGangWarsActive(fped.tped.gang_wars[0])
    end
end

function module.PedMain()

        if imgui.BeginTabBar("Ped") then
            imgui.Spacing()
            if imgui.BeginTabItem("Checkbox") then
                imgui.Columns(2,nil,false)
                fcommon.CheckBox({ address = 0x969157,name = "Elvis everywhere"})
                fcommon.CheckBox({ address = 0x969140,name = "Everyone is armed"})
                fcommon.CheckBox({ address = 0x96915B,name = "Gangs control the streets"})
                fcommon.CheckBox({ address = 0x96915A,name = "Gang members everywhere"})
                fcommon.CheckBox({ address = 0x96913F,name = "Have bounty on head"})
                imgui.NextColumn()
                fcommon.CheckBox({name = "Gang wars",var = module.tped.gang_wars,func = function()
                    setGangWarsActive(module.tped.gang_wars[0])
                    if module.tped.gang_wars[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
                end})
                fcommon.CheckBox({ address = 0x969158,name = "Peds attack with rockets"})
                fcommon.CheckBox({ address = 0x969175,name = "Peds riot"})
                fcommon.CheckBox({ address = 0x96915D,name = "Slut magnet"})

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
                    SetDensity("Ballas",0)
                    SetDensity("Da nang boys",4)
                    SetDensity("Gang9",8)
                    SetDensity("Gang10",9)
                    SetDensity("Grove street families",1)
                    SetDensity("Los santos vagos",2)
                    SetDensity("Mafia",5)
                    SetDensity("Mountain cloud triad",6)
                    SetDensity("San fierro rifa",3)
                    SetDensity("Varrio los aztecas",7)
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
                                    
                for dir in lfs.dir(module.tped.path) do
                    if doesDirectoryExist(module.tped.path .. dir) and dir ~= "." and dir ~= ".." then
                        if imgui.BeginTabItem(dir) then
                            imgui.Spacing()
                            if imgui.BeginChild("Ped list Child") then
                                for subdir in lfs.dir(module.tped.path .. dir) do
                                    if doesDirectoryExist(module.tped.path .. dir .. "\\" .. subdir) and subdir ~= "." and subdir ~= ".." then
                                        list = {}
                                        for pic in lfs.dir(module.tped.path .. dir .. "\\" .. subdir) do
                                            if doesFileExist(module.tped.path .. dir .. "\\" .. subdir .. "\\" .. pic) then
                                                table.insert(list,tonumber(string.sub(pic,1,-5)))
                                            end
                                        end
                                        fcommon.ShowEntries(subdir,list,110,55,module.tped.images,(module.tped.path .. dir .. "\\" .. subdir .. "\\"),".jpg",module.SpawnPed,module.GetName,true)
                                    end
                                end
                                imgui.EndChild()
                            end
                            imgui.EndTabItem()
                        end
                    end
                end
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",module.tped.search_text,ffi.sizeof(module.tped.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text("Found peds :(" .. ffi.string(module.tped.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Ped Child") then
                        lua_thread.create(function()
                            for dir in lfs.dir(module.tped.path) do
                                if doesDirectoryExist(module.tped.path .. dir) and dir ~= "." and dir ~= ".." then  
                                    for subdir in lfs.dir(module.tped.path .. dir) do
                                        if doesDirectoryExist(module.tped.path .. dir .. "\\" .. subdir) and subdir ~= "." and subdir ~= ".." then
                                            for pic in lfs.dir(module.tped.path .. dir .. "\\" .. subdir) do
                                                wait(0)
                                                if doesFileExist(module.tped.path .. dir .. "\\" .. subdir .. "\\" .. pic) then
                                                    temp = {}
                                                    model = tonumber(string.sub(pic,1,-5))
                                                    for key, value in pairs(module.tped.model_list) do
                                                        if value == model then
                                                            model = nil
                                                        end
                                                    end
                                                    table.insert(module.tped.model_list,model)
                                                    table.insert(temp,model)
                                                    fcommon.LoadTextures(module.tped.images,(module.tped.path .. dir .. "\\" .. subdir .. "\\"),temp,".jpg")
                                                end
                                            end
                                        end
                                    end
                                end
                            end
                        end)
                        fcommon.ShowEntries(nil,module.tped.model_list,110,55,module.tped.images,nil,".jpg",module.SpawnPed,module.GetName,true,module.tped.search_text)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
        end
    end
end
return module
