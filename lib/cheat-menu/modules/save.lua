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

function module.LoadJson(filename)
    local full_path = tcheatmenu.dir .. "saves//" .. filename .. ".json"
    local file = io.open(full_path, "r")
    local table = decodeJson(file:read("*a"))
    file:close()
    return table
end

function module.SaveJson(filename,table)
    local full_path = tcheatmenu.dir .. "saves//" .. filename .. ".json"
    local file = assert(io.open(full_path, "w"))
    file:write(encodeJson(table))
    file:close()
end


return module