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

module.tvehicle =
{
    aircraft =
    {
        camera = imgui.new.bool(fconfig.Get('tvehicle.aircraft.camera',false)),
        index = fconfig.Get('tvehicle.aircraft.index',2),
        spawn_in_air = imgui.new.bool(fconfig.Get('tvehicle.aircraft.spawn_in_air',true)),
        zoom = { -5.0,-15.0,-20.0,-30.0,-40.0},
    },
    auto_load_handling = imgui.new.bool(fconfig.Get('tvehicle.auto_load_handling',false)),
    auto_load_paint    = imgui.new.bool(fconfig.Get('tvehicle.auto_load_paint',false)),
    auto_load_tune     = imgui.new.bool(fconfig.Get('tvehicle.auto_load_tune',false)),
    apply_material_filter = imgui.new.bool(fconfig.Get('tvehicle.apply_material_filter',true)),
    color =
    {
        default = -1,
        rgb     = imgui.new.float[3](0.0,0.0,0.0),
    },
    components =
    {
        images = {},
        list   = {},
        names  =
        {
            "Default",
            "wheel_rf_dummy",
            "wheel_lb_dummy",
            "wheel_lf_dummy",
            "wheel_rb_dummy",
            "chassis_dummy",
            "chassis_vlo",
            "exhaust_ok",
            "chassis",
            "ug_nitro",
            "door_lr_dummy",
            "bump_rear_dummy",
            "bonnet_dummy",
            "bump_front_dummy",
            "door_lf_dummy",
            "door_rf_dummy",
            "door_rr_dummy",
            "windscreen_dummy",
            "boot_dummy"
        },
        ui_names =
        {
            "Default",
            "Front right wheel",
            "Back left wheel",
            "Front left wheel",
            "Back right wheel",
            "Chassis ok",
            "Chassis lod",
            "Exhaust",
            "Chassis",
            "Nitro",
            "Back left door",
            "Rear bumper",
            "Boonet",
            "Front bumper",
            "Front left door",
            "Front right door",
            "Back right door",
            "Windscreen",
            "Boot"
        },
        path   =  tcheatmenu.dir .. "vehicles\\components\\",
        saved = false,
        selected = imgui.new.int(0),
        value  = imgui.new.int(0),
    },
    doors = 
    {
        "Hood",
    	"Boot",
    	"Front left door",
    	"Front right door",
    	"Rear left door",
        "Rear right door",
        "All",
    },
    door_menu_button = imgui.new.int(0),
    disable_car_engine = imgui.new.bool(fconfig.Get('tvehicle.disable_car_engine',false)),
    first_person_camera = imgui.new.bool(fconfig.Get('tvehicle.first_person_camera',false)),
    heavy = imgui.new.bool(fconfig.Get('tvehicle.heavy',false)),
    images = {},
    invisible_car = imgui.new.bool(fconfig.Get('tvehicle.invisible_car',false)),
    lights = imgui.new.bool(fconfig.Get('tvehicle.lights',false)),
    lock_doors = imgui.new.bool(false),
    lock_speed = imgui.new.bool(fconfig.Get('tvehicle.lock_speed',false)),
    names = fcommon.LoadJson("vehicle"),
    paintjobs  = 
    {
        cache_images = {},
        current_paintjob = imgui.new.int(-1);
        path   =  tcheatmenu.dir .. "vehicles\\paintjobs",
        search_text = imgui.new.char[20](),
        images = {},
        texture = nil
    },
    no_vehicles = imgui.new.bool(fconfig.Get('tvehicle.no_vehicles',false)),
    no_damage = imgui.new.bool(fconfig.Get('tvehicle.no_damage',false)),
    path = tcheatmenu.dir .. "vehicles\\images",
    quick_spawn  = imgui.new.bool(fconfig.Get('tvehicle.quick_spawn',false)),
    random_colors = imgui.new.bool(fconfig.Get('tvehicle.random_colors',false)),
    random_colors_traffic = imgui.new.bool(fconfig.Get('tvehicle.random_colors_traffic',false)),
    search_text = imgui.new.char[20](),
    spawn_inside = imgui.new.bool(fconfig.Get('tvehicle.spawn_inside',true)),
    speed = imgui.new.int(fconfig.Get('tvehicle.speed',0)),
    stay_on_bike = imgui.new.bool(fconfig.Get('tvehicle.stay_on_bike',false)),
    trains =
    {
        ["449"]     = 9,
        ["400"]     = 9,
        ["537"]     = 10,
        ["569"]     = 10,
        ["538"]     = 11,
        ["570"]     = 11,
    },
    unlimited_nitro = imgui.new.bool(fconfig.Get('tvehicle.unlimited_nitro',false)), 
    visual_damage   = imgui.new.bool(fconfig.Get('tvehicle.visual_damage',false)),
    watertight_car = imgui.new.bool(fconfig.Get('tvehicle.watertight_car',false)),
}

module.tvehicle.components.list  = imgui.new['const char*'][#module.tvehicle.components.ui_names](module.tvehicle.components.ui_names)

module.IsValidModForVehicle = ffi.cast('bool(*)(int model, int cvehicle)',0x49B010)

IsThisModelATrain = ffi.cast('bool(*)(int model)',0x4C5AD0)

-- Used in quick spawner (fcommon)
function module.CBaseModelInfo(name)
    local pInfo = allocateMemory(16)
    callFunction(0x004C5940,2,2,name,pInfo)
    local model = readMemory(pInfo,4,false)
    freeMemory(pInfo)

    if getNameOfVehicleModel(model) == name then
        return model
    else
        return 0
    end
end

-- Returns name of vehicle
function module.GetModelName(id)
    return getNameOfVehicleModel(tonumber(id)) or module.tvehicle.names[id] or ""
end

-- Spawns a vehicle for player
function module.GiveVehicleToPlayer(model)
    model = tonumber(model)
    if isModelAvailable(model) then
        x,y,z = getCharCoordinates(PLAYER_PED)
        if isCharInAnyCar(PLAYER_PED) and module.tvehicle.spawn_inside[0] then
            vehicle = getCarCharIsUsing(PLAYER_PED)
            speed = getCarSpeed(vehicle)
            warpCharFromCarToCoord(PLAYER_PED,x,y,z)
            deleteCar(vehicle)
        end

        if module.tvehicle.aircraft.spawn_in_air[0] and (isThisModelAHeli(model) or isThisModelAPlane(model)) then
            z = 400
        end

        if IsThisModelATrain(model) == true then

            id = module.tvehicle.trains[tostring(model)]

            for key,value in pairs(module.tvehicle.trains) do
                if value == id and key ~= tostring(model) then
                    model2 = tonumber(key)
                    break
                end
            end

            deleteAllTrains()
            requestModel(model)
            requestModel(model2)
            loadAllModelsNow()
            if math.random(0,1) == 0 then
                vehicle = createMissionTrain(id,x,y,z)
            else
                vehicle = createMissionTrain(id,x,y,z,true)
            end
            removeCharElegantly(getDriverOfCar(vehicle))
            if module.tvehicle.spawn_inside[0] then
                warpCharIntoCar(PLAYER_PED,vehicle)
            end
            markMissionTrainAsNoLongerNeeded(vehicle)
            markModelAsNoLongerNeeded(model2)
        else
            requestModel(model)
            loadAllModelsNow()

            if not module.tvehicle.spawn_inside[0] then
                vehicle = spawnVehicleByCheating(model)
            else
                vehicle = createCar(model,x,y,z)
                setCarHeading(vehicle,getCharHeading(PLAYER_PED))
                warpCharIntoCar(PLAYER_PED,vehicle)
                setCarForwardSpeed(vehicle,speed)
            end

            markCarAsNoLongerNeeded(vehicle)
        end
        markModelAsNoLongerNeeded(model)
        fcommon.CheatActivated()
    end
end

function DoorMenu(func)
    local vehicle = getCarCharIsUsing(PLAYER_PED)
    local seats   = getMaximumNumberOfPassengers(vehicle) + 1 -- passenger + driver 
    
    if seats == 4 then
        doors = 5
    else
        doors = 3
    end
    if imgui.Button(module.tvehicle.doors[7],imgui.ImVec2(fcommon.GetSize(1))) then
        for i=0,doors,1 do
            func(vehicle,i)
        end
    end
    for i=0,doors,1 do
        if imgui.Button(module.tvehicle.doors[i+1],imgui.ImVec2(fcommon.GetSize(2))) then
            func(vehicle,i)
        end
        if i%2 ~= 1 then
            imgui.SameLine()
        end
    end

end   


function module.RandomColors()
    while true do
        if isCharInAnyCar(PLAYER_PED) and module.tvehicle.random_colors[0] and not module.tvehicle.random_colors_traffic[0] then
            local primary_color = math.random(fconst.VEHICLE_COLOR.MIN_VALUE,fconst.VEHICLE_COLOR.MAX_VALUE)
            local secondary_color = math.random(fconst.VEHICLE_COLOR.MIN_VALUE,fconst.VEHICLE_COLOR.MAX_VALUE)
            local car = getCarCharIsUsing(PLAYER_PED)
            if isCharInAnyCar(PLAYER_PED) and module.tvehicle.random_colors[0] then
                changeCarColour(car,primary_color,secondary_color)
            else
                break
            end
            wait(1000)
        end
        wait(0)
    end
end

function module.RandomTrafficColors()
    while true do
        if module.tvehicle.random_colors_traffic[0] then
            local carX,carY,carZ = getCharCoordinates(PLAYER_PED)

            local result, car = findAllRandomVehiclesInSphere(carX,carY,carZ,100.0,false,true)

            if result then
                while result do
                    local primary_color = math.random(fconst.VEHICLE_COLOR.MIN_VALUE,fconst.VEHICLE_COLOR.MAX_VALUE)
                    local secondary_color = math.random(fconst.VEHICLE_COLOR.MIN_VALUE,fconst.VEHICLE_COLOR.MAX_VALUE)
                    changeCarColour(car,primary_color,secondary_color)
                    result, car = findAllRandomVehiclesInSphere(carX,carY,carZ,100.0,true,true)
                end
            end
            wait(1000)
        end
        wait(0)
    end
end

function LoadCarHandlingData(model)

    local address = callFunction(0x00403DA0,1,1,model)
    local phandling = readMemory((address + 0x4A),2,false)
    phandling = phandling * 0xE0
    phandling = phandling + 0xC2B9DC

    local CDM = readMemory((phandling + 0xC8),4,false)
    local fMass_in = readMemory((phandling + 0x8),4,false)
    CDM = CDM/2000.0
    CDM = CDM/fMass_in

    fcommon.RwMemory(phandling + 0x9C,1,fconfig.Get(string.format("tvehicle.handling.%d.ABS",model)))
    fcommon.RwMemory(phandling + 0x98,4,fconfig.Get(string.format("tvehicle.handling.%d.Brake bias",model)),nil,true)
    fcommon.RwMemory(phandling + 0x94,4,fconfig.Get(string.format("tvehicle.handling.%d.Brake deceleration",model)),nil,true)
    fcommon.RwMemory(phandling + 0x14,4,fconfig.Get(string.format("tvehicle.handling.%d.Centre of mass X",model)),nil,true)
    fcommon.RwMemory(phandling + 0x18,4,fconfig.Get(string.format("tvehicle.handling.%d.Centre of mass Y",model)),nil,true)
    fcommon.RwMemory(phandling + 0x1C,4,fconfig.Get(string.format("tvehicle.handling.%d.Centre of mass Z",model)),nil,true)
    fcommon.RwMemory(phandling + 0xC8,4,fconfig.Get(string.format("tvehicle.handling.%d.Collision damage multiplier",model)),nil,true)
    fcommon.RwMemory(phandling + CDM,4,fconfig.Get(string.format("tvehicle.handling.%d.Damage multiplier",model)),nil,true)
    fcommon.RwMemory(phandling + 0xB0,4,fconfig.Get(string.format("tvehicle.handling.%d.Damping level",model)),nil,true)
    fcommon.RwMemory(phandling + 0x10,4,fconfig.Get(string.format("tvehicle.handling.%d.Drag mult",model)),nil,true)
    fcommon.RwMemory(phandling + 0x74,1,fconfig.Get(string.format("tvehicle.handling.%d.Drive type",model)))
    fcommon.RwMemory(phandling + 0x7C,4,fconfig.Get(string.format("tvehicle.handling.%d.Engine acceleration",model)),nil,true)
    fcommon.RwMemory(phandling + 0x80,4,fconfig.Get(string.format("tvehicle.handling.%d.Engine inertia",model)),nil,true)
    fcommon.RwMemory(phandling + 0x75,4,fconfig.Get(string.format("tvehicle.handling.%d.Engine type",model)),nil,true)
    fcommon.RwMemory(phandling + 0xDC,4,fconfig.Get(string.format("tvehicle.handling.%d.Front lights",model)),nil,true)
    fcommon.RwMemory(phandling + 0xAC,4,fconfig.Get(string.format("tvehicle.handling.%d.Force level",model)),nil,true)
    fcommon.RwMemory(phandling + 0xB4,4,fconfig.Get(string.format("tvehicle.handling.%d.High speed damping",model)),nil,true)
    fcommon.RwMemory(phandling + 0xBC,4,fconfig.Get(string.format("tvehicle.handling.%d.Lower limit",model)),nil,true)
    fcommon.RwMemory(phandling + 0x4,4,fconfig.Get(string.format("tvehicle.handling.%d.Mass",model)),nil,true)
    fcommon.RwMemory(phandling + 0x84,4,fconfig.Get(string.format("tvehicle.handling.%d.Max velocity",model)),nil,true)
    fcommon.RwMemory(phandling + 0xC8,4,fconfig.Get(string.format("tvehicle.handling.%d.Monetary value",model)))
    fcommon.RwMemory(phandling + 0x76,1,fconfig.Get(string.format("tvehicle.handling.%d.Number of gears",model)))
    fcommon.RwMemory(phandling + 0x20,1,fconfig.Get(string.format("tvehicle.handling.%d.Percent submerged",model)))
    fcommon.RwMemory(phandling + 0xDD,1,fconfig.Get(string.format("tvehicle.handling.%d.Rear lights",model)))
    fcommon.RwMemory(phandling + 0xD4,4,fconfig.Get(string.format("tvehicle.handling.%d.Seat offset distance",model)),nil,true)
    fcommon.RwMemory(phandling + 0xA0,4,fconfig.Get(string.format("tvehicle.handling.%d.Steering lock",model)),nil,true)
    fcommon.RwMemory(phandling + 0xC0,4,fconfig.Get(string.format("tvehicle.handling.%d.Suspension bias",model)),nil,true)
    fcommon.RwMemory(phandling + 0xA8,4,fconfig.Get(string.format("tvehicle.handling.%d.Traction bias",model)),nil,true)
    fcommon.RwMemory(phandling + 0xA4,4,fconfig.Get(string.format("tvehicle.handling.%d.Traction loss",model)),nil,true)
    fcommon.RwMemory(phandling + 0xC ,4,fconfig.Get(string.format("tvehicle.handling.%d.Turn mass",model)),nil,true)
    fcommon.RwMemory(phandling + 0x28,4,fconfig.Get(string.format("tvehicle.handling.%d.Traction multiplier",model)),nil,true)
    fcommon.RwMemory(phandling + 0xB8,4,fconfig.Get(string.format("tvehicle.handling.%d.Upper limit",model)),nil,true)
    fcommon.RwMemory(phandling + 0xDE,1,fconfig.Get(string.format("tvehicle.handling.%d.Vehicle anim group",model)),nil,true)
    
end

function module.UnlimitedNitro()
    while true do
        if isCharInAnyCar(PLAYER_PED) and module.tvehicle.unlimited_nitro[0] then
            local car = getCarCharIsUsing(PLAYER_PED)
            local pCar = getCarPointer(car)
            if isThisModelACar(getCarModel(car)) then
                module.AddComponentToVehicle(1010,car,true)
                
                while module.tvehicle.unlimited_nitro[0] do

                    writeMemory(0x969165,1,0,false) -- ALl cars have nitro
                    writeMemory(0x96918B,1,0,false) -- All taxis have nitro

                    if getCarCharIsUsing(PLAYER_PED) ~= car then
                        break
                    end

                    local nitro_state = fcommon.RwMemory(pCar+0x8A4,4,nil,false,true)
                    
                    if nitro_state == fconst.NITRO_STATE.FULL or nitro_state == fconst.NITRO_STATE.EMPTY or nitro_state == fconst.NITRO_STATE.DISCHANGRED then
                        giveNonPlayerCarNitro(car)
                    end

                    wait(0)
                end
            end
        end
        wait(0)
    end
end

--------------------------------------------------
-- Camera

function module.AircraftCamera()

    while true do
        if (isCharInAnyHeli(PLAYER_PED)
        or isCharInAnyPlane(PLAYER_PED)) 
        and module.tvehicle.aircraft.camera[0] then
            while isCharInAnyHeli(PLAYER_PED)
            or isCharInAnyPlane(PLAYER_PED) do
                
                -- FirstPersonCamera controls the camera if its enabled
                if module.tvehicle.aircraft.camera[0] == false or module.tvehicle.first_person_camera[0] then break end 

                local vehicle = getCarCharIsUsing(PLAYER_PED)
                local roll = getCarRoll(vehicle)

                attachCameraToVehicle(vehicle,0.0,module.tvehicle.aircraft.zoom[module.tvehicle.aircraft.index],2.5,0.0,0.0,0.0,(roll*-1),2)
                if isKeyDown(tkeys.camera_zoom) then
                    while isKeyDown(tkeys.camera_zoom) do
                        wait(0)
                    end
                    module.tvehicle.aircraft.index = module.tvehicle.aircraft.index + 1
                    if module.tvehicle.aircraft.index > #module.tvehicle.aircraft.zoom then
                        module.tvehicle.aircraft.index  = 0
                    end
                end
                wait(0)
            end
            restoreCameraJumpcut()
        end
        wait(0)
    end
end

function module.FirstPersonCamera()
    while true do
        local total_x = 0
        local total_y = 0
        if module.tvehicle.first_person_camera[0] and not isCharOnFoot(PLAYER_PED) then
            while module.tvehicle.first_person_camera[0] do

                if isCharOnFoot(PLAYER_PED) then
                    break 
                end
                
                x,y = getPcMouseMovement()
                total_x = total_x + x
                total_y = total_y + y

                local roll = 0.0
                if module.tvehicle.aircraft.camera[0] == true then -- check if new aircraft camera is enabled
                    local vehicle = getCarCharIsUsing(PLAYER_PED)
                    roll = getCarRoll(vehicle)
                end
                attachCameraToChar(PLAYER_PED,0.0, 0.1, 0.6, total_x, 180, total_y, (roll*-1), 2)
                wait(0)
            end
            restoreCameraJumpcut()  
        end
        wait(0)
    end
end

--------------------------------------------------

--------------------------------------------------
-- Vehicle Paintjobs

function module.ForEachCarComponent(func,skip)
    if isCharInAnyCar(PLAYER_PED) then
        car = getCarCharIsUsing(PLAYER_PED)
        for _, comp in ipairs(mad.get_all_vehicle_components(car)) do
            for _, obj in ipairs(comp:get_objects()) do
                for _, mat in ipairs(obj:get_materials()) do
                    func(mat,comp,car)
                    if skip == true then
                        goto _skip
                    end
                end
            end
            ::_skip::
        end
        markCarAsNoLongerNeeded(car)
    else
        printHelpString("Player ~r~not ~w~in car")
    end
end

function module.GetTextureName(name)
    if name == nil then
        return ""
    else
        return name
    end
end

function module.OnEnterVehicle()
    while true do
        if isCharInAnyCar(PLAYER_PED) then
            local car = getCarCharIsUsing(PLAYER_PED)
            local model = getCarModel(car)

            -- Auto load handling data
            if module.tvehicle.auto_load_handling[0] then
                LoadCarHandlingData(model)
            end

            -- Auto load paint data
            if module.tvehicle.auto_load_paint[0] then
                ApplyTexture(nil,true)
                ApplyColor(true)
                giveVehiclePaintjob(car,fconfig.Get(string.format( "tvehicle.paint.%d.Paintjob",model),-1))
            end

            -- Auto load tuning data
            if module.tvehicle.auto_load_tune[0] then
                if fconfig.tconfig.tune[tostring(model)] ~= nil then
                    for _,component in ipairs(fconfig.tconfig.tune[tostring(model)]) do
                        module.AddComponentToVehicle(component,car,true)
                    end
                end
            end
            
            if module.tvehicle.invisible_car[0] then
                setCarVisible(car,false)
            end
            if module.tvehicle.watertight_car[0] then
                setCarWatertight(car,false)
            end
           
            

            while isCharInCar(PLAYER_PED,car) do
                wait(0)
            end
        else

            fconfig.tconfig.temp_texture_name = nil
            fconfig.tconfig.temp_color_rgb = nil

        end
        wait(0)
    end
end

function ApplyColor(load_saved_color)

    if fconfig.tconfig.temp_color_rgb == nil then fconfig.tconfig.temp_color_rgb = {} end
    module.ForEachCarComponent(function(mat,comp,car)

        local r, g, b, old_a = mat:get_color()
        local model = getCarModel(car)

        -- -1.0 used as nil
        if load_saved_color then
            module.tvehicle.color.rgb[0] = fconfig.Get(string.format("tvehicle.paint.%s.components.%s.Red",model,comp.name),-1.0)
            module.tvehicle.color.rgb[1] = fconfig.Get(string.format("tvehicle.paint.%s.components.%s.Green",model,comp.name),-1.0)
            module.tvehicle.color.rgb[2] = fconfig.Get(string.format("tvehicle.paint.%s.components.%s.Blue",model,comp.name),-1.0)
        end

        if (module.tvehicle.color.rgb[0] ~= -1.0 and module.tvehicle.color.rgb[0] ~= -1.0 and module.tvehicle.color.rgb[0] ~= -1.0)
        and (not module.tvehicle.apply_material_filter[0] or (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF)) then
            
            if module.tvehicle.components.selected[0] == 0 then
                
                mat:set_color(module.tvehicle.color.rgb[0]*255, module.tvehicle.color.rgb[1]*255, module.tvehicle.color.rgb[2]*255, 255.0)
                
                if module.tvehicle.paintjobs.texture ~= nil then
                    mat:set_texture(module.tvehicle.paintjobs.texture)
                end

                if fconfig.tconfig.temp_color_rgb[comp.name] == nil then fconfig.tconfig.temp_color_rgb[comp.name] = {} end
                fconfig.tconfig.temp_color_rgb[comp.name][1] = module.tvehicle.color.rgb[0]
                fconfig.tconfig.temp_color_rgb[comp.name][2] = module.tvehicle.color.rgb[1]
                fconfig.tconfig.temp_color_rgb[comp.name][3] = module.tvehicle.color.rgb[2]
            end

            if comp.name == module.tvehicle.components.names[module.tvehicle.components.selected[0]+1] or load_saved_color then
                
                mat:set_color(module.tvehicle.color.rgb[0]*255, module.tvehicle.color.rgb[1]*255, module.tvehicle.color.rgb[2]*255, 255.0)
                
                if module.tvehicle.paintjobs.texture ~= nil then
                    mat:set_texture(module.tvehicle.paintjobs.texture)                
                end

                if fconfig.tconfig.temp_color_rgb[comp.name] == nil then fconfig.tconfig.temp_color_rgb[comp.name] = {} end
                fconfig.tconfig.temp_color_rgb[comp.name][1] = module.tvehicle.color.rgb[0]
                fconfig.tconfig.temp_color_rgb[comp.name][2] = module.tvehicle.color.rgb[1]
                fconfig.tconfig.temp_color_rgb[comp.name][3] = module.tvehicle.color.rgb[2]
            end

        end
        module.tvehicle.color.default = getCarColours(car)
    end)  
end

function ApplyTexture(texture_name,load_saved_texture)

    if fconfig.tconfig.temp_texture_name == nil then fconfig.tconfig.temp_texture_name = {} end
    module.ForEachCarComponent(function(mat,comp,car)
        local r, g, b, old_a = mat:get_color()

        local model = getCarModel(car)

        if load_saved_texture then
            filename = fconfig.Get(string.format( "tvehicle.paint.%d.components.%s.Texture",model,comp.name),"")
        else
            filename = texture_name
        end

        local fullpath = module.tvehicle.paintjobs.path .. "\\images\\" .. filename .. ".png"

        if doesFileExist(fullpath) then
            
            if module.tvehicle.paintjobs.cache_images[filename] == nil then
                module.tvehicle.paintjobs.cache_images[filename] = mad.load_png_texture(fullpath)
            end

            module.tvehicle.paintjobs.texture = module.tvehicle.paintjobs.cache_images[filename]


            if not module.tvehicle.apply_material_filter[0] or (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF) then
                if module.tvehicle.components.selected[0] == 0 and not load_saved_texture then
                    mat:set_texture(module.tvehicle.paintjobs.texture)
                    fconfig.tconfig.temp_texture_name[comp.name] = filename
                end
                if comp.name == module.tvehicle.components.names[module.tvehicle.components.selected[0]+1] or load_saved_texture then
                    mat:set_texture(module.tvehicle.paintjobs.texture)
                    fconfig.tconfig.temp_texture_name[comp.name] = filename
                end
            end
        end
        module.tvehicle.color.default = getCarColours(car)
    end)
    
end

--------------------------------------------------


--------------------------------------------------
-- Add car component - tune

function module.AddComponentToVehicle(component,car,hide_msg)
    component = tonumber(component)
    if isCharInAnyCar(PLAYER_PED) then
        if car == nil then
            car = getCarCharIsUsing(PLAYER_PED)
        end
        if isModelAvailable(component) and doesVehicleExist(car) then
            requestVehicleMod(component)
            loadAllModelsNow()
            addVehicleMod(car,component)
            if hide_msg ~= true then
                printHelpString("Component ~g~added")
            end
            markModelAsNoLongerNeeded(component)
        end
    end
end

function module.RemoveComponentFromVehicle(component,car,hide_msg)
    component = tonumber(component)
    if car == nil then
        car = getCarCharIsUsing(PLAYER_PED)
    end
    if doesVehicleExist(car) then
        removeVehicleMod(car,component)
        if hide_msg ~= true then
            printHelpString("Component ~r~removed")
        end
    end
end

--------------------------------------------------


-- Main function
function module.VehicleMain()
    imgui.Spacing()

    if imgui.Button("Repair vehicle",imgui.ImVec2(fcommon.GetSize(2))) then
        if isCharInAnyCar(PLAYER_PED) then
            local car = getCarCharIsUsing(PLAYER_PED)
            fixCar(car)
            fcommon.CheatActivated()
        end
    end
    imgui.SameLine()
    if imgui.Button("Flip vehicle",imgui.ImVec2(fcommon.GetSize(2))) then
        
        if isCharInAnyCar(PLAYER_PED) then
            local car   = getCarCharIsUsing(PLAYER_PED)

            setCarRoll(car,getCarRoll(car) + 180)
            setCarRoll(car,getCarRoll(car)) -- rotation fix
            fcommon.CheatActivated()
        end
    end
    imgui.Spacing()
    if imgui.BeginTabBar("Vehicles") then
        
        if imgui.BeginTabItem("Checkbox") then
            if imgui.BeginChild("Checkbox") then
                imgui.Spacing()

                imgui.Columns(2,nil,false)
                
                fcommon.CheckBoxValue("Aggressive drivers",0x96914F)
                fcommon.CheckBoxValue("All cars have nitro",0x969165)
                fcommon.CheckBoxValue("All taxis have nitro",0x96918B)
                fcommon.CheckBoxValue("Boats fly",0x969153)
                fcommon.CheckBoxValue("Cars fly",0x969160)
                fcommon.CheckBoxVar("Car heavy",module.tvehicle.heavy)
                fcommon.CheckBoxValue("Decreased traffic",0x96917A)
                fcommon.CheckBoxVar("Don't fall off bike",module.tvehicle.stay_on_bike)
                fcommon.CheckBoxValue("Drive on water",0x969152)
                fcommon.CheckBoxVar("Disable car engine",module.tvehicle.disable_car_engine)
                fcommon.CheckBoxVar("First person camera",module.tvehicle.first_person_camera)
                fcommon.CheckBoxValue("Float away when hit",0x969166)
                fcommon.CheckBoxValue("Green traffic lights",0x96914E)
                fcommon.CheckBoxVar("Invisible car",module.tvehicle.invisible_car,nil,
                function()
                    if isCharInAnyCar(PLAYER_PED) then
                        local car = getCarCharIsUsing(PLAYER_PED)
                        setCarVisible(car,not module.tvehicle.invisible_car[0])
                    end
                end)

                imgui.NextColumn()

                fcommon.CheckBoxFunc("Lights on",module.tvehicle.lights,
                function()
                    if isCharInAnyCar(PLAYER_PED) then
                        car = getCarCharIsUsing(PLAYER_PED)
                        if module.tvehicle.lights[0] then
                            forceCarLights(car,2)
                            addOneOffSound(x,y,z,1052)
                            fcommon.CheatActivated()
                        else
                            forceCarLights(car,1)
                            addOneOffSound(x,y,z,1053)
                            fcommon.CheatDeactivated()
                        end
                    else
                        printHelpString("Player ~r~not~w~ in car")
                    end
                end)
                fcommon.CheckBoxFunc("Lock doors",module.tvehicle.lock_doors,
                function()
                    if isCharInAnyCar(PLAYER_PED) then
                        if getCarDoorLockStatus(car) == 4 then
                            lockCarDoors(car,1)
                            fcommon.CheatDeactivated()
                        else
                            lockCarDoors(car,4)
                            fcommon.CheatActivated()
                        end
                    else
                        printHelpString("Player ~r~not~w~ in car")
                    end
                end)
    
                fcommon.CheckBoxVar("New aircraft camera",module.tvehicle.aircraft.camera)
                fcommon.CheckBoxVar("No damage",module.tvehicle.no_damage)
                fcommon.CheckBoxFunc("No traffic vehicles",module.tvehicle.no_vehicles,
                function()
                    if module.tvehicle.no_vehicles[0] then
                        writeMemory(0x434237,1,0x73,false) -- change condition to unsigned (0-255)
                        writeMemory(0x434224,1,0,false)
                        writeMemory(0x484D19,1,0x83,false) -- change condition to unsigned (0-255)
                        writeMemory(0x484D17,1,0,false)
                        fcommon.CheatActivated()
                    else
                        writeMemory(0x434237,1,-1063242627,false) -- change condition to unsigned (0-255)
                        writeMemory(0x434224,1,940431405,false)
                        writeMemory(0x484D19,1,292493,false) -- change condition to unsigned (0-255)
                        writeMemory(0x484D17,1,1988955949,false)
                        fcommon.CheatDeactivated()
                    end
                end)
                fcommon.CheckBoxVar("No visual damage",module.tvehicle.visual_damage)
                fcommon.CheckBoxValue("Perfect handling",0x96914C)
                fcommon.CheckBoxVar("Random colors",module.tvehicle.random_colors,"Paints players car with random colors every second")
                fcommon.CheckBoxVar("Random traffic colors",module.tvehicle.random_colors_traffic,"Paints traffic cars with random colors every second")
                fcommon.CheckBoxValue("Tank mode",0x969164) 
                fcommon.CheckBoxValue("Train camera fix",5416239,nil,fconst.TRAIN_CAM_FIX.ON,fconst.TRAIN_CAM_FIX.OFF) 
                fcommon.CheckBoxVar("Unlimited nitro",module.tvehicle.unlimited_nitro,"Enabling this would disable\n\nAll cars have nitro\nAll taxis have nitro")
                fcommon.CheckBoxVar("Watertight car",module.tvehicle.watertight_car,nil,
                function()
                    if isCharInAnyCar(PLAYER_PED) then
                        local car = getCarCharIsUsing(PLAYER_PED)
                        setCarWatertight(car,module.tvehicle.watertight_car[0])
                    end
                end)
                fcommon.CheckBoxValue("Wheels only",0x96914B)
        
                imgui.Columns(1)
                
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Menus") then
            if imgui.BeginChild("Menus") then

                fcommon.DropDownMenu("Enter nearest vehicle as",function()
                    local vehicle,ped = storeClosestEntities(PLAYER_PED)
                    if vehicle ~= -1 then
                        local seats = getMaximumNumberOfPassengers(vehicle)
                        imgui.Spacing()
                        imgui.Columns(2,nil,false)
                        imgui.Text("Vehicle: " .. module.GetModelName(getCarModel(vehicle)))
                        imgui.NextColumn()
                        imgui.Text(string.format("Total seats: %d",seats+1))
                        imgui.Columns(1)
                        imgui.Spacing()
                        imgui.Separator()
                        imgui.Spacing()
                        if imgui.Button("Driver",imgui.ImVec2(fcommon.GetSize(2))) then
                            warpCharIntoCar(PLAYER_PED,vehicle)
                        end
                        for i=0,seats-1,1 do
                            if i%2 ~= 1 then
                                imgui.SameLine()
                            end
                            if imgui.Button("Passenger " .. tostring(i+1),imgui.ImVec2(fcommon.GetSize(2))) then
                                warpCharIntoCarAsPassenger(PLAYER_PED,vehicle,i)
                            end
                        end
                    else
                        imgui.Text("No near by vehicles")
                    end
                end)

                fcommon.DropDownMenu("Traffic options",function()
                    imgui.Columns(2,nil,false)
                    fcommon.RadioButton("Color",{"Black","Pink"},{0x969151,0x969150})
                    
                    imgui.NextColumn()
                    fcommon.RadioButton("Type",{"Cheap","Country","Fast"},{0x96915E,0x96917B,0x96915F})
                    imgui.Columns(1)
                end)

                if isCharInAnyCar(PLAYER_PED) then
                    local car = getCarCharIsUsing(PLAYER_PED)
                    fcommon.UpdateAddress({name = 'Vehicle dirt level',address = getCarPointer(car) + 1200 ,size = 4,min = 0,max = 15, default = 7.5,is_float = true})
                    fcommon.UpdateAddress({name = 'Vehicle nitro count',address = getCarPointer(car) + 0x48A ,size = 1,min = 0,max = 15, default = 7.5,is_float = false})

                    fcommon.DropDownMenu("Vehicle doors",function()
                        if isCharInAnyCar(PLAYER_PED) and not (isCharOnAnyBike(PLAYER_PED) or isCharInAnyBoat(PLAYER_PED) 
                        or isCharInAnyHeli(PLAYER_PED) or isCharInAnyPlane(PLAYER_PED)) then    
                            if imgui.RadioButtonIntPtr("Damage", module.tvehicle.door_menu_button,0) then end
                            imgui.SameLine()
                            if imgui.RadioButtonIntPtr("Fix", module.tvehicle.door_menu_button,1) then end
                            imgui.SameLine()
                            if imgui.RadioButtonIntPtr("Open", module.tvehicle.door_menu_button,2) then end
                            imgui.SameLine()
                            if imgui.RadioButtonIntPtr("Pop", module.tvehicle.door_menu_button,3) then end
                            imgui.Spacing()
                            imgui.Separator()
                            imgui.Spacing()
    
                            if module.tvehicle.door_menu_button[0] == 0 then
                                if module.tvehicle.visual_damage[0] == false then
                                    DoorMenu(function(vehicle,door)
                                        damageCarDoor(vehicle,door)
                                    end)
                                else
                                    imgui.Text("No visual damage enabled")
                                end
                            end
                            if module.tvehicle.door_menu_button[0] == 1 then
                                DoorMenu(function(vehicle,door)
                                    fixCarDoor(vehicle,door)
                                end)
                            end
                            if module.tvehicle.door_menu_button[0] == 2 then
                                DoorMenu(function(vehicle,door)
                                    openCarDoor(vehicle,door)
                                end)
                            end
                            if module.tvehicle.door_menu_button[0] == 3 then
                                DoorMenu(function(vehicle,door)
                                    popCarDoor(vehicle,door,true)
                                end)
                            end
                        else
                            imgui.Text("Player not in car")
                        end
                        
                    end)
                    fcommon.DropDownMenu("Vehicle speed",function()
                        
                        imgui.Columns(2,nil,false)
                        fcommon.CheckBoxVar("Lock speed",module.tvehicle.lock_speed)
                        imgui.NextColumn()
                        imgui.Columns(1)
                        if imgui.InputInt("Set",module.tvehicle.speed) then
                        end
                        if imgui.Button("Set speed",imgui.ImVec2(fcommon.GetSize(2))) then
                            if isCharInAnyCar(PLAYER_PED) then
                                car = getCarCharIsUsing(PLAYER_PED)
                                setCarForwardSpeed(car,module.tvehicle.speed[0])
                            end
                        end
                        imgui.SameLine()
                        if imgui.Button("Instant stop",imgui.ImVec2(fcommon.GetSize(2))) then
                            if isCharInAnyCar(PLAYER_PED) then
                                car = getCarCharIsUsing(PLAYER_PED)
                                setCarForwardSpeed(car,0.0)
                            end
                        end
                        if module.tvehicle.speed[0] > 500 then
                            module.tvehicle.speed[0] = 500
                        end
                        if module.tvehicle.speed[0] < 0 then
                            module.tvehicle.speed[0] = 0
                        end
                    end)
                end
                
                fcommon.UpdateAddress({name = 'Vehicle density multiplier',address = 0x8A5B20,size = 4,min = 0,max = 10, default = 1,is_float = true})

                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Spawn") then

            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("Quick vehicle",module.tvehicle.quick_spawn,string.format("Vehicle can be spawned from quick spawner using %s\n\nControls:\nEnter : Stop reading key press\nDelete : Erase full string\nBackspace : Erase last character",fcommon.GetHotKeyNames(tcheatmenu.hot_keys.quick_spawner)))
            fcommon.CheckBoxVar("Spawn inside",module.tvehicle.spawn_inside,"Spawn inside vehicle as driver")

            imgui.NextColumn()
            fcommon.CheckBoxVar("Spawn aircraft in air",module.tvehicle.aircraft.spawn_in_air)
            imgui.Columns(1)

            imgui.Spacing()
            if imgui.BeginTabBar("Vehicles list") then
                imgui.Spacing()
                if imgui.BeginTabItem("List") then
                    fcommon.DrawImages(fconst.IDENTIFIER.VEHICLE,fconst.DRAW_TYPE.LIST,module.tvehicle.images,fconst.VEHICLE.IMAGE_HEIGHT,fconst.VEHICLE.IMAGE_WIDTH,module.GiveVehicleToPlayer,nil,module.GetModelName)
                    imgui.EndTabItem()
                end 
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",module.tvehicle.search_text,ffi.sizeof(module.tvehicle.search_text)) then end
                    imgui.SameLine()
        
                    imgui.Spacing()
                    imgui.Text("Vehicles found :(" .. ffi.string(module.tvehicle.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()

                    fcommon.DrawImages(fconst.IDENTIFIER.VEHICLE,fconst.DRAW_TYPE.SEARCH,module.tvehicle.images,fconst.VEHICLE.IMAGE_HEIGHT,fconst.VEHICLE.IMAGE_WIDTH,module.GiveVehicleToPlayer,nil,module.GetModelName,ffi.string(module.tvehicle.search_text))
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        if isCharInAnyCar(PLAYER_PED) then
            local car = getCarCharIsUsing(PLAYER_PED)
            local model = getCarModel(car)
            if imgui.BeginTabItem("Paint") then
                imgui.Spacing()
                if imgui.Button("Load customization",imgui.ImVec2(fcommon.GetSize(2))) then
                    ApplyTexture(nil,true)
                    ApplyColor(true)
                    giveVehiclePaintjob(car,fconfig.Get(string.format( "tvehicle.paint.%d.Paintjob",model),-1))
                    printHelpString("Customizaiton loaded")
                end
                imgui.SameLine()
                if imgui.Button("Save customizaion",imgui.ImVec2(fcommon.GetSize(2))) then
                    
                    if fconfig.tconfig.temp_color_rgb ~= nil then
                        for comp_name,table in pairs(fconfig.tconfig.temp_color_rgb) do
                            fconfig.Set(fconfig.tconfig.paint,string.format("%d.components.%s.Red",model,comp_name),table[1])
                            fconfig.Set(fconfig.tconfig.paint,string.format("%d.components.%s.Green",model,comp_name),table[2])
                            fconfig.Set(fconfig.tconfig.paint,string.format("%d.components.%s.Blue",model,comp_name),table[3])
                        end
                    else
                        for model_name,model_table in pairs(fconfig.tconfig.paint) do
                            if model_name == tostring(model) then
                                for comp_name,comp_table in pairs(model_table["components"]) do
                                    fconfig.Set(fconfig.tconfig.paint,string.format("%s.components.%s.Red",model_name,comp_name),nil)
                                    fconfig.Set(fconfig.tconfig.paint,string.format("%s.components.%s.Green",model_name,comp_name),nil)
                                    fconfig.Set(fconfig.tconfig.paint,string.format("%s.components.%s.Blue",model_name,comp_name),nil)
                                end
                                break
                            end
                        end 
                    end

                    if fconfig.tconfig.temp_texture_name ~= nil then
                        for comp_name,file_name in pairs(fconfig.tconfig.temp_texture_name) do
                            fconfig.Set(fconfig.tconfig.paint,string.format("%d.components.%s.Texture",model,comp_name),file_name)
                        end
                    else
                        for model_name,model_table in pairs(fconfig.tconfig.paint) do
                            if model_name == tostring(model) then
                                for comp_name,comp_table in pairs(model_table["components"]) do
                                    fconfig.Set(fconfig.tconfig.paint,string.format("%s.components.%s.Texture",model_name,comp_name),nil)
                                end
                            end
                        end 
                    end

                    local current_paintjob = nil

                    if module.tvehicle.paintjobs.current_paintjob[0] ~= -1 then -- None
                        current_paintjob = module.tvehicle.paintjobs.current_paintjob[0]
                    end

                    fconfig.Set(fconfig.tconfig.paint,string.format("%d.Paintjob",model),current_paintjob)

                    fconfig.Write()
                    printHelpString("Customizaiton saved")
                end
                if imgui.Button("Reset color",imgui.ImVec2(fcommon.GetSize(2))) then
                    fconfig.tconfig.temp_color_rgb = nil

                    module.ForEachCarComponent(function(mat,comp,car)
                        mat:reset_color()
                    end)
                    module.tvehicle.color.default = -1
                    printHelpString("Color reset")
                end
                imgui.SameLine()
                if imgui.Button("Reset texture",imgui.ImVec2(fcommon.GetSize(2))) then
                    fconfig.tconfig.temp_texture_name = nil
                    module.ForEachCarComponent(function(mat,comp,car)
                        mat:reset_texture()
                    end)
                    module.tvehicle.paintjobs.texture = nil
                    printHelpString("Texture reset")
                end

                imgui.Spacing()
                imgui.Columns(2,nil,false)
                fcommon.CheckBoxVar("Auto load paint data",module.tvehicle.auto_load_paint)
                imgui.NextColumn()
                fcommon.CheckBoxVar("Apply material filter",module.tvehicle.apply_material_filter)
                imgui.Columns(1)
                imgui.Spacing()
                
                if imgui.ColorEdit3("Color",module.tvehicle.color.rgb) then
                    ApplyColor()
                end
                imgui.Combo("Component",module.tvehicle.components.selected,module.tvehicle.components.list,#module.tvehicle.components.names)
                
                local paintjobs_count =  getNumAvailablePaintjobs(car)
                
                module.tvehicle.paintjobs.current_paintjob[0] = getCurrentVehiclePaintjob(car)

                if paintjobs_count > 0 then
                    if imgui.InputInt("Paintjob",module.tvehicle.paintjobs.current_paintjob) then
                        if module.tvehicle.paintjobs.current_paintjob[0] >= -1 and module.tvehicle.paintjobs.current_paintjob[0] <= paintjobs_count then
                            giveVehiclePaintjob(car,module.tvehicle.paintjobs.current_paintjob[0])
                        end
                    end
                end

                imgui.Spacing()
                if imgui.InputText("Search",module.tvehicle.paintjobs.search_text,ffi.sizeof(module.tvehicle.paintjobs.search_text)) then end

                imgui.Spacing()

                imgui.Text("Custom paintjobs found :(" .. ffi.string(module.tvehicle.paintjobs.search_text) .. ")")
                imgui.Separator()
                imgui.Spacing()
    
                fcommon.DrawImages(fconst.IDENTIFIER.PAINTJOB,fconst.DRAW_TYPE.SEARCH,module.tvehicle.paintjobs.images,fconst.PAINTJOB.IMAGE_HEIGHT,fconst.PAINTJOB.IMAGE_WIDTH,ApplyTexture,nil,module.GetTextureName,ffi.string(module.tvehicle.paintjobs.search_text))
                        
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem('Tune') then
                imgui.Spacing()
                if imgui.Button("Load components",imgui.ImVec2(fcommon.GetSize(2))) then
                    if fconfig.tconfig.tune[tostring(model)] ~= nil then
                        for _,component in ipairs(fconfig.tconfig.tune[tostring(model)]) do
                            module.AddComponentToVehicle(component,car,true)
                        end
                    end
                    printHelpString("Components loaded")
                end
                imgui.SameLine()
                if imgui.Button("Save components",imgui.ImVec2(fcommon.GetSize(2))) then

                    fconfig.tconfig.tune[tostring(model)] = {} 

                    for x=1,fconst.COMPONENT.TOTAL_SLOTS,1 do
                        local comp_model = getCurrentCarMod(car,x)

                        if comp_model ~= -1 then
                            table.insert(fconfig.tconfig.tune[tostring(model)],comp_model)
                        end
                    end

                    fconfig.Write()
                    printHelpString("Components saved")
                end
                if imgui.Button("Reset car components",imgui.ImVec2(fcommon.GetSize(2))) then
                    for x=1,fconst.COMPONENT.TOTAL_SLOTS,1 do
                        local comp_model = getCurrentCarMod(car,x)

                        if comp_model ~= -1 then
                            module.RemoveComponentFromVehicle(comp_model,car,true)
                        end
                    end
                    printHelpString("Car components reset")
                end
                imgui.SameLine()
                if imgui.Button("Reset config data",imgui.ImVec2(fcommon.GetSize(2))) then
                    fconfig.tconfig.tune[tostring(model)] = {}
                    fconfig.Write()
                    printHelpString("Tune config data reset")
                end
                imgui.Spacing()
                fcommon.CheckBoxVar("Auto load tune data",module.tvehicle.auto_load_tune)
                fcommon.InformationTooltip("Left click to add component\nRight click to remove component")
                imgui.Spacing()
                if imgui.BeginChild("Tune") then
                    imgui.Spacing()
                    fcommon.DrawImages(fconst.IDENTIFIER.COMPONENT,fconst.DRAW_TYPE.LIST,module.tvehicle.components.images,fconst.COMPONENT.IMAGE_HEIGHT,fconst.COMPONENT.IMAGE_WIDTH,module.AddComponentToVehicle,module.RemoveComponentFromVehicle)
                    imgui.EndChild()
                end
                imgui.EndTabItem()
            end
            if imgui.BeginTabItem("Handling") then

                -------------------------------------------------------
                local address = callFunction(0x00403DA0,1,1,model)
                local phandling = readMemory((address + 0x4A),2,false)
                phandling = phandling * 0xE0
                phandling = phandling + 0xC2B9DC

                local CDM = readMemory((phandling + 0xC8),4,false)
                local fMass_in = readMemory((phandling + 0x8),4,false)
                CDM = CDM/2000.0
                CDM = CDM/fMass_in
                -------------------------------------------------------

                imgui.Spacing()
                if imgui.Button("Load handling",imgui.ImVec2(fcommon.GetSize(2))) then

                    LoadCarHandlingData(model)
                    printHelpString("Handling loaded")

                end
                imgui.SameLine()
                if imgui.Button("Save handling",imgui.ImVec2(fcommon.GetSize(2))) then
                    
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.ABS",model),fcommon.RwMemory(phandling + 0x9C,1))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Brake bias",model),fcommon.RwMemory(phandling + 0x98,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Brake deceleration",model),fcommon.RwMemory(phandling + 0x94,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Centre of mass X",model),fcommon.RwMemory(phandling + 0x14,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Centre of mass Y",model),fcommon.RwMemory(phandling + 0x18,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Centre of mass Z",model),fcommon.RwMemory(phandling + 0x1C,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Collision damage multiplier",model),fcommon.RwMemory(phandling + 0xC8,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Damage multiplier",model),fcommon.RwMemory(phandling + CDM,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Damping level",model),fcommon.RwMemory(phandling + 0xB0,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Drag mult",model),fcommon.RwMemory(phandling + 0x10,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Drive type",model),fcommon.RwMemory(phandling + 0x74,1))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Engine acceleration",model),fcommon.RwMemory(phandling + 0x7C,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Engine inertia",model),fcommon.RwMemory(phandling + 0x80,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Engine type",model),fcommon.RwMemory(phandling + 0x75,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Front lights",model),fcommon.RwMemory(phandling + 0xDC,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Force level",model),fcommon.RwMemory(phandling + 0xAC,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.High speed damping",model),fcommon.RwMemory(phandling + 0xB4,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Lower limit",model),fcommon.RwMemory(phandling + 0xBC,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Mass",model),fcommon.RwMemory(phandling + 0x4,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Max velocity",model),fcommon.RwMemory(phandling + 0x84,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Monetary value",model),fcommon.RwMemory(phandling + 0xC8,4))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Number of gears",model),fcommon.RwMemory(phandling + 0x76,1))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Percent submerged",model),fcommon.RwMemory(phandling + 0x20,1))                    
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Rear lights",model),fcommon.RwMemory(phandling + 0xDD,1))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Seat offset distance",model),fcommon.RwMemory(phandling + 0xD4,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Steering lock",model),fcommon.RwMemory(phandling + 0xA0,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Suspension bias",model),fcommon.RwMemory(phandling + 0xC0,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Traction bias",model),fcommon.RwMemory(phandling + 0xA8,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Traction loss",model),fcommon.RwMemory(phandling + 0xA4,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Turn mass",model),fcommon.RwMemory(phandling + 0xC,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Traction multiplier",model),fcommon.RwMemory(phandling + 0x28,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Upper limit",model),fcommon.RwMemory(phandling + 0xB8,4,nil,nil,true))
                    fconfig.Set(fconfig.tconfig.handling,string.format("%d.Vehicle anim group",model),fcommon.RwMemory(phandling + 0xDE,1))
                    fconfig.Write()
                    printHelpString("Handling saved")
                end
                if imgui.Button("Reset game handling",imgui.ImVec2(fcommon.GetSize(2))) then
                    local cHandlingDataMgr = readMemory(0x05BFA96,4,false)
                    callMethod(0x5BD830,cHandlingDataMgr,0,0)
                    printHelpString("Handling reset")
                end
                
                imgui.SameLine()
                if imgui.Button("Reset config data",imgui.ImVec2(fcommon.GetSize(2))) then
                    local cHandlingDataMgr = readMemory(0x05BFA96,4,false)
                    callMethod(0x5BD830,cHandlingDataMgr,0,0)
                    fconfig.tconfig.handling = {}
                    fconfig.Set(fconfig.tconfig.read.tvehicle,"handling",nil)
                    printHelpString("Handling config data reset")
                end
                
                imgui.Spacing()
                fcommon.CheckBoxVar("Auto load handling data",module.tvehicle.auto_load_handling)
                imgui.Spacing()
                if imgui.BeginChild("Handling") then

                    fcommon.RadioButtonFunc("ABS",{"On","Off"},{1.0,0.0},phandling + 0x9C)
                    fcommon.UpdateAddress({name = 'Brake bias',address = phandling + 0x98 ,size = 4,min = 0,max = 1,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Brake deceleration',address = phandling + 0x94 ,size = 4,min = 0.1,max = 10,is_float = true,factor = 3.9999999e-4,cvalue = 0.1})
                    fcommon.UpdateAddress({name = 'Centre of mass X',address = phandling + 0x14 ,size = 1,min = -10.0,max = 10.0,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Centre of mass Y',address = phandling + 0x18 ,size = 1,min = -10.0,max = 10.0,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Centre of mass Z',address = phandling + 0x1C ,size = 1,min = -10.0,max = 10.0,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Collision damage multiplier',address = phandling + 0xC8 ,size = 4,min = 0.2,max = 5.0,is_float = true,cvalue = 0.05})                    
                    fcommon.UpdateAddress({name = 'Damage multiplier',address = phandling + CDM ,size = 4,min = 1,max = 10000,is_float = true})
                    fcommon.UpdateAddress({name = 'Damping level',address = phandling + 0xB0 ,size = 4,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Drag mult',address = phandling + 0x10 ,size = 4,min = 1,max = 20.0,is_float = true})
                    fcommon.RadioButtonFunc("Drive type",{"FWD","RWD","4WD"},{fconst.DRIVE_TYPE.FWD,fconst.DRIVE_TYPE.RWD,fconst.DRIVE_TYPE.AWD},phandling + 0x74)
                    fcommon.UpdateAddress({name = 'Engine acceleration',address = phandling + 0x7C ,size = 4,min = 1,max = 10000,is_float = true,factor = 3.9999999e-4})
                    fcommon.UpdateAddress({name = 'Engine inertia',address = phandling + 0x80 ,size = 4,min = 0,max = 50,is_float = true})
                    fcommon.RadioButtonFunc("Engine type",{"Petrol","Diseal","Electric"},{fconst.ENGINE_TYPE.PETROL,fconst.ENGINE_TYPE.DISEAL,fconst.ENGINE_TYPE.ELECTRIC},phandling + 0x75)
                    fcommon.RadioButtonFunc("Front lights",{"Long","Small","Big","Tall"},{fconst.LIGHTS.LONG,fconst.LIGHTS.SMALL,fconst.LIGHTS.BIG,fconst.LIGHTS.TALL},phandling + 0xDC)
                    fcommon.UpdateAddress({name = 'Force level',address = phandling + 0xAC ,size = 4,is_float = true,cvalue = 0.1})
                    fcommon.UpdateAddress({name = 'High speed damping',address = phandling + 0xB4 ,size = 4,is_float = true,cvalue = 0.1})
                    fcommon.UpdateAddress({name = 'Lower limit',address = phandling + 0xBC ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Mass',address = phandling + 0x4 ,size = 4,min = 1,max = 50000,is_float = true})
                    fcommon.UpdateAddress({name = 'Max velocity',address = phandling + 0x84 ,size = 4,min = 1,max = 10000,is_float = true,factor = 5.5555599e-3})
                    fcommon.UpdateAddress({name = 'Monetary value',address = phandling + 0xD8 ,size = 4,min = 1,max = 100000})
                    fcommon.UpdateAddress({name = 'Number of gears',address = phandling + 0x76 ,size = 1,min = 1,max = 10})
                    fcommon.UpdateAddress({name = 'Percent submerged',address = phandling + 0x20 ,size = 1,min = 10,max = 120})
                    fcommon.RadioButtonFunc("Rear lights",{"Long","Small","Big","Tall"},{fconst.LIGHTS.LONG,fconst.LIGHTS.SMALL,fconst.LIGHTS.BIG,fconst.LIGHTS.TALL},phandling + 0xDD)
                    fcommon.UpdateAddress({name = 'Seat offset distance',address = phandling + 0xD4 ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Steering lock',address = phandling + 0xA0 ,size = 4,min = 10,max = 40,is_float = true})
                    fcommon.UpdateAddress({name = 'Suspension bias',address = phandling + 0xC0 ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Traction bias',address = phandling + 0xA8 ,size = 4,min = 0,max = 1,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Traction loss',address = phandling + 0xA4 ,size = 4,min = 0,max = 1,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Turn mass',address = phandling + 0xC ,size = 4,min = -50000,max = 50000,is_float = true})
                    fcommon.UpdateAddress({name = 'Traction multiplier',address = phandling + 0x28 ,size = 4,min = 0.2,max = 2,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Upper limit',address = phandling + 0xB8 ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Vehicle anim group',address = phandling + 0xDE ,size = 1})
                    imgui.EndChild()
                end
            end
        end
        imgui.EndTabBar()
    end
end

return module
