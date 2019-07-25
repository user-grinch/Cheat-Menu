-- This module contains all the functions used in peds

local module = {}

local tpeds =
{
    path   = getGameDirectory() .. "\\moonloader\\lib\\cheat-menu\\peds\\",
    images = {},
    type   =
    {
        list     = {},
        names =
        {
          --  "PLAYER1",
          --  "PLAYER2",
            "PLAYER3",
            "PLAYER_NETWORK",
            "PLAYER_UNUSED",
            "CIVMALE",
            "CIVFEMALE",
            "COP",
            "Ballas",
            "Grove Street Families",
            "Los Santos Vagos",
            "San Fierro Rifa",
            "Da Nang Boys",
            "Mafia",
            "Mountain Cloud Triads",
            "Varrio Los Aztecas",
            "GANG9",
            "GANG10",
            "DEALER",
            "EMERGENCY",
            "FIREMAN",
            "CRIMINAL",
            "BUM",
            "SPECIAL",
            "PROSTITUTE",
        },
        selected = imgui.new.int(fconfig.get('tpeds.type.selected') or 0),
    },
    names =
    {
        [0]   = "CJ",
        [1]   = "TRUTH",
        [2]   = "MACCER",
        [3]   = "ANDRE",
        [4]   = "BBTHIN",
        [5]   = "BB",
        [6]   = "EMMET",
        [7]   = "MALE01",
        [8]   = "JANITOR",
        [9]   = "BFORI",
        [11]  = "VBFYCRP",
        [12]  = "BFYRI",
        [13]  = "BFYST",
        [14]  = "BMORI",
        [15]  = "BMOST",
        [16]  = "BMYAP",
        [17]  = "BMYBU",
        [18]  = "BMYBE",
        [19]  = "BMYDJ",
        [20]  = "BMYRI",
        [21]  = "BMYCR",
        [22]  = "BMYST",
        [23]  = "WMYBMX",
        [24]  = "WBDYG1",
        [25]  = "WBDYG2",
        [26]  = "WMYBP",
        [27]  = "WMYCON",
        [28]  = "BMYDRUG",
        [29]  = "WMYDRUG",
        [30]  = "HMYDRUG",
        [31]  = "DWFOLC",
        [32]  = "DWMOLC1",
        [33]  = "DWMOLC2",
        [34]  = "DWMYLC1",
        [35]  = "HMOGAR",
        [36]  = "WMYGOL1",
        [37]  = "WMYGOL2",
        [38]  = "HFORI",
        [39]  = "HFOST",
        [40]  = "HFYRI",
        [41]  = "HFYST",
        [43]  = "HMORI",
        [44]  = "HMOST",
        [45]  = "HMYBE",
        [46]  = "HMYRI",
        [47]  = "HMYCR",
        [48]  = "HMYST",
        [49]  = "OMOKUNG",
        [50]  = "WMYMECH",
        [51]  = "BMYMOUN",
        [52]  = "WMYMOUN",
        [53]  = "OFORI",
        [54]  = "OFOST",
        [55]  = "OFYRI",
        [56]  = "OFYST",
        [57]  = "OMORI",
        [58]  = "OMOST",
        [59]  = "OMYRI",
        [60]  = "OMYST",
        [61]  = "WMYPLT",
        [62]  = "WMOPJ",
        [63]  = "BFYPRO",
        [64]  = "HFYPRO",
        [66]  = "BMYPOL1",
        [67]  = "BMYPOL2",
        [68]  = "WMOPREA",
        [69]  = "SBFYST",
        [70]  = "WMOSCI",
        [71]  = "WMYSGRD",
        [72]  = "SWMYHP1",
        [73]  = "SWMYHP2",
        [75]  = "SWFOPRO",
        [76]  = "WFYSTEW",
        [77]  = "SWMOTR1",
        [78]  = "WMOTR1",
        [79]  = "BMOTR1",
        [80]  = "VBMYBOX",
        [81]  = "VWMYBOX",
        [82]  = "VHMYELV",
        [83]  = "VBMYELV",
        [84]  = "VIMYELV",
        [85]  = "VWFYPRO",
        [87]  = "VWFYST1",
        [88]  = "WFORI",
        [89]  = "WFOST",
        [90]  = "WFYJG",
        [91]  = "WFYRI",
        [92]  = "WFYRO",
        [93]  = "WFYST",
        [94]  = "WMORI",
        [95]  = "WMOST",
        [96]  = "WMYJG",
        [97]  = "WMYLG",
        [98]  = "WMYRI",
        [99]  = "WMYRO",
        [100] = "WMYCR",
        [101] = "WMYST",
        [102] = "BALLAS1",
        [103] = "BALLAS2",
        [104] = "BALLAS3",
        [105] = "FAM1",
        [106] = "FAM2",
        [107] = "FAM3",
        [108] = "LSV1",
        [109] = "LSV2",
        [110] = "LSV3",
        [111] = "MAFFA",
        [112] = "MAFFB",
        [113] = "MAFBOSS",
        [114] = "VLA1",
        [115] = "VLA2",
        [116] = "VLA3",
        [117] = "TRIADA",
        [118] = "TRIADB",
        [120] = "TRIBOSS",
        [121] = "DNB1",
        [122] = "DNB2",
        [123] = "DNB3",
        [124] = "VMAFF1",
        [125] = "VMAFF2",
        [126] = "VMAFF3",
        [127] = "VMAFF4",
        [128] = "DNMYLC",
        [129] = "DNFOLC1",
        [130] = "DNFOLC2",
        [131] = "DNFYLC",
        [132] = "DNMOLC1",
        [133] = "DNMOLC2",
        [134] = "SBMOTR2",
        [135] = "SWMOTR2",
        [136] = "SBMYTR3",
        [137] = "SWMOTR3",
        [138] = "WFYBE",
        [139] = "BFYBE",
        [140] = "HFYBE",
        [141] = "SOFYBU",
        [142] = "SBMYST",
        [143] = "SBMYCR",
        [144] = "BMYCG",
        [145] = "WFYCRK",
        [146] = "HMYCM",
        [147] = "WMYBU",
        [148] = "BFYBU",
        [150] = "WFYBU",
        [151] = "DWFYLC1",
        [152] = "WFYPRO",
        [153] = "WMYCONB",
        [154] = "WMYBE",
        [155] = "WMYPIZZ",
        [156] = "BMOBAR",
        [157] = "CWFYHB",
        [158] = "CWMOFR",
        [159] = "CWMOHB1",
        [160] = "CWMOHB2",
        [161] = "CWMYFR",
        [162] = "CWMYHB1",
        [163] = "BMYBOUN",
        [164] = "WMYBOUN",
        [165] = "WMOMIB",
        [166] = "BMYMIB",
        [167] = "WMYBELL",
        [168] = "BMOCHIL",
        [169] = "SOFYRI",
        [170] = "SOMYST",
        [171] = "VWMYBJD",
        [172] = "VWFYCRP",
        [173] = "SFR1",
        [174] = "SFR2",
        [175] = "SFR3",
        [176] = "BMYBAR",
        [177] = "WMYBAR",
        [178] = "WFYSEX",
        [179] = "WMYAMMO",
        [180] = "BMYTATT",
        [181] = "VWMYCR",
        [182] = "VBMOCD",
        [183] = "VBMYCR",
        [184] = "VHMYCR",
        [185] = "SBMYRI",
        [186] = "SOMYRI",
        [187] = "SOMYBU",
        [188] = "SWMYST",
        [189] = "WMYVA",
        [190] = "COPGRL3",
        [191] = "GUNGRL3",
        [192] = "MECGRL3",
        [193] = "NURGRL3",
        [194] = "CROGRL3",
        [195] = "GANGRL3",
        [196] = "CWFOFR",
        [197] = "CWFOHB",
        [198] = "CWFYFR1",
        [199] = "CWFYFR2",
        [200] = "CWMYHB2",
        [201] = "DWFYLC2",
        [202] = "DWMYLC2",
        [203] = "OMYKARA",
        [204] = "WMYKARA",
        [205] = "WFYBURG",
        [206] = "VWMYCD",
        [207] = "VHFYPRO",
        [209] = "OMONOOD",
        [210] = "OMOBOAT",
        [211] = "WFYCLOT",
        [212] = "VWMOTR1",
        [213] = "VWMOTR2",
        [214] = "VWFYWAI",
        [215] = "SBFORI",
        [216] = "SWFYRI",
        [217] = "WMYCLOT",
        [218] = "SBFOST",
        [219] = "SBFYRI",
        [220] = "SBMOCD",
        [221] = "SBMORI",
        [222] = "SBMOST",
        [223] = "SHMYCR",
        [224] = "SOFORI",
        [225] = "SOFOST",
        [226] = "SOFYST",
        [227] = "SOMOBU",
        [228] = "SOMORI",
        [229] = "SOMOST",
        [230] = "SWMOTR5",
        [231] = "SWFORI",
        [232] = "SWFOST",
        [233] = "SWFYST",
        [234] = "SWMOCD",
        [235] = "SWMORI",
        [236] = "SWMOST",
        [237] = "SHFYPRO",
        [238] = "SBFYPRO",
        [239] = "SWMOTR4",
        [240] = "SWMYRI",
        [241] = "SMYST",
        [242] = "SMYST2",
        [243] = "SFYPRO",
        [244] = "VBFYST2",
        [245] = "VBFYPRO",
        [246] = "VHFYST3",
        [247] = "BIKERA",
        [248] = "BIKERB",
        [249] = "BMYPIMP",
        [250] = "SWMYCR",
        [251] = "WFYLG",
        [252] = "WMYVA2",
        [253] = "BMOSEC",
        [254] = "BIKDRUG",
        [255] = "WMYCH",
        [256] = "SBFYSTR",
        [257] = "SWFYSTR",
        [258] = "HECK1",
        [259] = "HECK2",
        [260] = "BMYCON",
        [261] = "WMYCD1",
        [262] = "BMOCD",
        [263] = "VWFYWA2",
        [264] = "WMOICE",
        [271] = "RYDER",
        [274] = "LAEMT1",
        [275] = "LVEMT1",
        [276] = "SFEMT1",
        [277] = "LAFD1",
        [278] = "LVFD1",
        [279] = "SFFD1",
        [280] = "LAPD1",
        [281] = "SFPD1",
        [282] = "LVPD1",
        [283] = "CSHER",
        [284] = "LAPDM1",
        [285] = "SWAT",
        [286] = "FBI",
        [287] = "ARMY",
        [288] = "DSHER",
        [289] = "ZERO",
        [290] = "ROSE",
        [291] = "PAUL",
        [292] = "CESAR",
        [293] = "OGLOC",
        [294] = "WUZIMU",
        [295] = "TORINO",
        [296] = "JIZZY",
        [297] = "MADDOGG",
        [298] = "CAT",
        [299] = "CLAUDE",
    },
    special =
    {
        -- list of SA special peds
        [1]   = "TRUTH",
        [2]   = "MACCER",
        [3]   = "ANDRE",
        [4]   = "BBTHIN",
        [5]   = "BB",
        [6]   = "EMMET",
        [8]   = "JANITOR",
        [42]  = "JETHRO",
        [65]  = "KENDL",
        [86]  = "RYDER1",
        [119] = "SINDACO",
        [149] = "SMOKEV",
        [190] = "COPGRL1",
        [191] = "GUNGRL1",
        [192] = "MECGRL1",
        [193] = "NURGRL1",
        [194] = "CROGRL1",
        [195] = "GANGRL1",
        [208] = "SUZIE",
        [265] = "TENPEN",
        [266] = "PULASKI",
        [267] = "HERN",
        [268] = "DWAYNE",
        [269] = "SMOKE",
        [270] = "SWEET",
        [272] = "FORELLI",
        [273] = "TBONE",
        [289] = "ZERO",
        [290] = "ROSE",
        [291] = "PAUL",
        [292] = "CESAR",
        [293] = "OGLOC",
        [294] = "WUZIMU",
        [295] = "TORINO",
        [296] = "JIZZY",
        [297] = "MADDOGG",
        [298] = "CAT",
        [299] = "CLAUDE",

    },
    models = {},
    search_text = imgui.new.char[20](fconfig.get('tpeds.search_text') or ""),
}

module.tpeds = tpeds


tpeds.type.list = imgui.new['const char*'][#tpeds.type.names](tpeds.type.names)

for i = 0,#tpeds.names,1 do
    table.insert(tpeds.models,i)
end

function module.GetName(model)
    if tpeds.names[model] then return tpeds.names[model] else return "" end
end

function module.GivePedToPlayer(model)

    if fpeds.tpeds.special[model] == nil then
        if isModelAvailable(model) then
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            createChar(tpeds.type.selected[0]+2,model,x,y,z)
            markModelAsNoLongerNeeded(model)
            printHelpString("Ped ~g~Spawned")
        end
    else
        if not hasSpecialCharacterLoaded(290) then
            loadSpecialCharacter(fpeds.tpeds.special[model],1)
            loadAllModelsNow()
        end
        x,y,z = getCharCoordinates(PLAYER_PED)
        createChar(1,290,x,y,z)
        unloadSpecialCharacter(290)
        printHelpString("Ped ~g~Spawned")
    end
end

function module.PedsMain()

    if imgui.BeginTabBar("Peds") then
        imgui.Spacing()
        if imgui.BeginTabItem("Checkboxes") then
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ address = 0x969175,name = "Peds Riot"})
            fcommon.CheckBox({ address = 0x969140,name = "Everyone is armed"})
            fcommon.CheckBox({ address = 0x96913F,name = "Have bounty on head"})
            imgui.NextColumn()
            fcommon.CheckBox({ address = 0x96915D,name = "Slut magnet"})
            fcommon.CheckBox({ address = 0x969157,name = "Elvis everywhere"})
            fcommon.CheckBox({ address = 0x969158,name = "Peds attack with rockets"})

            imgui.Columns(1)
            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            fcommon.RadioButton("Recruit Anyone",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E})

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Menus") then
            fcommon.UpdateAddress({name = "Pedestrian Density",address = 0x8D2530,size = 4,max = 100,help_text = "Sets the pedestrain density in current zone."})
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Spawn") then
            imgui.Spacing()
            if imgui.Combo("Ped type", tpeds.type.selected,tpeds.type.list,#tpeds.type.names) then end
            imgui.Text("Peds list")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginTabBar("Peds List") then
                if imgui.BeginTabItem("Gangs") then
                    if imgui.BeginChild("Gangs list Window") then
                        fcommon.ShowEntries("Ballas",{102,103,104},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Da Nang Boys",{121,122,123},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Groove Families",{105,106,107,269,270,271},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Los Santos Vagos",{108,109,110},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Mafia",{111,112,113},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Russian Mafia",{124,125,126,127},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("San Fierro Rifa",{173,174,175},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("San Fierro Triads",{117,118,120},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Varrios Los Aztecas",{114,115,116},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem("Civillians") then
                    if imgui.BeginChild("Civillians list Window") then
                        fcommon.ShowEntries("Antagonist",{290,291,292,293,294,295,296,297,298,299},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Beach",{18,45,138,139,140,154},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Bouncer",{163,164,165,166},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Boxer",{80,81},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Businessman",{17,141,147,148,150,177,227},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Country",{157,158,159,160,161,162,196,197,198,199,200},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Golf",{36,37},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Grl",{190,191,192,193,194,195},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Heckler",{258,259},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Hippie",{72,73},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Jogger",{90,96},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Karate Student",{203,204},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Pol",{66,67},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Low Class Male",{32,33,34,128,132,133,202},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Low Class Female",{31,129,130,131,151,201},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Mountain Biker",{51,52},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Rich Male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Rich Female",{9,12,40,53,55,88,91,169,215,216,219,224,231},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Roller Blade",{92,99},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Street Male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Street Female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Tramp Male",{78,79,134,135,136,137,212,213,230,239},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Tramp Female",{77,256,257},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Elvis",{82,83,84},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Criminals") then
                    if imgui.BeginChild("Criminals list Window") then
                        fcommon.ShowEntries("Biker",{247,248},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Body Guard",{24,25},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Criminal",{21,47,100,143,181,183,184,223,250},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Drug Dealer",{28,29,30,154},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem("Jobs") then
                    if imgui.BeginChild("Jobs list Window") then
                        fcommon.ShowEntries("Cab Driver",{182,206,220,234,261,262},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Construction",{27,153,260},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Croupier",{11,171,172},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Clothes Seller",{211,217},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Fire Fighter",{277,278,279},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Law Enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Life Guard",{97,251},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Medic",{274,275,276},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Shop Seller",{205,155,156,167,168,176,177,179,180},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Valet",{189,252,},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries("Worker",{16,50,61,253,255},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Misc") then
                    if imgui.BeginChild("Misc list Window") then
                        fcommon.ShowEntries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Search') then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",tpeds.search_text,ffi.sizeof(tpeds.search_text)) then end
			imgui.SameLine()

			imgui.Spacing()
			imgui.Text("Found entries:(" .. ffi.string(tpeds.search_text) .. ")")
			imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Ped Entries") then
                fcommon.ShowEntries(nil,tpeds.models,100,60,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true,tpeds.search_text)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end
return module
