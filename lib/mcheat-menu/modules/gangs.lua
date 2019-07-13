-- This module contains all the functions related to gangs

local module = {}


local gang_wars = imgui.ImBool(false)
-- SA Gang table
local gang_density = {
    ["Ballas"] = imgui.ImInt(),
    ["Grove Street Families"] = imgui.ImInt(),
    ["Los Santos Vagos"] = imgui.ImInt(),
    ["San Fierro Rifa"] = imgui.ImInt(),
    ["Da Nang Boys"] = imgui.ImInt(),
    ["Mafia"] = imgui.ImInt(),
    ["Mountain Cloud Triad"] = imgui.ImInt(),
    ["Varrio Los Aztecas"] = imgui.ImInt(),
    ["Gang 9"] = imgui.ImInt(),
    ["Gang 10"] = imgui.ImInt()
}

-- Sets gang density in current zone
function set_density(title,id)
    x,y,z = getCharCoordinates(PLAYER_PED)
        
    gang_density[title].v = getZoneGangStrength((getNameOfInfoZone(x,y,z)),id)
    if imgui.SliderInt(title,gang_density[title],0,255) then
        setZoneGangStrength((getNameOfInfoZone(x,y,z)),id,gang_density[title].v)
        clearSpecificZonesToTriggerGangWar()
        setGangWarsActive(true)
    end
end

-- List of all gangs to set density
function  zone_density()
    if imgui.BeginMenu("Zone Gang Density") then
        imgui.Text("Zone Gang Density")
        imgui.Separator()
        imgui.Spacing()
        set_density("Ballas",0)
        set_density("Grove Street Families",1)
        set_density("Los Santos Vagos",2)
        set_density("San Fierro Rifa",3)
        set_density("Da Nang Boys",4)
        set_density("Mafia",5)
        set_density("Mountain Cloud Triad",6)
        set_density("Varrio Los Aztecas",7)
        set_density("Gang 9",8)
        set_density("Gang 10",9)
        imgui.Spacing()
        imgui.Text("Note:You'll need ExGangWars plugin to get turf colors for some gangs.")
        imgui.EndMenu()
    end
end

function module.gangs_section()
    imgui.Text("Checkboxes")
    imgui.Separator()
    imgui.Spacing()
    fcommon.check_box({ address = 0x96915B,name = "Gangs control the streets"})
    fcommon.check_box({ address = 0x96915A,name = "Gang members everywhere"})
    if imgui.Checkbox("Gang wars",gang_wars) then
        setGangWarsActive(gang_wars.v)
        if gang_wars.v then fcommon.CheatActivated() else fcommon.CheatDeactivated() end
    end
    zone_density()
end

return module