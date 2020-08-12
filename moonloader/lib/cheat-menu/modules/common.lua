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

function module.LockThread(text)
    
    if tcheatmenu.thread_locks[text] == nil then
        tcheatmenu.thread_locks[text] = true
        return true
    end

    return false
end

function module.UnlockThread(text)
    
    if tcheatmenu.thread_locks[text] ~= nil then
        tcheatmenu.thread_locks[text] = nil
    end
end

function module.SingletonThread(func,thread_lock_key)
    thread_lock_key = thread_lock_key or tostring(func) -- get the function address if no key provided
    lua_thread.create(function()
        if module.LockThread(thread_lock_key) then
            func()
            module.UnlockThread(thread_lock_key)
        end
        end,func,thread_lock_key)
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
-- imgui functions

function module.DropDownList(label,table,selected,func)
    if imgui.BeginCombo(label, selected) then
        for key,val in fcommon.spairs(table) do
            if key ~= selected then
                if imgui.MenuItemBool(key) and func ~= nil then
                    func(key,val)
                end
            end
        end
        imgui.EndCombo()
    end
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

function module.InformationTooltip(text)
    if fmenu.tmenu.show_tooltips[0] and text ~= nil then
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
function module.ConfigPanel(label,func)
    if func ~= nil then
        imgui.SameLine()
        imgui.InvisibleButton("c##".. label,imgui.CalcTextSize("c"))
        local drawlist = imgui.GetWindowDrawList()
        drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x,imgui.GetItemRectMin().y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),"c")
        
        
        if imgui.IsItemClicked(0) then
            tcheatmenu.window.panel_func = function()
                imgui.TextWrapped(string.format("%s configuraion",label))
                imgui.Separator()
                if imgui.Button("Hide",imgui.ImVec2(fcommon.GetSize(1))) then
                    tcheatmenu.window.panel_func = nil
                end
                
                imgui.Dummy(imgui.ImVec2(0,10))
                func()
            end
        end
    end
end

function module.InputText(label,var,hint)
    imgui.InputText(label,var,ffi.sizeof(var))

    if hint ~= nil and ffi.string(var) == "" then
        local min = imgui.GetItemRectMin()
        local drawlist = imgui.GetWindowDrawList()
        drawlist:AddText(imgui.ImVec2(min.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),hint)
    end

    return imgui.IsItemActivated()
end

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

function module.Tabs(label,names,func)

    if tcheatmenu.tab_data[label] == nil then
        tcheatmenu.tab_data[label] = 1
    end

    imgui.Spacing()

    local square_sz = imgui.GetFrameHeight()*0.9
    local drawlist = imgui.GetWindowDrawList()
    local btn_color = nil

    if imgui.BeginChild(label,imgui.ImVec2(tcheatmenu.window.size.X-imgui.GetStyle().WindowPadding.x*2,square_sz)) then
        for i=1,#names,1 do
            if tcheatmenu.tab_data[label] == i then
                btn_color = imgui.GetColorU32(imgui.Col.TabActive)
            else
                btn_color = imgui.GetColorU32(imgui.Col.Tab)
            end
      
            if imgui.InvisibleButton("##InvisibleButton".. names[i], imgui.ImVec2(imgui.CalcTextSize(names[i]).x+10,square_sz)) then
                tcheatmenu.tab_data[label] = i
            end
            if imgui.IsItemHovered() then
                btn_color = imgui.GetColorU32(imgui.Col.TabHovered)
            end

            drawlist:AddRectFilled(imgui.GetItemRectMin(), imgui.GetItemRectMax(), btn_color)
            local offset_x = (imgui.GetItemRectSize().x - imgui.CalcTextSize(names[i]).x)/2
            local offset_y = (imgui.GetItemRectSize().y - imgui.CalcTextSize(names[i]).y)/2
            
            drawlist:AddText(imgui.ImVec2(imgui.GetItemRectMin().x+offset_x,imgui.GetItemRectMin().y+offset_y), imgui.GetColorU32(imgui.Col.Text),names[i])
            imgui.SameLine()
        end
        
        local minx  = imgui.GetWindowPos().x
        local maxy  = imgui.GetItemRectMax().y
        drawlist:AddLine(imgui.ImVec2(minx-5,maxy), imgui.ImVec2(minx+tcheatmenu.window.size.X,maxy), imgui.GetColorU32(imgui.Col.TabActive))
        fcommon.InformationTooltip("If your window width is small you\ncan scroll by Shift + Mouse wheel")

        imgui.EndChild()
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
        if imgui.Button("Reset to default",imgui.ImVec2(fcommon.GetSize(1))) then
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

function DrawImage(identifier,func_on_left_click,func_on_right_click,image_table,const_image_height,const_image_width,model,image,model_name)

    if identifier ~= fconst.IDENTIFIER.COMPONENT or casts.CVehicle.IsValidModForVehicle(tonumber(model),getCarPointer(getCarCharIsUsing(PLAYER_PED))) then
        if type(image) ~= "string" then
            local images_in_row = math.floor(imgui.GetWindowContentRegionWidth()/const_image_width)
            local const_image_width = (imgui.GetWindowContentRegionWidth() - imgui.StyleVar.ItemSpacing*(images_in_row-0.6*images_in_row))/images_in_row

            if imgui.ImageButton(image,imgui.ImVec2(const_image_width,const_image_height),imgui.ImVec2(0,0),imgui.ImVec2(1,1),1,imgui.ImVec4(1,1,1,1),imgui.ImVec4(1,1,1,1)) then
                func_on_left_click(model)
            end
            if func_on_right_click ~= nil and imgui.IsItemClicked(1) then
                tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data = 
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

            if tcheatmenu.temp_data.draw_entries_func[identifier].entry_count % images_in_row ~= 0 then
                imgui.SameLine(0.0,4.0)
            end
            tcheatmenu.temp_data.draw_entries_func[identifier].entry_count = tcheatmenu.temp_data.draw_entries_func[identifier].entry_count + 1
        end
    end
end

function DrawText(identifier,func_on_left_click,func_on_right_click,entry,text,key,category)
    if imgui.MenuItemBool(text) then
        func_on_left_click(entry,category)
    end

    if func_on_right_click ~= nil and imgui.IsItemClicked(1) then
        tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data = 
        {
            func = func_on_right_click,
            key = key,
            category = category
        }
    end
end


function module.DrawEntries(identifier,draw_type,func_on_left_click,func_on_right_click,func_get_name,data_table,const_image_height,const_image_width)

    --------------------------------------------------
    -- Setup the temp table

    if tcheatmenu.temp_data.draw_entries_func[identifier] == nil then
        tcheatmenu.temp_data.draw_entries_func[identifier] = {
            filter   = imgui.ImGuiTextFilter(),
            entry_count   = 1,
            entry_table   = {["All"] = {}},
            selected = "All",
            veh_model = nil,
        }
    end

    if imgui.IsMouseClicked(1) then
        tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data = nil
    end

    if identifier == fconst.IDENTIFIER.COMPONENT then
        local hveh = getCarCharIsUsing(PLAYER_PED)
        local model = getCarModel(hveh)

        if tcheatmenu.temp_data.draw_entries_func[identifier].veh_model ~= model then
            tcheatmenu.temp_data.draw_entries_func[identifier].veh_model = model
            for category,table in pairs(data_table) do
                for model,data in pairs(table) do
                    if casts.CVehicle.IsValidModForVehicle(tonumber(model),getCarPointer(hveh)) then
                        if tcheatmenu.temp_data.draw_entries_func[identifier].entry_table[category] == nil then
                            tcheatmenu.temp_data.draw_entries_func[identifier].entry_table[category] = {}
                        end
                        tcheatmenu.temp_data.draw_entries_func[identifier].entry_table[category][model] = data
                    end
                end
            end
        end
    else
        tcheatmenu.temp_data.draw_entries_func[identifier].entry_table = data_table
        
        if tcheatmenu.temp_data.draw_entries_func[identifier].entry_table["All"] == nil then
            tcheatmenu.temp_data.draw_entries_func[identifier].entry_table["All"] = {}
        end
    end

    --------------------------------------------------
    -- Interface Header

    local width = imgui.GetWindowContentRegionWidth() - 8

    imgui.SetNextItemWidth(width/2)
    fcommon.DropDownList("##List",tcheatmenu.temp_data.draw_entries_func[identifier].entry_table,tcheatmenu.temp_data.draw_entries_func[identifier].selected,
    function(key,val) 
        tcheatmenu.temp_data.draw_entries_func[identifier].selected = key
    end)

    imgui.SameLine()

    imgui.SetNextItemWidth(width/2)
    
    tcheatmenu.temp_data.draw_entries_func[identifier].filter:Draw("##Filter")
    if tcheatmenu.temp_data.draw_entries_func[identifier].filter:PassFilter('') then
        local min = imgui.GetItemRectMin()
        local drawlist = imgui.GetWindowDrawList()
        drawlist:AddText(imgui.ImVec2(min.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.TextDisabled),"Search")
    end
    imgui.Spacing()

    --------------------------------------------------
    -- Call the drawing function

    if imgui.BeginChild("##Draw") then 
        for category,table in pairs(tcheatmenu.temp_data.draw_entries_func[identifier].entry_table) do
            if draw_type == fconst.DRAW_TYPE.IMAGE then
                lua_thread.create(LoadImages,table)
            end
            if tcheatmenu.temp_data.draw_entries_func[identifier].selected == "All" or category == tcheatmenu.temp_data.draw_entries_func[identifier].selected then
                for label,entry in pairs(table) do
                    local name = func_get_name(label)
                    if draw_type == fconst.DRAW_TYPE.IMAGE then
                        lua_thread.create(LoadImages,table)
                    end
                    if tcheatmenu.temp_data.draw_entries_func[identifier].filter:PassFilter(name) then
                        if draw_type == fconst.DRAW_TYPE.IMAGE then
                            DrawImage(identifier,func_on_left_click,func_on_right_click,table,const_image_height,const_image_width,label,entry,name)
                        else
                            DrawText(identifier,func_on_left_click,func_on_right_click,entry,name,label,category)
                        end
                    end
                end
            end
        end
        if tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data ~= nil and imgui.BeginPopupContextWindow() then	
            
            if draw_type == fconst.DRAW_TYPE.IMAGE then
                imgui.Text(tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data.category)
            else
                imgui.Text(tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data.key)
            end
            imgui.Separator()
            imgui.Spacing()
            tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data.func(tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data.key,tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data.category)
            if imgui.MenuItemBool("Close") then 
				tcheatmenu.temp_data.draw_entries_func[identifier].context_menu_data = nil	
            end
            imgui.EndPopup()       
        end
        
        tcheatmenu.temp_data.draw_entries_func[identifier].entry_count = 1
        imgui.EndChild()
    end
    --------------------------------------------------

end

function module.RadioButtonAddressEx(label,label_table,values,memory,save)
    if save == nil then save = true end
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
    local sizeX = fcommon.GetSize(3)
    local sizeY = imgui.GetWindowHeight()/10

    fcommon.DropDownMenu(label,function()
        
        local count = 1
        for name,address in fcommon.spairs(table) do
            if imgui.Button(name,imgui.ImVec2(sizeX,sizeY)) then
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

function module.CheckBoxVar(name,var,tooltip,func,panel_func,show_help_msg)
    show_help_msg = show_help_msg or true

    if imgui.Checkbox(name, var) then

        if show_help_msg then
            if var[0] then
                fcommon.CheatActivated()
            else
                fcommon.CheatDeactivated()
            end
        end
        if func ~= nil then
            func()
        end
    end

    module.InformationTooltip(tooltip)
    module.ConfigPanel(name,panel_func)
end

function module.CheckBoxFunc(name,var,func,tooltip,panel_func)

    if imgui.Checkbox(name, var) then
        func()
    end

    module.InformationTooltip(tooltip)

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

    if var[0] == fconst.CHECKBOX_STATE.NOT_CONFIGURED then
        drawlist:AddRectFilled(imgui.ImVec2(min.x+3,min.y+3), imgui.ImVec2(max.x-3,max.y-3), imgui.GetColorU32(imgui.Col.CheckMark))
    end
    if var[0] == fconst.CHECKBOX_STATE.ON then
        RenderCheckMark(drawlist, imgui.ImVec2(min.x+pad,min.y+pad), imgui.GetColorU32(imgui.Col.CheckMark), square_sz - pad * 2.0)
    end

    drawlist:AddText(imgui.ImVec2(max.x+imgui.GetStyle().ItemInnerSpacing.x,min.y+imgui.GetStyle().FramePadding.y), imgui.GetColorU32(imgui.Col.Text),label)
    return imgui.IsItemClicked(0)
end

function module.CheckBox3Var(name,var,tooltip,func,panel_func,show_help_msg)
    show_help_msg = show_help_msg or true

    if module.CheckBox3(name, var) then

        if show_help_msg then
            if var[0] == fconst.CHECKBOX_STATE.ON then
                fcommon.CheatActivated()
            end
            if var[0] == fconst.CHECKBOX_STATE.OFF then
                fcommon.CheatDeactivated()
            end
        end
        if func ~= nil then
            func()
        end
    end

    module.InformationTooltip(tooltip)
    module.ConfigPanel(name,panel_func)
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

    module.DropDownMenu(arg.name,function()

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
            if imgui.Button("Minimum##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
                module.RwMemory(arg.address,arg.size,arg.min,nil,arg.is_float,arg.mul)
                if arg.save then
                    fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.min,arg.is_float,arg.mul})
                end
            end

            if arg.default ~= nil then
                imgui.SameLine()
                if imgui.Button("Default##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
                    module.RwMemory(arg.address,arg.size,arg.default,nil,arg.is_float,arg.mul)
                    if arg.save then
                        fconfig.Set(fconfig.tconfig.memory_data,string.format("0x%6.6X",arg.address),{arg.size,arg.default,arg.is_float,arg.mul})
                    end
                end
            end

            if arg.max ~= nil then
                imgui.SameLine()
                if imgui.Button("Maximum##".. arg.name,imgui.ImVec2(fcommon.GetSize(buttons))) then
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
    module.DropDownMenu(label,function()
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
            module.SingletonThread(module.ReadKeyPress,"ReadKeyPress")
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
    while tcheatmenu.read_key_press do

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
        wait(0)
    end
end

--------------------------------------------------
-- Misc

-- Indexes image paths recursively from root directory
function module.IndexFiles(mainDir,store_table,req_ext,load_images)
    load_images = load_images or false

    process_file = function(file_path,element)
        if doesFileExist(file_path) then
            local _,file_name,file_ext = string.match(file_path, "(.-)([^\\/]-%.?([^%.\\/]*))$") 
            file_name = string.sub(file_name,1,-5)
            if req_ext == file_ext then
                if store_table[element] == nil then
                    store_table[element] = {}
                end
                if fmenu.tmenu.fast_load_images[0] and load_images then
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
            if not fmenu.tmenu.fast_load_images[0] then
                wait(0)
            end
        end
        if doesFileExist(ele_path) then
            process_file(ele_path,element)
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

return module
