-- This module contains functions for visuals tab

local module = {}
local show_coordinates = imgui.ImBool(false)

local resX,resY = getScreenResolution()

local tdisplay = 
{
    text =
    {
        X = imgui.ImInt(math.floor(resX/100)),
        Y = imgui.ImInt(math.floor(resY/1.16)),
    },
    zone_names = imgui.ImBool(true),
    car_names  = imgui.ImBool(true),
}
module.tdisplay = tdisplay
module.show_coordinates = show_coordinates



function sphere_color(title,address)
    value = imgui.ImInt(memory.read(address,4))
    if imgui.InputInt(title,value) then
        memory.write(address,value.v,4)
    end
end

function module.visuals_section()
    imgui.Spacing()
    imgui.Text("Check Boxes")
    imgui.Separator()
    imgui.Spacing() 

    imgui.Columns(2)

    fcommon.check_box({ address = 0xA444A0,name = "Enable Hud"})
    fcommon.check_box({ address = 0xA444A4,name = "Gray radar"})
    fcommon.check_box({ address = 0xBA676C,name = "Enable Radar",value = 0,value2 = 2})
    fcommon.check_box({ address = 0x58F58D,name = "Money font border"})
    fcommon.check_box({ address = 0x589123,name = "Body armour border"})
    fcommon.check_box({ address = 0x589207,name = "Breath meter border"})
    

    imgui.NextColumn()

    fcommon.ValueSwitch({ name = "Show Coordinates",var = show_coordinates,show_help_popups = true,help_text = "Draws coordinates on-screen.\n\nOptions:\nPressing (Left Ctrl + C) stores coords to clipboard."})
    fcommon.check_box({ address = 0x589355,name = "Health bar percentage"})
    fcommon.check_box({ address = 0x589125,name = "Body armour percentage"})
    fcommon.check_box({ address = 0x589209,name = "Breath meter percentage"})
    fcommon.check_box({ address = 0x58DD1B,name = "Hide 0 star wanted level",value = 0x90})

    if imgui.Checkbox("Display zone names",tdisplay.zone_names) then
        displayZoneNames(tdisplay.zone_names.v)
        if tdisplay.zone_names.v then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
    end

    if imgui.Checkbox("Display car names",tdisplay.car_names) then
        displayCarNames(tdisplay.car_names.v)
        if tdisplay.car_names.v then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
    end
    

    imgui.Columns(1)

    imgui.Spacing()
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing() 
    if imgui.BeginMenu("Text draws") then
        imgui.Text("Text draws")
        fcommon.information_tooltip("Controls the position of text draws(FPS,car health,car speed,car gear...)")
        imgui.Separator()
        imgui.Spacing()
        local resX,resY = getScreenResolution()
        if imgui.SliderInt("Set X",tdisplay.text.X,0,resX) then end
        if imgui.SliderInt("Set Y",tdisplay.text.Y,0,resY) then end
        imgui.EndMenu()
    end
    if imgui.BeginMenu("Sphere Color") then
        imgui.Text("Sphere Color")
        imgui.Separator()
        imgui.Spacing()
        sphere_color("Red",0x725BD0)
        sphere_color("Green",0x725BCC)
        sphere_color("Blue",0x725BD5)
        imgui.EndMenu()
    end

    fcommon.popup_menu({ name = "Radar X Position",address = 0x858A10,size = 4,max = 999999,help_text = "Changes radar vertical position"})
    fcommon.popup_menu({ name = "Radar Y Position",address = 0x866B70,size = 4,max = 999999,help_text = "Changes radar horizantal position"})
    fcommon.popup_menu({ name = "Wanted star border",address = 0x58DD41,size = 1,max = 3})         
    fcommon.popup_menu({ name = "Active wanted star Y-pos",address = 0x858CCC,size = 4,max = 999999})
    fcommon.popup_menu({ name = "Wanted star color",address = 0x58DDC9,size = 1,max = 30})
    fcommon.popup_menu({ name = "Money color id",address = 0x58F492 ,size = 1,max = 30})
    fcommon.popup_menu({ name = "Debt color id",address = 0x58F4D4 ,size = 1,max = 30})
    
end


return module