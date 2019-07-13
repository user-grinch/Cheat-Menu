-- This module contains all the functions related to teleportation tab

local module = {}

-- Teleport table
local tteleport =
{
    shotcut =  imgui.new.bool(false),
    coords  =  imgui.new.char[24](),
    current_coords = imgui.new.bool(false),
}

module.tteleport = tteleport


function module.Teleport(x,y,z)
    if x == nil then
        bool, x,y,z = getTargetBlipCoordinates()
        z = -100
    end
    if x ~= 0 and y ~= 0 then
        setCharCoordinates(PLAYER_PED,x,y,z)
        printHelpString("~g~Teleported ~w~to location.")
    end
end


function module.TeleportationMain()

    if imgui.BeginTabBar('Teleportation') then
        if imgui.BeginTabItem('Teleport') then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.ValueSwitch("Quick teleport",fteleport.tteleport.shotcut,nil,"Teleport to marker can be quickly accessed by pressing (X + Y) key combinartion.")
            imgui.NextColumn()
            fcommon.ValueSwitch("Current Coordinates",fteleport.tteleport.current_coords,nil,"Set current coordinates")
            imgui.Columns(1)

            if imgui.InputText("Co-ordinates",tteleport.coords,ffi.sizeof(tteleport.coords)) then end

            if tteleport.current_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
            end

            fcommon.InformationTooltip("Enter XYZ coordinates.\nFormat : X,Y,Z")
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button("Teleport to coord",imgui.ImVec2(fcommon.GetSize(2))) then
                local x,y,z = (ffi.string(tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
                module.Teleport(x,y,z)
            end
            imgui.SameLine()
            if imgui.Button("Teleport to marker",imgui.ImVec2(fcommon.GetSize(2))) then
                module.Teleport()
            end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
