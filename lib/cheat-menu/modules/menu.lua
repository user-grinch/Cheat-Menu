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
	auto_update_check   = imgui.new.bool(fconfig.get('tmenu.auto_update_check',true)),
	auto_reload 		= imgui.new.bool(fconfig.get('tmenu.auto_reload',true)),
	disable_in_samp		= imgui.new.bool(fconfig.get('tmenu.disable_in_samp',false)),
	lock_player   		= imgui.new.bool(fconfig.get('tmenu.lock_player',false)),
	overlay             = 
	{
		coordinates     = imgui.new.bool(fconfig.get('tmenu.overlay.coordinates',false)),
		fps             = imgui.new.bool(fconfig.get('tmenu.overlay.fps',false)),
		show            = imgui.new.bool(true),
		offset          = imgui.new.int(10),
    	position        = {"Custom","Top Left","Top Right","Bottom Left","Bottom Right","Close"},
    	position_array  = {},
		position_index  = imgui.new.int(fconfig.get('tmenu.overlay.position_index',4)),
		health          = imgui.new.bool(fconfig.get('tmenu.overlay.health',false)),
		speed           = imgui.new.bool(fconfig.get('tmenu.overlay.speed',false)),		
	},
	show_tooltips	    = imgui.new.bool(fconfig.get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.get('tmenu.show_crash_message',true)),
    update_available = false,
}

module.tmenu.overlay.position_array = imgui.new['const char*'][#module.tmenu.overlay.position](module.tmenu.overlay.position)


function module.CheckUpdates()
    local https = nil
    if pcall(function()
            require("socket")
            https = require("ssl.https")
            end) then
		
		if string.find( script.this.version,"wip") then
			link = "https://raw.githubusercontent.com/inanahammad/test-repo/master/cheat-menu.lua"
		else
			link = "https://api.github.com/repos/inanahammad/Cheat-Menu/tags"
		end

        local body, code, headers, status = https.request(link)
        
        if not body then 
            print(code) 
            print(status) 
            printHelpString("~r~Failed~w~ to check for update") 
		else
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
        end
        
    else
        printHelpString("Failed to check for updates. The rest of the menu is still functional. You can disable auto update check from 'Menu'")
    end
end

function module.MenuMain()

	if imgui.BeginTabBar("MenuTab") then
		if imgui.BeginTabItem("Config") then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = "Auto check for updates",var = module.tmenu.auto_update_check})
			fcommon.CheckBox({name = "Auto reload",var = module.tmenu.auto_reload,help_text = "Script will automatically reload itself if\nany crash occurs(Might cause issues)"})
			fcommon.CheckBox({name = "Disable in SAMP",var = module.tmenu.disable_in_samp})
			imgui.NextColumn()
			fcommon.CheckBox({name = "Lock player",var = module.tmenu.lock_player,help_text = "Lock player controls while the menu is open"})
			fcommon.CheckBox({name = "Show crash message",var = module.tmenu.show_crash_message})
			fcommon.CheckBox({name = "Show tooltips",var = module.tmenu.show_tooltips})
			
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("Overlay") then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = "Show coordinates",var = module.tmenu.overlay.coordinates,show_help_popups = true})
			fcommon.CheckBox({name = "Show FPS",var = module.tmenu.overlay.fps})	
			imgui.NextColumn()
			fcommon.CheckBox({name = "Show health",var = module.tmenu.overlay.health})
			fcommon.CheckBox({name = "Show speed",var = module.tmenu.overlay.speed})
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
		if imgui.BeginTabItem("About") then
			imgui.BulletText(string.format("%s v%s",script.this.name,script.this.version))
			imgui.BulletText(string.format("Version number : %d",script.this.version_num))
			imgui.SameLine()
			if imgui.Button("Check for update",imgui.ImVec2(120,20)) then
				lua_thread.create(module.CheckUpdates)
			end
			imgui.BulletText(string.format("Author : %s",script.this.authors[1]))
			imgui.BulletText(string.format("Imgui : v%s",imgui._VERSION))
			imgui.BulletText("Discord : Grinch_#3311")
			imgui.BulletText("Thanks : Junior-Djjr,Um_Geek & GTA community.")
			imgui.BulletText("Forum Topic : ")
			imgui.SameLine()
			if imgui.Button("Copy to clipboard",imgui.ImVec2(150,20)) then
				imgui.SetClipboardText(script.this.url)
				printHelpString("~g~Sucessfully~w~ copied to clipboard")
			end
			imgui.EndTabItem()
		end
		imgui.EndTabBar()
	end

end

return module
