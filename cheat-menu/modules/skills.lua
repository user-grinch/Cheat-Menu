-- This module contains all the function for skills tab

local module = {}

function module.skills_section()
    imgui.Spacing()
    imgui.Text("Skills")
    imgui.Separator()
    imgui.Spacing()

    
    if imgui.Button("Max Vehicle Skills",imgui.ImVec2(155,50)) then
        callFunction(0x4399D0,1,1,false)
        displayNonMinigameHelpMessages(false)
        fcommon.CheatActivated()
    end
    imgui.SameLine()
    if imgui.Button("Max Weapon Skills",imgui.ImVec2(155,50)) then     
        for i=69,79,1 do
            setFloatStat(i)
        end         
        fcommon.CheatActivated()
    end

    imgui.Spacing()
    imgui.Text("Vehicles")
    imgui.Separator()
    imgui.Spacing()
    fcommon.update_stat({ name = "Driving",stat = 160})
    fcommon.update_stat({ name = "Flying",stat = 223})
    fcommon.update_stat({ name = "Bike",stat = 229})
    fcommon.update_stat({ name = "Cycling",stat = 230})

    imgui.Spacing()
    imgui.Text("Weapons")
    imgui.Separator()
    imgui.Spacing()
    fcommon.update_stat({ name = "Pistol",stat = 69})
    fcommon.update_stat({ name = "Silenced Pistol",stat = 70})
    fcommon.update_stat({ name = "Desert Eagle",stat = 71})
    fcommon.update_stat({ name = "Shotgun",stat = 72})
    fcommon.update_stat({ name = "Sawn-Off Shotgun",stat = 73})
    fcommon.update_stat({ name = "Combat Shotgun",stat = 74})
    fcommon.update_stat({ name = "Machine Pistol",stat = 75})
    fcommon.update_stat({ name = "SMG",stat = 76})
    fcommon.update_stat({ name = "AK-47",stat = 77})
    fcommon.update_stat({ name = "M4",stat = 78})
    fcommon.update_stat({ name = "Rifle",stat = 79})
    imgui.Spacing()
    imgui.Text("More Skills")
    imgui.Separator()
    imgui.Spacing()
    fcommon.update_stat({ name = "Appearance",stat = 80})
    fcommon.update_stat({ name = "Gambling",stat = 81})
end

return module