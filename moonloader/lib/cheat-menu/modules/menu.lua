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

module.tmenu =
{	
	auto_update_check   = imgui.new.bool(fconfig.Get('tmenu.auto_update_check',true)),
	auto_reload 		= imgui.new.bool(fconfig.Get('tmenu.auto_reload',true)),
	auto_scale          = imgui.new.bool(fconfig.Get('tmenu.auto_scale',true)),
	command             = 
	{
		filter          = imgui.ImGuiTextFilter(),
		height          = 40,
		input_field     = imgui.new.char[256](),
		list            = {},
		show            = imgui.new.bool(false),
	},
	crash_text          = "",
	debug               =
	{
		read_config     = imgui.new.bool(fconfig.Get('tmenu.debug.read_config',true)),
		write_config    = imgui.new.bool(fconfig.Get('tmenu.debug.write_config',true)),
		write_info      = imgui.new.bool(fconfig.Get('tmenu.debug.write_info',false)),
	},
	disable_in_samp		= imgui.new.bool(fconfig.Get('tmenu.disable_in_samp',true)),
	draw_text_only      = imgui.new.bool(fconfig.Get('tmenu.draw_text_only',false)),
	fast_load_images    = imgui.new.bool(fconfig.Get('tmenu.fast_load_images',false)),
	lock_player   		= imgui.new.bool(fconfig.Get('tmenu.lock_player',false)),
	overlay             = 
	{
		coordinates     = imgui.new.bool(fconfig.Get('tmenu.overlay.coordinates',false)),
		fps             = imgui.new.bool(fconfig.Get('tmenu.overlay.fps',false)),
		show            = imgui.new.bool(true),
		offset          = imgui.new.int(10),
    	position        = {"Custom","Top Left","Top Right","Bottom Left","Bottom Right"},
    	position_array  = {},
		position_index  = imgui.new.int(fconfig.Get('tmenu.overlay.position_index',4)),
		health          = imgui.new.bool(fconfig.Get('tmenu.overlay.health',false)),
		pos_x           = imgui.new.int(fconfig.Get('tmenu.overlay.pos_x',0)),
		pos_y           = imgui.new.int(fconfig.Get('tmenu.overlay.pos_y',0)),
		speed           = imgui.new.bool(fconfig.Get('tmenu.overlay.speed',false)),		
	},
	repo_version        = nil,
	show_tooltips	    = imgui.new.bool(fconfig.Get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.Get('tmenu.show_crash_message',true)),
	update_status       = fconst.UPDATE_STATUS.HIDE_MSG,
}

module.tmenu.overlay.position_array = imgui.new['const char*'][#module.tmenu.overlay.position](module.tmenu.overlay.position)

--------------------------------------------------
-- Command window

function module.FindArgument(t,string)
    for k,v in ipairs(t) do
        if v == string then
            return true
        end
    end
    return false
end

function module.RegisterCommand(string,call_back_func,desc,usage)
    module.tmenu.command.list[string] = {call_back_func,desc,usage}
end

function module.ExecuteCommand()

    local string = ffi.string(module.tmenu.command.input_field)
	local t = {}
	
    for w in string:gmatch("%S+") do 
        table.insert(t,w)
	end
	
	for v,k in pairs(module.tmenu.command.list) do
        if v == t[1] then
            k[1](t)
            return
        end
	end
end

function module.RegisterAllCommands()

	module.RegisterCommand("reload",function(t)
		thisScript():reload()
	end,"Reload cheat menu")

	module.RegisterCommand("reloadall",function(t)
		reloadScripts()
	end,"Reload all moonloader scripts")

	module.RegisterCommand("tp",function(t)
        if t[4] == nil then t[4] = getGroundZFor3dCoord(x,y,100) end
		lua_thread.create(fteleport.Teleport,tonumber(t[2]),tonumber(t[3]),tonumber(t[4]))
	end,"Teleport to coordinates","{int X} {int Y} {int Z}(optional)")
	
	module.RegisterCommand("settime",function(t)
        setTimeOfDay(t[2],t[3])
        printHelpString("Time set")
	end,"Sets in-game time","{int hour} {int minute}")
	
	module.RegisterCommand("cheatmenu",function(t)
        tcheatmenu.window.show[0] = not tcheatmenu.window.show[0]
    end,"Open or close cheat menu")

    module.RegisterCommand("sethealth",function(t)
       setCharHealth(PLAYER_PED,tonumber(t[2]))
       printHelpString("Set health to " .. t[2])
	end,"Sets player health to value","{int health}")
	
    module.RegisterCommand("setmaxhealth",function(t)
        setCharMaxHealth(PLAYER_PED,tonumber(t[2]))
        printHelpString("Set max health to " .. t[2])
	end,"Sets player max health to value","{int max_health}")
	
    module.RegisterCommand("copycoordinates",function(t)
        local x,y,z = getCharCoordinates(PLAYER_PED)
        setClipboardText(string.format("%s %s %s",math.floor(x),math.floor(y),math.floor(z)))
        printHelpString("Coordinates copied to clipboard")
    end,"Copies coordinates to clipboard")

    module.RegisterCommand("setcarspeed",function(t)
        if isCharInAnyCar(PLAYER_PED) then
            local car = getCarCharIsUsing(PLAYER_PED)
            setCarForwardSpeed(car,tonumber(t[2]))
            printHelpString("Car speed set to " ..t[2])
        else
            printHelpString("Player is not in any car")
        end
    end,"Sets vehicle speed","{int speed}")

    module.RegisterCommand("restorecam",function(t)
        restoreCamera()
	end,"Restores camera to default")

	module.RegisterCommand("cameramode",function(t)
        fgame.tgame.camera.bool[0] = not fgame.tgame.camera.bool[0]
	end,"Enable or disable camera mode")
	
	module.RegisterCommand("veh",function(t)
		if t[2] == nil then 
			printHelpString("No vehicle name/model provided") 
			return 
		end

		local model = tonumber(t[2])

        if type(model) == "nil" then
			model = fvehicle.GetModelInfo(string.upper(t[2])) 

			if model ~= 0 and isModelAvailable(model) then  
				if isThisModelABoat(model) 
				or isThisModelACar(model)
				or isThisModelAHeli(model)
				or isThisModelAPlane(model) then
					fvehicle.GiveVehicleToPlayer(model)
				else
					printHelpString("This is not a vehicle model")
				end
			else
				printHelpString("Failed to get model from name")
			end
		end
		

	end,"Spawns vehicle","{vehicle name}")

    module.RegisterCommand("wep",function(t)
		if t[2] == nil then 
			printHelpString("No weapon name/id provided") 
			return 
		end

        local model = tonumber(t[2])

        if type(model) == "nil" then
            model = fweapon.CBaseWeaponInfo(string.upper(t[2]))  
            if model == 0 then  
                printHelpString("Failed to get model from name")
                return
            end
			t[2] = model
			fweapon.GiveWeapon(t[2])
        end
    end,"Spawns weapon","{vehicle name}")
end
--------------------------------------------------

function module.httpRequest(request, body, handler) -- copas.http
    -- start polling task
    if not copas.running then
        copas.running = true
        lua_thread.create(function()
            wait(0)
            while not copas.finished() do
                local ok, err = copas.step(0)
                if ok == nil then error(err) end
                wait(0)
            end
            copas.running = false
        end)
    end
    -- do request
    if handler then
        return copas.addthread(function(r, b, h)
            copas.setErrorHandler(function(err) h(nil, err) end)
            h(http.request(r, b))
        end, request, body, handler)
    else
        local results
        local thread = copas.addthread(function(r, b)
            copas.setErrorHandler(function(err) results = {nil, err} end)
            results = table.pack(http.request(r, b))
        end, request, body)
        while coroutine.status(thread) ~= 'dead' do wait(0) end
        return table.unpack(results)
    end
end


function module.CheckUpdates()

	if string.find( script.this.version,"beta") then
		link = "https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/moonloader/cheat-menu.lua"
	else
		link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags"
	end

	module.httpRequest(link, nil, function(body, code, headers, status)
		if body then
			print(link, 'OK', status)
			if string.find( script.this.version,"beta") then
				repo_version = body:match("script_version_number%((%d+)%)")
				this_version = script.this.version_num
			else
				repo_version = decodeJson(body)[1].name
				this_version = script.this.version
			end

			if  repo_version ~= nil then
				if tostring(repo_version) > tostring(this_version) then
					module.tmenu.update_status = fconst.UPDATE_STATUS.NEW_UPDATE
					printHelpString("New update available")
				else
					printHelpString("No updates found")
				end
			else
				printHelpString("Couldn't connect to github. The rest of the menu is still functional. You can disable auto update check from 'Menu'")
			end
		else
			print(link, 'Error', code)
		end
	end)
end

function module.DownloadHandler(id, status, p1, p2)
	print("Update status: " .. status)
	if status == fconst.UPDATE_STATUS.INSTALL then
		fmenu.tmenu.update_status = fconst.UPDATE_STATUS.INSTALL
		printHelpString("Download complete. Click the 'Install update' button to finish.")
	end
end

function DownloadUpdate()
	if string.find( script.this.version,"beta") then
		module.httpRequest("https://github.com/user-grinch/Cheat-Menu/archive/master.zip", nil, function(body, code, headers, status)  
			print(link, 'OK', status)
			downloadUrlToFile("https://github.com/user-grinch/Cheat-Menu/archive/master.zip",string.format("%supdate.zip",tcheatmenu.dir),module.DownloadHandler)
		end)
	else
		module.httpRequest("https://api.github.com/repos/user-grinch/Cheat-Menu/tags", nil, function(body, code, headers, status)  
			print(link, 'OK', status)
			module.tmenu.repo_version = tostring(decodeJson(body)[1].name)
			downloadUrlToFile("https://github.com/user-grinch/Cheat-Menu/archive/".. module.tmenu.repo_version .. ".zip",string.format("%supdate.zip",tcheatmenu.dir),module.DownloadHandler)
		end)
	end
	
	printHelpString("Download has started. You'll get notified when the download completes.")
	module.tmenu.update_status = fconst.UPDATE_STATUS.DOWNLOADING
end

-- Main function
function module.MenuMain()

	fcommon.Tabs("Menu",{"Config","Overlay","Commands","Hoykeys","Styles","License","About"},{
		function()
			if imgui.Button("Reset to default",imgui.ImVec2(fcommon.GetSize(2))) then
				module.tmenu.crash_text = "Default configuration ~g~restored"
				fconfig.tconfig.reset = true
				thisScript():reload()
			end
			imgui.SameLine()
			if imgui.Button("Reload",imgui.ImVec2(fcommon.GetSize(2))) then
				module.tmenu.crash_text = "Cheat Menu ~g~reloaded"
				thisScript():reload()
			end
			imgui.Dummy(imgui.ImVec2(0,5))
			imgui.Columns(2,nil,false)
			fcommon.CheckBoxVar("Auto reload",module.tmenu.auto_reload,"Reload cheat menu automatically\nin case of a crash.\n\nMight cause crash loop sometimes.")
			fcommon.CheckBoxVar("Auto scale",module.tmenu.auto_scale,"Automatically scale menu according to size")
			fcommon.CheckBoxVar("Check for updates",module.tmenu.auto_update_check)
			fcommon.InformationTooltip("Cheat Menu will automatically check for updates\nonline. This requires an internet connection and\
will download files from github repository.")
			fcommon.CheckBoxVar("Disable in SAMP",module.tmenu.disable_in_samp,"Cheat Menu doesn't endorse using cheats\
on multiplayer and is created for offline\nusage only.\n\nUsing cheats online might ruin others\ngameplay and get yourself banned.")	
			fcommon.CheckBoxVar("Draw text only",module.tmenu.draw_text_only,"Replace the menu images with text names\
This might improve the menu performance")	
			
			imgui.NextColumn()
			fcommon.CheckBoxVar("Fast load images",module.tmenu.fast_load_images,"Loads vehicles, weapons, peds etc. images\nat menu startup.\n \
This may increase game startup time or\nfreeze it for few seconds but improve\nmenu performance.")
			fcommon.CheckBoxVar("Lock player",module.tmenu.lock_player,"Lock player controls while the menu is open")
			fcommon.CheckBoxVar("Show crash message",module.tmenu.show_crash_message)
			fcommon.CheckBoxVar("Show tooltips",module.tmenu.show_tooltips)
			fcommon.InformationTooltip("Shows usage tips beside options.")
			imgui.Columns(1)
			
			if string.find( script.this.version,"beta") then
				imgui.Dummy(imgui.ImVec2(0,10))
				imgui.Columns(2,nil,false)
				fcommon.CheckBoxVar("Read config file",module.tmenu.debug.read_config)
				fcommon.CheckBoxVar("Write config file",module.tmenu.debug.write_config)
				imgui.NextColumn()
				fcommon.CheckBoxVar("Write debug info",module.tmenu.debug.write_info,"Write extra debug info in log")
				imgui.Columns(1)
			end
			
		end,
		function()
			imgui.Columns(2,nil,false)
			fcommon.CheckBoxVar("Show coordinates",module.tmenu.overlay.coordinates)
			fcommon.CheckBoxVar("Show FPS",module.tmenu.overlay.fps)	

			imgui.NextColumn()

			fcommon.CheckBoxVar("Show vehicle health",module.tmenu.overlay.health)
			fcommon.CheckBoxVar("Show vehicle speed",module.tmenu.overlay.speed)
			imgui.Columns(1)

			imgui.Spacing()
			imgui.Combo("Position", module.tmenu.overlay.position_index,module.tmenu.overlay.position_array,#module.tmenu.overlay.position)
			fcommon.InformationTooltip("You can also right click on the\noverlay to access these options")
		end,
		function()
			module.tmenu.command.filter:Draw("Filter")
			fcommon.InformationTooltip(string.format("Open command window using %s\nand close using Enter",fcommon.GetHotKeyNames(tcheatmenu.hot_keys.command_window)))
			imgui.Spacing()

			if imgui.BeginChild("Command entries") then
				for v,k in fcommon.spairs(module.tmenu.command.list) do
					if module.tmenu.command.filter:PassFilter(v) and imgui.CollapsingHeader(v) then
						imgui.Spacing()
						if k[2] ~= nil then
							imgui.TextWrapped("Description: " .. k[2])
						end

						if k[3] == nil then k[3] = "" end
						imgui.TextWrapped("Usage: " .. v .. " " .. k[3])

						imgui.Separator()
					end
				end
				imgui.EndChild()
			end
		end,
		function()
			fcommon.HotKey(tcheatmenu.hot_keys.menu_open,"Open or close cheat menu")
			fcommon.HotKey(tcheatmenu.hot_keys.command_window,"Open command window")

			imgui.Dummy(imgui.ImVec2(0,10))

			fcommon.HotKey(tcheatmenu.hot_keys.mc_paste,"Paste memory address")
			fcommon.HotKey(tcheatmenu.hot_keys.quick_screenshot,"Take quick screenshot")
			fcommon.HotKey(tcheatmenu.hot_keys.asc_key,"Activate aim skin changer")
			fcommon.HotKey(tcheatmenu.hot_keys.quick_teleport,"Toggle quick teleport")

			imgui.Dummy(imgui.ImVec2(0,10))

			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_forward,"Camera mode forward")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_backward,"Camera mode backward")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_left,"Camera mode left")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_right,"Camera mode right")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_slow,"Camera mode slower movement")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_fast,"Camera mode faster movement")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_up,"Camera mode up (lock on player)")
			fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_down,"Camera mode down (lock on player)")
			imgui.Dummy(imgui.ImVec2(0,10))

			imgui.TextWrapped("You can reset these config to default from 'Reset to default' button under 'Config' tab")
		end,
		function()
			if fstyle.tstyle.status then
				if imgui.Button("Delete style",imgui.ImVec2(fcommon.GetSize(2))) then
					fstyle.tstyle.styles_table[(fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1])] = nil
					fstyle.tstyle.list = fstyle.getStyles()
					fstyle.tstyle.array = imgui.new['const char*'][#fstyle.tstyle.list](fstyle.tstyle.list)
					fcommon.SaveJson("styles",fstyle.tstyle.styles_table)
				end
				imgui.SameLine()
				if imgui.Button("Save style",imgui.ImVec2(fcommon.GetSize(2))) then
					fstyle.saveStyles(imgui.GetStyle(), ffi.string(fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]))
					fstyle.tstyle.list  = fstyle.getStyles()
					fstyle.tstyle.array = imgui.new['const char*'][#fstyle.tstyle.list](fstyle.tstyle.list)
					
				end
			end

			imgui.Spacing()

			imgui.InputText('##styleName', fstyle.tstyle.name, ffi.sizeof(fstyle.tstyle.name) - 1) 
			imgui.SameLine()
			if imgui.Button("Add new style") then
				fstyle.saveStyles(imgui.GetStyle(), ffi.string(fstyle.tstyle.name))
				fstyle.tstyle.list = fstyle.getStyles()
				fstyle.tstyle.array = imgui.new['const char*'][#fstyle.tstyle.list](fstyle.tstyle.list)
			end

			if fstyle.tstyle.status then
				
				if imgui.Combo('Select style', fstyle.tstyle.selected, fstyle.tstyle.array, #fstyle.tstyle.list) then
					fstyle.applyStyle(imgui.GetStyle(), fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1])
					fstyle.tstyle.selected_name = fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]
				end
				
				fstyle.StyleEditor()
			end
		end,
		function()
			imgui.TextWrapped("This program is free software: you can redistribute it and/or modify it under the terms of the \z
			GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or \z
			(at your option) any later version. \n\n\z

			This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied \z
			warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. \n\n\z

			You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.\n\n\n\z

			Copyright (C) 2019-2020 Grinch_ \n")
		end,
		function()
			if imgui.Button("Check for updates",imgui.ImVec2(fcommon.GetSize(2))) then
				module.CheckUpdates()
			end
			imgui.SameLine()
			if imgui.Button("Goto GitHub repo",imgui.ImVec2(fcommon.GetSize(2))) then
				os.execute('explorer "https://github.com/user-grinch/Cheat-Menu"')
			end
			imgui.Spacing()
			imgui.Columns(2,nil,false)
			imgui.Text(string.format("%s v%s",script.this.name,script.this.version))
			imgui.Text(string.format("Build: %d",script.this.version_num))

			imgui.NextColumn()
			imgui.Text(string.format("Author: %s",script.this.authors[1]))
			imgui.Text(string.format("Imgui:   v%s",imgui._VERSION))
			imgui.Columns(1)
			imgui.Dummy(imgui.ImVec2(0,10))
			imgui.TextWrapped("Need help/ facing issues/ have suggestions?\nContact me on discord, Grinch_#3311 or on forum.")
			imgui.TextWrapped("\nPlease provide 'moonloader.log' in case of debugging.")
			imgui.Dummy(imgui.ImVec2(0,10))
			
			imgui.TextWrapped("Special thanks to,")
			imgui.Columns(2,nil,false)
			imgui.TextWrapped("Junior-Djjr")
			imgui.TextWrapped("Um_Geek")
			imgui.TextWrapped("randazz0")
			imgui.TextWrapped("Fabio")
			imgui.TextWrapped("kuba--")
			imgui.TextWrapped("Israel")
			imgui.TextWrapped("guru guru")
			imgui.TextWrapped("Modding community")
			imgui.TextWrapped("Rockstar Games")
			imgui.NextColumn()
			imgui.TextWrapped("For his help")
			imgui.TextWrapped("For his help")
			imgui.TextWrapped("For ImStyleSerializer")
			imgui.TextWrapped("For Garage Save Extender")
			imgui.TextWrapped("For C zip library")
			imgui.TextWrapped("For Neon api")
			imgui.TextWrapped("For Timecyc stuff")
			imgui.Columns(1)
		end
	})
end

return module
