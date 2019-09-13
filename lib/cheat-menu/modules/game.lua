-- This module contains all the functions related to game tab
local module = {}

local tgame =
{
    ss_shortcut = imgui.new.bool(fconfig.get('tgame.ss_shortcut',false)),
    keep_stuff = imgui.new.bool(false),
    unlock_interior = imgui.new.bool(false),
    disable_cheats  = imgui.new.bool(fconfig.get('tgame.disable_cheats',false)),
    current_weather = 0,
    day_names = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
    weather_names = ftables.weather.table,
    fps =
    {
        limit = imgui.new.int(fconfig.get('tgame.fps.limit',30)),
    },
    airbreak = imgui.new.bool(false),
    disable_help_popups = imgui.new.bool(fconfig.get('tgame.disable_help_popups',false)),
    stats =
    {
        search_text = imgui.new.char[20](),
        names = ftables.stats.table,
    },
}

module.tgame = tgame

local days_list  = imgui.new['const char*'][#tgame.day_names](tgame.day_names)
local weather_list  = imgui.new['const char*'][#tgame.weather_names](tgame.weather_names)

-- Game Interriors list
local interior_names = ftables.interiors.table

function CheatsEntry(func,names)
    
    local sizeX = (imgui.GetWindowWidth() - imgui.StyleVar.FramePadding - imgui.StyleVar.ItemSpacing)/2.93
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
    if tgame.airbreak[0] then
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

function SetCurrentWeekday()
    imgui.Spacing()
    local current_day = imgui.new.int(readMemory(0xB7014E,1,false)-1)
    if imgui.Combo("Day", current_day,days_list,#tgame.day_names) then
        writeMemory(0xB7014E,1,current_day[0]+1,false)
        fcommon.CheatActivated()
    end
end

function SetWeather()
    imgui.Spacing()
    local current_weather = imgui.new.int(readMemory(0xC81320,2,false))
    if imgui.Combo("Weather", current_weather,weather_list,#tgame.weather_names) then
        writeMemory(0xC81320,2,current_weather[0],false)
        fcommon.CheatActivated()
    end
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

            fcommon.CheckBox({name = "Airbreak mode",var = tgame.airbreak,help_text = "Controls:\nW : Forward\tS : Backward\nA  : Left  \t\tD : Right\nArrow_Up\t : Move up\nArrow_Down : Move Down",func = function()
                if tgame.airbreak[0] ==  true then
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
            fcommon.CheckBox({name = "Disable cheats",var = tgame.disable_cheats,func = function()
                if tgame.disable_cheats[0] == true then
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
            fcommon.CheckBox({ name = "Disable help popups",var = tgame.disable_help_popups ,show_help_popups = true,help_text = "Disables wasted & arrested popups that\nappear in a new game.Requires restart."})
            fcommon.CheckBox({ address = 0x96C009,name = 'Free PNS'})

            imgui.NextColumn()
            
            fcommon.CheckBox({name = "Keep stuff",var = tgame.keep_stuff,help_text = "Keep stuff after arrest/death" ,func = function()
                if tgame.keep_stuff[0] == false then
                    switchArrestPenalties(true)
                    switchDeathPenalties(true)
                else
                    switchArrestPenalties(false)
                    switchDeathPenalties(false)
                end
            end})
            fcommon.CheckBox({ name = 'Screenshot shortcut',var = tgame.ss_shortcut,show_help_popups = true,help_text = "Take screenshot using (Left Ctrl + S) key combination"})
            fcommon.CheckBox({name = 'Unlock interior',var = tgame.unlock_interior,func = function()
                if tgame.unlock_interior[0] == true then
                    for _,name in ipairs(interior_names) do
                        switchEntryExit(name,1)
                        fcommon.CheatActivated()
                    end
                else
                    for _,name in ipairs(interior_names) do
                        switchEntryExit(name,0)
                        fcommon.CheatDeactivated()
                    end
                end
            end})
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

                fcommon.UpdateAddress({name = 'Days passed',address = 0xB79038 ,size = 4,min = 0,max = 1000})
                fcommon.DropDownMenu('FPS',function()

                    imgui.Columns(2,nil,false)
                    imgui.Text("Minimum" .. " = 1")
                    
                    imgui.NextColumn()
                    imgui.Text("Maximum" .. " = 999")
                    imgui.Columns(1)

                    imgui.PushItemWidth(imgui.GetWindowWidth()-50)
                    if imgui.InputInt('FPSLimit',tgame.fps.limit) then
                        memory.write(0xC1704C,(tgame.fps.limit[0]+1),1)
                        memory.write(0xBA6794,1,1)
                    end
                    if tgame.fps.limit[0] < 1 then
                        tgame.fps.limit[0] = 1
                    end

                    imgui.PopItemWidth()

                    imgui.Spacing()
                    if imgui.Button("Minimum",imgui.ImVec2(fcommon.GetSize(3))) then
                        memory.write(0xC1704C,1,1)
                        memory.write(0xBA6794,1,1)
                        tgame.fps.limit[0] = 1
                    end
                    imgui.SameLine()
                    if imgui.Button("Default",imgui.ImVec2(fcommon.GetSize(3))) then
                        memory.write(0xC1704C,30,1)
                        memory.write(0xBA6794,1,1)
                        tgame.fps.limit[0] = 30
                    end
                    imgui.SameLine()
                    if imgui.Button("Maximum",imgui.ImVec2(fcommon.GetSize(3))) then
                        memory.write(0xBA6794,0,1)
                        tgame.fps.limit[0] = 999
                    end




                end)
                fcommon.UpdateAddress({name = 'Game speed',address = 0xB7CB64,size = 4,max = 100,min = 0, is_float =true, default = 1})
                fcommon.UpdateAddress({name = 'Gravity',address = 0x863984,size = 4,max = 1,min = -1,is_float = true})
                SetTime()
                fcommon.UpdateAddress({name = 'Vehicle density multiplier',address = 0x8A5B20,size = 4,min = 0,max = 100, default = 10})
               
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
        if imgui.BeginTabItem("Stats") then
            imgui.Spacing()
            if imgui.Button("Max vehicle stats",imgui.ImVec2(fcommon.GetSize(2))) then
                callFunction(0x4399D0,1,1,false)
                displayNonMinigameHelpMessages(false)
                fcommon.CheatActivated()
            end
            imgui.SameLine()
            if imgui.Button("Max weapon stats",imgui.ImVec2(fcommon.GetSize(2))) then
                for i=69,79,1 do
                    setFloatStat(i)
                end
                fcommon.CheatActivated()
            end
            imgui.Spacing()
            if imgui.BeginTabBar("Stats") then
                imgui.Spacing()

                if imgui.BeginTabItem("Vehicles") then
                    fcommon.UpdateStat({ name = "Bike",stat = 229})
                    fcommon.UpdateStat({ name = "Cycling",stat = 230})
                    fcommon.UpdateStat({ name = "Driving",stat = 160})
                    fcommon.UpdateStat({ name = "Flying",stat = 223})        
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Weapons") then
                    fcommon.UpdateStat({ name = "AK47",stat = 77})
                    fcommon.UpdateStat({ name = "Combat shotgun",stat = 74})
                    fcommon.UpdateStat({ name = "Desert eagle",stat = 71})
                    
                    fcommon.UpdateStat({ name = "Machine pistol",stat = 75})
                    fcommon.UpdateStat({ name = "M4",stat = 78})
                    fcommon.UpdateStat({ name = "Pistol",stat = 69})
                    fcommon.UpdateStat({ name = "Sawn off shotgun",stat = 73})
                    fcommon.UpdateStat({ name = "Shotgun",stat = 72})
                    fcommon.UpdateStat({ name = "Silenced pistol",stat = 70})
                    fcommon.UpdateStat({ name = "SMG",stat = 76})
                    fcommon.UpdateStat({ name = "Rifle",stat = 79})
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("GF") then

                    imgui.Spacing()
                    if imgui.Button("Max GF progress",imgui.ImVec2(fcommon.GetSize(1))) then
                        for i=252,257,1 do
                            setFloatStat(i,100)
                        end
                        clearHelp()
                        fcommon.CheatActivated()
                    end
                    imgui.Spacing()
                    imgui.Text("Progress")
                    imgui.Separator()
                    imgui.Spacing()
                    fcommon.UpdateStat({ name = "Barbara",stat = 255,max = 100})
                    fcommon.UpdateStat({ name = "Denise",stat = 252,max = 100})
                    fcommon.UpdateStat({ name = "Helena",stat = 254,max = 100})
                    fcommon.UpdateStat({ name = "Katie",stat = 256,max = 100})
                    fcommon.UpdateStat({ name = "Michelle",stat = 253,max = 100})
                    fcommon.UpdateStat({ name = "Millie",stat = 257,max = 100})
                    imgui.EndTabItem()
                end
                if imgui.BeginTabItem("Search") then
                    imgui.Spacing()
                    imgui.Columns(1)
                    if imgui.InputText("Search",tgame.stats.search_text,ffi.sizeof(tgame.stats.search_text)) then end
                    imgui.SameLine()

                    imgui.Spacing()
                    imgui.Text("Found entries :(" .. ffi.string(tgame.stats.search_text) .. ")")
                    imgui.Separator()
                    imgui.Spacing()
                    if imgui.BeginChild("Stat Entries") then
                        for i=0,342,1 do
                            if tgame.stats.names[i] ~= nil then
                                if (ffi.string(tgame.stats.search_text) == "") or (string.upper(tgame.stats.names[i]):find(string.upper(ffi.string(tgame.stats.search_text))) ~= nil) then
                                    fcommon.UpdateStat({ name = tgame.stats.names[i],stat = i})
                                end
                            end
                        end
                        imgui.Spacing()
                        imgui.EndChild()
                    end
                    imgui.EndTabItem()
                end
                imgui.EndTabBar()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
