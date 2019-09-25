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

-- Please don't remove credits below.

local module = {}

function module.AboutMain()
    imgui.BulletText(string.format("%s v%s",script.this.name,script.this.version))
    imgui.BulletText(string.format("Version number : %d",script.this.version_num))
    imgui.SameLine()
    if imgui.Button("Check for update",imgui.ImVec2(120,20)) then
        fupdate.CheckUpdates()
    end
    imgui.BulletText(string.format("Author : %s",script.this.authors[1]))
    imgui.BulletText(string.format("Imgui : v%s",imgui._VERSION))
    imgui.BulletText("Discord : Grinch_#3311")
    imgui.BulletText("Thanks : Junior-Djjr,Um_Geek & GTA community.")
    imgui.BulletText("Forum Topic : ")
    imgui.SameLine()
    if imgui.Button("Copy to clipboard",imgui.ImVec2(150,20)) then
        imgui.SetClipboardText(script.this.url)
        printHelpString("~g~Sucessfully~w~ copied to clipboard")
    end
end

return module
