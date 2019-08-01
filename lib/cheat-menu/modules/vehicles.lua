-- This module contains all the functions for vehicles tab

local module = {}
local tvehicles =
{
    components =
    {
        saved = false,
        path   =  getGameDirectory() .. "\\moonloader\\lib\\cheat-menu\\vehicles\\components\\",
        images = {},
        value  = imgui.new.int(0),
        list   = {},
        selected = imgui.new.int(0),
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

    },
    aircraft =
    {
        camera = imgui.new.bool(fconfig.get('tvehicles.aircraft.camera',false)),
        spawn_in_air = imgui.new.bool(fconfig.get('tvehicles.aircraft.spawn_in_air',true)),
        zoom = { -5.0,-15.0,-20.0,-30.0,-40.0},
        index = fconfig.get('tvehicles.aircraft.index',2),
    },
    search_text = imgui.new.char[20](),
    models = {},
    show = {
        speed = imgui.new.bool(fconfig.get('tvehicles.show.speed',false)),
        health = imgui.new.bool(fconfig.get('tvehicles.show.health',false)),
    },

    color =
    {
        rgb     = imgui.new.float[3](0.0,0.0,0.0),
        default = -1,
    },
    lights =
    {
        all    = imgui.new.bool(fconfig.get('tvehicles.lights.all',false)),
        --fleft  = imgui.new.bool(false),
        --fright = imgui.new.bool(false),
        --bleft  = imgui.new.bool(false),
        --bright = imgui.new.bool(false),
    },
    images = {},
    quick_spawn  = imgui.new.bool(fconfig.get('tvehicles.quick_spawn',false)),
    spawn_inside = imgui.new.bool(fconfig.get('tvehicles.spawn_inside',true)),
    path = getGameDirectory() .. "\\moonloader\\lib\\cheat-menu\\vehicles\\",
    visual_damage = imgui.new.bool(fconfig.get('tvehicles.visual_damage',false)),
    heavy = imgui.new.bool(fconfig.get('tvehicles.heavy',false)),
    hydraulic = imgui.new.bool(false),
    lock_health = imgui.new.bool(fconfig.get('tvehicles.lock_health',false)),
    stay_on_bike = imgui.new.bool(fconfig.get('tvehicles.stay_on_bike',false)),
    speed = imgui.new.int(fconfig.get('tvehicles.speed',0)),
    lock_speed = imgui.new.bool(fconfig.get('tvehicles.lock_speed',false)),
}


module.tvehicles = tvehicles
tvehicles.components.list  = imgui.new['const char*'][#tvehicles.components.names](tvehicles.components.names)

for i = 401,611,1 do
    table.insert(tvehicles.models,i)
end

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

function module.GiveVehicleToPlayer(model)
    if isModelAvailable(model) then
        x,y,z = getCharCoordinates(PLAYER_PED)
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)
            speed = getCarSpeed(car)
            warpCharFromCarToCoord(PLAYER_PED,x,y,z)
            deleteCar(car)
        end

        if tvehicles.aircraft.spawn_in_air[0] and (isThisModelAHeli(model) or isThisModelAPlane(model)) then
            z = 400
        end
        requestModel(model)
        loadAllModelsNow()
        car = createCar(model,x,y,z)
        markModelAsNoLongerNeeded(model)
        heading = getCharHeading(PLAYER_PED)
        if tvehicles.spawn_inside[0] then
            warpCharIntoCar(PLAYER_PED,car)
        end
        setCarHeading(car,heading)
        setCarForwardSpeed(car,speed)
        markCarAsNoLongerNeeded(car)
        fcommon.CheatActivated()
    end
end


function module.AddComponentToVehicle(component)

    if isCharInAnyCar(PLAYER_PED) then
        local car = storeCarCharIsInNoSave(PLAYER_PED)
        if isModelAvailable(component) then
            requestVehicleMod(component)
            loadAllModelsNow()
            addVehicleMod(car,component)
            fcommon.CheatActivated()
            markModelAsNoLongerNeeded(component)
        end
    end
end


function module.ForEachCarComponent(func)
    if isCharInAnyCar(PLAYER_PED) then
        car = storeCarCharIsInNoSave(PLAYER_PED)
        for _, comp in ipairs(mad.get_all_vehicle_components(car)) do
            for _, obj in ipairs(comp:get_objects()) do
                for _, mat in ipairs(obj:get_materials()) do
                    func(mat,comp,car)
                end
            end
        end
        markCarAsNoLongerNeeded(car)
    end
end

function module.AircraftCamera()
    if tvehicles.aircraft.camera[0] == true then
        while isCharInAnyHeli(PLAYER_PED)
        or isCharInAnyPlane(PLAYER_PED) do
            if tvehicles.aircraft.camera[0] == false then break end

            local vehicle = storeCarCharIsInNoSave(PLAYER_PED)
            local roll = getCarRoll(vehicle)

            attachCameraToVehicle(vehicle,0.0,tvehicles.aircraft.zoom[tvehicles.aircraft.index],2.5,0.0,0.0,0.0,(roll*-1),2)
            if isKeyDown(tkeys.aircraft_zoom) then
                while isKeyDown(tkeys.aircraft_zoom) do
                    wait(0)
                end
                tvehicles.aircraft.index = tvehicles.aircraft.index + 1
                if tvehicles.aircraft.index > #tvehicles.aircraft.zoom then
                    tvehicles.aircraft.index  = 0
                end
            end
            wait(0)
        end
        restoreCameraJumpcut()
    end
end

function module.VehiclesMain()
    imgui.Spacing()

    if imgui.Button("Repair Vehicle",imgui.ImVec2(fcommon.GetSize(3))) then
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)
            fixCar(car)
            fcommon.CheatActivated()
        end
    end
    imgui.SameLine()
    if imgui.Button("Unflip Vehicle",imgui.ImVec2(fcommon.GetSize(3))) then
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)
            setCarRoll(car,0)
            fcommon.CheatActivated()
        end
    end
    imgui.SameLine()
    if imgui.Button("Lock Doors",imgui.ImVec2(fcommon.GetSize(3))) then
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)
            if getCarDoorLockStatus(car) == 4 then
                lockCarDoors(car,1)
                fcommon.CheatDeactivated()
            else
                lockCarDoors(car,4)
                fcommon.CheatActivated()
            end
        end
    end
    imgui.Spacing()
    if imgui.BeginTabBar("Vehicles") then

        if imgui.BeginTabItem("Checkboxes") then
            imgui.Spacing()

            imgui.Columns(2,nil,false)
            fcommon.CheckBox({name = "Car heavy",var = tvehicles.heavy})
            fcommon.CheckBox({name = "Show health",var = tvehicles.show.health})
            fcommon.CheckBox({name = "Stay on Bike",var = tvehicles.stay_on_bike,help_text = "Prevents falling from bikes."})

            if isCharInAnyCar(PLAYER_PED) then
                car = getCarCharIsUsing(PLAYER_PED)
                tvehicles.hydraulic[0] = doesCarHaveHydraulics(car)
            end
            if imgui.Checkbox("Car hydraulic",tvehicles.hydraulic) then
                if isCharInAnyCar(PLAYER_PED) then
                    if fvehicles.tvehicles.hydraulic[0] then
                        setCarHydraulics(car,true)
                        fcommon.CheatActivated()
                    else
                        setCarHydraulics(car,false)
                        fcommon.CheatDeactivated()
                    end
                end
            end
            fcommon.CheckBox({name = "Lock Car health",var = tvehicles.lock_health})
            fcommon.CheckBox({name = "No car visual damage",var = tvehicles.visual_damage})
            fcommon.CheckBox({ address = 0x969164,name = "Tank mode"})
            fcommon.CheckBox({ address = 0x96914B,name = "Wheels only",help_text = "Only can wheels are shown."})
            fcommon.CheckBox({ address = 0x96914E,name = "All lights green",help_text = "All traffic lights are always green."})
            fcommon.CheckBox({name = "New aircraft camera",var = tvehicles.aircraft.camera})

            imgui.NextColumn()

            fcommon.CheckBox({ address = 0x969160,name = "Cars can fly"})
            fcommon.CheckBox({ address = 0x969153,name = "Boats can fly"})
            fcommon.CheckBox({ address = 0x96914C,name = "Perfect handling"})
            fcommon.CheckBox({ address = 0x96917A,name = "Decreased Traffic"})
            fcommon.CheckBox({ address = 0x96914F,name = "Aggressive drivers"})
            fcommon.CheckBox({ address = 0x969165,name = "All cars have nitro"})
            fcommon.CheckBox({ address = 0x969152,name = "Cars can drive on water"})
            fcommon.CheckBox({ address = 0x969166,name = "Cars float away when hit"})
            if imgui.Checkbox("Vehicle lights on",tvehicles.lights.all) then
                if isCharInAnyCar(PLAYER_PED) then
                    car = storeCarCharIsInNoSave(PLAYER_PED)
                    if fvehicles.tvehicles.lights.all[0] == true then
                        forceCarLights(car,2)
                        addOneOffSound(x,y,z,1052)
                    else
                        forceCarLights(car,1)
                        addOneOffSound(x,y,z,1053)
                    end
                end
            end
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.RadioButton("Traffic Type",{"Cheap Traffic","Fast Traffic","Country Traffic"},{0x96915E,0x96915F,0x96917B})
            imgui.NextColumn()
            fcommon.RadioButton("Traffic Color",{"Pink Traffic","Black Traffic"},{0x969150,0x969151})
            imgui.Columns(1)
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Menus') then

            fcommon.DropDownMenu("Speed",function()
                imgui.Columns(2,nil,false)
                fcommon.CheckBox({name = "Show speed",var = tvehicles.show.speed})
                imgui.NextColumn()
                fcommon.CheckBox({name = "Lock speed",var = tvehicles.lock_speed})
                imgui.Columns(1)
                if imgui.InputInt("Set",tvehicles.speed) then
                end
                if imgui.Button("Set Speed",imgui.ImVec2(fcommon.GetSize(2))) then
                    if tvehicles.speed[0] > 500 then
                        tvehicles.speed[0] = 500
                    end
                    if isCharInAnyCar(PLAYER_PED) then
                        car = storeCarCharIsInNoSave(PLAYER_PED)
                        setCarForwardSpeed(car,tvehicles.speed[0])
                    end
                end
                imgui.SameLine()
                if imgui.Button("Instant Stop",imgui.ImVec2(fcommon.GetSize(2))) then
                    if isCharInAnyCar(PLAYER_PED) then
                        car = storeCarCharIsInNoSave(PLAYER_PED)
                        setCarForwardSpeed(car,0.0)
                    end
                end
            end)
            fcommon.DropDownMenu("Colors",function()
                if imgui.Button("Reset car color",imgui.ImVec2(fcommon.GetSize(1))) then
                    module.ForEachCarComponent(function(mat,car)
                        mat:reset_color()
                        tvehicles.color.default = getCarColours(car)
                    end)
                end
                imgui.Spacing()
                imgui.Combo("Component(s)",tvehicles.components.selected,tvehicles.components.list,#tvehicles.components.names)

                if imgui.ColorPicker3("Color",tvehicles.color.rgb) then
                    module.ForEachCarComponent(function(mat,comp,car)
                        local r, g, b, old_a = mat:get_color()
                        fixCar(car)
                        if tvehicles.components.selected[0] == 0 and (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF) then
                            mat:set_color(tvehicles.color.rgb[0]*255, tvehicles.color.rgb[1]*255, tvehicles.color.rgb[2]*255, 255.0)
                        end
                        if comp.name == tvehicles.components.names[tvehicles.components.selected[0]+1] then
                            mat:set_color(tvehicles.color.rgb[0]*255, tvehicles.color.rgb[1]*255, tvehicles.color.rgb[2]*255, 255.0)
                        end
                        tvehicles.color.default = getCarColours(car)
                    end)
                end
            end)

            imgui.EndTabItem()
        end
        if isCharInAnyCar(PLAYER_PED) then
            if imgui.BeginTabItem('Tune') then
                imgui.Spacing()
                if imgui.Button("Restore vehicle mods",imgui.ImVec2(150,25)) then
                    if isCharInAnyCar(PLAYER_PED) and tvehicles.components.saved then
                        callFunction(0x49B3C0,0,0)
                    end
                end
                imgui.SameLine()
                if imgui.Button("Save vehicle mods",imgui.ImVec2(150,25)) then
                    if isCharInAnyCar(PLAYER_PED) then
                        callFunction(0x49B280,0,0)
                        tvehicles.components.saved = true
                    end
                end
                imgui.Spacing()
                if imgui.BeginChild('Tune') then
                    imgui.Spacing()
                    fcommon.ShowEntries("Bullbar",{1100,1109,1110,1115,1116,1123,1125},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Exhaust",{1018,1019,1020,1021,1022,1028,1029,1034,1037,1043,1044,1045,1046,1059,1064,1065,1066,1089,1092,1104,1105,1113,1114,1126,1127,1129,1132,1135,1136},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Front Bumper",{1117,1152,1153,1155,1157,1160,1165,1166,1169,1170,1171,1172,1173,1174,1175,1179,1181,1182,1185,1188,1189,1190,1191},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Front sign",{1111,1112},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Hood",{1004,1005,1011,1012},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Lamps",{1013,1024},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Misc",{1086,1087},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Nitros",{1008,1009,1010},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Rear Bumper",{1140,1141,1148,1149,1150,1151,1154,1156,1159,1161,1167,1168,1176,1177,1178,1180,1183,1184,1186,1187,1192,1193},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Roof",{1006,1032,1033,1035,1038,1053,1054,1055,1061,1067,1068,1088,1091,1103,1028,1130,1131},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Sideskirt",{1007,1017,1026,1027,1030,1031,1036,1039,1040,1041,1042,1047,1048,1051,1052,1056,1057,1062,1063,1069,1070,1071,1072,1090,1093,1094,1095,1099,1101,1102,1106,1107,1108,1118,1119,1120,1121,1122,1124,1133,1134,1137},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Spoiler",{1000,1001,1002,1003,1014,1015,1016,1023,1049,1050,1058,1060,1138,1139,1146,1147,1158,1162,1163,1164},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Vents",{1142,1143,1144,1145},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,false)
                    fcommon.ShowEntries("Wheels",{1025,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1096,1097,1098},75,150,tvehicles.components.images,tvehicles.components.path,".jpg", fvehicles.AddComponentToVehicle,nil,true)
                    imgui.EndChild()
                end
                imgui.EndTabItem()
            end
        end
        if imgui.BeginTabItem('Spawn') then

            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({name = "Quick vehicle",var = tvehicles.quick_spawn,help_text = "Vehicle can be spawned from quick spawner using (Left Ctrl + Q). \n\nControls:\nEnter = Stop reading key press\nDelete = Erase full string\nBackspace = Erase last character"})
            fcommon.CheckBox({name = "Spawn Inside",var = tvehicles.spawn_inside,help_text = "Spawn inside vehicle as driver."})

            imgui.NextColumn()
            fcommon.CheckBox({name = "Spawn aircraft in air",var = tvehicles.aircraft.spawn_in_air})
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Text("Vehicles list")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Vehicle list Window") then
                fcommon.ShowEntries("Airplanes",{592,577,511,512,593,520,553,476,519,460,513},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Helicopters",{548,425,417,487,488,497,563,447,469},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Boats",{472,473,493,595,484,430,453,452,446,454},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Bikes",{581,509,481,462,521,463,510,522,461,448,468,586},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("2-Door & Compact cars",{602,496,401,518,527,589,419,587,533,526,474,545,517,410,600,436,439,549,491},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("4-Door & Luxury cars",{445,604,507,585,466,492,546,551,516,467,426,547,405,580,409,550,566,540,421,529},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Civil Service",{485,431,438,437,574,420,525,408,552},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Government Vehicles",{416,433,427,490,528,407,544,523,470,596,598,599,597,432,601,428},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Heavy & Utility trucks",{499,609,498,524,532,578,486,406,573,455,403,423,414,443,515,514,531,456,588},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Light trucks & Vans",{459,422,482,605,530,418,572,582,413,440,543,583,478,554},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("SUVs & Wagons",{579,400,404,489,505,479,442,458},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Lowriders",{536,575,534,567,535,576,412},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Muscle Cars",{402,542,603,475},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("RC Vehicles",{441,464,594,501,465,564},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Recreational",{568,424,504,457,483,508,571,500,444,556,557,471,495,539},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Street Racers",{429,541,542,415,480,562,565,434,494,502,503,411,559,561,560,506,451,558,555,477},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Trailers",{435,450,584,591,606,607,608,610,611},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                fcommon.ShowEntries("Trains",{449,537,538,569,570,590},75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Search') then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",tvehicles.search_text,ffi.sizeof(tvehicles.search_text)) then end
            imgui.SameLine()

            imgui.Spacing()
            imgui.Text("Found entries:(" .. ffi.string(tvehicles.search_text) .. ")")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Vehicle Entries") then
                fcommon.ShowEntries(nil,tvehicles.models,75,150,tvehicles.images,tvehicles.path,".jpg", fvehicles.GiveVehicleToPlayer,getNameOfVehicleModel,true,tvehicles.search_text)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
