local module = {}

local config_path    =  tcheatMenu.dir ..  "cheat-menu.json"
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
        tcheatMenu =
        {
            window =
            {
                size =
                {
                    X = tcheatMenu.window.size.X,
                    Y = tcheatMenu.window.size.Y,
                },
                overlay =
                {
                    main     = tcheatMenu.window.overlay.main[0],
                    distance = tcheatMenu.window.overlay.distance,
                    corner   = tcheatMenu.window.overlay.corner[0],
                },
            },

            menubuttons =
            {
                current = tcheatMenu.menubuttons.current,
            },
        },

        tteleport =
        {
            shortcut       = fteleport.tteleport.shortcut[0],
            auto_z         = fteleport.tteleport.auto_z[0],
            insert_coords  = fteleport.tteleport.insert_coords[0],
            coords         = ffi.string(fteleport.tteleport.coords),
            search_text    = ffi.string(fteleport.tteleport.search_text),
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
            skins             =
            {
                search_text   = ffi.string(fplayer.tplayer.skins.search_text),
            },
            style =
            {
                fighting =
                {
                    selected = fplayer.tplayer.style.fighting.selected[0],
                },
                walking =
                {
                    selected = fplayer.tplayer.style.walking.selected[0],
                },
            },
            stats =
            {
                search_text = ffi.string(fplayer.tplayer.stats.search_text),
            },
        },
        tweapons =
        {
            quick_spawn = fweapons.tweapons.quick_spawn[0],
            search_text = ffi.string(fweapons.tweapons.search_text),
        },
        tpeds  =
        {
            type   =
            {
                selected = fpeds.tpeds.type.selected[0],
            },
            search_text = ffi.string(fpeds.tpeds.search_text),
        },
        tmissions =
        {
            search_text = ffi.string(fmissions.tmissions.search_text),
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
            quick_spawn  = fvehicles.tvehicles.quick_spawn[0],
            spawn_inside = fvehicles.tvehicles.spawn_inside[0],
            aircraft =
            {
                camera       = fvehicles.tvehicles.aircraft.camera[0],
                spawn_in_air = fvehicles.tvehicles.aircraft.spawn_in_air[0],
                index        = fvehicles.tvehicles.aircraft.index,
            },
            lock_health   = fvehicles.tvehicles.lock_health[0],
            visual_damage = fvehicles.tvehicles.visual_damage[0],
            heavy         = fvehicles.tvehicles.heavy[0],
            stay_on_bike  = fvehicles.tvehicles.stay_on_bike[0],
            lock_speed    = fvehicles.tvehicles.lock_speed[0],
            speed         = fvehicles.tvehicles.speed[0],
            lights = {
                all  = fvehicles.tvehicles.lights.all[0],
            },
        },
        tmenu =
        {
            auto_reload         = fmenu.tmenu.auto_reload[0],
            manual_mouse        = fmenu.tmenu.manual_mouse[0],
            show_tooltips       = fmenu.tmenu.show_tooltips[0],
            show_crash_message  = fmenu.tmenu.show_crash_message[0],
            disable_in_samp     = fmenu.tmenu.disable_in_samp[0],
            overlay = 
            {
                coordinates = fmenu.tmenu.overlay.coordinates[0],
                fps         = fmenu.tmenu.overlay.fps[0],
                health      = fmenu.tmenu.overlay.health[0],
                speed       = fmenu.tmenu.overlay.speed[0],
            },
        },
        tlanguage =
        {
            current   = flanguage.tlanguage.current,
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
