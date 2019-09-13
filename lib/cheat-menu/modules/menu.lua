-- This module contains all the functions related to the cheat-menu configuration

local module = {}

local tmenu =
{
	overlay = 
	{
		health = imgui.new.bool(fconfig.get('tmenu.overlay.health',false)),
		speed = imgui.new.bool(fconfig.get('tmenu.overlay.speed',false)),
		coordinates = imgui.new.bool(fconfig.get('tmenu.overlay.coordinates',false)),
		fps  = imgui.new.bool(fconfig.get('tmenu.overlay.fps',false)),
	},
	auto_update_check   = imgui.new.bool(fconfig.get('tmenu.auto_update_check',true)),
	auto_reload 		= imgui.new.bool(fconfig.get('tmenu.auto_reload',true)),
	manual_mouse 		= imgui.new.bool(fconfig.get('tmenu.manual_mouse',false)),
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
			fcommon.CheckBox({name = "Auto reload",var = fmenu.tmenu.auto_reload,help_text = "Script will automatically reload itself if\nany crash occurs(Might cause issues)",})
			fcommon.CheckBox({name = "Disable in SAMP",var = fmenu.tmenu.disable_in_samp})
			imgui.NextColumn()
			fcommon.CheckBox({name = "Manual mouse",var = fmenu.tmenu.manual_mouse,help_text = "Manually show/hide mouse cursor\n(Double click right mouse button)",})
			fcommon.CheckBox({name = "Show crash message",var = fmenu.tmenu.show_crash_message})
			fcommon.CheckBox({name = "Show tooltips",var = fmenu.tmenu.show_tooltips})
			
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem("Overlay") then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = "Show coordinates",var = tmenu.overlay.coordinates,show_help_popups = true,help_text = "Draws coordinates on-screen.\n\nOptions:\nPressing (Left Ctrl + C) stores coords to clipboard."})
			fcommon.CheckBox({name = "Show FPS",var = tmenu.overlay.fps})	
			imgui.NextColumn()
			fcommon.CheckBox({name = "Show health",var = tmenu.overlay.health})
			fcommon.CheckBox({name = "Show speed",var = tmenu.overlay.speed})
			imgui.Columns(1)	
			
			imgui.Spacing()
			imgui.Separator()
			imgui.Spacing()
			if imgui.Combo("Position", tcheatmenu.window.overlay.corner,tcheatmenu.window.overlay.list,#tcheatmenu.window.overlay.names) then
				if tcheatmenu.window.overlay.corner[0] == 5 then
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
