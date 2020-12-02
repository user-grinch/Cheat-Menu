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

function module.spairs(t, f)
    local a = {}
    for n in pairs(t) do table.insert(a, n) end
    table.sort(a, f)
    local i = 0    
    local iter = function ()   
      i = i + 1
      if a[i] == nil then return nil
      else return a[i], t[a[i]]
      end
    end
    return iter
end

function module.CalcTableSize(table)
    local count = 0
    for k,v in pairs(table) do
        count = count + 1
    end
    return count
end

function module.pool(pool)

    local entities = {}
    local cur_index = 0
    local address = nil

    if pool == "veh" then
        address = 0xB74494 
    end
    if pool == "char" then
        address = 0xB74490  
    end
    if pool == "obj" then
        address = 0xB7449C   
    end

    local address = readMemory(address,4,false)
    address = address + 0x04
    address = readMemory(address,4,false)
    local x = 0

    while x <= 0x6D00 do
        local entity = readMemory(address,1,false)
        address = address + 1
        if entity >= 0x00 and entity < 0x80 then
            entity = entity + x
            table.insert(entities,entity)
        end
        x = x + 0x100
    end

    local iter = function () 
        cur_index = cur_index + 1
        return entities[cur_index]
    end
    return iter
end


local thread_locks = {} 

function module.CreateThread(func)

    lua_thread.create(function(func)

        if thread_locks[func] == nil then -- is thread key unlcoked
            thread_locks[func] = true
            func()
            thread_locks[func] = nil -- unlock the thread key
        end

    end,func)
end

--------------------------------------------------
-- JSON functions

function module.LoadJson(filename)
    local full_path = tcheatmenu.dir .. "json//" .. filename .. ".json"
    if doesFileExist(full_path) then
        local file = io.open(full_path, "r")
        local status, table = pcall(decodeJson,file:read("*a"))
        file:close()
        
        if status and table then
            return table
        else
            tcheatmenu.fail_loading_json = true
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
                print("Unable to delete file " .. dest_file)
            else
                os.rename(main_file,dest_file)
            end
            
        end
    end
    lfs.rmdir(main_dir)
end

function module.InformationTooltip(text)
    if fmenu.tmenu.show_tooltips[0] then
        if text ~= nil then
            imgui.SameLine()
            imgui.InvisibleButton("?##".. text,imgui.CalcTextSize("?"))
            local drawlist = imgui.GetWindowDrawList()
            drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x,imgui.GetItemRectMin().y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),"?")
            
            if imgui.IsItemHovered() then
                imgui.BeginTooltip()
                imgui.SetTooltip(text)
                imgui.EndTooltip()
            end
        end
    end
end

--------------------------------------------------
-- imgui functions

-- Calculates width of element(button) acoording to count
function module.GetSize(count,no_spacing)
  
    x = x or 20
    count = count or 1
    if count == 1 then no_spacing = true end

    local factor = imgui.GetStyle().ItemSpacing.x/2

    if count == 3 then
        factor = imgui.GetStyle().ItemSpacing.x/1.403
    end

    if no_spacing == true then 
        x = imgui.GetWindowContentRegionWidth()/count
    else
        x = imgui.GetWindowContentRegionWidth()/count - factor
    end

    return x,imgui.GetFrameHeight()*1.3
end

function module.CRGBAColorPicker(label,base_addr,default_col,save)
    save = save or true
    local CRGBA = imgui.new.float[4](memory.read(base_addr,1)/255,memory.read(base_addr+1,1)/255,memory.read(base_addr+2,1)/255,memory.read(base_addr+3,1)/255)
    
    fcommon.DropDownMenu(label,function()
        if imgui.ColorPicker4("Pick color##" ..label,CRGBA) then
            memory.write(base_addr,CRGBA[0]*255,1)
            memory.write(base_addr+1,CRGBA[1]*255,1)
            memory.write(base_addr+2,CRGBA[2]*255,1)
            memory.write(base_addr+3,CRGBA[3]*255,1)
            if save then
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",base_addr),{1,CRGBA[0]*255,false,1})
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",base_addr+1),{1,CRGBA[1]*255,false,1})
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",base_addr+2),{1,CRGBA[2]*255,false,1})
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",base_addr+3),{1,CRGBA[3]*255,false,1})
            end
        end
        imgui.Spacing()
        if imgui.Button("Reset to default",imgui.ImVec2(module.GetSize(1))) then
            memory.write(base_addr,default_col[1],1)
            memory.write(base_addr+1,default_col[2],1)
            memory.write(base_addr+2,default_col[3],1)
            memory.write(base_addr+3,255,1)

            fconfig.tconfig.memory_data[tostring(base_addr)] = nil
            fconfig.tconfig.memory_data[tostring(base_addr+1)] = nil
            fconfig.tconfig.memory_data[tostring(base_addr+2)] = nil
            fconfig.tconfig.memory_data[tostring(base_addr+3)] = nil
        end
    end)
end


function module.GetLocationInfo(x,y,z)
	local interior = getActiveInterior() 

	local town_name = "San Andreas"
	local city =  getCityPlayerIsIn(PLAYER_PED)

	if city == 0 then
		town_name = "CS"
	end
	if city == 1 then
		town_name = "LS"
	end
	if city == 2 then
		town_name = "SF"
	end
	if city == 3 then
		town_name = "LV"
	end

	if interior == 0 then
		local zone_name = getGxtText(getNameOfZone(x,y,z))

		return string.format("%s, %s",zone_name,town_name)
	else
		return string.format("Interior %d, %s",getCharActiveInterior(PLAYER_PED),town_name)
	end
end

--------------------------------------------------
-- Function DrawEntries

local draw_entries_data = {}

function DrawImage(identifier,func_on_left_click,func_on_right_click,image_table,const_image_height,const_image_width,model,image,model_name)

    if type(image) ~= "string" then
        local images_in_row = math.floor(imgui.GetWindowContentRegionWidth()/const_image_width)
        local const_image_width = (imgui.GetWindowContentRegionWidth() - imgui.StyleVar.ItemSpacing*(images_in_row-0.6*images_in_row))/images_in_row

        if imgui.ImageButton(image,imgui.ImVec2(const_image_width,const_image_height),imgui.ImVec2(0,0),imgui.ImVec2(1,1),1,imgui.ImVec4(1,1,1,1),imgui.ImVec4(1,1,1,1)) then
            func_on_left_click(model)
        end
        if func_on_right_click ~= nil and imgui.IsItemClicked(1) then
            draw_entries_data[identifier].context_menu_data = 
            {
                func = func_on_right_click,
                key = model,
                category = model_name
            }
        end
    
        if imgui.IsItemHovered() then
            local drawlist = imgui.GetWindowDrawList()
            drawlist:AddRectFilled(imgui.GetItemRectMin(), imgui.GetItemRectMax(), imgui.GetColorU32(imgui.Col.ModalWindowDimBg))

            if imgui.CalcTextSize(model_name).x > const_image_width then
                model_name = model_name:gsub(" ","\n")
            end

            local offset_x = (imgui.GetItemRectSize().x - imgui.CalcTextSize(model_name).x)/2
            local offset_y = 10
            drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x+offset_x,imgui.GetItemRectMin().y+offset_y), imgui.GetColorU32(imgui.Col.Text),model_name)
        end

        if draw_entries_data[identifier].entry_count % images_in_row ~= 0 then
            imgui.SameLine(0.0,4.0)
        end
        draw_entries_data[identifier].entry_count = draw_entries_data[identifier].entry_count + 1
    end
end

function DrawText(identifier,func_on_left_click,func_on_right_click,entry,text,key,category)
    if imgui.MenuItemBool(text) then
        func_on_left_click(entry,category)
    end

    if func_on_right_click ~= nil and imgui.IsItemClicked(1) then
        draw_entries_data[identifier].context_menu_data = 
        {
            func = func_on_right_click,
            key = key
        }
    end
end


function module.DrawEntries(identifier,draw_type,func_on_left_click,func_on_right_click,func_get_name,data_table,const_image_height,const_image_width,verify_model_func)

    --------------------------------------------------
    -- Setup the temp table

    if draw_entries_data[identifier] == nil then
        draw_entries_data[identifier] = {
            filter   = imgui.ImGuiTextFilter(),
            entry_count   = 1,
            selected = "All",
        }
        
        if draw_type == fconst.DRAW_TYPE.IMAGE then
            lua_thread.create(function(data_table)
                for _,table in pairs(data_table) do
                    for model,image in pairs(table) do
                        if type(image) == "string" then
                            table[model] = imgui.CreateTextureFromFile(image)
                            wait(0)
                        end
                    end
                end
            end,data_table)
        end
        data_table[draw_entries_data[identifier].selected] = {}
    end

    if imgui.IsMouseClicked(1) then
        draw_entries_data[identifier].context_menu_data = nil
    end

    --------------------------------------------------
    -- Interface Header

    local width = imgui.GetWindowContentRegionWidth() - 8

    imgui.SetNextItemWidth(width/2)
    fcommon.DropDownListStr("##List",data_table,draw_entries_data[identifier].selected,
    function(key,val) 
        draw_entries_data[identifier].selected = key
    end)

    imgui.SameLine()

    imgui.SetNextItemWidth(width/2)
    
    draw_entries_data[identifier].filter:Draw("##Filter")
    if draw_entries_data[identifier].filter:PassFilter('') then
        local min = imgui.GetItemRectMin()
        local drawlist = imgui.GetWindowDrawList()
        drawlist:AddText(imgui.ImVec2(min.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),"Search")
    end
    imgui.Spacing()

    --------------------------------------------------
    -- Call the drawing function

    if imgui.BeginChild("##Draw") then 

        for category,table in pairs(data_table) do
            if draw_entries_data[identifier].selected == "All" or category == draw_entries_data[identifier].selected then
                for label,entry in pairs(table) do
                    local name = func_get_name(label)
                    if draw_entries_data[identifier].filter:PassFilter(name) and (verify_model_func == nil or verify_model_func(label)) then
                        if draw_type == fconst.DRAW_TYPE.IMAGE then
                            if identifier ~= fconst.IDENTIFIER.COMPONENT
                            or casts.CVehicle.IsValidModForVehicle(tonumber(label),getCarPointer(getCarCharIsUsing(PLAYER_PED))) then
                                DrawImage(identifier,func_on_left_click,func_on_right_click,table,const_image_height,const_image_width,label,entry,name)
                            end
                        else
                            DrawText(identifier,func_on_left_click,func_on_right_click,entry,name,label,category)
                        end
                    end
                end
            end
        end
        if draw_entries_data[identifier].context_menu_data ~= nil and imgui.BeginPopupContextWindow() then	
            
            if draw_type == fconst.DRAW_TYPE.IMAGE then
                imgui.Text(draw_entries_data[identifier].context_menu_data.key)
            end
            imgui.Separator()
            imgui.Spacing()
            draw_entries_data[identifier].context_menu_data.func(draw_entries_data[identifier].context_menu_data.key,draw_entries_data[identifier].context_menu_data.category)
            if imgui.MenuItemBool("Close") then 
				draw_entries_data[identifier].context_menu_data = nil	
            end
            imgui.EndPopup()       
        end
        
        draw_entries_data[identifier].entry_count = 1
        imgui.EndChild()
    end
    --------------------------------------------------

end
--------------------------------------------------

function module.RadioButtonAddressEx(label,label_table,values,memory,save)
    if save == nil then save = true end
    fcommon.DropDownMenu(label,function()
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
                if save then
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",memory),{1,values[i]})
                end
                button[0] = i
            end
            if i == btn_in_column then
                imgui.NextColumn()
            end
        end
        imgui.Columns(1)

    end)
end

function module.RadioButtonAddress(label,rb_table,addr_table,default)

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
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",addr_table[j]),{1,0})
            end
            module.CheatActivated()
        end
    end
    imgui.Columns(1)
end


function module.CallFuncButtons(label,table)

    fcommon.DropDownMenu(label,function()
        
        local count = 1
        for name,address in fcommon.spairs(table) do
            if imgui.Button(name,imgui.ImVec2(module.GetSize(3))) then
                callFunction(address,0,0)
                fcommon.CheatActivated()
            end
            if count % 3 ~= 0 then
                imgui.SameLine()
            end
            count = count + 1
        end
    end)
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

function module.CheckBoxVar(name,var,tooltip,panel_func,show_help_msg)
    show_help_msg = show_help_msg or true
    local temp = var[0]

    if imgui.Checkbox(name, var) then

        if show_help_msg then
            if var[0] then
                fcommon.CheatActivated()
            else
                fcommon.CheatDeactivated()
            end
        end
    end    

    module.InformationTooltip(tooltip)
    module.ConfigPanel(name,panel_func)

    if temp ~= var[0] then return true end
end

function module.CheckBoxFunc(name,var,func,tooltip)

    if imgui.Checkbox(name, var) then
        func()
    end

    module.InformationTooltip(tooltip)

end

function module.CheckBox3Var(name,var,tooltip,panel_func,show_help_msg)
    show_help_msg = show_help_msg or true
    local temp = var[0]
    tooltip = "Check mark - Enabled\nSquare mark - No changes\nEmpty box - Disabled\n\n" .. tooltip

    fcommon.CheckBox3(name, var)

    if imgui.IsItemClicked(0) or imgui.IsItemClicked(1) then
        if show_help_msg then
            fcommon.CheatActivated()
        end
    end

    module.InformationTooltip(tooltip)
    module.ConfigPanel(name,panel_func)

    if temp ~= var[0] then return true end
end

--------------------------------------------------

function module.InputFloat(label,var,func,min,max,cval)

    cval = cval or 1

    if imgui.InputFloat("##".. label,var,0.0,0.0,"%.5f") then
        if func ~= nil then
            func()
        end
    end

    local size = imgui.GetItemRectSize().y
    imgui.SameLine(0.0,4.0)

    if imgui.Button("-##".. label,imgui.ImVec2(size,size)) then
        var[0] = var[0] - cval
    end

    imgui.SameLine(0.0,4.0)
    
    if imgui.Button("+##".. label,imgui.ImVec2(size,size)) then
        var[0] = var[0] + cval
    end

    imgui.SameLine(0.0,4.0)

    imgui.Text(label)

    if min ~= nil and var[0] < min then
        var[0] = min
    end

    if max ~= nil and var[0] > max then
        var[0] = max
    end
end

function module.UpdateStat(arg)
    if arg.min == nil then arg.min = 0 end
    if arg.default == nil then arg.default = 0 end
    if arg.max == nil then arg.max = 1000 end

    fcommon.DropDownMenu(arg.name,function()

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
        if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(module.GetSize(3))) then
            setFloatStat(arg.stat,arg.min)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.min)
        end
        imgui.SameLine()
        if imgui.Button("Default##".. arg.name,imgui.ImVec2(module.GetSize(3))) then
            setFloatStat(arg.stat,arg.default)
            fconfig.Set(fconfig.tconfig.stat_data,tostring(arg.stat),arg.default)
        end
        imgui.SameLine()
        if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(module.GetSize(3))) then
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

function module.UpdateAddress(arg)

    if arg.cvalue == nil then arg.cvalue = 1.0  end
    if arg.save == nil then arg.save = true end
    if arg.mul == nil then arg.mul = 1.0  end

    local buttons = 3

    if arg.min == nil then
        buttons = buttons - 1
    end

    if arg.default == nil then
        buttons = buttons - 1
    end

    if arg.max == nil then
        buttons = buttons - 1
    end

    fcommon.DropDownMenu(arg.name,function()

        local value = imgui.new.float(module.RwMemory(arg.address,arg.size,nil,nil,arg.is_float,arg.mul))
        
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
            module.RwMemory(arg.address,arg.size,value[0],nil,arg.is_float,arg.mul)
            if arg.save then
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,value[0],arg.is_float,arg.mul})
            end
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("-##".. arg.name,imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] - arg.cvalue),nil,arg.is_float,arg.mul)
            if arg.save then
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,(value[0] - arg.cvalue),arg.is_float,arg.mul})
            end
        end
        imgui.SameLine(0.0,4.0)
        if imgui.Button("+##".. arg.name,imgui.ImVec2(20,20)) then
            module.RwMemory(arg.address,arg.size,(value[0] + arg.cvalue),nil,arg.is_float,arg.mul)
            if arg.save then
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,(value[0] + arg.cvalue),arg.is_float,arg.mul})
            end
        end
        imgui.SameLine(0.0,4.0)
        imgui.Text("Set")
        if buttons > 0 then
            imgui.Spacing()
            if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(module.GetSize(buttons))) then
                module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.mul)
                if arg.save then
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.min,arg.is_float,arg.mul})
                end
            end

            if arg.default ~= nil then
                imgui.SameLine()
                if imgui.Button("Default##".. arg.name,imgui.ImVec2(module.GetSize(buttons))) then
                    module.RwMemory(arg.address,arg.size,arg.default,nil,arg.is_float,arg.mul)
                    if arg.save then
                        fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.default,arg.is_float,arg.mul})
                    end
                end
            end

            if arg.max ~= nil then
                imgui.SameLine()
                if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(module.GetSize(buttons))) then
                    module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float,arg.mul)
                    if arg.save then
                        fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.max,arg.is_float,arg.mul})
                    end
                end
            end
            imgui.SameLine()
            imgui.Spacing()
        end
        if (arg.min ~= nil) and (value[0] < arg.min) then
            module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.mul)
            if arg.save then
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.min,arg.is_float,arg.mul})
            end
        end

        if (arg.max ~= nil) and (value[0] > arg.max) then
            module.RwMemory(arg.address,arg.size,arg.max,nil,arg.is_float,arg.mul)
            if arg.save then
                fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.max,arg.is_float,arg.mul})
            end
        end
    end)
end

-- Provides options to change memory bits
function module.UpdateBits(label,name_table,address,size)

    if name_table == nil then name_table = {} end
    local bits = size*8
    local val = readMemory(address,size,false)
    local number = 1
    fcommon.DropDownMenu(label,function()
        imgui.Columns(2,nil,false)
        for i=1,bits,1 do

            local state = imgui.new.bool(false)   

            if bit.band(val,number) == number then
                state[0] = true
            end

            if name_table[tostring(i)] == nil then name_table[tostring(i)] = "Unknown ##" .. tostring(i) end

            if imgui.Checkbox(name_table[tostring(i)],state) then
                val = bit.bxor(val,number)
                writeMemory(address,size,val,false)
            end

            if i == bits/2 then
                imgui.NextColumn()
            end
            number = number * 2
        end
        imgui.Columns(1)
    end)
end

--------------------------------------------------
-- Misc

-- Indexes image paths recursively from root directory
function module.IndexFiles(mainDir,store_table,req_ext)
    load_images = load_images or false

    process_file = function(file_path,element)
        if doesFileExist(file_path) then
            local _,file_name,file_ext = string.match(file_path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 
            file_name = string.sub(file_name,1,-5)
            if req_ext == file_ext then
                if store_table[element] == nil then
                    store_table[element] = {}
                end
                if (req_ext == "jpg" or req_ext == "png") and fmenu.tmenu.fast_load_images[0] then
                    store_table[element][file_name] = imgui.CreateTextureFromFile(file_path)
                else
                    store_table[element][file_name] = file_path
                end
            end
        end
    end

    for element in lfs.dir(mainDir) do
        local ele_path = mainDir .. "\\" .. element
        if doesDirectoryExist(ele_path) and element ~= "." and element ~= ".." then
            for file in lfs.dir(ele_path) do
                local file_path = ele_path .. "\\" .. file
                process_file(file_path,element)
            end
        end
        if doesFileExist(ele_path) then
            process_file(ele_path,element)
        end
    end
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
function module.RwMemory(address,size,value,protect,is_float,mul)
    if protect == nil then protect = false end
    if mul == nil then mul = 1 end

    if value == nil then
        local return_val = nil
        if is_float == true then
            return_val = memory.getfloat(address,protect)
        else
            return_val =  readMemory(address,size,protect)
        end

        return return_val*mul
    else
        if is_float == true then
            memory.setfloat(address,value/mul,protect)
        else
            writeMemory(address,size,value/mul,protect)
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


--------------------------------------------------
-- Custom imgui widgets
--------------------------------------------------
-- Scrollable Tabs Implementation

local _tabs = 
{
    current_bar = nil,
    panel_func = nil,
}


function module.BeginTabBar(label)

    if _tabs[label] == nil then 
        _tabs[label] = 
        {
            bar_data = {},
            selected_tab = "",
        }
    end
    _tabs.current_bar = label
    
    local draw_list = imgui.GetWindowDrawList(),

    imgui.Spacing()
    if #_tabs[label].bar_data > 0 then
        imgui.BeginChild(label,imgui.ImVec2(imgui.GetWindowWidth()-imgui.GetStyle().WindowPadding.x*2,imgui.GetFrameHeight()*0.9))

        imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing, imgui.ImVec2(4.0))
        for k,v in ipairs(_tabs[label].bar_data) do
            DrawTabElements(draw_list,v)
        end
        imgui.PopStyleVar()
        
        local minx  = imgui.GetWindowPos().x
        local maxy  = imgui.GetItemRectMax().y
        draw_list:AddLine(imgui.ImVec2(minx-5,maxy), imgui.ImVec2(minx+imgui.GetWindowWidth()+3,maxy), imgui.GetColorU32(imgui.Col.TabActive))

        imgui.InvisibleButton("?##InfoTooltip",imgui.CalcTextSize("?"))
        draw_list:AddText(imgui.ImVec2(imgui.GetItemRectMin().x,imgui.GetItemRectMin().y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),"?")
        
        if imgui.IsItemHovered() then
            imgui.BeginTooltip()
            imgui.SetTooltip("If your window width is small you\ncan scroll by Shift + Mouse wheel")
            imgui.EndTooltip()
        end
        imgui.EndChild()
        imgui.Spacing()
    end
    imgui.BeginChild("##TabStuff" .. label)
    return true
end

function module.BeginTabItem(label)

    if _tabs.current_bar == nil then return false end

    local exists = false
    
    for k,v in ipairs(_tabs[_tabs.current_bar].bar_data) do
        if v == label then
            exists = true
        end
    end
    if not exists then
        _tabs[_tabs.current_bar].bar_data[#(_tabs[_tabs.current_bar].bar_data)+1] = label
    end


    if _tabs[_tabs.current_bar].selected_tab == label then
        if _tabs.panel_func == nil then
            return true
        else
            _tabs.panel_func()
        end
    end
end

function DrawTabElements(draw_list,label)
        
    if _tabs[_tabs.current_bar].selected_tab == "" then _tabs[_tabs.current_bar].selected_tab = label end

    if _tabs[_tabs.current_bar].selected_tab == label then
        btn_color = imgui.GetColorU32(imgui.Col.TabActive)
    else
        btn_color = imgui.GetColorU32(imgui.Col.Tab)
    end

    if imgui.InvisibleButton("##InvisibleButton".. label, imgui.ImVec2(imgui.CalcTextSize(label).x+10,imgui.GetFrameHeight()*0.9)) then
        _tabs[_tabs.current_bar].selected_tab = label
        _tabs.panel_func = nil
    end
    
    if imgui.IsItemHovered() then
        btn_color = imgui.GetColorU32(imgui.Col.TabHovered)
    end

    draw_list:AddRectFilled(imgui.GetItemRectMin(), imgui.GetItemRectMax(), btn_color)
    local offset_x = (imgui.GetItemRectSize().x - imgui.CalcTextSize(label).x)/2
    local offset_y = (imgui.GetItemRectSize().y - imgui.CalcTextSize(label).y)/2
    
    draw_list:AddText(imgui.ImVec2(imgui.GetItemRectMin().x+offset_x,imgui.GetItemRectMin().y+offset_y), imgui.GetColorU32(imgui.Col.Text),label)
    imgui.SameLine()
    
end

function module.EndTabItem(label) end 
function module.EndTabBar(label) 
    imgui.EndChild()
    _tabs.current_bar = nil
end


function module.ConfigPanel(label,func)
    if func ~= nil then
        imgui.SameLine()
        imgui.InvisibleButton("c##".. label,imgui.CalcTextSize("c"))
        local drawlist = imgui.GetWindowDrawList()
        drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x,imgui.GetItemRectMin().y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),"c")
        
        
        if imgui.IsItemClicked(0) then
            _tabs.panel_func = function()
                imgui.TextWrapped(string.format("%s configuraion",label))
                imgui.Separator()
                if imgui.Button("Hide",imgui.ImVec2(module.GetSize(1))) then
                    _tabs.panel_func = nil
                end
                
                imgui.Dummy(imgui.ImVec2(0,10))
                func()
            end
        end
    end
end


function module.CreateMenus(names,funcs)

    imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing,imgui.ImVec2(0,0.5))

    local button = imgui.ColorConvertFloat4ToU32(imgui.GetStyle()['Colors'][21])

    for i=1,#names,1 do
        if tcheatmenu.current_menu == i then
            imgui.GetStyle().Colors[imgui.Col.Button] = imgui.GetStyle().Colors[imgui.Col.ButtonActive]
        end
        if imgui.Button(names[i],imgui.ImVec2(module.GetSize(4,true))) then
            tcheatmenu.current_menu = i
            _tabs.panel_func = nil
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


--------------------------------------------------
-- Hotkey Implementation

local _hotkeys = 
{
    currently_active = nil,
}

function module.GetHotKeyNames(hotkeys)

    if hotkeys[1] == hotkeys[2] then
        return string.format(" %s",vkeys.id_to_name(hotkeys[1]))
    else
        return string.format(" %s + %s",vkeys.id_to_name(hotkeys[1]),vkeys.id_to_name(hotkeys[2]))
    end

end

function module.OnHotKeyPress(hotkeys,func)
    if isKeyDown(hotkeys[1]) and isKeyDown(hotkeys[2]) and _hotkeys.currently_active ~= hotkeys then
        while isKeyDown(hotkeys[1]) and isKeyDown(hotkeys[2]) do
            wait(0)
        end
        func()
    end
end


function ReadKeyPress()
    while _hotkeys.read_key_press do

        for i=32,255,1 do
            if isKeyDown(i) then
                _hotkeys.currently_active[1] = i
                break
            end
        end

        for i=255,32,-1 do
            if isKeyDown(i) then
                _hotkeys.currently_active[2] = i
                break
            end
        end
        
        if _hotkeys.currently_active[1] ~= _hotkeys.currently_active[2] then
            while isKeyDown(_hotkeys.currently_active[1]) or isKeyDown(_hotkeys.currently_active[2]) do
                wait(0)
            end
        end
        wait(0)
    end
end

function module.HotKey(label,hotkeys)
    local active = false
    local x,y = module.GetSize(3)
    y = y/1.2
    
    if hotkeys == _hotkeys.currently_active then
        imgui.PushStyleColor(imgui.Col.Button,imgui.GetStyle().Colors[imgui.Col.ButtonActive])
        active = true
    end

    if hotkeys[1] == hotkeys[2] then
        text = vkeys.id_to_name(hotkeys[1])
    else
        text = vkeys.id_to_name(hotkeys[1]) .. " + " .. vkeys.id_to_name(hotkeys[2])
    end

    if imgui.Button(text,imgui.ImVec2(x,y)) then
        if _hotkeys.currently_active == hotkeys then
            _hotkeys.read_key_press = false
            _hotkeys.currently_active = {}
        else
            _hotkeys.currently_active = hotkeys
            _hotkeys.read_key_press = true
            lua_thread.create(ReadKeyPress)
        end
    end

    if active then
        imgui.PopStyleColor()
    end  

    if not imgui.IsWindowFocused() or not imgui.IsItemVisible() then
        _hotkeys.currently_active = {}
    end


    imgui.SameLine()

    imgui.Text(label)
end
--------------------------------------------------
-- Misc

function module.HorizontalSelector(label,var,table)
    local rtn = false
    local drawlist = imgui.GetWindowDrawList()
    local hframe = imgui.GetFrameHeight()
    local inner_spacing = imgui.GetStyle().ItemInnerSpacing.x
    local width = imgui.GetWindowContentRegionWidth()*0.65 - 4*hframe - inner_spacing + 6

    imgui.InvisibleButton("##1" .. label,imgui.ImVec2(width,hframe))
    local min = imgui.GetItemRectMin()
    local max = imgui.GetItemRectMax()
    drawlist:AddRectFilled(min, max, imgui.GetColorU32(imgui.Col.FrameBg))
    drawlist:AddText(imgui.ImVec2(min.x+inner_spacing,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.Text),table[var[0]+2])

    
    imgui.SameLine(0,inner_spacing)

    if imgui.Button("<##2" .. label,imgui.ImVec2(2*hframe,hframe)) then
        rtn = -1

        if var[0] > -1 then
            var[0] = var[0] - 1
        else 
            var[0] = #table-2
        end
    end

    imgui.SameLine(0,inner_spacing)

    if imgui.Button(">##2" .. label,imgui.ImVec2(2*hframe,hframe)) then
        rtn = 1

        if var[0] < (#table-2) then
            var[0] = var[0] + 1
        else
            var[0] = -1
        end
    end

    min = imgui.GetItemRectMin()
    max = imgui.GetItemRectMax()
    imgui.SameLine()
    imgui.InvisibleButton("##4" ..label,imgui.ImVec2(imgui.CalcTextSize(label).x,hframe))
    drawlist:AddText(imgui.ImVec2(max.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.Text),label)

    return rtn
end

function module.DropDownListStr(label,table,selected,func)
    if imgui.BeginCombo(label, selected) then
        for key,val in module.spairs(table) do
            if key ~= selected then
                if imgui.MenuItemBool(key) and func ~= nil then
                    func(key,val)
                end
            end
        end
        imgui.EndCombo()
    end
end

function module.DropDownListNumber(label,table,selected)
    local rtn = false

    if imgui.BeginCombo(label, table[selected[0]]) then
        for key,val in ipairs(table) do
            if key ~= selected[0] then
                if imgui.MenuItemBool(val)then
                    rtn = true
                    selected[0] = key
                end
            end
        end
        imgui.EndCombo()
    end
    return rtn
end

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

function RenderCheckMark(drawlist, pos, col, sz)

    local thickness = sz / 5.0
    thickness = thickness < 1.0 and 1.0 or thickness

    sz = sz - thickness * 0.5
    pos.x = pos.x + thickness * 0.25
    pos.y = pos.y + thickness * 0.25

    local third = sz / 3.0
    local bx = pos.x + third
    local by = pos.y + sz - third * 0.5

    drawlist:PathLineTo(imgui.ImVec2(bx - third, by - third))
    drawlist:PathLineTo(imgui.ImVec2(bx, by))
    drawlist:PathLineTo(imgui.ImVec2(bx + third * 2.0, by - third * 2.0))
    drawlist:PathStroke(col, false, thickness)
end

-- 3 state checkbox, 1 = enabled, 0 = disabled, -1 = not configured
function module.CheckBox3(label,var)
    local square_sz = imgui.GetFrameHeight()
    local drawlist = imgui.GetWindowDrawList()
    local color = imgui.GetColorU32(imgui.Col.FrameBg)
    local text_size = imgui.CalcTextSize(label)

    if imgui.InvisibleButton("##InvCheckboxBtn" .. label, imgui.ImVec2(square_sz+text_size.x+imgui.GetStyle().ItemInnerSpacing.x,square_sz)) then
        var[0] = var[0] - 1
        var[0] = var[0] < -1 and 1 or var[0]
    end

    if imgui.IsItemClicked(1) then
        var[0] = var[0] + 1
        var[0] = var[0] > 1 and -1 or var[0]
    end

    if imgui.IsItemHovered() then
        color = imgui.GetColorU32(imgui.Col.FrameBgHovered)
    end
    
    local min = imgui.GetItemRectMin()
    local max = imgui.ImVec2(imgui.GetItemRectMax().x-text_size.x-imgui.GetStyle().ItemInnerSpacing.x,imgui.GetItemRectMax().y)
    drawlist:AddRectFilled(min, max, color)
    local pad = math.floor(square_sz / 6.0)
    pad = pad < 1.0 and 1.0 or pad 

    if var[0] == -1 then
        drawlist:AddRectFilled(imgui.ImVec2(min.x+3,min.y+3), imgui.ImVec2(max.x-3,max.y-3), imgui.GetColorU32(imgui.Col.CheckMark))
    end
    if var[0] == 1 then
        RenderCheckMark(drawlist, imgui.ImVec2(min.x+pad,min.y+pad), imgui.GetColorU32(imgui.Col.CheckMark), square_sz - pad * 2.0)
    end

    drawlist:AddText(imgui.ImVec2(max.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.Text),label)
    return imgui.IsItemClicked(0)
end

--------------------------------------------------


return module
