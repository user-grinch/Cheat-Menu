-- This module contains all the fuction for player tab

local module = {}

local tplayer =
{
    clothes           = 
    {
        path          = tcheatMenu.dir .. "clothes\\",
        images        = {},
    },
    god               = imgui.new.bool(fconfig.get('tplayer.god',false)),
    aimSkinChanger    = imgui.new.bool(fconfig.get('tplayer.aimSkinChanger',false)),
    neverWanted       = imgui.new.bool(fconfig.get('tplayer.neverWanted',false) ),
    cjBody            = imgui.new.int(fconfig.get('tplayer.cjBody',0)),
    skins =
    {
        search_text    = imgui.new.char[20](fconfig.get('tplayer.skins.search_text',"")),
    },
    style =
    {
        fighting =
        {
            selected = imgui.new.int(fconfig.get('tplayer.style.fighting.selected',0)),
            names    = {"Default","Boxing","Kung fu","Kick Boxing","Punch Kick"},
            list     = {},
        },
        walking =
        {
            selected = imgui.new.int(fconfig.get('tplayer.style.walking.selected',0)),
            names    = {"man","shuffle","oldman","gang1","gang2","oldfatman","fatman","jogger","drunkman","blindman","swat","woman","shopping","busywoman","sexywoman","pro","oldwoman","fatwoman","jogwoman","oldfatwoman","skate"},
            list     = {},
        },
    },
    stats =
    {
        search_text = imgui.new.char[20](fconfig.get('tplayer.stats.search_text',"")),
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
            unloadSpecialCharacter(model)
        end
        loadSpecialCharacter(fpeds.tpeds.special[model],1)
        loadAllModelsNow()
        setPlayerModel(PLAYER_HANDLE,290)
    end
    clearCharTasksImmediately(PLAYER_PED)
    printHelpString(flanguage.GetText("player.SkinChanged"))
end

function HealthArmour()
    fcommon.DropDownMenu(flanguage.GetText("player.Health"),function()
        local health = imgui.new.int(getCharHealth(PLAYER_PED))

        imgui.Columns(2,nil,false)
        imgui.Text(flanguage.GetText("common.Minimum") .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text(flanguage.GetText("common.Maximum") .. " = " .. tostring(255))
        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt(flanguage.GetText("common.Set"),health) then
            if health[0] > 100 then
                setFloatStat(24,health[0]*5.686)
                
            else
                setFloatStat(24,569.0)
            end

            setCharHealth(PLAYER_PED,health[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button(flanguage.GetText("common.Minimum"),imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,0)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Default"),imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,100)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Maximum"),imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,1450.0)
            setCharHealth(PLAYER_PED,255)
        end

        if health[0] < 0 then
            setCharHealth(PLAYER_PED,0)
        end

        if health[0] >  255 then
            setCharHealth(PLAYER_PED, 255)
        end
    end)

    fcommon.DropDownMenu(flanguage.GetText("player.Armour"),function()
        local armour = imgui.new.int()
        local max_armour = 100
        local min_armour = 0
        armour[0] = getCharArmour(PLAYER_PED)

        imgui.Columns(2,nil,false)
        imgui.Text(flanguage.GetText("common.Minimum") .. " = " .. min_armour)
        imgui.NextColumn()
        imgui.Text(flanguage.GetText("common.Maximum") .. " = " .. max_armour)
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt(flanguage.GetText("common.Set"),armour) then

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
        imgui.Spacing()
        if imgui.Button(flanguage.GetText("common.Minimum"),imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Default"),imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Maximum"),imgui.ImVec2(fcommon.GetSize(3))) then
            addArmourToChar(PLAYER_PED, max_armour)
        end
    end)
end

function module.GetClotheName(name)
    local model, texture = name:match("([^$]+)$([^$]+)")
    return texture
end

function module.ChangePlayerClothe(name,body_part)
    local model, texture = name:match("([^$]+)$([^$]+)")
    
    setPlayerModel(PLAYER_HANDLE,0)
    givePlayerClothesOutsideShop(PLAYER_HANDLE,0,0,body_part)
    givePlayerClothesOutsideShop(PLAYER_HANDLE,texture,model,body_part)
    buildPlayerModel(PLAYER_HANDLE)
    printHelpString(flanguage.GetText("player.ClotheChanged"))
end

function ShowClothes(label,path,body_part)

    local mask = path .. "*.jpg"
    local handle,file = findFirstFile(mask)
    local model_table = {}

    while handle and file do
        table.insert( model_table,#model_table+1,string.sub( file,1,-5))
        file = findNextFile(handle)
    end
    fcommon.ShowEntries(label,model_table,100,80,tplayer.clothes.images,path,".jpg",fplayer.ChangePlayerClothe,fplayer.GetClotheName,true,nil,body_part)
end


function WantedLevelMenu()
    
    fcommon.DropDownMenu(flanguage.GetText("player.WantedLevel"),function()
        local  _,wl = storeWantedLevel(PLAYER_HANDLE)
        local wanted_level = imgui.new.int(wl)
        local max_wanted_level = imgui.new.int(readMemory(0x58DFE4,1,false))
        
        imgui.Columns(2,nil,false)
        imgui.Text(flanguage.GetText("common.Minimum") .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text(flanguage.GetText("common.Maximum") .. " = " .. tostring(6))
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)

        if imgui.InputInt(flanguage.GetText("common.Set"),wanted_level) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,wanted_level[0])
        end
        imgui.PopItemWidth()
   
        imgui.Spacing()
        if imgui.Button(flanguage.GetText("common.Minimum"),imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Default"),imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Maximum"),imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,max_wanted_level[0])
        end
    end)
end

function SkinChangerMenu()
    imgui.Spacing()
    fcommon.CheckBox({name = flanguage.GetText("player.AimSkinChanger"),var = tplayer.aimSkinChanger})
    fcommon.InformationTooltip(flanguage.GetText("player.AimSkinChangerToolTip"))

    imgui.Spacing()
    if imgui.BeginTabBar(flanguage.GetText("player.Skins")) then
        if imgui.BeginTabItem(flanguage.GetText('peds.Gangs')) then
            imgui.Spacing()
            if imgui.BeginChild("Gangs list Window") then
                fcommon.ShowEntries(flanguage.GetText("peds.Ballas"),{102,103,104},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.DaNangBoys"),{121,122,123},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.GroveStreetFamilies"),{105,106,107,269,270,271},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.LosSantosVagos"),{108,109,110},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Mafia"),{111,112,113,124,125,126,127},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.SanFierroRifa"),{173,174,175},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.MountainCloudTriad"),{117,118,120},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.VarriosLosAztecas"),{114,115,116},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem(flanguage.GetText('peds.Civilians')) then
            imgui.Spacing()
            if imgui.BeginChild("Civilians list Window") then
                fcommon.ShowEntries(flanguage.GetText("peds.Antagonist"),{290,291,292,293,294,295,296,297,298,299},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Beach"),{18,45,138,139,140,154},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Bouncer"),{163,164,165,166},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Boxer"),{80,81},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Businessman"),{17,141,147,148,150,177,227},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Country"),{157,158,159,160,161,162,196,197,198,199,200},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Golf"),{36,37},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Grl"),{190,191,192,193,194,195},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Heckler"),{258,259},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Hippie"),{72,73},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Jogger"),{90,96},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.KarateStudent"),{203,204},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Pol"),{66,67},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.LowClassMale"),{32,33,34,128,132,133,202},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.LowClassFemale"),{31,129,130,131,151,201},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.MountainBiker"),{51,52},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.RichMale"),{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.RichFemale"),{9,12,40,53,55,88,91,169,215,216,219,224,231},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.RollerBlade"),{92,99},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.StreetMale"),{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.StreetFemale"),{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.TrampMale"),{78,79,134,135,136,137,212,213,230,239},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.TrampFemale"),{77,256,257},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Elvis"),{82,83,84},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText('peds.Criminals')) then
            imgui.Spacing()
            if imgui.BeginChild("Criminals list Window") then
                fcommon.ShowEntries(flanguage.GetText("peds.Biker"),{247,248},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.BodyGuard"),{24,25},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Criminal"),{21,47,100,143,181,183,184,223,250},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.DrugDealer"),{28,29,30,154},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem(flanguage.GetText('peds.Jobs')) then
            imgui.Spacing()
            if imgui.BeginChild("Jobs list Window") then
                fcommon.ShowEntries(flanguage.GetText("peds.CabDriver"),{182,206,220,234,261,262},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Construction"),{27,153,260},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Croupier"),{11,171,172},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.ClothesSeller"),{211,217},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.FireFighter"),{277,278,279},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.LawEnforcement"),{71,265,266,267,280,281,282,283,284,285,286,287,288},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.LifeGuard"),{97,251},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Medic"),{274,275,276},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Prostitute"),{63,64,75,85,87,152,178,207,237,238,243,245,249},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.ShopSeller"),{205,155,156,167,168,176,177,179,180},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Valet"),{189,252,},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                fcommon.ShowEntries(flanguage.GetText("peds.Worker"),{16,50,61,253,255},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("common.Misc")) then
            imgui.Spacing()
            if imgui.BeginChild("Other list Window") then
                fcommon.ShowEntries(flanguage.GetText("common.Misc"),{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText('common.Search')) then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText(flanguage.GetText("common.Search"),tplayer.skins.search_text,ffi.sizeof(tplayer.skins.search_text)) then end
			imgui.SameLine()

			imgui.Spacing()
			imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tplayer.skins.search_text) .. ")")
			imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Skin Entries") then
                fcommon.ShowEntries(nil,fpeds.tpeds.models,110,55,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fplayer.ChangePlayerModel,fpeds.GetName,true,tplayer.skins.search_text)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end

end

function WalkingStyle()
    if imgui.Combo(flanguage.GetText("player.WalkingStyle"), tplayer.style.walking.selected,tplayer.style.walking.list,#tplayer.style.walking.names) then
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

    if imgui.Combo(flanguage.GetText("player.FightingStyle"), tplayer.style.fighting.selected,tplayer.style.fighting.list,#tplayer.style.fighting.names) then
        giveMeleeAttackToChar(PLAYER_PED,tplayer.style.fighting.selected[0]+4,6)
        fcommon.CheatActivated()
    end

end

function module.PlayerMain()

    if imgui.BeginTabBar('Player') then

        if imgui.BeginTabItem(flanguage.GetText("common.Checkboxes")) then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ address = 0x969179,name = flanguage.GetText("player.AimWhileDriving")})
            fcommon.CheckBox({ var = tplayer.god,name  = flanguage.GetText("player.GodMode")})
            fcommon.CheckBox({ address = 0x969161,name = flanguage.GetText("player.HigherCycleJumps")})
            fcommon.CheckBox({ address = 0x969178,name = flanguage.GetText("player.InfiniteAmmo")}) 
            fcommon.CheckBox({ address = 0x96916E,name = flanguage.GetText("player.InfiniteOxygen")})
            fcommon.CheckBox({ address = 0xB7CEE4,name = flanguage.GetText("player.InfiniteRun")})
            
            imgui.NextColumn()

            fcommon.CheckBox({ address = getCharPointer(PLAYER_PED)+0x598,name = flanguage.GetText("player.LockPlayerControl")})
            fcommon.CheckBox({ address = 0x96916C,name = flanguage.GetText("player.MegaJump")})
            fcommon.CheckBox({ address = 0x969173,name = flanguage.GetText("player.MegaPunch")})
            fcommon.CheckBox({ address = 0x969174,name = flanguage.GetText("player.NeverGetHungry")})
            fcommon.CheckBox({name = flanguage.GetText("player.NeverWanted"),var = tplayer.neverWanted,func = function()
                callFunction(0x4396C0,1,0,false)
                if tplayer.neverWanted[0] then
                    fcommon.CheatActivated()
                else
                    fcommon.CheatDeactivated()
                end
            end})
           
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            imgui.Text(flanguage.GetText("player.Body"))
            if imgui.RadioButtonIntPtr(flanguage.GetText("player.Fat"),tplayer.cjBody,1) then
                callFunction(0x439110,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr(flanguage.GetText("player.Muscle"),tplayer.cjBody,2) then
                -- body not changing to muscular after changing to fat fix
                callFunction(0x439190,1,1,false)
                callFunction(0x439150,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr(flanguage.GetText("player.Skinny"),tplayer.cjBody,3) then
                callFunction(0x439190,1,1,false)
                fcommon.CheatActivated()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("common.Menus")) then
            imgui.Spacing()
            
            fcommon.UpdateStat({ name = flanguage.GetText("player.Energy"),stat = 165})
            fcommon.UpdateStat({ name = flanguage.GetText("player.Fat"),stat = 21})
            HealthArmour()
            fcommon.UpdateStat({ name = flanguage.GetText("player.LungCapacity"),stat = 225})
            fcommon.UpdateAddress({name = flanguage.GetText("player.Money"),address = 0xB7CE50,size = 4,min = -9999999,max = 9999999})
            fcommon.UpdateStat({ name = flanguage.GetText("player.Muscle"),stat = 23})
            fcommon.UpdateStat({ name = flanguage.GetText("player.Respect"),stat = 68,max = 2450}) 
            fcommon.UpdateStat({ name = flanguage.GetText("player.SexAppeal"),stat = 25})
            fcommon.UpdateStat({ name = flanguage.GetText("player.Stamina"),stat = 22})
            WantedLevelMenu()

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("player.Skins")) then
            SkinChangerMenu()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("player.Clothes")) then
            imgui.Spacing()
            if imgui.Button(flanguage.GetText("player.RemoveClothes"),imgui.ImVec2(fcommon.GetSize(1))) then
                for i=0, 17 do givePlayerClothes(PLAYER_HANDLE,0,0,i) end
                buildPlayerModel(PLAYER_HANDLE)
            end
            imgui.Spacing()

            if imgui.BeginTabBar(flanguage.GetText("common.List")) then
                imgui.Spacing()
                
                if imgui.BeginTabItem(flanguage.GetText("common.List")) then
                    if imgui.BeginChild("Clothes") then
        
                        ShowClothes("Extras",tplayer.clothes.path .. "Extras\\",17)   
                        ShowClothes("Glasses",tplayer.clothes.path .. "Glasses\\",15)
                        ShowClothes("Hats",tplayer.clothes.path .. "Hats\\",16)
                        ShowClothes("Heads",tplayer.clothes.path .. "Heads\\",1)
                        ShowClothes("Necklaces",tplayer.clothes.path .. "Necklaces\\",13)
                        ShowClothes("Shirts",tplayer.clothes.path .. "Shirts\\",0)
                        ShowClothes("Shoes",tplayer.clothes.path .. "Shoes\\",3)
                        ShowClothes("Tattoos back",tplayer.clothes.path .. "Tattoos back\\",8)
                        ShowClothes("Tattoos left chest",tplayer.clothes.path .. "Tattoos left chest\\",9)
                        ShowClothes("Tattoos left lower arm",tplayer.clothes.path .. "Tattoos left lower arm\\",4)
                        ShowClothes("Tattoos left upper arm",tplayer.clothes.path .. "Tattoos left upper arm\\",5)
                        ShowClothes("Tattoos lower back",tplayer.clothes.path .. "Tattoos lower back\\",12)  
                        ShowClothes("Tattoos right chest",tplayer.clothes.path .. "Tattoos right chest\\",10)
                        ShowClothes("Tattoos right lower arm",tplayer.clothes.path .. "Tattoos right lower arm\\",7)
                        ShowClothes("Tattoos right upper arm",tplayer.clothes.path .. "Tattoos right upper arm\\",6)
                        ShowClothes("Tattoos stomach",tplayer.clothes.path .. "Tattoos stomach\\",11)
                        ShowClothes("Trousers",tplayer.clothes.path .. "Trousers\\",2)
                        ShowClothes("Watches",tplayer.clothes.path .. "Watches\\",14)
                        
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end 
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("player.Styles")) then
            imgui.Spacing()
            FightingStyle()
            WalkingStyle()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("player.Stats")) then
            imgui.Spacing()
            if imgui.Button(flanguage.GetText("player.MaxVehicleSkills"),imgui.ImVec2(fcommon.GetSize(2))) then
                callFunction(0x4399D0,1,1,false)
                displayNonMinigameHelpMessages(false)
                fcommon.CheatActivated()
            end
            imgui.SameLine()
            if imgui.Button(flanguage.GetText("player.MaxWeaponSkills"),imgui.ImVec2(fcommon.GetSize(2))) then
                for i=69,79,1 do
                    setFloatStat(i)
                end
                fcommon.CheatActivated()
            end
            imgui.Spacing()
            if imgui.BeginTabBar("Stats") then
                imgui.Spacing()

                if imgui.BeginTabItem(flanguage.GetText("player.Vehicles")) then
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Bike"),stat = 229})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Cycling"),stat = 230})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Driving"),stat = 160})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Flying"),stat = 223})        
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem(flanguage.GetText("player.Weapons")) then
                    fcommon.UpdateStat({ name = flanguage.GetText("player.AK47"),stat = 77})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.CombatShotgun"),stat = 74})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.DesertEagle"),stat = 71})
                    
                    fcommon.UpdateStat({ name = flanguage.GetText("player.MachinePistol"),stat = 75})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.M4"),stat = 78})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Pistol"),stat = 69})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.SawnOffShotgun"),stat = 73})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Shotgun"),stat = 72})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.SilencedPistol"),stat = 70})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.SMG"),stat = 76})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Rifle"),stat = 79})
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem(flanguage.GetText("player.GF")) then

                    imgui.Spacing()
                    if imgui.Button(flanguage.GetText("player.MaxGFProgress"),imgui.ImVec2(fcommon.GetSize(1))) then
                        for i=252,257,1 do
                            setFloatStat(i,100)
                        end
                        clearHelp()
                        fcommon.CheatActivated()
                    end
                    imgui.Spacing()
                    imgui.Text(flanguage.GetText("player.Progress"))
                    imgui.Separator()
                    imgui.Spacing()
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Barbara"),stat = 255,max = 100})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Denise"),stat = 252,max = 100})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Helena"),stat = 254,max = 100})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Katie"),stat = 256,max = 100})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Michelle"),stat = 253,max = 100})
                    fcommon.UpdateStat({ name = flanguage.GetText("player.Millie"),stat = 257,max = 100})
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem(flanguage.GetText("common.Search")) then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText(flanguage.GetText("common.Search"),tplayer.stats.search_text,ffi.sizeof(tplayer.stats.search_text)) then end
                    imgui.SameLine()

                    imgui.Spacing()
                    imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tplayer.stats.search_text) .. ")")
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
