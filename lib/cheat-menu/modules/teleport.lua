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

-- Teleport table
local tteleport =
{
    shortcut       = imgui.new.bool(fconfig.get('tteleport.shortcut',false)),
    coords         = imgui.new.char[24](fconfig.get('tteleport.coords',"")),
    auto_z         = imgui.new.bool(fconfig.get('tteleport.auto_z',false)),
    insert_coords  = imgui.new.bool(fconfig.get('tteleport.insert_coords',false)),
    search_text    = imgui.new.char[64](fconfig.get('tteleport.search_text',"")),
}

module.tteleport = tteleport


local coordinates = ftable.coordinates.table


function module.Teleport(x, y, z,interior_id)
	if x == nil then
		_, x,y,z = getTargetBlipCoordinates()
		interior_id = 0
	end
	if tteleport.auto_z[0] then
		z = getGroundZFor3dCoord(x, y, z)
	end

	setCharInterior(PLAYER_PED,interior_id)
	setInteriorVisible(interior_id)
	clearExtraColours(true)
	requestCollision(x,y)
	activateInteriorPeds(true)
	setCharCoordinates(PLAYER_PED, x, y, z)
	loadScene(x,y,z)

end


function ShowTeleportEntry(label, x, y, z,interior_id)
	if imgui.MenuItemBool(label) then
		module.Teleport(x, y, z,interior_id)
	end
end

function module.TeleportMain()

    if imgui.BeginTabBar("Teleport") then
        if imgui.BeginTabItem("Teleport") then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({name = "AutoZCoordinates",var = fteleport.tteleport.auto_z,help_text ="The script would get Z coord automatically", })
			fcommon.CheckBox({name = "InsertCoordinates",var = fteleport.tteleport.insert_coords,help_text ="Insert current coordinates"})
			imgui.NextColumn()
			fcommon.CheckBox({name = "QuickTeleport",var = fteleport.tteleport.shortcut,help_text ="Teleport to marker using (X + Y) key combinartion"})
            imgui.Columns(1)

            if imgui.InputText("Coordinates",tteleport.coords,ffi.sizeof(tteleport.coords)) then end

            if tteleport.insert_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
            end

            fcommon.InformationTooltip("Enter XYZ coordinates.\nFormat : X,Y,Z")
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button("Teleport to coord",imgui.ImVec2(fcommon.GetSize(2))) then
                local x,y,z = (ffi.string(tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
                module.Teleport(x, y, z,0)
            end
            imgui.SameLine()
            if imgui.Button("Teleport to marker",imgui.ImVec2(fcommon.GetSize(2))) then
                module.Teleport()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem("Search") then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",tteleport.search_text,ffi.sizeof(tteleport.search_text)) then end
			imgui.SameLine()

			imgui.Spacing()
			imgui.Text("Found entries :(" .. ffi.string(tteleport.search_text) .. ")")
			imgui.Separator()
			imgui.Spacing()
			if imgui.BeginChild("Teleport entries") then
				for name, coord in pairs(coordinates) do
					local interior_id, x, y, z = coord:match("([^, ]+), ([^, ]+), ([^, ]+), ([^, ]+)")
					if ffi.string(tteleport.search_text) == "" then
						ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
					else
						if string.upper(name):find(string.upper(ffi.string(tteleport.search_text))) ~= nil  then
							ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
						end
					end
				end
				imgui.EndChild()
			end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
