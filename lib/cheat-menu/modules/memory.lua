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
    address       = imgui.new.char[10](fconfig.Get('tmemory.address',"")),
    clear_entries = imgui.new.bool(fconfig.Get('tmemory.clear_entries',false)),
    is_float      = imgui.new.bool(fconfig.Get('tmemory.is_float',false)),
    size          = imgui.new.int(fconfig.Get('tmemory.size',1)),
    value         = imgui.new.int(fconfig.Get('tmemory.value',0)),
    vp            = imgui.new.bool(fconfig.Get('tmemory.vp',false)),
}

-- Main function
function module.MemoryMain()
 
    if isKeyDown(tkeys.control_key)
    and isKeyDown(tkeys.mc_paste) then
        imgui.StrCopy(module.tmemory.address, imgui.GetClipboardText(),ffi.sizeof(module.tmemory.address))
    end

    if imgui.BeginTabBar("Memory") then
        if imgui.BeginTabItem("Read") then

            imgui.Text("Memory value : " .. module.tmemory.value[0])
            imgui.Spacing()
            imgui.InputText("Address", module.tmemory.address,ffi.sizeof(module.tmemory.address))
            fcommon.InformationTooltip("Ctrl + V to paste address")
            imgui.SliderInt("Size", module.tmemory.size,1,4)

            if module.tmemory.size[0] == 4 then
                imgui.Columns(3,nil,false)
                imgui.Checkbox("Float",module.tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(2,nil,false)
            end


            imgui.Checkbox("Virtual protect", module.tmemory.vp)
            imgui.NextColumn()
            imgui.Checkbox("Clear entries", module.tmemory.clear_entries)
            imgui.Columns(1)

            if imgui.Button("Read",imgui.ImVec2(fcommon.GetSize(1))) then

                if ffi.string(module.tmemory.address) ~= "" then
                    module.tmemory.value[0] = fcommon.RwMemory(tonumber(ffi.string(module.tmemory.address)),module.tmemory.size[0],nil,module.tmemory.vp[0],module.tmemory.is_float[0])
                    if module.tmemory.clear_entries[0] == true then
                        module.tmemory.value[0] = 0
                        imgui.StrCopy(module.tmemory.address,"")
                        module.tmemory.size[0] = 0
                        module.tmemory.vp[0] = false
                        module.tmemory.is_float[0] = false
                    end
                end
            end

            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Write") then
            imgui.InputInt("Value", module.tmemory.value)
            imgui.InputText("Address", module.tmemory.address,ffi.sizeof(module.tmemory.address))
            fcommon.InformationTooltip("You can simply paste copied address using Ctrl + V")
            imgui.SliderInt("Size", module.tmemory.size,1,4)

            if module.tmemory.size[0] == 4 then
                imgui.Columns(3,nil,false)
                imgui.Checkbox("Float",module.tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(2,nil,false)
            end


            imgui.Checkbox("Virtual protect", module.tmemory.vp)
            imgui.NextColumn()
            imgui.Checkbox("Clear entries", module.tmemory.clear_entries)
            imgui.Columns(1)

            if imgui.Button("Write",imgui.ImVec2(fcommon.GetSize(1))) then
                if ffi.string(module.tmemory.address) ~= "" then
                    fcommon.RwMemory(tonumber(ffi.string(module.tmemory.address)),module.tmemory.size[0],module.tmemory.value[0],module.tmemory.vp[0],module.tmemory.is_float[0])
                    if module.tmemory.clear_entries[0] == true then
                        module.tmemory.value[0] = 0
                        imgui.StrCopy(module.tmemory.address,"")
                        module.tmemory.size[0] = 0
                        module.tmemory.vp[0] = false
                        module.tmemory.is_float[0] = false
                    end
                    printHelpString("Value ~g~Updated")
                end
            end

            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
