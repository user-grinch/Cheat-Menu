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
    handling = {},
    paint    = {},
    path     = tcheatmenu.dir ..  "json/config.json",
    reset    = false,
    read     = fcommon.LoadJson("config"),
    temp_texture_name = nil,
    temp_color_rgb = nil,
    tune     = {},
}


function module.Get(s,default)
    if module.tconfig.read == nil then return default end

    local t = module.tconfig.read
    
    for key in s:gmatch('[^.]+') do
      if t[key] == nil then return default end
      t = t[key]
    end

    if t == nil then
        return default
    else
        return t
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

module.tconfig.handling = module.Get("tvehicle.handling",{})
module.tconfig.paint = module.Get("tvehicle.paint",{})
module.tconfig.tune = module.Get("tvehicle.tune",{})


function module.Write()
    local write_table = {}
    if module.tconfig.reset == false then
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
                hot_keys = 
                {
                    asc_key              = tcheatmenu.hot_keys.asc_key,
                    camera_mode_forward  = tcheatmenu.hot_keys.camera_mode_forward,
                    camera_mode_flip     = tcheatmenu.hot_keys.camera_mode_flip,
                    camera_mode_backward = tcheatmenu.hot_keys.camera_mode_backward,
                    camera_mode_x_axis   = tcheatmenu.hot_keys.camera_mode_x_axis,
                    camera_mode_y_axis   = tcheatmenu.hot_keys.camera_mode_y_axis,
                    camera_mode_z_axis   = tcheatmenu.hot_keys.camera_mode_z_axis,
                    menu_open            = tcheatmenu.hot_keys.menu_open,
                    quick_screenshot     = tcheatmenu.hot_keys.quick_screenshot,
                    quick_spawner        = tcheatmenu.hot_keys.quick_spawner,
                    quick_teleport       = tcheatmenu.hot_keys.quick_teleport,
                },
                window =
                {
                    size =
                    {
                        X = tcheatmenu.window.size.X,
                        Y = tcheatmenu.window.size.Y,
                    },
                },
                current_menu = tcheatmenu.current_menu,
            },
            tgame =
            {
                camera                    =
                {
                    speed              = fgame.tgame.camera.speed[0],
                    rotation           = 0.0,
                    lock_x_axis = fgame.tgame.camera.lock_x_axis[0],
                    lock_y_axis = fgame.tgame.camera.lock_y_axis[0],
                    lock_z_axis = fgame.tgame.camera.lock_z_axis[0],
                },
                disable_cheats      = fgame.tgame.disable_cheats[0],
                disable_help_popups = fgame.tgame.disable_help_popups[0],
                disable_replay      = fgame.tgame.disable_replay[0],
                fps_limit           = fgame.tgame.fps_limit[0],
                ghost_cop_cars      = fgame.tgame.ghost_cop_cars[0],
                keep_stuff          = fgame.tgame.keep_stuff[0],
                freeze_mission_timer= fgame.tgame.freeze_mission_timer[0],
                freeze_time         = fgame.tgame.freeze_time[0], 
                random_cheats       = fgame.tgame.random_cheats[0], 
                solid_water         = fgame.tgame.solid_water[0],
                ss_shortcut         = fgame.tgame.ss_shortcut[0],
                sync_system_time    = fgame.tgame.sync_system_time[0],
            },
            tmenu =
            {
                auto_update_check   = fmenu.tmenu.auto_update_check[0],
                auto_reload         = fmenu.tmenu.auto_reload[0],
                disable_in_samp     = fmenu.tmenu.disable_in_samp[0],
                lock_player         = fmenu.tmenu.lock_player[0],
                overlay = 
                {           
                    coordinates     = fmenu.tmenu.overlay.coordinates[0],
                    fps             = fmenu.tmenu.overlay.fps[0],
                    health          = fmenu.tmenu.overlay.health[0],
                    position_index  = fmenu.tmenu.overlay.position_index[0],
                    pos_x           = fmenu.tmenu.overlay.pos_x[0],
                    pos_y           = fmenu.tmenu.overlay.pos_y[0],
                    show            = fmenu.tmenu.overlay.show[0],
                    speed           = fmenu.tmenu.overlay.speed[0],
                },
                fast_load_images    = fmenu.tmenu.fast_load_images[0],
                show_tooltips       = fmenu.tmenu.show_tooltips[0],
                show_crash_message  = fmenu.tmenu.show_crash_message[0],
            },
            tstyle =
            {
            --    font            = fstyle.tstyle.font[0],
                selected        = fstyle.tstyle.selected[0],
            },
            tped  =
            {
                gang = 
                {
                    wars = fped.tped.gang.wars[0],
                },
                ped_health_display = fped.tped.ped_health_display[0],
                type   =
                {
                    index = fped.tped.type.index[0],
                },
            },
            tplayer =
            {
                aimSkinChanger      = fplayer.tplayer.aimSkinChanger[0],
                cjBody              = fplayer.tplayer.cjBody[0],
                god                 = fplayer.tplayer.god[0],
                invisible           = fplayer.tplayer.invisible[0],
                keep_position       = fplayer.tplayer.keep_position[0],
            },
            tteleport =
            {
                auto_z         = fteleport.tteleport.auto_z[0],
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
                auto_load_handling  = fvehicle.tvehicle.auto_load_handling[0],
                auto_load_paint     = fvehicle.tvehicle.auto_load_paint[0],
                auto_load_tune      = fvehicle.tvehicle.auto_load_tune[0],
                apply_material_filter = fvehicle.tvehicle.apply_material_filter[0],
                disable_car_engine  = fvehicle.tvehicle.disable_car_engine[0],
                first_person_camera = fvehicle.tvehicle.first_person_camera[0],
                gxt_name_table   = fvehicle.tvehicle.gxt_name_table,
                handling         = module.tconfig.handling, -- Generated runtime
                heavy            = fvehicle.tvehicle.heavy[0],
                quick_spawn      = fvehicle.tvehicle.quick_spawn[0],
                invisible_car    = fvehicle.tvehicle.invisible_car[0],
                lights           = fvehicle.tvehicle.lights[0],
                lock_speed       = fvehicle.tvehicle.lock_speed[0],
                no_damage        = fvehicle.tvehicle.no_damage[0],
                no_vehicles      = fvehicle.tvehicle.no_vehicles[0],
                paint            = fconfig.tconfig.paint, -- Generated runtime
                random_colors    = fvehicle.tvehicle.random_colors[0],
                random_colors_traffic    = fvehicle.tvehicle.random_colors_traffic[0],
                spawn_inside     = fvehicle.tvehicle.spawn_inside[0],
                speed            = fvehicle.tvehicle.speed[0],
                stay_on_bike     = fvehicle.tvehicle.stay_on_bike[0],
                tune             = fconfig.tconfig.tune, -- Generated runtime
                visual_damage    = fvehicle.tvehicle.visual_damage[0],
                unlimited_nitro  = fvehicle.tvehicle.unlimited_nitro[0],
                watertight_car   = fvehicle.tvehicle.watertight_car[0],
            },
            tvisual =
            {
                negative         = ffi.string(fvisual.tvisual.money.negative),
                positive         = ffi.string(fvisual.tvisual.money.positive),
            },
            tweapon =
            {
                auto_aim          = fweapon.tweapon.auto_aim[0],
                fast_reload       = fweapon.tweapon.fast_reload[0],
                max_accuracy      = fweapon.tweapon.max_accuracy[0], 
                max_ammo_clip     = fweapon.tweapon.max_ammo_clip[0], 
                max_move_speed    = fweapon.tweapon.max_move_speed[0], 
                max_skills        = fweapon.tweapon.max_skills[0], 
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
                long_target_range = fweapon.tweapon.long_target_range[0],
                long_weapon_range = fweapon.tweapon.long_weapon_range[0],
                no_reload         = fweapon.tweapon.no_reload[0],
                ped               = fweapon.tweapon.ped[0],
                quick_spawn       = fweapon.tweapon.quick_spawn[0],
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
