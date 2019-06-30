-- This module contains all the functions for vehicles tab

local module = {}
local tvehicles =
{
    image_size =
    {
        x = resX/14,
        y = resY/15,
    },
    components = 
    {
        path =  getGameDirectory() .. "\\moonloader\\cheat-menu\\vehicles\\components\\",
        images = {},
        value = imgui.ImInt(0),
        names = 
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
            "boot_dummy",
        },

    },
    show = {
        speed = imgui.ImBool(false),
        health = imgui.ImBool(false),
    },

    color = 
    {
        rgb     = imgui.ImFloat3(0.0,0.0,0.0),
        default = -1,
    },
    lights = 
    {
        all    = imgui.ImBool(false),
        fleft  = imgui.ImBool(false),
        fright = imgui.ImBool(false),
        bleft  = imgui.ImBool(false),
        bright = imgui.ImBool(false),
    },
    images = {},
    spawn_plane_in_air = imgui.ImBool(true),
    quick_spawn = imgui.ImBool(false),
    path = getGameDirectory() .. "\\moonloader\\cheat-menu\\vehicles\\",
    visual_damage = imgui.ImBool(false),
    heavy = imgui.ImBool(false),
    hydraulic = imgui.ImBool(false),
    full_health = imgui.ImBool(false),
    stay_on_bike = imgui.ImBool(false),
    speed = imgui.ImInt(0),
    lock_speed = imgui.ImBool(false),
}


module.tvehicles = tvehicles

function module.CBaseModelInfo(name)
    local pInfo = allocateMemory(16)
    callFunction(0x004C5940,2,2,name,pInfo)
    local model = readMemory(pInfo,4,false)
    freeMemory(pInfo)
    return model
end

function module.give_vehicle_to_player(model)
    if isModelAvailable(model) then
        x,y,z = getCharCoordinates(PLAYER_PED)
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)
            speed = getCarSpeed(car)
            warpCharFromCarToCoord(PLAYER_PED,x,y,z)
            deleteCar(car)
        end
        if isThisModelAPlane(model) and tvehicles.spawn_plane_in_air.v == true then
            z = 400
        end
        requestModel(model)
        loadAllModelsNow()
        car = createCar(model,x,y,z)
        markModelAsNoLongerNeeded(model)
        heading = getCharHeading(PLAYER_PED)
        warpCharIntoCar(PLAYER_PED,car)
        setCarHeading(car,heading)
        setCarForwardSpeed(car,speed)
        markCarAsNoLongerNeeded(car)
        fcommon.CheatActivated()
    end
end


function module.add_component_to_vehicle(component)
    
    if isCharInAnyCar(PLAYER_PED) then
        local car = storeCarCharIsInNoSave(PLAYER_PED)
    --    local CVehicle = getCarPointer(car)

      --  if callMethod(0x6D62A0,CVehicle,2,2,CVehicle,component) == 0 then
            if isModelAvailable(component) then
                requestVehicleMod(component)
                loadAllModelsNow()
                addVehicleMod(car,component)
                fcommon.CheatActivated()
                markModelAsNoLongerNeeded(component)
            end
       -- else
           -- removeVehicleMod(car,component)
       -- end
    end
end


function module.for_each_car_component(func)
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

function module.vehicles_section()
    imgui.Spacing()
    imgui.Text("Vehicles")
    imgui.Separator()
    imgui.Spacing()

    if imgui.Button("Repair Vehicle",imgui.ImVec2(fcommon.getsize(3),50)) then
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)        
            fixCar(car)
            fcommon.CheatActivated()
        end
    end
    imgui.SameLine()
    if imgui.Button("Unflip Vehicle",imgui.ImVec2(fcommon.getsize(3),50)) then
        if isCharInAnyCar(PLAYER_PED) then
            car = storeCarCharIsInNoSave(PLAYER_PED)        
            setCarRoll(car,0)
            fcommon.CheatActivated()
            requestVehicleMod(1078)
            loadAllModelsNow()
            addVehicleMod(car,1078)
        end
    end
    imgui.SameLine()
    if imgui.Button("Lock Doors",imgui.ImVec2(fcommon.getsize(3),50)) then
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
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()

    if imgui.BeginMenu("Speed") then
        imgui.Spacing()
        imgui.Text("Vehicle Speed")
        fcommon.information_tooltip("Sets player vehicle speed.")
        imgui.Separator()
        imgui.Spacing()
        fcommon.ValueSwitch({ name = "Show speed",var = tvehicles.show.speed,show_help_popups = true})
        imgui.SameLine()
        fcommon.ValueSwitch({ name = "Lock speed",var = tvehicles.lock_speed,show_help_popups = true})
        if imgui.InputInt("Set",tvehicles.speed) then
        end
        if imgui.Button("Set Speed",imgui.ImVec2(75,22)) then
            if tvehicles.speed.v > 500 then
                tvehicles.speed.v = 500 
            end
            if isCharInAnyCar(PLAYER_PED) then
                car = storeCarCharIsInNoSave(PLAYER_PED)
                setCarForwardSpeed(car,tvehicles.speed.v)
            end
        end
        imgui.SameLine()
        if imgui.Button("Instant Stop",imgui.ImVec2(75,22)) then
            if isCharInAnyCar(PLAYER_PED) then
                car = storeCarCharIsInNoSave(PLAYER_PED)
                setCarForwardSpeed(car,0.0)
            end
        end
        imgui.EndMenu()
    end

    if imgui.BeginMenu("Colors") then
        imgui.Spacing()
        imgui.Text("Vehicle Colors")
        fcommon.information_tooltip("Sets vehicle color")
        imgui.Separator()
        imgui.Spacing()
        if imgui.Button("Reset car color",imgui.ImVec2(100.0,25.0)) then
            module.for_each_car_component(function(mat,car)
                mat:reset_color()
                tvehicles.color.default = getCarColours(car)
            end)
        end
        imgui.Combo("Component(s)",tvehicles.components.value,tvehicles.components.names)
        if imgui.ColorPicker3("Color", tvehicles.color.rgb) then
            module.for_each_car_component(function(mat,comp,car)
                local r, g, b, old_a = mat:get_color()
                fixCar(car)
                if tvehicles.components.value.v == 0 and (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF) then
                    mat:set_color(tvehicles.color.rgb.v[1]*255, tvehicles.color.rgb.v[2]*255, tvehicles.color.rgb.v[3]*255, 255.0)
                end
                if comp.name == tvehicles.components.names[tvehicles.components.value.v+1] then
                    mat:set_color(tvehicles.color.rgb.v[1]*255, tvehicles.color.rgb.v[2]*255, tvehicles.color.rgb.v[3]*255, 255.0)
                end
                tvehicles.color.default = getCarColours(car)
            end)
        end
        imgui.EndMenu()
    end

    if imgui.BeginMenu("Modifications") then
        imgui.Spacing()
        imgui.Text("Vehicle Modifications")
        imgui.Separator()
        imgui.Spacing()
        if isCharInAnyCar(PLAYER_PED) then
            if imgui.Button("Restore vehicle mods",imgui.ImVec2(150,25)) then
                if isCharInAnyCar(PLAYER_PED) then 
                    callFunction(0x49B3C0,0,0)
                end
            end
            fcommon.information_tooltip("Restores vehicle mods from memory.")
            imgui.SameLine()
            if imgui.Button("Save vehicle mods",imgui.ImVec2(150,25)) then
                if isCharInAnyCar(PLAYER_PED) then
                    callFunction(0x49B280,0,0)
                end
            end
            fcommon.information_tooltip("Saves vehicle mods in memory.")
        
            fcommon.show_entries("Bullbar",{1100,1109,1110,1115,1116,1123,1125},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Exhaust",{1018,1019,1020,1021,1022,1028,1029,1034,1037,1043,1044,1045,1046,1059,1064,1065,1066,1089,1092,1104,1105,1113,1114,1126,1127,1129,1132,1135,1136},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Front Bumper",{1117,1152,1153,1155,1157,1160,1165,1166,1169,1170,1171,1172,1173,1174,1175,1179,1181,1182,1185,1188,1189,1190,1191},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Front sign",{1111,1112},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Hood",{1004,1005,1011,1012},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Lamps",{1013,1024},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Misc",{1086,1087},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Nitros",{1008,1009,1010},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Rear Bumper",{1140,1141,1148,1149,1150,1151,1154,1156,1159,1161,1167,1168,1176,1177,1178,1180,1183,1184,1186,1187,1192,1193},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Roof",{1006,1032,1033,1035,1038,1053,1054,1055,1061,1067,1068,1088,1091,1103,1028,1130,1131},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Sideskirt",{1007,1017,1026,1027,1030,1031,1036,1039,1040,1041,1042,1047,1048,1051,1052,1056,1057,1062,1063,1069,1070,1071,1072,1090,1093,1094,1095,1099,1101,1102,1106,1107,1108,1118,1119,1120,1121,1122,1124,1133,1134,1137},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Spoiler",{1000,1001,1002,1003,1014,1015,1016,1023,1049,1050,1058,1060,1138,1139,1146,1147,1158,1162,1163,1164},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Vents",{1142,1143,1144,1145},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,false)
            fcommon.show_entries("Wheels",{1025,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1096,1097,1098},3,tvehicles.components.images,tvehicles.components.path,".jpg",tvehicles.image_size, fvehicles.add_component_to_vehicle,nil,true)
        else
            imgui.Text("Player not in a vehicle.")
        end
        imgui.EndMenu()
    end

    if imgui.BeginMenu("Miscellaneous") then
        imgui.Spacing()
        imgui.Text("Miscellaneous")
        imgui.Separator()
        imgui.Spacing()
        fcommon.radio_menu("Traffic Color",{"Pink Traffic","Black Traffic"},{0x969150,0x969151},"horizantal")
        fcommon.radio_menu("Traffic Type",{"Cheap Traffic","Fast Traffic","Country Traffic"},{0x96915E,0x96915F,0x96917B},"horizantal")
        imgui.Spacing()
        imgui.Text("More")
        imgui.Separator()
        imgui.Spacing()
        imgui.Columns(2)
        
        fcommon.ValueSwitch({ name = "Car heavy",var = tvehicles.heavy,show_help_popups = true})
        fcommon.ValueSwitch({ name = "Show health",var = tvehicles.show.health,show_help_popups = true})
        fcommon.ValueSwitch({ name = "Stay on Bike",var = tvehicles.stay_on_bike,show_help_popups = true})
        fcommon.information_tooltip("Prevents falling from bikes.")
        
        if isCharInAnyCar(PLAYER_PED) then
            car = getCarCharIsUsing(PLAYER_PED)
            tvehicles.hydraulic.v = doesCarHaveHydraulics(car)
        end
        if imgui.Checkbox("Car hydraulic",tvehicles.hydraulic) then
            if isCharInAnyCar(PLAYER_PED) then
                if fvehicles.tvehicles.hydraulic.v then
                    setCarHydraulics(car,true)
                    fcommon.CheatActivated()
                else
                    setCarHydraulics(car,false)
                    fcommon.CheatDeactivated()
                end
            end
        end
        fcommon.ValueSwitch({ name = "Lock Car health",var = tvehicles.full_health,show_help_popups = true})
        fcommon.ValueSwitch({ name = "No car visual damage",var = tvehicles.visual_damage,show_help_popups = true})
        fcommon.check_box({ address = 0x969164,name = "Tank mode"})
        fcommon.check_box({ address = 0x96914B,name = "Wheels only",help_text = "Only can wheels are shown."})
        fcommon.check_box({ address = 0x96914E,name = "All lights green",help_text = "All traffic lights are always green."})

        imgui.NextColumn()

        fcommon.check_box({ address = 0x969160,name = "Cars can fly"})
        fcommon.check_box({ address = 0x969153,name = "Boats can fly"})
        fcommon.check_box({ address = 0x96914C,name = "Perfect handling"})
        fcommon.check_box({ address = 0x96917A,name = "Decreased Traffic"})
        fcommon.check_box({ address = 0x96914F,name = "Aggressive drivers"})
        fcommon.check_box({ address = 0x969165,name = "All cars have nitro"})
        fcommon.check_box({ address = 0x969152,name = "Cars can drive on water"})
        fcommon.check_box({ address = 0x969166,name = "Cars float away when hit"})

        if imgui.Checkbox("Vehicle lights on",tvehicles.lights.all) then
            if isCharInAnyCar(PLAYER_PED) then
                car = storeCarCharIsInNoSave(PLAYER_PED)
                if fvehicles.tvehicles.lights.all.v == true then
                    forceCarLights(car,2)
                    addOneOffSound(x,y,z,1052)
                else
                    forceCarLights(car,1)
                    addOneOffSound(x,y,z,1053)
                end
            end
        end 
        imgui.EndMenu()
    end

    imgui.Spacing()
    imgui.Text("Spawn Options")
    imgui.Separator()
    imgui.Spacing()
    
    fcommon.ValueSwitch({ name = "Quick vehicle",var = tvehicles.quick_spawn,help_text = "Vehicle can be spawned from quick spawner using (Left Ctrl + Q). \n\nControls:\nEnter = Stop reading key press\nDelete = Erase full string\nBackspace = Erase last character"}) 
    fcommon.ValueSwitch({ name = "Spawn planes in air",var = tvehicles.spawn_plane_in_air})
    
    imgui.Spacing()
    imgui.Text("Vehicles list")
    imgui.Separator()
    imgui.Spacing()
    fcommon.show_entries("Airplanes",{592,577,511,512,593,520,553,476,519,460,513},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Helicopters",{548,425,417,487,488,497,563,447,469},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Boats",{472,473,493,595,484,430,453,452,446,454},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Bikes",{581,509,481,462,521,463,510,522,461,448,468,586},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("2-Door & Compact cars",{602,496,401,518,527,589,419,587,533,526,474,545,517,410,600,436,439,549,491},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("4-Door & Luxury cars",{445,604,507,585,466,492,546,551,516,467,426,547,405,580,409,550,566,540,421,529},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Civil Service",{485,431,438,437,574,420,525,408,552},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Government Vehicles",{416,433,427,490,528,407,544,523,470,596,598,599,597,432,601,428},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Heavy & Utility trucks",{499,609,498,524,532,578,486,406,573,455,403,423,414,443,515,514,531,456,588},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Light trucks & Vans",{459,422,482,605,530,418,572,582,413,440,543,583,478,554},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("SUVs & Wagons",{579,400,404,489,505,479,442,458},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Lowriders",{536,575,534,567,535,576,412},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Muscle Cars",{402,542,603,475},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Street Racers",{429,542,415,480,562,565,434,494,502,503,411,559,561,560,506,451,558,555,477},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("RC Vehicles",{441,464,594,501,465,564},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
    fcommon.show_entries("Recreational",{568,424,504,457,483,508,571,500,444,556,557,471,495,539},3,tvehicles.images,tvehicles.path,".jpg",tvehicles.image_size, fvehicles.give_vehicle_to_player,getNameOfVehicleModel,true)
end

return module