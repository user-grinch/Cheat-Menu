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

local tmenu =
{
	overlay = 
	{
		offset          = imgui.new.int(10),
    	show            = imgui.new.bool(true),
    	position        = {"Custom","Top Left","Top Right","Bottom Left","Bottom Right","Close"},
    	position_array  = {},
    	position_index  = imgui.new.int(fconfig.get('tmenu.overlay.position_index',4)),
		health          = imgui.new.bool(fconfig.get('tmenu.overlay.health',false)),
		speed           = imgui.new.bool(fconfig.get('tmenu.overlay.speed',false)),
		coordinates     = imgui.new.bool(fconfig.get('tmenu.overlay.coordinates',false)),
		fps             = imgui.new.bool(fconfig.get('tmenu.overlay.fps',false)),
	},
	
	auto_update_check   = imgui.new.bool(fconfig.get('tmenu.auto_update_check',true)),
	auto_reload 		= imgui.new.bool(fconfig.get('tmenu.auto_reload',true)),
	lock_player   		= imgui.new.bool(fconfig.get('tmenu.lock_player',false)),
	show_tooltips	    = imgui.new.bool(fconfig.get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.get('tmenu.show_crash_message',true)),
	disable_in_samp		= imgui.new.bool(fconfig.get('tmenu.disable_in_samp',false)),
}

module.tmenu = tmenu

function module.MenuMain()

	if imgui.BeginTabBar("MenuTab") then
		if imgui.BeginTabItem("Config") then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = "Auto check for updates",var = fmenu.tmenu.auto_update_check})
			fcommon.CheckBox({name = "Auto reload",var = fmenu.tmenu.auto_reload,help_text = "Script will automatically reload itself if\nany crash occurs(Might cause issues)"})
			fcommon.CheckBox({name = "Disable in SAMP",var = fmenu.tmenu.disable_in_samp})
			imgui.NextColumn()
			fcommon.CheckBox({name = "Lock player",var = fmenu.tmenu.lock_player,help_text = "Lock player controls while the menu is open"})
			fcommon.CheckBox({name = "Show crash message",var = fmenu.tmenu.show_crash_message})
			fcommon.CheckBox({name = "Show tooltips",var = fmenu.tmenu.show_tooltips})
			
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("Overlay") then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = "Show coordinates",var = tmenu.overlay.coordinates,show_help_popups = true})
			fcommon.CheckBox({name = "Show FPS",var = tmenu.overlay.fps})	
			imgui.NextColumn()
			fcommon.CheckBox({name = "Show health",var = tmenu.overlay.health})
			fcommon.CheckBox({name = "Show speed",var = tmenu.overlay.speed})
			imgui.Columns(1)	
			
			imgui.Spacing()
			imgui.Separator()
			imgui.Spacing()
			if imgui.Combo("Position", fmenu.tmenu.overlay.position_index,fmenu.tmenu.overlay.position_array,#fmenu.tmenu.overlay.position) then
				if fmenu.tmenu.overlay.position_index[0] == 5 then
					fgame.tgame.fps.bool[0] = false
					fvehicles.tvehicles.show.speed[0] = false
					fvehicles.tvehicles.show.health[0] = false
					fvisuals.tvisuals.show_coordinates[0] = false
				end
			end
			imgui.EndTabItem()
		end
		imgui.EndTabBar()
	end

end

return module
