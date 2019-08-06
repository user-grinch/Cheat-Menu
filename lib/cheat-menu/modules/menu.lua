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

	if imgui.BeginTabBar("MenuTab") then
		if imgui.BeginTabItem(flanguage.GetText('menu.Config')) then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = flanguage.GetText('menu.AutoReload'),var = fmenu.tmenu.auto_reload,help_text = flanguage.GetText('menu.AutoReloadToolTip')})
			fcommon.CheckBox({name = flanguage.GetText('menu.ShowCrashMessage'),var = fmenu.tmenu.show_crash_message})
			imgui.NextColumn()
			fcommon.CheckBox({name = flanguage.GetText('menu.ShowTooltips'),var = fmenu.tmenu.show_tooltips})
			imgui.Columns(1)
			imgui.Spacing()
			if imgui.Combo(flanguage.GetText('menu.Language'), flanguage.tlanguage.selected,flanguage.tlanguage.list,#flanguage.tlanguage.names) then
				flanguage.tlanguage.current = flanguage.tlanguage.names[flanguage.tlanguage.selected[0]+1]
				printHelpString(flanguage.GetText('menu.LanguageChanged'))
			end
			imgui.EndTabItem()
		end
		if imgui.BeginTabItem(flanguage.GetText('menu.StyleEditor')) then
			if imgui.BeginChild("Menu") then
				imgui.ShowStyleEditor()
				imgui.EndTabItem()
				imgui.EndChild()
			end
		end
		imgui.EndTabBar()
	end

end

return module
