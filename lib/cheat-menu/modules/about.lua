--[[ This file contains about tab functions

    Please don't remove credits below.
]]--
local module = {}

function module.AboutMain()
    imgui.BulletText(string.format("%s v%s(%d)",script.this.name,script.this.version,script.this.version_num))
    imgui.BulletText(string.format("%s : %s",flanguage.GetText('about.Author'),script.this.authors[1]))
    imgui.BulletText(string.format("%s : v%s",flanguage.GetText('about.Imgui'),imgui._VERSION))
    imgui.BulletText(flanguage.GetText('about.Discord') .. " : Grinch_#3311")
    imgui.BulletText(flanguage.GetText('about.Thanks') .. " : Junior-Djjr,Um_Geek & GTA community.")
    imgui.BulletText(flanguage.GetText('about.OfficialTopic') .. " : ")
    imgui.SameLine()
    if imgui.Button(flanguage.GetText('about.CopyToClipboard'),imgui.ImVec2(150,20)) then
        imgui.SetClipboardText(script.this.url)
        printHelpString(flanguage.GetText('about.CopySuccessMSG'))
    end
end

return module
