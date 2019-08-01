--[[ This file contains about tab functions

    Please don't remove credits below.
]]--
local module = {}

function module.AboutMain()
    imgui.BulletText(string.format("%s v%s(%d)",script.this.name,script.this.version,script.this.version_num))
    imgui.BulletText(string.format("Author : %s",script.this.authors[1]))
    imgui.BulletText(string.format("Imgui  : v%s",imgui._VERSION))
    imgui.BulletText("Discord : Grinch_#3311")
    imgui.BulletText("Thanks  : Junior-Djjr,Um_Geek & GTA community.")
    imgui.BulletText("Official Topic: ")
    imgui.SameLine()
    if imgui.Button("Copy link to clipboard",imgui.ImVec2(150,20)) then
        imgui.SetClipboardText(script.this.url)
        printHelpString("Link copy ~g~successful.")
    end
end

return module
