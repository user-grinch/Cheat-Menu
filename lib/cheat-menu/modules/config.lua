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

module.tconfig =
{
    path  = tcheatmenu.dir ..  "json/cheat-menu.json",
    reset = false,
    read  = fcommon.LoadJson("cheat-menu"),
    write = {},
}

function module.Write()

    if module.tconfig.reset == false then
        module.tconfig.write =
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
                disable_cheats      = fgame.tgame.disable_cheats[0],
                disable_help_popups = fgame.tgame.disable_help_popups[0],
                disable_replay      = fgame.tgame.disable_replay[0],
                fps_limit           = fgame.tgame.fps_limit[0],
                ghost_cop_cars      = fgame.tgame.ghost_cop_cars[0],
                keep_stuff          = fgame.tgame.keep_stuff[0],
                random_cheats       = fgame.tgame.random_cheats[0], 
                solid_water         = fgame.tgame.solid_water[0],
                ss_shortcut         = fgame.tgame.ss_shortcut[0],
            },
            tmemory =
            {
                address       = ffi.string(fmemory.tmemory.address),
                clear_entries = fmemory.tmemory.clear_entries[0],
                is_float      = fmemory.tmemory.is_float[0],
                size          = fmemory.tmemory.size[0],
                value         = fmemory.tmemory.value[0],
                vp            = fmemory.tmemory.vp[0],          
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
                    show            = fmenu.tmenu.overlay.show[0],
                    speed           = fmenu.tmenu.overlay.speed[0],
                },
                show_tooltips       = fmenu.tmenu.show_tooltips[0],
                show_crash_message  = fmenu.tmenu.show_crash_message[0],
            },
            tped  =
            {
                gang = 
                {
                    wars = fped.tped.gang.wars[0],
                },
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
                first_person_camera = fvehicle.tvehicle.first_person_camera[0],
                heavy            = fvehicle.tvehicle.heavy[0],
                quick_spawn      = fvehicle.tvehicle.quick_spawn[0],
                lights           = fvehicle.tvehicle.lights[0],
                lock_speed       = fvehicle.tvehicle.lock_speed[0],
                no_damage        = fvehicle.tvehicle.no_damage[0],
                spawn_inside     = fvehicle.tvehicle.spawn_inside[0],
                speed            = fvehicle.tvehicle.speed[0],
                stay_on_bike     = fvehicle.tvehicle.stay_on_bike[0],
                visual_damage    = fvehicle.tvehicle.visual_damage[0],
                unlimited_nitro  = fvehicle.tvehicle.unlimited_nitro[0],
            },
            tvisual =
            {
                money =
                {
                    negative = ffi.string(fvisual.tvisual.money.negative),
                    positive = ffi.string(fvisual.tvisual.money.positive),
                },
            },
            tweapon =
            {
                auto_aim        = fweapon.tweapon.auto_aim[0],
                fast_reload     = fweapon.tweapon.fast_reload[0],
                no_reload       = fweapon.tweapon.no_reload[0],
                ped             = fweapon.tweapon.ped[0],
                quick_spawn     = fweapon.tweapon.quick_spawn[0],
            }, 
        }
    end

    local file = io.open(module.tconfig.path,'w')
    if file then
        file:write(encodeJson(module.tconfig.write))
        io.close(file)
    end
end

-- Get value from loaded config table
function module.Get(s,default)
    if module.tconfig.read == nil then return default end

    local t = module.tconfig.read
    
    for key in s:gmatch('[^.]+') do
      if t[ key ] == nil then return default end
      t = t[ key ]
    end

    if t == nil then
        return default
    else
        return t
    end
end

return module
