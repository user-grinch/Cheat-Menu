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
	insert_coords           = imgui.new.bool(fconfig.Get('tteleport.insert_coords',false)),
	radar_sprites			= fcommon.LoadJson("radar sprite"),
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

	if math.abs(x) > 99999 or math.abs(y) > 99999 or math.abs(z) > 99999 then
		printHelpString("Coordinate too high")
		return
	end

	if interior_id == nil then
		interior_id = 0
	end
	
	lockPlayerControl(true)
	doFade(false,200)
	wait(200)

	setCharInterior(PLAYER_PED,interior_id)
	setInteriorVisible(interior_id)
	clearExtraColours(true)
	-- loadScene(x,y,z)
	-- requestCollision(x,y)
	activateInteriorPeds(true)

	if isCharInAnyCar(PLAYER_PED) then
		local car = getCarCharIsUsing(PLAYER_PED)
		setCarCoordinates(car,x,y,z)
	else
		setCharCoordinates(PLAYER_PED, x, y, z)
	end

	local timer = getGameTimer()
	
	if target or z == nil then
		while true do
			wait(0)
			local px,py = getCharCoordinates(PLAYER_PED)

			if isCharInAnyCar(PLAYER_PED) then
				local car = getCarCharIsUsing(PLAYER_PED)
				setCarCoordinates(car,x,y,-100)
			else
				setCharCoordinates(PLAYER_PED, x, y, -100)
			end

			if px == x and py == y then
				break
			end

			if getGameTimer() - timer > 500 then
				break
			end

		end
	end
	doFade(true,200)
	lockPlayerControl(false)
end

-- Main function
function module.TeleportMain()

	if fcommon.BeginTabBar("TeleportBar") then
		if fcommon.BeginTabItem("Teleport") then

			imgui.Columns(2,nil,false)
			fcommon.CheckBoxVar("Insert coordinates",module.tteleport.insert_coords)
			imgui.NextColumn()
			fcommon.CheckBoxVar("Quick teleport",module.tteleport.shortcut,"Teleport to marker using" ..  fcommon.GetHotKeyNames(tcheatmenu.hot_keys.quick_teleport))
			imgui.Columns(1)
			
			imgui.Spacing()
            imgui.InputTextWithHint("Coordinates","x, y, z",module.tteleport.coords,ffi.sizeof(module.tteleport.coords))
		

            if module.tteleport.insert_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(module.tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
			end

			if (isKeyDown(vkeys.VK_LCONTROL) or isKeyDown(vkeys.VK_RCONTROL)) and isKeyDown(vkeys.VK_V) then
				imgui.StrCopy(module.tteleport.coords,getClipboardText())
			end
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button("Teleport to coord",imgui.ImVec2(fcommon.GetSize(2))) then
				local x,y,z = (ffi.string(module.tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
				if tonumber(x) ~= nil and tonumber(y) ~= nil and tonumber(z) ~= nil then
					lua_thread.create(module.Teleport,x, y, z,0)
				else
					printHelpString("No coordinate found")
				end
            end
            imgui.SameLine()
            if imgui.Button("Teleport to marker",imgui.ImVec2(fcommon.GetSize(2))) then
                lua_thread.create(module.Teleport)
			end

			fcommon.EndTabItem()
		end
		if fcommon.BeginTabItem("Search") then

			-- Get sprite data, isn't saved in coordinte.json
			module.tteleport.coordinates["Radar"] = {}
			for i = 0xBA86F0,0xBAA248,0x28 do -- 0xBAA248 = 0xBA86F0+175*0x28
				local radarSprite = memory.read(i+36,1)
				if radarSprite ~= 0 then
					local x,y,z = memory.getfloat(i+8),memory.getfloat(i+12),memory.getfloat(i+16)
					module.tteleport.coordinates["Radar"][string.format("%s, %s",module.tteleport.radar_sprites[tostring(radarSprite)],fcommon.GetLocationInfo(x,y,z):sub(1,-5))] = string.format("0, %f, %f, %f",x,y,z)
				end
			end 		

			fcommon.DrawEntries(fconst.IDENTIFIER.TELEPORT,fconst.DRAW_TYPE.TEXT,function(text)
				local interior_id, x, y, z = text:match("([^, ]+), ([^, ]+), ([^, ]+), ([^, ]+)")
				lua_thread.create(module.Teleport,x, y, z,interior_id)
			end,
			function(text)
				
				if imgui.MenuItemBool("Remove location") then 
					for category,table in pairs(module.tteleport.coordinates) do
						for key,val in pairs(table) do
							if key == text then
								print("REMOVED")
								module.tteleport.coordinates[category][key] = nil
								goto end_loop
							end
						end
					end
					::end_loop::
	
					printHelpString("Coordinate ~r~removed")
				end
					
			end,function(a) return a end,module.tteleport.coordinates)

			fcommon.EndTabItem()
		end
		if fcommon.BeginTabItem("Custom") then

			imgui.InputTextWithHint("Name","Groove street",module.tteleport.coord_name,ffi.sizeof(module.tteleport.coord_name))
			imgui.InputTextWithHint("Coordinates","x, y, z",module.tteleport.coords,ffi.sizeof(module.tteleport.coords))

			if module.tteleport.insert_coords[0] then
				local x,y,z = getCharCoordinates(PLAYER_PED)

                imgui.StrCopy(module.tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
			end
			imgui.Spacing()
			if imgui.Button("Save location",imgui.ImVec2(fcommon.GetSize(1))) then
				if ffi.string(module.tteleport.coord_name) == "" then
					printHelpString("No name found")
				else
					local x,y,z = (ffi.string(module.tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
					if tonumber(x) ~= nil and tonumber(y) ~= nil and tonumber(z) ~= nil then
						module.tteleport.coordinates["Custom"][ffi.string(module.tteleport.coord_name)] = string.format("%d, %s",getCharActiveInterior(PLAYER_PED), ffi.string(module.tteleport.coords))
						fcommon.SaveJson("coordinate",module.tteleport.coordinates)
						module.tteleport.coordinates = fcommon.LoadJson("coordinate")
						printHelpString("Entry ~g~added")
					else
						printHelpString("No coordinate found")
					end
				end
			end
		end
	end
end

return module
