-- This module contains all the functions related to gangs

local module = {}

gangs =
{
    wars = imgui.new.bool(false),
    density = {
        ["Ballas"] = imgui.new.int(),
        ["Grove Street Families"] = imgui.new.int(),
        ["Los Santos Vagos"] = imgui.new.int(),
        ["San Fierro Rifa"] = imgui.new.int(),
        ["Da Nang Boys"] = imgui.new.int(),
        ["Mafia"] = imgui.new.int(),
        ["Mountain Cloud Triad"] = imgui.new.int(),
        ["Varrio Los Aztecas"] = imgui.new.int(),
        ["Gang 9"] = imgui.new.int(),
        ["Gang 10"] = imgui.new.int(),
    },
}


function SetDensity(title,id)
    x,y,z = getCharCoordinates(PLAYER_PED)
        
    gang_density[title].v = getZoneGangStrength((getNameOfInfoZone(x,y,z)),id)
    if imgui.SliderInt(title,gang_density[title],0,255) then
        setZoneGangStrength((getNameOfInfoZone(x,y,z)),id,gang_density[title].v)
        clearSpecificZonesToTriggerGangWar()
        setGangWarsActive(true)
    end
end

function module.gangs_section()
    imgui.Text("Checkboxes")
    imgui.Separator()
    imgui.Spacing()
    fcommon.CheckBox({ address = 0x96915B,name = "Gangs control the streets"})
    fcommon.CheckBox({ address = 0x96915A,name = "Gang members everywhere"})

    fcommon.CheckBox({name = "Gang wars",var = tplayer.neverWanted,func = function()
        setGangWarsActive(gang_wars[0])
        if gang_wars[0] then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
    end})

    if imgui.BeginMenu("Zone Gang Density") then
        imgui.Text("Zone Gang Density")
        imgui.Separator()
        imgui.Spacing()
        SetDensity("Ballas",0)
        SetDensity("Grove Street Families",1)
        SetDensity("Los Santos Vagos",2)
        SetDensity("San Fierro Rifa",3)
        SetDensity("Da Nang Boys",4)
        SetDensity("Mafia",5)
        SetDensity("Mountain Cloud Triad",6)
        SetDensity("Varrio Los Aztecas",7)
        SetDensity("Gang 9",8)
        SetDensity("Gang 10",9)
        imgui.Spacing()
        imgui.Text("Note:You'll need ExGangWars plugin to get turf colors for some gangs.")
        imgui.EndMenu()
    end
end

return module