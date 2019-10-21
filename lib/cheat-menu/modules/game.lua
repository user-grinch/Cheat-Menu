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
    airbreak                = imgui.new.bool(false),
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
    disable_cheats          = imgui.new.bool(fconfig.get('tgame.disable_cheats',false)),
    disable_help_popups     = imgui.new.bool(fconfig.get('tgame.disable_help_popups',false)),
    fps_limit               = imgui.new.int(fconfig.get('tgame.fps_limit',30)),
    ghost_cop_cars          = imgui.new.bool(fconfig.get('tgame.ghost_cop_cars',false)),
    keep_stuff              = imgui.new.bool(fconfig.get('tgame.keep_stuff',false)),
    random_cheats           = imgui.new.bool(fconfig.get('tgame.random_cheats',false)),
    script_manager          =
    {
        search_text         = imgui.new.char[64](""),
        disabled            = {},
    },
    ss_shortcut             = imgui.new.bool(fconfig.get('tgame.ss_shortcut',false)), 
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

function module.AirbreakMode()
    if module.tgame.airbreak[0] then
        if isKeyDown(tkeys.airbreak_up) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            setCharCoordinates(PLAYER_PED,x,y,z+2.0)
        end
        if isKeyDown(tkeys.airbreak_down) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            setCharCoordinates(PLAYER_PED,x,y,z-2.0)
        end
        if isKeyDown(tkeys.airbreak_forward) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            angle = getCharHeading(PLAYER_PED)

            angle =  angle+90
            x = x + 2.0 * math.cos(angle * math.pi/180)
            y = y + 2.0 * math.sin(angle * math.pi/180)

            setCharCoordinates(PLAYER_PED,x,y,z-1.0)
        end
        if isKeyDown(tkeys.airbreak_backward) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            angle = getCharHeading(PLAYER_PED)

            angle =  angle+90
            x = x - 2.0 * math.cos(angle * math.pi/180)
            y = y - 2.0 * math.sin(angle * math.pi/180)

            setCharCoordinates(PLAYER_PED,x,y,z-1.0)
        end
        if isKeyDown(tkeys.airbreak_left)then
            setCharHeading(PLAYER_PED,getCharHeading(PLAYER_PED)+1.0)
            setCameraBehindPlayer()
        end
        if isKeyDown(tkeys.airbreak_right)then
            setCharHeading(PLAYER_PED,getCharHeading(PLAYER_PED)-1.0)
            setCameraBehindPlayer()
        end
    end
end

function SetTime()
    fcommon.DropDownMenu("Time",function()
        imgui.Spacing()

        local days_passed = imgui.new.int(memory.read(0xB79038 ,4))
        local hour = imgui.new.int(memory.read(0xB70153,1))
        local minute = imgui.new.int(memory.read(0xB70152,1))

        fcommon.CheckBox({ address = 0x96913B,name = "Faster clock"})
        imgui.SameLine()
        fcommon.CheckBox({ address = 0x969168,name = "Freeze time"})
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

function ShowScriptDisabled(name,path,search_text)
    fcommon.DropDownMenu(name .. "##" .. path,function()

        imgui.Spacing()
        imgui.SameLine()
        imgui.Text("Active: false")
        imgui.Spacing()
        imgui.SameLine()
        imgui.TextWrapped("Filepath: " .. path)
        
        if imgui.Button("Load##" .. path,imgui.ImVec2(fcommon.GetSize(1))) then
            if doesFileExist(path) then 
                script.load(path)
                module.tgame.script_manager.disabled[name] = nil
                printHelpString("Script loaded")
            end
        end
    end,true)
end

function ShowScriptEnabled(script,search_text,index)
    fcommon.DropDownMenu(script.name .. "##" .. index,function()
        local authors = "Authors: "
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
        imgui.TextWrapped(string.sub(authors,1,-3))
        imgui.Text("Version: " .. tostring(script.version))
        imgui.Text("Version num: " .. tostring(script.version_num))
        imgui.NextColumn()
        imgui.Text("Script ID: " .. script.id)
        imgui.Text("Active: " .. tostring( not script.frozen))
        imgui.TextWrapped("Filename: " .. string.sub(script.filename,1,-5))
        imgui.Columns(1)
        if properties ~= "" then
            imgui.Spacing()
            imgui.SameLine()
            imgui.TextWrapped("Properties: " .. string.sub(properties,1,-3))
        end
        if dependencies ~= "" then
            imgui.Spacing()
            imgui.SameLine()
            imgui.TextWrapped("Dependencies: " .. string.sub(dependencies,1,-3))
        end
        if description ~= "" then
            imgui.Spacing()
            imgui.SameLine()
            imgui.TextWrapped("Description: " .. script.description)
        end
        if imgui.Button("Reload##" .. index,imgui.ImVec2(fcommon.GetSize(2))) then
            script:reload()
            printHelpString("Script reloaded")
        end
        imgui.SameLine()
        if imgui.Button("Unload##" .. index,imgui.ImVec2(fcommon.GetSize(2))) then
            module.tgame.script_manager.disabled[script.name] = script.path
            script:unload()
            printHelpString("Script unloaded")
        end
    end)
end

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

            fcommon.CheckBox({name = "Airbreak mode",var = module.tgame.airbreak,help_text = "Controls:\nW : Forward\tS : Backward\nA  : Left  \t\tD : Right\nArrow_Up\t : Move up\nArrow_Down : Move Down",func = function()
                if module.tgame.airbreak[0] ==  true then
                    lockPlayerControl(true)
                    setCharCollision(PLAYER_PED,false)
                    setCameraBehindPlayer()
                    fcommon.CheatActivated()
                else
                    lockPlayerControl(false)
                    setCharCollision(PLAYER_PED,true)
                    fcommon.CheatDeactivated()
                end
            end})
            fcommon.CheckBox({name = "Disable cheats",var = module.tgame.disable_cheats,func = function()
                if module.tgame.disable_cheats[0] == true then
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
            end})
            fcommon.CheckBox({ name = "Disable help popups",var = module.tgame.disable_help_popups ,show_help_popups = true,help_text = "Disables wasted & arrested popups that\nappear in a new game.Requires restart."})
            fcommon.CheckBox({ address = 0x96C009,name = 'Free PNS'})
            fcommon.CheckBox({name = "Ghost cop vehicles",var = module.tgame.ghost_cop_cars,func = function()        
                for key,value in pairs(module.tgame.cop) do
                    if  module.tgame.ghost_cop_cars[0] then
                        writeMemory(tonumber(key),4,math.random(400,611),false)
                    else
                        writeMemory(tonumber(key),4,value,false)
                    end
                end
            end})

            imgui.NextColumn()
            
            fcommon.CheckBox({name = "Keep stuff",var = module.tgame.keep_stuff,help_text = "Keep stuff after arrest/death" ,func = function()
                switchArrestPenalties(module.tgame.keep_stuff[0])
                switchDeathPenalties(module.tgame.keep_stuff[0])
            end})
            fcommon.CheckBox({ name = "Random cheats",var = module.tgame.random_cheats ,show_help_popups = true,help_text = "Activates random cheats every 10 seconds\nSuicide cheat is excluded"})
            fcommon.CheckBox({ name = 'Screenshot shortcut',var = module.tgame.ss_shortcut,show_help_popups = true,help_text = "Take screenshot using (Left Ctrl + S) key combination"})
            fcommon.CheckBox({ address = 0xB6F065,name = 'Widescreen'})
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
            imgui.Spacing()
			if imgui.InputText("Search",module.tgame.script_manager.search_text,ffi.sizeof(module.tgame.script_manager.search_text)) then end
			fcommon.InformationTooltip("Moonloader scripts manager")
			imgui.Spacing()
			imgui.Text("Found scripts :(" .. ffi.string(module.tgame.script_manager.search_text) .. ")")
			imgui.Separator()
			imgui.Spacing()
			if imgui.BeginChild("Script entries") then
                for index, script in ipairs(script.list()) do
                    if ffi.string(module.tgame.script_manager.search_text) == "" then
						ShowScriptEnabled(script,"",index)
					else
						if string.upper(script.name):find(string.upper(ffi.string(module.tgame.script_manager.search_text))) ~= nil  then
							ShowScriptEnabled(script,ffi.string(module.tgame.script_manager.search_text),index)
						end
					end
                end
                for name,path in pairs(module.tgame.script_manager.disabled) do
                    if ffi.string(module.tgame.script_manager.search_text) == "" then
						ShowScriptDisabled(name,path,"",index)
					else
						if string.upper(name):find(string.upper(ffi.string(module.tgame.script_manager.search_text))) ~= nil  then
							ShowScriptDisabled(name,path,ffi.string(module.tgame.script_manager.search_text),index)
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
