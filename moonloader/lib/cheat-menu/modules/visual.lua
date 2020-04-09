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

module.tvisual =
{
    car_names                   = imgui.new.bool(true),
    money =     
    {       
        negative_memory         = allocateMemory(4),
        negative                = imgui.new.char[20](fconfig.Get('tvisual.negative',memory.tostring(readMemory(0x58F50A,4,false)))),
        positive_memory         = allocateMemory(4),
        positive                = imgui.new.char[20](fconfig.Get('tvisual.positive',memory.tostring(readMemory(0x58F4C8,4,false)))),
    },
    lock_weather                = imgui.new.bool(fconfig.Get('tvisual.lock_weather',false)),   
    timecyc                     =
    {   timecyc_24_plugin       = getModuleHandle("timecycle24"),
        ambient                 = imgui.new.float[3](),
        ambient_obj             = imgui.new.float[3](),
        sky_top                 = imgui.new.float[3](),
        sky_bottom              = imgui.new.float[3](),
        sun_core                = imgui.new.float[3](),
        sun_corona              = imgui.new.float[3](),
        sun_size                = imgui.new.int(),
        sprite_size             = imgui.new.int(),
        sprite_brightness       = imgui.new.int(),
        shadow_strength         = imgui.new.int(),
        light_shadow_strength   = imgui.new.int(),
        pole_shadow_strength    = imgui.new.int(),
        far_clip                = imgui.new.int(),   
        fog_start               = imgui.new.int(),   
        lights_on_ground_brightness = imgui.new.int(), 
        low_clouds              = imgui.new.float[3](),
        fluffy_clouds           = imgui.new.float[3](),
        water                   = imgui.new.float[4](),
        postfx1                 = imgui.new.float[4](),
        postfx2                 = imgui.new.float[4](),
        cloud_alpha             = imgui.new.int(),
        waterfog_alpha          = imgui.new.int(),
        high_light_min_intensity= imgui.new.int(),
        directional_mult        = imgui.new.int(),
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
                "EXTRACOLOURS 2"
            },
            array               = {},
        },
    },
    zone_names          = imgui.new.bool(true),
}

module.tvisual.timecyc.weather.array  = imgui.new['const char*'][#module.tvisual.timecyc.weather.names](module.tvisual.timecyc.weather.names)
print(module.tvisual.timecyc.timecyc_24_plugin)

------------------------------------------------
-- CTimecyc

local CTimecyc = 
{
    curr_weather          = ffi.cast("short*", 0xC81320),
    next_weather          = ffi.cast("short*", 0xC8131C),
      
    hours                 = ffi.cast("unsigned char*", 0xB70153),  
    mins                  = ffi.cast("unsigned char*", 0xB70152), 
    secs                  = ffi.cast("unsigned short*", 0xB70150),
      
    time_scale            = ffi.cast("unsigned int*", 0xB7015C),
      
    initialise            = ffi.cast("void (__cdecl*)(void)", 0x5BBAC0), 
      
    ambient_red           = ffi.cast("unsigned char*", memory.getuint32(0x560C61)),
    ambient_green         = ffi.cast("unsigned char*", memory.getuint32(0x55F4D6)),
    ambient_blue          = ffi.cast("unsigned char*", memory.getuint32(0x55F4E8)),
      
    ambient_obj_red       = ffi.cast("unsigned char*", memory.getuint32(0x55F4FA)),
	ambient_obj_green     = ffi.cast("unsigned char*", memory.getuint32(0x55F50C)),
    ambient_obj_blue      = ffi.cast("unsigned char*", memory.getuint32(0x55F51E)),
          
	sky_top_red           = ffi.cast("unsigned char*", memory.getuint32(0x55F531)),
	sky_top_green         = ffi.cast("unsigned char*", memory.getuint32(0x55F53D)),
    sky_top_blue          = ffi.cast("unsigned char*", memory.getuint32(0x55F549)),
          
	sky_bottom_red        = ffi.cast("unsigned char*", memory.getuint32(0x55F555)),
	sky_bottom_green      = ffi.cast("unsigned char*", memory.getuint32(0x55F561)),
    sky_bottom_blue       = ffi.cast("unsigned char*", memory.getuint32(0x55F56D)),
          
	sun_core_red          = ffi.cast("unsigned char*", memory.getuint32(0x55F579)),
	sun_core_green        = ffi.cast("unsigned char*", memory.getuint32(0x55F585)),
    sun_core_blue         = ffi.cast("unsigned char*", memory.getuint32(0x55F591)),
          
	sun_corona_red        = ffi.cast("unsigned char*", memory.getuint32(0x55F59D)),
	sun_corona_green      = ffi.cast("unsigned char*", memory.getuint32(0x55F5A9)),
    sun_corona_blue       = ffi.cast("unsigned char*", memory.getuint32(0x55F5B5)),
          
	sun_size              = ffi.cast("unsigned char*", memory.getuint32(0x55F5C0)),
	sprite_size           = ffi.cast("unsigned char*", memory.getuint32(0x55F5D2)),
    sprite_brightness     = ffi.cast("unsigned char*", memory.getuint32(0x55F5E4)),
    
	shadow_strength       = ffi.cast("unsigned char*", memory.getuint32(0x55F5F7)),
	light_shadow_strength = ffi.cast("unsigned char*", memory.getuint32(0x55F603)),
    pole_shadow_strength  = ffi.cast("unsigned char*", memory.getuint32(0x55F60F)),
    
	far_clip                    = ffi.cast("short*", memory.getuint32(0x55F61B)),
    fog_start                   = ffi.cast("short*", memory.getuint32(0x55F62E)),
    lights_on_ground_brightness = ffi.cast("unsigned char*", memory.getuint32(0x55F640)),
    
	low_clouds_red   = ffi.cast("unsigned char*", memory.getuint32(0x55F653)),
	low_clouds_green = ffi.cast("unsigned char*", memory.getuint32(0x55F65F)),
    low_clouds_blue  = ffi.cast("unsigned char*", memory.getuint32(0x55F66B)),
    
	fluffy_clouds_red   = ffi.cast("unsigned char*", memory.getuint32(0x55F677)),
	fluffy_clouds_green = ffi.cast("unsigned char*", memory.getuint32(0x55F683)),
    fluffy_clouds_blue  = ffi.cast("unsigned char*", memory.getuint32(0x55F690)),
    
	water_red   = ffi.cast("unsigned char*", memory.getuint32(0x55F69C)),
	water_green = ffi.cast("unsigned char*", memory.getuint32(0x55F6B0)),
    water_blue  = ffi.cast("unsigned char*", memory.getuint32(0x55F6C3)),
    water_alpha = ffi.cast("unsigned char*", memory.getuint32(0x55F6D6)),
    
	postfx1_red   = ffi.cast("unsigned char*", memory.getuint32(0x55F6E9)),
	postfx1_green = ffi.cast("unsigned char*", memory.getuint32(0x55F6FC)),
	postfx1_blue  = ffi.cast("unsigned char*", memory.getuint32(0x55F70F)),
    postfx1_alpha = ffi.cast("unsigned char*", memory.getuint32(0x55F725)),
    
	postfx2_red   = ffi.cast("unsigned char*", memory.getuint32(0x55F73B)),
	postfx2_green = ffi.cast("unsigned char*", memory.getuint32(0x55F751)),
	postfx2_blue  = ffi.cast("unsigned char*", memory.getuint32(0x55F767)),
    postfx2_alpha = ffi.cast("unsigned char*", memory.getuint32(0x55F77D)),
    
    cloud_alpha = ffi.cast("unsigned char*", memory.getuint32(0x55F793)),
    waterfog_alpha   = ffi.cast("unsigned char*", memory.getuint32(0x55F7B8)),
	high_light_min_intensity = ffi.cast("unsigned char*", memory.getuint32(0x55F7A9)),
	directional_mult = ffi.cast("unsigned char*", memory.getuint32(0x55F7C7)),
}

function module.LockWeather()
    while true do

        if module.tvisual.lock_weather[0] then
            local weather = CTimecyc.curr_weather[0]
            while module.tvisual.lock_weather[0] do
                CTimecyc.curr_weather[0] = weather
                CTimecyc.next_weather[0] = weather
                wait(0)
            end
        end
        wait(0)
    end
end

function GetCurrentHourTimeId(hour)

    local h = CTimecyc.hours[0]

    if hour == 24 then 
        return h
    end

    if h < 5 then  return 0  end
	if h == 5 then  return 1 end
    if h == 6 then  return 2 end
    
	if 7 <= h and h < 12 then  return 3 end
    if 12 <= h and h < 19 then  return 4 end
    
	if h == 19 then  return 5 end
	if h == 20 or h == 21 then  return 6 end
	if h == 22 or h == 23 then  return 7 end
end

function UpdateTimecycData(val)

    module.tvisual.timecyc.ambient[0] = CTimecyc.ambient_red[val]/255
    module.tvisual.timecyc.ambient[1] = CTimecyc.ambient_green[val]/255
    module.tvisual.timecyc.ambient[2] = CTimecyc.ambient_blue[val]/255

    module.tvisual.timecyc.ambient_obj[0] = CTimecyc.ambient_obj_red[val]/255
    module.tvisual.timecyc.ambient_obj[1] = CTimecyc.ambient_obj_green[val]/255
    module.tvisual.timecyc.ambient_obj[2] = CTimecyc.ambient_obj_blue[val]/255

    module.tvisual.timecyc.sky_top[0] = CTimecyc.sky_top_red[val]/255
    module.tvisual.timecyc.sky_top[1] = CTimecyc.sky_top_green[val]/255
    module.tvisual.timecyc.sky_top[2] = CTimecyc.sky_top_blue[val]/255

    module.tvisual.timecyc.sky_bottom[0] = CTimecyc.sky_bottom_red[val]/255
    module.tvisual.timecyc.sky_bottom[1] = CTimecyc.sky_bottom_green[val]/255
    module.tvisual.timecyc.sky_bottom[2] = CTimecyc.sky_bottom_blue[val]/255

    module.tvisual.timecyc.sun_core[0] = CTimecyc.sun_core_red[val]/255
    module.tvisual.timecyc.sun_core[1] = CTimecyc.sun_core_green[val]/255
    module.tvisual.timecyc.sun_core[2] = CTimecyc.sun_core_blue[val]/255

    module.tvisual.timecyc.sun_corona[0] = CTimecyc.sun_corona_red[val]/255
    module.tvisual.timecyc.sun_corona[1] = CTimecyc.sun_corona_green[val]/255
    module.tvisual.timecyc.sun_corona[2] = CTimecyc.sun_corona_blue[val]/255

    module.tvisual.timecyc.sun_size[0] = CTimecyc.sun_size[val]

    module.tvisual.timecyc.sprite_brightness[0] = CTimecyc.sprite_brightness[val]
    module.tvisual.timecyc.sprite_size[0]       = CTimecyc.sprite_size[val]

    module.tvisual.timecyc.shadow_strength[0]       = CTimecyc.shadow_strength[val]
    module.tvisual.timecyc.light_shadow_strength[0] = CTimecyc.light_shadow_strength[val]
    module.tvisual.timecyc.pole_shadow_strength[0]  = CTimecyc.pole_shadow_strength[val]

    module.tvisual.timecyc.fog_start[0]                    = CTimecyc.fog_start[val]
    module.tvisual.timecyc.far_clip[0]                     = CTimecyc.far_clip[val]
    module.tvisual.timecyc.lights_on_ground_brightness[0]  = CTimecyc.lights_on_ground_brightness[val]
   
    module.tvisual.timecyc.low_clouds[0] = CTimecyc.low_clouds_red[val]/255
    module.tvisual.timecyc.low_clouds[1] = CTimecyc.low_clouds_green[val]/255
    module.tvisual.timecyc.low_clouds[2] = CTimecyc.low_clouds_blue[val]/255

    module.tvisual.timecyc.fluffy_clouds[0] = CTimecyc.fluffy_clouds_red[val]/255
    module.tvisual.timecyc.fluffy_clouds[1] = CTimecyc.fluffy_clouds_green[val]/255
    module.tvisual.timecyc.fluffy_clouds[2] = CTimecyc.fluffy_clouds_blue[val]/255

    module.tvisual.timecyc.water[0] = CTimecyc.water_red[val]/255
    module.tvisual.timecyc.water[1] = CTimecyc.water_green[val]/255
    module.tvisual.timecyc.water[2] = CTimecyc.water_blue[val]/255
    module.tvisual.timecyc.water[3] = CTimecyc.water_alpha[val]/255

    module.tvisual.timecyc.postfx1[0] = CTimecyc.postfx1_red[val]/255
    module.tvisual.timecyc.postfx1[1] = CTimecyc.postfx1_green[val]/255
    module.tvisual.timecyc.postfx1[2] = CTimecyc.postfx1_blue[val]/255
    module.tvisual.timecyc.postfx1[3] = CTimecyc.postfx1_alpha[val]/255

    module.tvisual.timecyc.postfx2[0] = CTimecyc.postfx2_red[val]/255
    module.tvisual.timecyc.postfx2[1] = CTimecyc.postfx2_green[val]/255
    module.tvisual.timecyc.postfx2[2] = CTimecyc.postfx2_blue[val]/255
    module.tvisual.timecyc.postfx2[3] = CTimecyc.postfx2_alpha[val]/255

    module.tvisual.timecyc.cloud_alpha[0]  = CTimecyc.cloud_alpha[val]
    module.tvisual.timecyc.waterfog_alpha[0]  = CTimecyc.waterfog_alpha[val]
    module.tvisual.timecyc.high_light_min_intensity[0]  = CTimecyc.high_light_min_intensity[val]
    module.tvisual.timecyc.directional_mult[0]  = CTimecyc.directional_mult[val]
end

function GenerateTimecycFile(hour)

    local file = io.open(getGameDirectory().."/timecyc.dat", "w")
    if hour == 24 then  
        file = io.open(getGameDirectory().."/timecyc_24h.dat", "w")
    end

    for i=0,(#module.tvisual.timecyc.weather.names-1),1 do
        file:write("\n\n//////////// " .. module.tvisual.timecyc.weather.names[i+1].. "\n")
        
        file:write("//\tAmb\t\t\t\t\tAmb Obj \t\t\t\tDir \t\t\t\t\tSky top\t\t\t\tSky bot\t\t\t\tSunCore\t\t\t\t\tSunCorona\t\t\tSunSz\tSprSz\tSprBght\t\tShdw\tLightShd\tPoleShd\t\tFarClp\t\tFogSt\tLightOnGround\tLowCloudsRGB\tBottomCloudRGB\t\tWaterRGBA\t\t\t\tARGB1\t\t\t\t\tARGB2\t\t\tCloudAlpha\t\tIntensityLimit\t\tWaterFogAlpha\tDirMult \n\n")
        
        for j=0,(hour-1),1 do

            if hour == 24 then 
				if (j >= 12) then
					file:write(string.format("// %s PM\n",j))
				else
					file:write(string.format("// %s AM\n",j))
				end
            else
				if j == 0 then file:write("// Midnight\n") end
				if j == 1 then file:write("// 5 AM\n") end
				if j == 2 then file:write("// 6 AM\n") end
				if j == 3 then file:write("// 7 AM\n") end
				if j == 4 then file:write("// Midday\n") end
				if j == 5 then file:write("// 7 PM\n") end
				if j == 6 then file:write("// 8 PM\n") end
				if j == 7 then file:write("// 10 PM\n") end
            end 

            local val = 23 * j + i

            file:write(
				string.format(
					"\t%d %d %d \t\t" .. -- AmbRGB
					"\t%d %d %d \t\t" .. -- AmbObjRGB
					"\t%d %d %d \t\t" .. -- DirRGB (unused?)
					"\t%d %d %d \t\t" .. -- SkyTopRGB
					"\t%d %d %d \t\t" .. -- SkyBotRGB
					"\t%d %d %d \t\t" .. -- SunCore RGB
					"\t%d %d %d \t\t" .. -- SunCorona RGB
					"\t%.1f\t\t%.1f\t\t%.1f\t\t" .. -- SunSz, SpriteSz, SpriteBrightness
					"\t%d \t%d \t\t\t%d\t\t" .. -- ShadStrenght, LightShadStreght, PoleShadStrenght
					"\t%.1f\t\t%.1f\t\t%.1f\t\t" .. -- fFarClip, fFogStart, fLightsOnGroundBrightness
					"\t\t%d %d %d\t" .. -- LowCloudsRGB
					"\t%d %d %d\t\t" .. -- FluffyCloudsRGB
					"\t%d %d %d %d\t\t" .. -- WaterRGBA
					"\t%d %d %d %d\t\t" .. -- PostFx1ARGB
					"\t%d %d %d %d" .. -- PostFx2ARGB
					"\t%d\t\t\t\t%d\t\t\t\t\t%d\t\t\t\t%.2f\t\t\n", -- CloudAlpha HiLiMinIntensity WaterFogAlpha DirectionalMult
					CTimecyc.ambient_red[val], CTimecyc.ambient_green[val],	CTimecyc.ambient_blue[val],
					CTimecyc.ambient_obj_red[val], CTimecyc.ambient_obj_green[val],	CTimecyc.ambient_obj_blue[val],
					255, 255, 255,
					CTimecyc.sky_top_red[val], CTimecyc.sky_top_green[val], CTimecyc.sky_top_blue[val],
					CTimecyc.sky_bottom_red[val], CTimecyc.sky_bottom_green[val], CTimecyc.sky_bottom_blue[val],
					CTimecyc.sun_core_red[val],CTimecyc.sun_core_green[val],CTimecyc.sun_core_blue[val],
					CTimecyc.sun_corona_red[val], CTimecyc.sun_corona_green[val], CTimecyc.sun_corona_blue[val],
					(CTimecyc.sprite_size[val] - 0.5) / 10.0,(CTimecyc.sprite_size[val] - 0.5) / 10.0,(CTimecyc.sprite_brightness[val] - 0.5) / 10.0,
					CTimecyc.shadow_strength[val],CTimecyc.light_shadow_strength[val],CTimecyc.pole_shadow_strength[val],
					CTimecyc.far_clip[val],CTimecyc.fog_start[val],	(CTimecyc.lights_on_ground_brightness[val] - 0.5) / 10.0,
					CTimecyc.low_clouds_red[val],CTimecyc.low_clouds_green[val],CTimecyc.low_clouds_blue[val],
					CTimecyc.fluffy_clouds_red[val],CTimecyc.fluffy_clouds_green[val],CTimecyc.fluffy_clouds_blue[val],
					CTimecyc.water_red[val],CTimecyc.water_blue[val],CTimecyc.water_blue[val],CTimecyc.water_alpha[val],
					CTimecyc.postfx1_alpha[val],CTimecyc.postfx1_red[val],CTimecyc.postfx1_green[val],CTimecyc.postfx1_blue[val],
					CTimecyc.postfx2_alpha[val],CTimecyc.postfx2_red[val],CTimecyc.postfx2_green[val],CTimecyc.postfx2_blue[val],
					CTimecyc.cloud_alpha[val],CTimecyc.high_light_min_intensity[val], CTimecyc.waterfog_alpha[val],	CTimecyc.directional_mult[val] / 100.0
				)
			)
        end
    end
    io.close(file)
end
------------------------------------------------

-- Main function
function module.VisualMain()
    fcommon.Tabs("Visual",{"Checkboxes","Menus","Timecyc editor"},{
        function()
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxValue('Armour border',0x589123)
            fcommon.CheckBoxValue('Armour percentage',0x589125)
            fcommon.CheckBoxValue('Breath border',0x589207)
            fcommon.CheckBoxValue('Breath percentage',0x589209)
            fcommon.CheckBoxFunc('Display car names',module.tvisual.car_names,
            function()    
                displayCarNames(module.tvisual.car_names[0]) 
                fconfig.Set(fconfig.tconfig.misc_data,"Display Car Names",module.tvisual.car_names[0])
                if module.tvisual.car_names[0] then 
                    fcommon.CheatActivated() 
                else 
                    fcommon.CheatDeactivated() 
                end
            end)
            fcommon.CheckBoxFunc('Display zone names',module.tvisual.zone_names,
            function()     
                displayZoneNames(module.tvisual.zone_names[0])
                fconfig.Set(fconfig.tconfig.misc_data,"Display Zone Names",module.tvisual.zone_names[0])
                if module.tvisual.zone_names[0] then 
                    fcommon.CheatActivated() 
                else 
                    fcommon.CheatDeactivated() 
                end
            end)
            fcommon.CheckBoxValue('Enable hud',0xBA6769)

            imgui.NextColumn()

            fcommon.CheckBoxValue('Enable radar',0xBA676C,nil,0,2)
            fcommon.CheckBoxValue('Gray radar',0xA444A4)
            fcommon.CheckBoxValue('Health border',0x589353)
            fcommon.CheckBoxValue('Health percentage',0x589355)
            fcommon.CheckBoxValue('Hide wanted level',0x58DD1B,nil,0x90)
            fcommon.CheckBoxVar('Lock weather',module.tvisual.lock_weather)
            imgui.Columns(1)
        end,
        function()
            fcommon.RadioButtonFunc("Debt color",{"Red (Default)","Green","Purple","Light purple","White","Black","Yellow","Pink","Gray","Dark red"},{0,1,2,3,4,5,6,7,8,9},0x58F4D4)
            fcommon.RadioButtonFunc("Money color",{"Red","Green (Default)","Purple","Light purple","White","Black","Yellow","Pink","Gray","Dark red"},{0,1,2,3,4,5,6,7,8,9},0x58F492)
            fcommon.RadioButtonFunc("Money font outline",{"No outline","Thin outline","Default outline"},{0,1,2},0x58F58D)
            fcommon.RadioButtonFunc("Money font style",{"Style 1","Style 2","Default style"},{1,2,3},0x58F57F)
            fcommon.DropDownMenu("Money text format",function()
                if imgui.InputText("Positive",module.tvisual.money.positive,ffi.sizeof(module.tvisual.money.positive)) then 
                    ffi.copy(ffi.cast("char*", module.tvisual.money.positive_memory), ffi.string(module.tvisual.money.positive))
                    writeMemory(0x58F4C8,4,module.tvisual.money.positive_memory,false)

                end
                if imgui.InputText("Negative",module.tvisual.money.negative,ffi.sizeof(module.tvisual.money.negative)) then

                    ffi.copy(ffi.cast("char*", module.tvisual.money.negative_memory), ffi.string(module.tvisual.money.negative))
                    writeMemory(0x58F50A,4,module.tvisual.money.negative_memory,false)

                end      
                if imgui.Button("Reset to default",imgui.ImVec2(fcommon.GetSize(1))) then

                    ffi.copy(ffi.cast("char(*)", module.tvisual.money.negative_memory), "-$%07d")
                    imgui.StrCopy(module.tvisual.money.negative,"-$%07d",ffi.sizeof(module.tvisual.money.negative))
                    writeMemory(0x58F50A,4,module.tvisual.money.negative_memory,false)

                    ffi.copy(ffi.cast("char(*)", module.tvisual.money.positive_memory), "$%08d")
                    imgui.StrCopy(module.tvisual.money.positive,"$%08d",ffi.sizeof(module.tvisual.money.positive))
                    writeMemory(0x58F4C8,4,module.tvisual.money.positive_memory,false)
            
                end                    
            end)

            --fcommon.UpdateAddress({ name = 'Radio station text color',address = 0xBAB24C ,size = 4,min=-999,default = 40,max = 999})
            fcommon.UpdateAddress({ name = 'Radar Height',address = 0x866B74,size = 4,min=0,default = 76,max = 999,is_float = true})
            fcommon.UpdateAddress({ name = 'Radar Width',address = 0x866B78,size = 4,min=0,default = 94,max = 999,is_float = true})
            fcommon.UpdateAddress({ name = 'Radar X position',address = 0x858A10,size = 4,min=-999,default = 40,max = 999,is_float = true,help_text = "Changes radar vertical position"})
            fcommon.UpdateAddress({ name = 'Radar Y position',address = 0x866B70,size = 4,min=-999,default = 104,max = 999,is_float = true,help_text = "Changes radar horizantal position"})
            fcommon.UpdateAddress({ name = 'Radar zoom',address = 0xA444A3,size = 1,min=0,default = 0,max = 170})
            fcommon.RadioButtonFunc("Wanted star border",{"No border","Default","Bold border"},{0,1,2},0x58DD41)
            fcommon.RadioButtonFunc("Wanted star color",{"Red","Green","Purple","Light purple","White","Black","Yellow (Default)","Pink","Gray","Dark red"},{0,1,2,3,4,5,6,7,8,9},0x58DDC9)
            fcommon.UpdateAddress({ name = 'Wanted star Y position',address = 0x858CCC,size = 4,is_float = true,min=-500,default = 12,max = 500})
            callFunction(0x71D730,1,1,200.0)
        end,
        function()
            if module.tvisual.timecyc.timecyc_24_plugin ~= 0 then
                HOUR = 24
            else
                HOUR = 8
            end

            local val = 23 * GetCurrentHourTimeId(HOUR) + CTimecyc.curr_weather[0]
            UpdateTimecycData(val)

            imgui.SetNextItemWidth(imgui.GetWindowContentRegionWidth()/1.7)
            if imgui.Button("Reset timecyc",imgui.ImVec2(fcommon.GetSize(2))) then
                CTimecyc.initialise()
                printHelpString("Timecyc reset")
            end
            imgui.SameLine()
            if imgui.Button("Generate timecyc file",imgui.ImVec2(fcommon.GetSize(2))) then
                GenerateTimecycFile(HOUR)
                printHelpString("File generated")
            end
            imgui.Spacing()
            local weather = imgui.new.int(CTimecyc.curr_weather[0])
            if imgui.Combo("Current weather", weather,module.tvisual.timecyc.weather.array,#module.tvisual.timecyc.weather.names) then
                if module.tvisual.lock_weather[0] then
                    printHelpString("Weather locked")
                else
                    CTimecyc.curr_weather[0] = weather[0]
                    printHelpString("Current weather set")
                end
            end

            weather = imgui.new.int(CTimecyc.next_weather[0])
            if imgui.Combo("Next weather", weather,module.tvisual.timecyc.weather.array,#module.tvisual.timecyc.weather.names) then
                if module.tvisual.lock_weather[0] then
                    printHelpString("Weather locked")
                else
                    CTimecyc.next_weather[0] = weather[0]
                    printHelpString("Next weather set")
                end
            end
            imgui.Spacing()

            fcommon.Tabs("Timecyc sub tab",{"Colors","Misc"},{
            function()
                
                if imgui.ColorEdit3("Ambient",module.tvisual.timecyc.ambient) then
                    CTimecyc.ambient_red[val]   = module.tvisual.timecyc.ambient[0]*255
                    CTimecyc.ambient_green[val] = module.tvisual.timecyc.ambient[1]*255
                    CTimecyc.ambient_blue[val]  = module.tvisual.timecyc.ambient[2]*255
                end
                fcommon.InformationTooltip("Ambient color on static map objects")

                if imgui.ColorEdit3("Ambient obj",module.tvisual.timecyc.ambient_obj) then
                    CTimecyc.ambient_obj_red[val]   = module.tvisual.timecyc.ambient_obj[0]*255
                    CTimecyc.ambient_obj_green[val] = module.tvisual.timecyc.ambient_obj[1]*255
                    CTimecyc.ambient_obj_blue[val]  = module.tvisual.timecyc.ambient_obj[2]*255
                end
                fcommon.InformationTooltip("Ambient color on dynamic map objects")

                if imgui.ColorEdit3("Fluffy clouds",module.tvisual.timecyc.fluffy_clouds) then
                    CTimecyc.fluffy_clouds_red[val]   = module.tvisual.timecyc.fluffy_clouds[0]*255
                    CTimecyc.fluffy_clouds_green[val] = module.tvisual.timecyc.fluffy_clouds[1]*255
                    CTimecyc.fluffy_clouds_blue[val]  = module.tvisual.timecyc.fluffy_clouds[2]*255
                end

                if imgui.ColorEdit3("Low clouds",module.tvisual.timecyc.low_clouds) then
                    CTimecyc.low_clouds_red[val]   = module.tvisual.timecyc.low_clouds[0]*255
                    CTimecyc.low_clouds_green[val] = module.tvisual.timecyc.low_clouds[1]*255
                    CTimecyc.low_clouds_blue[val]  = module.tvisual.timecyc.low_clouds[2]*255
                end

                if imgui.ColorEdit4("Postfx 1",module.tvisual.timecyc.postfx1) then
                    CTimecyc.postfx1_red[val]   = module.tvisual.timecyc.postfx1[0]*255
                    CTimecyc.postfx1_green[val] = module.tvisual.timecyc.postfx1[1]*255
                    CTimecyc.postfx1_blue[val]  = module.tvisual.timecyc.postfx1[2]*255
                    CTimecyc.postfx1_alpha[val] = module.tvisual.timecyc.postfx1[3]*255
                end
                fcommon.InformationTooltip("Color correction 1")

                if imgui.ColorEdit4("Postfx 2",module.tvisual.timecyc.postfx2) then
                    CTimecyc.postfx2_red[val]   = module.tvisual.timecyc.postfx2[0]*255
                    CTimecyc.postfx2_green[val] = module.tvisual.timecyc.postfx2[1]*255
                    CTimecyc.postfx2_blue[val]  = module.tvisual.timecyc.postfx2[2]*255
                    CTimecyc.postfx2_alpha[val] = module.tvisual.timecyc.postfx2[3]*255
                end
                fcommon.InformationTooltip("Color correction 2")

                if imgui.ColorEdit3("Sky bottom",module.tvisual.timecyc.sky_bottom) then
                    CTimecyc.sky_bottom_red[val]   = module.tvisual.timecyc.sky_bottom[0]*255
                    CTimecyc.sky_bottom_green[val] = module.tvisual.timecyc.sky_bottom[1]*255
                    CTimecyc.sky_bottom_blue[val]  = module.tvisual.timecyc.sky_bottom[2]*255
                end
                
                if imgui.ColorEdit3("Sun core",module.tvisual.timecyc.sun_core) then
                    CTimecyc.sun_core_red[val]   = module.tvisual.timecyc.sun_core[0]*255
                    CTimecyc.sun_core_green[val] = module.tvisual.timecyc.sun_core[1]*255
                    CTimecyc.sun_core_blue[val]  = module.tvisual.timecyc.sun_core[2]*255
                end

                if imgui.ColorEdit3("Sun corona",module.tvisual.timecyc.sun_corona) then
                    CTimecyc.sun_corona_red[val]   = module.tvisual.timecyc.sun_corona[0]*255
                    CTimecyc.sun_corona_green[val] = module.tvisual.timecyc.sun_corona[1]*255
                    CTimecyc.sun_corona_blue[val]  = module.tvisual.timecyc.sun_corona[2]*255
                end

                if imgui.ColorEdit3("Sky top",module.tvisual.timecyc.sky_top) then
                    CTimecyc.sky_top_red[val]   = module.tvisual.timecyc.sky_top[0]*255
                    CTimecyc.sky_top_green[val] = module.tvisual.timecyc.sky_top[1]*255
                    CTimecyc.sky_top_blue[val]  = module.tvisual.timecyc.sky_top[2]*255
                end

                if imgui.ColorEdit4("Water",module.tvisual.timecyc.water) then
                    CTimecyc.water_red[val]   = module.tvisual.timecyc.water[0]*255
                    CTimecyc.water_green[val] = module.tvisual.timecyc.water[1]*255
                    CTimecyc.water_blue[val]  = module.tvisual.timecyc.water[2]*255
                    CTimecyc.water_alpha[val]  = module.tvisual.timecyc.water[3]*255
                end
            end,
            function()

                imgui.PushItemWidth(imgui.GetWindowContentRegionWidth()/2)
                if imgui.SliderInt("Cloud alpha", module.tvisual.timecyc.cloud_alpha, 0, 255) then
                    CTimecyc.cloud_alpha[val]   = module.tvisual.timecyc.cloud_alpha[0]
                end

                if imgui.SliderInt("Directional mult", module.tvisual.timecyc.directional_mult, 0, 255) then
                    CTimecyc.directional_mult[val]   = module.tvisual.timecyc.directional_mult[0]
                end
                fcommon.InformationTooltip("Direct light on peds & vehicles")

                if imgui.SliderInt("Far clip", module.tvisual.timecyc.far_clip, 0, 2000) then
                    CTimecyc.far_clip[val]   = module.tvisual.timecyc.far_clip[0]
                end
                fcommon.InformationTooltip("Visibility range")

                if imgui.SliderInt("High light min intensity", module.tvisual.timecyc.high_light_min_intensity, 0, 255) then
                    CTimecyc.high_light_min_intensity[val]   = module.tvisual.timecyc.high_light_min_intensity[0]
                end
                fcommon.InformationTooltip("Intensity limit for PS2 radiosity effect")

                if imgui.SliderInt("Fog start", module.tvisual.timecyc.fog_start, 0, 2000) then
                    CTimecyc.fog_start[val]   = module.tvisual.timecyc.fog_start[0]
                end

                if imgui.SliderInt("Light on ground brightness", module.tvisual.timecyc.lights_on_ground_brightness, 0, 255) then
                    CTimecyc.lights_on_ground_brightness[val]   = module.tvisual.timecyc.lights_on_ground_brightness[0]
                end

                if imgui.SliderInt("Light shadow strength", module.tvisual.timecyc.light_shadow_strength, 0, 255) then
                    CTimecyc.light_shadow_strength[val]   = module.tvisual.timecyc.light_shadow_strength[0]
                end

                if imgui.SliderInt("Pole shadow strength", module.tvisual.timecyc.pole_shadow_strength, 0, 255) then
                    CTimecyc.pole_shadow_strength[val]   = module.tvisual.timecyc.pole_shadow_strength[0]
                end

                if imgui.SliderInt("Shadow strength", module.tvisual.timecyc.shadow_strength, 0, 255) then
                    CTimecyc.shadow_strength[val]   = module.tvisual.timecyc.shadow_strength[0]
                end

                if imgui.SliderInt("Sprite brightness", module.tvisual.timecyc.sprite_brightness, 0, 127) then
                    CTimecyc.sprite_brightness[val]   = module.tvisual.timecyc.sprite_brightness[0]
                end

                if imgui.SliderInt("Sprite size", module.tvisual.timecyc.sprite_size, 0, 127) then
                    CTimecyc.sprite_size[val]   = module.tvisual.timecyc.sprite_size[0]
                end
                
                if imgui.SliderInt("Sun size", module.tvisual.timecyc.sun_size, 0, 127) then
                    CTimecyc.sun_size[val]   = module.tvisual.timecyc.sun_size[0]
                end

                if imgui.SliderInt("Water fog alpha", module.tvisual.timecyc.waterfog_alpha, 0, 255) then
                    CTimecyc.waterfog_alpha[val]   = module.tvisual.timecyc.waterfog_alpha[0]
                end
            end})

        end
    })
end

return module
