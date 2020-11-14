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

module.tped =
{
    gang =
    {
        is_exgangwars_installed = false,
        index = imgui.new.int(0),
        list = 
        {
            "Ballas",
            "Grove street families",
            "Los santos vagos",
            "San fierro rifa",
            "Da nang boys",
            "Mafia",
            "Mountain cloud triad",
            "Varrio los aztecas",
            "Gang9",
            "Gang10",
        },
        names = 
        {   
            ["Ballas"] = 0,
            ["Da nang boys"] = 4,
            ["Gang9"] = 8,
            ["Gang10"] = 9,
            ["Grove street families"] = 1,
            ["Los santos vagos"] = 2,
            ["Mafia"] = 5,
            ["Mountain cloud triad"] = 6,
            ["San fierro rifa"] = 3,
            ["Varrio los aztecas"] = 7,
        },  
        wars = imgui.new.bool(fconfig.Get('tped.gang.wars',false)),
    },
    images      = {},
    names       = fcommon.LoadJson("ped"),
    path        = tcheatmenu.dir .. "peds\\",
    ped_health_display = imgui.new.bool(fconfig.Get('tped.ped_health_display',false)),
    selected    = nil,
    spawned_peds= 
    {
        list = {},
        ped_bleed = imgui.new.bool(fconfig.Get('tped.spawned_peds.ped_bleed',false)),
        ped_accuracy= imgui.new.int(fconfig.Get('tped.spawned_peds.ped_accuracy',50)),
        ped_health = imgui.new.int(fconfig.Get('tped.spawned_peds.ped_health',100)),
        ped_type_list = {"Civ male","Civ female","Cop","Ballas","Grove Street Families","Los Santos Vagos",
                        "San Fierro Rifa","Da Nang Boys","Mafia","Mountain Cloud Triads","Varrio Los Aztecas",
                        "Gang 9","Medic","Dealer","Criminal","Fireman","Prostitute"},
        ped_type_selected = imgui.new.int(fconfig.Get('tped.spawned_peds.ped_type_selected',1)),
        ped_weapon_ammo = imgui.new.int(fconfig.Get('tped.spawned_peds.ped_weapon_ammo',99999)),
        ped_weapon_selected = fconfig.Get('tped.spawned_peds.ped_weapon_selected',"None"),
        ped_weapon_id = imgui.new.int(fconfig.Get('tped.spawned_peds.ped_weapon_id',-1)),
        stand_still = imgui.new.bool(fconfig.Get('tped.spawned_peds.stand_still',true)),
    },
    special     = fcommon.LoadJson("ped special"),
}

module.tped.gang.array = imgui.new['const char*'][#module.tped.gang.list](module.tped.gang.list)

if getModuleHandle("ExGangWars.asi") ~= 0 then
    module.tped.gang.is_exgangwars_installed = true
end

-- Returns ped name
function module.GetModelName(model)
    if module.tped.names[model] then 
        return module.tped.names[model] 
    else 
        return "" 
    end
end

function module.SpawnPed(model)  
 
    if  module.tped.names[model] ~= nil then
        local ped = nil
        if module.tped.special[model] == nil then
            model = tonumber(model)
            requestModel(model)
            loadAllModelsNow()

            local x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.spawned_peds.ped_type_selected[0]+3,model,x,y,z)

            markModelAsNoLongerNeeded(model)
            module.tped.spawned_peds.list[ped] = tostring(getCharModel(ped))

        else
            if hasSpecialCharacterLoaded(model) then
                unloadSpecialCharacter(model)
            end
            loadSpecialCharacter(module.tped.special[tostring(model)],1)
            loadAllModelsNow()

            local x,y,z = getCharCoordinates(PLAYER_PED)
            ped = createChar(module.tped.spawned_peds.ped_type_selected[0]+3,290,x,y,z) -- CIVMALE

            module.tped.spawned_peds.list[ped] = tostring(getCharModel(ped))
            markModelAsNoLongerNeeded(module.tped.special[tostring(model)])
        end
        if ped ~= nil then
            if not module.tped.spawned_peds.stand_still[0] then
                markCharAsNoLongerNeeded(ped)
            end
            setCharHealth(ped,module.tped.spawned_peds.ped_health[0])
            setCharBleeding(ped,module.tped.spawned_peds.ped_bleed[0])
            setCharAccuracy(ped,module.tped.spawned_peds.ped_accuracy[0])

            if module.tped.spawned_peds.ped_weapon_id ~= -1 then
                local model = getWeapontypeModel(module.tped.spawned_peds.ped_weapon_id)

                if isModelAvailable(model) then
                    requestModel(model)
                    loadAllModelsNow()
                    giveWeaponToChar(ped,module.tped.spawned_peds.ped_weapon_id,module.tped.spawned_peds.ped_weapon_ammo[0])
                end
            end
            printHelpString("Ped ~g~Spawned")
        end
    end
end

function module.PedHealthDisplay()
    while module.tped.ped_health_display[0] do
        local result, char = getCharPlayerIsTargeting(PLAYER_HANDLE)

        if result then

            local health = getCharHealth(char)
            local x,y,z = getCharCoordinates(char)
            local screenX,screenY = convert3DCoordsToScreen(x,y,z+1.0)
            mad.draw_text(tostring(health),screenX,screenY,1,0.8,0.4,0,false,false,false,255,255,255,255,false)

        end
        wait(0)
    end

end

function module.RemoveAllSpawnedPeds()
    for ped,model in pairs(module.tped.spawned_peds.list) do
        removeCharElegantly(ped)
        module.tped.spawned_peds.list[ped] = nil
    end
end

function GetLargestGangInZone()
    local gang = 0
    local density = 0

    for title,id in pairs(module.tped.gang.names) do

        local x,y,z = getCharCoordinates(PLAYER_PED)

        local gang_desity = getZoneGangStrength(getNameOfInfoZone(x,y,z),id)
       
        if gang_desity > density then
            density = gang_desity
            gang = id
        end
    end
    return gang
end


function SetSelectedWeapon(weapon)
    module.tped.spawned_peds.ped_weapon_id = weapon
    module.tped.spawned_peds.ped_weapon_selected = fweapon.GetModelName(weapon)
end

function module.PedMain()
     
    if fcommon.BeginTabBar('PedBar') then
        if fcommon.BeginTabItem('Checkboxes') then
            imgui.Columns(2,nil,false)
            if fcommon.CheckBoxVar("Display target health",module.tped.ped_health_display) then
                fcommon.CreateThread(module.PedHealthDisplay)
            end
            fcommon.CheckBoxValue("Elvis everywhere",0x969157)
            fcommon.CheckBoxValue("Everyone is armed",0x969140)
            fcommon.CheckBoxValue("Gangs control streets",0x96915B)
            fcommon.CheckBoxValue("Gangs everywhere",0x96915A)
            if fcommon.CheckBoxVar("Gang wars",module.tped.gang.wars,nil,
            function()
                if imgui.Button("Start gang war",imgui.ImVec2(fcommon.GetSize(2))) then
                    if GetLargestGangInZone() == 1 then
                        callFunction(0x444300,0,0) -- defensive gang war
                    else
                        callFunction(0x446050,0,0) -- offensive gang war
                    end
                    setGangWarsActive(true)
                end   
                imgui.SameLine()
                if imgui.Button("End gang war",imgui.ImVec2(fcommon.GetSize(2))) then
                    callFunction(0x4464C0,0,0)
                    setGangWarsActive(true)
                end      
                
                imgui.Dummy(imgui.ImVec2(0,20))
                imgui.TextWrapped("Gang zone density:")
                imgui.Spacing()
                for title,id in pairs(module.tped.gang.names) do

                    local x,y,z = getCharCoordinates(PLAYER_PED)
    
                    local density = imgui.new.int(getZoneGangStrength(getNameOfInfoZone(x,y,z),id))
                    imgui.PushItemWidth(imgui.GetWindowWidth()/2)
                    if imgui.SliderInt(title,density,0,255) then
                        setZoneGangStrength(getNameOfInfoZone(x,y,z),id,density[0])
                        clearSpecificZonesToTriggerGangWar()
                    end
                end
                imgui.PopItemWidth()
                imgui.Dummy(imgui.ImVec2(0,20))
                if not module.tped.gang.is_exgangwars_installed then
                    imgui.TextWrapped("You'll need ExGangWars plugin to display some turf colors")
                    imgui.Spacing()
                    if imgui.Button("Download ExGangWars",imgui.ImVec2(fcommon.GetSize(1))) then
                        os.execute('explorer "https://gtaforums.com/topic/682194-extended-gang-wars/"')
                    end
                end
            end) then
                setGangWarsActive(module.tped.gang.wars[0])
            end
            
            imgui.NextColumn()

            fcommon.CheckBoxValue("No road peds",0x8D2538,nil,0,25)
            fcommon.CheckBoxValue("Peds mayhem",0x96913E)
            fcommon.CheckBoxValue("Peds attack with rockets",0x969158)
            fcommon.CheckBoxValue("Peds riot",0x969175)
            fcommon.CheckBoxValue("Slut magnet",0x96915D)
            
            imgui.Columns(1)
            imgui.Spacing()
        end
        if fcommon.BeginTabItem('Menus') then
            fcommon.UpdateAddress({name = 'Pedestrian density multiplier',address = 0x8D2530,size = 4,min = 0,max = 10, default = 1,is_float = true})
            fcommon.DropDownMenu("Recruit anyone",function()
                fcommon.RadioButtonAddress("Select Weapon",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E})
            end)
        end
        if fcommon.BeginTabItem('Spawn') then
            if imgui.Button("Remove all spawned peds",imgui.ImVec2(fcommon.GetSize(1))) then
                module.RemoveAllSpawnedPeds()  
                printHelpString("All peds removed")        
            end
            imgui.Spacing()
            
            if fcommon.BeginTabBar("SpawnPedBar") then
                if fcommon.BeginTabItem('Spawner') then
                    fcommon.DrawEntries(fconst.IDENTIFIER.PED,fconst.DRAW_TYPE.IMAGE,module.SpawnPed,nil,module.GetModelName,module.tped.images,fconst.PED.IMAGE_HEIGHT,fconst.PED.IMAGE_WIDTH)
                end
                if fcommon.BeginTabItem('Config') then
                    imgui.Columns(2,nil,false)
                    fcommon.CheckBoxVar("Don't move",module.tped.spawned_peds.stand_still)   
                    imgui.NextColumn()
                    fcommon.CheckBoxVar("Ped bleed",module.tped.spawned_peds.ped_bleed)   
                    imgui.Columns(1)                 
                    imgui.Spacing()
                    imgui.Text("Selected weapon: " .. module.tped.spawned_peds.ped_weapon_selected)
                    fcommon.ConfigPanel("Select weapon",function()
                        if imgui.Button("Clear weapon selection",imgui.ImVec2(fcommon.GetSize(1))) then
                            module.tped.spawned_peds.ped_weapon_id = -1
                            module.tped.spawned_peds.ped_weapon_selected = "None"
                        end
                        imgui.Spacing()
                        imgui.InputInt("Weapon ammo", module.tped.spawned_peds.ped_weapon_ammo) 
                        imgui.Spacing()
                        imgui.Text("Selected weapon: " .. module.tped.spawned_peds.ped_weapon_selected)
                        fcommon.DrawEntries(fconst.IDENTIFIER.WEAPON,fconst.DRAW_TYPE.IMAGE,SetSelectedWeapon,nil,fweapon.GetModelName,fweapon.tweapon.images,fconst.WEAPON.IMAGE_HEIGHT,fconst.WEAPON.IMAGE_WIDTH,function(a) return a ~= "Jetpack"end )
                    end)
                    imgui.Spacing()
                    imgui.SliderInt("Accuracy", module.tped.spawned_peds.ped_accuracy, 0.0, 100.0)
                    if imgui.InputInt("Health", module.tped.spawned_peds.ped_health)  then
                        module.tped.spawned_peds.ped_health[0] = module.tped.spawned_peds.ped_health[0] < 0 and 0 or module.tped.spawned_peds.ped_health[0]
                        module.tped.spawned_peds.ped_health[0] = module.tped.spawned_peds.ped_health[0] > 1000 and 1000 or module.tped.spawned_peds.ped_health[0]
                    end
                    fcommon.DropDownListNumber("Ped type",module.tped.spawned_peds.ped_type_list,module.tped.spawned_peds.ped_type_selected)
                end
                fcommon.EndTabBar()
            end
        end
        fcommon.EndTabBar()
    end
end
return module
