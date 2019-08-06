-- This module contains all the functions related to memory control tab

local module = {}

-- Memory table
local tmemory =
{
    address = imgui.new.char[10](fconfig.get('tmemory.address',"")),
    size = imgui.new.int(fconfig.get('tmemory.size',1)),
    vp = imgui.new.bool(fconfig.get('tmemory.vp',false)),
    clear_entries = imgui.new.bool(fconfig.get('tmemory.clear_entries',false)),
    value = imgui.new.int(fconfig.get('tmemory.value',0)),
    is_float = imgui.new.bool(fconfig.get('tmemory.is_float',false)),
}

module.tmemory = tmemory

function module.MemoryMain()
 
    if isKeyDown(tkeys.control_key)
    and isKeyDown(tkeys.mc_paste) then
        imgui.StrCopy(tmemory.address, imgui.GetClipboardText(),ffi.sizeof(tmemory.address))
    end

    if imgui.BeginTabBar(flanguage.GetText("memory.Memory")) then
        if imgui.BeginTabItem(flanguage.GetText("memory.Read")) then

            imgui.Text(flanguage.GetText("memory.MemoryValue") .. " : " .. tmemory.value[0])
            imgui.Spacing()
            imgui.InputText(flanguage.GetText("memory.Address"), tmemory.address,ffi.sizeof(tmemory.address))
            fcommon.InformationTooltip(flanguage.GetText("memory.AddressToolTip"))
            imgui.SliderInt(flanguage.GetText("memory.Size"), tmemory.size,1,4)

            if tmemory.size[0] == 4 then
                imgui.Columns(3,nil,false)
                imgui.Checkbox(flanguage.GetText("memory.Float"),tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(2,nil,false)
            end


            imgui.Checkbox(flanguage.GetText("memory.VirtualProtect"), tmemory.vp)
            imgui.NextColumn()
            imgui.Checkbox(flanguage.GetText("memory.ClearEntries"), tmemory.clear_entries)
            imgui.Columns(1)

            if imgui.Button(flanguage.GetText("memory.ReadAddress"),imgui.ImVec2(fcommon.GetSize(1))) then

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

        if imgui.BeginTabItem(flanguage.GetText("memory.Write")) then
            imgui.InputText(flanguage.GetText("memory.Address"), tmemory.address,ffi.sizeof(tmemory.address))
            imgui.InputInt(flanguage.GetText("memory.Value"), tmemory.value)
            imgui.SliderInt(flanguage.GetText("memory.Size"), tmemory.size,1,4)

            if tmemory.size[0] == 4 then
                imgui.Columns(3,nil,false)
                imgui.Checkbox(flanguage.GetText("memory.Float"),tmemory.is_float)
                imgui.NextColumn()
            else
                imgui.Columns(2,nil,false)
            end


            imgui.Checkbox(flanguage.GetText("memory.VirtualProtect"), tmemory.vp)
            imgui.NextColumn()
            imgui.Checkbox(flanguage.GetText("memory.ClearEntries"), tmemory.clear_entries)
            imgui.Columns(1)

            if imgui.Button(flanguage.GetText("memory.WriteAddress"),imgui.ImVec2(fcommon.GetSize(1))) then
                if ffi.string(tmemory.address) ~= "" then
                    fcommon.RwMemory(tonumber(ffi.string(tmemory.address)),tmemory.size[0],tmemory.value[0],tmemory.vp[0],tmemory.is_float[0])
                    if tmemory.clear_entries[0] == 1 then
                        tmemory.value[0] = 0
                        imgui.StrCopy(tmemory.address,"")
                        tmemory.size[0] = 0
                        tmemory.vp[0] = false
                        tmemory.is_float[0] = false
                    end
                    printHelpString(flanguage.GetText("memory.CompleteMSG"))
                end
            end

            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
