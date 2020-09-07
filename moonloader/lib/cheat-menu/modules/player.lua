-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019-2020 Grinch_

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
    aimSkinChanger      = imgui.new.bool(fconfig.Get('tplayer.aimSkinChanger',false)),
    cjBody              = imgui.new.int(fconfig.Get('tplayer.cjBody',0)),
    clothes             = 
    { 
        images          = {},
        path            = tcheatmenu.dir .. "clothes\\",
    },
    custom_skins        =
    {
        filter          = imgui.ImGuiTextFilter(),
        is_modloader_installed = false,
        names           = {},
        path            = string.format("%s\\modloader\\Custom Skins",getGameDirectory())
    },
    god                 = imgui.new.bool(fconfig.Get('tplayer.god',false)),
    health_regeneration = {
        bool            = imgui.new.bool(fconfig.Get('tplayer.health_regeneration.bool',false)),
        increment_value = imgui.new.int(fconfig.Get('tplayer.health_regeneration.increment_value',2)),
        interval        = imgui.new.int(fconfig.Get('tplayer.health_regeneration.interval',5000)),
    },
    invisible           = imgui.new.bool(fconfig.Get('tplayer.invisible',false)),
    keep_position       = imgui.new.bool(fconfig.Get('tplayer.keep_position',false)),
    model_val           = nil,
    never_wanted        = imgui.new.bool(false),
}

function module.CustomSkinsSetup()
    local hmodloader = getModuleHandle("modloader.asi")
    if hmodloader ~= 0 then
        local dir = fplayer.tplayer.custom_skins.path

        module.tplayer.custom_skins.is_modloader_installed = true

        if not doesDirectoryExist(dir) then
            createDirectory(dir)
        end

        fcommon.IndexFiles(dir,fplayer.tplayer.custom_skins.names,"dff")

    end
end

function module.KeepPosition()
    while module.tplayer.keep_position[0] do
        if isPlayerDead(PLAYER_HANDLE) then
            local x,y,z = getCharCoordinates(PLAYER_PED)
            wait(1000)
            setCharCoordinates(PLAYER_PED,x,y,z)
        end
        wait(0)
    end
end

function module.ChangePlayerModel(model)

    local modeldff = (model .. ".dff")

    if fped.tped.names[model] ~= nil or fplayer.tplayer.custom_skins.names[modeldff] ~= nil then
        if fped.tped.special[model] == nil and fplayer.tplayer.custom_skins.names[modeldff] == nil then
            model = tonumber(model)
            if isModelAvailable(model) then
                requestModel(model)
                loadAllModelsNow()
                setPlayerModel(PLAYER_HANDLE,model)
                markModelAsNoLongerNeeded(model)
            end
        else
            if fped.tped.special[model] ~= nil then
                model = fped.tped.special[model]
            end

            loadSpecialCharacter(model,1)
            loadAllModelsNow()
            setPlayerModel(PLAYER_HANDLE,290)
            unloadSpecialCharacter(290)
        end
        
        local hveh = nil
        if isCharInAnyCar(PLAYER_PED) then
            hveh = getCarCharIsUsing(PLAYER_PED)
            speed = getCarSpeed(hveh)
        end
        clearCharTasksImmediately(PLAYER_PED)
        if hveh ~= nil then
            taskWarpCharIntoCarAsDriver(PLAYER_PED,hveh)
            setCarForwardSpeed(hveh,speed)
        end
        printHelpString("~g~Skin~w~ changed")
    end
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


function module.RegenerateHealth()
    while module.tplayer.health_regeneration.bool[0] do
        local max_health = math.floor(mad.get_char_max_health(PLAYER_PED))
        local health = getCharHealth(PLAYER_PED)

        if max_health > health then
            setCharHealth(PLAYER_PED,health+module.tplayer.health_regeneration.increment_value[0])
        end

        wait(module.tplayer.health_regeneration.interval[0])
    end
end

--------------------------------------------------
-- Cloth functions

function module.ChangePlayerCloth(name)
    local body_part, model, texture = name:match("([^$]+)$([^$]+)$([^$]+)")
    
    setPlayerModel(PLAYER_HANDLE,0)
    
    -- char[] Overflow ?
    if texture == "cutoffchinosblue" then
        givePlayerClothes(PLAYER_HANDLE,-697413025,744365350,body_part)
    else 
        if texture == "sneakerbincblue" then
            givePlayerClothes(PLAYER_HANDLE,-915574819,-2099005073,body_part)
        else
            givePlayerClothesOutsideShop(PLAYER_HANDLE,0,0,body_part)
            givePlayerClothesOutsideShop(PLAYER_HANDLE,texture,model,body_part)
        end
    end
    buildPlayerModel(PLAYER_HANDLE) 

    printHelpString("Clothes changed")
    
    local veh = nil
    local speed = 0
    if isCharInAnyCar(PLAYER_PED) then
        veh = getCarCharIsUsing(PLAYER_PED)
        speed = getCarSpeed(veh)
    end
    clearCharTasksImmediately(PLAYER_PED)
    if veh ~= nil then
        taskWarpCharIntoCarAsDriver(PLAYER_PED,veh)
        setCarForwardSpeed(veh,speed)
    end
end

function module.RemoveThisCloth(name)
    if imgui.MenuItemBool("Remove cloth") then 
        local body_part, model, texture = name:match("([^$]+)$([^$]+)$([^$]+)")
        givePlayerClothes(PLAYER_HANDLE,0,0,body_part)
        buildPlayerModel(PLAYER_HANDLE)
        printHelpString("Cloth ~r~removed")
    end
end
--------------------------------------------------

-- Main function
function module.PlayerMain()
    
    if imgui.Button("Copy coordinates",imgui.ImVec2(fcommon.GetSize(2))) then
        local x,y,z = getCharCoordinates(PLAYER_PED)
        setClipboardText(string.format( "%d,%d,%d",x,y,z))
        printHelpString("Coordinates copied")
    end
    imgui.SameLine()
    if imgui.Button("Suicide",imgui.ImVec2(fcommon.GetSize(2))) then
        setCharHealth(PLAYER_PED,0)
    end
    imgui.Spacing()

    fcommon.Tabs("Player",{"Checkboxes","Menus","Appearance"},{
        function()
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("God mode",module.tplayer.god)
            fcommon.CheckBoxValue("Have bounty on head",0x96913F)
            fcommon.CheckBoxVar("Health regeneration",module.tplayer.health_regeneration.bool,nil,fcommon.SingletonThread(module.RegenerateHealth,"RegenerateHealth"),
            function()
                imgui.SliderInt("Increment value", module.tplayer.health_regeneration.increment_value, 0, 25)
                imgui.SliderInt("Interval", module.tplayer.health_regeneration.interval, 0, 10000)
                fcommon.InformationTooltip("The wait time between increment\nin milliseconds")
            end)
            fcommon.CheckBoxValue("Higher cycle jumps",0x969161)
            fcommon.CheckBoxValue("Infinite oxygen",0x96916E)
            fcommon.CheckBoxValue("Infinite run",0xB7CEE4)
            fcommon.CheckBoxVar("Invisible player",module.tplayer.invisible,"Player can't enter/exit vehicle",
            function()
                if module.tplayer.invisible[0] then
                    module.tplayer.model_val = readMemory((getCharPointer(PLAYER_PED)+1140),4,false)
                    writeMemory(getCharPointer(PLAYER_PED)+1140,4,2,false)
                    fcommon.CheatActivated()
                else
                    writeMemory((getCharPointer(PLAYER_PED)+1140),4,fplayer.tplayer.model_val,false)
                    fcommon.CheatDeactivated()
                end
            end)
            imgui.NextColumn()
            fcommon.CheckBoxVar("Keep position",module.tplayer.keep_position,"Auto teleport to the position you died from",
            function()
                fcommon.SingletonThread(module.KeepPosition,"KeepPosition")
            end)
            fcommon.CheckBoxValue("Lock player control",getCharPointer(PLAYER_PED)+0x598)
            fcommon.CheckBoxValue("Mega jump",0x96916C)
            fcommon.CheckBoxValue("Mega punch",0x969173)
            fcommon.CheckBoxValue("Never get hungry",0x969174)

            module.tplayer.never_wanted[0] = readMemory(0x969171 ,1,false)
            fcommon.CheckBoxVar("Never wanted",module.tplayer.never_wanted,nil,
            function()
                callFunction(0x4396C0,1,0,false)
                if module.tplayer.never_wanted[0] then
                    fcommon.CheatActivated()
                else
                    fcommon.CheatDeactivated()
                end
                fconfig.Set(fconfig.tconfig.misc_data,"Never Wanted",module.tplayer.never_wanted[0])
            end)
           
            imgui.Columns(1)
        end,
        function()
            fcommon.UpdateAddress({name = "Armour",address = getCharPointer(PLAYER_PED)+0x548,size = 4,min = 0,default =0,max = 100, is_float = true})
            fcommon.DropDownMenu("Body",function()
                if imgui.RadioButtonIntPtr("Fat",module.tplayer.cjBody,1) then
                    callFunction(0x439110,1,1,false)
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",1)
                    fcommon.CheatActivated()
                end
                if imgui.RadioButtonIntPtr("Muscle",module.tplayer.cjBody,2) then
                    -- body not changing to muscular after changing to fat fix
                    callFunction(0x439190,1,1,false)
                    callFunction(0x439150,1,1,false)
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",2)
                    fcommon.CheatActivated()
                end
                if imgui.RadioButtonIntPtr("Skinny",module.tplayer.cjBody,3) then
                    callFunction(0x439190,1,1,false)
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",3)
                    fcommon.CheatActivated()
                end
                if imgui.RadioButtonIntPtr("None",module.tplayer.cjBody,0) then
                    fconfig.Set(fconfig.tconfig.misc_data,"Body",0)
                    fcommon.CheatActivated()
                end
            end)
            fcommon.UpdateStat({ name = "Energy",stat = 165})
            fcommon.UpdateStat({ name = "Fat",stat = 21})
            fcommon.UpdateAddress({name = "Health",address = getCharPointer(PLAYER_PED)+0x540,size = 4,min = 0,default =100,max = 255, is_float = true})
            fcommon.UpdateStat({ name = "Lung capacity",stat = 225})
            fcommon.UpdateStat({ name = "Max health",stat = 24,min = 0,default = 569,max = 1450})
            fcommon.UpdateAddress({name = "Money",address = 0xB7CE50,size = 4,min = -9999999,max = 9999999})
            fcommon.UpdateStat({ name = "Muscle",stat = 23})
            fcommon.UpdateStat({ name = "Respect",stat = 68,max = 2450}) 
            fcommon.UpdateStat({ name = "Stamina",stat = 22})
            
            WantedLevelMenu()
        end,
        function()
            fcommon.CheckBoxVar("Aim skin changer", module.tplayer.aimSkinChanger,"Activate using, Aim ped +".. fcommon.GetHotKeyNames(tcheatmenu.hot_keys.asc_key))
   
            fcommon.Tabs("Appearance",{"Clothes","Ped skins","Custom skins"},{
                function()
                    if getCharModel(PLAYER_PED) == 0 then
                        if imgui.Button("Remove clothes",imgui.ImVec2(fcommon.GetSize(1))) then
                            for i=0, 17 do givePlayerClothes(PLAYER_HANDLE,0,0,i) end
                            buildPlayerModel(PLAYER_HANDLE)
                            printHelpString("Clothes ~r~removed")
                        end
                        
                        imgui.Dummy(imgui.ImVec2(0,10))        
                        fcommon.DrawEntries(fconst.IDENTIFIER.CLOTHES,fconst.DRAW_TYPE.IMAGE,module.ChangePlayerCloth,module.RemoveThisCloth,function() return "" end,module.tplayer.clothes.images,fconst.CLOTH.IMAGE_HEIGHT,fconst.CLOTH.IMAGE_WIDTH)    
                    else
                        imgui.TextWrapped("You need to be in CJ skin to change clothes.")
                        imgui.Spacing()
                        if imgui.Button("Change to CJ skin",imgui.ImVec2(fcommon.GetSize(1))) then
                            setPlayerModel(PLAYER_HANDLE,0)

                            local veh = nil
                            local speed = 0
                            if isCharInAnyCar(PLAYER_PED) then
                                veh = getCarCharIsUsing(PLAYER_PED)
                                speed = getCarSpeed(veh)
                            end
                            clearCharTasksImmediately(PLAYER_PED)
                            if veh ~= nil then
                                taskWarpCharIntoCarAsDriver(PLAYER_PED,veh)
                                setCarForwardSpeed(veh,speed)
                            end
                        end
                    end
                end,
                function()
                    fcommon.DrawEntries(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.IMAGE,module.ChangePlayerModel,nil,fped.GetModelName,fped.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH)
                end,
                function()
                    if module.tplayer.custom_skins.is_modloader_installed then
                        module.tplayer.custom_skins.filter:Draw("Search")
                        if module.tplayer.custom_skins.filter:PassFilter('') then
                            local min = imgui.GetItemRectMin()
                            local drawlist = imgui.GetWindowDrawList()
                            drawlist:AddText(imgui.ImVec2(min.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),string.format("Total skins: %d",fcommon.CalcTableSize(fplayer.tplayer.custom_skins.names)))
                        end
                        fcommon.InformationTooltip(string.format("Place your dff & txd files inside,\n'%s'\n\
Note:\nFile names can't exceed 8 characters.\nDon't change names while the game is running",fplayer.tplayer.custom_skins.path))
                        imgui.Spacing()

                        if imgui.BeginChild("Custom skins") then
                            for model_name,_ in pairs(fplayer.tplayer.custom_skins.names) do
                                if module.tplayer.custom_skins.filter:PassFilter(model_name) then
                                    model_name = string.sub(model_name,1,-5)
                                    if #model_name < 9 and imgui.MenuItemBool(model_name) then
                                        fplayer.ChangePlayerModel(model_name)
                                    end
                                end
                            end
                            imgui.EndChild()
                        end
                    else
                        if imgui.Button("Download Modloader",imgui.ImVec2(fcommon.GetSize(1))) then
                            os.execute('explorer "https://gtaforums.com/topic/669520-mod-loader/"')
                        end
                        imgui.Spacing()
                        imgui.TextWrapped("Modloader is not installed. Please install modloader.")
                    end
                end
            })
        end
    })
end

return module
