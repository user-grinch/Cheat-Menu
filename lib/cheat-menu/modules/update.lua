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

    local https = nil
    if pcall(function()
            require("socket")
            https = require("ssl.https")
            end) then

        if string.find( script.this.version,"staging") then
            link = "https://raw.githubusercontent.com/inanahammad/Cheat-Menu/staging/cheat-menu.lua"
        else
            link = "https://raw.githubusercontent.com/inanahammad/Cheat-Menu/master/cheat-menu.lua"
        end

        local body, code, headers, status = https.request(link)
        
        if not body then 
            print(code) 
            print(status) 
            printHelpString("~r~Failed~w~ to check for update") 
        else
            tcheatmenu.update.version_number = tonumber(body:match("script_version_number%((%d+)%)"))
            if  tcheatmenu.update.version_number ~= nil then
                if tcheatmenu.update.version_number ~= script.this.version_num then
                    tcheatmenu.update.available = true
                else
                    printHelpString("Using latest version")
                end
            else
                printHelpString("Couldn't connect to github")
            end
        end
    else
        print("Update: Necessary modules not found")
    end


end

return module