-- This module contains all the functions related to the cheat-menu configuration

local module = {}

local tmenu =
{
	auto_reload = imgui.new.bool(fconfig.get('tmenu.auto_reload',false)),
	show_tooltips = imgui.new.bool(fconfig.get('tmenu.show_tooltips',true)),
	show_crash_message = imgui.new.bool(fconfig.get('tmenu.show_crash_message',true)),
}
module.tmenu = tmenu

function module.MenuMain()
	if imgui.BeginChild("Menu") then
		if imgui.BeginTabBar("MenuTab") then
			if imgui.BeginTabItem("Checkboxes") then
				imgui.Columns(2,nil,false)
				fcommon.CheckBox({name = "Auto reload",var = fmenu.tmenu.auto_reload,help_text = "Script would automatically reload itself if any crash occurs.Might cause issues."})
				fcommon.CheckBox({name = "Show crash message",var = fmenu.tmenu.show_crash_message})
				imgui.NextColumn()
				fcommon.CheckBox({name = "Show tooltips",var = fmenu.tmenu.show_tooltips})
				imgui.Columns(1)
				imgui.EndTabItem()
			end
			if imgui.BeginTabItem("Style Editor") then
				imgui.ShowStyleEditor()
				imgui.EndTabItem()
			end
			imgui.EndTabBar()
		end
		imgui.EndChild()
	end
end

return module
