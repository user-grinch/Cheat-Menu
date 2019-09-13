--[[ This file contains about tab functions

    Please don't remove credits below.
]]--
local module = {}

function module.AboutMain()
    imgui.BulletText(string.format("%s v%s(%d)",script.this.name,script.this.version,script.this.version_num))
    imgui.SameLine()
    if imgui.Button("Check for update",imgui.ImVec2(120,20)) then
        if not pcall(fupdate.CheckUpdates) then
            printHelpString("Failed to update.")
        end
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
