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

module.tvisual =
{
    car_names  = imgui.new.bool(true),
    money =
    {
        negative = imgui.new.char[20](memory.tostring(readMemory(0x58F50A,4,false))),
        positive = imgui.new.char[20](memory.tostring(readMemory(0x58F4C8,4,false))),
    },
    zone_names = imgui.new.bool(true),
}

-- Main function
function module.VisualMain()
    if imgui.BeginTabBar("Visual") then
        imgui.Spacing()
        if imgui.BeginTabItem('Checkbox') then
            imgui.Spacing()

            imgui.Columns(2,nil,false)
            fcommon.CheckBoxValue('Body armour border',0x589123)
            fcommon.CheckBoxValue('Body armour percentage',0x589125)
            fcommon.CheckBoxValue('Breath meter border',0x589207)
            fcommon.CheckBoxValue('Breath meter percentage',0x589209)
            fcommon.CheckBoxFunc('Display zone names',module.tvisual.zone_names,
            function()     
                if module.tvisual.zone_names[0] then 
                    displayZoneNames(true)
                    fcommon.CheatActivated() 
                else 
                    displayZoneNames(false)
                    fcommon.CheatDeactivated() 
                end
            end)
            fcommon.CheckBoxFunc('Display car names',module.tvisual.car_names,
            function()     
                if module.tvisual.car_names[0] then 
                    displayCarNames(true)
                    fcommon.CheatActivated() 
                else 
                    displayCarNames(false)
                    fcommon.CheatDeactivated() 
                end
            end)

            imgui.NextColumn()

            fcommon.CheckBoxValue('Enable hud',0xBA6769)
            fcommon.CheckBoxValue('Enable radar',0xBA676C,nil,0,2)
            fcommon.CheckBoxValue('Gray radar',0xA444A4)
            fcommon.CheckBoxValue('Health meter border',0x589353)
            fcommon.CheckBoxValue('Health bar percentage',0x589355)
            fcommon.CheckBoxValue('Hide wanted level',0x58DD1B,nil,0x90)
            imgui.Columns(1)
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Menu') then
            if imgui.BeginChild("") then
                imgui.Spacing()

                fcommon.UpdateAddress({ name = 'Active wanted star posY',address = 0x858CCC,size = 4,is_float = true,min=-500,default = 12,max = 500})
                fcommon.UpdateAddress({ name = 'Debt color',address = 0x58F4D4 ,size = 1,min = 0,default = 0,max = 9})
                fcommon.UpdateAddress({ name = 'Money color',address = 0x58F492 ,size = 1,min = 0,default = 1,max = 9})
                fcommon.UpdateAddress({ name = 'Money font outline',address = 0x58F58D,size = 1,min=0,default = 2,max = 2})
                fcommon.UpdateAddress({ name = 'Money font style',address = 0x58F57F ,size = 1,min=0,default = 3,max = 3})
                fcommon.DropDownMenu("Money text format",function()
                    if imgui.InputText("Positive",module.tvisual.money.positive,ffi.sizeof(module.tvisual.money.positive)) then 
                        local var = allocateMemory(4)
                        ffi.copy(ffi.cast("char*", var), ffi.string(module.tvisual.money.positive))
                        writeMemory(0x58F4C8,4,var,false)
                    end
                    if imgui.InputText("Negative",module.tvisual.money.negative,ffi.sizeof(module.tvisual.money.negative)) then
                        local var = allocateMemory(4)
                        ffi.copy(ffi.cast("char*", var), ffi.string(module.tvisual.money.negative))
                        writeMemory(0x58F50A,4,var,false)
                    end
                    
                    if imgui.Button("Reset to default",imgui.ImVec2(fcommon.GetSize(1))) then
                        local var = allocateMemory(4)
                        local var2 = allocateMemory(4)
                        ffi.copy(ffi.cast("char(*)", var), "-$%07d")
                        imgui.StrCopy(module.tvisual.money.negative,"-$%07d",ffi.sizeof(module.tvisual.money.negative))
                        writeMemory(0x58F50A,4,var,false)
                        ffi.copy(ffi.cast("char(*)", var2), "$%08d")
                        imgui.StrCopy(module.tvisual.money.positive,"$%08d",ffi.sizeof(module.tvisual.money.positive))
                        writeMemory(0x58F4C8,4,var2,false)
                    end                    

                end)
                fcommon.UpdateAddress({ name = 'Radar X position',address = 0x858A10,size = 4,min=-999,default = 40,max = 999,is_float = true,help_text = "Changes radar vertical position"})
                fcommon.UpdateAddress({ name = 'Radar Y position',address = 0x866B70,size = 4,min=-999,default = 104,max = 999,is_float = true,help_text = "Changes radar horizantal position"})
                fcommon.UpdateAddress({ name = 'Wanted star border',address = 0x58DD41,size = 1,min = 0,default = 1,max = 3})
                fcommon.UpdateAddress({ name = 'Wanted star color',address = 0x58DDC9,size = 1,max = 9})
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
