-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019-2020 Grinch_

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

module.tcommon = 
{
    read_key_press = false, 
}

-- Sorted pairs function
function module.spairs(t, f)
    local a = {}
    for n in pairs(t) do table.insert(a, n) end
    table.sort(a, f)
    local i = 0      -- iterator variable
    local iter = function ()   -- iterator function
      i = i + 1
      if a[i] == nil then return nil
      else return a[i], t[a[i]]
      end
    end
    return iter
end

-- Key spawner used to spawn vehicles & weapons trigger: Ctrl + Q
-- Launched in a separate thread
function module.QuickSpawner()

    memory.write(0x00969110,0,1)
    local result = ''
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

        local text = result

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

--------------------------------------------------
-- imgui functions

-- Custom imgui.CollapsingHeader
function module.DropDownMenu(label,func,text_disabled)
    if label ~= nil then
        if text_disabled then
            imgui.PushStyleColor(imgui.Col.Text,imgui.ImVec4(128,128,128,0.3))
        end
        if imgui.CollapsingHeader(label) then
            if text_disabled then
                imgui.PopStyleColor()
            end
            imgui.Spacing()

            func()

            imgui.Spacing()
            imgui.Separator()
            imgui.Spacing()
        else
            if text_disabled then
                imgui.PopStyleColor()
            end
        end
    else
        func()

        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()
    end
end

-- Information popups used to display controls and details about elements
function module.InformationTooltip(text)
    if fmenu.tmenu.show_tooltips[0] and text ~= nil then
        imgui.SameLine()
        imgui.TextColored(imgui.ImVec4(128,128,128,0.3),'(?)')
        if imgui.IsItemHovered() then
            imgui.BeginTooltip()
            imgui.SetTooltip(text)
            imgui.EndTooltip()
        end
    end
end

-- Calculates width of element(button) acoording to count
function module.GetSize(count,no_spacing)
  
    x = x or 20
    y = y or 25
    count = count or 1
    if count == 1 then no_spacing = true end

    if no_spacing == true then 
        x = imgui.GetWindowContentRegionWidth()/count
    else
        x = imgui.GetWindowContentRegionWidth()/count - imgui.StyleVar.ItemSpacing/(count+1)
    end

    y = (imgui.GetWindowHeight()/25)
    if y < 25 then y = 25 end

    return x,y
end

-- Creates top level menus
function module.CreateMenus(names,funcs)

    imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing,imgui.ImVec2(0,0.5))

    local button = imgui.ColorConvertFloat4ToU32(imgui.GetStyle()['Colors'][21])

    for i=1,#names,1 do
        if tcheatmenu.current_menu == i then
            imgui.GetStyle().Colors[imgui.Col.Button] = imgui.GetStyle().Colors[imgui.Col.ButtonActive]
        end
        if imgui.Button(names[i],imgui.ImVec2(module.GetSize(4,true))) then
            tcheatmenu.current_menu = i
        end
        if tcheatmenu.current_menu == i then
            imgui.GetStyle().Colors[imgui.Col.Button] = imgui.ColorConvertU32ToFloat4(button)
        end
       
        if i%4 ~= 0 then
            imgui.SameLine()
        end
    end

    imgui.PopStyleVar()
    imgui.Dummy(imgui.ImVec2(0,5))
    funcs[tcheatmenu.current_menu]()
  
end

-- Shows loaded images with on_click and on_hover calls
function module.DrawImages(identifier,draw_type,loaded_images_list,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name)

    local filter = imgui.ImGuiTextFilter()

    -- Calculate image count in a row
    local images_in_row = math.floor(imgui.GetWindowContentRegionWidth()/const_image_width)

    const_image_width = (imgui.GetWindowContentRegionWidth() - imgui.StyleVar.ItemSpacing*(images_in_row-0.5*images_in_row))/images_in_row

    local image_count   = 1

    -------------------------
    -- Draws a single image
    draw_image = function(identifier,image_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter,model,image)
        local model_name = nil
        if func_get_name ~= nil then
            model_name = func_get_name(model)
        end

        if draw_type == fconst.DRAW_TYPE.LIST or filter:PassFilter(model_name) then
            if imgui.ImageButton(image,imgui.ImVec2(const_image_width,const_image_height),imgui.ImVec2(0,0),imgui.ImVec2(1,1),1,imgui.ImVec4(1,1,1,1),imgui.ImVec4(1,1,1,1)) then
                func_on_left_click(model)
            end
            if imgui.IsMouseClicked(1) and func_on_right_click ~= nil then
                func_on_right_click(model)
            end
        
            if model_name ~= nil then
                if imgui.IsItemHovered() then
                    imgui.BeginTooltip()
                    if identifier ~= fconst.IDENTIFIER.PAINTJOB and identifier ~= fconst.IDENTIFIER.CLOTH then
                        imgui.SetTooltip(model .. "\n" .. model_name)
                    else
                        imgui.SetTooltip(model_name)
                    end
                    imgui.EndTooltip()
                end
            end

            if image_count % images_in_row ~= 0 then
                imgui.SameLine(0.0,4.0)
            end
            image_count = image_count + 1
        end
    end
    -------------------------

    -- Draw images in a listed order for list tabs
    if draw_type == fconst.DRAW_TYPE.LIST then
        if imgui.BeginChild("") then 
            for table_name,image_table in module.spairs(loaded_images_list) do
                local valid_table = nil
                for model,image in module.spairs(image_table) do
                    if identifier ~= fconst.IDENTIFIER.COMPONENT or fvehicle.IsValidModForVehicle(tonumber(model),getCarPointer(getCarCharIsUsing(PLAYER_PED))) then
                        if valid_table == nil then valid_table = {} end
                        valid_table[model] = image
                    end
                end
                if valid_table ~= nil then
                    if imgui.CollapsingHeader(table_name) then
                        imgui.Spacing()
                        for model,image in pairs(valid_table) do
                            draw_image(identifier,valid_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter,model,image)
                        end
                        imgui.Spacing()
                        imgui.Separator()
                        imgui.Spacing()
                    end
                end
            end
            imgui.EndChild()
        end
    end

    -- Draw all images one by one for search tabs
    if draw_type == fconst.DRAW_TYPE.SEARCH then 

        filter:Draw("Filter")
        imgui.Spacing()
        imgui.Separator()
        imgui.Spacing()

        if imgui.BeginChild("") then 
            for _,image_table in pairs(loaded_images_list) do
                for model,image in pairs(image_table) do
                    draw_image(identifier,image_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter,model,image)
                end
            end
            imgui.EndChild()
        end
    end
    
end

function module.RadioButtonFunc(label,label_table,values,memory,save_func,save_func_parameter_table)
    module.DropDownMenu(label,function()
        local button = imgui.new.int(module.RwMemory(memory,1))

        local btn_in_column = math.floor(#label_table/2)
        
        if (#label_table % 2) ~= 0 then -- if odd number
            btn_in_column = btn_in_column + 1
        end 

        imgui.Columns(2,nil,false)
        for i = 1, #label_table,1 do

            if imgui.RadioButtonIntPtr(label_table[i] .. "##" .. label,button,values[i]) then
                module.RwMemory(memory,1,values[i])
                button[0] = i
                if save_func ~= nil then
                    save_func(save_func_parameter_table,label,values[i])
                end
            end
            if i == btn_in_column then
                imgui.NextColumn()
            end
        end
        imgui.Columns(1)

    end)
end

function module.RadioButton(label,rb_table,addr_table,default)

    if default == nil then default = true end

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

    if default == true then --  unused in handling section
        if imgui.RadioButtonIntPtr("Default ##" ..label,button,#addr_table + 1) then
            for j = 1,#addr_table,1 do
                writeMemory(addr_table[j],1,0,false)
            end
            module.CheatActivated()
        end
    end
end


--------------------------------------------------
-- Checkbox functions

function module.CheckBoxValue(name,address,tooltip,enable_value,disable_value)

    if enable_value == nil then enable_value = 1 end
    if disable_value == nil then disable_value = 0 end

    local var = imgui.new.bool(false)

    if fcommon.RwMemory(address,1) == enable_value  then
        var[0] = true
    end

    if imgui.Checkbox(name, var) then
        if var[0] then
            fcommon.RwMemory(address,1,enable_value)
            fcommon.CheatActivated()
        else
            fcommon.RwMemory(address,1,disable_value)
            fcommon.CheatDeactivated()
        end
    end
    
    module.InformationTooltip(tooltip)

end

function module.CheckBoxVar(name,var,tooltip,func)

    if imgui.Checkbox(name, var) then
        if var[0] then
            fcommon.CheatActivated()
        else
            fcommon.CheatDeactivated()
        end
        if func ~= nil then
            func() -- do other stuff
        end
    end

    module.InformationTooltip(tooltip)
    
end

function module.CheckBoxFunc(name,var,func,tooltip)

    if imgui.Checkbox(name, var) then
        func()
    end

    module.InformationTooltip(tooltip)
end

--------------------------------------------------

-- Provides input options to change game stats
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
        if imgui.InputInt("Set##".. arg.name,value) then
            setFloatStat(arg.stat,value[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.min)
        end
        imgui.SameLine()
        if imgui.Button("Default##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.default)
        end
        imgui.SameLine()
        if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
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

-- Similar UI to the previous function
-- Provides input options to change game memory values
function module.UpdateAddress(arg)
    if arg.cvalue == nil then arg.cvalue = 1.0 end

    local buttons = 3

    if arg.default == nil then
        buttons = buttons - 1
    end

    if arg.max == nil then
        buttons = buttons - 1
    end

    arg.func = function(arg,value)
        if arg.save_func ~= nil then
            arg.save_func(arg.save_func_parameter_table,arg.name,value)
        end
    end

    module.DropDownMenu(arg.name,function()

        local value = imgui.new.float(module.RwMemory(arg.address,arg.size,nil,nil,arg.is_float,arg.factor))

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
            module.RwMemory(arg.address,arg.size,value[0],nil,arg.is_float,arg.factor)
            arg.func(arg,value[0])
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("-##".. arg.name,imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] - arg.cvalue),nil,arg.is_float,arg.factor)
            arg.func(arg,(value[0] - arg.cvalue))
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("+##".. arg.name,imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] + arg.cvalue),nil,arg.is_float,arg.factor)
            arg.func(arg,(value[0] + arg.cvalue))
        end
        imgui.SameLine(0.0,4.0)
        imgui.Text("Set")
        imgui.Spacing()
        if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.factor)
            arg.func(arg,arg.min)
        end

        if arg.default ~= nil then
            imgui.SameLine()
            if imgui.Button("Default##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
                module.RwMemory(arg.address,arg.size,arg.default,nil,arg.is_float,arg.factor)
                arg.func(arg,arg.default)
            end
        end

        if arg.max ~= nil then
            imgui.SameLine()
            if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
                module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float,arg.factor)
                arg.func(arg,arg.max)
            end
        end
        imgui.SameLine()
        imgui.Spacing()
        if (arg.min ~= nil) and (value[0] < arg.min) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.factor)
            arg.func(arg,arg.min)
        end

        if (arg.max ~= nil) and (value[0] > arg.max) then
            module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float,arg.factor)
            arg.func(arg,arg.max)
        end
    end)
end

function module.GetHotKeyNames(hot_key)

    if hot_key[1] == hot_key[2] then
        return string.format(" %s",vkeys.id_to_name(hot_key[1]))
    else
        return string.format(" %s + %s",vkeys.id_to_name(hot_key[1]),vkeys.id_to_name(hot_key[2]))
    end

end

function module.OnHotKeyPress(hotkey,func)
    if isKeyDown(hotkey[1]) and isKeyDown(hotkey[2]) and tcheatmenu.hot_keys.currently_active ~= hotkey then
        fcommon.KeyWait(hotkey[1],hotkey[2])
        func()
    end
end

function module.HotKey(index,info_text)
    local active = false
    local x,y  = fcommon.GetSize(3)
    
    if index == tcheatmenu.hot_keys.currently_active then
        imgui.PushStyleColor(imgui.Col.Button,imgui.GetStyle().Colors[imgui.Col.ButtonActive])
        active = true
    end

    if index[1] == index[2] then
        text = vkeys.id_to_name(index[1])
    else
        text = vkeys.id_to_name(index[1]) .. " + " .. vkeys.id_to_name(index[2])
    end

    if imgui.Button(text,imgui.ImVec2(x,y-y/5)) then
        if tcheatmenu.hot_keys.currently_active == index then
            module.tcommon.read_key_press = false
            tcheatmenu.hot_keys.currently_active = {}
        else
            tcheatmenu.hot_keys.currently_active = index
            module.tcommon.read_key_press = true
        end
    end

    if active then
        imgui.PopStyleColor()
    end  

    if not imgui.IsWindowFocused() or not imgui.IsItemVisible() then
        tcheatmenu.hot_keys.currently_active = {}
    end


    imgui.SameLine()

    imgui.Text(info_text)
end


function module.ReadKeyPress()
    while true do

        if module.tcommon.read_key_press then

            for i=0,127,1 do
                if isKeyDown(i) then
                    tcheatmenu.hot_keys.currently_active[1] = i
                    break
                end
            end
    
            for i=127,0,-1 do
                if isKeyDown(i) then
                    tcheatmenu.hot_keys.currently_active[2] = i
                    break
                end
            end
            
            if tcheatmenu.hot_keys.currently_active[1] ~= tcheatmenu.hot_keys.currently_active[2] then
                while isKeyDown(tcheatmenu.hot_keys.currently_active[1]) or isKeyDown(tcheatmenu.hot_keys.currently_active[2]) do
                    wait(0)
                end
            end

        end
        
        wait(0)
    end
end

--------------------------------------------------


--------------------------------------------------
-- Functions for loading & saving files

function module.LoadAndSetFonts()
    local mask = tcheatmenu.dir .. "fonts//*.ttf"

    local handle, name = findFirstFile(mask)

    while handle and name do
        local font = imgui.GetIO().Fonts:AddFontFromFileTTF(string.format( "%sfonts//%s",tcheatmenu.dir,name), 14)
        name = findNextFile(handle)
        --if name == fstyle.tstyle.font then
        --    imgui.GetIO().FontDefault = font
        --end
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
    return nil
end

function module.SaveJson(filename,table)
    local full_path = tcheatmenu.dir .. "json//" .. filename .. ".json"
    local file = assert(io.open(full_path, "w"))
    file:write(encodeJson(table))
    file:close()
end
--------------------------------------------------


--------------------------------------------------
-- Misc

-- Loads images recursively from root directory
function module.LoadImages(mainDir,store_image_table,req_ext)
    for dir in lfs.dir(mainDir) do
        local dir_path = mainDir .. "\\" .. dir
        if doesDirectoryExist(dir_path) and dir ~= "." and dir ~= ".." then
            for file in lfs.dir(dir_path) do
                local file_path = dir_path .. "\\" .. file
                if doesFileExist(file_path) then
                    local _,file_name,file_ext = string.match(file_path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 
                    file_name = string.sub(file_name,1,-5)
                    if req_ext == file_ext then
                        if store_image_table[dir] == nil then
                            store_image_table[dir] = {}
                        end
                        store_image_table[dir][file_name] = imgui.CreateTextureFromFile(file_path)
                    end
                end
                if not fmenu.tmenu.fast_load_images[0] then
                    wait(0)
                end
            end
        end
    end
end

function module.ReleaseImages(main_table)
    for _,image_table in pairs(main_table) do
        for model,image in pairs(image_table) do
            imgui.ReleaseTexture(image)
        end
    end
end

-- Used to read/write from/to memory
function module.RwMemory(address,size,value,protect,is_float,factor)
    if protect == nil then protect = false end
    if factor == nil then factor = 1 end

    if value == nil then
        local return_val = nil
        if is_float == true then
            return_val = memory.getfloat(address,protect)
        else
            return_val =  readMemory(address,size,protect)
        end

        return return_val/factor
    else
        if is_float == true then
            memory.setfloat(address,value*factor,protect)
        else
            writeMemory(address,size,value*factor,protect)
        end
    end
end

function module.CheatActivated()
    printHelpString("Cheat ~g~Activated")
end

function module.CheatDeactivated()
    printHelpString("Cheat ~r~Deactivated")
end

function module.KeyWait(key1,key2)
    while isKeyDown(key1)
    and isKeyDown(key2) do
        wait(0)
    end
end
--------------------------------------------------

return module
