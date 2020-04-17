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

module.tvehicle =
{
    aircraft =
    {
        camera = imgui.new.bool(fconfig.Get('tvehicle.aircraft.camera',false)),
        index = fconfig.Get('tvehicle.aircraft.index',2),
        spawn_in_air = imgui.new.bool(fconfig.Get('tvehicle.aircraft.spawn_in_air',true)),
        zoom = { -5.0,-15.0,-20.0,-30.0,-40.0},
    },
    apply_material_filter = imgui.new.bool(fconfig.Get('tvehicle.apply_material_filter',true)),
    color =
    {
        car_data_table = {},
        col_data_table = {},
        default = -1,
        parse_done = false,
        radio_btn = imgui.new.int(1),
        rgb      = imgui.new.float[3](0.0,0.0,0.0),
        show_all = imgui.new.bool(fconfig.Get('tvehicle.color.show_all',false))
    },
    components   =
    {
        filter   = imgui.ImGuiTextFilter(),
        images   = {},
        list     = {},
        names    = {},
        path     =  tcheatmenu.dir .. "vehicles\\components\\",
        saved    = false,
        selected = imgui.new.int(0),
        value    = imgui.new.int(0),
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
    filter          = imgui.ImGuiTextFilter(),
    gsx             =
    {
        handle      = getModuleHandle("gsx.asi"),
        p_table     = {},
        veh_data    = {},
    },
    gxt_name        = imgui.new.char[32](""),
    gxt_name_table  = fconfig.Get('tvehicle.gxt_name_table',{}),
    heavy = imgui.new.bool(fconfig.Get('tvehicle.heavy',false)),
    hidden_objects = {},
    images = {},
    invisible_car = imgui.new.bool(fconfig.Get('tvehicle.invisible_car',false)),
    license_plate_text = imgui.new.char[9](fconfig.Get('tvehicle.license_plate_text',"GTA__SAN")),
    lights = imgui.new.bool(fconfig.Get('tvehicle.lights',false)),
    lock_doors = imgui.new.bool(false),
    lock_speed = imgui.new.bool(fconfig.Get('tvehicle.lock_speed',false)),
    names = fcommon.LoadJson("vehicle"),
    paintjobs  = 
    {
        cache_images     = {},
        current_paintjob = imgui.new.int(-1);
        filter           = imgui.ImGuiTextFilter(),
        path             =  tcheatmenu.dir .. "vehicles\\paintjobs",
        images           = {},
        texture          = nil
    },
    neon   = 
    {
        checkbox = imgui.new.bool(fconfig.Get('tvehicle.neon.checkbox',false)),
        data     = fcommon.LoadJson("neon data"),
        pulsing  = imgui.new.bool(false),
        rb_value = imgui.new.int(-1),
    },
    no_vehicles = imgui.new.bool(fconfig.Get('tvehicle.no_vehicles',false)),
    no_damage = imgui.new.bool(fconfig.Get('tvehicle.no_damage',false)),
    path = tcheatmenu.dir .. "vehicles\\images",
    random_colors = imgui.new.bool(fconfig.Get('tvehicle.random_colors',false)),
    random_colors_traffic = imgui.new.bool(fconfig.Get('tvehicle.random_colors_traffic',false)),
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
    watertight_car  = imgui.new.bool(fconfig.Get('tvehicle.watertight_car',false)),
}

module.IsValidModForVehicle = ffi.cast('bool(*)(int model, int cvehicle)',0x49B010)
IsThisModelATrain = ffi.cast('bool(*)(int model)',0x4C5AD0)
CVehicleModelInfo = ffi.cast("uintptr_t*", 0x00A9B0C8)

--------------------------------------------------
-- Neon

result, handle = loadDynamicLibrary("neon_api.asi")
module.tvehicle.neon["Handle"] = handle

if module.tvehicle.neon["Handle"] == 0 then
    tcheatmenu.window.missing_components = true
else
    result, proc = getDynamicLibraryProcedure("SetFlag",module.tvehicle.neon["Handle"])
    module.tvehicle.neon["SetFlag"] = proc

    result, proc = getDynamicLibraryProcedure("GetFlag",module.tvehicle.neon["Handle"])
    module.tvehicle.neon["GetFlag"] = proc

    result, proc = getDynamicLibraryProcedure("SetX",module.tvehicle.neon["Handle"])
    module.tvehicle.neon["SetX"] = proc

    result, proc = getDynamicLibraryProcedure("SetY",module.tvehicle.neon["Handle"])
    module.tvehicle.neon["SetY"] = proc

    result, proc = getDynamicLibraryProcedure("InstallNeon",module.tvehicle.neon["Handle"])
    module.tvehicle.neon["InstallNeon"] = proc
end

function InstallNeon(pCar,color,pulsing)
    local car = getVehiclePointerHandle(pCar)
    color = color or module.tvehicle.neon.rb_value[0]
    pulsing = pulsing or module.tvehicle.neon.pulsing[0]
    if module.tvehicle.neon["Handle"] ~= 0 then
        if module.tvehicle.neon["InstallNeon"] and module.tvehicle.neon["SetX"] and module.tvehicle.neon["SetY"] then
            callFunction(module.tvehicle.neon["InstallNeon"],3,3,pCar,color,pulsing)
            
            local data = module.tvehicle.neon.data[module.GetNameOfVehicleModel(getCarModel(car))] or { X = 0.0, Y = 0.0}
  
            callFunction(module.tvehicle.neon["SetX"],2,2,pCar,data.X)
            callFunction(module.tvehicle.neon["SetY"],2,2,pCar,data.Y)
            if module.tvehicle.gsx.handle ~= 0  then
                module.GSXSet(car,"cm_neon_color",color)
                module.GSXSet(car,"cm_neon_pulsing",pulsing)
            end
        end
    end
end

function module.TrafficNeons()
    
    while true do
        if module.tvehicle.neon.checkbox[0] and module.tvehicle.neon["Handle"] ~= 0 then
            local x,y,z = getCharCoordinates(PLAYER_PED)

            local result, car = findAllRandomVehiclesInSphere(x,y,z,100.0,false,true)
            
            if result then
                while result do
                    local temp = 0
                    local pCar = getCarPointer(car)

                    if getVehicleClass(car) == fconst.VEHICLE_CLASS.NORMAL then
                        temp = math.random(1,20) -- 5%
                    end
                    if getVehicleClass(car) == fconst.VEHICLE_CLASS.RICH_FAMILY then
                        temp = math.random(1,5) -- 20%
                    end
                    if getVehicleClass(car) == fconst.VEHICLE_CLASS.EXECUTIVE then
                        temp = math.random(1,3) -- 30%
                    end
                    if temp == 1 and callFunction(module.tvehicle.neon["GetFlag"],1,1,pCar) ~= 0x10 then
                        if getCarCharIsUsing(PLAYER_PED) ~= car then
                            InstallNeon(pCar,math.random(0,6),math.random(0,1))
                        end
                    end
                    callFunction(module.tvehicle.neon["SetFlag"],2,2,pCar,0x10)
                    result, car = findAllRandomVehiclesInSphere(x,y,z,100.0,true,true)
                end
            end
        end
        wait(0)
    end
end

--------------------------------------------------
-- Garage Save Extender (GSX) https://gtaforums.com/topic/925563-garage-save-extender/
-- gsx-data https://forum.mixmods.com.br/f16-utilidades/t2954-gsx-data-usar-gsx-em-scripts-lua-facilmente

if module.tvehicle.gsx.handle == 0 then
    tcheatmenu.window.missing_components = true
else
    ffi.cdef[[
    typedef uint32_t GSXCVehicle;
    typedef struct { float x, y, z; } CVector;
    typedef struct __attribute__((packed, aligned(1))) {
        CVector pos;
        uint32_t handling_flags;
        uint8_t flags;
        uint8_t field_11;
        uint16_t model;
        uint16_t carmods[15];
        uint8_t colour[4];
        uint8_t radio_station;
        uint8_t extra1;
        uint8_t extra2;
        uint8_t bomb_type;
        uint8_t paintjob;
        uint8_t nitro_count;
        uint8_t angleX;
        uint8_t angleY;
        uint8_t angleZ;
        uint8_t field_3F;
    } CStoredCar;
    typedef struct __attribute__((packed, aligned(1))) { GSXCVehicle veh; int32_t status; size_t when; } journalNews;
    typedef struct __attribute__((packed, aligned(1))) { GSXCVehicle veh; int32_t status; } apiCarNotify;
    typedef struct __attribute__((packed, aligned(1))) { GSXCVehicle veh; int32_t status; CStoredCar *gameStoredData; } externalCallbackStructure;
    typedef void(__cdecl externalCbFun_t)(const externalCallbackStructure*);
    int __cdecl addNotifyCallback(externalCbFun_t fun);
    int __cdecl addNotifyTempCallback(externalCbFun_t fun);
    void __cdecl removeNotifyCallback(int cbRef);
    void __cdecl removeNotifyTempCallback(int cbRef);
    int __cdecl getNewCarGrgForeach(size_t *i, apiCarNotify *out);
    void __cdecl setDataToSaveLaterVehPtr(GSXCVehicle veh, const char *name, int size, void *ptr, bool forceCopyNow);
    void __cdecl pushDirectlyToSavedData(GSXCVehicle veh, const char *name, int size, void *ptr);
    int __cdecl dataToSaveLaterExists(GSXCVehicle veh, const char *name);
    void __cdecl removeToLoadDataVehPtr(GSXCVehicle veh, const char *name);
    void __cdecl removeToSaveLaterVehPtr(GSXCVehicle veh, const char *name);
    int __cdecl dataToLoadExists(GSXCVehicle veh, const char *name);
    void*  __cdecl getLoadDataByVehPtr(GSXCVehicle veh, const char *name);
    const char*  __cdecl GSX_getCompileTime();
    const char*  __cdecl GSX_getVersionString();
    float __cdecl GSX_getVersionNum();
    int __cdecl getDataToLoadSize(GSXCVehicle veh, const char *name);
    int __cdecl getDataToSaveSize(GSXCVehicle veh, const char *name);
    ]]

    local gsx = ffi.load("gsx.asi")

    module.DataToLoadExists = ffi.cast("int (*)(int,const char*)",gsx.dataToLoadExists)
    module.GetDataToLoadSize = ffi.cast("int (*)(int,const char*)",gsx.getDataToLoadSize)
    module.GetLoadDataByVehPtr = ffi.cast("int (*)(int,const char*)",gsx.getLoadDataByVehPtr)
    module.PushDirectlyToSavedData = ffi.cast("int (*)(int,const char*,int,int)",gsx.pushDirectlyToSavedData)
    module.RemoveNotifyCallback = ffi.cast("void (*)(int)",gsx.removeNotifyCallback)

    module.GSXpNotifyCallback = gsx.addNotifyCallback(function(data)
        local t = module.tvehicle.gsx.veh_data
        if data.status == 1 then -- save vehicle
            local wdata = encodeJson(t[data.veh])
            if wdata ~= nil then
                module.PushDirectlyToSavedData(data.veh,"CM_DATA",#wdata,memory.strptr(wdata));
            end
        else -- load vehicle
            if module.DataToLoadExists(data.veh,"CM_DATA") == 1 then
                local pdata = module.GetLoadDataByVehPtr(data.veh,"CM_DATA")
                local size = module.GetDataToLoadSize(data.veh,"CM_DATA") 
                t[data.veh] = decodeJson(memory.tostring(pdata,size,false))
            end
        end
    end)

end

function module.GSXProcessVehicles()

    if module.tvehicle.gsx.handle == 0 then
        return
    end

    local t = module.tvehicle.gsx.veh_data
    local s = module.tvehicle.gsx.p_table

    while true do 
        
        for i, car in ipairs(getAllVehicles()) do
            local pveh = getCarPointer(car)
            if t[pveh] == nil then 
                if module.DataToLoadExists(pveh,"CM_DATA") == 1 then
                    local pdata = module.GetLoadDataByVehPtr(pveh,"CM_DATA")
                    local size  = module.GetDataToLoadSize(pveh,"CM_DATA") 
                    t[pveh]     = decodeJson(memory.tostring(pdata,size,false))
                    s[car]      = pveh
                end
            end
        end

        for car, pveh in pairs(s) do
            if not doesVehicleExist(car) then 
                t[pveh] = nil
                s[car] = nil
                print("REMOVED")
            end
        end
        
        wait(10)
    end
end

function module.GSXSet(car,str,val)
    local t = module.tvehicle.gsx.veh_data

    if doesVehicleExist(car) then 
        local pveh = getCarPointer(car)
        if  t[pveh] == nil then 
            t[pveh] = {}
        end
        t[pveh][str] = val
    end
end

function module.GSXGet(car,str)
    local t = module.tvehicle.gsx.veh_data

    if doesVehicleExist(car) then 
        local pveh = getCarPointer(car)
        if t[pveh] then
            return t[pveh][str]
        end
    end
    return nil
end

--------------------------------------------------
-- Model/ info

function module.GetNameOfVehicleModel(model)
    return ffi.string(ffi.cast("char*", CVehicleModelInfo[tonumber(model)] + 0x32)) or ""
end

function module.GetModelInfo(name)
    local pInfo = allocateMemory(16)
    
    callFunction(0x4C5940,2,2,name,pInfo)
    local model = readMemory(pInfo,4,false)
    freeMemory(pInfo)
    
    if module.GetNameOfVehicleModel(model) ~= "" then
        return model
    else
        return 0
    end
end

-- Spawns a vehicle for player
function module.GiveVehicleToPlayer(model)
    model = tonumber(model)
    if isModelAvailable(model) then
        x,y,z = getCharCoordinates(PLAYER_PED)
        if isCharInAnyCar(PLAYER_PED) and ( module.tvehicle.spawn_inside[0]) then
            vehicle = getCarCharIsUsing(PLAYER_PED)
            speed = getCarSpeed(vehicle)
            warpCharFromCarToCoord(PLAYER_PED,x,y,z)
            deleteCar(vehicle)
        end

        if (module.tvehicle.aircraft.spawn_in_air[0]) and (isThisModelAHeli(model) or isThisModelAPlane(model)) then
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
            customPlateForNextCar(model,ffi.string(module.tvehicle.license_plate_text))
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
                if isKeyDown(0x56) then
                    while isKeyDown(0x56) do
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
-- Color/ paintjob

function module.ForEachCarComponent(func,skip,car)
    car = car or getCarCharIsUsing(PLAYER_PED)
    if car ~= nil then

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

function module.ParseCarcols()
    local file_path = string.format("%s/data/carcols.dat",getGameDirectory())
    local col_data = false
    local car_data = false

    if doesFileExist(file_path) then
  
        for line in io.lines(file_path) do
          --  print(line)
            if line == "col" then
                col_data = true
                goto continue
            end
            if line == "car" or line == "car4" then
                car_data = true
                goto continue
            end 
            if line == "end" then
                if col_data then col_data = false end
                if car_data then car_data = false end
                goto continue
            end
            if col_data then
                local r, g, b = string.match(line,"(%d+).(%d+).(%d+)")

                if r and g and b then
                    table.insert(module.tvehicle.color.col_data_table,string.format("%d %d %d",r,g,b))
                end
            end
            if car_data then
                local name = nil
                for x in string.gmatch(line,"[^,]+") do
                    if type(tonumber(x)) == "nil" then
                        name = string.upper(x)
                        module.tvehicle.color.car_data_table[name] = {}
                    end
                    if type(tonumber(x)) == "number" then
                        table.insert(module.tvehicle.color.car_data_table[name],tonumber(x))
                    end
                end
            end
            ::continue::
            wait(0)
        end
        module.tvehicle.color.parse_done = true
    end
end

function ApplyColor(load_saved_color,car)

    module.ForEachCarComponent(function(mat,comp,car)

        local r, g, b, old_a = mat:get_color()

        -- -1.0 used as nil
        if load_saved_color and module.tvehicle.gsx.handle ~= 0  then
            module.tvehicle.color.rgb[0] = module.GSXGet(car,"cm_color_red_" .. comp.name) or -1
            module.tvehicle.color.rgb[1] = module.GSXGet(car,"cm_color_green_" .. comp.name) or -1
            module.tvehicle.color.rgb[2] = module.GSXGet(car,"cm_color_blue_" .. comp.name) or -1
        end

        if (module.tvehicle.color.rgb[0] ~= -1.0 and module.tvehicle.color.rgb[0] ~= -1.0 and module.tvehicle.color.rgb[0] ~= -1.0)
        and (not module.tvehicle.apply_material_filter[0] or (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF)) then
            
            if module.tvehicle.components.selected[0] == 0 and not load_saved_texture then   
                mat:set_color(module.tvehicle.color.rgb[0]*255, module.tvehicle.color.rgb[1]*255, module.tvehicle.color.rgb[2]*255, 255.0)
                
                if module.tvehicle.gsx.handle ~= 0  then
                    module.GSXSet(car,"cm_color_red_" .. comp.name,module.tvehicle.color.rgb[0])
                    module.GSXSet(car,"cm_color_green_" .. comp.name,module.tvehicle.color.rgb[1])
                    module.GSXSet(car,"cm_color_blue_" .. comp.name,module.tvehicle.color.rgb[2])
                end
            end

            if comp.name == module.tvehicle.components.names[module.tvehicle.components.selected[0]+1] or load_saved_color then     
                mat:set_color(module.tvehicle.color.rgb[0]*255, module.tvehicle.color.rgb[1]*255, module.tvehicle.color.rgb[2]*255, 255.0)
                
                if module.tvehicle.gsx.handle ~= 0  then
                    module.GSXSet(car,"cm_color_red_" .. comp.name,module.tvehicle.color.rgb[0])
                    module.GSXSet(car,"cm_color_green_" .. comp.name,module.tvehicle.color.rgb[1])
                    module.GSXSet(car,"cm_color_blue_" .. comp.name,module.tvehicle.color.rgb[2])
                end
            end

        end
        module.tvehicle.color.default = getCarColours(car)
    end,false,car)  
end

function ApplyTexture(filename,load_saved_texture,car)

    module.ForEachCarComponent(function(mat,comp,car)
        local r, g, b, old_a = mat:get_color()

        local model = getCarModel(car)

        if load_saved_texture and module.tvehicle.gsx.handle ~= 0  then
            filename = module.GSXGet(car,"cm_texture_" .. comp.name)
        end

        if filename ~= nil then
            local fullpath = module.tvehicle.paintjobs.path .. "\\images\\" .. filename .. ".png"

            if doesFileExist(fullpath) then
                
                if module.tvehicle.paintjobs.cache_images[filename] == nil then
                    module.tvehicle.paintjobs.cache_images[filename] = mad.load_png_texture(fullpath)
                end

                module.tvehicle.paintjobs.texture = module.tvehicle.paintjobs.cache_images[filename]


                if not module.tvehicle.apply_material_filter[0] or (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF) then
                    if module.tvehicle.components.selected[0] == 0 and not load_saved_texture then
                        mat:set_texture(module.tvehicle.paintjobs.texture)
                        if module.tvehicle.gsx.handle ~= 0  then 
                            module.GSXSet(car,"cm_texture_" .. comp.name,filename)
                        end 
                    end
                    if comp.name == module.tvehicle.components.names[module.tvehicle.components.selected[0]+1] or load_saved_texture then
                        mat:set_texture(module.tvehicle.paintjobs.texture)
                        if module.tvehicle.gsx.handle ~= 0  then 
                            module.GSXSet(car,"cm_texture_" .. comp.name,filename)
                        end 
                    end
                end
            end
        end
        module.tvehicle.color.default = getCarColours(car)
    end,false,car)
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

--------------------------------------------------
-- Component/ tune

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
-- Misc

function module.OnEnterVehicle()

    while true do
        
        if isCharInAnyCar(PLAYER_PED) then
            local car        = getCarCharIsUsing(PLAYER_PED)
            local pCar       = getCarPointer(car)
            local model      = getCarModel(car)
            local model_name = module.tvehicle.gxt_name_table[module.GetNameOfVehicleModel(model)] or getGxtText(module.GetNameOfVehicleModel(model))

            -- Get vehicle components
            module.tvehicle.hidden_objects = {}
            module.tvehicle.components.names = {"default"}

            module.ForEachCarComponent(function(mat,comp,car)
                table.insert(module.tvehicle.components.names,comp.name)
            end,true)
            module.tvehicle.components.list  = imgui.new['const char*'][#module.tvehicle.components.names](module.tvehicle.components.names)

            --Load gsx data
            if module.tvehicle.gsx.handle ~= 0  then
                ApplyTexture(nil,true)
                ApplyColor(true)
                module.tvehicle.neon.rb_value[0] = module.GSXGet(car,"cm_neon_color") or -1
                module.tvehicle.neon.pulsing[0]  = module.GSXGet(car,"cm_neon_pulsing") or false
                InstallNeon(pCar)
            end
            
            imgui.StrCopy(module.tvehicle.gxt_name,model_name)

            while isCharInCar(PLAYER_PED,car) do
                wait(0)
            end
        end
        wait(0)
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
-- Main

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
    fcommon.Tabs("Vehicles",{"Checkboxes","Menus","Spawn","Paint","Tune","Handling"},{
        function()
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

            imgui.NextColumn()

            fcommon.CheckBoxFunc("Lock doors",module.tvehicle.lock_doors,
            function()
                if isCharInAnyCar(PLAYER_PED) then
                    local car   = getCarCharIsUsing(PLAYER_PED)
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
            fcommon.CheckBoxValue("New train camera",5416239,nil,fconst.TRAIN_CAM_FIX.ON,fconst.TRAIN_CAM_FIX.OFF) 
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
            fcommon.CheckBoxVar("Random colors",module.tvehicle.random_colors,"Paints players car with random\ncolors every second")
            fcommon.CheckBoxVar("Random traffic colors",module.tvehicle.random_colors_traffic,"Paints traffic cars with random colors every second")
            fcommon.CheckBoxValue("Tank mode",0x969164) 
            fcommon.CheckBoxVar("Traffic neons",module.tvehicle.neon.checkbox,"Adds neon lights to traffic vehicles.\nOnly some vehicles will have them.")
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
        end,
        function()
            fcommon.DropDownMenu("Enter nearest vehicle as",function()
                local vehicle,ped = storeClosestEntities(PLAYER_PED)
                if vehicle ~= -1 then
                    local seats = getMaximumNumberOfPassengers(vehicle)
                    imgui.Spacing()
                    imgui.Columns(2,nil,false)
                    imgui.Text(module.GetNameOfVehicleModel(getCarModel(vehicle)))
                    imgui.NextColumn()
                    imgui.Text(string.format("Total seats: %d",seats+1))
                    imgui.Columns(1)

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
            fcommon.DropDownMenu("License plate text",function()
                imgui.InputText("Text", module.tvehicle.license_plate_text,ffi.sizeof(module.tvehicle.license_plate_text))
                fcommon.InformationTooltip("The text of vehicle license plate\nwhich you spawn using cheat-menu")
            end)

            fcommon.DropDownMenu("Traffic options",function()
                fcommon.RadioButton("Color",{"Black","Pink"},{0x969151,0x969150})
                imgui.Spacing()
                fcommon.RadioButton("Type",{"Cheap","Country","Fast"},{0x96915E,0x96917B,0x96915F})
            end)

            if isCharInAnyCar(PLAYER_PED) then
                local car = getCarCharIsUsing(PLAYER_PED)
                local pCar = getCarPointer(car)
                local model = getCarModel(car)

                fcommon.UpdateAddress({name = 'Density multiplier',address = 0x8A5B20,size = 4,min = 0,max = 10, default = 1,is_float = true})
                fcommon.UpdateAddress({name = 'Dirt level',address = pCar + 1200 ,size = 4,min = 0,max = 15, default = 7.5,is_float = true})
                fcommon.DropDownMenu("Doors",function()
                    if isCharInAnyCar(PLAYER_PED) and not (isCharOnAnyBike(PLAYER_PED) or isCharInAnyBoat(PLAYER_PED) 
                    or isCharInAnyHeli(PLAYER_PED) or isCharInAnyPlane(PLAYER_PED)) then    
                        imgui.Columns(2,nil,false)
                        imgui.RadioButtonIntPtr("Damage", module.tvehicle.door_menu_button,0) 
                        imgui.RadioButtonIntPtr("Fix", module.tvehicle.door_menu_button,1) 
                        imgui.NextColumn()
                        imgui.RadioButtonIntPtr("Open", module.tvehicle.door_menu_button,2) 
                        imgui.RadioButtonIntPtr("Pop", module.tvehicle.door_menu_button,3)
                        imgui.Columns(1)

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
                -- fcommon.DropDownMenu("Hide components",function()
                --     if isCharInAnyCar(PLAYER_PED) then
                --         local count = 1

                --         imgui.Columns(2,nil,false)

                --         for _, comp in ipairs(mad.get_all_vehicle_components(car)) do

                --             if module.tvehicle.hidden_objects[comp.name] == nil then
                --                 module.tvehicle.hidden_objects[comp.name] = imgui.new.bool(false)
                --             end
                --             imgui.Checkbox(comp.name, module.tvehicle.hidden_objects[comp.name])
                --             for _, obj in ipairs(comp:get_objects()) do
                --                 obj:hide(module.tvehicle.hidden_objects[comp.name][0])
                --             end
                --             count = count + 1 
                --             if count == math.floor((#module.tvehicle.components.names/2)+1) then
                --                 imgui.NextColumn()
                --             end

                --         end
                --         imgui.Columns(1)
                --     end
                -- end)
                fcommon.DropDownMenu("Neons",function()
                    if module.tvehicle.neon["Handle"] ~= 0 then
                        imgui.Columns(3,nil,false)
                        imgui.RadioButtonIntPtr("Blue", module.tvehicle.neon.rb_value,2) 
                        imgui.RadioButtonIntPtr("Cyan", module.tvehicle.neon.rb_value,5)
                        imgui.RadioButtonIntPtr("Green", module.tvehicle.neon.rb_value,1) 
                        imgui.NextColumn()
                        imgui.RadioButtonIntPtr("None", module.tvehicle.neon.rb_value,-1)
                        imgui.RadioButtonIntPtr("Purple", module.tvehicle.neon.rb_value,6)
                        imgui.RadioButtonIntPtr("Red", module.tvehicle.neon.rb_value,0) 
                        imgui.NextColumn()
                        imgui.RadioButtonIntPtr("White", module.tvehicle.neon.rb_value,3)
                        imgui.RadioButtonIntPtr("Yellow", module.tvehicle.neon.rb_value,4)
                        imgui.Columns(1)

                        imgui.Dummy(imgui.ImVec2(0,20))
                        imgui.Checkbox("Pulsing",module.tvehicle.neon.pulsing)
                        fcommon.InformationTooltip("Neons will blink continuously")
                        imgui.Spacing()

                        if imgui.Button("Install Neon",imgui.ImVec2(fcommon.GetSize(1))) then
                            InstallNeon(pCar)
                        end
                    else
                        imgui.TextWrapped("Unable to find 'neon_api.asi'. Please reinstall Cheat Menu.")
                    end
                end)
                fcommon.UpdateAddress({name = 'Nitro count',address = pCar + 0x48A ,size = 1,min = 0,max = 15, default = 7.5,is_float = false})
                fcommon.DropDownMenu("Set name",function()

                    imgui.Text(string.format( "Model name = %s",module.GetNameOfVehicleModel(getCarModel(car))))
                    imgui.Spacing()
                    imgui.InputText("Name", module.tvehicle.gxt_name,ffi.sizeof(module.tvehicle.gxt_name))

                    imgui.Spacing()
                    if imgui.Button("Set",imgui.ImVec2(fcommon.GetSize(3))) then
                        setGxtEntry(module.GetNameOfVehicleModel(getCarModel(car)),ffi.string(module.tvehicle.gxt_name))
                        fcommon.CheatActivated()
                    end
                    imgui.SameLine()
                    if imgui.Button("Save",imgui.ImVec2(fcommon.GetSize(3))) then
                        module.tvehicle.gxt_name_table[module.GetNameOfVehicleModel(getCarModel(car))] = ffi.string(module.tvehicle.gxt_name)
                    end
                    imgui.SameLine()
                    if imgui.Button("Clear all",imgui.ImVec2(fcommon.GetSize(3))) then
                        module.tvehicle.gxt_name_table = {}
                    end
                end)
                fcommon.DropDownMenu("Set speed",function()
                    
                    fcommon.CheckBoxVar("Lock speed",module.tvehicle.lock_speed)
                    imgui.Spacing()
                    imgui.InputInt("Set",module.tvehicle.speed)
                     
                    imgui.Spacing()
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
                fcommon.UpdateAddress({name = 'Wheel scale',address = pCar+0x458,size = 4,min = 0,max = 10, default = 1,is_float = true})
                --fcommon.UpdateAddress({name = 'ZZZZZ',address = pCar+0x489,size = 4,min = -10,max = 10, default = 1,is_float = false})

            end            
        end,
        function()
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("Spawn inside",module.tvehicle.spawn_inside,"Spawn inside vehicle as driver")

            imgui.NextColumn()
            fcommon.CheckBoxVar("Spawn aircraft in air",module.tvehicle.aircraft.spawn_in_air)
            imgui.Columns(1)

            imgui.Spacing()
            fcommon.Tabs("Vehicles list",{"List","Search"},{
                function()
                    fcommon.DrawImages(fconst.IDENTIFIER.VEHICLE,fconst.DRAW_TYPE.LIST,module.tvehicle.images,fconst.VEHICLE.IMAGE_HEIGHT,fconst.VEHICLE.IMAGE_WIDTH,module.GiveVehicleToPlayer,nil,module.GetNameOfVehicleModel,module.tvehicle.filter)
                end,
                function()
                    fcommon.DrawImages(fconst.IDENTIFIER.VEHICLE,fconst.DRAW_TYPE.SEARCH,module.tvehicle.images,fconst.VEHICLE.IMAGE_HEIGHT,fconst.VEHICLE.IMAGE_WIDTH,module.GiveVehicleToPlayer,nil,module.GetNameOfVehicleModel,module.tvehicle.filter)
                end
            })
        end,
        function()
            if isCharInAnyCar(PLAYER_PED) then
                local car = getCarCharIsUsing(PLAYER_PED)
                local model = getCarModel(car)
            
                if imgui.Button("Reset color",imgui.ImVec2(fcommon.GetSize(2))) then

                    module.ForEachCarComponent(function(mat,comp,car)
                        mat:reset_color()
                        if module.tvehicle.gsx.handle ~= 0  then
                            module.GSXSet(car,"cm_color_red_" .. comp.name,module.tvehicle.color.rgb[0])
                            module.GSXSet(car,"cm_color_green_" .. comp.name,module.tvehicle.color.rgb[1])
                            module.GSXSet(car,"cm_color_blue_" .. comp.name,module.tvehicle.color.rgb[2])
                        end
                    end)
                    module.tvehicle.color.default = -1
                    printHelpString("Color reset")
                end
                imgui.SameLine()
                if imgui.Button("Reset texture",imgui.ImVec2(fcommon.GetSize(2))) then
                    fconfig.tconfig.temp_texture_name = nil
                    module.ForEachCarComponent(function(mat,comp,car)
                        mat:reset_texture()
                        if module.tvehicle.gsx.handle ~= 0  then 
                            module.GSXSet(car,"cm_texture_" .. comp.name,filename)
                        end 
                    end)
                    module.tvehicle.paintjobs.texture = nil
                    printHelpString("Texture reset")
                end

                imgui.Spacing()
                imgui.Columns(2,nil,false)
                fcommon.CheckBoxVar("Material filter",module.tvehicle.apply_material_filter)
                fcommon.InformationTooltip("Filters material while applying color/ texture\nDisable if something doesn't work properly")
                imgui.NextColumn()
                imgui.Columns(1)
                imgui.Spacing()
                
                if imgui.ColorEdit3("Color",module.tvehicle.color.rgb) then
                    ApplyColor()
                end
                fcommon.ConfigPanel({nil,"Color"},function()
                    if not isCharInAnyCar(PLAYER_PED) then
                        tcheatmenu.window.panel_func = nil
                    end
                    if module.tvehicle.color.parse_done then
                        fcommon.CheckBoxVar("Show all", module.tvehicle.color.show_all,"Show all carcol colors")
                        imgui.Spacing()
                        
                        local name = module.GetNameOfVehicleModel(getCarModel(car))
                        
                        if module.tvehicle.color.car_data_table[name] ~= nil then
                            local shown_colors = {}
                            imgui.Text("Color:")
                            imgui.Spacing()
                            imgui.Columns(2,nil,false)
                            imgui.RadioButtonIntPtr("Color 1", module.tvehicle.color.radio_btn, 1)
                            imgui.RadioButtonIntPtr("Color 2", module.tvehicle.color.radio_btn, 2)
                            imgui.NextColumn()
                            imgui.RadioButtonIntPtr("Color 3", module.tvehicle.color.radio_btn, 3)
                            imgui.RadioButtonIntPtr("Color 4", module.tvehicle.color.radio_btn, 4)
                            imgui.Spacing()
                            imgui.Columns(1)
                            imgui.Text("Select color preset:")
                            imgui.Spacing()

                            if imgui.BeginChild("Colors") then
                                local x,y = fcommon.GetSize(1)
                                local btns_in_row = math.floor(imgui.GetWindowContentRegionWidth()/(y*2))
                                local btn_size = (imgui.GetWindowContentRegionWidth() - imgui.StyleVar.ItemSpacing*(btns_in_row-0.75*btns_in_row))/btns_in_row
                                local btn_count = 1

                                func = function(v)
                                    if not shown_colors[v] then
                                        local t = {}
                                        local k =  1
                                     
                                        for i in string.gmatch(module.tvehicle.color.col_data_table[v+1],"%w+") do 
                                            table.insert( t,tonumber(i))
                                        end

                                        if imgui.ColorButton("Color " .. tostring(v),imgui.ImVec4(t[1]/255,t[2]/255,t[3]/255,255),0,imgui.ImVec2(btn_size,btn_size)) then
                                            writeMemory(getCarPointer(car) + 1075 + module.tvehicle.color.radio_btn[0],1,tonumber(v),false)
                                            module.ForEachCarComponent(function(mat,comp,car)
                                                mat:reset_color()
                                                if module.tvehicle.gsx.handle ~= 0  then
                                                    module.GSXSet(car,"cm_color_red_" .. comp.name,module.tvehicle.color.rgb[0])
                                                    module.GSXSet(car,"cm_color_green_" .. comp.name,module.tvehicle.color.rgb[1])
                                                    module.GSXSet(car,"cm_color_blue_" .. comp.name,module.tvehicle.color.rgb[2])
                                                end
                                            end)
                                        end
                                        if imgui.IsItemHovered() then
                                            local drawlist = imgui.GetWindowDrawList()
                                            drawlist:AddRectFilled(imgui.GetItemRectMin(), imgui.GetItemRectMax(), imgui.GetColorU32(imgui.Col.ModalWindowDimBg))
                                        end
                                        shown_colors[v] = true
                                        if btn_count % btns_in_row ~= 0 then
                                            imgui.SameLine(0.0,4.0)
                                        end
                                        btn_count = btn_count + 1
                                    end
                                end

                                if module.tvehicle.color.show_all[0] then       
                                    for v=0,(#module.tvehicle.color.col_data_table-1),1 do
                                        func(v)
                                    end
                                else
                                    for k,v in ipairs(module.tvehicle.color.car_data_table[name]) do
                                        func(v)
                                    end
                                end
                                imgui.EndChild()
                            end
                        end
                    else
                        imgui.Text("Parsing 'carcols.dat'")
                    end
                end)
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

                fcommon.DrawImages(fconst.IDENTIFIER.PAINTJOB,fconst.DRAW_TYPE.SEARCH,module.tvehicle.paintjobs.images,fconst.PAINTJOB.IMAGE_HEIGHT,fconst.PAINTJOB.IMAGE_WIDTH,ApplyTexture,nil,module.GetTextureName,module.tvehicle.paintjobs.filter)
            else
                imgui.TextWrapped("Player needs to be inside a vehicle for options to show up here.")
            end
        end,
        function()
            if isCharInAnyCar(PLAYER_PED) then
                local car = getCarCharIsUsing(PLAYER_PED)
                local model = getCarModel(car)

                if imgui.Button("Reset vehicle components",imgui.ImVec2(fcommon.GetSize(1))) then
                    for x=1,fconst.COMPONENT.TOTAL_SLOTS,1 do
                        local comp_model = getCurrentCarMod(car,x)

                        if comp_model ~= -1 then
                            module.RemoveComponentFromVehicle(comp_model,car,true)
                        end
                    end
                    printHelpString("Vehicle components reset")
                end
                imgui.Spacing()
                imgui.Text("Info")
                fcommon.InformationTooltip("Left click to add component\nRight click to remove component")
                imgui.Spacing()
                if imgui.BeginChild("Tune") then
                    imgui.Spacing()
                    fcommon.DrawImages(fconst.IDENTIFIER.COMPONENT,fconst.DRAW_TYPE.LIST,module.tvehicle.components.images,fconst.COMPONENT.IMAGE_HEIGHT,fconst.COMPONENT.IMAGE_WIDTH,module.AddComponentToVehicle,module.RemoveComponentFromVehicle,nil,module.tvehicle.components.filter)
                    imgui.EndChild()
                end
            else
                imgui.TextWrapped("Player needs to be inside a vehicle for options to show up here.")
            end
        end,
        function()
            if isCharInAnyCar(PLAYER_PED) then
                local car = getCarCharIsUsing(PLAYER_PED)
                local model = getCarModel(car)
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

                if imgui.Button("Reset game handling",imgui.ImVec2(fcommon.GetSize(1))) then
                    local cHandlingDataMgr = readMemory(0x05BFA96,4,false)
                    callMethod(0x5BD830,cHandlingDataMgr,0,0)
                    printHelpString("Handling reset")
                end
    
                imgui.Spacing()
                if imgui.BeginChild("Handling") then

                    fcommon.RadioButtonFunc("ABS",{"On","Off"},{1.0,0.0},phandling + 0x9C)
                    fcommon.UpdateAddress({name = 'Brake bias',address = phandling + 0x98 ,size = 4,min = 0,max = 1,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Brake deceleration',address = phandling + 0x94 ,size = 4,min = 0.1,max = 10,is_float = true,factor = 3.9999999e-4,cvalue = 0.1})
                    fcommon.UpdateAddress({name = 'Centre of mass X',address = phandling + 0x14 ,size = 1,min = -10.0,max = 10.0,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Centre of mass Y',address = phandling + 0x18 ,size = 1,min = -10.0,max = 10.0,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Centre of mass Z',address = phandling + 0x1C ,size = 1,min = -10.0,max = 10.0,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Collision damage multiplier',address = phandling + 0xC8 ,size = 4,min = 0.2,max = 5.0,is_float = true,cvalue = 0.05})                    
                    fcommon.UpdateAddress({name = 'Damage multiplier',address = phandling + CDM ,size = 4,min = 0.0,max = 5.0,is_float = true})
                    fcommon.UpdateAddress({name = 'Damping level',address = phandling + 0xB0 ,size = 4,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Drag mult',address = phandling + 0x10 ,size = 4,min = 1,max = 20.0,is_float = true})
                    fcommon.RadioButtonFunc("Drive type",{"Front wheel drive","Rear wheel drive","Four wheel drive"},{fconst.DRIVE_TYPE.FWD,fconst.DRIVE_TYPE.RWD,fconst.DRIVE_TYPE.AWD},phandling + 0x74)
                    fcommon.UpdateAddress({name = 'Engine acceleration',address = phandling + 0x7C ,size = 4,min = 1,max = 10,is_float = true,factor = 3.9999999e-4})
                    fcommon.UpdateAddress({name = 'Engine inertia',address = phandling + 0x80 ,size = 4,min = 0,max = 50,is_float = true})
                    fcommon.RadioButtonFunc("Engine type",{"Petrol","Diseal","Electric"},{fconst.ENGINE_TYPE.PETROL,fconst.ENGINE_TYPE.DISEAL,fconst.ENGINE_TYPE.ELECTRIC},phandling + 0x75)
                    fcommon.RadioButtonFunc("Front lights",{"Long","Small","Big","Tall"},{fconst.LIGHTS.LONG,fconst.LIGHTS.SMALL,fconst.LIGHTS.BIG,fconst.LIGHTS.TALL},phandling + 0xDC)
                    fcommon.UpdateAddress({name = 'Force level',address = phandling + 0xAC ,size = 4,is_float = true,cvalue = 0.1})
                    fcommon.UpdateAddress({name = 'High speed damping',address = phandling + 0xB4 ,size = 4,is_float = true,cvalue = 0.1})
                    fcommon.UpdateAddress({name = 'Lower limit',address = phandling + 0xBC ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Mass',address = phandling + 0x4 ,size = 4,min = 1,max = 50000,is_float = true})
                    fcommon.UpdateAddress({name = 'Max velocity',address = phandling + 0x84 ,size = 4,min = 1,max = 150,is_float = true,factor = 5.5555599e-3})
                    fcommon.UpdateAddress({name = 'Monetary value',address = phandling + 0xD8 ,size = 4,min = 1,max = 100000})
                    fcommon.UpdateAddress({name = 'Number of gears',address = phandling + 0x76 ,size = 1,min = 1,max = 10})
                    fcommon.UpdateAddress({name = 'Percent submerged',address = phandling + 0x20 ,size = 1,min = 10,max = 120})
                    fcommon.RadioButtonFunc("Rear lights",{"Long","Small","Big","Tall"},{fconst.LIGHTS.LONG,fconst.LIGHTS.SMALL,fconst.LIGHTS.BIG,fconst.LIGHTS.TALL},phandling + 0xDD)
                    fcommon.UpdateAddress({name = 'Seat offset distance',address = phandling + 0xD4 ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Steering lock',address = phandling + 0xA0 ,size = 4,min = 10,max = 40,is_float = true})
                    fcommon.UpdateAddress({name = 'Suspension bias',address = phandling + 0xC0 ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Traction bias',address = phandling + 0xA8 ,size = 4,min = 0,max = 1,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Traction loss',address = phandling + 0xA4 ,size = 4,min = 0,max = 1,is_float = true,cvalue = 0.01})
                    fcommon.UpdateAddress({name = 'Turn mass',address = phandling + 0xC ,size = 4,min = 0,max = 20,is_float = true})
                    fcommon.UpdateAddress({name = 'Traction multiplier',address = phandling + 0x28 ,size = 4,min = 0.2,max = 2,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Upper limit',address = phandling + 0xB8 ,size = 4,is_float = true,cvalue = 0.05})
                    fcommon.UpdateAddress({name = 'Vehicle anim group',address = phandling + 0xDE ,size = 1})
                    imgui.EndChild()
                end
            else
                imgui.TextWrapped("Player needs to be inside a vehicle for options to show up here.")
            end
        end
    })
end

return module
