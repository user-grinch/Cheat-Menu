-- This module contains all common functions used all over the menu

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

            local weapon =  fweapons.CBaseWeaponInfo(text)

            if fweapons.tweapons.quick_spawn[0] == true and weapon ~= 0 then
                fweapons.GiveWeaponToPlayer(weapon)
                return
            end

            local vehicle = fvehicles.CBaseModelInfo(text)

            if fvehicles.tvehicles.quick_spawn[0] == true and vehicle ~= 0 then
                fvehicles.GiveVehicleToPlayer(vehicle)
                return
            end
            text = text:sub(2)
        end
        wait(0)
    end
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
    printHelpString(flanguage.GetText("common.CheatActivated"))
end

function module.CheatDeactivated()
    printHelpString(flanguage.GetText("common.CheatDeactivated"))
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

function IsValidModForVehicle(model)
    local CVehicle =  getCarPointer(storeCarCharIsInNoSave(PLAYER_PED))
    if callMethod(0x49B010,CVehicle,2,2,model,CVehicle) == 1 then
        return true
    end
end

function module.UiCreateButtons(names,funcs)

    imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing,imgui.ImVec2(0,0))

    for i=1,#names,1 do
        if tcheatMenu.menubuttons.current == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.060,0.530,0.980,1.0))
        end
        if imgui.Button(names[i],imgui.ImVec2(module.GetSize(4,imgui.GetWindowWidth()/4 - 4*imgui.StyleVar.WindowPadding,20))) then
            tcheatMenu.menubuttons.current = i
        end
        if tcheatMenu.menubuttons.current == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.260,0.590,0.980,0.400))
        end

        if i%4 ~= 0 then
            imgui.SameLine()
        end
    end

    imgui.PopStyleVar()
    imgui.Spacing()

    for i=1,#funcs,1 do
        if tcheatMenu.menubuttons.current == i then
            imgui.Spacing()
            funcs[i]()
            break
        end
    end
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
    local rows = 0
    
    for i=0,20,1 do
        if math.floor(imgui.GetWindowWidth()/i) < width then
            width = (imgui.GetWindowWidth()/(i*1.2))
            if (func_load_model == fvehicles.GiveVehicleToPlayer) or (func_load_model == fvehicles.AddComponentToVehicle) then
                rows = i
            else
                rows = i - 1
            end
            break
        end
    end

    if search_text == nil then search_text = "" end

    
    for i=1,#model_table,1 do
        if skip_check == true or IsValidModForVehicle(model_table[i]) then
            fcommon.DropDownMenu(title,function()
                local skipped_entries = 0
                for j=1,#model_table,1 do
                    if store_table[tostring(model_table[j])] ~= nil then
                        if (search_text == "") or (string.upper(func_show_tooltip(model_table[j])):find(string.upper(ffi.string(search_text))) ~= nil) then
                            if skip_check == true or IsValidModForVehicle(model_table[j]) then
                                if imgui.ImageButton(store_table[tostring(model_table[j])],imgui.ImVec2(width,height),imgui.ImVec2(0,0),imgui.ImVec2(1,1),1,imgui.ImVec4(1,1,1,1),imgui.ImVec4(1,1,1,1)) then
                                    if body_part == nil then
                                        func_load_model(model_table[j])
                                    else
                                        func_load_model(model_table[j],body_part)
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
                        lua_thread.create(LoadTextures,store_table,image_path,model_table,image_extention)
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

    if imgui.RadioButtonIntPtr(flanguage.GetText("common.Default") .." ".. string.lower(label),button,#addr_table + 1) then
        for j = 1,#addr_table,1 do
            writeMemory(addr_table[j],1,0,false)
        end
        module.CheatActivated()
    end
end

function module.RwMemory(address,size,value,protect,is_float)
    if protect ~= true then protect = false end
    if is_float ~= true then is_float = false end

    if value == nil then
        if is_float then
            return memory.getfloat(address,protect)
        else
            return memory.read(address,size,protect)
        end
    else
        if is_float then
            memory.setfloat(address,value,protect)
        else
            memory.write(address,value,size,protect)
        end
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
            imgui.Text(flanguage.GetText("common.Minimum") .. " = " .. arg.min)
        end
        imgui.NextColumn()
        if arg.max ~= nil then
            imgui.Text(flanguage.GetText("common.Maximum") .. " = " .. arg.max)
        end

        imgui.Columns(1)

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt(flanguage.GetText("common.Set"),value) then
            setFloatStat(arg.stat,value[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button(flanguage.GetText("common.Minimum"),imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.min)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Default"),imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.default)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Maximum"),imgui.ImVec2(fcommon.GetSize(3))) then
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

function module.UpdateAddress(arg)
    if arg.is_float == nil then arg.is_float = false end
    if arg.default == nil then arg.default = 0 end

    module.DropDownMenu(arg.name,function()

        local value = imgui.new.int(module.RwMemory(arg.address,arg.size,nil,nil,arg.is_float))

        imgui.Columns(2,nil,false)
        if arg.min ~= nil then
            imgui.Text(flanguage.GetText("common.Minimum") .. " = " .. arg.min)
        end
        imgui.NextColumn()
        if arg.max ~= nil then
            imgui.Text(flanguage.GetText("common.Maximum") .. " = " .. arg.max)
        end
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt(flanguage.GetText("common.Set"),value) then
            module.RwMemory(arg.address,arg.size,value[0],nil,arg.is_float)
        end
        imgui.PopItemWidth()
   
        imgui.Spacing()
        if imgui.Button(flanguage.GetText("common.Minimum"),imgui.ImVec2(fcommon.GetSize(3))) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Default"),imgui.ImVec2(fcommon.GetSize(3))) then
            module.RwMemory(arg.address,arg.size,arg.default,nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button(flanguage.GetText("common.Maximum"),imgui.ImVec2(fcommon.GetSize(3))) then
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
