-- This module contains all the functions related to game tab

local module = {}

local tgame =
{
    ss_shortcut = imgui.new.bool(false),
    keep_stuff = imgui.new.bool(false),
    unlock_interior = imgui.new.bool(false),
    current_weather = 0,
    day_names = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
    weather_names = {"EXTRASUNNY LA","SUNNY LA","EXTRASUNNY SMOG LA","SUNNY SMOG LA","CLOUDY LA","SUNNY SF","EXTRASUNNY SF","CLOUDY SF","RAINY SF",
                "FOGGY SF","SUNNY VEGAS","EXTRASUNNY VEGAS","CLOUDY VEGAS","EXTRASUNNY COUNTRYSIDE","SUNNY COUNTRYSIDE","CLOUDY COUNTRYSIDE",
                "RAINY COUNTRYSIDE","EXTRASUNNY DESERT","SUNNY DESERT","SANDSTORM DESERT","UNDERWATER","EXTRACOLOURS 1","EXTRACOLOURS 2",
              },
}
module.tgame = tgame
local days_list  = imgui.new['const char*'][#tgame.day_names](tgame.day_names)
local weather_list  = imgui.new['const char*'][#tgame.weather_names](tgame.weather_names)



-- FPS table
local tfps =
{
    bool = imgui.new.bool(false),
    total = 0,
    time = 0,
    show = 0,
    limit = imgui.new.int(30)
}

module.tfps = tfps


-- Airbreak table
tairbreak =
{
    bool = imgui.new.bool(false)
}

-- Game Interriors list
local interior_names = {
    "ABATOIR",
    "AMMUN1",
    "CARMOD1",
    "FDREST1",
    "GF1",
    "JETINT",
    "LACS1",
    "LAHS1B",
    "MAFCAS",
    "MAFCAS2",
    "SMASHTV",
    "SVVGHO1",
    "SWEETS",
    "TSDINER",
    "WUZIBET",
    "BARBERS",
    "BDUPS1",
    "CARMOD2",
    "CARTER",
    "GF2",
    "LAHS1A",
    "LASTRIP",
    "RYDERS",
    "SVVGHO2",
    "VGHSB1",
    "VGHSB3",
    "BARBER2",
    "BDUPS",
    "BIKESCH",
    "BROTHL1",
    "CARLS",
    "CARMOD3",
    "CHANGER",
    "CSSPRT",
    "DRIVES",
    "DRIVES2",
    "GENOTB",
    "GF3",
    "LAHSB4",
    "OGLOCS",
    "PAPER",
    "PDOMES",
    "PDOMES2",
    "POLICE3",
    "SEXSHOP",
    "S1TEST",
    "STRIP2",
    "STUDIO",
    "TATTO3",
    "AMMUN2",
    "DINER1",
    "DIRBIKE",
    "GF4",
    "LAHS2A",
    "LAHSS6",
    "SFHSM2",
    "X711S2",
    "CSDESGN",
    "DINER2",
    "FDPIZA",
    "GANG",
    "GF5",
    "GYM1",
    "LACRAK",
    "LAHSB3",
    "MADDOGS",
    "MDDOGS",
    "SFHSB1",
    "SVHOT1",
    "VGHSM2",
    "AMMUN3",
    "AMMUN5",
    "BROTHEL",
    "GF6",
    "GYM2",
    "LAHSB1",
    "POLICE1",
    "RCPLAY",
    "REST2",
    "SFHSB2",
    "SFHSS2",
    "SVCUNT",
    "SVSFSM",
    "X7_11S",
    "8TRACK",
    "AMMUN4",
    "GYM3",
    "LAHSB2",
    "OFTEST",
    "BURHOUS",
    "SFHSS1",
    "SVLAMD",
    "FDCHICK",
    "LAHS2B",
    "SFHSB3",
    "SVGNMT2",
    "SVVGMD",
    "DESHOUS",
    "FDBURG",
    "POLICE2",
    "SVGNMT1",
    "TRICAS",
    "SVSFMD",
    "VGHSM3",
    "X711S3",
    "BAR2",
    "SVLASM",
    "BARBER3",
    "MOROOM",
    "SVLABIG",
    "CSEXL",
    "AIRPOR2",
    "AIRPORT",
    "CSCHP",
    "MOTEL1",
    "SFHSM1",
    "VGHSS1",
    "VGSHM2",
    "VGSHM3",
    "VGSHS2",
    "TATTOO",
    "X7_11C",
    "BAR1",
    "DAMIN",
    "FDDONUT",
    "TATTO2",
    "X7_11D",
    "ATRIUME",
    "ATRIUMX",
    "CLOTHGP",
    "GENWRHS",
    "UFOBAR",
    "X7_11B"
}

-- Change specific town cars
function AddTownVehicleEntry(title,address,vehicles,rows)
    imgui.Text(title)
    for i = 1,#vehicles,1 do
        if fvehicles.tvehicles.images[tostring(vehicles[i])] == nil then
            fvehicles.tvehicles.images[tostring(vehicles[i])] = "LOADING"
            lua_thread.create(fcommon.LoadTexture,fvehicles.tvehicles.images,fvehicles.tvehicles.path,vehicles[i],".jpg")
        else
            local x,y = fcommon.GetSize(rows)
            if imgui.ImageButton(fvehicles.tvehicles.images[tostring(vehicles[i])],imgui.ImVec2(x/1.5,y*3.8)) then
                writeMemory(address,4,vehicles[i],false)
                fcommon.CheatActivated()
            end
            if imgui.IsItemHovered() then
                imgui.BeginTooltip()
                imgui.SetTooltip(getNameOfVehicleModel(vehicles[i]))
                imgui.EndTooltip()
            end
        end
        imgui.SameLine()
    end
    imgui.NewLine()
end

function module.AirbreakMode()
    if tairbreak.bool[0] then
        if isKeyDown(keys.airbreak_up) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            setCharCoordinates(PLAYER_PED,x,y,z+2.0)
        end
        if isKeyDown(keys.airbreak_down) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            setCharCoordinates(PLAYER_PED,x,y,z-2.0)
        end
        if isKeyDown(keys.airbreak_forward) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            angle = getCharHeading(PLAYER_PED)

            angle =  angle+90
            x = x + 2.0 * math.cos(angle * math.pi/180)
            y = y + 2.0 * math.sin(angle * math.pi/180)

            setCharCoordinates(PLAYER_PED,x,y,z-1.0)
        end
        if isKeyDown(keys.airbreak_backward) then
            x,y,z = getCharCoordinates(PLAYER_PED)
            angle = getCharHeading(PLAYER_PED)

            angle =  angle+90
            x = x - 2.0 * math.cos(angle * math.pi/180)
            y = y - 2.0 * math.sin(angle * math.pi/180)

            setCharCoordinates(PLAYER_PED,x,y,z-1.0)
        end
        if isKeyDown(keys.airbreak_left)then
            setCharHeading(PLAYER_PED,getCharHeading(PLAYER_PED)+1.0)
            setCameraBehindPlayer()
        end
        if isKeyDown(keys.airbreak_right)then
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
            memory.write(0xB70152 ,minut[0],1)
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
    if imgui.Button("Save Game",imgui.ImVec2(fcommon.GetSize(1))) then
        if isCharOnFoot(PLAYER_PED) then
            activateSaveMenu()
        else
            printHelpString("Can't save while not on foot!")
        end
    end
    imgui.Spacing()
    if imgui.BeginTabBar("Game") then

        if imgui.BeginTabItem("Checkboxes") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)

            --Airbreak Button
            fcommon.CheckBox({name = "Airbreak Mode",var = tairbreak.bool,help_text = "Controls:\nW = Forward\tS = Backward\nA  = Left  \t\tD = Right\nArrow_Up\t = Move up\nArrow_Down = Move Down" ,func = function()
                if tairbreak.bool[0] ==  true then
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
            fcommon.CheckBox({ address = 0x96C009,name = "Free paynspray"})

            fcommon.CheckBox({name = "Keep stuff",var = tgame.keep_stuff,help_text = "Keep stuff after arrest/death" ,func = function()
                if tgame.keep_stuff[0] == false then
                    switchArrestPenalties(true)
                    switchDeathPenalties(true)
                else
                    switchArrestPenalties(false)
                    switchDeathPenalties(false)
                end
            end})
            fcommon.CheckBox({name = "Unlock Interior",var = tgame.unlock_interior,func = function()
                if tgame.unlock_interior[0] == true then
                    for _,name in ipairs(interior_names) do
                        switchEntryExit(name,1)
                        printHelpString("Interiors Unlocked")
                        fcommon.CheatActivated()
                    end
                else
                    for _,name in ipairs(interior_names) do
                        switchEntryExit(name,0)
                        fcommon.CheatDeactivated()
                    end
                end
            end})
            imgui.NextColumn()
            fcommon.CheckBox({ address = 0x969167,name = "Always Midnight"})
            fcommon.CheckBox({ address = 0xB6F065,name = "Widescreen"})
            fcommon.CheckBox({ name = "Screenshot shotcut",var = tgame.ss_shortcut,show_help_popups = true,help_text = "Takes screenshot when (Left Ctrl + S) is pressed."})
            imgui.Columns(1)

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()

            SetCurrentWeekday()
            SetWeather()
            imgui.Spacing()
            fcommon.RadioButton("Game Themes",{"Country","Beach","Ninja","Funhouse"},{0x96917D ,0x969159 ,0x96915C ,0x969176 })
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Menus") then
            if imgui.BeginChild("Game") then
                imgui.Spacing()

                SetTime()

                fcommon.DropDownMenu("FPS",function()
                    imgui.Spacing()
                    if imgui.Checkbox("Show FPS",tfps.bool) then
                    end
                    if imgui.InputInt("FPS Limit",tfps.limit) then
                        memory.write(0xC1704C,(tfps.limit[0]+1),1)
                        memory.write(0xBA6794,1,1)
                    end
                    if tfps.limit[0] < 0 then
                        tfps.limit[0] = 0
                    end
                end)
                fcommon.UpdateAddress({name = "Game Speed",address = 0xB7CB64,size = 4,max = 100,min = 0, is_float =true})
                fcommon.UpdateAddress({name = "Days Passed",address = 0xB79038 ,size = 4,max = 10000})
                fcommon.UpdateAddress({name = "Vehicle Density Multiplier",address = 0x8A5B20,size = 4,max =100})
                fcommon.UpdateAddress({name = "Photographs Taken Number",address = 0xB790B8 ,size = 1,max = 1000})
                fcommon.UpdateAddress({name = "Tags number",address = 0xA9AD74,size = 1,max = 1000})
                fcommon.UpdateAddress({name = "Horseshoes Number",address = 0xB791E4,size = 1,max = 1000})
                fcommon.UpdateAddress({name = "Safehouse Visits Number",address = 0xB79040 ,size = 1,max = 1000})

                fcommon.UpdateAddress({name = "Gravity",address = 0x863984,size = 4,max = 1,min = -1,is_float = true})

                fcommon.DropDownMenu("Cop vehicles",function()
                    imgui.Spacing()
                    AddTownVehicleEntry("Police Ranger",0x8a5a8c,{427,490,528,599},4)
                    AddTownVehicleEntry("Police LS\t   ",0x8a5a90,{596,597,598},4)
                    AddTownVehicleEntry("Police SF\t   ",0x8a5a94,{596,597,598},4)
                    AddTownVehicleEntry("Police LV\t   ",0x8a5a98,{596,597,598},4)
                    AddTownVehicleEntry("Police Bike\t",0x8a5a9c,{523},4)
                end)
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        imgui.EndTabBar()
    end
end

return module
