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

local tmissions =
{
    search_text = imgui.new.char[64](fconfig.get('tmissions.search_text',"")),
    names       = ftable.missions.table,
    list = {},
}

for i = 0,#tmissions.names,1 do
    table.insert(tmissions.list,i)
end

module.tmissions = tmissions

function MissionEntry(title,list,search_text)
    if search_text == nil then search_text = "" end

    fcommon.DropDownMenu(title,function()
        imgui.Spacing()
        for _,i in pairs(list) do
            if (ffi.string(search_text) == "") or ((string.upper(tmissions.names[i])):find(string.upper(ffi.string(search_text))) ~= nil) then
                ShowMissionEntry(i)
            end
        end
    end)
end

function ShowMissionEntry(i)
    if imgui.MenuItemBool(tmissions.names[i]) then
        if getGameGlobal(glob.ONMISSION) == 0 then
            clearWantedLevel(PLAYER_HANDLE)
            lockPlayerControl(true)
            setEveryoneIgnorePlayer(PLAYER_HANDLE,true)
            displayHud(false)
            displayRadar(false)
            setLaRiots(false)
            doFade(true,1000)
            local progress = getProgressPercentage()
            setProgressTotal(100)
            loadAndLaunchMissionInternal(i)
            lockPlayerControl(false)
            displayHud(true)
            displayRadar(true)
            playerMadeProgress(progress)
            fcommon.CheatActivated()
        else
            printHelpString('Already in a mission')
        end
    end
end

function module.MissionMain()
    imgui.Spacing()
    if imgui.BeginChild("Missions list") then
        if imgui.BeginTabBar("MissionsList") then
            if imgui.BeginTabItem("LS") then
                imgui.Spacing()
                MissionEntry('Big smoke',{27,28,29,30})
                MissionEntry('Catalina',{40})
                MissionEntry('Cesar vialpando',{36,45,48})
                MissionEntry('Frank tenpenny',{22,23,39})
                MissionEntry('Introduction',{11,12})
                MissionEntry('OG loc',{31,32,33,34})
                MissionEntry('Robbery',{41,42,43,44})
                MissionEntry('Ryder',{24,25,26})
                MissionEntry('Sweet',{13,14,15,16,17,18,19,20,21,37,38})
                MissionEntry('The truth',{46,47})
                imgui.EndTabItem()
        end
            if imgui.BeginTabItem('SF') then
                imgui.Spacing()
                MissionEntry('Carl johnson',{49,50,51})
                MissionEntry('Frank tenpenny',{52})
                MissionEntry('Loco syndicate',{58,59,60,61,62,63,64,65,66})
                MissionEntry('Wang cars',{67,68,69,70,71})
                MissionEntry('WuZiMu',{53,54,55,56,57})
                MissionEntry('Zero',{72,73,74})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('LV') then
                imgui.Spacing()
                MissionEntry('Caligulas casino',{89,90,91,92})
                MissionEntry('Frank tenpenny',{93,94})
                MissionEntry('Heist',{96,97,98,99,100,101})
                MissionEntry('Madd dogg',{95})
                MissionEntry('The four dragons casino',{84,85,86,87,88,102})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Desert') then
                imgui.Spacing()
                MissionEntry('MikeToreno',{75,76,77,78})
                MissionEntry('Verdant meadows airstrip',{79,80,81,82,83})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Back to LS') then
                imgui.Spacing()
                MissionEntry('Carl johnson',{103,104,105})
                MissionEntry('Riot',{108,109,110,111,112})
                MissionEntry('Sweet',{106,107})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Others') then
                imgui.Spacing()
                MissionEntry('Arena missions',{128,129})
                MissionEntry('GYM missions',{114,115,116})
                MissionEntry('Miscellaneous',{113,117,118,119,120,130,131,132,133,134})
                MissionEntry('Sub missions',{121,122,123,124,125,126,127})
                MissionEntry('Video games',{3,4,5,6,7,8,9,10})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Search') then
                imgui.Spacing()
                imgui.Columns(1)
                if imgui.InputText('Search',tmissions.search_text,ffi.sizeof(tmissions.search_text)) then end
                imgui.SameLine()

                imgui.Spacing()
                imgui.Text("FoundEntries :(" .. ffi.string(tmissions.search_text) .. ")")
                imgui.Separator()
                imgui.Spacing()
                if imgui.BeginChild("MissionsEntries") then
                    MissionEntry(nil,tmissions.list,tmissions.search_text)
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
