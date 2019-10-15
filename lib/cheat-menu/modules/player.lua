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

module.tplayer =
{
    aimSkinChanger    = imgui.new.bool(fconfig.get('tplayer.aimSkinChanger',false)),
    cjBody            = imgui.new.int(fconfig.get('tplayer.cjBody',0)),
    clothes           = 
    { 
        images        = {},
        path          = tcheatmenu.dir .. "clothes\\",
    },
    god               = imgui.new.bool(fconfig.get('tplayer.god',false)),
    never_wanted      = imgui.new.bool(false),
    skins =
    {
        search_text   = imgui.new.char[20](),
    },
}

function module.ChangePlayerModel(model)
    if  fped.tped.names[model] ~= nil then
        if fped.tped.special[model] == nil then
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
            loadSpecialCharacter(fped.tped.special[model],1)
            loadAllModelsNow()
            setPlayerModel(PLAYER_HANDLE,290)
        end
        local car = nil
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)
        end
        clearCharTasksImmediately(PLAYER_PED)
        if car ~= nil then
            taskWarpCharIntoCarAsDriver(PLAYER_PED,car)
        end
        printHelpString("~g~Skin~w~ changed")
    end
end

function HealthArmour()
    fcommon.DropDownMenu("Health",function()
        local health = imgui.new.int(getCharHealth(PLAYER_PED))

        imgui.Columns(2,nil,false)
        imgui.Text("Minimum" .. " = " .. tostring(0))
        imgui.NextColumn()
        imgui.Text("Maximum" .. " = " .. tostring(255))
        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-70)
        if imgui.InputInt("Set ##Health",health) then
            if health[0] > 100 then
                setFloatStat(24,health[0]*5.686)
                
            else
                setFloatStat(24,569.0)
            end

            setCharHealth(PLAYER_PED,health[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Minimum ##Health",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,0)
        end
        imgui.SameLine()
        if imgui.Button("Default ##Health",imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(24,569.0)
            setCharHealth(PLAYER_PED,100)
        end
        imgui.SameLine()
        if imgui.Button("Maximum ##Health",imgui.ImVec2(fcommon.GetSize(3))) then
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

        imgui.PushItemWidth(imgui.GetWindowWidth()-70)
        if imgui.InputInt("Set ##Armour",armour) then

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
        if imgui.Button("Minimum ##Armour",imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button("Default ##Armour",imgui.ImVec2(fcommon.GetSize(3))) then
            damageChar(PLAYER_PED,  getCharArmour(PLAYER_PED),true)
        end
        imgui.SameLine()
        if imgui.Button("Maximum ##Armour",imgui.ImVec2(fcommon.GetSize(3))) then
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
    printHelpString("Clothes changed")
end

function ShowClothes(label,path,body_part,search_text)

    local mask = path .. "*.jpg"
    local handle,file = findFirstFile(mask)
    local model_table = {}

    while handle and file do
        table.insert( model_table,#model_table+1,string.sub( file,1,-5))
        file = findNextFile(handle)
    end
    fcommon.ShowEntries(label,model_table,100,80,module.tplayer.clothes.images,path,".jpg",fplayer.ChangePlayerClothe,fplayer.GetClotheName,true,nil,body_part,search_text)
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
    fcommon.CheckBox({name = "Aim skin changer",var = module.tplayer.aimSkinChanger})
    fcommon.InformationTooltip("Aim ped with a gun & press enter")

    imgui.Spacing()
    if imgui.BeginTabBar("Skins") then

        for dir in lfs.dir(fped.tped.path) do
            if doesDirectoryExist(fped.tped.path .. dir) and dir ~= "." and dir ~= ".." then
                if imgui.BeginTabItem(dir) then
                    imgui.Spacing()
                    if imgui.BeginChild("Skins##" .. dir) then
                        for subdir in lfs.dir(fped.tped.path .. dir) do
                            if doesDirectoryExist(fped.tped.path .. dir .. "\\" .. subdir) and subdir ~= "." and subdir ~= ".." then
                                list = {}
                                for pic in lfs.dir(fped.tped.path .. dir .. "\\" .. subdir) do
                                    if doesFileExist(fped.tped.path .. dir .. "\\" .. subdir .. "\\" .. pic) then
                                        table.insert(list,tonumber(string.sub(pic,1,-5)))
                                    end
                                end
                                fcommon.ShowEntries(subdir,list,110,55,fped.tped.images,(fped.tped.path .. dir .. "\\" .. subdir .. "\\"),".jpg",fplayer.ChangePlayerModel,fped.GetName,true)
                            end
                        end
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
            end
        end
        if imgui.BeginTabItem("Search") then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",module.tplayer.skins.search_text,ffi.sizeof(module.tplayer.skins.search_text)) then end
            imgui.SameLine()

            imgui.Spacing()
            imgui.Text("Found skins :(" .. ffi.string(module.tplayer.skins.search_text) .. ")")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Skin entries") then
                lua_thread.create(function()
                    for dir in lfs.dir(fped.tped.path) do
                        if doesDirectoryExist(fped.tped.path .. dir) and dir ~= "." and dir ~= ".." then  
                            for subdir in lfs.dir(fped.tped.path .. dir) do
                                if doesDirectoryExist(fped.tped.path .. dir .. "\\" .. subdir) and subdir ~= "." and subdir ~= ".." then
                                    for pic in lfs.dir(fped.tped.path .. dir .. "\\" .. subdir) do
                                        wait(0)
                                        if doesFileExist(fped.tped.path .. dir .. "\\" .. subdir .. "\\" .. pic) then
                                            temp = {}
                                            model = tonumber(string.sub(pic,1,-5))
                                            for key, value in pairs(fped.tped.model_list) do
                                                if value == model then
                                                    model = nil
                                                end
                                            end
                                            table.insert(fped.tped.model_list,model)
                                            table.insert(temp,model)
                                            fcommon.LoadTextures(fped.tped.images,(fped.tped.path .. dir .. "\\" .. subdir .. "\\"),temp,".jpg")
                                        end
                                    end
                                end
                            end
                        end
                    end
                end)
                fcommon.ShowEntries(nil,fped.tped.model_list,110,55,fped.tped.images,nil,".jpg",fplayer.ChangePlayerModel,fped.GetName,true,module.tplayer.skins.search_text)
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
            fcommon.CheckBox({ var = module.tplayer.god,name  = "God mode"})
            fcommon.CheckBox({ address = 0x969161,name = "Higher cycle jumps"})
            fcommon.CheckBox({ address = 0x969178,name = "Infinite ammo"}) 
            fcommon.CheckBox({ address = 0x96916E,name = "Infinite oxygen"})
            fcommon.CheckBox({ address = 0xB7CEE4,name = "Infinite run"})
            
            imgui.NextColumn()

            fcommon.CheckBox({ address = getCharPointer(PLAYER_PED)+0x598,name = "Lock player control"})
            fcommon.CheckBox({ address = 0x96916C,name = "Mega jump"})
            fcommon.CheckBox({ address = 0x969173,name = "Mega punch"})
            fcommon.CheckBox({ address = 0x969174,name = "Never get hungry"})

            module.tplayer.never_wanted[0] = readMemory(0x969171 ,1,false)
            fcommon.CheckBox({name = "Never wanted",var = module.tplayer.never_wanted,func = function()
                callFunction(0x4396C0,1,0,false)
                if module.tplayer.never_wanted[0] then
                    fcommon.CheatActivated()
                else
                    fcommon.CheatDeactivated()
                end
            end})
           
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            imgui.Text("Body")
            if imgui.RadioButtonIntPtr("Fat",module.tplayer.cjBody,1) then
                callFunction(0x439110,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr("Muscle",module.tplayer.cjBody,2) then
                -- body not changing to muscular after changing to fat fix
                callFunction(0x439190,1,1,false)
                callFunction(0x439150,1,1,false)
                fcommon.CheatActivated()
            end
            if imgui.RadioButtonIntPtr("Skinny",module.tplayer.cjBody,3) then
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
            fcommon.UpdateStat({ name = "Stamina",stat = 22})
            WantedLevelMenu()

            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Skin") then
            SkinChangerMenu()
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Clothes") then
            imgui.Spacing()
            if imgui.Button("Remove clothes",imgui.ImVec2(fcommon.GetSize(1))) then
                for i=0, 17 do givePlayerClothes(PLAYER_HANDLE,0,0,i) end
                buildPlayerModel(PLAYER_HANDLE)
                printHelpString("Clothes ~r~removed")
            end
            imgui.Spacing()

            if imgui.BeginTabBar("Clothes") then
                imgui.Spacing()
                
                if imgui.BeginTabItem("List") then
                    if imgui.BeginChild("Clothes") then
        
                        ShowClothes("Extras",fplayer.tplayer.clothes.path .. "Extras\\",17)   
                        ShowClothes("Glasses",fplayer.tplayer.clothes.path .. "Glasses\\",15)
                        ShowClothes("Hats",fplayer.tplayer.clothes.path .. "Hats\\",16)
                        ShowClothes("Heads",fplayer.tplayer.clothes.path .. "Heads\\",1)
                        ShowClothes("Necklaces",fplayer.tplayer.clothes.path .. "Necklaces\\",13)
                        ShowClothes("Shirts",fplayer.tplayer.clothes.path .. "Shirts\\",0)
                        ShowClothes("Shoes",fplayer.tplayer.clothes.path .. "Shoes\\",3)
                        ShowClothes("Tattoos back",fplayer.tplayer.clothes.path .. "Tattoos back\\",8)
                        ShowClothes("Tattoos left chest",fplayer.tplayer.clothes.path .. "Tattoos left chest\\",9)
                        ShowClothes("Tattoos left lower arm",fplayer.tplayer.clothes.path .. "Tattoos left lower arm\\",4)
                        ShowClothes("Tattoos left upper arm",fplayer.tplayer.clothes.path .. "Tattoos left upper arm\\",5)
                        ShowClothes("Tattoos lower back",fplayer.tplayer.clothes.path .. "Tattoos lower back\\",12)  
                        ShowClothes("Tattoos right chest",fplayer.tplayer.clothes.path .. "Tattoos right chest\\",10)
                        ShowClothes("Tattoos right lower arm",fplayer.tplayer.clothes.path .. "Tattoos right lower arm\\",7)
                        ShowClothes("Tattoos right upper arm",fplayer.tplayer.clothes.path .. "Tattoos right upper arm\\",6)
                        ShowClothes("Tattoos stomach",fplayer.tplayer.clothes.path .. "Tattoos stomach\\",11)
                        ShowClothes("Trousers",fplayer.tplayer.clothes.path .. "Trousers\\",2)
                        ShowClothes("Watches",fplayer.tplayer.clothes.path .. "Watches\\",14)
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
