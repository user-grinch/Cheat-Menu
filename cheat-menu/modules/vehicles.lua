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
        value = imgui.ImInt(0),
        list = 
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
    list = {},
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


vehicle_entry = function(model)

    if tvehicles.list[tostring(model)] == nil then
        lua_thread.create(fcommon.load_texture,tvehicles.list,tvehicles.path,model,".jpg")
        while tvehicles.list[tostring(model)] == nil do
            wait(0)
        end
    end

    if imgui.ImageButton(tvehicles.list[tostring(model)],imgui.ImVec2(tvehicles.image_size.x,tvehicles.image_size.y)) then 
        module.give_vehicle_to_player(model)
    end
    if imgui.IsItemHovered() then
        imgui.BeginTooltip() 
        imgui.SetTooltip(getNameOfVehicleModel(model))
        imgui.EndTooltip()
    end
end

function module.set_car_color(func)
    if isCharInAnyCar(PLAYER_PED) then
        car = storeCarCharIsInNoSave(PLAYER_PED)
        for _, comp in ipairs(mad.get_all_vehicle_components(car)) do
            for _, obj in ipairs(comp:get_objects()) do
                for _, mat in ipairs(obj:get_materials()) do
                    func(mat,comp)
                    tvehicles.color.default = getCarColours(car)
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

    if imgui.BeginMenu("Vehicle Speed") then
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

    if imgui.BeginMenu("Vehicle Colors") then
        imgui.Spacing()
        imgui.Text("Vehicle Colors")
        fcommon.information_tooltip("Sets vehicle color")
        imgui.Separator()
        imgui.Spacing()
        if imgui.Button("Reset car color",imgui.ImVec2(100.0,25.0)) then
            module.set_car_color(function(mat)
                mat:reset_color()
            end)
        end
        imgui.Combo("Component(s)",tvehicles.components.value,tvehicles.components.list)
        if imgui.ColorPicker3("Color", tvehicles.color.rgb) then
            module.set_car_color(function(mat,comp)
                local r, g, b, old_a = mat:get_color()
                fixCar(car)
                if tvehicles.components.value.v == 0 and (r == 0x3C and g == 0xFF and b == 0x00) or (r == 0xFF and g == 0x00 and b == 0xAF) then
                    mat:set_color(tvehicles.color.rgb.v[1]*255, tvehicles.color.rgb.v[2]*255, tvehicles.color.rgb.v[3]*255, 255.0)
                end
                if comp.name == tvehicles.components.list[tvehicles.components.value.v+1] then
                    mat:set_color(tvehicles.color.rgb.v[1]*255, tvehicles.color.rgb.v[2]*255, tvehicles.color.rgb.v[3]*255, 255.0)
                end
            end)
        end
        imgui.EndMenu()
    end


 --[[
     Click to open Unfinished state

    if not ( not  imgui.IsRootWindowFocused() and imgui.IsAnyWindowHovered()) and not imgui.IsItemHovered() then
        test = false
    end
    
    if imgui.IsItemClicked() then
        test = true
    end
    ]]--


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
    fcommon.entries("Airplanes",vehicle_entry,{592,577,511,512,593,520,553,476,519,460,513})
    fcommon.entries("Helicopters",vehicle_entry,{548,425,417,487,488,497,563,447,469})
    fcommon.entries("Boats",vehicle_entry,{472,473,493,595,484,430,453,452,446,454})
    fcommon.entries("Bikes",vehicle_entry,{581,509,481,462,521,463,510,522,461,448,468,586})
    fcommon.entries("2-Door & Compact cars",vehicle_entry,{602,496,401,518,527,589,419,587,533,526,474,545,517,410,600,436,439,549,491})
    fcommon.entries("4-Door & Luxury cars",vehicle_entry,{445,604,507,585,466,492,546,551,516,467,426,547,405,580,409,550,566,540,421,529})
    fcommon.entries("Civil Service",vehicle_entry,{485,431,438,437,574,420,525,408,552})
    fcommon.entries("Government Vehicles",vehicle_entry,{416,433,427,490,528,407,544,523,470,596,598,599,597,432,601,428})
    fcommon.entries("Heavy & Utility trucks",vehicle_entry,{499,609,498,524,532,578,486,406,573,455,403,423,414,443,515,514,531,456,588})
    fcommon.entries("Light trucks & Vans",vehicle_entry,{459,422,482,605,530,418,572,582,413,440,543,583,478,554})
    fcommon.entries("SUVs & Wagons",vehicle_entry,{579,400,404,489,505,479,442,458})
    fcommon.entries("Lowriders",vehicle_entry,{536,575,534,567,535,576,412})
    fcommon.entries("Muscle Cars",vehicle_entry,{402,542,603,475})
    fcommon.entries("Street Racers",vehicle_entry,{429,542,415,480,562,565,434,494,502,503,411,559,561,560,506,451,558,555,477})
    fcommon.entries("RC Vehicles",vehicle_entry,{441,464,594,501,465,564})
    fcommon.entries("Recreational",vehicle_entry,{568,424,504,457,483,508,571,500,444,556,557,471,495,539})
end

return module