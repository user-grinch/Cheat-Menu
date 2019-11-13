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

module.tmenu =
{	
	auto_update_check   = imgui.new.bool(fconfig.Get('tmenu.auto_update_check',true)),
	auto_reload 		= imgui.new.bool(fconfig.Get('tmenu.auto_reload',true)),
	crash_text          = "",
	disable_in_samp		= imgui.new.bool(fconfig.Get('tmenu.disable_in_samp',false)),
	lock_player   		= imgui.new.bool(fconfig.Get('tmenu.lock_player',false)),
	overlay             = 
	{
		coordinates     = imgui.new.bool(fconfig.Get('tmenu.overlay.coordinates',false)),
		fps             = imgui.new.bool(fconfig.Get('tmenu.overlay.fps',false)),
		show            = imgui.new.bool(true),
		offset          = imgui.new.int(10),
    	position        = {"Custom","Top Left","Top Right","Bottom Left","Bottom Right","Close"},
    	position_array  = {},
		position_index  = imgui.new.int(fconfig.Get('tmenu.overlay.position_index',4)),
		health          = imgui.new.bool(fconfig.Get('tmenu.overlay.health',false)),
		speed           = imgui.new.bool(fconfig.Get('tmenu.overlay.speed',false)),		
	},
	show_tooltips	    = imgui.new.bool(fconfig.Get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.Get('tmenu.show_crash_message',true)),
    update_available = false,
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
    local https = nil
	if pcall(function()
			copas = require 'copas'
			http = require 'copas.http'
            end) then
		
		if string.find( script.this.version,"wip") then
			link = "https://raw.githubusercontent.com/inanahammad/Cheat-Menu/master/cheat-menu.lua"
		else
			link = "https://api.github.com/repos/inanahammad/Cheat-Menu/tags"
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
			fcommon.CheckBoxVar("Disable in SAMP",module.tmenu.disable_in_samp)
			imgui.NextColumn()
			fcommon.CheckBoxVar("Lock player",module.tmenu.lock_player,"Lock player controls while the menu is open")
			fcommon.CheckBoxVar("Show crash message",module.tmenu.show_crash_message)
			fcommon.CheckBoxVar("Show tooltips",module.tmenu.show_tooltips)
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("Overlay") then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBoxVar("Show coordinates",module.tmenu.overlay.coordinates)
			fcommon.CheckBoxVar("Show FPS",module.tmenu.overlay.fps)	
			imgui.NextColumn()
			fcommon.CheckBoxVar("Show vehicle health",module.tmenu.overlay.health)
			fcommon.CheckBoxVar("Show vehicle speed",module.tmenu.overlay.speed)
			imgui.Columns(1)	
			
			imgui.Spacing()
			imgui.Separator()
			imgui.Spacing()
			if imgui.Combo("Position", module.tmenu.overlay.position_index,module.tmenu.overlay.position_array,#module.tmenu.overlay.position) then
				if module.tmenu.overlay.position_index == 5 then
					fgame.tgame.fps.bool[0] = false
					fvehicles.tvehicles.show.speed[0] = false
					fvehicles.tvehicles.show.health[0] = false
					fvisuals.tvisuals.show_coordinates[0] = false
				end
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

			Copyright (C) 2019 Grinch_ \n")

			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("About") then
			imgui.Spacing()
			if imgui.Button("Check for updates",imgui.ImVec2(fcommon.GetSize(2))) then
				module.CheckUpdates()
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
			imgui.TextWrapped("Modding community")
			imgui.NextColumn()
			imgui.TextWrapped("Um_Geek")
			imgui.TextWrapped("Rockstar Games")
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		imgui.EndTabBar()
	end

end

return module
