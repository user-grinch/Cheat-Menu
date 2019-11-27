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

module.tmemory    =
{
    address            = imgui.new.char[10](""),
    offset             = imgui.new.char[10](""),
    is_float           = imgui.new.bool(fconfig.Get('tmemory.is_float',false)),
    list               = fcommon.LoadJson("memory"),
    name               = imgui.new.char[20](""),
    radio_button       = imgui.new.int(fconfig.Get('tmemory.radio_button',0)), 
    search_text        = imgui.new.char[20](),
    size               = imgui.new.int(fconfig.Get('tmemory.size',1)),
    value              = imgui.new.int(fconfig.Get('tmemory.value',0)),
    vp                 = imgui.new.bool(fconfig.Get('tmemory.vp',false)),
}

function MemoryEntry(name,size,address)
    if imgui.MenuItemBool(name)then
        imgui.StrCopy(module.tmemory.address,address)
        imgui.StrCopy(module.tmemory.offset,"")
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
        if size == "unknown" then 
            module.tmemory.size[0] = 0
            module.tmemory.is_float[0] = false
        end
        printHelpString("Address set")
    end
    if imgui.IsItemClicked(1) then
		module.tmemory.list[size .. "$" .. name] = nil
		fcommon.SaveJson("memory",module.tmemory.list)
		module.tmemory.list = fcommon.LoadJson("memory")
		printHelpString("Address ~r~removed")
	end
end

-- Main function
function module.MemoryMain()
 
    if isKeyDown(tkeys.control_key)
    and isKeyDown(tkeys.mc_paste) then
        imgui.StrCopy(module.tmemory.address, imgui.GetClipboardText(),ffi.sizeof(module.tmemory.address))
    end
    if imgui.BeginTabBar("Memory") then
        if imgui.BeginTabItem("Read") then

            imgui.Spacing()
            imgui.Text("Pointer")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("None", module.tmemory.radio_button, 0)
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Car", module.tmemory.radio_button, 1)
            fcommon.InformationTooltip("Get inside a car to get pointer")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Char", module.tmemory.radio_button, 2)
            fcommon.InformationTooltip("Aim with a gun to get pointer")
            imgui.Dummy(imgui.ImVec2(0,10))

            imgui.Columns(2,nil,false)
            imgui.Text("Memory value : " .. module.tmemory.value[0])
            imgui.NextColumn()

            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CAR then
                if isCharInAnyCar(PLAYER_PED) then
                    local car = getCarCharIsUsing(PLAYER_PED)
                    local pCar = getCarPointer(car)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pCar))
                end
            end
            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CHAR then
                bool, module.tmemory.char_handle =  getCharPlayerIsTargeting(PLAYER_HANDLE)
                
                if bool then
                    local pChar = getCharPointer(module.tmemory.char_handle)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pChar))
                end
            end
            imgui.Columns(1)
            imgui.Spacing()

            imgui.InputText("Address", module.tmemory.address,ffi.sizeof(module.tmemory.address))
            fcommon.InformationTooltip("Ctrl + V to paste")
            imgui.InputText("Offset", module.tmemory.offset,ffi.sizeof(module.tmemory.offset))
            fcommon.InformationTooltip("Blank = no offset")
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
                if ffi.string(module.tmemory.address) ~= "" then
                    module.tmemory.value[0] = fcommon.RwMemory(tonumber(ffi.string(module.tmemory.address))+tonumber(ffi.string(module.tmemory.offset)),module.tmemory.size[0],nil,module.tmemory.vp[0],module.tmemory.is_float[0])
                end
            end
            imgui.SameLine()
            if imgui.Button("Clear",imgui.ImVec2(fcommon.GetSize(2))) then
                module.tmemory.value[0] = 0
                imgui.StrCopy(module.tmemory.address,"")
                imgui.StrCopy(module.tmemory.offset,"")
                module.tmemory.size[0] = 0
                module.tmemory.vp[0] = false
                module.tmemory.is_float[0] = false
                module.tmemory.radio_button[0] = 0
                printHelpString("Entries cleared")
            end

            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Write") then
            imgui.Spacing()
            imgui.Text("Pointer")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("None", module.tmemory.radio_button, 0)
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Car", module.tmemory.radio_button, 1)
            fcommon.InformationTooltip("Get inside a car to get pointer")
            imgui.SameLine()
            imgui.RadioButtonIntPtr("Char", module.tmemory.radio_button, 2)
            fcommon.InformationTooltip("Aim with a gun to get pointer")
            imgui.Dummy(imgui.ImVec2(0,10))

            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CAR then
                if isCharInAnyCar(PLAYER_PED) then
                    local car = getCarCharIsUsing(PLAYER_PED)
                    local pCar = getCarPointer(car)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pCar))
                end
            end
            if module.tmemory.radio_button[0] == fconst.MEMORY_RB.CHAR then
                bool, module.tmemory.char_handle =  getCharPlayerIsTargeting(PLAYER_HANDLE)
                
                if bool then
                    local pChar = getCharPointer(module.tmemory.char_handle)
                    imgui.StrCopy(module.tmemory.address,string.format("0x%8.8X",pChar))
                end
            end
            
            imgui.InputInt("Value", module.tmemory.value)
            imgui.InputText("Address", module.tmemory.address,ffi.sizeof(module.tmemory.address))
            fcommon.InformationTooltip("Ctrl + V to paste")
            imgui.InputText("Offset", module.tmemory.offset,ffi.sizeof(module.tmemory.offset))
            fcommon.InformationTooltip("Blank = no offset")
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
                if ffi.string(module.tmemory.address) ~= "" then
                    fcommon.RwMemory(tonumber(ffi.string(module.tmemory.address))+tonumber(ffi.string(module.tmemory.offset)),module.tmemory.size[0],module.tmemory.value[0],module.tmemory.vp[0],module.tmemory.is_float[0])
                    printHelpString("Value ~g~Updated")
                end
            end
            imgui.SameLine()
            if imgui.Button("Clear",imgui.ImVec2(fcommon.GetSize(2))) then
                module.tmemory.value[0] = 0
                imgui.StrCopy(module.tmemory.address,"")
                imgui.StrCopy(module.tmemory.offset,"")
                module.tmemory.size[0] = 0
                module.tmemory.vp[0] = false
                module.tmemory.is_float[0] = false
                module.tmemory.radio_button[0] = 0
                printHelpString("Entries cleared")
            end
            

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Search") then

            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",module.tmemory.search_text,ffi.sizeof(module.tmemory.search_text)) then end
            imgui.SameLine()
            fcommon.InformationTooltip("Right click over any of these entries to remove them")
            imgui.Spacing()
            imgui.Text("Addresses found :(" .. ffi.string(module.tmemory.search_text) .. ")")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Stat Entries") then
                for name_size,address in fcommon.spairs(module.tmemory.list) do
                    size, name = name_size:match("([^$]+)$([^$]+)")
                    if (string.upper(name):find(string.upper(ffi.string(module.tmemory.search_text)))) then
                        MemoryEntry(name, size, address)
                    end
                end       
                imgui.Spacing()
                imgui.EndChild()
            end

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Custom") then    -- Add custom memory tab
            imgui.Spacing()
            if imgui.InputText("Name",module.tmemory.name,ffi.sizeof(module.tmemory.name)) then end
            if imgui.InputText("Address",module.tmemory.address,ffi.sizeof(module.tmemory.address)) then end
            imgui.SliderInt("Size", module.tmemory.size,1,4)
            imgui.Checkbox("Float",module.tmemory.is_float)
            imgui.Spacing()
            if imgui.Button("Add address",imgui.ImVec2(fcommon.GetSize(1))) then

                local size = ""
                if module.tmemory.size[0] == 1 then 
                    size = "byte"
                end
                if module.tmemory.size[0] == 2 then 
                    size = "word"
                end
                if module.tmemory.size[0] == 4 then 
                    if module.tmemory.is_float[0] == true then
                        size = "float"
                    else
                        size = "dword"
                    end
                end
                
                module.tmemory.list[size .. "$" .. ffi.string(module.tmemory.name)] = ffi.string(module.tmemory.address)
                fcommon.SaveJson("memory",module.tmemory.list)
                module.tmemory.list = fcommon.LoadJson("memory")
                printHelpString("Address ~g~added")
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
