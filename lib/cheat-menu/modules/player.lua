-- This module contains all the fuction for player tab

local module = {}

local tplayer =
{
    god            = imgui.new.bool(fconfig.get('tplayer.god') or false),
    aimSkinChanger = imgui.new.bool(fconfig.get('tplayer.aimSkinChanger') or false),
    neverWanted    = (fconfig.get('tplayer.neverWanted') or false),
    cjBody         = imgui.new.int(fconfig.get('tplayer.cjBody') or 0),
    skins =
    {
        search_text    = imgui.new.char[20](fconfig.get('tplayer.skins.search_text') or ""),
    },
    style =
    {
        fighting =
        {
            selected = imgui.new.int(fconfig.get('tplayer.style.fighting.selected') or 0),
            names    = {"Default","Boxing","Kung fu","Kick Boxing","Punch Kick"},
            list     = {},
        },
        walking =
        {
            selected = imgui.new.int(fconfig.get('tplayer.style.walking.selected') or 0),
            names    = {"man","shuffle","oldman","gang1","gang2","oldfatman","fatman","jogger","drunkman","blindman","swat","woman","shopping","busywoman","sexywoman","pro","oldwoman","fatwoman","jogwoman","oldfatwoman","skate"},
            list     = {},
        },
    },
    stats =
    {
        search_text = imgui.new.char[20](fconfig.get('tplayer.stats.search_text') or ""),
        names =
        {
            [0]   = "Progress made",
            [1]	  = "Total progress",
            [2]	  = "Furthest Hoop",
            [3]	  = "Distance travelled on foot",
            [4]	  = "Distance travelled by car",
            [5]	  = "Distance travelled by motorbike",
            [6]	  = "Distance travelled by boat",
            [7]	  = "Distance travelled by golf cart",
            [8]	  = "Distance travelled by helicopter",
            [9]	  = "Distance travelled by plane",
            [10]  = "Longest Wheelie distance",
            [11]  = "Longest Stoppie distance",
            [12]  = "Longest 2 wheels distance",
            [13]  = "Weapon Budget",
            [14]  = "Fashion Budget",
            [15]  = "Property Budget",
            [16]  = "Auto Repair and Painting Budget",
            [17]  = "Longest Wheelie time",
            [18]  = "Longest Stoppie time",
            [19]  = "Longest 2 wheels time",
            [20]  = "Food Budget",
            [21]  = "Fat",
            [22]  = "Stamina",
            [23]  = "Muscle",
            [24]  = "Max Health",
            [25]  = "Sex appeal",
            [26]  = "Distance travelled by swimming",
            [27]  = "Distance travelled by bicycle",
            [28]  = "Distance travelled on treadmill",
            [29]  = "Distance travelled on exercise bike",
            [30]  = "Tattoo budget",
            [31]  = "Hairdressing budget",
            [32]  = "Girlfriend budget",
            [33]  = "Prostitute budget",
            [34]  = "Furniture budget",
            [35]  = "Money spent gambling",
            [36]  = "Money made from pimping",
            [37]  = "Money won gambling",
            [38]  = "Biggest gambling win",
            [39]  = "Biggest gambling loss",
            [40]  = "Largest burglary swag",
            [41]  = "Money made from burglary",
            [42]  = "Money spent building property",
            [43]  = "unused",
            [44]  = "Longest treadmill time",
            [45]  = "Longest exercise bike time",
            [46]  = "Heaviest weight on bench press",
            [47]  = "Heaviest weight on dumbbells",
            [48]  = "Best time in 8-Track",
            [49]  = "BMX best time",
            [50]  = "Lightest weight",
            [51]  = "Longest chase time with 5 or more stars",
            [52]  = "Last chase time with 5 or more stars",
            [53]  = "Wage bill",
            [54]  = "Strip club budget",
            [55]  = "Car modification budget",
            [56]  = "Time spent shopping",
            [57]  = "Time spent gambling",
            [58]  = "Time spent on longest mission",
            [59]  = "Time spent on quickest mission",
            [60]  = "Average mission time",
            [61]  = "Drugs budget",
            [62]  = "Total shopping budget",
            [63]  = "Time spent underwater",
            [64]  = "Total respect",
            [65]  = "Girlfriend respect",
            [66]  = "Clothes respect",
            [67]  = "Fitness respect",
            [68]  = "Respect",
            [69]  = "Pistol Skill",
            [70]  = "Silenced Pistol Skill",
            [71]  = "Desert Eagle Skill",
            [72]  = "Shotgun Skill",
            [73]  = "Sawn-Off Shotgun Skill",
            [74]  = "Combat Shotgun Skill",
            [75]  = "Machine Pistol Skill",
            [76]  = "SMG Skill",
            [77]  = "AK-47 Skill",
            [78]  = "M4 Skill",
            [79]  = "Rifle Skill",
            [80]  = "Appearance",
            [81]  = "Gambling",
            [120] =	"People wasted by others",
            [121] =	"People you've wasted",
            [122] =	"Road Vehicles destroyed",
            [123] =	"Boats destroyed",
            [124] =	"Planes & Helicopters destroyed",
            [125] =	"Cost of property damaged",
            [126] =	"Bullets fired",
            [127] =	"Kgs of explosives used",
            [128] =	"Bullets that hit",
            [129] =	"Tires popped with gunfire",
            [130] =	"Number of headshots",
            [131] =	"Total number of wanted stars attained",
            [132] =	"Total number of wanted stars evaded",
            [133] =	"Times busted",
            [134] =	"Days passed in game",
            [135] =	"Number of hospital visits",
            [136] =	"Safehouse visits",
            [137] =	"Times cheated",
            [138] =	"Vehicle Resprays",
            [139] =	"Maximum INSANE Jump distance",
            [140] =	"Maximum INSANE Jump height",
            [141] =	"Maximum INSANE Jump flips",
            [142] =	"Maximum INSANE Jump rotation",
            [143] =	"Best INSANE stunt awarded",
            [144] =	"Unique Jumps found",
            [145] =	"Unique Jumps done",
            [146] =	"Mission attempts",
            [147] =	"Missions passed",
            [148] =	"Total number of missions in game",
            [149] =	"Cash made in a Taxi",
            [150] =	"Passengers dropped off",
            [151] =	"People saved in an Ambulance",
            [152] =	"Criminals killed on Vigilante Mission",
            [153] =	"Total fires extinguished",
            [154] =	"Packages Delivered",
            [155] =	"Assassinations",
            [156] =	"Last dance score",
            [157] =	"Highest Vigilante Mission level",
            [158] =	"Highest Paramedic Mission level",
            [159] =	"Highest Firefighter Mission level",
            [160] =	"Driving skill",
            [161] =	"Number of Truck missions passed",
            [162] =	"Money made in Truck",
            [163] =	"Recruited gang members killed",
            [164] =	"Armor",
            [165] =	"Energy",
            [166] =	"Photographs Taken",
            [167] =	"Rampages attempted",
            [168] =	"Rampages passed",
            [169] =	"Flight time",
            [170] =	"Times drowned",
            [171] =	"Number of girls pimped",
            [172] =	"Best position in 8-Track",
            [173] =	"Time on jetpack",
            [174] =	"Shooting range levels passed",
            [175] =	"Most cars parked on 'Valet Parking'",
            [176] =	"Kills since last checkpoint",
            [177] =	"Total legitimate kills",
            [178] =	"Number of 'Bloodring' kills",
            [179] =	"Total time in bloodring",
            [180] =	"No more hurricanes Flag",
            [181] =	"City unlocked",
            [182] =	"Number of Police bribes",
            [183] =	"Number of cars stolen",
            [184] =	"Current number of girlfriends",
            [185] =	"Number of disastrous dates",
            [186] =	"Number of girls dated",
            [187] =	"Number of times scored with a girl",
            [188] =	"Number of successful dates",
            [189] =	"Number of girls dumped",
            [190] =	"Number of prostitutes visited",
            [191] =	"Number of houses burgled",
            [192] =	"Number of safes cracked",
            [193] =	"Burgular status",
            [194] =	"Number of stolen items sold",
            [195] =	"Number of eight balls in pool",
            [196] =	"Total number of wins playing pool",
            [197] =	"Total number of losses playing pool",
            [198] =	"Times visited the gym",
            [199] =	"Body type",
            [200] =	"Number of meals eaten",
            [201] =	"Money made in Quarry",
            [202] =	"Time taken to Complete Quarry",
            [203] =	"Number of furniture purchased",
            [204] =	"Favorite console game",
            [205] =	"Highest civilian peds killed on Rampage",
            [206] =	"Highest police peds killed on Rampage",
            [207] =	"Highest civilian vehicles destroyed on Rampage",
            [208] =	"Highest police vehicles destroyed on Rampage",
            [209] =	"Highest number of tanks destroyed on Rampage",
            [210] =	"Pimping level",
            [211] =	"Kickstart best score",
            [212] =	"Best lap time in 8-Track",
            [213] =	"Number of vehicles exported",
            [214] =	"Number of vehicles imported",
            [215] =	"Highest basketball score",
            [216] =	"Fires started",
            [217] =	"Amount of drugs sold",
            [218] =	"Amount of drugs bought",
            [219] =	"Best lap time in dirt track",
            [220] =	"Best time in dirt track",
            [221] =	"Best position in dirt track",
            [222] =	"NRG-500 best time",
            [223] =	"Flying skill",
            [224] =	"Respect Mission",
            [225] =	"Lung capacity",
            [226] =	"Game gore rating",
            [227] =	"Game sex rating",
            [228] =	"Respect Mission Total",
            [229] =	"Bike skill",
            [230] =	"Cycling skill",
            [231] =	"Snapshots taken",
            [232] =	"Total snapshots",
            [233] =	"Luck",
            [234] =	"Territories taken over",
            [235] =	"Territories lost",
            [236] =	"Territories held",
            [237] =	"Highest number of territories held",
            [238] =	"Gang members recruited",
            [239] =	"Enemy gang members killed",
            [240] =	"Friendly gang members killed",
            [241] =	"Horseshoes collected",
            [242] =	"Total horseshoes",
            [243] =	"Oysters collected",
            [244] =	"Total oysters",
            [245] =	"Calories",
            [246] =	"2 Player Rampage best time",
            [247] =	"2 Player Car Rampage best time",
            [248] =	"2 Player On Foot Rampage best time",
            [249] =	"2 Player Chopper Rampage best time",
            [250] =	"2 Player Bike Rampage best time",
            [251] =	"'Keepie Uppy' best time",
            [252] =	"Progress with Denise",
            [253] =	"Progress with Michelle",
            [254] =	"Progress with Helena",
            [255] =	"Progress with Barbara",
            [256] =	"Progress with Katie",
            [257] =	"Progress with Millie",
            [258] =	"Best position in Lowrider Race",
            [259] =	"Best time in Lowrider Race",
            [260] =	"Best position in Little Loop",
            [261] =	"Best time in Little Loop",
            [262] =	"Best position in Backroad Wanderer",
            [263] =	"Best time in Backroad Wanderer",
            [264] =	"Best position in City Circuit",
            [265] =	"Best time in City Circuit",
            [266] =	"Best position in Vinewood",
            [267] =	"Best time in Vinewood",
            [268] =	"Best position in Freeway",
            [269] =	"Best time in Freeway",
            [270] =	"Best position in Into the Country",
            [271] =	"Best time in Into the Country",
            [272] =	"Best position in Badlands A",
            [273] =	"Best time in Badlands A",
            [274] =	"Best position in Badlands B",
            [275] =	"Best time in Badlands B",
            [276] =	"Best position in Dirtbike Danger",
            [277] =	"Best time in Dirtbike Danger",
            [278] =	"Best position in Bandito County",
            [279] =	"Best time in Bandito County",
            [280] =	"Best position in Go-Go-Kart",
            [281] =	"Best time in Go-Go-Kart",
            [282] =	"Best position in San Fierro Fastlane",
            [283] =	"Best time in San Fierro Fastlane",
            [284] =	"Best position in San Fierro Hills",
            [285] =	"Best time in San Fierro Hills",
            [286] =	"Best position in Country Endurance",
            [287] =	"Best time in Country Endurance",
            [288] =	"Best position in SF to LV",
            [289] =	"Best time in SF to LV",
            [290] =	"Best position in Dam Rider",
            [291] =	"Best time in Dam Rider",
            [292] =	"Best position in Desert Tricks",
            [293] =	"Best time in Desert Tricks",
            [294] =	"Best position in LV Ringroad",
            [295] =	"Best time in LV Ringroad",
            [296] =	"Best time in World War Aces",
            [297] =	"Best time in Barnstorming",
            [298] =	"Best time in Military Service",
            [299] =	"Best time in Chopper Checkpoint",
            [300] =	"Best time in Whirly Bird Waypoint",
            [301] =	"Best time in Heli Hell",
            [302] =	"Drive-thru mission accomplished",
            [303] =	"Management Issues mission accomplished",
            [304] =	"555 WE TIP mission accomplished",
            [305] =	"Yay Ka-Boom-Boom mission accomplished",
            [306] =	"Fish in a Barrel mission accomplished",
            [307] =	"Breaking the Bank at Caligula's mission accomplished",
            [308] =	"A Home In The Hills mission accomplished",
            [309] =	"maybe set riot mode",
            [310] =	"Ryder's mission Robbing Uncle Sam accomplished",
            [311] =	"Mike Toreno mission accomplished",
            [312] =	"Architectural Espionage mission accomplished",
            [313] =	"Jizzy mission accomplished",
            [314] =	"Reuniting the Families mission accomplished",
            [315] =	"Small Town Bank mission accomplished",
            [316] =	"Photo Opportunity mission accomplished",
            [317] =	"Don Peyote mission accomplished",
            [318] =	"Local Liquor Store mission accomplished",
            [319] =	"Badlands mission accomplished",
            [320] =	"Playing time",
            [321] =	"Hidden Packages found",
            [322] =	"Tags sprayed",
            [323] =	"Least favorite gang",
            [324] =	"Gang members wasted",
            [325] =	"Criminals wasted",
            [326] =	"Most Favorite Radio Station",
            [327] =	"Least Favorite Radio Station",
            [328] =	"Current weapon skill",
            [329] =	"Weapon skill levels",
            [330] =	"Pilot ranking",
            [331] =	"Strongest Gang",
            [332] =	"2nd strongest gang",
            [333] =	"3rd strongest gang",
            [334] =	"Money lost gambling",
            [335] =	"Riot Mission accomplished",
            [336] =	"Gang strength",
            [337] =	"Territory under control",
            [338] =	"Are you going to San Fierro? mission accomplished",
            [339] =	"High Noon mission accomplished",
            [340] =	"The Green Sabre mission accomplished",
            [341] =	"maybe Catalina meeting",
            [342] =	"maybe Wu Zi meeting",
        },
    },
}

module.tplayer = tplayer

tplayer.style.fighting.list = imgui.new['const char*'][#tplayer.style.fighting.names](tplayer.style.fighting.names)
tplayer.style.walking.list  = imgui.new['const char*'][#tplayer.style.walking.names](tplayer.style.walking.names)

function module.ChangePlayerModel(model)
    if fpeds.tpeds.special[model] == nil then
        if isModelAvailable(model) then
            requestModel(model)
            loadAllModelsNow()
            setPlayerModel(PLAYER_HANDLE,model)
            markModelAsNoLongerNeeded(model)
        end
    else
        if not hasSpecialCharacterLoaded(290) then
            loadSpecialCharacter(fpeds.tpeds.special[model],1)
            loadAllModelsNow()
        end
        setPlayerModel(PLAYER_HANDLE,290)
        unloadSpecialCharacter(290)
    end
    printHelpString("Skin ~g~Changed")
end

function HealthArmour()
    fcommon.DropDownMenu("Health",function()
        local health = imgui.new.int()
        local max = math.floor(getFloatStat(24)/5.68)
        local min = 0

        health[0] = getCharHealth(PLAYER_PED)

        imgui.Columns(3,nil,false)
        imgui.Text("Max = " .. max)
        imgui.NextColumn()
        imgui.Text("Current = " .. health[0])
        imgui.NextColumn()
        imgui.Text("Min = " .. min)
        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt("Set",health) then
            setCharHealth(PLAYER_PED,health[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Increase",imgui.ImVec2(fcommon.GetSize(4))) and health[0] <  max then
            setCharHealth(PLAYER_PED,(health[0]+10))
        end
        imgui.SameLine()
        if imgui.Button("Decrease",imgui.ImVec2(fcommon.GetSize(4)))  and health[0] > 0 then
            setCharHealth(PLAYER_PED,(health[0]-10))
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(4))) then
            setCharHealth(PLAYER_PED, max)
        end
        imgui.SameLine()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(4))) then
            setCharHealth(PLAYER_PED,0)
        end

        if health[0] < 0 then
            setCharHealth(PLAYER_PED,0)
        end

        if health[0] >  max then
            setCharHealth(PLAYER_PED, max)
        end
    end)

    fcommon.DropDownMenu("Armour",function()
        local armour = imgui.new.int()
        local max_armour = 100
        local min_armour = 0
        armour[0] = getCharArmour(PLAYER_PED)

        imgui.Columns(3,nil,false)
        imgui.Text("Max = " .. max_armour)
        imgui.NextColumn()
        imgui.Text("Current = " .. armour[0])
        imgui.NextColumn()
        imgui.Text("Min = " .. min_armour)
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt("Set",armour) then

            if armour[0] < 0 then
                armour[0] = 0
            end
            if getCharArmour(PLAYER_PED) < armour[0] then
                addArmourToChar(PLAYER_PED,(armour[0]-getCharArmour(PLAYER_PED)))
            end
            if getCharArmour(PLAYER_PED) > armour[0] then
                damageChar(PLAYER_PED,getCharArmour(PLAYER_PED)-armour[0],true)
            end
        end
        imgui.PopItemWidth()

        if imgui.Button("Increase",imgui.ImVec2(fcommon.GetSize(4))) and armour[0] <  max_armour then
            addArmourToChar(PLAYER_PED,10)
        end
        imgui.SameLine()
        if imgui.Button("Decrease",imgui.ImVec2(fcommon.GetSize(4)))  and armour[0] > 0 then


            if getCharArmour(PLAYER_PED) > 10 then
                damageChar(PLAYER_PED,10,1)
            else
                damageChar(PLAYER_PED,getCharArmour(PLAYER_PED),1)
            end
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(4))) then
            addArmourToChar(PLAYER_PED, max_armour)
        end
        imgui.SameLine()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(4))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.Separator()
    end)
end

function WantedLevelMenu()
    fcommon.DropDownMenu("Wanted Level",function()
        local CPlayer = getCharPointer(PLAYER_PED)
        local  _,wl = storeWantedLevel(PLAYER_HANDLE)
        local wanted_level = imgui.new.int(wl)
        local max_wanted_level = imgui.new.int(readMemory(0x58DFE4,1,false))

        if imgui.SliderInt("Maximum",max_wanted_level,0,6) then
           writeMemory(0x58DFE4,1,max_wanted_level[0],false)
        end
        imgui.Spacing()

        if imgui.SliderInt("Current",wanted_level,0,max_wanted_level[0]) then
            callFunction(0x4396F0,1,0,false)
            alterWantedLevel(PLAYER_HANDLE,wanted_level[0])
        end

        if imgui.Button("Maximum") then
            callFunction(0x4396F0,1,0,false)
            alterWantedLevel(PLAYER_HANDLE,max_wanted_level[0])
        end
        imgui.SameLine()
        if imgui.Button("Minimum") then
            callFunction(0x4396F0,1,0,false)
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        fcommon.CheckBox({name = "Never wanted",var = tplayer.neverWanted,func = function()
            callFunction(0x4396C0,1,0,false)
            if tplayer.neverWanted[0] then
                fcommon.CheatActivated()
            else
                fcommon.CheatDeactivated()
            end
        end})
    end)
end

function SkinChangerMenu()
    imgui.Spacing()
    fcommon.CheckBox({name = "Aim Skin Changer",var = tplayer.aimSkinChanger})
    fcommon.InformationTooltip("Aim at a pedestrian and then press Enter/Return key to switch to that skin.")

    imgui.Spacing()
    if imgui.BeginTabBar("Skins") then
        if imgui.BeginTabItem('Gangs') then
            imgui.Spacing()
            if imgui.BeginChild("Gangs list Window") then
                fcommon.ShowEntries("Ballas",{102,103,104},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Da Nang Boys",{121,122,123},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Groove Families",{105,106,107,269,270,271},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Los Santos Vagos",{108,109,110},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Mafia",{111,112,113},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Russian Mafia",{124,125,126,127},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("San Fierro Rifa",{173,174,175},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("San Fierro Triads",{117,118,120},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Varrios Los Aztecas",{114,115,116},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Civillians') then
            imgui.Spacing()
            if imgui.BeginChild("Civillians list Window") then
                fcommon.ShowEntries("Antagonist",{290,291,292,293,294,295,296,297,298,299},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Beach",{18,45,138,139,140,154},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Bouncer",{163,164,165,166},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Boxer",{80,81},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Businessman",{17,141,147,148,150,177,227},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Country",{157,158,159,160,161,162,196,197,198,199,200},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Golf",{36,37},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Grl",{190,191,192,193,194,195},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Heckler",{258,259},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Hippie",{72,73},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Jogger",{90,96},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Karate Student",{203,204},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Pol",{66,67},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Low Class Male",{32,33,34,128,132,133,202},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Low Class Female",{31,129,130,131,151,201},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Mountain Biker",{51,52},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Rich Male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Rich Female",{9,12,40,53,55,88,91,169,215,216,219,224,231},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Roller Blade",{92,99},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Street Male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Street Female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Tramp Male",{78,79,134,135,136,137,212,213,230,239},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Tramp Female",{77,256,257},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Elvis",{82,83,84},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Criminals') then
            imgui.Spacing()
            if imgui.BeginChild("Criminals list Window") then
                fcommon.ShowEntries("Biker",{247,248},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Body Guard",{24,25},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Criminal",{21,47,100,143,181,183,184,223,250},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Drug Dealer",{28,29,30,154},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Jobs') then
            imgui.Spacing()
            if imgui.BeginChild("Jobs list Window") then
                fcommon.ShowEntries("Cab Driver",{182,206,220,234,261,262},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Construction",{27,153,260},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Croupier",{11,171,172},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Clothes Seller",{211,217},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Fire Fighter",{277,278,279},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Law Enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Life Guard",{97,251},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Medic",{274,275,276},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Shop Seller",{205,155,156,167,168,176,177,179,180},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Valet",{189,252,},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries("Worker",{16,50,61,253,255},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Other') then
            imgui.Spacing()
            if imgui.BeginChild("Other list Window") then
                fcommon.ShowEntries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Search') then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",tplayer.skins.search_text,ffi.sizeof(tplayer.skins.search_text)) then end
			imgui.SameLine()

			imgui.Spacing()
			imgui.Text("Found entries:(" .. ffi.string(tplayer.skins.search_text) .. ")")
			imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Skin Entries") then
                fcommon.ShowEntries(nil,fpeds.tpeds.models,100,60,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true,tplayer.skins.search_text)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end

end

function WalkingStyle()
    if imgui.Combo("Walking Style", tplayer.style.walking.selected,tplayer.style.walking.list,#tplayer.style.walking.names) then
        writeMemory(0x609A4E,4,-1869574000,true)
        writeMemory(0x609A52,2,37008,true)
        requestAnimation(tplayer.style.walking.names[tplayer.style.walking.selected[0]+1])
        loadAllModelsNow()
        setAnimGroupForChar(PLAYER_PED,tplayer.style.walking.names[tplayer.style.walking.selected[0]+1])
        removeAnimation(tplayer.style.walking.names[tplayer.style.walking.selected[0]+1])
        fcommon.CheatActivated()
    end
end

function FightingStyle()

    if imgui.Combo("Fighting Style", tplayer.style.fighting.selected,tplayer.style.fighting.list,#tplayer.style.fighting.names) then
        giveMeleeAttackToChar(PLAYER_PED,tplayer.style.fighting.selected[0]+4,6)
        fcommon.CheatActivated()
    end

end

function module.PlayerMain()

    if imgui.BeginTabBar('Player') then

        if imgui.BeginTabItem('Checkboxes') then
            imgui.Spacing()
            imgui.Columns(2,nil,false)

            if imgui.Checkbox("God Mode",tplayer.god) then fcommon.CheatActivated() end
            fcommon.CheckBox({ address = 0xB7CEE4,name = "Infinite run"})
            fcommon.CheckBox({ address = 0x969178,name = "Infinite ammo"})
            fcommon.CheckBox({ address = 0x96916D,name = "Infinite health"})
            fcommon.CheckBox({ address = 0x96916E,name = "Infinite oxygen"})
            fcommon.CheckBox({ address = getCharPointer(PLAYER_PED)+0x598,name = "Lock player control"})

            imgui.NextColumn()

            fcommon.CheckBox({ address = 0xB7CEE6,name = "Fireproof",help_text = "Player is immue to fire."})
            fcommon.CheckBox({ address = 0x96916C,name = "Mega jump"})
            fcommon.CheckBox({ address = 0x969173,name = "Mega punch"})
            fcommon.CheckBox({ address = 0x969161,name = "Huge bunny hop",help_text = "Huge cycle jumps."})
            fcommon.CheckBox({ address = 0x969174,name = "Never get hungry"})
            fcommon.CheckBox({ address = 0x969179,name = "Aim while driving",help_text = "Player can shoot while driving."})
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            imgui.Text("Body")
            if imgui.RadioButtonIntPtr("Fat",tplayer.cjBody,1) then
                callFunction(0x439110,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr("Muscle",tplayer.cjBody,2) then
                -- body not changing to muscular after changing to fat fix
                callFunction(0x439190,1,1,false)
                callFunction(0x439150,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr("Skinny",tplayer.cjBody,3) then
                callFunction(0x439190,1,1,false)
                fcommon.CheatActivated()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Menu') then
            imgui.Spacing()
            fcommon.UpdateAddress({name = "Money",address = 0xB7CE50,size = 4})
            HealthArmour()
            fcommon.UpdateStat({ name = "Energy",stat = 165})
            fcommon.UpdateStat({ name = "Max Health",stat = 24,max = 1450})
            WantedLevelMenu()

            fcommon.UpdateStat({ name = "Lung Capacity",stat = 225})
            fcommon.UpdateStat({ name = "Fat",stat = 21})
            fcommon.UpdateStat({ name = "Stamina",stat = 22})
            fcommon.UpdateStat({ name = "Muscle",stat = 23})
            fcommon.UpdateStat({ name = "Sex Appeal",stat = 25})
            fcommon.UpdateStat({ name = "Respect",stat = 68,max = 2450})
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('GF') then

            imgui.Spacing()
            if imgui.Button("Max GF Progress",imgui.ImVec2(fcommon.GetSize(1))) then
                for i=252,257,1 do
                    setFloatStat(i,100)
                end
                clearHelp()
                printHelpString("Girlfriend progress set to maximum")
            end
            imgui.Spacing()
            imgui.Text("Progress")
            imgui.Separator()
            imgui.Spacing()
            fcommon.UpdateStat({ name = "Denise",stat = 252,max = 100})
            fcommon.UpdateStat({ name = "Michelle",stat = 253,max = 100})
            fcommon.UpdateStat({ name = "Helena",stat = 254,max = 100})
            fcommon.UpdateStat({ name = "Barbara",stat = 255,max = 100})
            fcommon.UpdateStat({ name = "Katie",stat = 256,max = 100})
            fcommon.UpdateStat({ name = "Millie",stat = 257,max = 100})
            imgui.Spacing()
            imgui.Text("Miscellaneous")
            imgui.Separator()
            imgui.Spacing()
            fcommon.UpdateStat({ name = "GF Count",stat = 184})
            fcommon.UpdateStat({ name = "Disastrous Dates",stat = 185})
            fcommon.UpdateStat({ name = "Girls Dated",stat = 186})
            fcommon.UpdateStat({ name = "Scored Dates",stat = 187})
            fcommon.UpdateStat({ name = "Successful Dates",stat = 188})
            fcommon.UpdateStat({ name = "Girls Dumped",stat = 189})
            fcommon.UpdateStat({ name = "Prostitutes Visited",stat = 190})
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Skins') then
            SkinChangerMenu()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Style') then
            imgui.Spacing()
            FightingStyle()
            WalkingStyle()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Stats') then
            imgui.Spacing()
            if imgui.Button("Max Vehicle Skills",imgui.ImVec2(fcommon.GetSize(2))) then
                callFunction(0x4399D0,1,1,false)
                displayNonMinigameHelpMessages(false)
                fcommon.CheatActivated()
            end
            imgui.SameLine()
            if imgui.Button("Max Weapon Skills",imgui.ImVec2(fcommon.GetSize(2))) then
                for i=69,79,1 do
                    setFloatStat(i)
                end
                fcommon.CheatActivated()
            end
            imgui.Spacing()
            if imgui.BeginTabBar("Stats") then
                imgui.Spacing()

                if imgui.BeginTabItem('Vehicle') then
                    fcommon.UpdateStat({ name = "Driving",stat = 160})
                    fcommon.UpdateStat({ name = "Flying",stat = 223})
                    fcommon.UpdateStat({ name = "Bike",stat = 229})
                    fcommon.UpdateStat({ name = "Cycling",stat = 230})
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem('Weapon') then
                    fcommon.UpdateStat({ name = "Pistol",stat = 69})
                    fcommon.UpdateStat({ name = "Silenced Pistol",stat = 70})
                    fcommon.UpdateStat({ name = "Desert Eagle",stat = 71})
                    fcommon.UpdateStat({ name = "Shotgun",stat = 72})
                    fcommon.UpdateStat({ name = "Sawn-Off Shotgun",stat = 73})
                    fcommon.UpdateStat({ name = "Combat Shotgun",stat = 74})
                    fcommon.UpdateStat({ name = "Machine Pistol",stat = 75})
                    fcommon.UpdateStat({ name = "SMG",stat = 76})
                    fcommon.UpdateStat({ name = "AK-47",stat = 77})
                    fcommon.UpdateStat({ name = "M4",stat = 78})
                    fcommon.UpdateStat({ name = "Rifle",stat = 79})
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem('Search') then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",tplayer.stats.search_text,ffi.sizeof(tplayer.stats.search_text)) then end
                    imgui.SameLine()

                    imgui.Spacing()
                    imgui.Text("Found entries:(" .. ffi.string(tplayer.stats.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Stat Entries") then
                        for i=0,342,1 do
                            if tplayer.stats.names[i] ~= nil then
                                if (ffi.string(tplayer.stats.search_text) == "") or (string.upper(tplayer.stats.names[i]):find(string.upper(ffi.string(tplayer.stats.search_text))) ~= nil) then
                                    fcommon.UpdateStat({ name = tplayer.stats.names[i],stat = i})
                                end
                            end
                        end
                        imgui.Spacing()
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
