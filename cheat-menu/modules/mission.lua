-- This module contains all stff regarding missons column

local module = {}

local missions = {

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
}

function mission_menu(title,list)
    if imgui.BeginMenu(title) then
        imgui.Text(title)
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
        for _,i in ipairs(list) do
            if imgui.MenuItem(missions[i]) then
                if getGameGlobal(glob.ONMISSION) == 1 then
                    setGameGlobal(glob.ONMISSION,0)
                    failCurrentMission()
                end
                clearWantedLevel(PLAYER_HANDLE)
                lockPlayerControl(true)
                setLaRiots(false)
                local progress = getProgressPercentage()
                playerMadeProgress(100)
                loadAndLaunchMissionInternal(i)
                lockPlayerControl(false)
                playerMadeProgress(progress)
                fcommon.CheatActivated() 
            end
        end
        imgui.EndMenu()
    end
end

function module.missions_section()
    imgui.Text("Missons")
    imgui.Spacing()
    imgui.Separator()
    imgui.Spacing()
    if imgui.Button("Abort Current Misson",imgui.ImVec2(fcommon.getsize(1),50)) then
        if getGameGlobal(glob.ONMISSION) == 1 then
            failCurrentMission()
            setGameGlobal(glob.ONMISSION,0)
            printBig('M_FAIL',5000,1)
        else 
            printHelpString("Player is not in a mission.")
        end  
    end
    imgui.Spacing()
    imgui.Text("All missoins")
    imgui.Separator()
    if imgui.BeginMenu("Los Santos") then
        imgui.Text("Los Santos Missions")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
        mission_menu("Introduction",{11,12})
        mission_menu("Sweet",{13,14,15,16,17,18,19,20,21,37,38})
        mission_menu("Big Smoke",{27,28,29,30})
        mission_menu("Ryder",{24,25,26})
        mission_menu("Cesar Vialpando",{36,45,48})
        mission_menu("OG Loc",{31,32,33,34})
        mission_menu("Frank Tenpenny",{22,23,39})
        mission_menu("Catalina",{40})
        mission_menu("The Truth",{46,47})
        mission_menu("Robbery",{41,42,43,44})
        imgui.EndMenu()
    end
    if imgui.BeginMenu("San Fierro") then
        imgui.Text("San Fierro Missions")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
        mission_menu("Carl Johnson",{49,50,51})
        mission_menu("Zero",{72,73,74})
        mission_menu("Loco Syndicate",{58,59,60,61,62,63,64,65,66})
        mission_menu("Wu Zi Mu",{53,54,55,56,57})
        mission_menu("Frank Tenpenny",{52})
        mission_menu("Wang Cars",{67,68,69,70,71})
        imgui.EndMenu()
    end
    if imgui.BeginMenu("Desert") then
        imgui.Text("Desert Missions")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
        mission_menu("Mike Toreno",{75,76,77,78})
        mission_menu("Verdant Meadows Airstrip",{79,80,81,82,83})
        imgui.EndMenu()
    end
    if imgui.BeginMenu("Las Venturas") then
        imgui.Text("Las Venturas Missions")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
        mission_menu("The Four Dragons Casino",{84,85,86,87,88,102})
        mission_menu("Heist",{96,97,98,99,100,101})
        mission_menu("Caligula's Casino",{89,90,91,92})
        mission_menu("Frank Tenpenny",{93,94})
        mission_menu("Madd Dogg",{95})
        imgui.EndMenu()
    end
    if imgui.BeginMenu("Back to Los Santos") then
        imgui.Text("Back to Los Santos Missions")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
        mission_menu("Carl Johnson",{103,104,105})
        mission_menu("Sweet",{106,107})
        mission_menu("Riot",{108,109,110,111,112})
        imgui.EndMenu()
    end
    mission_menu("GYM Missions",{114,115,116})
    mission_menu("Sub Missions",{121,122,123,124,125,126,127})
    mission_menu("Arena Missions",{128,129})
    mission_menu("Miscellaneous",{113,117,118,119,120,130,131,132,133,134})
    mission_menu("Video Games",{3,4,5,6,7,8,9,10})
end
return module