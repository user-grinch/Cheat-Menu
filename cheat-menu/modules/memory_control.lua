-- This module contains all the functions related to memory control tab

local module = {}

-- Memory table
local tmemory =
{
    rw = imgui.ImInt(0), -- 1 = write , 0 = read
    address = imgui.ImBuffer(9),
    size = imgui.ImInt(),
    vp = imgui.ImBool(false),
    clear_entries = imgui.ImBool(false),
    value = imgui.ImInt(),
    is_float = imgui.ImBool()
}

function module.memory_control_section()

    if isKeyDown(config.keys.control_key) 
    and isKeyDown(config.keys.mc_paste) then
        tmemory.address.v =  imgui.GetClipboardText()
    end

    if imgui.RadioButton("Read",tmemory.rw,0) then
    end
    imgui.SameLine()
    if imgui.RadioButton("Write",tmemory.rw,1) then
    end
    imgui.Spacing()
    imgui.Separator()
    imgui.Spacing()  

    if tmemory.rw.v == 0 then
        imgui.Text("Memory Value : " .. tmemory.value.v)
        imgui.Spacing()  
        imgui.InputText("Address", tmemory.address)
        fcommon.information_tooltip("You can simply paste copied address using Ctrl + V")
        imgui.SliderInt("Size", tmemory.size,1,4)
        if tmemory.size.v == 4 then
            if imgui.Checkbox("Float",tmemory.is_float) then 
            end
            imgui.SameLine()
        end
        imgui.Checkbox("Virtual Protect", tmemory.vp)
        imgui.SameLine()
        imgui.Checkbox("Clear entries", tmemory.clear_entries)
        if imgui.Button("Read Address",imgui.ImVec2(100,20)) then --and #tmemory.address.v == 8 then
            tmemory.value.v = fcommon.rw_memory(tonumber(tmemory.address.v),tmemory.size.v,nil,tmemory.vp.v,tmemory.is_float.v)
            if tmemory.clear_entries.v == 1 then
                tmemory.value.v = 0
                tmemory.address.v = ""
                tmemory.size.v = 0
                tmemory.vp.v = false
                tmemory.is_float.v = false
            end
           
        end
    else
        imgui.InputText("Address", tmemory.address)
        imgui.InputInt("Value", tmemory.value)
        imgui.SliderInt("Size", tmemory.size,1,4)
        if tmemory.size.v == 4 then
            if imgui.Checkbox("Float",tmemory.is_float) then 
            end
            imgui.SameLine()
        end
        imgui.Checkbox("Virtual Protect", tmemory.vp)
        imgui.SameLine()
        imgui.Checkbox("Clear entries", tmemory.clear_entries)
        if imgui.Button("Write Address",imgui.ImVec2(100,20)) and #tmemory.address.v == 8 then
            fcommon.rw_memory(tonumber(tmemory.address.v),tmemory.size.v,tmemory.value.v,tmemory.vp.v,tmemory.is_float.v)
            if tmemory.clear_entries.v == 1 then
                tmemory.value.v = 0
                tmemory.address.v = ""
                tmemory.size.v = 0
                tmemory.vp.v = false
                tmemory.is_float.v = false
            end
        end
    end
end

return module