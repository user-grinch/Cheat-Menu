-- This module contains all the functions used in peds

local module = {}

local tpeds =
{
    path   = tcheatmenu.dir .. "peds\\",
    images = {},
    type   =
    {
        list     = {},
        names = ftable.peds.types,
        
        selected = imgui.new.int(fconfig.get('tpeds.type.selected',0)),
    },
    names   = ftable.peds.list,
    special = ftable.peds.special,
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
        end
    else
        if hasSpecialCharacterLoaded(model) then
            unloadSpecialCharacter(model)
        end

        loadSpecialCharacter(tpeds.special[model],1)
        loadAllModelsNow()
        x,y,z = getCharCoordinates(PLAYER_PED)
        createChar(tpeds.type.selected[0]+2,290,x,y,z)
    end
    printHelpString("Ped ~g~Spawned")
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

function module.PedMain()

    if imgui.BeginTabBar("Ped") then
        imgui.Spacing()
        if imgui.BeginTabItem("Checkboxe") then
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ address = 0x969157,name = "Elvis everywhere"})
            fcommon.CheckBox({ address = 0x969140,name = "Everyone is armed"})
            fcommon.CheckBox({ address = 0x96915B,name = "Gangs control the streets"})
            fcommon.CheckBox({ address = 0x96915A,name = "Gang members everywhere"})
            fcommon.CheckBox({ address = 0x96913F,name = "Have bounty on head"})
            imgui.NextColumn()
            fcommon.CheckBox({name = "Gang wars",var = tpeds.gangs.wars,func = function()
                setGangWarsActive(tpeds.gangs.wars[0])
                if tpeds.gangs.wars[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
            end})
            fcommon.CheckBox({ address = 0x969158,name = "Peds attack with rockets"})
            fcommon.CheckBox({ address = 0x969175,name = "Peds riot"})
            fcommon.CheckBox({ address = 0x96915D,name = "Slut magnet"})

            imgui.Columns(1)
            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            fcommon.RadioButton("Recruit anyone",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E})

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Menu") then
            fcommon.UpdateAddress({name = "Pedestrian density",address = 0x8D2530,size = 4,default = 100,max = 100})
            if imgui.CollapsingHeader("Gang zone density") then
                imgui.Separator()
                imgui.PushItemWidth(imgui.GetWindowWidth() - 200)
                SetDensity("Ballas",0)
                SetDensity("Da nang boys",4)
                SetDensity("Gang9",8)
                SetDensity("Gang10",9)
                SetDensity("Grove street families",1)
                SetDensity("Los santos vagos",2)
                SetDensity("Mafia",5)
                SetDensity("Mountain cloud triad",6)
                SetDensity("San fierro rifa",3)
                SetDensity("Varrio los aztecas",7)
                imgui.PopItemWidth()
                imgui.Spacing()
                imgui.Text("Turf Color")
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Spawn") then
            imgui.Spacing()
            if imgui.Combo("Ped type", tpeds.type.selected,tpeds.type.list,#tpeds.type.names) then end
            imgui.Text("List")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginTabBar("Peds list") then
                if imgui.BeginTabItem("Gang") then
                    if imgui.BeginChild("Gangs list Window") then
                        fcommon.ShowEntries("Ballas",{102,103,104},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Da nang boys",{121,122,123},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Grove street families",{105,106,107,269,270,271},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Los santos vagos",{108,109,110},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Mafia",{111,112,113,124,125,126,127},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Mountain cloud triad",{117,118,120},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("San fierro rifa",{173,174,175},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Varrios los aztecas",{114,115,116},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem("Civilians") then
                    if imgui.BeginChild("Civillians list Window") then
                        fcommon.ShowEntries("Antagonist",{290,291,292,293,294,295,296,297,298,299},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Beach",{18,45,138,139,140,154},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Bouncer",{163,164,165,166},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Boxer",{80,81},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Businessman",{17,141,147,148,150,177,227},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Country",{157,158,159,160,161,162,196,197,198,199,200},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Golf",{36,37},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Grl",{190,191,192,193,194,195},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Heckler",{258,259},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Hippie",{72,73},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Jogger",{90,96},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Karate student",{203,204},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Pol",{66,67},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Low class male",{32,33,34,128,132,133,202},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Low class female",{31,129,130,131,151,201},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Mountain biker",{51,52},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Rich male",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Rich female",{9,12,40,53,55,88,91,169,215,216,219,224,231},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Roller blade",{92,99},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Street male",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Street female",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Tramp male",{78,79,134,135,136,137,212,213,230,239},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Tramp female",{77,256,257},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Elvis",{82,83,84},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Criminals") then
                    if imgui.BeginChild("Criminals list Window") then
                        fcommon.ShowEntries("Biker",{247,248},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Body guard",{24,25},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Criminal",{21,47,100,143,181,183,184,223,250},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Drug dealer",{28,29,30,154},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end

                if imgui.BeginTabItem("Jobs") then
                    if imgui.BeginChild("Jobs list Window") then
                        fcommon.ShowEntries("Cab driver",{182,206,220,234,261,262},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Construction",{27,153,260},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Croupier",{11,171,172},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Clothes seller",{211,217},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Fire dighter",{277,278,279},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Law enforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Life guard",{97,251},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Medic",{274,275,276},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Shop seller",{205,155,156,167,168,176,177,179,180},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Valet",{189,252,},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        fcommon.ShowEntries("Worker",{16,50,61,253,255},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Misc") then
                    if imgui.BeginChild("Misc list Window") then
                        fcommon.ShowEntries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true)
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",tpeds.search_text,ffi.sizeof(tpeds.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text("Foun entries :(" .. ffi.string(tpeds.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Ped entries") then
                        fcommon.ShowEntries(nil,tpeds.models,110,55,tpeds.images,tpeds.path,".jpg",module.GivePedToPlayer,module.GetName,true,tpeds.search_text)
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
