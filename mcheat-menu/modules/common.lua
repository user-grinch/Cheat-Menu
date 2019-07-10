-- This module contains all common functions used all over the menu

local module = {}

function module.DropDownMenu(label,func)
    if imgui.CollapsingHeader(label) then
        imgui.Spacing()

        func()

        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
    end
end

function module.quick_spawner()

    fcommon.KeyWait(keys.control_key,keys.quickspawner_key)

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
        elseif wasKeyPressed(memory.read(0x00969110,1,false)) then
            result = string.format('%s%s',result,memory.tostring(0x00969110,1,false));
        end
        printStringNow(string.format('[%s]',result),1500);
        text = result

        for i = 0,#result,1 do
           local weapon =  fweapons.CBaseWeaponInfo(text)
            if fweapons.tweapons.quick_spawn.v == true
            and fweapons.CBaseWeaponInfo(text) < 47 and weapon > 0 then
                fweapons.give_weapon_to_player(weapon)
                return
            end

            local vehicle = fvehicles.CBaseModelInfo(text)
            if fvehicles.tvehicles.quick_spawn.v == true and vehicle > 400 and vehicle < 100000 then
                fvehicles.give_vehicle_to_player(vehicle)
                return
            end
            text = text:sub(2)
        end
        wait(0)
    end
end

function module.information_tooltip(text)
    if imgui.IsItemHovered() then
        imgui.BeginTooltip()
        imgui.Spacing()
        imgui.SetTooltip(text)
        imgui.Dummy(imgui.ImVec2(50,50))
        imgui.EndTooltip()
    end
end

function module.CheatActivated()
    printHelpString("Cheat ~g~Activated")
end

function module.CheatDeactivated()
    printHelpString("Cheat ~r~Deactivated")
end

function module.ValueSwitch(name,var,show_help_popups,help_text)

    if help_text == nil then help_text = false end
    if imgui.Checkbox(name,var) then
        if show_help_popups then
            if var[0] then
                module.CheatActivated()
            else
                module.CheatDeactivated()
            end
        end
    end
    --if help_text ~= nil then
      --  module.information_tooltip(help_text)
    --end
end

function module.GetSize(count)
    if count == 1 then
        return ((imgui.GetWindowWidth()- 15*imgui.StyleVar.WindowPadding) / count), (imgui.GetWindowHeight()/30)
    else
        return ((imgui.GetWindowWidth()- 2*imgui.StyleVar.WindowPadding - (count-1)*imgui.StyleVar.ItemSpacing) / count), (imgui.GetWindowHeight()/30)
    end
end

function module.LoadTextures(list,path,model,extention)
    local func_path = path .. tostring(model) .. extention
    local image = imgui.CreateTextureFromFile(func_path)
    list[tostring(model)] = image
end

function IsValidModForVehicle(table)
    if isCharInAnyCar(PLAYER_PED) then
        local CVehicle =  getCarPointer(storeCarCharIsInNoSave(PLAYER_PED))
        if callMethod(0x49B010,CVehicle,2,2,table,CVehicle) == 1 then
             return true
        end
    else
        return false
    end
end

function module.UiCreateButtons(names,funcs)

    imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing,imgui.ImVec2(0,0))

    for i=1,#names,1 do
        if cheatMenu.menubuttons.current == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.060,0.530,0.980,1.0))
        end
        if imgui.Button(names[i],imgui.ImVec2(imgui.GetWindowWidth()/4 - 4*imgui.StyleVar.WindowPadding,imgui.GetWindowHeight()/30)) then
            cheatMenu.menubuttons.current = i
        end
        if cheatMenu.menubuttons.current == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.260,0.590,0.980,0.400))
        end

        if i%4 ~= 0 then
            imgui.SameLine()
        end
    end

    imgui.PopStyleVar()
    imgui.Spacing()

    for i=1,#funcs,1 do
        if cheatMenu.menubuttons.current == i then
            imgui.Spacing()
            funcs[i]()
            break
        end
    end
end

function module.ShowEntries(title,model_table,rows,store_table,image_path,image_extention,image_size,func_load_model,func_show_tooltip,skip_check)

    for j=1,#model_table,1 do
        if IsValidModForVehicle(model_table[j]) or skip_check == true then
            fcommon.DropDownMenu(title,function()
                imgui.Spacing()

                for i=1,#model_table,1 do

                    if store_table[tostring(model_table[i])] ~= "LOADING"  then

                        if store_table[tostring(model_table[i])] == nil then
                            store_table[tostring(model_table[i])] = "LOADING"
                            lua_thread.create(module.load_texture,store_table,image_path,model_table[i],image_extention)
                        else
                            if skip_check == false then
                                if IsValidModForVehicle(model_table[i]) then
                                    if imgui.ImageButton(store_table[tostring(model_table[i])],imgui.ImVec2(image_size.x,image_size.y)) then
                                        func_load_model(model_table[i])
                                    end
                                end
                            else
                                local x,y = fcommon.getSize(rows)
                                if imgui.ImageButton(store_table[tostring(model_table[i])],imgui.ImVec2(x/1.2,y*4)) then
                                    func_load_model(model_table[i])
                                end
                            end
                            if func_show_tooltip ~= nil then
                                if imgui.IsItemHovered() then
                                    imgui.BeginTooltip()
                                    imgui.SetTooltip(func_show_tooltip(model_table[i]))
                                    imgui.EndTooltip()
                                end
                            end
                        end
                    else
                        imgui.Spinner("Loading", 15, 3, imgui.GetColorU32(imgui.GetStyle().Colors[imgui.Col.ButtonHovered]))
                    end

                    if (i == 1) or (i % rows ~= 0) then
                        imgui.SameLine()
                    end
                end
                imgui.NewLine()
                imgui.Spacing()
                imgui.Separator()
                imgui.Spacing()
            end)
            break
        end
    end
end

function module.RadioButton(label,rb_table,addr_table,var)
    local button = imgui.new.int(#addr_table + 1)
    local i = 0

    imgui.Text(label)

    for i = 1, #addr_table,1 do
        if readMemory(addr_table[i],1,false) == 1 then
            button[0] = i
            break
        end
    end

    if imgui.RadioButtonIntPtr(rb_table[i],button,i) then
        if i == (#addr_table + 1) then
            for j = 1, #addr_table,1 do
                writeMemory(addr_table[j],1,0,false)
            end
            button[0] = i
            writeMemory(addr_table[i],1,1,false)
            module.CheatActivated()
        end
    end

    if imgui.RadioButtonIntPtr("Default",button,#addr_table + 1) then
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

function module.CheatActivated()
    printHelpString("Cheat ~g~Activated")
end

function module.CheatDeactivated()
    printHelpString("Cheat ~r~Deactivated")
end

function module.ButtonsMenu(func,names,sizeX,sizeY,style)
    if imgui.BeginMenu(names[1]) then
        imgui.Spacing()
        imgui.Text(names[1])
        imgui.Separator()
        imgui.Spacing()

        for i = 1, #func do
            if imgui.Button(names[i+1],imgui.ImVec2(sizeX,sizeY)) then
                callFunction(func[i],1,1)
                module.CheatActivated()
            end
            if not (i % 4 == 0) and style == "horizantal" then
                imgui.SameLine()
            end
        end
        imgui.EndMenu()
    end

end


function module.CheckBox(arg)
    arg.value = arg.value or 1
    arg.value2 = arg.value2 or 0
    local func_bool= imgui.new.bool()
    if (readMemory(arg.address,1,false)) == arg.value2 then
        func_bool[0] = false
    else
        func_bool[0] = true
    end

    if imgui.Checkbox(arg.name,func_bool) then
        if func_bool[0] then
            writeMemory(arg.address,1,arg.value,false)
            module.CheatActivated()
        else
            writeMemory(arg.address,1,arg.value2,false)
            module.CheatDeactivated()
        end
    end
    if arg.help_text ~= nil then
        module.information_tooltip(arg.help_text)
    end
end



function module.UpdateStat(arg)
    if arg.min == nil then arg.min = 0 end
    if arg.max == nil then arg.max = 1000 end

    if imgui.CollapsingHeader(arg.name) then
        imgui.Spacing()
        if arg.help_text ~= nil then
            fcommon.information_tooltip(arg.help_text)
        end

        local change_value = math.floor((arg.max - arg.min)/10)
        local value = imgui.new.int(math.floor(getFloatStat(arg.stat)))

        imgui.Columns(3,nil,false)
        imgui.Text("Max = " .. arg.max)
        imgui.NextColumn()
        imgui.Text("Current = " .. value[0])
        imgui.NextColumn()
        imgui.Text("Min = " .. arg.min)
        imgui.Columns(1)

        if imgui.InputInt("Set",value) then
            setFloatStat(arg.stat,value[0])
        end
        if imgui.Button("Increase",imgui.ImVec2(fcommon.getSize(4))) then
            setFloatStat(arg.stat,(value[0]+change_value))
        end
        imgui.SameLine()
        if imgui.Button("Decrease",imgui.ImVec2(fcommon.getSize(4))) then
            setFloatStat(arg.stat,(value[0]-change_value))
        end
        imgui.SameLine()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.getSize(4))) then
            setFloatStat(arg.stat,arg.min)
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.getSize(4))) then
            setFloatStat(arg.stat,arg.max)
        end
        if value[0] < arg.min then
            setFloatStat(arg.stat,arg.min)
        end
        if value[0] > arg.max then
            setFloatStat(arg.stat,arg.max)
        end
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
    end
end

function module.UpdateAddress(arg)
    if arg.min == nil then arg.min = 0 end
    if arg.is_float == nil then arg.is_float = false end

    if imgui.CollapsingHeader(arg.name,true) then
        imgui.Spacing()
        local value = imgui.new.int(math.floor(module.rw_memory(arg.address,arg.size,nil,nil,arg.is_float)))

        imgui.Columns(3,nil,false)
        imgui.Text("Max = " .. arg.max)
        imgui.NextColumn()
        imgui.Text("Current = " .. value[0])
        imgui.NextColumn()
        imgui.Text("Min = " .. arg.min)
        imgui.Columns(1)

        imgui.Spacing()

        imgui.PushItemWidth(imgui.GetWindowWidth()-50)
        if imgui.InputInt("Set",value) then
            module.rw_memory(arg.address,arg.size,value[0],nil,arg.is_float)
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Increase",imgui.ImVec2(fcommon.getSize(4))) and value[0] < arg.max then
            module.rw_memory(arg.address,arg.size,(value[0]+math.floor(arg.max/10)),nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Decrease",imgui.ImVec2(fcommon.getSize(4)))  and value[0] > arg.min then
            module.rw_memory(arg.address,arg.size,(value[0]-math.floor(arg.max/10)),nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Maximum",imgui.ImVec2(fcommon.getSize(4))) then
            module.rw_memory(arg.address,arg.size,arg.max,nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Minimum",imgui.ImVec2(fcommon.getSize(4))) then
            module.rw_memory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end
        imgui.Spacing()
        if value[0] < arg.min then
            module.rw_memory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end

        if value[0] > arg.max then
            module.rw_memory(arg.address,arg.size,arg.max,nil,arg.is_float)
        end
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
    end
end

function module.KeyWait(key1,key2)
    while isKeyDown(key1)
    and isKeyDown(key2) do
        wait(0)
    end
end

return module
