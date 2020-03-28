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

-- Please don't remove credits below.

local module = {}

module.tstat      =
{    
    filter        = imgui.ImGuiTextFilter(),
    names         = fcommon.LoadJson("stat"),
}     

-- Main function
function module.StatMain()

    if imgui.Button("Max vehicle stats",imgui.ImVec2(fcommon.GetSize(2))) then
        callFunction(0x4399D0,1,1,false)
        displayNonMinigameHelpMessages(false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Max weapon stats",imgui.ImVec2(fcommon.GetSize(2))) then
        for i=69,79,1 do
            setFloatStat(i,1000)
        end
        fcommon.CheatActivated()
    end
    
    fcommon.Tabs("Stats",{"Vehicle","Weapon","Girlfriend","Search"},{
        function()
            fcommon.UpdateStat({ name = "Bike",stat = 229})
            fcommon.UpdateStat({ name = "Cycling",stat = 230})
            fcommon.UpdateStat({ name = "Driving",stat = 160})
            fcommon.UpdateStat({ name = "Flying",stat = 223})  
        end,
        function()
            fcommon.UpdateStat({ name = "AK47",stat = 77})
            fcommon.UpdateStat({ name = "Combat shotgun",stat = 74})
            fcommon.UpdateStat({ name = "Desert eagle",stat = 71})
            
            fcommon.UpdateStat({ name = "Machine pistol",stat = 75})
            fcommon.UpdateStat({ name = "M4",stat = 78})
            fcommon.UpdateStat({ name = "Pistol",stat = 69})
            fcommon.UpdateStat({ name = "Sawn off shotgun",stat = 73})
            fcommon.UpdateStat({ name = "Shotgun",stat = 72})
            fcommon.UpdateStat({ name = "Silenced pistol",stat = 70})
            fcommon.UpdateStat({ name = "SMG",stat = 76})
            fcommon.UpdateStat({ name = "Rifle",stat = 79})
        end,
        function()
            if imgui.Button("Max GF progress",imgui.ImVec2(fcommon.GetSize(1))) then
                for i=252,257,1 do
                    setFloatStat(i,100)
                end
                clearHelp()
                fcommon.CheatActivated()
            end
            imgui.Spacing()
            fcommon.UpdateStat({ name = "Barbara",stat = 255,max = 100})
            fcommon.UpdateStat({ name = "Denise",stat = 252,max = 100})
            fcommon.UpdateStat({ name = "Helena",stat = 254,max = 100})
            fcommon.UpdateStat({ name = "Katie",stat = 256,max = 100})
            fcommon.UpdateStat({ name = "Michelle",stat = 253,max = 100})
            fcommon.UpdateStat({ name = "Millie",stat = 257,max = 100})
        end,
        function()
            module.tstat.filter:Draw("Filter")
            imgui.Spacing()

            if imgui.BeginChild("Stat Entries") then
                for snum, sname in pairs(module.tstat.names) do
                    if snum ~= "" then
                        if module.tstat.filter:PassFilter(sname) then
                            fcommon.UpdateStat({ name = sname,stat = snum})
                        end
                    end
                end
                imgui.Spacing()
                imgui.EndChild()
            end
        end
    })
end

return module
