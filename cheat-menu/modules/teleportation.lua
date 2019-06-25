-- This module contains all the functions related to teleportation tab

local module = {}

-- Teleport table
local tteleport =
{
    shotcut =  imgui.ImBool(false),
    coords  =  imgui.ImBuffer(24),
    current_coords = imgui.ImBool(false),
}

module.tteleport = tteleport


function module.teleport(x,y,z)
    if x == nil then
        bool, x,y,z = getTargetBlipCoordinates()
        z = -100
    end
    if x ~= 0 and y ~= 0 then
        setCharCoordinates(PLAYER_PED,x,y,z) 
    end
end


function module.teleportation_section()
    imgui.Spacing()
    x,y,z = getCharCoordinates(PLAYER_PED)
    imgui.Text(string.format("Teleportation (Current Coordinates: %d %d %d)", math.floor(x) , math.floor(y) , math.floor(z)))  
    imgui.Separator()
    
    imgui.Columns(2)
    fcommon.ValueSwitch({name = "Quick teleport",var = tteleport.shotcut,help_text = "Teleport to marker can be quickly accessed by pressing (X + Y) key combinartion."})
    imgui.NextColumn()
    fcommon.ValueSwitch({name = "Current Coordinates",var = tteleport.current_coords,help_text = "Set current coordinates"})
    imgui.Columns(1)

    if imgui.InputText("Co-ordinates",tteleport.coords,24) then
    end 
    if tteleport.current_coords.v then
        tteleport.coords.v = string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z))
    end
    fcommon.information_tooltip("Enter XYZ coordinates.\nFormat : X,Y,Z")
    imgui.Dummy(imgui.ImVec2(0,10))
    
    if imgui.Button("Teleport to coord",imgui.ImVec2(fcommon.getsize(2),0)) then
        local x,y,z = tteleport.coords.v:match("([^,]+),([^,]+),([^,]+)")
        module.teleport(x,y,z)
    end
    imgui.SameLine()
    if imgui.Button("Teleport to marker",imgui.ImVec2(fcommon.getsize(2),0)) then
        module.teleport()
    end  
end

return module