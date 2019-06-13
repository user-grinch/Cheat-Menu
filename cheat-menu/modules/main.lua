-- This module contains few functions parts which requires looping every frame

local module = {}
function changePlayerSpecialModel(player,str,id)
	if id==nil then id = 1 end
	local model = 289 + id
	if hasSpecialCharacterLoaded(model) then 
        unloadSpecialCharacter(model)
    end	
    loadSpecialCharacter(str,1)
    loadAllModelsNow()
	setPlayerModel(player,model)
end 


function module.main_section()

    if isKeyDown(0x31) then
       --Test
        local loc = getGameDirectory() .. '/moonloader/cheat-menu/cheat-menu.ini'
        printString(loc,1000)
        test =
        {
            a = 10, 
            c = "a",
            b = "c",
        }
        
        if ini.save({
            a = 10, 
            c = "a",
            b = "c",
        }, loc) then
            printString("success!",1000)
        end
    end

    -- Screenshot
    if fgame.ss_shotcut.v == true 
    and isKeyDown(config.config.keys.control_key) and isKeyDown(config.config.keys.screenshot_key) then 
        takePhoto(true)
        printHelpString("Screenshot taken ~g~successfully")
        fcommon.keywait(config.config.keys.control_key,config.config.keys.screenshot_key)
    end


    -- Quick Spawner
    if isKeyDown(config.keys.control_key) and isKeyDown(config.keys.quickspawner_key) then
        if fvehicles.tvehicles.quick_spawn.v or fweapons.tweapons.quick_spawn.v then 
            fcommon.quick_spawner()
        end
    end
        
    -- Airbreak feature
    fgame.airbreak_mode()

    -- Draw coordinates on screen
    if fvisuals.show_coordinates.v == true then
        x,y,z = getCharCoordinates(PLAYER_PED)
        printString(string.format("Coordinates: %d %d %d", math.floor(x) , math.floor(y) , math.floor(z)),1000)
        if isKeyDown(config.keys.control_key)
        and isKeyDown(config.keys.coords_copy) then
            fcommon.keywait(config.keys.control_key,config.keys.coords_copy)
            setClipboardText(string.format("%d %d %d", x,y,z))
            printHelpString("Coordinates saved to clipboard")
        end
    end

    if fplayer.player_proofs.v then
        setCharProofs(PLAYER_PED,true,true,true,true,true)
    else
        setCharProofs(PLAYER_PED,false,false,false,false,false)
    end

    -- Aim Skin Changer
    if fplayer.aim_skin_changer.v and isKeyDown(config.keys.akc_return_key) then
        local bool,char = getCharPlayerIsTargeting(PLAYER_HANDLE)
        if bool == true then
            local model = getCharModel(char)
            local player_model = getCharModel(PLAYER_PED)
            if model ~= player_model then 
                while not isModelAvailable(model) do
                    requestModel(model)
                    loadAllModelsNow()
                    wait(0)
                end
                setPlayerModel(PLAYER_HANDLE,model)
                clearCharTasks(PLAYER_PED)
            end
        end
    end


    -- Teleportation
    if fteleport.tteleport.shotcut.v == true
    and isKeyDown(config.keys.teleport_key1) 
    and isKeyDown(config.keys.teleport_key2) then
        fteleport.teleport()
    end

    -- Draw FPS
    if fgame.tfps.bool.v == true then
        local framerate = math.floor(imgui.GetIO().Framerate)
        mad.draw_text("FPS " .. framerate,fvisuals.tdisplay.text.X.v,fvisuals.tdisplay.text.Y.v-60)
    end
   
    -- Vehicle related stuff which is required to run every frame
    if isCharInAnyCar(PLAYER_PED) then
        car = getCarCharIsUsing(PLAYER_PED)

        if fvehicles.tvehicles.show.gears.v == true then
            speed = getCarSpeed(car)
            total_gears = getCarNumberOfGears(car)
            current_gear = getCarCurrentGear(car)
            mad.draw_text("Gear " .. current_gear.. "/" .. total_gears,fvisuals.tdisplay.text.X.v,fvisuals.tdisplay.text.Y.v-40)
        end

        if fvehicles.tvehicles.show.speed.v == true then
            speed = getCarSpeed(car)
            mad.draw_text("Speed " .. math.floor(speed),fvisuals.tdisplay.text.X.v,fvisuals.tdisplay.text.Y.v-20)
        end

        if fvehicles.tvehicles.show.health.v == true then
            health = getCarHealth(car)
            mad.draw_text("Health " .. math.floor(health/10),fvisuals.tdisplay.text.X.v,fvisuals.tdisplay.text.Y.v)
        end

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

        if fvehicles.tvehicles.hydraulic.v then
            setCarHydraulics(car,true)
        else
            setCarHydraulics(car,false)
        end

        if fvehicles.tvehicles.visual_damage.v then
            setCarCanBeVisiblyDamaged(car,false)
        else
            setCarCanBeVisiblyDamaged(car,true)
        end
    end

end
return module