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

module.tconfig =
{
    memory_data = {},
    misc_data   = {},
    path        = tcheatmenu.dir ..  "json/config.json",
    reset       = false,
    read        = fcommon.LoadJson("config"),
    stat_data   = {},
    save_config = true,
}


function module.Get(s,default,t)
    if t == nil then t = module.tconfig.read end
    if t == nil then return default end
    
    local status, rtn = pcall(function()
        for key in s:gmatch('[^.]+') do
            if t[key] == nil then return default end
            t = t[key]
        end
  
        if t == nil then
            return default
        else
            return t
        end
    end)
 
    if status then
        return rtn
    else
        print(string.format("Error occured while getting value of %s. Returning default value %s",s,tostring(default)))
        return default
    end
end

function module.Set(t,path,value)

    local x = 0
    for key in path:gmatch('[^.]+') do
        x = x + 1
    end

    local y = 0
    for key in path:gmatch('[^.]+') do
        y = y + 1
        if x == y then
            t[key] = value
        else
            if t[key] == nil then t[key] = {} end
            t = t[key]
        end
    end
end

module.tconfig.memory_data = module.Get('tmemory_data',{})
module.tconfig.misc_data = module.Get('tmisc_data',{})
module.tconfig.stat_data = module.Get('tstat_data',{})

function module.SetConfigData()
    for k,v in pairs(module.tconfig.memory_data) do
        fcommon.RwMemory(tonumber(k),v[1],v[2],nil,v[3],v[4])
    end

    for k,v in pairs(module.tconfig.stat_data) do
        setFloatStat(tonumber(k),v)
    end
    
    setCharHealth(PLAYER_PED,module.Get("tmisc_data.Health",getCharHealth(PLAYER_PED)))
    
    if getCharArmour(PLAYER_PED) < module.Get("tmisc_data.Armour",getCharArmour(PLAYER_PED)) then
        addArmourToChar(PLAYER_PED,module.Get("tmisc_data.Armour",0)-getCharArmour(PLAYER_PED))
    else
        damageChar(PLAYER_PED,getCharArmour(PLAYER_PED)-module.Get("tmisc_data.Armour",0),true)
    end

    -- Never Wanted
    if readMemory(0x969171,1,false) == 0 and  module.Get("tmisc_data.Never Wanted",0) == true then
        callFunction(0x4396C0,1,0,false)
    end

    -- Body
    local body = module.Get("tmisc_data.Body",nil)
    if body == 1 then
        callFunction(0x439110,1,1,false)
    end
    if body == 2 then
        callFunction(0x439190,1,1,false)
        callFunction(0x439150,1,1,false)
    end
    if body == 3 then
        callFunction(0x439190,1,1,false)
    end

    -- Car & zone names
    local car_name = module.Get("tmisc_data.Display Car Names",nil)

    if car_name ~= nil then
        displayCarNames(car_name)
        fvisual.tvisual.car_names[0] = car_name
    end

    local zone_name = module.Get("tmisc_data.Display Zone Names",nil)

    if zone_name ~= nil then
        displayZoneNames(zone_name)
        fvisual.tvisual.zone_names[0] = zone_name
    end
    
end

function module.Write()

    if not module.tconfig.save_config then return end

    local write_table = {}
    if not module.tconfig.reset then
        write_table =
        {
            tanimation =
            {
                fighting =
                {
                    selected = fanimation.tanimation.fighting.selected[0],
                },
                loop             = fanimation.tanimation.loop[0],
                ped              = fanimation.tanimation.ped[0],
                secondary        = fanimation.tanimation.secondary[0],
                walking =
                {
                    selected = fanimation.tanimation.walking.selected[0],
                },
            },
            tcheatmenu =
            {
                coord =
                {
                    X = tcheatmenu.coord.X,
                    Y = tcheatmenu.coord.Y,
                },
                current_menu = tcheatmenu.current_menu,
                size =
                {
                    X = tcheatmenu.size.X,
                    Y = tcheatmenu.size.Y,
                },
            },
            tgame =
            {
                camera            =
                {    
                    fov             = fgame.tgame.camera.fov[0],   
                    movement_speed  = fgame.tgame.camera.movement_speed[0],
                },
                disable_cheats      = fgame.tgame.disable_cheats[0],
                disable_help_popups = fgame.tgame.disable_help_popups[0],
                disable_replay      = fgame.tgame.disable_replay[0],
                forbidden_area_wanted_level = fgame.tgame.forbidden_area_wanted_level[0],
                fps_limit           = fgame.tgame.fps_limit[0],
                ghost_cop_cars      = fgame.tgame.ghost_cop_cars[0],
                keep_stuff          = fgame.tgame.keep_stuff[0],
                freeze_mission_timer= fgame.tgame.freeze_mission_timer[0],
                freeze_time         = fgame.tgame.freeze_time[0], 
                object_spawner      = 
                {
                    set_player_coord= fgame.tgame.object_spawner.set_player_coord[0],
                },
                random_cheats       = 
                {
                    cheat_activate_timer   = fgame.tgame.random_cheats.cheat_activate_timer[0], 
                    cheat_deactivate_timer = fgame.tgame.random_cheats.cheat_deactivate_timer[0], 
                    checkbox               = fgame.tgame.random_cheats.checkbox[0], 
                    disabled_cheats        = fgame.tgame.random_cheats.disabled_cheats, 
                    disable_cheat_checkbox = fgame.tgame.random_cheats.disable_cheat_checkbox[0], 
                },
                script_manager      = 
                {
                    scripts         = fgame.tgame.script_manager.scripts,
                },
                solid_water         = fgame.tgame.solid_water[0],
                ss_shortcut         = fgame.tgame.ss_shortcut[0],
                sync_system_time    = fgame.tgame.sync_system_time[0],
            },
            tmemory_data = module.tconfig.memory_data,
            tmisc_data   = module.tconfig.misc_data,
            tstat_data   = module.tconfig.stat_data,
            tmenu =
            {
                auto_update_check   = fmenu.tmenu.auto_update_check[0],
                auto_reload         = fmenu.tmenu.auto_reload[0],
                fast_load_images    = fmenu.tmenu.fast_load_images[0],
                font				=
                {
                    selected	    = fmenu.tmenu.font.selected,
                    size  		    = fmenu.tmenu.font.size[0],
                },
                get_beta_updates    = fmenu.tmenu.get_beta_updates[0],
                hot_keys = 
                {
                    asc_key                 = fmenu.tmenu.hot_keys.asc_key,
                    camera_mode             = fmenu.tmenu.hot_keys.camera_mode,
                    camera_mode_forward     = fmenu.tmenu.hot_keys.camera_mode_forward,
                    camera_mode_backward    = fmenu.tmenu.hot_keys.camera_mode_backward,
                    camera_mode_left        = fmenu.tmenu.hot_keys.camera_mode_left,
                    camera_mode_right       = fmenu.tmenu.hot_keys.camera_mode_right,
                    camera_mode_slow        = fmenu.tmenu.hot_keys.camera_mode_slow,
                    camera_mode_fast        = fmenu.tmenu.hot_keys.camera_mode_fast,
                    camera_mode_up          = fmenu.tmenu.hot_keys.camera_mode_up,
                    camera_mode_down        = fmenu.tmenu.hot_keys.camera_mode_down,
                    command_window          = fmenu.tmenu.hot_keys.command_window,
                    menu_open               = fmenu.tmenu.hot_keys.menu_open,
                    quick_screenshot        = fmenu.tmenu.hot_keys.quick_screenshot,
                    quick_teleport          = fmenu.tmenu.hot_keys.quick_teleport,
                },
                lock_player         = fmenu.tmenu.lock_player[0],
                overlay = 
                {           
                    coordinates     = fmenu.tmenu.overlay.coordinates[0],
                    fps             = fmenu.tmenu.overlay.fps[0],
                    health          = fmenu.tmenu.overlay.health[0],
                    location        = fmenu.tmenu.overlay.location[0],
                    position_index  = fmenu.tmenu.overlay.position_index[0],
                    pos_x           = fmenu.tmenu.overlay.pos_x[0],
                    pos_y           = fmenu.tmenu.overlay.pos_y[0],
                    show            = fmenu.tmenu.overlay.show[0],
                    speed           = fmenu.tmenu.overlay.speed[0],
                    transparent_bg  = fmenu.tmenu.overlay.transparent_bg[0],
                },
                show_tooltips       = fmenu.tmenu.show_tooltips[0],
                show_crash_message  = fmenu.tmenu.show_crash_message[0],
            },
            tstyle =
            {
                font_size       = fstyle.tstyle.font_size,
                selected_name   = fstyle.tstyle.selected_name,
            },
            tped  =
            {
                gang = 
                {
                    wars = fped.tped.gang.wars[0],
                },
                ped_health_display = fped.tped.ped_health_display[0],
            },
            tplayer =
            {
                aimSkinChanger      = fplayer.tplayer.aimSkinChanger[0],
                cjBody              = fplayer.tplayer.cjBody[0],
                enable_saving           = fplayer.tplayer.enable_saving[0],
                god                 = fplayer.tplayer.god[0],
                invisible           = fplayer.tplayer.invisible[0],
                keep_position       = fplayer.tplayer.keep_position[0],
                save_data           = fplayer.tplayer.save_data,
            },
            tteleport =
            {
                insert_coords  = fteleport.tteleport.insert_coords[0],
                shortcut       = fteleport.tteleport.shortcut[0],
            },
            tvehicle =
            {
                aircraft =
                {
                    camera       = fvehicle.tvehicle.aircraft.camera[0],
                    index        = fvehicle.tvehicle.aircraft.index,
                    spawn_in_air = fvehicle.tvehicle.aircraft.spawn_in_air[0],
                },
                car_engine  = fvehicle.tvehicle.car_engine[0],
                color =
                {
                    show_all = fvehicle.tvehicle.color.show_all[0],
                },
                components = 
                {
                    enable_saving = fvehicle.tvehicle.components.enable_saving[0],
                    save_data     = fvehicle.tvehicle.components.save_data,
                },
                apply_material_filter = fvehicle.tvehicle.apply_material_filter[0],
                first_person_camera = 
                {
                    bool            = fvehicle.tvehicle.first_person_camera.bool[0],
                },
                gxt_name_table   = fvehicle.tvehicle.gxt_name_table,
                heavy            = fvehicle.tvehicle.heavy[0],
                invisible_car    = fvehicle.tvehicle.invisible_car[0],
                license_plate_text = ffi.string(fvehicle.tvehicle.license_plate_text),
                lights           = fvehicle.tvehicle.lights[0],
                lock_speed       = fvehicle.tvehicle.lock_speed[0],
                neon             = 
                {
                    checkbox     = fvehicle.tvehicle.neon.checkbox[0],
                },
                no_damage        = fvehicle.tvehicle.no_damage[0],
                no_vehicles      = fvehicle.tvehicle.no_vehicles[0],
                paintjobs  = 
                {
                    enable_saving    = fvehicle.tvehicle.paintjobs.enable_saving[0],
                    save_data        = fvehicle.tvehicle.paintjobs.save_data,
                },
                rainbow_colors    = 
                {   bool         = fvehicle.tvehicle.rainbow_colors.bool[0],
                    traffic      = fvehicle.tvehicle.rainbow_colors.traffic[0],
                    speed       = fvehicle.tvehicle.rainbow_colors.speed[0],
                },
                rainbow_neons    = 
                {   bool         = fvehicle.tvehicle.rainbow_neons.bool[0],
                    traffic      = fvehicle.tvehicle.rainbow_neons.traffic[0],
                    speed    = fvehicle.tvehicle.rainbow_neons.speed[0],
                },
                spawn_inside     = fvehicle.tvehicle.spawn_inside[0],
                speed            = fvehicle.tvehicle.speed[0],
                stay_on_bike     = fvehicle.tvehicle.stay_on_bike[0],
                visual_damage    = fvehicle.tvehicle.visual_damage[0],
                unlimited_nitro  = fvehicle.tvehicle.unlimited_nitro[0],
                watertight_car   = fvehicle.tvehicle.watertight_car[0],
            },
            tvisual =
            {
                lock_weather     = fvisual.tvisual.lock_weather[0],
                disable_motion_blur = fvisual.tvisual.disable_motion_blur[0],
                radio_channel_names = fvisual.tvisual.radio_channel_names[0],
            },
            tweapon =
            {
                auto_aim          = fweapon.tweapon.auto_aim[0],
                ammo_count         = fweapon.tweapon.ammo_count[0],
                gang               = 
                {
                    enable_weapon_editor = fweapon.tweapon.gang.enable_weapon_editor[0],
                },
                fast_reload       = fweapon.tweapon.fast_reload[0],
                max_accuracy      = fweapon.tweapon.max_accuracy[0], 
                max_ammo_clip     = fweapon.tweapon.max_ammo_clip[0], 
                max_move_speed    = fweapon.tweapon.max_move_speed[0], 
                huge_damage       = fweapon.tweapon.huge_damage[0],
                gang_weapons      =
                {
                    {fweapon.tweapon.gang.used_weapons[1][1],fweapon.tweapon.gang.used_weapons[1][2],fweapon.tweapon.gang.used_weapons[1][3]}, -- Ballas
                    {fweapon.tweapon.gang.used_weapons[2][1],fweapon.tweapon.gang.used_weapons[2][2],fweapon.tweapon.gang.used_weapons[2][3]}, -- Grove
                    {fweapon.tweapon.gang.used_weapons[3][1],fweapon.tweapon.gang.used_weapons[3][2],fweapon.tweapon.gang.used_weapons[3][3]}, -- Vagos
                    {fweapon.tweapon.gang.used_weapons[4][1],fweapon.tweapon.gang.used_weapons[4][2],fweapon.tweapon.gang.used_weapons[4][3]}, -- SF Rifa
                    {fweapon.tweapon.gang.used_weapons[5][1],fweapon.tweapon.gang.used_weapons[5][2],fweapon.tweapon.gang.used_weapons[5][3]}, -- Da Nang Boys
                    {fweapon.tweapon.gang.used_weapons[6][1],fweapon.tweapon.gang.used_weapons[6][2],fweapon.tweapon.gang.used_weapons[6][3]}, -- Mafia
                    {fweapon.tweapon.gang.used_weapons[7][1],fweapon.tweapon.gang.used_weapons[7][2],fweapon.tweapon.gang.used_weapons[7][3]}, -- Triads
                    {fweapon.tweapon.gang.used_weapons[8][1],fweapon.tweapon.gang.used_weapons[8][2],fweapon.tweapon.gang.used_weapons[8][3]}, -- VLA
                    {fweapon.tweapon.gang.used_weapons[9][1],fweapon.tweapon.gang.used_weapons[9][2],fweapon.tweapon.gang.used_weapons[9][3]}, -- Gang 9 
                    {fweapon.tweapon.gang.used_weapons[10][1],fweapon.tweapon.gang.used_weapons[10][2],fweapon.tweapon.gang.used_weapons[10][3]}, -- Gang 10
                },
                long_range = fweapon.tweapon.long_range[0],
                ped               = fweapon.tweapon.ped[0],
            }, 
        }
    end

    local file = io.open(module.tconfig.path,'w')
    if file then
        file:write(encodeJson(write_table))
        io.close(file)
    end
end

return module
