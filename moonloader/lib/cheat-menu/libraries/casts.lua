local module = {}
local ffi = require 'ffi'
local memory = require 'memory'

ffi.cdef 
[[
    typedef unsigned int CBaseModelInfo;
]]

module.CBaseModelInfo = 
{
}

module.CModelInfo = 
{
    GetModelInfoFromModel = ffi.cast("CBaseModelInfo(*)(unsigned int model)",0x403DA0),
    GetModelInfoFromName = ffi.cast("CBaseModelInfo(*)(const char *modelName, unsigned int pmodel_id_return)",0x4C5940),
    IsTrainModel = ffi.cast('bool(*)(int model)',0x4C5AD0),
    IsVehicleModel = ffi.cast('int(*)(int model)',0x4C5C80),
    ms_modelInfoPtrs = ffi.cast("uintptr_t*",readMemory(0x403DA4 + 3,4,false)) -- FLA Compatible
}

module.CVehicle = 
{
    IsValidModForVehicle = ffi.cast('bool(*)(int model, int pVeh)',0x49B010)
}

function module.CModelInfo.GetNameFromModel(model)
    return ffi.string(ffi.cast("char*", casts.CModelInfo.ms_modelInfoPtrs[tonumber(model)] + 0x32)) or ""
end

function module.CModelInfo.GetModelFromName(name)
    local pmodel = allocateMemory(4)
    
    casts.CModelInfo.GetModelInfoFromName(name,pmodel)

    local model = readMemory(pmodel,4,false)
    freeMemory(pmodel)

    if model > 0 and model < 1000000 and module.CModelInfo.GetNameFromModel(model) ~= "" then
        return model
    else
        return 0
    end
end


module.CTimeCyc = 
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


function module.CBaseModelInfo.GetModelType(model_id)

    local pinfo = module.CModelInfo.GetModelInfoFromModel(model_id)
    local info = readMemory(pinfo,4,false)

    if info ~= 0 then
        return callFunction(readMemory(info+0x10, 4,false), 1, 1, model_id)
    end
end




return module