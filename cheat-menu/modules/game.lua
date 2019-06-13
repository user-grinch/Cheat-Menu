-- This module contains all the functions related to game tab

local module = {}


local ss_shotcut = imgui.ImBool(false)
local keep_stuff = imgui.ImBool()
local unlock_interior = imgui.ImBool()
local current_weather = 0

module.ss_shotcut = ss_shotcut



-- FPS table
local tfps = 
{
    bool = imgui.ImBool(false),
    total = 0,
    time = 0,
    show = 0,
    limit = imgui.ImInt(30)
}

module.tfps = tfps


-- Airbreak table
tairbreak =
{
    bool = imgui.ImBool(false)
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
function add_town_vehicle_entry(title,address,vehicles)
    imgui.Text(title)
    for i = 1,#vehicles,1 do 
        imgui.SameLine()
        if imgui.ImageButton(fvehicles.tvehicles.list[tostring(vehicles[i])],imgui.ImVec2(70,40)) then 
            memory.write(address,vehicles[i],4)
            fcommon.CheatActivated()
        end
    end
end

function module.airbreak_mode()  
    if tairbreak.bool.v then
        if isKeyDown(config.keys.airbreak_up) then 
            x,y,z = getCharCoordinates(PLAYER_PED)
            setCharCoordinates(PLAYER_PED,x,y,z+2.0)
        end
        if isKeyDown(config.keys.airbreak_down) then 
            x,y,z = getCharCoordinates(PLAYER_PED)
            setCharCoordinates(PLAYER_PED,x,y,z-2.0)
        end
        if isKeyDown(config.keys.airbreak_forward) then 
            x,y,z = getCharCoordinates(PLAYER_PED)
            angle = getCharHeading(PLAYER_PED)

            angle =  angle+90
            x = x + 2.0 * math.cos(angle * math.pi/180)
            y = y + 2.0 * math.sin(angle * math.pi/180)

            setCharCoordinates(PLAYER_PED,x,y,z-1.0)
        end
        if isKeyDown(config.keys.airbreak_backward) then 
            x,y,z = getCharCoordinates(PLAYER_PED)
            angle = getCharHeading(PLAYER_PED)

            angle =  angle+90
            x = x - 2.0 * math.cos(angle * math.pi/180)
            y = y - 2.0 * math.sin(angle * math.pi/180)

            setCharCoordinates(PLAYER_PED,x,y,z-1.0)
        end
        if isKeyDown(config.keys.airbreak_left)then 
            setCharHeading(PLAYER_PED,getCharHeading(PLAYER_PED)+1.0)
            setCameraBehindPlayer()
        end
        if isKeyDown(config.keys.airbreak_right)then 
            setCharHeading(PLAYER_PED,getCharHeading(PLAYER_PED)-1.0)
            setCameraBehindPlayer()
        end
    end
end


function set_current_weekday()
    if imgui.BeginMenu("Set Current Weekday") then
        imgui.Spacing()
        imgui.Text("Set Current Weekday")
        imgui.Separator()
        imgui.Spacing()
        local current_day = imgui.ImInt()
        local days = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"}

        current_day.v = memory.read(0xB7014E,1)
        
        for i = 1,#days do
            if imgui.RadioButton(days[i],current_day,i) then
                value = i
                memory.write(0xB7014E,value,1)
            end
        end
        imgui.EndMenu()
    end
end

function set_time()
    if imgui.BeginMenu("Game Time") then
        imgui.Spacing()
        imgui.Text("Game Time")
        imgui.Separator()
        imgui.Spacing()

        local days_passed = imgui.ImInt(memory.read(0xB79038 ,4))
        local hour = imgui.ImInt(memory.read(0xB70153,1))
        local minute = imgui.ImInt(memory.read(0xB70152,1))

        fcommon.check_box({ address = 0x96913B,name = "Faster clock"})
        imgui.SameLine()
        fcommon.check_box({ address = 0x969168,name = "Freeze time"})
        if imgui.InputInt("Current hour",hour) then
            memory.write(0xB70153 ,hour.v,1)
        end

        if imgui.InputInt("Current minute",minute) then
            memory.write(0xB70152 ,minute.v,1)
        end

        if minute.v > 59 then
            hour.v = hour.v + 1
            minute.v = 0
        end

        if hour.v > 23 then
            memory.write(0xB70153 ,0,1)
            current_day = memory.read(0xB7014E,1)
            memory.write(0xB7014E,(current_day+1),1)
            days_passed = memory.read( 0xB79038,4)
            memory.write( 0xB79038,(days_passed+1),4)
        end

        if minute.v < 0 then
            hour.v = hour.v - 1
            minute.v = 59
        end
        
        if hour.v < 0 then
            memory.write(0xB70153 ,23,1)
            current_day = memory.read(0xB7014E,1)
            memory.write(0xB7014E,(current_day-1),1)
            days_passed = memory.read( 0xB79038,4)
            memory.write( 0xB79038,(days_passed-1),4)
        end
        imgui.EndMenu()
    end
end

function set_weather(list)
    if imgui.BeginMenu("Weather") then
        imgui.Text("Select Weather")
        fcommon.information_tooltip("Select current game weather.")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()

        for i = 1,#list,1 do
            local bool ,text
            if i == current_weather then
                text = "     Selected"
                bool = true
            else
                text = ""
                bool = false
            end
            if imgui.MenuItem(list[i],text,bool,true) then
                writeMemory(0xC81320,2,i,false)
                current_weather = i
                fcommon.CheatActivated()
            end
        end
        imgui.EndMenu()
    end
end

function module.game_section()
    imgui.Text("Game")
    imgui.Separator()
    if imgui.Button("Save Game",imgui.ImVec2(155,50)) then
        if isCharOnFoot(PLAYER_PED) then
            activateSaveMenu()
        else
            printHelpString("Can't save while not on foot!")
        end
    end
    imgui.Text("Check Boxes")
    imgui.Separator()
    imgui.Spacing()
    imgui.Columns(2)

    --Airbreak Button
    if imgui.Checkbox("Airbreak Mode",tairbreak.bool) then
        if tairbreak.bool.v ==  true then
            lockPlayerControl(true)
            setCharCollision(PLAYER_PED,false)
            setCameraBehindPlayer()
            fcommon.CheatActivated()
        else 
            lockPlayerControl(false)
            setCharCollision(PLAYER_PED,true)
            fcommon.CheatDeactivated()
        end
    end
    fcommon.information_tooltip("Controls:\nW = Forward\tS = Backward\nA  = Left  \t\tD = Right\nArrow_Up\t = Move up\nArrow_Down = Move Down")
    fcommon.check_box({ address = 0x96C009,name = "Free paynspray"})
    if imgui.Checkbox("Keep stuff after arrest/death",keep_stuff) then
        if keep_stuff.v == false then
            switchArrestPenalties(true)
            switchDeathPenalties(true)
        else
            switchArrestPenalties(false)
            switchDeathPenalties(false)
        end
    end
    imgui.NextColumn()
    fcommon.check_box({ address = 0x969167,name = "Always Midnight"})
    fcommon.check_box({ address = 0xB6F065,name = "Widescreen"})
    fcommon.ValueSwitch({ name = "Screenshot shotcut",var = ss_shotcut,show_help_popups = true,help_text = "Takes screenshot when (Left Ctrl + S) is pressed."})
    if imgui.Checkbox("Unlock Interior",unlock_interior) then 
        if unlock_interior.v == true then
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
    end
    imgui.Columns(1)
    imgui.Spacing()
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()
    
    set_time()

    if imgui.BeginMenu("Game FPS") then
        imgui.Text("Game FPS")
        fcommon.information_tooltip("Frames Per Second")
        imgui.Separator()
        imgui.Spacing()
        if imgui.Checkbox("Show FPS",tfps.bool) then
        end
        if imgui.InputInt("FPS Limit",tfps.limit) then
            memory.write(0xC1704C,(tfps.limit.v+1),1)
            memory.write(0xBA6794,1,1)
        end
        if tfps.limit.v < 0 then
            tfps.limit.v = 0
        end
        imgui.EndMenu()
    end
    fcommon.popup_menu({name = "Game Speed",address = 0xB7CB64,size = 4,max = 100,min = 0, is_float =true})
    fcommon.popup_menu({name = "Days Passed",address = 0xB79038 ,size = 4,max = 10000})
    set_current_weekday()
    fcommon.popup_menu({name = "Vehicle Density Multiplier",address = 0x8A5B20,size = 4,max =100})
    fcommon.popup_menu({name = "Photographs Taken Number",address = 0xB790B8 ,size = 1,max = 1000})
    fcommon.popup_menu({name = "Tags number",address = 0xA9AD74,size = 1,max = 1000})
    fcommon.popup_menu({name = "Horseshoes Number",address = 0xB791E4,size = 1,max = 1000})
    fcommon.popup_menu({name = "Safehouse Visits Number",address = 0xB79040 ,size = 1,max = 1000})

    set_weather({"EXTRASUNNY_LA","SUNNY_LA","EXTRASUNNY_SMOG_LA","SUNNY_SMOG_LA","CLOUDY_LA","SUNNY_SF","EXTRASUNNY_SF","CLOUDY_SF",
                "RAINY_SF","FOGGY_SF","SUNNY_VEGAS","EXTRASUNNY_VEGAS","CLOUDY_VEGAS","EXTRASUNNY_COUNTRYSIDE","SUNNY_COUNTRYSIDE",
                "CLOUDY_COUNTRYSIDE","RAINY_COUNTRYSIDE","EXTRASUNNY_DESERT","SUNNY_DESERT","SANDSTORM_DESERT","UNDERWATER","EXTRACOLOURS_1",
                "EXTRACOLOURS_2"})
    
    if imgui.BeginMenu("Game Themes") then
        fcommon.radio_menu("Game Themes",{"Country","Beach","Ninja","Funhouse"},{0x96917D ,0x969159 ,0x96915C ,0x969176 },"vertical")
        imgui.EndMenu()
    end

    fcommon.popup_menu({name = "Gravity",address = 0x863984,size = 4,max = 10,min = -10})

    if imgui.BeginMenu("Cop Vehicles") then
        imgui.Text("Cop Vehicles")
        imgui.Separator()
        imgui.Spacing()
        add_town_vehicle_entry("Police Ranger",0x8a5a8c,{427,490,528,599})
        add_town_vehicle_entry("Police LS\t   ",0x8a5a90,{596,597,598})
        add_town_vehicle_entry("Police SF\t   ",0x8a5a94,{596,597,598})
        add_town_vehicle_entry("Police LV\t   ",0x8a5a98,{596,597,598})
        add_town_vehicle_entry("Police Bike\t",0x8a5a9c,{523})
        imgui.EndMenu()
    end
end

return module