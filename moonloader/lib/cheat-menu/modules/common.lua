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
        imgui.TextColored(imgui.ImVec4(128,128,128,0.3),'?')
        
        if imgui.IsItemHovered() then
            imgui.BeginTooltip()
            imgui.SetTooltip(text)
            imgui.EndTooltip()
        end
    end
end

function module.ComboBox(label,selected,table,func)
    text = selected
    
    imgui.SetNextItemWidth(imgui.GetWindowContentRegionWidth()/2- imgui.StyleVar.ItemSpacing) 
    if imgui.BeginCombo("##" .. label,imgui.new.char[64](text)) then
        for k,v in ipairs(table) do
            imgui.Selectable(v) 
            if imgui.IsItemClicked(0) then
                selected = v
                if func ~= nil then
                    func(k,v)
                end
            end
        end
        imgui.EndCombo()
    end
end

-- Config panel
function module.ConfigPanel(func_arg_table,func)
    if func ~= nil then
        imgui.SameLine()
        imgui.TextColored(imgui.ImVec4(128,128,128,0.3),'c')
        
        if imgui.IsItemClicked(0) then
            tcheatmenu.window.panel_func = function()
                imgui.TextWrapped(string.format("%s configuraion",func_arg_table[2]))
                imgui.Separator()
                if imgui.Button("Hide",imgui.ImVec2(fcommon.GetSize(1))) then
                    tcheatmenu.window.panel_func = nil
                end
                if func_arg_table[1] ~= nil then
                    imgui.Dummy(imgui.ImVec2(0,10))
                    func_arg_table[1](func_arg_table[2],func_arg_table[3],func_arg_table[4])
                end
                imgui.Dummy(imgui.ImVec2(0,10))
                func()
            end
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

function module.WriteDebug(text)
    if fmenu.tmenu.debug.write_info[0] then
        print("[DEBUG] " .. text)
    end
end

function module.Tabs(label,names,func)

    if tcheatmenu.tab_data[label] == nil then
        tcheatmenu.tab_data[label] = 1
    end

    local button = imgui.ColorConvertFloat4ToU32(imgui.GetStyle()['Colors'][21])
    local buttonhovered = imgui.ColorConvertFloat4ToU32(imgui.GetStyle()['Colors'][22])
    local buttonactive = imgui.ColorConvertFloat4ToU32(imgui.GetStyle()['Colors'][23])
    
    imgui.Spacing()
    if imgui.BeginChild(label,imgui.ImVec2(tcheatmenu.window.size.X-imgui.GetStyle().WindowPadding.x*2,imgui.CalcTextSize(names[i]).y+10)) then
        imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing, imgui.ImVec2(4,0))

        imgui.GetStyle().Colors[imgui.Col.Button] = imgui.GetStyle().Colors[imgui.Col.Tab]
        imgui.GetStyle().Colors[imgui.Col.ButtonHovered] = imgui.GetStyle().Colors[imgui.Col.TabHovered]
        imgui.GetStyle().Colors[imgui.Col.ButtonActive] = imgui.GetStyle().Colors[imgui.Col.TabActive]

        for i=1,#names,1 do
            if tcheatmenu.tab_data[label] == i then
                imgui.GetStyle().Colors[imgui.Col.Button] = imgui.GetStyle().Colors[imgui.Col.TabActive]
            else
                imgui.GetStyle().Colors[imgui.Col.Button] = imgui.GetStyle().Colors[imgui.Col.Tab]
            end

            if imgui.Button(names[i],imgui.ImVec2(imgui.CalcTextSize(names[i]).x+10,imgui.CalcTextSize(names[i]).y+5)) then
                tcheatmenu.tab_data[label] = i
            end

            imgui.SameLine()
        end
        fcommon.InformationTooltip("If your window width is small you\ncan scroll by Shift + Mouse wheel")
        imgui.GetStyle().Colors[imgui.Col.Button] = imgui.ColorConvertU32ToFloat4(button)
        imgui.GetStyle().Colors[imgui.Col.ButtonHovered] = imgui.ColorConvertU32ToFloat4(buttonhovered)
        imgui.GetStyle().Colors[imgui.Col.ButtonActive] = imgui.ColorConvertU32ToFloat4(buttonactive)

        imgui.Separator()
        imgui.EndChild()
        imgui.PopStyleVar(1)
    end
    if func[tcheatmenu.tab_data[label]] ~= nil then
        imgui.Spacing()
           
        if imgui.BeginChild("") then
            if tcheatmenu.window.panel_func == nil then
                func[tcheatmenu.tab_data[label]]()
            else
                tcheatmenu.window.panel_func()
            end
            imgui.EndChild()
        end
    end
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
    if tcheatmenu.current_menu ~= 0 then
        funcs[tcheatmenu.current_menu]()
    end
end


function LoadImages(image_table)

    for model,image in pairs(image_table) do
        if type(image) == "string" then
            image_table[model] = imgui.CreateTextureFromFile(image)
        end
        wait(0)
    end
    
end

-- Shows loaded images with on_click and on_hover calls
function module.DrawImages(identifier,draw_type,loaded_images_list,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter)

    -- Calculate image count in a row
    local images_in_row = math.floor(imgui.GetWindowContentRegionWidth()/const_image_width)

    const_image_width = (imgui.GetWindowContentRegionWidth() - imgui.StyleVar.ItemSpacing*(images_in_row-0.4*images_in_row))/images_in_row

    local image_count   = 1

    -------------------------
    -- Draws a single image
    draw_image = function(identifier,image_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter,model,image)
        local model_name = nil
        if func_get_name ~= nil then
            model_name = func_get_name(model)
        end

        if draw_type == fconst.DRAW_TYPE.LIST or filter:PassFilter(model_name) then
            if type(image) ~= "string" then
                if identifier ~= fconst.IDENTIFIER.COMPONENT or fvehicle.IsValidModForVehicle(tonumber(model),getCarPointer(getCarCharIsUsing(PLAYER_PED))) then
                    if imgui.ImageButton(image,imgui.ImVec2(const_image_width,const_image_height),imgui.ImVec2(0,0),imgui.ImVec2(1,1),1,imgui.ImVec4(1,1,1,1),imgui.ImVec4(1,1,1,1)) then
                        func_on_left_click(model)
                    end
                    if imgui.IsMouseClicked(1) and func_on_right_click ~= nil then
                        func_on_right_click(model)
                    end
                
                    if model_name ~= nil then
                        if imgui.IsItemHovered() then
                            local drawlist = imgui.GetWindowDrawList()
                            drawlist:AddRectFilled(imgui.GetItemRectMin(), imgui.GetItemRectMax(), imgui.GetColorU32(imgui.Col.ModalWindowDimBg))

                            if identifier == fconst.IDENTIFIER.CLOTH and func_get_name ~= nil then
                                model, model_name = func_get_name(model,true)
                            end

                            if imgui.CalcTextSize(model_name).x > const_image_width then
                                model_name = model_name:gsub(" ","\n")
                            end

                            if identifier ~= fconst.IDENTIFIER.PAINTJOB then
                                local offset_x = (imgui.GetItemRectSize().x - imgui.CalcTextSize(model).x)/2
                                local offset_y = 5

                                drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x+offset_x,imgui.GetItemRectMin().y+offset_y), imgui.GetColorU32(imgui.Col.Text),model)
                                
                                local offset_x = (imgui.GetItemRectSize().x - imgui.CalcTextSize(model_name).x)/2
                                local offset_y = imgui.CalcTextSize(model).y+5
                                drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x+offset_x,imgui.GetItemRectMin().y+offset_y), imgui.GetColorU32(imgui.Col.Text),model_name)
                                
                            else
                                local text = string.format("%s",model)

                                local offset_x = (imgui.GetItemRectSize().x - imgui.CalcTextSize(model).x)/2
                                local offset_y = 5

                                drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x+offset_x,imgui.GetItemRectMin().y+offset_y), imgui.GetColorU32(imgui.Col.Text),model)
                                
                            end
                        end
                    end

                    if image_count % images_in_row ~= 0 then
                        imgui.SameLine(0.0,4.0)
                    end
                    image_count = image_count + 1
                end
            end
        end
    end

    draw_text = function(identifier,func_on_left_click,func_on_right_click,func_get_name,filter,model)
        local model_name = ""
        local label = ""

        if func_get_name ~= nil then
            model_name = func_get_name(model)
        end
        
        if identifier == fconst.IDENTIFIER.PAINTJOB or identifier == fconst.IDENTIFIER.COMPONENT or identifier == fconst.IDENTIFIER.CLOTHES then
            if identifier == fconst.IDENTIFIER.CLOTHES then
                local body_part, model_name , texture_name = fplayer.GetClothName(model,true)
                label = string.format( "%s - %s - %s",body_part, model_name , texture_name)
            else
                label = model:gsub("$"," ")
            end
        else
            label = string.format( "%s - %s",model_name,model)
        end

        if draw_type == fconst.DRAW_TYPE.LIST or filter:PassFilter(model_name) then
            if identifier ~= fconst.IDENTIFIER.COMPONENT or fvehicle.IsValidModForVehicle(tonumber(model),getCarPointer(getCarCharIsUsing(PLAYER_PED))) then
                if imgui.MenuItemBool(label) and func_on_left_click ~= nil then
                    func_on_left_click(model)
                end 
                if imgui.IsMouseClicked(1) and func_on_right_click ~= nil then
                    func_on_right_click(model)
                end
            end
        end
    end
    -------------------------

    -- Draw images in a listed order for list tabs
    if draw_type == fconst.DRAW_TYPE.LIST then
        if imgui.BeginChild("") then 
            for table_name,image_table in module.spairs(loaded_images_list) do
                local show = true
                if identifier == fconst.IDENTIFIER.COMPONENT then
                    show = false
                    for model,image in pairs(image_table) do
                        if fvehicle.IsValidModForVehicle(tonumber(model),getCarPointer(getCarCharIsUsing(PLAYER_PED))) then
                            show = true
                        end
                    end
                end
                if show and imgui.CollapsingHeader(table_name) then
                    if not fmenu.tmenu.draw_text_only[0] then
                        lua_thread.create(LoadImages,image_table)
                    end
                    imgui.Spacing()
                    image_count = 1
                    for model,image in pairs(image_table) do
                        if fmenu.tmenu.draw_text_only[0] then
                            draw_text(identifier,func_on_left_click,func_on_right_click,func_get_name,filter,model)
                        else
                            draw_image(identifier,valid_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter,model,image)
                        end
                    end
                    imgui.Spacing()
                    imgui.Separator()
                end
            end
            imgui.EndChild()
        end
    end

    -- Draw all images one by one for search tabs
    if draw_type == fconst.DRAW_TYPE.SEARCH then 

        filter:Draw("Filter")
        imgui.Spacing()

        if imgui.BeginChild("") then 
            for _,image_table in pairs(loaded_images_list) do
                if not fmenu.tmenu.draw_text_only[0] then
                    lua_thread.create(LoadImages,image_table)
                end
                for model,image in pairs(image_table) do
                    if fmenu.tmenu.draw_text_only[0] then
                        draw_text(identifier,func_on_left_click,func_on_right_click,func_get_name,filter,model)
                    else
                        draw_image(identifier,valid_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,filter,model,image)
                    end
                end
            end
            imgui.EndChild()
        end
    end
    
end

function module.RadioButtonFunc(label,label_table,values,memory)
    module.DropDownMenu(label,function()
        local button = imgui.new.int(module.RwMemory(memory,1))

        local btn_in_column = math.floor(#label_table/2)
        
        if (#label_table % 2) ~= 0 then -- if odd number
            btn_in_column = btn_in_column + 1
        end 
        
        if #label_table > 1 then
            imgui.Columns(2,nil,false)
        end
        for i = 1, #label_table,1 do

            if imgui.RadioButtonIntPtr(label_table[i] .. "##" .. label,button,values[i]) then
                module.RwMemory(memory,1,values[i])
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",memory),{1,values[i]})
                button[0] = i
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
    local btn_in_column = math.floor(#addr_table/2+1)

    imgui.Text(label)
    imgui.Columns(2,nil,false)
    for i = 1, #addr_table,1 do
        if readMemory(addr_table[i],1,false) == 1 then
            button[0] = i
        end
        if imgui.RadioButtonIntPtr(rb_table[i],button,i) then
            for j = 1,#addr_table,1 do
                writeMemory(addr_table[j],1,0,false)
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",addr_table[j]),{1,0})
            end
            button[0] = i
            writeMemory(addr_table[i],1,1,false)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",addr_table[i]),{1,1})
            module.CheatActivated()
        end
        if i == btn_in_column then
            imgui.NextColumn()
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
    imgui.Columns(1)
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
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",address),{1,enable_value})
            fcommon.CheatActivated()
        else
            fcommon.RwMemory(address,1,disable_value)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",address),{1,disable_value})
            fcommon.CheatDeactivated()
        end
    end
    
    module.InformationTooltip(tooltip)

end

function module.CheckBoxVar(name,var,tooltip,func,panel_func)

    if imgui.Checkbox(name, var) then
        if var[0] then
            fcommon.CheatActivated()
        else
            fcommon.CheatDeactivated()
        end
        if func ~= nil then
            func()
        end
    end

    module.InformationTooltip(tooltip)

    module.ConfigPanel({module.CheckBoxVar,name,var,tooltip},panel_func)
    
end

function module.CheckBoxFunc(name,var,func,tooltip,panel_func)

    if imgui.Checkbox(name, var) then
        func()
    end

    module.InformationTooltip(tooltip)

    module.ConfigPanel({module.CheckBoxVar,name,var,tooltip},panel_func)
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
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),value[0])
        end
        imgui.PopItemWidth()

        imgui.Spacing()
        if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.min)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.min)
        end
        imgui.SameLine()
        if imgui.Button("Default##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.default)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.default)
        end
        imgui.SameLine()
        if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(fcommon.GetSize(3))) then
            setFloatStat(arg.stat,arg.max)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.max)
        end
        if value[0] < arg.min then
            setFloatStat(arg.stat,arg.min)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.min)
        end
        if value[0] > arg.max then
            setFloatStat(arg.stat,arg.max)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.max)
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
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,value[0],arg.is_float,arg.factor})
            arg.func(arg,value[0])
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("-##".. arg.name,imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] - arg.cvalue),nil,arg.is_float,arg.factor)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,(value[0] - arg.cvalue),arg.is_float,arg.factor})
            arg.func(arg,(value[0] - arg.cvalue))
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("+##".. arg.name,imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] + arg.cvalue),nil,arg.is_float,arg.factor)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,(value[0] + arg.cvalue),arg.is_float,arg.factor})
            arg.func(arg,(value[0] + arg.cvalue))
        end
        imgui.SameLine(0.0,4.0)
        imgui.Text("Set")
        imgui.Spacing()
        if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.factor)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.min,arg.is_float,arg.factor})
            arg.func(arg,arg.min)
        end

        if arg.default ~= nil then
            imgui.SameLine()
            if imgui.Button("Default##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
                module.RwMemory(arg.address,arg.size,arg.default,nil,arg.is_float,arg.factor)
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.default,arg.is_float,arg.factor})
                arg.func(arg,arg.default)
            end
        end

        if arg.max ~= nil then
            imgui.SameLine()
            if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
                module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float,arg.factor)
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.max,arg.is_float,arg.factor})
                arg.func(arg,arg.max)
            end
        end
        imgui.SameLine()
        imgui.Spacing()
        if (arg.min ~= nil) and (value[0] < arg.min) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.factor)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.min,arg.is_float,arg.factor})
            arg.func(arg,arg.min)
        end

        if (arg.max ~= nil) and (value[0] > arg.max) then
            module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float,arg.factor)
            fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.max,arg.is_float,arg.factor})
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

    if imgui.Button(text,imgui.ImVec2(x,y/1.2)) then
        if tcheatmenu.hot_keys.currently_active == index then
            tcheatmenu.read_key_press = false
            tcheatmenu.hot_keys.currently_active = {}
        else
            tcheatmenu.hot_keys.currently_active = index
            tcheatmenu.read_key_press = true
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

        if tcheatmenu.read_key_press then

            for i=32,255,1 do
                if isKeyDown(i) then
                    tcheatmenu.hot_keys.currently_active[1] = i
                    break
                end
            end
    
            for i=255,32,-1 do
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

function module.LoadJson(filename)
    local full_path = tcheatmenu.dir .. "json//" .. filename .. ".json"
    if doesFileExist(full_path) then
        local file = io.open(full_path, "r")
        local status, table = pcall(decodeJson,file:read("*a"))
        file:close()
        if status and table then
            return table
        else
            tcheatmenu.window.fail_loading_json = true
            print("Failed to load json file, " .. filename)
        end
    end
    return {}
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

-- Indexes image paths recursively from root directory

function module.IndexImages(mainDir,store_image_table,req_ext)
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
                        if fmenu.tmenu.fast_load_images[0] then
                            store_image_table[dir][file_name] = imgui.CreateTextureFromFile(file_path)
                        else
                            store_image_table[dir][file_name] = file_path
                        end
                    end
                end
            end
            wait(0)
        end
    end
end

function module.MoveFiles(main_dir,dest_dir)
    for f in lfs.dir(main_dir) do
        local main_file = main_dir .. "/" .. f

        if doesDirectoryExist(main_file) and f ~= "." and f ~= ".." then
            module.MoveFiles(main_file,dest_dir .. "/" .. f)
        end

        if doesFileExist(main_file) then
            dest_file = dest_dir .. "/" .. f
            if not doesDirectoryExist(dest_dir) then
                lfs.mkdir(dest_dir)
            end
            
            if doesFileExist(dest_file) then
                os.remove(dest_file)
            end
            if doesFileExist(dest_file) then
                os.remove(main_file)
                print("[UPDATE] Unable to delete file " .. dest_file)
            else
                os.rename(main_file,dest_file)
            end
            
        end
    end
    lfs.rmdir(main_dir)
end

function module.ReleaseImages(main_table)
    for _,image_table in pairs(main_table) do
        for model,image in pairs(image_table) do
            if type(image) ~= "string" then
                imgui.ReleaseTexture(image)
            end
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
