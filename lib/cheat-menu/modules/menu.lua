-- This module contains all the functions related to the cheat-menu configuration

local module = {}

local tmenu =
{
	auto_reload 		= imgui.new.bool(fconfig.get('tmenu.auto_reload',false)),
	show_tooltips	    = imgui.new.bool(fconfig.get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.get('tmenu.show_crash_message',true)),
	disable_in_samp		= imgui.new.bool(fconfig.get('tmenu.disable_in_samp',true)),
}

module.tmenu = tmenu

function module.MenuMain()

	if imgui.BeginTabBar("MenuTab") then
		if imgui.BeginTabItem(flanguage.GetText('menu.Config')) then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({name = flanguage.GetText('menu.AutoReload'),var = fmenu.tmenu.auto_reload,help_text = flanguage.GetText('menu.AutoReloadToolTip')})
			fcommon.CheckBox({name = flanguage.GetText('menu.DisableInSAMP'),var = fmenu.tmenu.disable_in_samp})
			imgui.NextColumn()
			fcommon.CheckBox({name = flanguage.GetText('menu.ShowCrashMessage'),var = fmenu.tmenu.show_crash_message})
			fcommon.CheckBox({name = flanguage.GetText('menu.ShowTooltips'),var = fmenu.tmenu.show_tooltips})
			
			imgui.Columns(1)
			
			imgui.EndTabItem()
		end
		-- if imgui.BeginTabItem(u8"こんにちは！テスト") then
		-- 	if imgui.BeginChild("Customize") then
		-- 		imgui.Spacing()
		-- 		if imgui.Combo(flanguage.GetText('menu.Language'), flanguage.tlanguage.selected,flanguage.tlanguage.list,#flanguage.tlanguage.names) then
		-- 			flanguage.tlanguage.current = flanguage.tlanguage.names[flanguage.tlanguage.selected[0]+1]
		-- 			if not pcall(flanguage.LoadCurrentLanguage) then
		-- 				printHelpString(flanguage.tlanguage.current .. " " .. flanguage.GetText('menu.LanguageLoadFailure'))
		-- 			else
		-- 				printHelpString(flanguage.tlanguage.current .. " " .. flanguage.GetText('menu.LanguageLoaded'))
		-- 			end
		-- 		end
		-- 		imgui.EndTabItem()
		-- 		imgui.EndChild()
		-- 	end
		-- end
		imgui.EndTabBar()
	end

end

return module
