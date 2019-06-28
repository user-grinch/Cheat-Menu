-- This module contains all common functions used all over the menu

local module = {}
local radio_button = imgui.ImInt()


function module.quick_spawner()

    fcommon.keywait(keys.control_key,keys.quickspawner_key)

    memory.write(0x00969110,0,1)
    result = ''
    while not wasKeyPressed(0x0D) do
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
        imgui.SetTooltip(text)
        imgui.EndTooltip()
    end
end

function module.ValueSwitch(arg)

    if arg.show_help_popups == nil then arg.show_help_popups = false end

    if imgui.Checkbox(arg.name,arg.var) then 
        if arg.show_help_popups then
            if arg.var.v then
                module.CheatActivated()
            else
                module.CheatDeactivated()
            end
        end
    end
    if arg.help_text ~= nil then
        module.information_tooltip(arg.help_text)
    end
end

function module.getsize(count)
   return (imgui.GetWindowWidth()-25) / count
end

function module.load_texture(list,path,model,extention)
    local func_path = path .. tostring(model) .. extention
    local image = imgui.CreateTextureFromFile(func_path)
    list[tostring(model)] = image
end

function module.show_entries(title,model_table,rows,store_table,image_path,image_extention,image_size,func_load_model,func_show_tooltip)
    if imgui.BeginMenu(title) then
        imgui.Spacing()
        imgui.Text(title)
        imgui.Separator()
        imgui.Spacing()
        
        for i=1,#model_table,1 do

            if store_table[tostring(model_table[i])] ~= "LOADING"  then

                if store_table[tostring(model_table[i])] == nil then
                    store_table[tostring(model_table[i])] = "LOADING"
                    lua_thread.create(module.load_texture,store_table,image_path,model_table[i],image_extention) 
                else   
        
                    if imgui.ImageButton(store_table[tostring(model_table[i])],imgui.ImVec2(image_size.x,image_size.y)) then 
                        func_load_model(model_table[i])
                    end
                    if imgui.IsItemHovered() then
                        imgui.BeginTooltip() 
                        imgui.SetTooltip(func_show_tooltip(model_table[i]))
                        imgui.EndTooltip()
                    end
        
                end
            else
                imgui.Spinner("Loading", 15, 3, imgui.GetColorU32(imgui.GetStyle().Colors[imgui.Col.ButtonHovered]))
            end

            if (i == 1) or (i % rows ~= 0) then
                imgui.SameLine()
            end    
        end

        imgui.EndMenu()
    end
end

function module.radio_menu(header_text,rb_table,addr_table,style)
    imgui.Text(header_text)
    if style == "horizantal" then
        imgui.SameLine()
    else
        imgui.Separator()
    end
    radio_button.v = #rb_table + 1
    for i = 1, #rb_table do
        local mem_value = memory.read(addr_table[i],1)

        if mem_value == 1 then
            radio_button.v = i
        end
    
        if imgui.RadioButton(rb_table[i],radio_button,i) then
            for j = 1, #rb_table do
                memory.write(addr_table[j],0,1)
            end
            memory.write(addr_table[i],1 , 1)
            module.CheatActivated()
        end
        if not (i % 4 == 0) and style == "horizantal" then
            imgui.SameLine()
        end
    end
    if imgui.RadioButton("Normal",radio_button,(#rb_table + 1)) then
        for j = 1, #rb_table do
            memory.write(addr_table[j],0,1)
        end
        module.CheatActivated()
    end
end


function module.update_stat(arg)
    if arg.min == nil then arg.min = 0 end
    if arg.max == nil then arg.max = 1000 end

    if imgui.BeginMenu(arg.name) then
        imgui.Spacing()
        imgui.Text(arg.name)
        if arg.help_text ~= nil then
            module.information_tooltip(arg.help_text)
        end
        imgui.Separator()
        imgui.Spacing()
        local change_value = math.floor((arg.max - arg.min)/10)
        local value = imgui.ImInt(math.floor(getFloatStat(arg.stat))) 
        imgui.Text("Current = " .. value.v .." Max = " .. arg.max .. " Min = " .. arg.min)
      
        if imgui.InputInt("Set",value) then
            setFloatStat(arg.stat,value.v)
        end
        if imgui.Button("Increase") then
            setFloatStat(arg.stat,(value.v+change_value))
        end
        imgui.SameLine()
        if imgui.Button("Decrease") then
            setFloatStat(arg.stat,(value.v-change_value))
        end
        imgui.SameLine()
        if imgui.Button("Minimum") then
            setFloatStat(arg.stat,arg.min)
        end
        imgui.SameLine()
        if imgui.Button("Maximum") then
            setFloatStat(arg.stat,arg.max)
        end
        if value.v < arg.min then
            setFloatStat(arg.stat,arg.min)
        end
        if value.v > arg.max then
            setFloatStat(arg.stat,arg.max)
        end
        imgui.EndMenu()
    end 
end


function module.rw_memory(address,size,value,protect,is_float)
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

function module.buttons_menu(func,names,sizeX,sizeY,style)
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


function module.check_box(arg)
    arg.value = arg.value or 1
    arg.value2 = arg.value2 or 0
    local func_bool= imgui.ImBool()
    if (readMemory(arg.address,1,false)) == arg.value2 then
        func_bool.v = false
    else
        func_bool.v = true
    end

    if imgui.Checkbox(arg.name,func_bool) then
        if func_bool.v then
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

function module.popup_menu(arg)
    if arg.min == nil then arg.min = 0 end
    if arg.is_float == nil then arg.is_float = false end
    if imgui.BeginMenu(arg.name) then
        imgui.Spacing()
        local value = imgui.ImInt(0)
        value.v = math.floor(module.rw_memory(arg.address,arg.size,nil,nil,arg.is_float))
        imgui.Text(arg.name)
        if arg.help_text ~= nil then
            module.information_tooltip(arg.help_text)
        end
        imgui.Separator()
        imgui.Spacing()
        imgui.Text("Current = " .. value.v .." Max = " .. arg.max .. " Min = " .. arg.min)

        if imgui.InputInt("Set",value) then
            module.rw_memory(arg.address,arg.size,value.v,nil,arg.is_float)
        end
    
        if imgui.Button("Increase") and value.v < arg.max then
            module.rw_memory(arg.address,arg.size,(value.v+math.floor(arg.max/10)),nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Decrease")  and value.v > arg.min then
            module.rw_memory(arg.address,arg.size,(value.v-math.floor(arg.max/10)),nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Maximum") then
            module.rw_memory(arg.address,arg.size,arg.max,nil,arg.is_float)
        end
        imgui.SameLine()
        if imgui.Button("Minimum") then
            module.rw_memory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end

        if value.v < arg.min then   
            module.rw_memory(arg.address,arg.size,arg.min,nil,arg.is_float)
        end

        if value.v > arg.max then
            module.rw_memory(arg.address,arg.size,arg.max,nil,arg.is_float)
        end
        
        imgui.EndMenu()
        imgui.Spacing() 
    end 
end

function module.keywait(key1,key2)
    while isKeyDown(key1) 
    and isKeyDown(key2) do
        wait(0)
    end
end

return module