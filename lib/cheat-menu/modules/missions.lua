-- This module contains all stff regarding missons column

local module = {}

local tmissions =
{
    search_text = imgui.new.char[64](fconfig.get('tmissions.search_text',"")),
    names       = ftables.missions.table,
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
            printHelpString(flanguage.GetText('missions.AlreadyInAMission'))
        end
    end
end

function module.MissionsMain()
    imgui.Spacing()
    if imgui.BeginChild("Missions list") then
        if imgui.BeginTabBar("MissionsList") then
            if imgui.BeginTabItem("LS") then
                imgui.Spacing()
                MissionEntry(flanguage.GetText('missions.BigSmoke'),{27,28,29,30})
                MissionEntry(flanguage.GetText('missions.Catalina'),{40})
                MissionEntry(flanguage.GetText('missions.CesarVialpando'),{36,45,48})
                MissionEntry(flanguage.GetText('missions.FrankTenpenny'),{22,23,39})
                MissionEntry(flanguage.GetText('missions.Introduction'),{11,12})
                MissionEntry(flanguage.GetText('missions.OGLoc'),{31,32,33,34})
                MissionEntry(flanguage.GetText('missions.Robbery'),{41,42,43,44})
                MissionEntry(flanguage.GetText('missions.Ryder'),{24,25,26})
                MissionEntry(flanguage.GetText('missions.Sweet'),{13,14,15,16,17,18,19,20,21,37,38})
                MissionEntry(flanguage.GetText('missions.TheTruth'),{46,47})
                imgui.EndTabItem()
        end
            if imgui.BeginTabItem(flanguage.GetText('missions.SF')) then
                imgui.Spacing()
                MissionEntry(flanguage.GetText('missions.CarlJohnson'),{49,50,51})
                MissionEntry(flanguage.GetText('missions.FrankTenpenny'),{52})
                MissionEntry(flanguage.GetText('missions.LocoSyndicate'),{58,59,60,61,62,63,64,65,66})
                MissionEntry(flanguage.GetText('missions.WangCars'),{67,68,69,70,71})
                MissionEntry(flanguage.GetText('missions.WuZiMu'),{53,54,55,56,57})
                MissionEntry(flanguage.GetText('missions.Zero'),{72,73,74})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem(flanguage.GetText('missions.LV')) then
                imgui.Spacing()
                MissionEntry(flanguage.GetText('missions.CaligulasCasino'),{89,90,91,92})
                MissionEntry(flanguage.GetText('missions.FrankTenpenny'),{93,94})
                MissionEntry(flanguage.GetText('missions.Heist'),{96,97,98,99,100,101})
                MissionEntry(flanguage.GetText('missions.MaddDogg'),{95})
                MissionEntry(flanguage.GetText('missions.TheFourDragonsCasino'),{84,85,86,87,88,102})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem(flanguage.GetText('missions.Desert')) then
                imgui.Spacing()
                MissionEntry(flanguage.GetText('missions.MikeToreno'),{75,76,77,78})
                MissionEntry(flanguage.GetText('missions.VerdantMeadowsAirstrip'),{79,80,81,82,83})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem(flanguage.GetText('missions.BackToLS')) then
                imgui.Spacing()
                MissionEntry(flanguage.GetText('missions.CarlJohnson'),{103,104,105})
                MissionEntry(flanguage.GetText('missions.Riot'),{108,109,110,111,112})
                MissionEntry(flanguage.GetText('missions.Sweet'),{106,107})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem(flanguage.GetText('missions.Others')) then
                imgui.Spacing()
                MissionEntry(flanguage.GetText('missions.ArenaMissions'),{128,129})
                MissionEntry(flanguage.GetText('missions.GYMMissions'),{114,115,116})
                MissionEntry(flanguage.GetText('missions.Miscellaneous'),{113,117,118,119,120,130,131,132,133,134})
                MissionEntry(flanguage.GetText('missions.SubMissions'),{121,122,123,124,125,126,127})
                MissionEntry(flanguage.GetText('missions.VideoGames'),{3,4,5,6,7,8,9,10})
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem(flanguage.GetText('common.Search')) then
                imgui.Spacing()
                imgui.Columns(1)
                if imgui.InputText(flanguage.GetText('common.Search'),tmissions.search_text,ffi.sizeof(tmissions.search_text)) then end
                imgui.SameLine()

                imgui.Spacing()
                imgui.Text(flanguage.GetText('common.FoundEntries') .. ":(" .. ffi.string(tmissions.search_text) .. ")")
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
