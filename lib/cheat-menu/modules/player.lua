-- This module contains all the fuction for player tab

local module = {}

local tplayer =
{
    animation = 
    {
        loop             = imgui.new.bool(fconfig.get('tplayer.animation.loop',false)),
        secondary        = imgui.new.bool(fconfig.get('tplayer.animation.secondary',false)),
        search_text      = imgui.new.char[20](),
        name             = imgui.new.char[20](),
        ifp_name         = imgui.new.char[20](),
    },
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
        search_text    = imgui.new.char[20](),
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
        search_text = imgui.new.char[20](),
        names = ftables.stats.table,
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

function AnimationEntry(file,animation)
    if imgui.MenuItemBool(animation)then
        PlayAnimation(file,animation)
    end
end

function PlayAnimation(file,animation)
    requestAnimation(file)
    loadAllModelsNow()
    
    if tplayer.animation.secondary[0] == true then
        taskPlayAnimSecondary(PLAYER_PED,animation,file,4.0,tplayer.animation.loop[0],0,0,0,-1)
    else
        taskPlayAnim(PLAYER_PED,animation,file,4.0,tplayer.animation.loop[0],0,0,0,-1)
    end
    removeAnimation(animation)
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
        if imgui.BeginTabItem(flanguage.GetText("player.Animation")) then
            imgui.Spacing()
            if imgui.Button(flanguage.GetText("player.StopAnimation"),imgui.ImVec2(fcommon.GetSize(1))) then
                clearCharTasks(PLAYER_PED)
                fcommon.CheatActivated()
            end
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({name = flanguage.GetText("player.Loop"),var = tplayer.animation.loop})
            imgui.NextColumn()
            fcommon.CheckBox({name = flanguage.GetText("player.Secondary"),var = tplayer.animation.secondary})
            imgui.Columns(1)
            imgui.Spacing() 

            if imgui.BeginTabBar("Animation") then
                imgui.Spacing()
                if imgui.BeginTabItem(flanguage.GetText("common.List")) then
                    if imgui.BeginChild("Stat Entries") then
                        for key,value in pairs(ftables.animation.table) do
                            AnimationEntry(key,value)
                        end                 
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem(flanguage.GetText("common.Search")) then

                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText(flanguage.GetText("common.Search"),tplayer.animation.search_text,ffi.sizeof(tplayer.animation.search_text)) then end
                    imgui.SameLine()

                    imgui.Spacing()
                    imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tplayer.animation.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Stat Entries") then
                        for key,value in pairs(ftables.animation.table) do
                            if (string.upper(value):find(string.upper(ffi.string(tplayer.animation.search_text)))) then
                                AnimationEntry(key,value)
                            end
                        end       
                        imgui.Spacing()
                        imgui.EndChild()
                    end
                    
                    imgui.EndTabItem()
                end


                if imgui.BeginTabItem(flanguage.GetText("common.Misc")) then
                    if imgui.Combo(flanguage.GetText("player.FightingAnimation"), tplayer.style.fighting.selected,tplayer.style.fighting.list,#tplayer.style.fighting.names) then
                        giveMeleeAttackToChar(PLAYER_PED,tplayer.style.fighting.selected[0]+4,6)
                        fcommon.CheatActivated()
                    end
                    if imgui.Combo(flanguage.GetText("player.WalkingAnimation"), tplayer.style.walking.selected,tplayer.style.walking.list,#tplayer.style.walking.names) then
                        writeMemory(0x609A4E,4,-1869574000,true)
                        writeMemory(0x609A52,2,37008,true)
                        requestAnimation(tplayer.style.walking.names[tplayer.style.walking.selected[0]+1])
                        loadAllModelsNow()
                        setAnimGroupForChar(PLAYER_PED,tplayer.style.walking.names[tplayer.style.walking.selected[0]+1])
                        removeAnimation(tplayer.style.walking.names[tplayer.style.walking.selected[0]+1])
                        fcommon.CheatActivated()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem(flanguage.GetText("common.Custom")) then
                    if imgui.InputText(flanguage.GetText("player.IFPName"),tplayer.animation.ifp_name,ffi.sizeof(tplayer.animation.ifp_name)) then end
                    if imgui.InputText(flanguage.GetText("player.AnimationName"),tplayer.animation.name,ffi.sizeof(tplayer.animation.name)) then end
                    imgui.Spacing()
                    if imgui.Button(flanguage.GetText("player.PlayAnimation"),imgui.ImVec2(fcommon.GetSize(1))) then
                        PlayAnimation(ffi.string(tplayer.animation.ifp_name),ffi.string(tplayer.animation.name))
                    end
                    imgui.EndTabItem()
                end
                
                imgui.EndTabBar()
            end
            
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
