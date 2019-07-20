-- This module contains all the functions related to memory control tab

local module = {}

-- Memory table
local tmemory =
{
    address = imgui.new.char[10](),
    size = imgui.new.int(1),
    vp = imgui.new.bool(false),
    clear_entries = imgui.new.bool(false),
    value = imgui.new.int(),
    is_float = imgui.new.bool()
}

function module.MemoryMain()

    if isKeyDown(keys.control_key)
    and isKeyDown(keys.mc_paste) then
        imgui.StrCopy(tmemory.address, imgui.GetClipboardText(),ffi.sizeof(tmemory.address))
    end

    if imgui.BeginTabBar('Memory') then
        if imgui.BeginTabItem('Read') then

            imgui.Text("Memory Value : " .. tmemory.value[0])
            imgui.Spacing()
            imgui.InputText("Address", tmemory.address,ffi.sizeof(tmemory.address))
            fcommon.InformationTooltip("You can simply paste copied address using Ctrl + V")
            imgui.SliderInt("Size", tmemory.size,1,4)

            if tmemory.size[0] == 4 then
                imgui.Columns(3,nil,false)
                imgui.Checkbox("Float",tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(2,nil,false)
            end


            imgui.Checkbox("Virtual Protect", tmemory.vp)
            imgui.NextColumn()
            imgui.Checkbox("Clear entries", tmemory.clear_entries)
            imgui.Columns(1)

            if imgui.Button("Read Address",imgui.ImVec2(fcommon.GetSize(1))) then

                if ffi.string(tmemory.address) ~= "" then
                    tmemory.value[0] = fcommon.RwMemory(tonumber(ffi.string(tmemory.address)),tmemory.size[0],nil,tmemory.vp[0],tmemory.is_float[0])
                    if tmemory.clear_entries[0] == 1 then
                        tmemory.value[0] = 0
                        imgui.StrCopy(tmemory.address,"")
                        tmemory.size[0] = 0
                        tmemory.vp[0] = false
                        tmemory.is_float[0] = false
                    end
                end
            end

            imgui.EndTabItem()
        end

        if imgui.BeginTabItem('Write') then
            imgui.InputText("Address", tmemory.address,ffi.sizeof(tmemory.address))
            imgui.InputInt("Value", tmemory.value)
            imgui.SliderInt("Size", tmemory.size,1,4)

            if tmemory.size[0] == 4 then
                imgui.Columns(3,nil,false)
                imgui.Checkbox("Float",tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(2,nil,false)
            end


            imgui.Checkbox("Virtual Protect", tmemory.vp)
            imgui.NextColumn()
            imgui.Checkbox("Clear entries", tmemory.clear_entries)
            imgui.Columns(1)

            if imgui.Button("Write Address",imgui.ImVec2(fcommon.GetSize(1))) then
                if ffi.string(tmemory.address) ~= "" then
                    fcommon.RwMemory(tonumber(ffi.string(tmemory.address)),tmemory.size[0],tmemory.value[0],tmemory.vp[0],tmemory.is_float[0])
                    if tmemory.clear_entries[0] == 1 then
                        tmemory.value[0] = 0
                        imgui.StrCopy(tmemory.address,"")
                        tmemory.size[0] = 0
                        tmemory.vp[0] = false
                        tmemory.is_float[0] = false
                    end
                    printHelpString("Memory ~g~Modified")
                end
            end

            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
