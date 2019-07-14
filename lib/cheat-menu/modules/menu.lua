-- This module contains all the functions related to the cheat-menu

local module = {}

function module.MenuMain()
	if imgui.BeginChild("Menu") then
		imgui.ShowStyleEditor()
	end
end

return module
