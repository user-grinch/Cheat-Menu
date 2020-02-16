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
	crash_text          = "",
	disable_in_samp		= imgui.new.bool(fconfig.Get('tmenu.disable_in_samp',true)),
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
	fast_load_images	= imgui.new.bool(fconfig.Get('tmenu.fast_load_images',false)),
	show_tooltips	    = imgui.new.bool(fconfig.Get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.Get('tmenu.show_crash_message',true)),
	update_available    = false,
}

module.tmenu.overlay.position_array = imgui.new['const char*'][#module.tmenu.overlay.position](module.tmenu.overlay.position)

function httpRequest(request, body, handler) -- copas.http
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
		
		if not (copas.connect(socket.tcp(),"www.github.com",80,1000)) then
			printHelpString("Unable to connect to GitHub")
			return
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
end


function module.CheckUpdates()
	if pcall(function()
			copas = require 'copas'
			http  = require 'copas.http'
			end) then

		if string.find( script.this.version,"wip") then
			link = "https://raw.githubusercontent.com/user-grinch/Cheat-Menu/master/cheat-menu.lua"
		else
			link = "https://api.github.com/repos/user-grinch/Cheat-Menu/tags"
		end

		httpRequest(link, nil, function(body, code, headers, status)
			if body then
				print(link, 'OK', status)
				if string.find( script.this.version,"wip") then
					repo_version = body:match("script_version_number%((%d+)%)")
					this_version = script.this.version_num
				else
					repo_version = decodeJson(body)[1].name
					this_version = script.this.version
				end
	
				if  repo_version ~= nil then
					if tostring(repo_version) > tostring(this_version) then
						module.tmenu.update_available = true
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
    else
        printHelpString("Failed to check for updates. The rest of the menu is still functional. You can disable auto update check from 'Menu'")
    end
end

-- Main function
function module.MenuMain()

	if imgui.BeginTabBar("MenuTab") then
		if imgui.BeginTabItem("Config") then
			
			imgui.Spacing()
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
			fcommon.CheckBoxVar("Auto check for updates",module.tmenu.auto_update_check)
			fcommon.CheckBoxVar("Auto reload",module.tmenu.auto_reload,"Reload cheat menu automatically\nin case of a crash")
			fcommon.CheckBoxVar("Auto scale",module.tmenu.auto_scale,"Automatically scale menu according to size")
			fcommon.CheckBoxVar("Disable in SAMP",module.tmenu.disable_in_samp,"Using cheats online might ruin\nothers gameply and get you banned")
			imgui.NextColumn()
			fcommon.CheckBoxVar("Lock player",module.tmenu.lock_player,"Lock player controls while the menu is open")
			fcommon.CheckBoxVar("Fast load images",module.tmenu.fast_load_images,"Loads images faster\nThe game will freeze during load")
			fcommon.CheckBoxVar("Show crash message",module.tmenu.show_crash_message)
			fcommon.CheckBoxVar("Show tooltips",module.tmenu.show_tooltips)
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("Overlay") then
			imgui.Spacing()
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
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("Hot keys") then
			
			imgui.Spacing()
			if imgui.BeginChild("Hot keys") then
				fcommon.HotKey(tcheatmenu.hot_keys.menu_open,"Open or close cheat menu")

				imgui.Dummy(imgui.ImVec2(0,10))

				fcommon.HotKey(tcheatmenu.hot_keys.mc_paste,"Paste memory address")
				fcommon.HotKey(tcheatmenu.hot_keys.quick_screenshot,"Take quick screenshot")
				fcommon.HotKey(tcheatmenu.hot_keys.asc_key,"Activate aim skin changer")
				fcommon.HotKey(tcheatmenu.hot_keys.quick_spawner,"Toggle quick spawner")
				fcommon.HotKey(tcheatmenu.hot_keys.quick_teleport,"Toggle quick teleport")

				imgui.Dummy(imgui.ImVec2(0,10))

				fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_forward,"Camera mode forward")
				fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_backward,"Camera mode backward")
				fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_x_axis,"Camera mode x axis movement")
				fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_y_axis,"Camera mode y axis movement")
				fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_z_axis,"Camera mode z axis movement")
				fcommon.HotKey(tcheatmenu.hot_keys.camera_mode_flip,"Camera mode axis movement flip")
				imgui.Dummy(imgui.ImVec2(0,10))

				imgui.TextWrapped("You can reset these config to default from 'Reset to default' button under 'Config' tab")
				imgui.EndChild()
			end

			imgui.EndTabItem()
		end

		if imgui.BeginTabItem("Style") then
			if imgui.BeginChild("Style") then
				imgui.Spacing()
				
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
						fstyle.tstyle.list = fstyle.getStyles()
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
					end
					
					fstyle.StyleEditor()
				end
				imgui.EndChild()
			end

			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("License") then
			imgui.TextWrapped("This program is free software: you can redistribute it and/or modify it under the terms of the \z
			GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or \z
			(at your option) any later version. \n\n\z

			This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied \z
			warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. \n\n\z

			You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.\n\n\n\z

			Copyright (C) 2019-2020 Grinch_ \n")

			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("About") then
			imgui.Spacing()
			if imgui.Button("Check for updates",imgui.ImVec2(fcommon.GetSize(2))) then
				lua_thread.create(module.CheckUpdates)
			end
			imgui.SameLine()
			if imgui.Button("Copy forum link",imgui.ImVec2(fcommon.GetSize(2))) then
				imgui.SetClipboardText(script.this.url)
				printHelpString("Copied to clipboard")
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
			imgui.TextWrapped("Modding community")
			imgui.TextWrapped("Rockstar Games")
			imgui.NextColumn()
			imgui.TextWrapped("For his help")
			imgui.TextWrapped("For his help")
			imgui.TextWrapped("For ImStyleSerializer")

			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		imgui.EndTabBar()
	end

end

return module
