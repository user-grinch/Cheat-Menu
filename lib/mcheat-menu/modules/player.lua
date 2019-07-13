-- This module contains all the fuction for player tab

local module = {}

local tplayer =
{
    god         = imgui.new.bool(false),
    aimSkinChanger = imgui.new.bool(false),
    neverWanted    = imgui.new.bool(false),
    cjBody         = imgui.new.int(),
    style =
    {
        fighting =
        {
            selected = imgui.new.int(0),
            names    = {"Default","Boxing","Kung fu","Kick Boxing","Punch Kick"},
            list     = {},
        },
        walking =
        {
            selected = imgui.new.int(0),
            names    = {"man","shuffle","oldman","gang1","gang2","oldfatman","fatman","jogger","drunkman","blindman","swat","woman","shopping","busywoman","sexywoman","pro","oldwoman","fatwoman","jogwoman","oldfatwoman","skate"},
            list     = {},
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
    clearCharTasksImmediately(PLAYER_PED)
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
        if imgui.Checkbox("Never wanted",tplayer.neverWanted) then
            callFunction(0x4396C0,1,0,false)
            if tplayer.neverWanted[0] then
                fcommon.CheatActivated()
            else
                fcommon.CheatDeactivated()
            end
        end
    end)
end

function SkinChangerMenu()
    imgui.Spacing()
    if imgui.Checkbox("Aim Skin Changer",tplayer.aimSkinChanger) then end
    fcommon.InformationTooltip("Aim at a pedestrian and then press Enter/Return key to switch to that skin.")

    imgui.Spacing()
    if imgui.BeginTabBar("Skins") then
        if imgui.BeginTabItem('Gangs') then
            imgui.Spacing()

            fcommon.ShowEntries("Ballas",{102,103,104},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Da Nang Boys",{121,122,123},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Groove Families",{105,106,107,269,270,271},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Los Santos Vagos",{108,109,110},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Mafia",{111,112,113},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Russian Mafia",{124,125,126,127},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("San Fierro Rifa",{173,174,175},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("San Fierro Triads",{117,118,120},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Varrios Los Aztecas",{114,115,116},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Civillians') then
            imgui.Spacing()

            fcommon.ShowEntries("Antagonist",{290,291,292,293,294,295,296,297,298,299},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Beach",{18,45,138,139,140,154},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Bouncer",{163,164,165,166},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Boxer",{80,81},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Businessman",{17,141,147,148,150,177,227},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Country",{157,158,159,160,161,162,196,197,198,199,200},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Golf",{36,37},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Grl",{190,191,192,193,194,195},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Heckler",{258,259},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Hippie",{72,73},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Jogger",{90,96},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Karate Student",{203,204},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Pol",{66,67},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Low Class Male",{32,33,34,128,132,133,202},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Low Class Female",{31,129,130,131,151,201},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Mountain Biker",{51,52},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Rich Male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Rich Female",{9,12,40,53,55,88,91,169,215,216,219,224,231},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Roller Blade",{92,99},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Street Male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Street Female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Tramp Male",{78,79,134,135,136,137,212,213,230,239},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Tramp Female",{77,256,257},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Elvis",{82,83,84},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Criminals') then
            imgui.Spacing()

            fcommon.ShowEntries("Biker",{247,248},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Body Guard",{24,25},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Criminal",{21,47,100,143,181,183,184,223,250},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Drug Dealer",{28,29,30,154},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Jobs') then
            imgui.Spacing()

            fcommon.ShowEntries("Cab Driver",{182,206,220,234,261,262},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Construction",{27,153,260},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Croupier",{11,171,172},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Clothes Seller",{211,217},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Fire Fighter",{277,278,279},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Law Enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Life Guard",{97,251},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Medic",{274,275,276},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Shop Seller",{205,155,156,167,168,176,177,179,180},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Valet",{189,252,},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.ShowEntries("Worker",{16,50,61,253,255},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Other') then
            imgui.Spacing()
            fcommon.ShowEntries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
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
            fcommon.UpdateAddress({name = "Money",address = 0xB7CE50,size = 4,max = 99999999,min = -99999999})
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
        if imgui.BeginTabItem('Girlfriend') then

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
        if imgui.BeginTabItem('Styles') then
            if imgui.BeginTabBar("Styles") then
                FightingStyle()
                WalkingStyle()
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
