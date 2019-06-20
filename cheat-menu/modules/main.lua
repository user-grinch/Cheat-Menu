-- This module contains few functions parts which requires looping every frame

local module = {}

function module.main_section()
    -- Screenshot
    if fgame.ss_shotcut.v == true 
    and isKeyDown(keys.control_key) and isKeyDown(keys.screenshot_key) then 
        takePhoto(true)
        printHelpString("Screenshot taken ~g~successfully")
        fcommon.keywait(keys.control_key,keys.screenshot_key)
    end


    -- Quick Spawner
    if isKeyDown(keys.control_key) and isKeyDown(keys.quickspawner_key) then
        if fvehicles.tvehicles.quick_spawn.v or fweapons.tweapons.quick_spawn.v then 
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

    if fplayer.player_proofs.v then
        setCharProofs(PLAYER_PED,true,true,true,true,true)
    else
        setCharProofs(PLAYER_PED,false,false,false,false,false)
    end

    -- Aim Skin Changer
    if fplayer.aim_skin_changer.v and isKeyDown(keys.akc_return_key) then
        fcommon.keywait(keys.akc_return_key,keys.akc_return_key)
        local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
        if bool == true then
            local model = getCharModel(char)
            change_player_model(model)
        end
    end


    -- Teleportation
    if fteleport.tteleport.shotcut.v == true
    and isKeyDown(keys.teleport_key1) 
    and isKeyDown(keys.teleport_key2) then
        fteleport.teleport()
    end
   
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