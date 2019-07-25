-- This module contains functions for visuals tab

local module = {}

local tvisuals =
{
    show_coordinates = imgui.new.bool(fconfig.get('tvisuals.show_coordinates') or false),
    zone_names = imgui.new.bool(true),
    car_names  = imgui.new.bool(true),
}

module.tvisuals = tvisuals

function module.VisualsMain()
    if imgui.BeginTabBar("Visuals") then
        imgui.Spacing()
        if imgui.BeginTabItem("Checkboxes") then
            imgui.Spacing()

            imgui.Columns(2,nil,false)

            fcommon.CheckBox({ address = 0xA444A0,name = "Enable Hud"})
            fcommon.CheckBox({ address = 0xA444A4,name = "Gray radar"})
            fcommon.CheckBox({ address = 0xBA676C,name = "Enable Radar",value = 0,value2 = 2})
            fcommon.CheckBox({ address = 0x58F58D,name = "Money font border"})
            fcommon.CheckBox({ address = 0x589123,name = "Body armour border"})
            fcommon.CheckBox({ address = 0x589207,name = "Breath meter border"})


            imgui.NextColumn()
            fcommon.CheckBox({ name = "Show Coordinates",var = tvisuals.show_coordinates,show_help_popups = true,help_text = "Draws coordinates on-screen.\n\nOptions:\nPressing (Left Ctrl + C) stores coords to clipboard."})
            fcommon.CheckBox({ address = 0x589355,name = "Health bar percentage"})
            fcommon.CheckBox({ address = 0x589125,name = "Body armour percentage"})
            fcommon.CheckBox({ address = 0x589209,name = "Breath meter percentage"})
            fcommon.CheckBox({ address = 0x58DD1B,name = "Hide wanted level",value = 0x90})

            if imgui.Checkbox("Display zone names",tvisuals.zone_names) then
                displayZoneNames(tvisuals.zone_names[0])
                if tvisuals.zone_names[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
            end

            if imgui.Checkbox("Display car names",tvisuals.car_names) then
                displayCarNames(tvisuals.car_names[0])
                if tvisuals.car_names[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
            end

            imgui.Columns(1)
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Menus") then
            if imgui.BeginChild("Menus") then
                imgui.Spacing()
                imgui.Text("Tweak with caution")
                imgui.Spacing()
                fcommon.UpdateAddress({ name = "Radar X Position",address = 0x858A10,size = 4,min=-999,max = 999,is_float = true,help_text = "Changes radar vertical position"})
                fcommon.UpdateAddress({ name = "Radar Y Position",address = 0x866B70,size = 4,min=-999,max = 999,is_float = true,help_text = "Changes radar horizantal position"})
                fcommon.UpdateAddress({ name = "Wanted star border",address = 0x58DD41,size = 1,max = 3})
                fcommon.UpdateAddress({ name = "Active wanted star Y-pos",address = 0x858CCC,size = 4,is_float = true,min=-999,max = 999})
                fcommon.UpdateAddress({ name = "Wanted star color",address = 0x58DDC9,size = 1,max = 30})
                fcommon.UpdateAddress({ name = "Money color id",address = 0x58F492 ,size = 1,max = 30})
                fcommon.UpdateAddress({ name = "Debt color id",address = 0x58F4D4 ,size = 1,max = 30})
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
