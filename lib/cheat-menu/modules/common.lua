-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019 Grinch_

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

local module = {}

function module.DropDownMenu(label,func)
    if label ~= nil then
        if imgui.CollapsingHeader(label) then
           imgui.Spacing()

            func()

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
        end
    else
        imgui.Spacing()

        func()

        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
    end
end


function module.QuickSpawner()

    fcommon.KeyWait(tkeys.control_key,tkeys.quickspawner_key)

    memory.write(0x00969110,0,1)
    result = ''
    while not wasKeyPressed(0x0D) do
        if #result > 25 then
            result = ''
        end
        if wasKeyPressed(0x2E) then
            result = ''
        elseif wasKeyPressed(0x08) then
            result = result:sub(1, -2)
        elseif wasKeyPressed(readMemory(0x00969110,1,false)) then
            result = string.format('%s%s',result,memory.tostring(0x00969110,1,false));
        end

        printStringNow(string.format('[%s]',result),1500)

        text = result

        for i = 0,#result,1 do

            local weapon =  fweapon.CBaseWeaponInfo(text)

            if fweapon.tweapon.quick_spawn[0] == true and weapon ~= 0 then
                fweapon.GiveWeapon(weapon)
                return
            end

            local vehicle = fvehicle.CBaseModelInfo(text)

            if fvehicle.tvehicle.quick_spawn[0] == true and vehicle ~= 0 then
                fvehicle.GiveVehicleToPlayer(vehicle)
                return
            end
            text = text:sub(2)
        end
        wait(0)
    end
end

function module.LoadJson(filename)
    local full_path = tcheatmenu.dir .. "json//" .. filename .. ".json"
    if doesFileExist(full_path) then
        local file = io.open(full_path, "r")
        local table = decodeJson(file:read("*a"))
        file:close()
        return table
    end
    return {}
end

function module.SaveJson(filename,table)
    local full_path = tcheatmenu.dir .. "json//" .. filename .. ".json"
    local file = assert(io.open(full_path, "w"))
    file:write(encodeJson(table))
    file:close()
end


function module.InformationTooltip(text)
    if fmenu.tmenu.show_tooltips[0] then
        imgui.SameLine()
        imgui.TextColored(imgui.ImVec4(128,128,128,0.3),'(?)')
        if imgui.IsItemHovered() then
            imgui.BeginTooltip()
            imgui.Spacing()
            imgui.SetTooltip(text)
            imgui.Dummy(imgui.ImVec2(50,50))
            imgui.EndTooltip()
        end
    end
end

function module.CheatActivated()
    printHelpString("Cheat ~g~Activated")
end

function module.CheatDeactivated()
    printHelpString("Cheat ~r~Deactivated")
end

function module.GetSize(count,x,y)
    -- minimum
    if x == nil then  x = 20 end
    if y == nil then  y = 25 end

    if ((imgui.GetWindowWidth() - imgui.StyleVar.FramePadding - imgui.StyleVar.ItemSpacing) / count) > x then
        x = (imgui.GetWindowWidth() - imgui.StyleVar.FramePadding - imgui.StyleVar.ItemSpacing) / (count-0.014)
    end

    if ((imgui.GetWindowHeight()/25) / count) > y then
        y = (imgui.GetWindowHeight()/25)
    end

    return x,y
end

function module.CreateMenus(names,funcs)

    imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing,imgui.ImVec2(0,0))
    
    for i=1,#names,1 do
        if tcheatmenu.current_menu == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.060,0.530,0.980,1.0))
        end
        if imgui.Button(names[i],imgui.ImVec2(module.GetSize(4,imgui.GetWindowWidth()/4 - 4*imgui.StyleVar.WindowPadding,20))) then
            tcheatmenu.current_menu = i
        end
        if tcheatmenu.current_menu == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.260,0.590,0.980,0.400))
        end
        if i%4 ~= 0 then
            imgui.SameLine()
        end
    end

    imgui.PopStyleVar()
    imgui.Spacing()
    funcs[tcheatmenu.current_menu]()
  
end

function LoadTextures(store_table,image_path,model_table,extention)
    for i=1,#model_table,1 do
        if store_table[tostring(model_table[i])] == nil then
            local path = image_path .. tostring(model_table[i]) .. extention
            store_table[tostring(model_table[i])] = imgui.CreateTextureFromFile(path)
        end
        wait(0)
    end
end

function module.ShowEntries(title,model_table,height,width,store_table,image_path,image_extention,func_load_model,func_show_tooltip,skip_check,search_text,body_part)

    local content_width = (imgui.GetWindowWidth() - imgui.StyleVar.FramePadding - imgui.StyleVar.ItemSpacing)

    local rows = math.floor(content_width/width)

    if search_text == nil then search_text = "" end

    local car = storeCarCharIsInNoSave(PLAYER_PED)
    for i=1,#model_table,1 do
        if skip_check == true or fvehicle.IsValidModForVehicle(model_table[i],getCarPointer(car)) then
            fcommon.DropDownMenu(title,function()
                local skipped_entries = 0
                for j=1,#model_table,1 do
                    if store_table[tostring(model_table[j])] ~= nil then
                        if (search_text == "") or (string.upper(func_show_tooltip(model_table[j])):find(string.upper(ffi.string(search_text))) ~= nil) then
                            if skip_check == true or fvehicle.IsValidModForVehicle(model_table[j],getCarPointer(car)) then
                                if imgui.ImageButton(store_table[tostring(model_table[j])],imgui.ImVec2(width,height),imgui.ImVec2(0,0),imgui.ImVec2(1,1),1,imgui.ImVec4(1,1,1,1),imgui.ImVec4(1,1,1,1)) then
                                    if body_part == nil then
                                        func_load_model(model_table[j])
                                    else
                                        func_load_model(model_table[j],body_part)
                                    end
                                end
                                if func_show_tooltip == nil then
                                    if imgui.IsMouseClicked(1) then
                                        removeVehicleMod(car,model_table[j])
                                        printHelpString("Component ~r~removed")
                                    end
                                end
                                
                                if func_show_tooltip ~= nil then
                                    if imgui.IsItemHovered() then
                                        if func_show_tooltip(model_table[j]) ~= "" then
                                            imgui.BeginTooltip()
                                            imgui.SetTooltip(func_show_tooltip(model_table[j]))
                                            imgui.EndTooltip()
                                        end
                                    end
                                end
                            end
                        else
                            skipped_entries = skipped_entries +1
                        end
                        if (j-skipped_entries) % rows ~= 0 then
                            imgui.SameLine()
                        end
                    else
                        if doesFileExist(image_path..model_table[j]..image_extention) then
                            lua_thread.create(LoadTextures,store_table,image_path,model_table,image_extention)
                        else
                            skipped_entries = skipped_entries +1
                        end
                    end
                end
            end)
        end
        break
    end
end

function module.RadioButton(label,rb_table,addr_table)
    local button = imgui.new.int(#addr_table + 1)

    imgui.Text(label)

    for i = 1, #addr_table,1 do
        if readMemory(addr_table[i],1,false) == 1 then
            button[0] = i
        end
        if imgui.RadioButtonIntPtr(rb_table[i],button,i) then
            for j = 1,#addr_table,1 do
                writeMemory(addr_table[j],1,0,false)
            end
            button[0] = i
            writeMemory(addr_table[i],1,1,false)
            module.CheatActivated()
        end
    end
    if imgui.RadioButtonIntPtr("Default ##" ..label,button,#addr_table + 1) then
        for j = 1,#addr_table,1 do
            writeMemory(addr_table[j],1,0,false)
        end
        module.CheatActivated()
    end
end

function module.CheckBox(arg)
    arg.value = arg.value or 1
    arg.value2 = arg.value2 or 0
    arg.var = arg.var or nil
    arg.func = arg.func or nil

    local func_bool =  arg.var or imgui.new.bool()
    if arg.var == nil then
        if (readMemory(arg.address,1,false)) == arg.value2 then
            func_bool[0] = false
        else
            func_bool[0] = true
        end
    end

    if imgui.Checkbox(arg.name,func_bool) then
        if arg.func == nil then
            if arg.var == nil then
                if func_bool[0] then
                    writeMemory(arg.address,1,arg.value,false)
                    module.CheatActivated()
                else
                    writeMemory(arg.address,1,arg.value2,false)
                    module.CheatDeactivated()
                end
            else
                if arg.var[0] then
                    module.CheatActivated()
                else
                    module.CheatDeactivated()
                end
                if arg.func ~= nil then arg.func() end
            end
        else
            arg.func()
        end
    end

    if arg.help_text ~= nil then
        module.InformationTooltip(arg.help_text)
    end
end



function module.UpdateStat(arg)
    if arg.min == nil then arg.min = 0 end
    if arg.default == nil then arg.default = 0 end
    if arg.max == nil then arg.max = 1000 end

    module.DropDownMenu(arg.name,function()

        if arg.help_text ~= nil then
            fcommon.InformationTooltip(arg.help_text)
        end

        local change_value = math.floor((arg.max - arg.min)/10)
        local value = imgui.new.int(math.floor(getFloatStat(arg.stat)))

        imgui.Columns(2,nil,false)
        if arg.min ~= nil then
            imgui.Text("Minimum = " .. arg.min)
        end
        imgui.NextColumn()
        if arg.max ~= nil then
            imgui.Text("Maximum = " .. arg.max)
        end

        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-70)
        if imgui.InputInt("Set ##".. arg.name,value) then
            setFloatStat(arg.stat,value[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Minimum ##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.min)
        end
        imgui.SameLine()
        if imgui.Button("Default ##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.default)
        end
        imgui.SameLine()
        if imgui.Button("Maximum ##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.max)
        end
        if value[0] < arg.min then
            setFloatStat(arg.stat,arg.min)
        end
        if value[0] > arg.max then
            setFloatStat(arg.stat,arg.max)
        end
    end)
end

function module.RwMemory(address,size,value,protect,is_float)
    if protect == nil then protect = false end

    if value == nil then
        if is_float == true then
            return memory.getfloat(address,protect)
        else
            return readMemory(address,size,protect)
        end
    else
        if is_float == true then
            memory.setfloat(address,value,protect)
        else
            memory.write(address,value,size,protect)
        end
    end
end

function module.UpdateAddress(arg)
    if arg.default == nil then arg.default = 0 end
    if arg.cvalue == nil then arg.cvalue = 1 end

    module.DropDownMenu(arg.name,function()

        local value = imgui.new.float(module.RwMemory(arg.address,arg.size,nil,nil,arg.is_float))

        imgui.Columns(2,nil,false)
        if arg.min ~= nil then
            imgui.Text("Minimum = " .. arg.min)
        end
        imgui.NextColumn()
        if arg.max ~= nil then
            imgui.Text("Maximum = " .. arg.max)
        end
        imgui.Columns(1)

        imgui.Spacing()

        if imgui.InputFloat("##".. arg.name,value) then
            module.RwMemory(arg.address,arg.size,value[0],nil,arg.is_float)
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("-",imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] - arg.cvalue),nil,arg.is_float)
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("+",imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] + arg.cvalue),nil,arg.is_float)
        end
        imgui.SameLine(0.0,4.0)
        imgui.Text("Set")
        imgui.Spacing()
        if imgui.Button("Minimum ##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Default ##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            module.RwMemory(arg.address,arg.size,arg.default,nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Maximum ##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float)
        end
        imgui.SameLine()
        imgui.Spacing()
        if (arg.min ~= nil) and (value[0] < arg.min) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end

        if (arg.max ~= nil) and (value[0] > arg.max) then
            module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float)
        end
    end)
end

function module.KeyWait(key1,key2)
    while isKeyDown(key1)
    and isKeyDown(key2) do
        wait(0)
    end
end

return module
