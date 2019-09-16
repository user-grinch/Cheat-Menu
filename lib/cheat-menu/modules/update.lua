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

function module.CheckUpdates()
    require("socket")
    local https = require("ssl.https")

    if string.find( script.this.version,"staging") then
        link = "https://raw.githubusercontent.com/inanahammad/Cheat-Menu/staging/cheat-menu.lua"
    else
        link = "https://raw.githubusercontent.com/inanahammad/Cheat-Menu/master/cheat-menu.lua"
    end

    local body, code, headers, status = https.request(link)
    
    if not body then error(code) print(status) printHelpString(flanguage.GetText("update.FailedToUpdate")) return end
    tcheatmenu.update.version_number = tonumber(body:match("script_version_number%((%d+)%)"))
    if tcheatmenu.update.version_number ~= script.this.version_num then
        tcheatmenu.update.available = true
    else
        printHelpString("Using ~g~latest~w~ version")
    end
end

function module.DownloadUpdates()
    require("socket")
    local https = require("ssl.https")
    local one, code, headers, status = https.request {
        url = "https://github.com/inanahammad/Cheat-Menu/archive/master.zip",
        protocol = "TLSv1.3",
        options = "all",
        verify = "none",
      }
    if not body then error(code) return end
    local f = assert(io.open('master.zip', 'wb')) -- open in "binary" mode
    f:write(body)
    f:close()
end

return module