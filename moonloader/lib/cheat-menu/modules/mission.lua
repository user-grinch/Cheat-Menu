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

module.tmission =
{
    names       = fcommon.LoadJson("mission"),
}

-- Main function
function module.MissionMain()
    imgui.Spacing()

    if imgui.Button("Fail current mission",imgui.ImVec2(fcommon.GetSize(1))) then
        if isPlayerControlOn(PLAYER_HANDLE) then
            failCurrentMission()
            fcommon.CheatActivated()
        end
    end

    imgui.Dummy(imgui.ImVec2(0,10))

    fcommon.DrawEntries(fconst.IDENTIFIER.MISSION,fconst.DRAW_TYPE.TEXT,function(key,val)
        if getCharActiveInterior(PLAYER_PED) == 0 then
            if getGameGlobal(glob.ONMISSION) == 0 then
                lockPlayerControl(true)
                doFade(true,1000)
                clearWantedLevel(PLAYER_HANDLE)
                loadAndLaunchMissionInternal(tonumber(key))
                printString(tostring(key),100)
                lockPlayerControl(false)
            else
                printHelpString('Already in a mission')
            end
        else
            printHelpString("Can't start mission inside interior")
        end
    end,
    nil,function(a) return a end,module.tmission.names)
    
end
return module
