-- This module contains all the functions used in peds

local module = {}

local tpeds = 
{
    path = getGameDirectory() .. "\\moonloader\\cheat-menu\\peds\\",
    list = {},
    image_size =
    {
        x = resX/27,
        y = resY/(resY/100),
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
}

module.tpeds = tpeds

function module.get_name(model)
    if tpeds.names[model] then return tpeds.names[model] else return "" end
end

function module.give_ped_to_player(model)

    if fpeds.tpeds.special[model] == nil then      
        if isModelAvailable(model) then 
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            createChar(4,model,x,y,z)
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

function module.peds_section()
    imgui.Text("Options")
    imgui.Separator()
    imgui.Spacing()
    fcommon.radio_menu("Recruit Anyone",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E},"horizantal")
    imgui.Columns(2)
    fcommon.check_box({ address = 0x969175,name = "Peds Riot"})
    fcommon.check_box({ address = 0x969140,name = "Everyone is armed"})
    fcommon.check_box({ address = 0x96913F,name = "Have bounty on head"})
    imgui.NextColumn()
    fcommon.check_box({ address = 0x96915D,name = "Slut magnet"})
    fcommon.check_box({ address = 0x969157,name = "Elvis everywhere"})
    fcommon.check_box({ address = 0x969158,name = "Peds attack with rockets"})
    imgui.Columns(1)
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()
    fcommon.popup_menu({name = "Pedestrian Density",address = 0x8D2530,size = 4,max = 100,help_text = "Sets the pedestrain density in current zone."})
    imgui.Spacing()
    imgui.Text("Peds list")
    imgui.Separator()
    imgui.Spacing()
    if imgui.BeginMenu("Gangs") then
        imgui.Text("Gangs")
        imgui.Separator()
        imgui.Spacing()

        fcommon.show_entries("Ballas",{102,103,104},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Da Nang Boys",{121,122,123},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Groove Families",{105,106,107,269,270,271},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Los Santos Vagos",{108,109,110},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Mafia",{111,112,113},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Russian Mafia",{124,125,126,127},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("San Fierro Rifa",{173,174,175},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("San Fierro Triads",{117,118,120},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Varrios Los Aztecas",{114,115,116},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        imgui.EndMenu()
    end

    if imgui.BeginMenu("Civillians") then  
        imgui.Text("Civillians")
        imgui.Separator()
        imgui.Spacing()
        fcommon.show_entries("Antagonist",{290,291,292,293,294,295,296,297,298,299},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Beach",{18,45,138,139,140,154},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Bouncer",{163,164,165,166},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Boxer",{80,81},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Businessman",{17,141,147,148,150,177,227},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Country",{157,158,159,160,161,162,196,197,198,199,200},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Golf",{36,37},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Grl",{190,191,192,193,194,195},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Heckler",{258,259},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Hippie",{72,73},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Jogger",{90,96},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Karate Student",{203,204},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Pol",{66,67},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Low Class Male",{32,33,34,128,132,133,202},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Low Class Female",{31,129,130,131,151,201},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Mountain Biker",{51,52},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Rich Male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Rich Female",{9,12,40,53,55,88,91,169,215,216,219,224,231},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Roller Blade",{92,99},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Street Male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Street Female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Tramp Male",{78,79,134,135,136,137,212,213,230,239},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Tramp Female",{77,256,257},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Elvis",{82,83,84},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        imgui.EndMenu()
    end
    if imgui.BeginMenu("Crime") then 
        imgui.Text("Crimes")
        imgui.Separator()
        imgui.Spacing() 
        fcommon.show_entries("Biker",{247,248},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Body Guard",{24,25},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Criminal",{21,47,100,143,181,183,184,223,250},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Drug Dealer",{28,29,30,154},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        imgui.EndMenu()
    end

    if imgui.BeginMenu("Jobs") then      
        imgui.Text("Jobs")
        imgui.Separator()
        imgui.Spacing()    
        fcommon.show_entries("Cab Driver",{182,206,220,234,261,262},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Construction",{27,153,260},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Croupier",{11,171,172},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Clothes Seller",{211,217},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Fire Fighter",{277,278,279},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Law Enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Life Guard",{97,251},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Medic",{274,275,276},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Shop Seller",{205,155,156,167,168,176,177,179,180},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Valet",{189,252,},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        fcommon.show_entries("Worker",{16,50,61,253,255},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
        imgui.EndMenu()
    end
    fcommon.show_entries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},5,tpeds.list,tpeds.path,".jpg",tpeds.image_size,fpeds.give_ped_to_player,fpeds.get_name)
end
return module