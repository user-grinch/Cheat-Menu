-- This module contains few functions parts which requires looping every frame

local module = {}

function module.()
    -- Screenshot
    if fgame.ss_shotcut.v == true 
    and isKeyDown(keys.control_key) and isKeyDown(keys.screenshot_key) then 
        takePhoto(true)
        printHelpString("Screenshot taken ~g~successfully")
        fcommon.keywait(keys.control_key,keys.screenshot_key)
    end


    -- Quick Spawner
    if isKeyDown(keys.control_key) and isKeyDown(keys.quickspawner_key) then
        if (fvehicles.tvehicles.quick_spawn.v or fweapons.tweapons.quick_spawn.v) then 
            fcommon.quick_spawner()
        end
    end
        
    -- This is to fix car colors not being applied using opcode changecarcolours
    if fvehicles.tvehicles.color.default ~= -1 then
        if isCharInAnyCar(PLAYER_PED) then
            color_id = getCarColours(car)
            if fvehicles.tvehicles.color.default ~= color_id then
                fvehicles.set_car_color(function(mat)
                    mat:reset_color()
                end)
            end
        end
    end

    -- Airbreak feature
    fgame.airbreak_mode()



    
   
    -- Vehicle related stuff which is required to run every frame
    if isCharInAnyCar(PLAYER_PED) then
        car = getCarCharIsUsing(PLAYER_PED)

        if fvehicles.tvehicles.full_health.v then
            setCarHealth(car,1000)
        end

        if fvehicles.tvehicles.stay_on_bike.v then
            setCharCanBeKnockedOffBike(PLAYER_PED,true)
        else
            setCharCanBeKnockedOffBike(PLAYER_PED,false)
        end
        
        if fvehicles.tvehicles.lock_speed.v then
            if fvehicles.tvehicles.speed.v > 500 then
                fvehicles.tvehicles.speed.v = 500 
            end
            setCarForwardSpeed(car,fvehicles.tvehicles.speed.v)
        end
        
        if fvehicles.tvehicles.heavy.v then
            setCarHeavy(car,true)
        else
            setCarHeavy(car,false)
        end

        if fvehicles.tvehicles.visual_damage.v then
            setCarCanBeVisiblyDamaged(car,false)
        else
            setCarCanBeVisiblyDamaged(car,true)
        end
    end
end
return module