-- This module contains all stff regarding missons column

local module = {}

local tmissions =
{
    search_text = imgui.new.char[64](fconfig.get('tmissions.search_text',"")),
    names       = {

        [0]	    = "Initial 1",
        [1]	    = "Initial 2",
        [2]	    = "Intro",
        [3]	    = "They Crawled From Uranus",
        [4]	    = "Dualuty",
        [5]	    = "Go Go Space Monkey",
        [6]	    = "Lets Get Ready To Bumble",
        [7]	    = "Inside Track Betting",
        [8]	    = "Pool",
        [9]	    = "Lowrider (Bet And Dance)",
        [10]	= "Beefy Baron",
        [11]	= "Big Smoke",
        [12]	= "Ryder",
        [13]	= "Tagging Up Turf",
        [14]	= "Cleaning The Hood",
        [15]	= "Drive-Thru",
        [16]	= "Nines And AKs",
        [17]	= "Drive-By",
        [18]	= "Sweets Girl",
        [19]	= "Cesar Vialpando",
        [20]	= "Los Sepulcros",
        [21]	= "Doberman",
        [22]	= "Burning Desire",
        [23]	= "Gray Imports",
        [24]	= "Home Invasion",
        [25]	= "Catalyst",
        [26]	= "Robbing Uncle Sam",
        [27]	= "OG Loc",
        [28]	= "Running Dog",
        [29]	= "Wrong Side of the Tracks",
        [30]	= "Just Business",
        [31]	= "Lifes a Beach",
        [32]	= "Madd Doggs Rhymes",
        [33]	= "Management Issues",
        [34]	= "House Party",
        [35]	= "Race Tournament / 8-track / Dirt Track",
        [36]	= "Lowrider (High Stakes)",
        [37]	= "Reuniting The Families",
        [38]	= "The Green Sabre",
        [39]	= "Badlands",
        [40]	= "First Date",
        [41]	= "Local Liquor Store",
        [42]	= "Small Town Bank",
        [43]	= "Tanker Commander",
        [44]	= "Against All Odds",
        [45]	= "King in Exile",
        [46]	= "Body Harvest",
        [47]	= "Are you going to San Fierro?",
        [48]	= "Wu Zi Mu / Farewell, My Love...",
        [49]	= "Wear Flowers In Your Hair",
        [50]	= "Deconstruction",
        [51]	= "555 WE TIP",
        [52]	= "Snail Trail",
        [53]	= "Mountain Cloud Boys",
        [54]	= "Ran Fa Li",
        [55]	= "Lure",
        [56]	= "Amphibious Assault",
        [57]	= "The Da Nang Thang",
        [58]	= "Photo Opportunity",
        [59]	= "Jizzy",
        [60]	= "Outrider",
        [61]	= "Ice Cold Killa",
        [62]	= "Torenos Last Flight",
        [63]	= "Yay Ka-Boom-Boom",
        [64]	= "Pier 69",
        [65]	= "T-Bone Mendez",
        [66]	= "Mike Toreno",
        [67]	= "Zeroing In",
        [68]	= "Test Drive",
        [69]	= "Customs Fast Track",
        [70]	= "Puncture Wounds",
        [71]	= "Back to School",
        [72]	= "Air Raid",
        [73]	= "Supply Lines",
        [74]	= "New Model Army",
        [75]	= "Monster",
        [76]	= "Highjack",
        [77]	= "Interdiction",
        [78]	= "Verdant Meadows",
        [79]	= "N.O.E.",
        [80]	= "Stowaway",
        [81]	= "Black Project",
        [82]	= "Green Goo",
        [83]	= "Learning to Fly",
        [84]	= "Fender Ketchup",
        [85]	= "Explosive Situation",
        [86]	= "Youve Had Your Chips",
        [87]	= "Fish in a Barrel",
        [88]	= "Don Peyote",
        [89]	= "Intensive Care",
        [90]	= "The Meat Business",
        [91]	= "Freefall",
        [92]	= "Saint Marks Bistro",
        [93]	= "Misappropriation",
        [94]	= "High Noon",
        [95]	= "Madd Dogg",
        [96]	= "Architectural Espionage",
        [97]	= "Key To Her Heart",
        [98]	= "Dam And Blast",
        [99]	= "Cop Wheels",
        [100]	= "Up, Up and Away!",
        [101]	= "Breaking the Bank at Caligulas",
        [102]	= "A Home In The Hills",
        [103]	= "Vertical Bird",
        [104]	= "Home Coming",
        [105]	= "Cut Throat Business",
        [106]	= "Beat Down on B Dup",
        [107]	= "Grove 4 Life",
        [108]	= "Riot",
        [109]	= "Los Desperados",
        [110]	= "End Of The Line (1)",
        [111]	= "End Of The Line (2)",
        [112]	= "End Of The Line (3)",
        [113]	= "Shooting range",
        [114]	= "Los Santos Gym Fight School",
        [115]	= "San Fierro Gym Fight School",
        [116]	= "Las Venturas Gym Fight School",
        [117]	= "Trucking",
        [118]	= "Quarry",
        [119]	= "Boat School",
        [120]	= "Bike School",

        -- Sub-Missions
        [121]	= "Taxi-Driver",
        [122]	= "Paramedic",
        [123]	= "Firefighter",
        [124]	= "Vigilante",
        [125]	= "Burglary",
        [126]	= "Freight Train",
        [127]	= "Pimping",
        [128]	= "Blood Ring",
        [129]	= "Kickstart",
        [130]	= "Beat the Cock!",
        [131]	= "Courier",
        [132]	= "The Chiliad Challenge",
        [133]	= "BMX / NRG-500 STUNT Mission",
        [134]	= "Buy Properties Mission",
    },
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
