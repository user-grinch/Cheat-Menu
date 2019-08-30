-- This module contains all the functions used in peds

local module = {}

local tpeds =
{
    path   = tcheatMenu.dir .. "peds\\",
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
        selected = imgui.new.int(fconfig.get('tpeds.type.selected',0)),
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
        [265] = "TENPEN",
        [266] = "PULASKI",
        [267] = "HERN",
        [268] = "DWAYNE",
        [269] = "SMOKE",
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
    search_text = imgui.new.char[20](fconfig.get('tpeds.search_text',"")),  
    gangs =
    {
        wars = imgui.new.bool(false),
    },
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
    if tpeds.special[model] == nil then
        if isModelAvailable(model) then
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            createChar(tpeds.type.selected[0]+2,model,x,y,z)
            markModelAsNoLongerNeeded(model)
            printHelpString(flanguage.GetText('peds.PedSpawnedMSG'))
        end
    else
        if hasSpecialCharacterLoaded(model) then
            unloadSpecialCharacter(model)
        end

        loadSpecialCharacter(tpeds.special[model],1)
        loadAllModelsNow()
        x,y,z = getCharCoordinates(PLAYER_PED)
        createChar(tpeds.type.selected[0]+2,290,x,y,z)
        printHelpString(flanguage.GetText('peds.PedSpawnedMSG'))
    end
end

function SetDensity(title,id)
    local x,y,z = getCharCoordinates(PLAYER_PED)
        
    local density = imgui.new.int(getZoneGangStrength(getNameOfInfoZone(x,y,z),id))
    if imgui.SliderInt(title,density,0,255) then
        setZoneGangStrength(getNameOfInfoZone(x,y,z),id,density[0])
        clearSpecificZonesToTriggerGangWar()
        setGangWarsActive(true)
    end
end

function module.PedsMain()

    if imgui.BeginTabBar("Peds") then
        imgui.Spacing()
        if imgui.BeginTabItem(flanguage.GetText("common.Checkboxes")) then
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ address = 0x969157,name = flanguage.GetText("peds.ElvisEverywhere")})
            fcommon.CheckBox({ address = 0x969140,name = flanguage.GetText("peds.EveryoneIsArmed")})
            fcommon.CheckBox({ address = 0x96915B,name = flanguage.GetText("peds.GangsControlTheStreets")})
            fcommon.CheckBox({ address = 0x96915A,name = flanguage.GetText("peds.GangMembersEverywhere")})
            fcommon.CheckBox({ address = 0x96913F,name = flanguage.GetText("peds.HaveBountyOnHead")})
            imgui.NextColumn()
            fcommon.CheckBox({name = flanguage.GetText("peds.GangWars"),var = tpeds.gangs.wars,func = function()
                setGangWarsActive(tpeds.gangs.wars[0])
                if tpeds.gangs.wars[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
            end})
            fcommon.CheckBox({ address = 0x969158,name = flanguage.GetText("peds.PedsAttackWithRockets")})
            fcommon.CheckBox({ address = 0x969175,name = flanguage.GetText("peds.PedsRiot")})
            fcommon.CheckBox({ address = 0x96915D,name = flanguage.GetText("peds.SlutMagnet")})

            imgui.Columns(1)
            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            fcommon.RadioButton(flanguage.GetText("peds.RecruitAnyone"),{flanguage.GetText("peds.9mm"),flanguage.GetText("peds.AK47"),flanguage.GetText("peds.Rockets")},{0x96917C,0x96917D,0x96917E})

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("common.Menus")) then
            fcommon.UpdateAddress({name = flanguage.GetText("peds.PedestrianDensity"),address = 0x8D2530,size = 4,default = 100,max = 100})
            if imgui.CollapsingHeader(flanguage.GetText("peds.ZoneGangDensity")) then
                imgui.Separator()
                imgui.PushItemWidth(imgui.GetWindowWidth() - 200)
                SetDensity(flanguage.GetText("peds.Ballas"),0)
                SetDensity(flanguage.GetText("peds.DaNangBoys"),4)
                SetDensity(flanguage.GetText("peds.Gang9"),8)
                SetDensity(flanguage.GetText("peds.Gang10"),9)
                SetDensity(flanguage.GetText("peds.GroveStreetFamilies"),1)
                SetDensity(flanguage.GetText("peds.LosSantosVagos"),2)
                SetDensity(flanguage.GetText("peds.Mafia"),5)
                SetDensity(flanguage.GetText("peds.MountainCloudTriad"),6)
                SetDensity(flanguage.GetText("peds.SanFierroRifa"),3)
                SetDensity(flanguage.GetText("peds.VarrioLosAztecas"),7)
                imgui.PopItemWidth()
                imgui.Spacing()
                imgui.Text(flanguage.GetText("peds.TurfColor"))
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem(flanguage.GetText("common.Spawn")) then
            imgui.Spacing()
            if imgui.Combo(flanguage.GetText("peds.PedType"), tpeds.type.selected,tpeds.type.list,#tpeds.type.names) then end
            imgui.Text(flanguage.GetText("common.List"))
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginTabBar("PedsList") then
                if imgui.BeginTabItem(flanguage.GetText("peds.Gangs")) then
                    if imgui.BeginChild("Gangs list Window") then
                        fcommon.ShowEntries(flanguage.GetText("peds.Ballas"),{102,103,104},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.DaNangBoys"),{121,122,123},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.GroveStreetFamilies"),{105,106,107,269,270,271},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.LosSantosVagos"),{108,109,110},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Mafia"),{111,112,113,124,125,126,127},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.MountainCloudTriad"),{117,118,120},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.SanFierroRifa"),{173,174,175},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.VarriosLosAztecas"),{114,115,116},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem(flanguage.GetText("peds.Civilians")) then
                    if imgui.BeginChild("Civillians list Window") then
                        fcommon.ShowEntries(flanguage.GetText("peds.Antagonist"),{290,291,292,293,294,295,296,297,298,299},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Beach"),{18,45,138,139,140,154},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Bouncer"),{163,164,165,166},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Boxer"),{80,81},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Businessman"),{17,141,147,148,150,177,227},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Country"),{157,158,159,160,161,162,196,197,198,199,200},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Golf"),{36,37},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Grl"),{190,191,192,193,194,195},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Heckler"),{258,259},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Hippie"),{72,73},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Jogger"),{90,96},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.KarateStudent"),{203,204},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Pol"),{66,67},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.LowClassMale"),{32,33,34,128,132,133,202},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.LowClassFemale"),{31,129,130,131,151,201},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.MountainBiker"),{51,52},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.RichMale"),{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.RichFemale"),{9,12,40,53,55,88,91,169,215,216,219,224,231},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.RollerBlade"),{92,99},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.StreetMale"),{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.StreetFemale"),{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.TrampMale"),{78,79,134,135,136,137,212,213,230,239},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.TrampFemale"),{77,256,257},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Elvis"),{82,83,84},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem(flanguage.GetText("peds.Criminals")) then
                    if imgui.BeginChild("Criminals list Window") then
                        fcommon.ShowEntries(flanguage.GetText("peds.Biker"),{247,248},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.BodyGuard"),{24,25},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Criminal"),{21,47,100,143,181,183,184,223,250},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.DrugDealer"),{28,29,30,154},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem(flanguage.GetText("peds.Jobs")) then
                    if imgui.BeginChild("Jobs list Window") then
                        fcommon.ShowEntries(flanguage.GetText("peds.CabDriver"),{182,206,220,234,261,262},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Construction"),{27,153,260},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Croupier"),{11,171,172},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.ClothesSeller"),{211,217},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.FireFighter"),{277,278,279},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.LawEnforcement"),{71,265,266,267,280,281,282,283,284,285,286,287,288},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.LifeGuard"),{97,251},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Medic"),{274,275,276},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Prostitute"),{63,64,75,85,87,152,178,207,237,238,243,245,249},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.ShopSeller"),{205,155,156,167,168,176,177,179,180},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Valet"),{189,252,},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        fcommon.ShowEntries(flanguage.GetText("peds.Worker"),{16,50,61,253,255},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem(flanguage.GetText("common.Misc")) then
                    if imgui.BeginChild("Misc list Window") then
                        fcommon.ShowEntries(flanguage.GetText("common.Misc"),{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem(flanguage.GetText("common.Search")) then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText(flanguage.GetText("common.Search"),tpeds.search_text,ffi.sizeof(tpeds.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tpeds.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Ped entries") then
                        fcommon.ShowEntries(nil,tpeds.models,110,55,tpeds.images,tpeds.path,".jpg",fpeds.GivePedToPlayer,fpeds.GetName,true,tpeds.search_text)
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
