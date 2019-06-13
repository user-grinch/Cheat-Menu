--[[ This file contains about tab functions

    Please don't remove credits below.
]]--
local module = {}

function module.about_section()
    local width =  imgui.GetWindowWidth()
    local text = "Cheat Menu"
    imgui.Dummy(imgui.ImVec2((width+string.len(text))/3,5))
    imgui.SameLine()
    imgui.Text(text)
    imgui.Spacing()
    imgui.Separator()
    imgui.BulletText(string.format('Version : %.2f (%s)',config.version.number,config.version.release))
    imgui.BulletText("Author  : Grinch_")
    imgui.BulletText("Discord : Grinch_#3311")
    imgui.BulletText("Thanks  : Junior-Djjr,Um_Geek & people of mixmods.")
    imgui.BulletText("Official Topic: ")
    imgui.SameLine()
    if imgui.Button("Copy link to clipboard",imgui.ImVec2(150,20)) then
        imgui.SetClipboardText("https://forum.mixmods.com.br/f5-scripts-codigos/t1777-lua-cheat-menu")
        printHelpString("Link copy ~g~successful.")
    end
    imgui.Separator()
end

return module