-- This module contains all the functions related to the cheat-menu configuration

local module = {}

local tmenu =
{
	auto_reload 		= imgui.new.bool(fconfig.get('tmenu.auto_reload',true)),
	show_tooltips	    = imgui.new.bool(fconfig.get('tmenu.show_tooltips',true)),
	show_crash_message  = imgui.new.bool(fconfig.get('tmenu.show_crash_message',true)),
	disable_in_samp		= imgui.new.bool(fconfig.get('tmenu.disable_in_samp',false)),
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
		if imgui.BeginTabItem(flanguage.GetText('menu.Overlay')) then
			imgui.Columns(2,nil,false)
			imgui.Spacing()
			fcommon.CheckBox({ name = flanguage.GetText('visuals.ShowCoordinates'),var = fvisuals.tvisuals.show_coordinates,show_help_popups = true,help_text = "Draws coordinates on-screen.\n\nOptions:\nPressing (Left Ctrl + C) stores coords to clipboard."})
			fcommon.CheckBox({name = flanguage.GetText('game.ShowFPS'),var = fgame.tgame.fps.bool})	
			imgui.NextColumn()
			fcommon.CheckBox({name = flanguage.GetText("vehicles.ShowHealth"),var = fvehicles.tvehicles.show.health})
			fcommon.CheckBox({name = flanguage.GetText("vehicles.ShowSpeed"),var = fvehicles.tvehicles.show.speed})
			imgui.Columns(1)	
			
			imgui.Spacing()
			imgui.Separator()
			imgui.Spacing()
			if imgui.Combo(flanguage.GetText("cheatmenu.Position"), tcheatMenu.window.overlay.corner,tcheatMenu.window.overlay.list,#tcheatMenu.window.overlay.names) then
				if tcheatMenu.window.overlay.corner[0] == 5 then
					fgame.tgame.fps.bool[0] = false
					fvehicles.tvehicles.show.speed[0] = false
					fvehicles.tvehicles.show.health[0] = false
					fvisuals.tvisuals.show_coordinates[0] = false
				end
			end
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
