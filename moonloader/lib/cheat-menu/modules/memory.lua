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

module.tmemory    =
{
    address            = imgui.new.char[10](""),
    filter             = imgui.ImGuiTextFilter(),
    offset             = imgui.new.char[10]("0"),
    is_float           = imgui.new.bool(fconfig.Get('tmemory.is_float',false)),
    list               = fcommon.LoadJson("memory"),
    name               = imgui.new.char[20](""),
    radio_button       = imgui.new.int(fconfig.Get('tmemory.radio_button',0)), 
    size               = imgui.new.int(fconfig.Get('tmemory.size',1)),
    value              = imgui.new.int(fconfig.Get('tmemory.value',0)),
    vp                 = imgui.new.bool(fconfig.Get('tmemory.vp',false)),
}

-- Main function
function module.MemoryMain()

    if fcommon.BeginTabBar("MemoryBar") then
        if fcommon.BeginTabItem("Read") then
            imgui.Text("Pointer")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("None", module.tmemory.radio_button, 0)
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Car", module.tmemory.radio_button, 1)
            fcommon.InformationTooltip("Get pointer of the nearest car")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Char", module.tmemory.radio_button, 2)
            fcommon.InformationTooltip("Get pointer of the nearest char")
            imgui.Dummy(imgui.ImVec2(0,10))

            imgui.Columns(2,nil,false)
            imgui.Text("Memory value : " .. module.tmemory.value[0])
            imgui.NextColumn()

            local car,ped = storeClosestEntities(PLAYER_PED)

            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CAR then      
                if car ~= -1 then
                    local pCar = getCarPointer(car)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pCar))
                end
            end
            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CHAR then
                
                if ped ~= -1 then
                    local pChar = getCharPointer(ped)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pChar))
                end
            end
            imgui.Columns(1)
            imgui.Spacing()

            imgui.InputTextWithHint("Address","0x000000",module.tmemory.address,ffi.sizeof(module.tmemory.address))
            imgui.InputText("Offset", module.tmemory.offset,ffi.sizeof(module.tmemory.offset))

            imgui.SliderInt("Size", module.tmemory.size,1,4)

            if module.tmemory.size[0] == 4 then
                imgui.Columns(2,nil,false)
                imgui.Checkbox("Float",module.tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(1,nil,false)
            end


            imgui.Checkbox("Virtual protect", module.tmemory.vp)
            imgui.Columns(1)
            imgui.Dummy(imgui.ImVec2(0,10))
            if imgui.Button("Read",imgui.ImVec2(fcommon.GetSize(2))) then

                if ffi.string(module.tmemory.offset) == "" then 
                    imgui.StrCopy(module.tmemory.offset,"0") 
                end

                if ffi.string(module.tmemory.address) ~= "" then
                    module.tmemory.value[0] = fcommon.RwMemory((tonumber(ffi.string(module.tmemory.address))+tonumber(ffi.string(module.tmemory.offset))),module.tmemory.size[0],nil,module.tmemory.vp[0],module.tmemory.is_float[0])
                else 
                    printHelpString("No address found")
                end
            end
            imgui.SameLine()
            if imgui.Button("Clear",imgui.ImVec2(fcommon.GetSize(2))) then
                module.tmemory.value[0] = 0
                imgui.StrCopy(module.tmemory.address,"")
                imgui.StrCopy(module.tmemory.offset,"0")
                module.tmemory.size[0] = 0
                module.tmemory.vp[0] = false
                module.tmemory.is_float[0] = false
                module.tmemory.radio_button[0] = 0
                printHelpString("Entries cleared")
            end
        end
        if fcommon.BeginTabItem("Write") then
            imgui.Text("Pointer")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("None", module.tmemory.radio_button, 0)
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Car", module.tmemory.radio_button, 1)
            fcommon.InformationTooltip("Get pointer of the nearest car")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Char", module.tmemory.radio_button, 2)
            fcommon.InformationTooltip("Get pointer of the nearest char")
            imgui.Dummy(imgui.ImVec2(0,10))

            local car,ped = storeClosestEntities(PLAYER_PED)

            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CAR then      
                if car ~= -1 then
                    local pCar = getCarPointer(car)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pCar))
                end
            end
            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CHAR then
                
                if ped ~= -1 then
                    local pChar = getCharPointer(ped)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pChar))
                end
            end
            
            imgui.InputInt("Value", module.tmemory.value)
            imgui.InputTextWithHint("Address","0x000000",module.tmemory.address,ffi.sizeof(module.tmemory.address))
            imgui.InputText("Offset", module.tmemory.offset,ffi.sizeof(module.tmemory.offset))
            imgui.SliderInt("Size", module.tmemory.size,1,4)

            if module.tmemory.size[0] == 4 then
                imgui.Columns(2,nil,false)
                imgui.Checkbox("Float",module.tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(1,nil,false)
            end


            imgui.Checkbox("Virtual protect", module.tmemory.vp)
            imgui.Columns(1)
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button("Write",imgui.ImVec2(fcommon.GetSize(2))) then
                
                if ffi.string(module.tmemory.offset) == "" then 
                    imgui.StrCopy(module.tmemory.offset,"0") 
                end

                if ffi.string(module.tmemory.address) ~= "" then
                    fcommon.RwMemory(tonumber(ffi.string(module.tmemory.address))+tonumber(ffi.string(module.tmemory.offset)),module.tmemory.size[0],module.tmemory.value[0],module.tmemory.vp[0],module.tmemory.is_float[0])
                    printHelpString("Value Updated")
                else 
                    printHelpString("No address found")
                end
            end
            imgui.SameLine()
            if imgui.Button("Clear",imgui.ImVec2(fcommon.GetSize(2))) then
                module.tmemory.value[0] = 0
                imgui.StrCopy(module.tmemory.address,"")
                imgui.StrCopy(module.tmemory.offset,"0")
                module.tmemory.size[0] = 0
                module.tmemory.vp[0] = false
                module.tmemory.is_float[0] = false
                module.tmemory.radio_button[0] = 0
                printHelpString("Entries cleared")
            end
        end
        if fcommon.BeginTabItem("Search") then
            fcommon.DrawEntries(fconst.IDENTIFIER.MEMORY,fconst.DRAW_TYPE.TEXT,
                function(address,size)
                    imgui.StrCopy(module.tmemory.address,address)
                    imgui.StrCopy(module.tmemory.offset,"0")
                    if size == "byte" then 
                        module.tmemory.size[0] = 1
                        module.tmemory.is_float[0] = false
                    end
                    if size == "word" then 
                        module.tmemory.size[0] = 2
                        module.tmemory.is_float[0] = false
                    end
                    if size == "dword" then 
                        module.tmemory.size[0] = 4
                        module.tmemory.is_float[0] = false
                    end
                    if size == "float" then 
                        module.tmemory.size[0] = 4
                        module.tmemory.is_float[0] = true
                    end
                    printHelpString("Address set")
                end,
                function(text)
                    if imgui.MenuItemBool("Remove memory") then
                        for category,table in pairs(module.tmemory.list) do
                            for key,val in pairs(table) do
                                if key == text then
                                    module.tmemory.list[category][key] = nil
                                    goto end_loop
                                end
                            end
                        end
                        ::end_loop::

                        printHelpString("Address removed")
                    end
                end,
                function(a) return a end,module.tmemory.list)
        end
        if fcommon.BeginTabItem("Custom") then
            imgui.InputTextWithHint("Name","New address",module.tmemory.name,ffi.sizeof(module.tmemory.name))
            imgui.InputTextWithHint("Address","0x000000",module.tmemory.address,ffi.sizeof(module.tmemory.address))
            imgui.SliderInt("Size", module.tmemory.size,1,4)
            imgui.Checkbox("Float",module.tmemory.is_float)
            imgui.Spacing()
            if imgui.Button("Add address",imgui.ImVec2(fcommon.GetSize(1))) then

                if ffi.string(module.tmemory.name) == "" then 
                    printHelpString("No name found")
                else
                    if ffi.string(module.tmemory.address) == "" or tonumber(ffi.string(module.tmemory.address)) == nil then
                        printHelpString("No address found")
                    else
                        local mem_type = ""
                        if module.tmemory.size[0] == 1 then 
                            mem_type = "byte"
                        end
                        if module.tmemory.size[0] == 2 then 
                            mem_type = "word"
                        end
                        if module.tmemory.size[0] == 4 then 
                            if module.tmemory.is_float[0] == true then
                                mem_type = "float"
                            else
                                mem_type = "dword"
                            end
                        end
                        
                        module.tmemory.list[mem_type][ffi.string(module.tmemory.name)] = ffi.string(module.tmemory.address)
                        fcommon.SaveJson("memory",module.tmemory.list)
                        module.tmemory.list = fcommon.LoadJson("memory")
                        printHelpString("Address added")
                    end
                end
                
            end
        end
        fcommon.EndTabBar()
    end
end

return module
