-- This module contains all the fuction for player tab

local module = {}

local cj_body = imgui.ImInt()
local aim_skin_changer = imgui.ImBool(false)
local player_proofs = imgui.ImBool(false)
local fighting_style = 0
local walking_style = 0
local never_wanted = imgui.ImBool(false)

module.aim_skin_changer = aim_skin_changer
module.player_proofs = player_proofs

function module.change_player_model(model)
    if fpeds.tpeds.special[model] == nil then      
        if isModelAvailable(model) then 
            requestModel(model)
            loadAllModelsNow()
            setPlayerModel(PLAYER_HANDLE,model)
            clearCharTasksImmediately(PLAYER_PED)
            markModelAsNoLongerNeeded(model)
            printHelpString("Skin ~g~Changed")
        end
    else
        if not hasSpecialCharacterLoaded(290) then 
            loadSpecialCharacter(fpeds.tpeds.special[model],1)
            loadAllModelsNow()
        end	
        setPlayerModel(PLAYER_HANDLE,290)
        clearCharTasksImmediately(PLAYER_PED)
        unloadSpecialCharacter(290)
        printHelpString("Skin ~g~Changed")
    end
end

function module.health_armour()
    imgui.Spacing()
    if imgui.BeginMenu("Health") then
        imgui.Spacing()
        local health = imgui.ImInt()
        local max = math.floor(getFloatStat(24)/5.68)

        health.v = getCharHealth(PLAYER_PED)
        imgui.Text("Health")
        imgui.Separator()
        imgui.Spacing()
        imgui.Text("Current = " .. health.v .." Max = " ..  max .. " Min = 0")
        
        if imgui.InputInt("Set",health) then
            setCharHealth(PLAYER_PED,health.v)
            
        end
        if imgui.Button("Increase") and health.v <  max then
            setCharHealth(PLAYER_PED,(health.v+10))
        end
        imgui.SameLine()
        if imgui.Button("Decrease")  and health.v > 0 then
            setCharHealth(PLAYER_PED,(health.v-10))
        end
        imgui.SameLine()
        if imgui.Button("Maximum") then
            setCharHealth(PLAYER_PED, max)
        end
        imgui.SameLine()
        if imgui.Button("Minimum") then
            setCharHealth(PLAYER_PED,0)
        end

        if health.v < 0 then   
            setCharHealth(PLAYER_PED,0)    
        end

        if health.v >  max then
            setCharHealth(PLAYER_PED, max) 
        end
        
        imgui.EndMenu()
        imgui.Spacing() 
    end 
    if imgui.BeginMenu("Armour") then
        imgui.Spacing()
        local armour = imgui.ImInt()
        local max_armour = 150
        armour.v = getCharArmour(PLAYER_PED)
        imgui.Text("Armour")
        imgui.Separator()
        imgui.Spacing()
        imgui.Text("Current = " .. armour.v .." Max = " ..  max_armour .. " Min = 0")
        
        if imgui.InputInt("Set",armour) then

            if armour.v < 0 then
                armour.v = 0
            end
            if getCharArmour(PLAYER_PED) < armour.v then
                addArmourToChar(PLAYER_PED,(armour.v-getCharArmour(PLAYER_PED)))
            end
            if getCharArmour(PLAYER_PED) > armour.v then
                damageChar(PLAYER_PED,getCharArmour(PLAYER_PED)-armour.v,true)
            end
        end
        if imgui.Button("Increase") and armour.v <  max_armour then
            addArmourToChar(PLAYER_PED,10)
        end
        imgui.SameLine()
        if imgui.Button("Decrease")  and armour.v > 0 then
           

            if getCharArmour(PLAYER_PED) > 10 then
                damageChar(PLAYER_PED,10,1)
            else
                damageChar(PLAYER_PED,getCharArmour(PLAYER_PED),1)
            end
        end
        imgui.SameLine()
        if imgui.Button("Maximum") then
            addArmourToChar(PLAYER_PED, max_armour)
        end
        imgui.SameLine()
        if imgui.Button("Minimum") then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        
        imgui.EndMenu()
    end 
end

function wanted_level_menu()
    if imgui.BeginMenu("Wanted Level") then
        imgui.Spacing()
        imgui.Text("Set Max Wanted Level")
        imgui.Separator()
        imgui.Spacing()

        local CPlayer = getCharPointer(PLAYER_PED)  
        local  _,wl = storeWantedLevel(PLAYER_HANDLE)
        local wanted_level = imgui.ImInt(wl)
        local max_wanted_level = imgui.ImInt(readMemory(0x58DFE4,1,false))

        if imgui.SliderInt("Max",max_wanted_level,0,6) then
           writeMemory(0x58DFE4,1,max_wanted_level.v,false)
        end

        imgui.Spacing()
        imgui.Text("Set Current Wanted Level")
        imgui.Separator()
        imgui.Spacing()

        if imgui.SliderInt("Set",wanted_level,0,max_wanted_level.v) then
            callFunction(0x4396F0,1,0,false)
            alterWantedLevel(PLAYER_HANDLE,wanted_level.v)
        end
        
        if imgui.Button("Maximum") then
            callMethod(0x609F50 ,CPlayer,2,2,max_wanted_level.v)
        end
        imgui.SameLine()
        if imgui.Button("Minimum") then
            callMethod(0x609F50 ,CPlayer,2,2,0)
        end
        imgui.SameLine()
        if imgui.Checkbox("Never wanted",never_wanted) then
            callFunction(0x4396C0,1,0,false)
            if never_wanted.v then
                fcommon.CheatActivated()
            else
                fcommon.CheatDeactivated()
            end
        end
        imgui.EndMenu()
    end
end

function gf_menu()
    if imgui.BeginMenu("Girlfriend",true) then
        imgui.Spacing()
        imgui.Text("Status")
        imgui.Separator()
        imgui.Spacing()
        if imgui.Button("Set Max GF Progress") then
            for i=252,257,1 do
                setFloatStat(i,100)
            end
            clearHelp()
            printHelpString("Girlfriend progress set to maximum")
        end
        imgui.Spacing()
        imgui.Text("Girlfriends")
        imgui.Separator()
        imgui.Spacing()
        fcommon.update_stat({ name = "Denise",stat = 252,max = 100})
        fcommon.update_stat({ name = "Michelle",stat = 253,max = 100})
        fcommon.update_stat({ name = "Helena",stat = 254,max = 100})
        fcommon.update_stat({ name = "Barbara",stat = 255,max = 100})
        fcommon.update_stat({ name = "Katie",stat = 256,max = 100})
        fcommon.update_stat({ name = "Millie",stat = 257,max = 100})
        imgui.Spacing()
        imgui.Text("Other Stats")
        imgui.Separator()
        imgui.Spacing()
        fcommon.update_stat({ name = "GF Count",stat = 184})
        fcommon.update_stat({ name = "Disastrous Dates",stat = 185})
        fcommon.update_stat({ name = "Girls Dated",stat = 186})
        fcommon.update_stat({ name = "Scored Dates",stat = 187})
        fcommon.update_stat({ name = "Successful Dates",stat = 188})
        fcommon.update_stat({ name = "Girls Dumped",stat = 189})
        fcommon.update_stat({ name = "Prostitutes Visited",stat = 190})
        imgui.EndMenu()
    end
end


skin_entry = function(model)

    if fpeds.fpeds.tpeds.images[tostring(model)] == nil then
        lua_thread.create(fcommon.load_texture,fpeds.fpeds.tpeds.images,fpeds.fpeds.tpeds.path,model,".jpg")
        while fpeds.fpeds.tpeds.images[tostring(model)] == nil do
            wait(0)
        end
    end

    if imgui.ImageButton(fpeds.fpeds.tpeds.images[tostring(model)],imgui.ImVec2(fpeds.fpeds.tpeds.image_size.x,fpeds.fpeds.tpeds.image_size.y)) then 
        change_player_model(model)
    end
    if fpeds.tpeds.all[model] ~= nil and imgui.IsItemHovered() then
        imgui.BeginTooltip() 
        imgui.SetTooltip(fpeds.tpeds.all[model])
        imgui.EndTooltip()
    end
end

function skin_changer_menu()
    if imgui.BeginMenu("Skin Changer",true) then
        imgui.Spacing()
        imgui.Text("Skin Changer")
        imgui.Separator()
        imgui.Spacing()
        imgui.Spacing()
        if imgui.Checkbox("Aim Skin Changer",aim_skin_changer) then end 
        fcommon.information_tooltip("Aim at a pedestrian and then press Enter/Return key to switch to that skin.")
        imgui.Text("Skin list")
        imgui.Separator()
        imgui.Spacing()
        if imgui.BeginMenu("Gangs") then
            imgui.Text("Gangs")
            imgui.Separator()
            imgui.Spacing()
    
            fcommon.show_entries("Ballas",{102,103,104},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Da Nang Boys",{121,122,123},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Groove Families",{105,106,107,269,270,271},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Los Santos Vagos",{108,109,110},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Mafia",{111,112,113},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Russian Mafia",{124,125,126,127},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("San Fierro Rifa",{173,174,175},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("San Fierro Triads",{117,118,120},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Varrios Los Aztecas",{114,115,116},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndMenu()
        end
    
        if imgui.BeginMenu("Civillians") then  
            imgui.Text("Civillians")
            imgui.Separator()
            imgui.Spacing()
            fcommon.show_entries("Antagonist",{290,291,292,293,294,295,296,297,298,299},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Beach",{18,45,138,139,140,154},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Bouncer",{163,164,165,166},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Boxer",{80,81},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Businessman",{17,141,147,148,150,177,227},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Country",{157,158,159,160,161,162,196,197,198,199,200},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Golf",{36,37},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Grl",{190,191,192,193,194,195},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Heckler",{258,259},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Hippie",{72,73},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Jogger",{90,96},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Karate Student",{203,204},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Pol",{66,67},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Low Class Male",{32,33,34,128,132,133,202},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Low Class Female",{31,129,130,131,151,201},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Mountain Biker",{51,52},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Rich Male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Rich Female",{9,12,40,53,55,88,91,169,215,216,219,224,231},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Roller Blade",{92,99},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Street Male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Street Female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Tramp Male",{78,79,134,135,136,137,212,213,230,239},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Tramp Female",{77,256,257},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Elvis",{82,83,84},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndMenu()
        end
        if imgui.BeginMenu("Crime") then 
            imgui.Text("Crimes")
            imgui.Separator()
            imgui.Spacing() 
            fcommon.show_entries("Biker",{247,248},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Body Guard",{24,25},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Criminal",{21,47,100,143,181,183,184,223,250},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Drug Dealer",{28,29,30,154},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndMenu()
        end
    
        if imgui.BeginMenu("Jobs") then      
            imgui.Text("Jobs")
            imgui.Separator()
            imgui.Spacing()    
            fcommon.show_entries("Cab Driver",{182,206,220,234,261,262},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Construction",{27,153,260},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Croupier",{11,171,172},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Clothes Seller",{211,217},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Fire Fighter",{277,278,279},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Law Enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Life Guard",{97,251},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Medic",{274,275,276},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Shop Seller",{205,155,156,167,168,176,177,179,180},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Valet",{189,252,},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            fcommon.show_entries("Worker",{16,50,61,253,255},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
            imgui.EndMenu()
        end
        fcommon.show_entries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},5,fpeds.tpeds.images,fpeds.tpeds.path,".jpg",fpeds.tpeds.image_size,fplayer.change_player_model,fpeds.get_name,true)
    
        imgui.EndMenu()
    end
end

function func_walking_style(list)
    if imgui.BeginMenu("Walking Styles") then
        imgui.Text("Select Style")
        fcommon.information_tooltip("Select player walking style.")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()

        for i = 1,#list,1 do
            
            local bool ,text
            if i == walking_style then
                text = "     Selected"
                bool = true
            else
                text = ""
                bool = false
            end
            if imgui.MenuItem(list[i]:gsub("^%l", string.upper),text,bool,true) then
                writeMemory(0x609A4E,4,-1869574000,true)
                writeMemory(0x609A52,2,37008,true)
                requestAnimation(list[i])
                loadAllModelsNow()
                setAnimGroupForChar(PLAYER_PED,list[i])
                removeAnimation(list[i])
                walking_style = i
                fcommon.CheatActivated()
            end
        end
        imgui.EndMenu()
    end
end

function func_fighting_style(list)
    if imgui.BeginMenu("Fighting Styles") then
        imgui.Text("Select Style")
        fcommon.information_tooltip("Select player fighting style.")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()

        for i = 1,#list,1 do
            local bool ,text
            if i == fighting_style then
                text = "     Selected"
                bool = true
            else
                text = ""
                bool = false
            end
            if imgui.MenuItem(list[i],text,bool,true) then
                giveMeleeAttackToChar(PLAYER_PED,i+3,6)
                fighting_style = i
                fcommon.CheatActivated()
            end
        end
        imgui.EndMenu()
    end
end

function module.player_section()
    imgui.Spacing()
    imgui.Text("Options")
    imgui.Separator()
    imgui.Spacing()
    imgui.Columns(2)

    if imgui.Checkbox("God Mode",player_proofs) then fcommon.CheatActivated() end 
    
    fcommon.check_box({ address = 0xB7CEE4,name = "Infinite run"})
    fcommon.check_box({ address = 0x969178,name = "Infinite ammo"})
    fcommon.check_box({ address = 0x96916D,name = "Infinite health"})
    fcommon.check_box({ address = 0x96916E,name = "Infinite oxygen"})
    fcommon.check_box({ address = getCharPointer(PLAYER_PED)+0x598,name = "Lock player control"})

    imgui.NextColumn()

    fcommon.check_box({ address = 0xB7CEE6,name = "Fireproof",help_text = "Player is immue to fire."})
    fcommon.check_box({ address = 0x96916C,name = "Mega jump"})
    fcommon.check_box({ address = 0x969173,name = "Mega punch"})
    fcommon.check_box({ address = 0x969161,name = "Huge bunny hop",help_text = "Huge cycle jumps."})
    fcommon.check_box({ address = 0x969174,name = "Never get hungry"})
    fcommon.check_box({ address = 0x969179,name = "Aim while driving",help_text = "Player can shoot while driving."}) 
    imgui.Columns(1)
    imgui.Spacing()
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()

    -- Body -------------------------------------
    imgui.Text("Body")
    imgui.SameLine()
    if imgui.RadioButton("Fat",cj_body,1) then
        callFunction(0x439110,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.RadioButton("Muscle",cj_body,2) then
        callFunction(0x439150,1,1,false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.RadioButton("Skinny",cj_body,3) then
        callFunction(0x439190,1,1,false)
        fcommon.CheatActivated()
    end
    ----------------------------------------------


    fcommon.popup_menu({name = "Money",address = 0xB7CE50,size = 4,max = 99999999,min = -99999999})
    module.health_armour()
    fcommon.update_stat({ name = "Energy",stat = 165})
    fcommon.update_stat({ name = "Max Health",stat = 24,max = 1450})
    wanted_level_menu()

    fcommon.update_stat({ name = "Lung Capacity",stat = 225})
    fcommon.update_stat({ name = "Fat",stat = 21})
    fcommon.update_stat({ name = "Stamina",stat = 22})
    fcommon.update_stat({ name = "Muscle",stat = 23})
    fcommon.update_stat({ name = "Sex Appeal",stat = 25})
    fcommon.update_stat({ name = "Respect",stat = 68,max = 2450})
    
    gf_menu()
    skin_changer_menu()
    func_fighting_style({"Default","Boxing","Kung fu","Kick Boxing","Punch Kick"})
    func_walking_style({"man","shuffle","oldman","gang1","gang2","oldfatman","fatman","jogger","drunkman","blindman","swat","woman","shopping","busywoman","sexywoman","pro","oldwoman","fatwoman","jogwoman","oldfatwoman","skate"})
end

return module