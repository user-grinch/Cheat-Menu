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

module.tteleport =
{
	auto_z                  = imgui.new.bool(fconfig.Get('tteleport.auto_z',false)),
	coords                  = imgui.new.char[24](""),
	coordinates             = fcommon.LoadJson("coordinate"),
	coord_name              = imgui.new.char[64](""),
    insert_coords           = imgui.new.bool(fconfig.Get('tteleport.insert_coords',false)),
	search_text             = imgui.new.char[64](""),
	shortcut                = imgui.new.bool(fconfig.Get('tteleport.shortcut',false)),
}

-- Teleports player to a specified coordinates
function module.Teleport(x, y, z,interior_id)
	if x == nil then
		_, x,y,z = getTargetBlipCoordinates()
		interior_id = 0
	end
	if module.tteleport.auto_z[0] then
		z = getGroundZFor3dCoord(x, y, z)
	end

	setCharInterior(PLAYER_PED,interior_id)
	setInteriorVisible(interior_id)
	clearExtraColours(true)
	requestCollision(x,y)
	activateInteriorPeds(true)
	setCharCoordinates(PLAYER_PED, x, y, z+3.0) -- +3.0 to prevent player from teleporting under map
	loadScene(x,y,z)

end

-- Displays the teleport location name
function ShowTeleportEntry(label, x, y, z,interior_id)
	if imgui.MenuItemBool(label) then
		module.Teleport(x, y, z,interior_id)
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

	if imgui.BeginTabBar("Teleport") then

        if imgui.BeginTabItem("Teleport") then	-- Teleport tab
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBoxVar("Get Z coordinates",module.tteleport.auto_z,"Get ground Z of your coordinates")
			fcommon.CheckBoxVar("Insert coordinates",module.tteleport.insert_coords,"Insert current coordinates")
			imgui.NextColumn()
			fcommon.CheckBoxVar("Quick teleport",module.tteleport.shortcut,"Teleport to marker using (X + Y)")
            imgui.Columns(1)

            if imgui.InputText("Coordinates",module.tteleport.coords,ffi.sizeof(module.tteleport.coords)) then end

            if module.tteleport.insert_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(module.tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
            end

            fcommon.InformationTooltip("Enter XYZ coordinates.\nFormat : X,Y,Z")
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button("Teleport to coord",imgui.ImVec2(fcommon.GetSize(2))) then
                local x,y,z = (ffi.string(module.tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
                module.Teleport(x, y, z,0)
            end
            imgui.SameLine()
            if imgui.Button("Teleport to marker",imgui.ImVec2(fcommon.GetSize(2))) then
                module.Teleport()
            end
            imgui.EndTabItem()
        end
		if imgui.BeginTabItem("Search") then -- Search tab
			imgui.Spacing()
			if imgui.InputText("Search",module.tteleport.search_text,ffi.sizeof(module.tteleport.search_text)) then end
			fcommon.InformationTooltip("Right click over any of these entries to remove them")
			imgui.Spacing()
			imgui.Text("Locations found :(" .. ffi.string(module.tteleport.search_text) .. ")")
			imgui.Separator()
			imgui.Spacing()
			if imgui.BeginChild("Teleport entries") then

				-- Loop through he list and display locations
				for name, coord in pairs(module.tteleport.coordinates) do
					local interior_id, x, y, z = coord:match("([^, ]+), ([^, ]+), ([^, ]+), ([^, ]+)")
					if ffi.string(module.tteleport.search_text) == "" then
						ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
					else
						if string.upper(name):find(string.upper(ffi.string(module.tteleport.search_text))) ~= nil  then
							ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
						end
					end
				end
				imgui.EndChild()
			end
            imgui.EndTabItem()
		end

		if imgui.BeginTabItem("Custom") then -- Add  custom locations tab
			imgui.Spacing()
			imgui.Columns(1)
			if imgui.InputText("Location name",module.tteleport.coord_name,ffi.sizeof(module.tteleport.coords)) then end
			if imgui.InputText("Coordinates",module.tteleport.coords,ffi.sizeof(module.tteleport.coords)) then end
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
            imgui.EndTabItem()
		end
        imgui.EndTabBar()
    end
end

return module
