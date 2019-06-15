--[[ This file contains about tab functions

    Please don't remove credits below.
]]--
local module = {}

function module.about_section()
    imgui.BulletText(string.format('Version : %.2f (%s)',version.number,version.release))
    imgui.BulletText("Author  : Grinch_")
    imgui.BulletText("Discord : Grinch_#3311")
    imgui.BulletText("Thanks  : Junior-Djjr,Um_Geek & GTA community.")
    imgui.BulletText("Official Topic: ")
    imgui.SameLine()
    if imgui.Button("Copy link to clipboard",imgui.ImVec2(150,20)) then
        imgui.SetClipboardText("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
        printHelpString("Link copy ~g~successful.")
    end
end

return module