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

module.tgame                =
{
    camera                  = 
    {
        bool                = imgui.new.bool(false),
        fov                 = imgui.new.int(fconfig.Get('tgame.camera.fov',70)),
        lock_on_player      = imgui.new.bool(false),
        movement_speed      = imgui.new.float(fconfig.Get('tgame.camera.movement_speed',0.2)),
        shake               = imgui.new.float(0.0),
    },
    cop = 
    {
        ["0x8a5a8c"] = 599, -- policeranger
        ["0x8a5a90"] = 596, -- policels
        ["0x8a5a94"] = 597, -- policesf
        ["0x8a5a98"] = 598, -- policelv
        ["0x8a8a9c"] = 523, -- policebike
    },
    day                     =
    {    
        names               = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
        array               = {},
    },    
    disable_cheats          = imgui.new.bool(fconfig.Get('tgame.disable_cheats',false)),
    disable_help_popups     = imgui.new.bool(fconfig.Get('tgame.disable_help_popups',false)),
    disable_replay          = imgui.new.bool(fconfig.Get('tgame.disable_replay',false)),
    fps_limit               = imgui.new.int(fconfig.Get('tgame.fps_limit',30)),
    freeze_mission_timer    = imgui.new.bool(fconfig.Get('tgame.freeze_mission_timer',false)), 
    freeze_time             = imgui.new.bool(fconfig.Get('tgame.freeze_time',false)), 
    ghost_cop_cars          = imgui.new.bool(fconfig.Get('tgame.ghost_cop_cars',false)),
    keep_stuff              = imgui.new.bool(fconfig.Get('tgame.keep_stuff',false)),
    random_cheats           = 
    {
        activated_cheats       = {},
        cheat_activate_timer   = imgui.new.int(fconfig.Get('tgame.random_cheats.cheat_activate_timer',10)),
        cheat_deactivate_timer = imgui.new.int(fconfig.Get('tgame.random_cheats.cheat_deactivate_timer',10)),
        cheat_name             = fcommon.LoadJson("cheat name"),
        checkbox               = imgui.new.bool(fconfig.Get('tgame.random_cheats.checkbox',false)),
        disable_cheat_checkbox = imgui.new.bool(fconfig.Get('tgame.random_cheats.disable_cheat_checkbox',false)),
        disabled_cheats        = fconfig.Get('tgame.random_cheats.disabled_cheats',{}),
    },
    script_manager          =
    {
        scripts             = fconfig.Get('tgame.script_manager.scripts',{}),
        skip_auto_reload    = false,
        not_loaded          = {},
    },
    solid_water             = imgui.new.bool(fconfig.Get('tgame.solid_water',false)),
    solid_water_object      = nil,
    ss_shortcut             = imgui.new.bool(fconfig.Get('tgame.ss_shortcut',false)), 
    sync_system_time        = imgui.new.bool(fconfig.Get('tgame.sync_system_time',false)), 
}

module.tgame.day.array      = imgui.new['const char*'][#module.tgame.day.names](module.tgame.day.names)


function module.SolidWater()
   
    while true do
        if module.tgame.solid_water[0] then
            while module.tgame.solid_water[0] do
                wait(0)
                local x,y,z = getCharCoordinates(PLAYER_PED)
                local water_height =  getWaterHeightAtCoords(x,y,false)

                if doesObjectExist(module.tgame.solid_water_object) then
                    deleteObject(module.tgame.solid_water_object)
                end

                if z > water_height and water_height ~= -1000 and not isCharInAnyBoat(PLAYER_PED) then     -- Don't create the object if player is under water/diving
                    module.tgame.solid_water_object = createObject(3095,x,y,water_height)
                    setObjectVisible(module.tgame.solid_water_object,false)
                end
            end

            if doesObjectExist(module.tgame.solid_water_object) then
                deleteObject(module.tgame.solid_water_object)
            end
            
        end
        wait(0)
    end
end

function module.CameraMode()

    while true do
        if module.tgame.camera.bool[0] then

            local x,y,z = getCharCoordinates(PLAYER_PED)

            local ped =  createRandomChar(x,y,z)

            freezeCharPositionAndDontLoadCollision(ped,true)
            setCharCollision(ped,false)
            setLoadCollisionForCharFlag(ped,false)
            setEveryoneIgnorePlayer(0,true)
            
            displayRadar(false)
            displayHud(false)
            setCharVisible(ped,false)

            local total_mouse_x = getCharHeading(PLAYER_PED)
            local total_mouse_y = 0
            local total_mouse_delta = 0
            

            setCharCoordinates(ped,x,y,z-20) 

            cameraSetLerpFov(getCameraFov(),module.tgame.camera.fov[0],1000,true)
            cameraPersistFov(true) 

            while module.tgame.camera.bool[0] do
                local factor = 1.0
                x, y, z = getCharCoordinates(ped)  
                local mouse_x, mouse_y =  getPcMouseMovement()

                total_mouse_x = total_mouse_x - mouse_x/6
                total_mouse_y = total_mouse_y + mouse_y/3
                
                if total_mouse_y > 170 then total_mouse_y = 170 end
                if total_mouse_y < -170 then total_mouse_y = -170 end
                

                if isKeyDown(tcheatmenu.hot_keys.camera_mode_slow[1] and tcheatmenu.hot_keys.camera_mode_slow[2]) then 
                    factor = factor*0.5
                end
                if isKeyDown(tcheatmenu.hot_keys.camera_mode_fast[1] and tcheatmenu.hot_keys.camera_mode_fast[2]) then 
                    factor = factor*2
                end


                if isKeyDown(tcheatmenu.hot_keys.camera_mode_forward[1] and tcheatmenu.hot_keys.camera_mode_forward[2]) then 
                    local angle = getCharHeading(ped) + 90

                    x = x + module.tgame.camera.movement_speed[0] * math.cos(angle * math.pi/180) * factor
                    y = y + module.tgame.camera.movement_speed[0] * math.sin(angle * math.pi/180) * factor
                    z = z + module.tgame.camera.movement_speed[0] * math.sin(total_mouse_y* math.pi/180) * factor
                end
        
                if isKeyDown(tcheatmenu.hot_keys.camera_mode_backward[1] and tcheatmenu.hot_keys.camera_mode_backward[2]) then 
                    local angle = getCharHeading(ped) + 90
                    
                    x = x - module.tgame.camera.movement_speed[0] * math.cos(angle * math.pi/180) * factor
                    y = y - module.tgame.camera.movement_speed[0] * math.sin(angle * math.pi/180) * factor
                    z = z - module.tgame.camera.movement_speed[0] * math.sin(total_mouse_y* math.pi/180) * factor
                end

                if isKeyDown(tcheatmenu.hot_keys.camera_mode_left[1] and tcheatmenu.hot_keys.camera_mode_left[2]) then 
                    local angle = getCharHeading(ped)
                    
                    x = x - module.tgame.camera.movement_speed[0] * math.cos(angle * math.pi/180) * factor
                    y = y - module.tgame.camera.movement_speed[0] * math.sin(angle * math.pi/180) * factor
                end

                if isKeyDown(tcheatmenu.hot_keys.camera_mode_right[1] and tcheatmenu.hot_keys.camera_mode_right[2]) then 
                    local angle = getCharHeading(ped)
                    
                    x = x + module.tgame.camera.movement_speed[0] * math.cos(angle * math.pi/180) * factor
                    y = y + module.tgame.camera.movement_speed[0] * math.sin(angle * math.pi/180) * factor
                end

                if module.tgame.camera.lock_on_player[0] then

                    local right = 0
                    local front = 0
                    local up = 0
                    
                    while module.tgame.camera.lock_on_player[0] and module.tgame.camera.bool[0] do
                        local mouse_x, mouse_y =  getPcMouseMovement()

                        total_mouse_x = total_mouse_x - mouse_x/6
                        total_mouse_y = total_mouse_y + mouse_y/6
                        if total_mouse_y > 170 then total_mouse_y = 170 end
                        if total_mouse_y < -170 then total_mouse_y = -170 end
                        factor = 1

                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_slow[1] and tcheatmenu.hot_keys.camera_mode_slow[2]) then 
                            factor = factor*0.5
                        end
                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_fast[1] and tcheatmenu.hot_keys.camera_mode_fast[2]) then 
                            factor = factor*2
                        end

                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_forward[1] and tcheatmenu.hot_keys.camera_mode_forward[2]) then 
                            front = front + 0.1 * factor
                        end
        
                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_backward[1] and tcheatmenu.hot_keys.camera_mode_backward[2]) then 
                            front = front - 0.1* factor
                        end

                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_left[1] and tcheatmenu.hot_keys.camera_mode_left[2]) then 
                            right = right - 0.1* factor
                        end
        
                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_right[1] and tcheatmenu.hot_keys.camera_mode_right[2]) then 
                            right = right + 0.1* factor
                        end

                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_up[1] and tcheatmenu.hot_keys.camera_mode_up[2]) then 
                            up = up - 0.1* factor
                        end
        
                        if isKeyDown(tcheatmenu.hot_keys.camera_mode_down[1] and tcheatmenu.hot_keys.camera_mode_down[2]) then 
                            up = up + 0.1* factor
                        end

                        attachCameraToChar(PLAYER_PED,right, front, up, 0.0, 180.0, total_mouse_y, 0.0, 2)

                        if total_mouse_delta + getMousewheelDelta() ~= total_mouse_delta then
                            total_mouse_delta = total_mouse_delta + getMousewheelDelta()
                            module.tgame.camera.fov[0] = module.tgame.camera.fov[0] - getMousewheelDelta()
                            if module.tgame.camera.fov[0] > 120 then
                                module.tgame.camera.fov[0] = 120
                            end
                            if module.tgame.camera.fov[0] < 5 then
                                module.tgame.camera.fov[0] = 5
                            end
                            cameraSetLerpFov(getCameraFov(),module.tgame.camera.fov[0],100,true)
                            cameraPersistFov(true) 
                        end
                        
                        wait(0)
                    end
                else
                    setCharHeading(ped,total_mouse_x)
                    attachCameraToChar(ped,0.0, 0.0, 20.0, 0.0, 180, total_mouse_y, 0.0, 2)
                    setCharCoordinates(ped,x,y,z-1.0)
                end

                if total_mouse_delta + getMousewheelDelta() ~= total_mouse_delta then
                    total_mouse_delta = total_mouse_delta + getMousewheelDelta()
                    module.tgame.camera.fov[0] = module.tgame.camera.fov[0] - getMousewheelDelta()
                    if module.tgame.camera.fov[0] > 120 then
                        module.tgame.camera.fov[0] = 120
                    end
                    if module.tgame.camera.fov[0] < 5 then
                        module.tgame.camera.fov[0] = 5
                    end
                    cameraSetLerpFov(getCameraFov(),module.tgame.camera.fov[0],100,true)
                    cameraPersistFov(true) 
                end
                wait(0)
            end
        
            cameraPersistFov(false) 

            displayRadar(true)
            displayHud(true)

            restoreCameraJumpcut()
            markCharAsNoLongerNeeded(ped)
            deleteChar(ped)
        end
        wait(0)
    end
end

function CheatsEntry(func,status,names)
    local sizeX = fcommon.GetSize(3)
    local sizeY = imgui.GetWindowHeight()/10

    fcommon.DropDownMenu(names[1],function()
        imgui.Spacing()

        for i = 1, #func do

            if imgui.Button(names[i+1],imgui.ImVec2(sizeX,sizeY)) then
                callFunction(func[i],1,1)
                if status == nil or status[i] == nil then
                    fcommon.CheatActivated()
                else
                    if readMemory(status[i],1,false) == 0 then
                        fcommon.CheatDeactivated()
                    else
                        fcommon.CheatActivated()
                    end
                end
            end
            if i % 3 ~= 0 then
                imgui.SameLine()
            end
        end
    end)
end

function module.SyncSystemTime()
    while true do
        if module.tgame.sync_system_time[0] then
            time = os.date("*t")
            local hour,minute = getTimeOfDay()
            if hour ~= time.hour or minute ~= time.min then
                setTimeOfDay(time.hour,time.min)
            end
        end
        wait(0)
    end
end

function module.RandomCheatsActivate()
    while true do
        if module.tgame.random_cheats.checkbox[0] then
            wait(module.tgame.random_cheats.cheat_activate_timer[0]*1000)
            if module.tgame.random_cheats.checkbox[0] then
                cheatid = math.random(0,91)
                callFunction(0x438370,1,1,cheatid)
                table.insert(module.tgame.random_cheats.activated_cheats,cheatid)
                printHelpString("~g~" .. module.tgame.random_cheats.cheat_name[tostring(cheatid)][1])
            end
        end
        wait(0)
    end
end

function module.RandomCheatsDeactivate()
    while true do
        if module.tgame.random_cheats.disable_cheat_checkbox[0] and module.tgame.random_cheats.activated_cheats then
            for _,x in ipairs(module.tgame.random_cheats.activated_cheats) do
                if module.tgame.random_cheats.cheat_name[tostring(x)][2] == "true" then
                    wait(module.tgame.random_cheats.cheat_deactivate_timer[0]*1000)
                    if module.tgame.random_cheats.disable_cheat_checkbox[0] then
                        callFunction(0x438370,1,1,module.tgame.random_cheats.activated_cheats[x])
                        printHelpString("~r~" .. module.tgame.random_cheats.cheat_name[tostring(x)][1])
                    end
                end
            end
        end
        wait(0)
    end
end

function module.FreezeTime()
    while true do
        if module.tgame.freeze_time[0] then

            local status = fvisual.tvisual.lock_weather[0]
            memory.write(0x969168,1,1)  -- Freeze time
            while module.tgame.freeze_time[0] do
                fvisual.tvisual.lock_weather[0] = true
                wait(0)
            end
            fvisual.tvisual.lock_weather[0] = status
            memory.write(0x969168,0,1)  -- Freeze time
        end
        wait(0)
    end
end

function SetTime()
    fcommon.DropDownMenu("Set time",function()
        imgui.Spacing()

        local days_passed = imgui.new.int(memory.read(0xB79038 ,4))
        local hour = imgui.new.int(memory.read(0xB70153,1))
        local minute = imgui.new.int(memory.read(0xB70152,1))

        if imgui.InputInt("Current hour",hour) then
            memory.write(0xB70153 ,hour[0],1)
        end

        if imgui.InputInt("Current minute",minute) then
            memory.write(0xB70152 ,minute[0],1)
        end

        if minute[0] > 59 then
            hour[0] = hour[0] + 1
            minute[0] = 0
        end

        if hour[0] > 23 then
            memory.write(0xB70153 ,0,1)
            current_day = memory.read(0xB7014E,1)
            memory.write(0xB7014E,(current_day+1),1)
            days_passed = memory.read( 0xB79038,4)
            memory.write( 0xB79038,(days_passed+1),4)
        end

        if minute[0] < 0 then
            hour[0] = hour[0] - 1
            minute[0] = 59
        end

        if hour[0] < 0 then
            memory.write(0xB70153 ,23,1)
            current_day = memory.read(0xB7014E,1)
            memory.write(0xB7014E,(current_day-1),1)
            days_passed = memory.read( 0xB79038,4)
            memory.write( 0xB79038,(days_passed-1),4)
        end
    end)
end

--------------------------------------------------
-- Functions of script manager

function module.LoadScriptsOnKeyPress()
    while true do
        for name,table in pairs(module.tgame.script_manager.scripts) do
            fcommon.OnHotKeyPress(table,function()
                local full_file_path = string.format( "%s\\%s.loadonkeypress",getWorkingDirectory(),name)
                local is_loaded = false
                for index, script in ipairs(script.list()) do
                    if full_file_path == script.path then
                        is_loaded = true
                    end
                end
                if is_loaded == false then
                    script.load(full_file_path)
                    printHelpString("Script loaded")
                else
                    printHelpString("Script already loaded")
                end 
                module.tgame.script_manager.not_loaded[name .. ".loadonkeypress"] = nil
            end)
        end
        wait(0)
    end
end

function module.MonitorScripts()
    local mainDir  = getWorkingDirectory()
    for file in lfs.dir(mainDir) do
        local full_file_path = mainDir .. "\\" .. file
        if doesFileExist(full_file_path) then

            local file_path,file_name,file_ext = string.match(full_file_path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 

            if (file_ext == "lua" or file_ext == "neverload" or file_ext == "loadonkeypress") and module.tgame.script_manager.not_loaded[file_name] == nil  then
                local is_loaded = false
                for index, script in ipairs(script.list()) do
                    if full_file_path == script.path then
                        is_loaded = true
                    end
                end
                if is_loaded == false then
                    module.tgame.script_manager.not_loaded[file_name] = full_file_path
                end 
            end
        end
    end
end

function ShowNotLoadedScripts(name,path)

    local _,file_name,file_ext = string.match(path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 

    fcommon.DropDownMenu(name .. "##" .. path,function()

        imgui.Spacing()
        imgui.SameLine()

        if file_ext ==  "lua" then
            imgui.Text("Status: Not loaded")
        end
        if file_ext ==  "neverload" then
            imgui.Text("Status: Never load")
        end
        if file_ext ==  "loadonkeypress" then
            imgui.Text("Status: Load on key press")
        end
        
        imgui.Spacing()
        imgui.SameLine()
        imgui.TextWrapped("Filepath: " .. path)
        
        if imgui.Button("Load##" .. path,imgui.ImVec2(fcommon.GetSize(1))) then
            if doesFileExist(path) then 

                local load_path = path
                if file_ext ==  "neverload" then
                    load_path = string.sub(path,1,-11)
                    os.rename(path,load_path)
                end
                if file_ext ==  "loadonkeypress" then
                    module.tgame.script_manager.scripts[name:sub(1,-16)] = nil
                    load_path = string.sub(path,1,-16)
                    os.rename(path,load_path)
                end
                module.tgame.script_manager.not_loaded[name] = nil
                script.load(load_path)
                printHelpString("Script loaded")
            end
        end
    end,true)
end

function ShowLoadedScript(script,index)
    fcommon.DropDownMenu(script.name .. "##" .. index,function()
        local _,file_name,file_ext = string.match(script.path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 
        local authors = ""
        for _,author in ipairs(script.authors) do
            authors = authors .. author .. ", "
        end
        local properties = ""
        for _,property in ipairs(script.properties) do
            properties = properties .. property .. ", "
        end
        local dependencies = ""
        for _,dependency in ipairs(script.dependencies) do
            dependencies = dependencies .. dependency .. ", "
        end

        imgui.Columns(2,nil,false)
        imgui.Text("Authors: ")
        imgui.SameLine(0.0,0.0)
        imgui.TextWrapped(string.sub(authors,1,-3))
        imgui.Text("Version: " .. tostring(script.version))
        imgui.Text("Version num: " .. tostring(script.version_num))
        imgui.NextColumn()
        imgui.Text("Script ID: " .. script.id)
        imgui.Text("Status: Loaded")
        imgui.Text("Filename: ")
        imgui.SameLine(0.0,0.0)
        imgui.TextWrapped(script.filename)
        imgui.Columns(1)
        if properties ~= "" then
            imgui.Spacing()
            imgui.SameLine()
            imgui.Text("Properties: ")
            imgui.SameLine(0.0,0.0)
            imgui.TextWrapped(string.sub(properties,1,-3))
        end
        if dependencies ~= "" then
            imgui.Spacing()
            imgui.SameLine()
            imgui.Text("Dependencies: ")
            imgui.SameLine(0.0,0.0)
            imgui.TextWrapped(string.sub(dependencies,1,-3))
        end
        if description ~= "" then
            imgui.Spacing()
            imgui.SameLine()
            imgui.Text("Description: ")
            imgui.SameLine(0.0,0.0)
            imgui.TextWrapped(script.description)
        end
        imgui.Spacing()

        if script.path:match(".loadonkeypress") then
            file_name = file_name:sub(1,-16)
        end

        tcheatmenu.hot_keys.script_manager_temp = module.tgame.script_manager.scripts[file_name] or  tcheatmenu.hot_keys.script_manager_temp

        fcommon.HotKey(tcheatmenu.hot_keys.script_manager_temp,"Load on keypress hotkey")
        imgui.Spacing()
        
        if imgui.Button("Never load##" .. index,imgui.ImVec2(fcommon.GetSize(2))) then
            printHelpString("Script set to never load")
            os.rename(script.path,script.path.. ".neverload")
            script:unload()
        end
        imgui.SameLine()

        if imgui.Button("Load on keypress##" .. index,imgui.ImVec2(fcommon.GetSize(2))) then
            if script.name == thisScript().name then
                printHelpString("Can't set for Cheat Menu")
            else
                module.tgame.script_manager.scripts[file_name] = {tcheatmenu.hot_keys.script_manager_temp[1],tcheatmenu.hot_keys.script_manager_temp[2]}
                printHelpString("Key set for the script.")

                if not script.path:match(".loadonkeypress") then
                    os.rename(script.path,script.path.. ".loadonkeypress")
                end
                script:unload()
            end
        end

        if imgui.Button("Reload##" .. index,imgui.ImVec2(fcommon.GetSize(2))) then
            if script.name == thisScript().name then
                module.tgame.script_manager.skip_auto_reload = true
            end  
            printHelpString("Script reloaded")
            script:reload()
        end
        imgui.SameLine()
        if imgui.Button("Unload##" .. index,imgui.ImVec2(fcommon.GetSize(2))) then
            if script.name == thisScript().name then
                module.tgame.script_manager.skip_auto_reload = true
            end
            printHelpString("Script unloaded")
            script:unload()
        end
    end)
end

function FollowPed(ped)
    local total_mouse_x = 0
    local total_mouse_y = 0
    while true do
        if not doesCharExist(ped) then
            restoreCamera()
            break
        end
        local x,y,z = getCharCoordinates(ped)
        local mouseX,mouseY = getPcMouseMovement()
        total_mouse_x = total_mouse_x + mouseX
        total_mouse_y = total_mouse_y + mouseY
        
        if doesCharExist(ped) then
            attachCameraToChar(ped,0.0,-2,3.0,0.0,total_mouse_x/20,total_mouse_y/20,0.0,2)
        end
        wait(0)
    end
end

--------------------------------------------------


-- Main function
function module.GameMain()
    if imgui.Button("Save game",imgui.ImVec2(fcommon.GetSize(2))) then
        if isCharOnFoot(PLAYER_PED) then
            activateSaveMenu()
        else
            printHelpString("Player is ~r~not~w~ on foot")
        end
    end
    imgui.SameLine()
    if imgui.Button("Copy coordinates",imgui.ImVec2(fcommon.GetSize(2))) then
        local x,y,z = getCharCoordinates(PLAYER_PED)
        setClipboardText(string.format( "%d,%d,%d",x,y,z))
        printHelpString("Coordinates copied")
    end
    
    fcommon.Tabs("Game",{"Checkboxes","Menus","Cheats","Script manager"},{
        function()
            
            local current_day = imgui.new.int(readMemory(0xB7014E,1,false)-1)
            imgui.SetNextItemWidth(imgui.GetWindowContentRegionWidth()/1.7)
            if imgui.Combo("Day", current_day,module.tgame.day.array,#module.tgame.day.names) then
                writeMemory(0xB7014E,1,current_day[0]+1,false)
                fcommon.CheatActivated()
            end
            
            imgui.Dummy(imgui.ImVec2(0,10))
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("Camera mode",module.tgame.camera.bool,string.format("Forward: %s\tBackward: %s\
Left: %s\t\t  Right: %s\n\nSlow movement: %s\nFast movement: %s\n\nRotation: Mouse\nZoom in/out : Mouse wheel \n\
Up : %s (Lock on player)\nDown: %s (Lock on player)",
            fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_forward),fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_backward),
            fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_left),fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_right),
            fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_slow),fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_fast),
            fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_up),
            fcommon.GetHotKeyNames(tcheatmenu.hot_keys.camera_mode_down)),nil,
            function()
                fcommon.CheckBoxVar("Lock on player",module.tgame.camera.lock_on_player,"Locks camera on player")

                imgui.Spacing()
                if imgui.SliderInt("FOV", module.tgame.camera.fov, 5,120) then
                    if module.tgame.camera.bool[0] then
                        cameraSetLerpFov(getCameraFov(),module.tgame.camera.fov[0],1000,true)
                        cameraPersistFov(true) 
                    end
                end
                imgui.SliderFloat("Movement Speed",module.tgame.camera.movement_speed, 0.0, 5.0)
                if imgui.SliderFloat("Shake", module.tgame.camera.shake, 0.0,100) then
                    if module.tgame.camera.bool[0] then
                        cameraSetShakeSimulationSimple(1,10000,module.tgame.camera.shake[0])
                    end
                end
                
                imgui.Spacing()
                if imgui.Button("Restore Camera",imgui.ImVec2(fcommon.GetSize(2))) then
                    restoreCamera()
                    module.tgame.camera.fov[0] = module.tgame.camera.fov[0]
                    cameraSetLerpFov(getCameraFov(),module.tgame.camera.fov[0],1000,true)
                    cameraPersistFov(true) 
                    module.tgame.camera.shake[0] = 0.0
                    cameraSetShakeSimulationSimple(1,1,0.0)
                    module.tgame.camera.movement_speed[0] = 0.2
                end
                imgui.SameLine()
                if imgui.Button("Warp player",imgui.ImVec2(fcommon.GetSize(2))) then
                    local cx,cy,cz = getActiveCameraCoordinates()
                    cz = getGroundZFor3dCoord(cx,cy,cz)
                    setCharCoordinates(PLAYER_PED,cx,cy,cz)
                    printHelpString("Player warped")
                end
            end)
            fcommon.CheckBoxFunc("Disable cheats",module.tgame.disable_cheats,
            function()
                if module.tgame.disable_cheats[0] then
                    writeMemory(0x004384D0 ,1,0xE9 ,false)
                    writeMemory(0x004384D1 ,4,0x000000D0 ,false)
                    writeMemory(0x004384D5 ,4,0x90909090 ,false)
                    fcommon.CheatActivated()
                else
                    writeMemory(0x004384D0 ,1,0x83,false)
                    writeMemory(0x004384D1 ,4,-0x7DF0F908,false)
                    writeMemory(0x004384D5 ,4,0xCC,false)
                    fcommon.CheatDeactivated()
                end
            end)
            fcommon.CheckBoxVar("Disable help popups",module.tgame.disable_help_popups,"Disables wasted & arrested popups that\nappear in a new game.Requires restart")
            fcommon.CheckBoxValue('Free PNS',0x96C009)
            fcommon.CheckBoxFunc("Freeze misson timer",module.tgame.freeze_mission_timer,function()
                if module.tgame.freeze_mission_timer[0] then
                    freezeOnscreenTimer(true)
                    fcommon.CheatActivated()
                else
                    freezeOnscreenTimer(false)
                    fcommon.CheatDeactivated()
                end
            end)
            fcommon.CheckBoxFunc("Disable F1 & F3 replay",module.tgame.disable_replay,function()
                if module.tgame.disable_replay[0] then
                    writeMemory(4588800,1,195,false)
                    fcommon.CheatActivated()
                else
                    writeMemory(4588800,1,160,false)
                    fcommon.CheatDeactivated()
                end
            end)
            fcommon.CheckBoxValue("Faster clock",0x96913B)            
            fcommon.CheckBoxVar("Freeze time",module.tgame.freeze_time)
            
            imgui.NextColumn()

            fcommon.CheckBoxFunc("Ghost cop vehicles",module.tgame.ghost_cop_cars,function()        
                for key,value in pairs(module.tgame.cop) do
                    if  module.tgame.ghost_cop_cars[0] then
                        writeMemory(tonumber(key),4,math.random(400,611),false)
                    else
                        writeMemory(tonumber(key),4,value,false)
                    end
                end
            end)
            fcommon.CheckBoxFunc("Keep stuff",module.tgame.keep_stuff,
            function()
                switchArrestPenalties(module.tgame.keep_stuff[0])
                switchDeathPenalties(module.tgame.keep_stuff[0])
            end,"Keep stuff after arrest/death")
            fcommon.CheckBoxVar("Random cheats",module.tgame.random_cheats.checkbox,"Activates random cheats after certain time",nil,
            function()
                fcommon.CheckBoxVar('Disable cheats',module.tgame.random_cheats.disable_cheat_checkbox,"Disable activated cheats after certain time")
                imgui.Spacing()
                imgui.SetNextItemWidth(imgui.GetWindowWidth()/2)
                imgui.SliderInt("Activate cheat timer", module.tgame.random_cheats.cheat_activate_timer, 10, 100)
                imgui.SetNextItemWidth(imgui.GetWindowWidth()/2)
                imgui.SliderInt("Deactivate cheat timer", module.tgame.random_cheats.cheat_deactivate_timer, 10, 100)
                imgui.Spacing()

                imgui.TextWrapped("Enabled cheats")
                imgui.Separator()
                if imgui.BeginChild("Cheats list") then  
                    for i=0,91,1 do   
                        if module.tgame.random_cheats.disabled_cheats[tostring(i)] then
                            selected = false
                        else
                            selected = true
                        end

                        if imgui.MenuItemBool(tostring(module.tgame.random_cheats.cheat_name[tostring(i)][1]),nil,selected) then
                            module.tgame.random_cheats.disabled_cheats[tostring(i)] = selected
                        end
                    end
                    imgui.EndChild()
                end
            end)
            fcommon.CheckBoxVar('Screenshot shortcut',module.tgame.ss_shortcut,"Take screenshot using" .. fcommon.GetHotKeyNames(tcheatmenu.hot_keys.quick_screenshot))
            fcommon.CheckBoxVar('Solid water',module.tgame.solid_water)
            fcommon.CheckBoxVar('Sync system time',module.tgame.sync_system_time)
            fcommon.CheckBoxValue('Widescreen',0xB6F065)
            imgui.Columns(1)
        
        end,
        function()

            fcommon.UpdateAddress({name = 'Days passed',address = 0xB79038 ,size = 4,min = 0,max = 9999})
            fcommon.DropDownMenu('FPS',function()

                imgui.Columns(2,nil,false)
                imgui.Text("Minimum" .. " = 1")
                
                imgui.NextColumn()
                imgui.Text("Maximum" .. " = 999")
                imgui.Columns(1)

                imgui.PushItemWidth(imgui.GetWindowWidth()-50)
                if imgui.InputInt('Set',module.tgame.fps_limit) then
                    memory.write(0xC1704C,(module.tgame.fps_limit[0]+1),1)
                    memory.write(0xBA6794,1,1)
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xC1704C),{1,module.tgame.fps_limit[0]+1})
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xBA6794),{1,1})
                end
                if module.tgame.fps_limit[0] < 1 then
                    module.tgame.fps_limit[0] = 1
                end

                imgui.PopItemWidth()

                imgui.Spacing()
                if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(3))) then
                    memory.write(0xC1704C,1,1)
                    memory.write(0xBA6794,1,1)
                    module.tgame.fps_limit[0] = 1
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xC1704C),{1,1})
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xBA6794),{1,1})
                end
                imgui.SameLine()
                if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
                    memory.write(0xC1704C,30,1)
                    memory.write(0xBA6794,1,1)
                    module.tgame.fps_limit[0] = 30
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xC1704C),{1,30})
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xBA6794),{1,1})
                end
                imgui.SameLine()
                if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
                    memory.write(0xBA6794,0,1)
                    memory.write(0xBA6794,1,1)
                    module.tgame.fps_limit[0] = 999
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xC1704C),{1,999})
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",0xBA6794),{1,1})
                end
            end)
            fcommon.UpdateAddress({name = 'Game speed',address = 0xB7CB64,size = 4,max = 10,min = 0, is_float =true, default = 1})
            fcommon.UpdateAddress({name = 'Gravity',address = 0x863984,size = 4,max = 1,min = -1, default = 0.008,cvalue = 0.001 ,is_float = true})
            SetTime()
            fcommon.DropDownMenu('Themes',function()
                fcommon.RadioButton('Select theme',{'Beach','Country','Fun house','Ninja'},{0x969159,0x96917D,0x969176,0x96915C})
            end)
        end,
        function()
            CheatsEntry({0x439110,0x439150,0x439190},nil,{'Body','Fat','Muscle','Skinny'})
            CheatsEntry({0x438E40,0x438FF0},nil,{'Fight','Health, armour\n200K money','Suicide'})
            CheatsEntry({0x438F90,0x438FC0},nil,{'Gameplay','Faster','Slower'})
            CheatsEntry({0x439360,0x4393D0},{0x96915A,0x96915B},{'Gangs','Gangs\neverywhere','Gang land'})
            CheatsEntry({0x4393F0,0x439710,0x439DD0,0x439C70,0x439B20},{0x96915D,0x969175,0x969158,0x969140,0x96913E},{'Peds','Slut magnet','Peds riot','Attack with\nrocket','Everyone Armed','Mayhem'})
            CheatsEntry({0x439930,0x439940,0x4399D0},nil,{'Skills','Stamina','Weapons','Vehicles'})
            CheatsEntry({0x4395B0,0x439600},nil,{'Spawn','Parachute','Jetpack'})
            CheatsEntry({0x439230,0x439720,0x439E50},{0x969159,0x969176,0x96915C},{'Themes','Beach','Fun house','Ninja'})
            CheatsEntry({0x4390D0,0x4390F0,0x4394B0,0x4394E0},{0x969150,0x969151,0x96915E,0x96915F},{'Traffic','Pink','Black','Cheap','Fast'})
            CheatsEntry({0x438F50,0x438F60,0x438F70,0x438F80,0x439570,0x439590},nil,{'Weather','Very sunny','Overcast','Rainy','Foggy','Thunderstorm','Sandstorm'})
            CheatsEntry({0x439D80,0x4398D0},{nil,0x969179},{'Vehicles','Blow up','Aim while\ndriving'})
            CheatsEntry({0x439540,0x4391D0,0x439F60,0x4395A0,0x439880},{0x969168,0x969157},{'Misc','Stop clock','Elvis\neverywhere','Countryside\ninvasion','Predator','Adrenaline'})
            CheatsEntry({0x438E90,0x438F20,0x4396F0,0x4396C0},{nil,nil,nil,0x969171},{'Wanted level','+2Star','Clear stars','Six star','Never wanted'})
            CheatsEntry({0x4385B0,0x438890,0x438B30},nil,{'Weapons','Set1','Set2','Set3'})
        end,
        function()
            if imgui.Button("Reload all scripts",imgui.ImVec2(fcommon.GetSize(1))) then
                fgame.tgame.script_manager.skip_auto_reload = true
                reloadScripts()
            end
            imgui.Spacing()

            if imgui.BeginChild("Script entries") then

                module.MonitorScripts()

                local filter = imgui.ImGuiTextFilter()
                filter:Draw("Filter")
                imgui.Spacing()
                
                for index, script in ipairs(script.list()) do
                    if filter:PassFilter(script.name) then
                        ShowLoadedScript(script,index)
                    end
                end

                for name,path in pairs(module.tgame.script_manager.not_loaded) do
                    if filter:PassFilter(name) then
                        ShowNotLoadedScripts(name,path)
                    end
                end
				imgui.EndChild()
			end
        end
    })
end

return module
