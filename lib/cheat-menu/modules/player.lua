-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019 Grinch_

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

local module = {}

local tplayer =
{
    clothes           = 
    { 
        path          = tcheatmenu.dir .. "clothes\\",
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
}

module.tplayer = tplayer

function module.ChangePlayerModel(model)
    if fped.tpeds.special[model] == nil then
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
        loadSpecialCharacter(fped.tpeds.special[model],1)
        loadAllModelsNow()
        setPlayerModel(PLAYER_HANDLE,290)
    end
    clearCharTasksImmediately(PLAYER_PED)
    printHelpString("Skin changed")
end

function HealthArmour()
    fcommon.DropDownMenu("Health",function()
        local health = imgui.new.int(getCharHealth(PLAYER_PED))

        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. tostring(255))
        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt("Set",health) then
            if health[0] > 100 then
                setFloatStat(24,health[0]*5.686)
                
            else
                setFloatStat(24,569.0)
            end

            setCharHealth(PLAYER_PED,health[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,0)
        end
        imgui.SameLine()
        if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,100)
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
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

    fcommon.DropDownMenu("Armour",function()
        local armour = imgui.new.int()
        local max_armour = 100
        local min_armour = 0
        armour[0] = getCharArmour(PLAYER_PED)

        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. min_armour)
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. max_armour)
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
        imgui.Spacing()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
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
    printHelpString("Clothe changed")
end

function ShowClothes(label,path,body_part,search_text)

    local mask = path .. "*.jpg"
    local handle,file = findFirstFile(mask)
    local model_table = {}

    while handle and file do
        table.insert( model_table,#model_table+1,string.sub( file,1,-5))
        file = findNextFile(handle)
    end
    fcommon.ShowEntries(label,model_table,100,80,tplayer.clothes.images,path,".jpg",fplayer.ChangePlayerClothe,fplayer.GetClotheName,true,nil,body_part,search_text)
end


function WantedLevelMenu()
    
    fcommon.DropDownMenu("Wanted level",function()
        local  _,wl = storeWantedLevel(PLAYER_HANDLE)
        local wanted_level = imgui.new.int(wl)
        local max_wanted_level = imgui.new.int(readMemory(0x58DFE4,1,false))
        
        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. tostring(6))
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)

        if imgui.InputInt("Set",wanted_level) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,wanted_level[0])
        end
        imgui.PopItemWidth()
   
        imgui.Spacing()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,0)
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
            callFunction(0x4396F0,1,0,false)      
            alterWantedLevel(PLAYER_HANDLE,max_wanted_level[0])
        end
    end)
end

function SkinChangerMenu()
    imgui.Spacing()
    fcommon.CheckBox({name = "Aim skin changer",var = tplayer.aimSkinChanger})
    fcommon.InformationTooltip("Aim skin changer tooltip")

    imgui.Spacing()
    if imgui.BeginTabBar("Skins") then
        if imgui.BeginTabItem('Gangs') then
            imgui.Spacing()
            if imgui.BeginChild("Gangs list Window") then
                fcommon.ShowEntries("Ballas",{102,103,104},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("DaNangBoys",{121,122,123},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("GroveStreetFamilies",{105,106,107,269,270,271},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("LosSantosVagos",{108,109,110},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Mafia",{111,112,113,124,125,126,127},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("SanFierroRifa",{173,174,175},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("MountainCloudTriad",{117,118,120},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("VarriosLosAztecas",{114,115,116},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Civilians') then
            imgui.Spacing()
            if imgui.BeginChild("Civilians list Window") then
                fcommon.ShowEntries("Antagonist",{290,291,292,293,294,295,296,297,298,299},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Beach",{18,45,138,139,140,154},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Bouncer",{163,164,165,166},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Boxer",{80,81},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Businessman",{17,141,147,148,150,177,227},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Country",{157,158,159,160,161,162,196,197,198,199,200},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Golf",{36,37},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Grl",{190,191,192,193,194,195},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Heckler",{258,259},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Hippie",{72,73},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Jogger",{90,96},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("KarateStudent",{203,204},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Pol",{66,67},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("LowClassMale",{32,33,34,128,132,133,202},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("LowClassFemale",{31,129,130,131,151,201},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("MountainBiker",{51,52},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("RichMale",{14,20,38,43,46,57,59,94,98,185,186,221,228,235,240,295},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("RichFemale",{9,12,40,53,55,88,91,169,215,216,219,224,231},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("RollerBlade",{92,99},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("StreetMale",{15,22,44,48,58,60,95,101,142,170,188,222,229,236,241,242},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("StreetFemale",{10,13,39,41,54,56,69,76,93,218,225,226,232,233,246,256,257},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("TrampMale",{78,79,134,135,136,137,212,213,230,239},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("TrampFemale",{77,256,257},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Elvis",{82,83,84},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Criminals') then
            imgui.Spacing()
            if imgui.BeginChild("Criminals list Window") then
                fcommon.ShowEntries("Biker",{247,248},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("BodyGuard",{24,25},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Criminal",{21,47,100,143,181,183,184,223,250},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("DrugDealer",{28,29,30,154},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Jobs') then
            imgui.Spacing()
            if imgui.BeginChild("Jobs list Window") then
                fcommon.ShowEntries("CabDriver",{182,206,220,234,261,262},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Construction",{27,153,260},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Croupier",{11,171,172},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("ClothesSeller",{211,217},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("FireFighter",{277,278,279},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("LawEnforcement",{71,265,266,267,280,281,282,283,284,285,286,287,288},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("LifeGuard",{97,251},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Medic",{274,275,276},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Prostitute",{63,64,75,85,87,152,178,207,237,238,243,245,249},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("ShopSeller",{205,155,156,167,168,176,177,179,180},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Valet",{189,252,},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                fcommon.ShowEntries("Worker",{16,50,61,253,255},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Misc") then
            imgui.Spacing()
            if imgui.BeginChild("Other list Window") then
                fcommon.ShowEntries("Misc",{0,1,2,7,19,23,26,35,49,62,68,70,76,144,145,146,209,210,214,263,268,272},110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
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
			imgui.Text("Found entries :(" .. ffi.string(tplayer.skins.search_text) .. ")")
			imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Skin Entries") then
                fcommon.ShowEntries(nil,fped.tpeds.models,110,55,fped.tpeds.images,fped.tpeds.path,".jpg",fplayer.ChangePlayerModel,fped.GetName,true,tplayer.skins.search_text)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end

end

function module.PlayerMain()

    if imgui.BeginTabBar('Player') then

        if imgui.BeginTabItem("Checkbox") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({ address = 0x969179,name = "Aim while driving"})
            fcommon.CheckBox({ var = tplayer.god,name  = "God mode"})
            fcommon.CheckBox({ address = 0x969161,name = "Higher cycle jumps"})
            fcommon.CheckBox({ address = 0x969178,name = "Infinite ammo"}) 
            fcommon.CheckBox({ address = 0x96916E,name = "Infinite oxygen"})
            fcommon.CheckBox({ address = 0xB7CEE4,name = "Infinite run"})
            
            imgui.NextColumn()

            fcommon.CheckBox({ address = getCharPointer(PLAYER_PED)+0x598,name = "Lock player control"})
            fcommon.CheckBox({ address = 0x96916C,name = "Mega jump"})
            fcommon.CheckBox({ address = 0x969173,name = "Mega punch"})
            fcommon.CheckBox({ address = 0x969174,name = "Never get hungry"})
            fcommon.CheckBox({name = "Never wanted",var = tplayer.neverWanted,func = function()
                callFunction(0x4396C0,1,0,false)
                if tplayer.neverWanted[0] then
                    fcommon.CheatActivated()
                else
                    fCheatDeactivated()
                end
            end})
           
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
        if imgui.BeginTabItem("Menu") then
            imgui.Spacing()
            
            fcommon.UpdateStat({ name = "Energy",stat = 165})
            fcommon.UpdateStat({ name = "Fat",stat = 21})
            HealthArmour()
            fcommon.UpdateStat({ name = "Lung capacity",stat = 225})
            fcommon.UpdateAddress({name = "Money",address = 0xB7CE50,size = 4,min = -9999999,max = 9999999})
            fcommon.UpdateStat({ name = "Muscle",stat = 23})
            fcommon.UpdateStat({ name = "Respect",stat = 68,max = 2450}) 
            fcommon.UpdateStat({ name = "Sex appeal",stat = 25})
            fcommon.UpdateStat({ name = "Stamina",stat = 22})
            WantedLevelMenu()

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Skin") then
            SkinChangerMenu()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Clothe") then
            imgui.Spacing()
            if imgui.Button("Remove clothes",imgui.ImVec2(fcommon.GetSize(1))) then
                for i=0, 17 do givePlayerClothes(PLAYER_HANDLE,0,0,i) end
                buildPlayerModel(PLAYER_HANDLE)
            end
            imgui.Spacing()

            if imgui.BeginTabBar("Clothes") then
                imgui.Spacing()
                
                if imgui.BeginTabItem("List") then
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
        imgui.EndTabBar()
    end
end

return module
