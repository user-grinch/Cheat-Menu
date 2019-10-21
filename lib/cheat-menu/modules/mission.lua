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

module.tmission =
{
    array       = {},
    names       = fcommon.LoadJson("mission"),
    search_text = imgui.new.char[20](),
}

for id, name in pairs(module.tmission.names) do
    if name ~= "" then
        table.insert(module.tmission.array,tonumber(id))
    end
end

function ShowMissionEntries(title,list,search_text)
    if search_text == nil then search_text = "" end

    fcommon.DropDownMenu(title,function()
        imgui.Spacing()
        for _,i in pairs(list) do
            if (ffi.string(search_text) == "") or ((string.upper(module.tmission.names[tostring(i)])):find(string.upper(ffi.string(search_text))) ~= nil) then
                if imgui.MenuItemBool(module.tmission.names[tostring(i)]) then
                    if getGameGlobal(glob.ONMISSION) == 0 then
                        clearWantedLevel(PLAYER_HANDLE)
                        lockPlayerControl(true)
                        doFade(true,1000)
                        loadAndLaunchMissionInternal(i)
                        lockPlayerControl(false)
                    else
                        printHelpString('Already in a mission')
                    end
                end
            end
        end
    end)
end

function module.MissionMain()
    imgui.Spacing()

    if imgui.Button("Fail current mission",imgui.ImVec2(fcommon.GetSize(1))) then
        if isPlayerControlOn(PLAYER_HANDLE) then
            failCurrentMission()
            fcommon.CheatActivated()
        end
    end
    imgui.Spacing()
    if imgui.BeginChild("Missions list") then
        if imgui.BeginTabBar("MissionsList") then
            if imgui.BeginTabItem("LS") then
                imgui.Spacing()
                ShowMissionEntries('Big smoke',{27,28,29,30})
                ShowMissionEntries('Catalina',{40})
                ShowMissionEntries('Cesar vialpando',{45})
                ShowMissionEntries('Frank tenpenny',{22,23,39})
                ShowMissionEntries('Introduction',{11,12})
                ShowMissionEntries('OG loc',{31,32,33,34})
                ShowMissionEntries('Robbery',{41,42,43,44})
                ShowMissionEntries('Ryder',{24,25,26})
                ShowMissionEntries('Sweet',{13,14,15,16,17,18,19,20,21,37,38})
                ShowMissionEntries('The truth',{46,47})
                imgui.EndTabItem()
        end
            if imgui.BeginTabItem('SF') then
                imgui.Spacing()
                ShowMissionEntries('Carl johnson',{49,50,51})
                ShowMissionEntries('Frank tenpenny',{52})
                ShowMissionEntries('Loco syndicate',{58,59,60,61,62,63,64,65,66})
                ShowMissionEntries('Wang cars',{67,68,69,70,71})
                ShowMissionEntries('WuZiMu',{53,54,55,56,57})
                ShowMissionEntries('Zero',{72,73,74})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('LV') then
                imgui.Spacing()
                ShowMissionEntries('Caligulas casino',{89,90,91,92})
                ShowMissionEntries('Frank tenpenny',{93,94})
                ShowMissionEntries('Heist',{96,97,98,99,100,101})
                ShowMissionEntries('Madd dogg',{95})
                ShowMissionEntries('The four dragons casino',{84,85,86,87,88,102})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Desert') then
                imgui.Spacing()
                ShowMissionEntries('Mike toreno',{75,76,77,78})
                ShowMissionEntries('Verdant meadows airstrip',{79,80,81,82,83})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Back to LS') then
                imgui.Spacing()
                ShowMissionEntries('Carl johnson',{103,104,105})
                ShowMissionEntries('Riot',{108,109,110,111,112})
                ShowMissionEntries('Sweet',{106,107})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Others') then
                imgui.Spacing()
                ShowMissionEntries('Arena missions',{128,129,130})
                ShowMissionEntries('Miscellaneous',{117,118})
                ShowMissionEntries('Video games',{3,4,5,6,7,9,10})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Search') then
                imgui.Spacing()
                imgui.Columns(1)
                if imgui.InputText('Search ',module.tmission.search_text,ffi.sizeof(module.tmission.search_text)) then 
                end
                imgui.Spacing()
                imgui.Text("Found missions :(" .. ffi.string(module.tmission.search_text) .. ")")
                imgui.Separator()
                imgui.Spacing()
                if imgui.BeginChild("MissionsEntries") then
                    ShowMissionEntries(nil,module.tmission.array,module.tmission.search_text)
                    imgui.EndChild()
                end
                imgui.EndTabItem()
            end
            imgui.EndTabBar()
        end
        imgui.EndChild()
    end
end
return module
