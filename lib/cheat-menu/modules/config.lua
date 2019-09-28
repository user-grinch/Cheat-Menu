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

local config_path    =  tcheatmenu.dir ..  "json/cheat-menu.json"
module.config_path   = config_path

local tconfig =
{
    read  = {},
    write = {},
}

module.tconfig = tconfig

function module.write()

    tconfig.write =
    {
        tcheatmenu =
        {
            menubuttons =
            {
                current = tcheatmenu.menubuttons.current,
            },
        },

        tteleport =
        {
            shortcut       = fteleport.tteleport.shortcut[0],
            auto_z         = fteleport.tteleport.auto_z[0],
            insert_coords  = fteleport.tteleport.insert_coords[0],
            coords         = ffi.string(fteleport.tteleport.coords),
        },
        tmemory =
        {
            address       = ffi.string(fmemory.tmemory.address),
            size          = fmemory.tmemory.size[0],
            vp            = fmemory.tmemory.vp[0],
            clear_entries = fmemory.tmemory.clear_entries[0],
            value         = fmemory.tmemory.value[0],
            is_float      = fmemory.tmemory.is_float[0],
        },
        tplayer =
        {
            god               = fplayer.tplayer.god[0],
            aimSkinChanger    = fplayer.tplayer.aimSkinChanger[0],
            neverWanted       = fplayer.tplayer.neverWanted[0],
            cjBody            = fplayer.tplayer.cjBody[0],
        },
        tanimation =
        {
            loop             = fanimation.tanimation.loop[0],
            ped              = fanimation.tanimation.ped[0],
            secondary        = fanimation.tanimation.secondary[0],
            fighting =
            {
                selected = fanimation.tanimation.fighting.selected[0],
            },
            walking =
            {
                selected = fanimation.tanimation.walking.selected[0],
            },
        },
        tweapon =
        {
            quick_spawn = fweapon.tweapon.quick_spawn[0],
            ped         = fweapon.tweapon.ped[0],
        },
        tvisual =
        {
            money =
            {
                positive = ffi.string(fvisual.tvisual.money.positive),
                negative = ffi.string(fvisual.tvisual.money.negative),
            },
        },
        tped  =
        {
            type   =
            {
                selected = fped.tped.type.selected[0],
            },
        },
        tgame =
        {
            ss_shortcut = fgame.tgame.ss_shortcut[0],
            disable_cheats = fgame.tgame.disable_cheats[0],
            fps =
            {
                limit = fgame.tgame.fps.limit[0],
            },
            disable_help_popups = fgame.tgame.disable_help_popups[0],
        },
        tvehicles =
        {
            quick_spawn  = fvehicle.tvehicles.quick_spawn[0],
            spawn_inside = fvehicle.tvehicles.spawn_inside[0],
            aircraft =
            {
                camera       = fvehicle.tvehicles.aircraft.camera[0],
                spawn_in_air = fvehicle.tvehicles.aircraft.spawn_in_air[0],
                index        = fvehicle.tvehicles.aircraft.index,
            },
            no_damage     = fvehicle.tvehicles.no_damage[0],
            visual_damage = fvehicle.tvehicles.visual_damage[0],
            heavy         = fvehicle.tvehicles.heavy[0],
            stay_on_bike  = fvehicle.tvehicles.stay_on_bike[0],
            lock_speed    = fvehicle.tvehicles.lock_speed[0],
            speed         = fvehicle.tvehicles.speed[0],
            lights = {
                all  = fvehicle.tvehicles.lights.all[0],
            },
        },
        tmenu =
        {
            auto_update_check   = fmenu.tmenu.auto_update_check[0],
            auto_reload         = fmenu.tmenu.auto_reload[0],
            show_tooltips       = fmenu.tmenu.show_tooltips[0],
            show_crash_message  = fmenu.tmenu.show_crash_message[0],
            disable_in_samp     = fmenu.tmenu.disable_in_samp[0],
            lock_player         = fmenu.tmenu.lock_player[0],
            overlay = 
            {
                show            = fmenu.tmenu.overlay.show[0],
                position_index  = fmenu.tmenu.overlay.position_index[0],
                coordinates     = fmenu.tmenu.overlay.coordinates[0],
                fps             = fmenu.tmenu.overlay.fps[0],
                health          = fmenu.tmenu.overlay.health[0],
                speed           = fmenu.tmenu.overlay.speed[0],
            },
        },
    }

    local file = io.open(config_path,'w')
    if file then
        file:write(encodeJson(tconfig.write))
        io.close(file)
    end
end

function module.Read()
    local file = io.open(config_path,'r')
    if file then
        local data = file:read("*all")
        io.close(file)
        if data then
            tconfig.read = decodeJson(data)
        end
    end
end

function module.get(s,default)
    if tconfig.read == nil then return default end

    local t = tconfig.read
    
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
