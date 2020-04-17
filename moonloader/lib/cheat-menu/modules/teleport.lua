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

module.tteleport =
{
	coords                  = imgui.new.char[64](""),
	coordinates             = fcommon.LoadJson("coordinate"),
	coord_name              = imgui.new.char[64](""),
	filter                  = imgui.ImGuiTextFilter(),
    insert_coords           = imgui.new.bool(fconfig.Get('tteleport.insert_coords',false)),
	shortcut                = imgui.new.bool(fconfig.Get('tteleport.shortcut',false)),
}

-- Teleports player to a specified coordinates
function module.Teleport(x, y, z,interior_id)

	local target = false

	if x == nil and y == nil then
		target, x,y,z = getTargetBlipCoordinates()

		if not target then
			printHelpString("No marker found")
			return
		end
	end

	if interior_id == nil then
		interior_id = 0
	end

	lockPlayerControl(true)
	doFade(false,300)
	wait(300)

	setCharInterior(PLAYER_PED,interior_id)
	setInteriorVisible(interior_id)
	clearExtraColours(true)
	loadScene(x,y,z)
	requestCollision(x,y)
	activateInteriorPeds(true)

	setCharCoordinates(PLAYER_PED, x, y, z)

	wait(300)
	if target or z == nil then
		while true do
			local px,py = getCharCoordinates(PLAYER_PED)
			setCharCoordinates(PLAYER_PED, x, y, -100.0)
			if px == x and py == y then
				break
			end
			wait(0)
		end
	end
	doFade(true,300)
	lockPlayerControl(false)
end

-- Displays the teleport location name
function ShowTeleportEntry(label, x, y, z,interior_id)
	if imgui.MenuItemBool(label) then
		lua_thread.create(module.Teleport,x, y, z,interior_id)
	end
	if imgui.IsItemClicked(1) then
		module.tteleport.coordinates[label] = nil
		fcommon.SaveJson("coordinate",module.tteleport.coordinates)
		module.tteleport.coordinates = fcommon.LoadJson("coordinate")
		printHelpString("Coordinate ~r~removed")
	end
end

-- Main function
function module.TeleportMain()

	fcommon.Tabs("Teleport",{"Teleport","Search","Custom"},{
		function()
			imgui.Columns(2,nil,false)
			fcommon.CheckBoxVar("Insert coordinates",module.tteleport.insert_coords,"Insert current coordinates")
			imgui.NextColumn()
			fcommon.CheckBoxVar("Quick teleport",module.tteleport.shortcut,"Teleport to marker using" ..  fcommon.GetHotKeyNames(tcheatmenu.hot_keys.quick_teleport))
			imgui.Columns(1)
			
			imgui.Spacing()

            imgui.InputText("Coordinates",module.tteleport.coords,ffi.sizeof(module.tteleport.coords))

            if module.tteleport.insert_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(module.tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
			end

			if (isKeyDown(vkeys.VK_LCONTROL) or isKeyDown(vkeys.VK_RCONTROL)) and isKeyDown(vkeys.VK_V) then
				imgui.StrCopy(module.tteleport.coords,getClipboardText())
			end
            fcommon.InformationTooltip("Enter XYZ coordinates.\nFormat : X,Y,Z")
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button("Teleport to coord",imgui.ImVec2(fcommon.GetSize(2))) then
				local x,y,z = (ffi.string(module.tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
				if x ~= nil and y ~= nil and z ~= nil then
					lua_thread.create(module.Teleport,x, y, z,0)
				else
					printHelpString("No coordinate found")
				end
            end
            imgui.SameLine()
            if imgui.Button("Teleport to marker",imgui.ImVec2(fcommon.GetSize(2))) then
                lua_thread.create(module.Teleport)
            end
		end,
		function()
			module.tteleport.filter:Draw("Filter")
			fcommon.InformationTooltip("Right click over any of these entries to remove them")
			imgui.Spacing()

			if imgui.BeginChild("Teleport entries") then
				for name, coord in pairs(module.tteleport.coordinates) do
					local interior_id, x, y, z = coord:match("([^, ]+), ([^, ]+), ([^, ]+), ([^, ]+)")

					if module.tteleport.filter:PassFilter(name) then
						ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
					end
				end
				imgui.EndChild()
			end
		end,
		function()
			imgui.Columns(1)
			imgui.InputText("Location name",module.tteleport.coord_name,ffi.sizeof(module.tteleport.coords))
			imgui.InputText("Coordinates",module.tteleport.coords,ffi.sizeof(module.tteleport.coords))
			fcommon.InformationTooltip("Enter XYZ coordinates.\nFormat : X,Y,Z")
			if module.tteleport.insert_coords[0] then
				local x,y,z = getCharCoordinates(PLAYER_PED)

                imgui.StrCopy(module.tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
			end
			imgui.Spacing()
			if imgui.Button("Save location",imgui.ImVec2(fcommon.GetSize(1))) then
				if ffi.string(module.tteleport.coord_name) == "" then
					imgui.StrCopy(module.tteleport.coord_name,"Untitled")
				end
				module.tteleport.coordinates[ffi.string(module.tteleport.coord_name)] = string.format("%d, %s",getActiveInterior(), ffi.string(module.tteleport.coords))
				fcommon.SaveJson("coordinate",module.tteleport.coordinates)
				module.tteleport.coordinates = fcommon.LoadJson("coordinate")
				printHelpString("Entry ~g~added")
            end
		end
	})
end

return module
