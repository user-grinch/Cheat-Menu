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

module.tgame                =
{
    camera                  =
    {
        bool                = imgui.new.bool(false),
        speed               = imgui.new.float(fconfig.Get('tgame.camera.speed',0.3)),
        rotation            = 0.0,
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
    random_cheats           = imgui.new.bool(fconfig.Get('tgame.random_cheats',false)),
    script_manager          =
    {
        skip_auto_reload    = false,
        not_loaded          = {},
        search_text         = imgui.new.char[64](""),
    },
    solid_water             = imgui.new.bool(fconfig.Get('tgame.solid_water',false)),
    ss_shortcut             = imgui.new.bool(fconfig.Get('tgame.ss_shortcut',false)), 
    weather                 =
    {    
        names               = 
        {
            "EXTRASUNNY LA",
            "SUNNY LA",
            "EXTRASUNNY SMOG LA",
            "SUNNY SMOG LA",
            "CLOUDY LA",
            "SUNNY SF",
            "EXTRASUNNY SF",
            "CLOUDY SF",
            "RAINY SF",
            "FOGGY SF",
            "SUNNY VEGAS",
            "EXTRASUNNY VEGAS",
            "CLOUDY VEGAS",
            "EXTRASUNNY COUNTRYSIDE",
            "SUNNY COUNTRYSIDE",
            "CLOUDY COUNTRYSIDE",
            "RAINY COUNTRYSIDE",
            "EXTRASUNNY DESERT",
            "SUNNY DESERT",
            "SANDSTORM DESERT",
            "UNDERWATER",
            "EXTRACOLOURS 1",
            "EXTRACOLOURS 2",
        },
        array               = {},
    },
}

module.tgame.day.array      = imgui.new['const char*'][#module.tgame.day.names](module.tgame.day.names)
module.tgame.weather.array  = imgui.new['const char*'][#module.tgame.weather.names](module.tgame.weather.names)


function module.SolidWater()
    local object = nil
    while true do
        if module.tgame.solid_water[0] then
            while module.tgame.solid_water[0] do
                wait(0)
                local x,y,z = getCharCoordinates(PLAYER_PED)
                local water_height =  getWaterHeightAtCoords(x,y,false)

                if doesObjectExist(object) then
                    deleteObject(object)
                    removeObjectElegantly(object)
                end

                if z > water_height and water_height ~= -1000 and not isCharInAnyBoat(PLAYER_PED) then     -- Don't create the object if player is under water/diving
                    object = createObject(3095,x,y,water_height)
                    
                    setObjectVisible(object,false)
                end
            end

            if doesObjectExist(object) then
                deleteObject(object)
                removeObjectElegantly(object)
            end
            
        end
        wait(0)
    end
end

function module.CameraMode()
    while true do
        if module.tgame.camera.bool[0] then
            freezeCharPositionAndDontLoadCollision(PLAYER_PED,true)
            setCharCollision(PLAYER_PED,false)
            
            writeMemory(0xBA676C,1,2,false) -- Radar
            writeMemory(0xBA6769,1,0,false) -- Hud

            local model_val = readMemory((getCharPointer(PLAYER_PED)+1140),4,false)
            writeMemory(getCharPointer(PLAYER_PED)+1140,4,2,false)

            local total_mouse_x = 0
            local total_mouse_y = 0

            while module.tgame.camera.bool[0] do
                local x, y, z = getCharCoordinates(PLAYER_PED)

               
                if isKeyDown(tkeys.camera_mode_extra) then -- LSHIFT
                    
                    if isKeyDown(tkeys.camera_mode_up) then -- W
                        z = z+module.tgame.camera.speed[0]
                    end
                    
                    if isKeyDown(tkeys.camera_mode_down) then -- S
                        z = z-module.tgame.camera.speed[0]
                    end

                else
                    if isKeyDown(tkeys.camera_mode_forward) then -- W
                        local angle = getCharHeading(PLAYER_PED) + 90

                        x = x + module.tgame.camera.speed[0] * math.cos(angle * math.pi/180)
                        y = y + module.tgame.camera.speed[0] * math.sin(angle * math.pi/180)
                    end
            
                    if isKeyDown(tkeys.camera_mode_backward) then -- S
                        local angle = getCharHeading(PLAYER_PED) + 90

                        x = x - module.tgame.camera.speed[0] * math.cos(angle * math.pi/180)
                        y = y - module.tgame.camera.speed[0] * math.sin(angle * math.pi/180) 
                    end

                end

                local mouse_x, mouse_y =  getPcMouseMovement()

                total_mouse_x = total_mouse_x + mouse_x
                total_mouse_y = total_mouse_y + mouse_y
                setCharHeading(PLAYER_PED,total_mouse_x*-module.tgame.camera.speed[0])
                module.tgame.camera.rotation = total_mouse_y
                module.tgame.camera.rotation = total_mouse_y

                setCharCoordinates(PLAYER_PED,x,y,z-1.0)
                attachCameraToChar(PLAYER_PED,0.0, 0.0, 0.0, 0.0, 180, module.tgame.camera.rotation, 0.0, 2)

                wait(0)
            end
            freezeCharPositionAndDontLoadCollision(PLAYER_PED,false)

            setCharCollision(PLAYER_PED,true)

            writeMemory(0xBA676C,1,0,false) -- Radar
            writeMemory(0xBA6769,1,1,false) -- Hud

            restoreCameraJumpcut()
            writeMemory((getCharPointer(PLAYER_PED)+1140),4,model_val,false)
        end
        wait(0)
    end
end

function CheatsEntry(func,names)
    local sizeX = imgui.GetWindowContentRegionWidth() / 3
    local sizeY = imgui.GetWindowHeight()/15
    fcommon.DropDownMenu(names[1],function()
        imgui.Spacing()

        for i = 1, #func do

            if imgui.Button(names[i+1],imgui.ImVec2(sizeX,sizeY)) then
                callFunction(func[i],1,1)
                fcommon.CheatActivated()
            end
            if i % 3 ~= 0 then
                imgui.SameLine()
            end
        end
    end)
end

function module.RandomCheats()
    while true do
        if module.tgame.random_cheats[0] then
            cheatid = math.random(1,92)
            if cheatid ~= 29 then  -- Suicide cheat
                callFunction(0x00438370,1,1,cheatid)
                fcommon.CheatActivated()
            end
            wait(10000)
        else
            wait(0)
        end
    end
end

function module.FreezeTime()
    while true do
        if module.tgame.freeze_time[0] then

            local weather = readMemory(0xC81320,2,false) -- Weather
            memory.write(0x969168,1,1)  -- Freeze time

            while module.tgame.freeze_time[0] do
                writeMemory(0xC81320,2,weather,false) -- Weather
                wait(0)
            end
            memory.write(0x969168,0,1)  -- Freeze time
        end
        wait(0)
    end
end

function SetTime()
    fcommon.DropDownMenu("Time",function()
        imgui.Spacing()

        local days_passed = imgui.new.int(memory.read(0xB79038 ,4))
        local hour = imgui.new.int(memory.read(0xB70153,1))
        local minute = imgui.new.int(memory.read(0xB70152,1))

        fcommon.CheckBoxValue("Faster clock",0x96913B)
        imgui.SameLine()
        fcommon.CheckBoxVar("Freeze time",module.tgame.freeze_time)
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

function SetCurrentWeekday()
    imgui.Spacing()
    local current_day = imgui.new.int(readMemory(0xB7014E,1,false)-1)
    if imgui.Combo("Day", current_day,module.tgame.day.array,#module.tgame.day.names) then
        writeMemory(0xB7014E,1,current_day[0]+1,false)
        fcommon.CheatActivated()
    end
end

function SetWeather()
    imgui.Spacing()
    local current_weather = imgui.new.int(readMemory(0xC81320,2,false))
    if imgui.Combo("Weather", current_weather,module.tgame.weather.array,#module.tgame.weather.names) then
        writeMemory(0xC81320,2,current_weather[0],false)
        fcommon.CheatActivated()
    end
end

--------------------------------------------------
-- Functions of script manager

function module.MonitorScripts()
    local mainDir  = getWorkingDirectory()
    for file in lfs.dir(mainDir) do
        local full_file_path = mainDir .. "\\" .. file
        if doesFileExist(full_file_path) then

            local file_path,file_name,file_ext = string.match(full_file_path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 

            if (file_ext == "lua" or file_ext == "neverload") and module.tgame.script_manager.not_loaded[file_name] == nil  then
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

function ShowNotLoadedScripts(name,path,search_text)
    fcommon.DropDownMenu(name .. "##" .. path,function()

        imgui.Spacing()
        imgui.SameLine()

        local _,file_name,file_ext = string.match(path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 

        if file_ext ==  "lua" then
            imgui.Text("Status: Not loaded")
        end
        if file_ext ==  "neverload" then
            imgui.Text("Status: Never load")
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
                script.load(load_path)
                module.tgame.script_manager.not_loaded[name] = nil
                printHelpString("Script loaded")
            end
        end
    end,true)
end

function ShowLoadedScript(script,search_text,index)
    fcommon.DropDownMenu(script.name .. "##" .. index,function()
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
        if imgui.Button("Never load##" .. index,imgui.ImVec2(fcommon.GetSize(3))) then
            printHelpString("Script set to never load")
            os.rename(script.path,script.path.. ".neverload")
            script:unload()
        end
        imgui.SameLine()
        if imgui.Button("Reload##" .. index,imgui.ImVec2(fcommon.GetSize(3))) then
            if script.name == thisScript().name then
                module.tgame.script_manager.skip_auto_reload = true
            end  
            printHelpString("Script reloaded")
            script:reload()
        end
        imgui.SameLine()
        if imgui.Button("Unload##" .. index,imgui.ImVec2(fcommon.GetSize(3))) then
            if script.name == thisScript().name then
                module.tgame.script_manager.skip_auto_reload = true
            end
            printHelpString("Script unloaded")
            script:unload()
        end
    end)
end

--------------------------------------------------


-- Main function
function module.GameMain()
    if imgui.Button("Save game",imgui.ImVec2(fcommon.GetSize(1))) then
        if isCharOnFoot(PLAYER_PED) then
            activateSaveMenu()
        else
            printHelpString("Player is ~r~not~w~ on foot")
        end
    end
    imgui.Spacing()
    if imgui.BeginTabBar("Game") then

        if imgui.BeginTabItem("Checkbox") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)

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
            fcommon.CheckBoxFunc("Ghost cop vehicles",module.tgame.ghost_cop_cars,function()        
                for key,value in pairs(module.tgame.cop) do
                    if  module.tgame.ghost_cop_cars[0] then
                        writeMemory(tonumber(key),4,math.random(400,611),false)
                    else
                        writeMemory(tonumber(key),4,value,false)
                    end
                end
            end)
            
            imgui.NextColumn()

            fcommon.CheckBoxFunc("Keep stuff",module.tgame.keep_stuff,
            function()
                switchArrestPenalties(module.tgame.keep_stuff[0])
                switchDeathPenalties(module.tgame.keep_stuff[0])
            end,"Keep stuff after arrest/death")
            fcommon.CheckBoxVar("Random cheats",module.tgame.random_cheats ,"Activates random cheats every 10 seconds\nSuicide cheat is excluded")
            fcommon.CheckBoxVar('Screenshot shortcut',module.tgame.ss_shortcut,"Take screenshot using (Left Ctrl + S)")
            fcommon.CheckBoxVar('Solid water',module.tgame.solid_water)
            fcommon.CheckBoxValue('Widescreen',0xB6F065)
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()

            SetCurrentWeekday()
            SetWeather()
            imgui.Spacing()
            fcommon.RadioButton('Game themes',{'Beach','Country','Fun house','Ninja'},{0x969159,0x96917D,0x969176,0x96915C})
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Menu') then
            if imgui.BeginChild("Game") then
                imgui.Spacing()

                fcommon.DropDownMenu('Camera mode',function()
                    fcommon.CheckBoxVar("Camera mode ##Checkbox",module.tgame.camera.bool,
                    "Controls:\nForward: W\tBackward: S\nMove up: Left Shift + W\nMove down: Left Shift + S\nRotation: Mouse")
                    imgui.InputFloat("Speed",module.tgame.camera.speed)
                    imgui.Spacing()
                    if imgui.Button("Slow",imgui.ImVec2(fcommon.GetSize(3))) then
                        module.tgame.camera.speed[0] = 0.03
                    end
                    imgui.SameLine()
                    if imgui.Button("Normal",imgui.ImVec2(fcommon.GetSize(3))) then
                        module.tgame.camera.speed[0] = 0.3
                    end
                    imgui.SameLine()
                    if imgui.Button("Fast",imgui.ImVec2(fcommon.GetSize(3))) then
                        module.tgame.camera.speed[0] = 3.0
                    end
                end)

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
                    end
                    imgui.SameLine()
                    if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
                        memory.write(0xC1704C,30,1)
                        memory.write(0xBA6794,1,1)
                        module.tgame.fps_limit[0] = 30
                    end
                    imgui.SameLine()
                    if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
                        memory.write(0xBA6794,0,1)
                        module.tgame.fps_limit[0] = 999
                    end
                end)
                fcommon.UpdateAddress({name = 'Game speed',address = 0xB7CB64,size = 4,max = 10,min = 0, is_float =true, default = 1})
                fcommon.UpdateAddress({name = 'Gravity',address = 0x863984,size = 4,max = 1,min = -1, default = 0.008,cvalue = 0.001 ,is_float = true})
                SetTime()
                
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Cheats') then
            imgui.Spacing()
            CheatsEntry({0x439110,0x439150,0x439190},{'Body','Fat','Muscle','Skinny'})
            CheatsEntry({0x438E40,0x438FF0},{'Fight','Health, armour\n200K money','Suicide'})
            CheatsEntry({0x438F90,0x438FC0},{'Gameplay','Faster','Slower'})
            CheatsEntry({0x439360,0x4393D0},{'Gangs','Gangs\neverywhere','Gang land'})
            CheatsEntry({0x4393F0,0x439710,0x439DD0,0x439C70,0x439B20},{'Peds','Slut magnet','Peds riot','Attack with\nrocket','Attack player','Mayhem'})
            CheatsEntry({0x439930,0x439940,0x4399D0},{'Skills','Stamina','Weapons','Vehicles'})
            CheatsEntry({0x4395B0,0x439600},{'Spawn','Parachute','Jetpack'})
            CheatsEntry({0x439230,0x439720,0x439E50},{'Themes','Beach','Fun house','Ninja'})
            CheatsEntry({0x4390D0,0x4390F0,0x4394B0,0x4394E0},{'Traffic','Pink','Black','Cheap','Fast'})
            CheatsEntry({0x438F50,0x438F60,0x438F70,0x438F80,0x439570,0x439590},{'Weather','Very sunny','Overcast','Rainy','Foggy','Thunderstorm','Sandstorm'})
            CheatsEntry({0x439D80,0x4398D0},{'Vehicles','Blow up','Aim while\ndriving'})
            CheatsEntry({0x439540,0x4391D0,0x439F60,0x4395A0,0x439880},{'Misc','Stop clock','Elvis\neverywhere','Countryside\ninvasion','Predator','Adrenaline'})
            CheatsEntry({0x438E90,0x438F20,0x4396F0,0x4396C0},{'Wanted level','+2Star','Clear stars','Six star','Never wanted'})
            CheatsEntry({0x4385B0,0x438890,0x438B30},{'Weapons','Set1','Set2','Set3'})
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Script Manager') then
            
            imgui.Spacing()
            if imgui.Button("Reload all scripts",imgui.ImVec2(fcommon.GetSize(1))) then
                reloadScripts()
            end
            imgui.Spacing()
            
			if imgui.InputText("Search",module.tgame.script_manager.search_text,ffi.sizeof(module.tgame.script_manager.search_text)) then end
			fcommon.InformationTooltip("Moonloader scripts manager")
			imgui.Spacing()
			imgui.Text("Scripts found :(" .. ffi.string(module.tgame.script_manager.search_text) .. ")")
			imgui.Separator()
			imgui.Spacing()
            if imgui.BeginChild("Script entries") then

                module.MonitorScripts()

                for index, script in ipairs(script.list()) do
                    if ffi.string(module.tgame.script_manager.search_text) == "" then
						ShowLoadedScript(script,"",index)
					else
						if string.upper(script.name):find(string.upper(ffi.string(module.tgame.script_manager.search_text))) ~= nil  then
							ShowLoadedScript(script,ffi.string(module.tgame.script_manager.search_text),index)
						end
					end
                end
                for name,path in pairs(module.tgame.script_manager.not_loaded) do
                    if ffi.string(module.tgame.script_manager.search_text) == "" then
						ShowNotLoadedScripts(name,path,"",index)
					else
						if string.upper(name):find(string.upper(ffi.string(module.tgame.script_manager.search_text))) ~= nil  then
							ShowNotLoadedScripts(name,path,ffi.string(module.tgame.script_manager.search_text),index)
						end
					end
                end
				imgui.EndChild()
			end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
