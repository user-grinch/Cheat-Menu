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

    fcommon.KeyWait(tkeys.control_key,tkeys.quickspawner_key)

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
-- imgui UI functions

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

-- Calculates width of element(button) acoording to count
function module.GetSize(count,no_spacing)
  
    if x == nil then  x = 20 end
    if y == nil then  y = 25 end
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

    imgui.PushStyleVarVec2(imgui.StyleVar.ItemSpacing,imgui.ImVec2(0,1))
    
    for i=1,#names,1 do
        if tcheatmenu.current_menu == i then
            imgui.PushStyleColor(imgui.Col.Button, imgui.ImVec4(0.060,0.530,0.980,1.0))
        end
        if imgui.Button(names[i],imgui.ImVec2(module.GetSize(4,true))) then
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
    imgui.Dummy(imgui.ImVec2(0,5))
    funcs[tcheatmenu.current_menu]()
  
end

-- Shows loaded images with on_click and on_hover calls
function module.DrawImages(identifier,draw_type,loaded_images_list,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,search_box_text)
        
    if search_box_text == nil then search_box_text = "" end

    local loading_a_image = false

    -- Calculate image count in a row
    local images_in_row = math.floor(imgui.GetWindowContentRegionWidth()/const_image_width)
    local image_count   = 1

    draw_image = function(identifier,image_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,search_box_text,model,image)
        local model_name = nil
        if func_get_name ~= nil then
            model_name = func_get_name(model)
        end
        if ((search_box_text == "") or ((string.upper(model_name):find(string.upper(search_box_text))) ~= nil)) then 
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
                            draw_image(identifier,valid_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,search_box_text,model,image)
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
        if imgui.BeginChild("") then 
            for _,image_table in pairs(loaded_images_list) do
                for model,image in pairs(image_table) do
                    draw_image(identifier,image_table,const_image_height,const_image_width,func_on_left_click,func_on_right_click,func_get_name,search_box_text,model,image)
                end
            end
            imgui.EndChild()
        end
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

-- Similar UI to the previous function
-- Provides input options to change game memory values
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

--------------------------------------------------


--------------------------------------------------
-- Functions for loading & saving json files

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
--------------------------------------------------


--------------------------------------------------
-- Misc

-- Loads images recursively from root directory
function module.LoadImages(mainDir,store_image_table,req_ext,dir)
    for file in lfs.dir(mainDir) do
        wait(0)
        if doesFileExist(mainDir .. "\\" .. file) then
            local _,file_name,file_ext = string.match(mainDir .. "\\" .. file, "(.-)([^\\/]-%.?([^%.\\/]*))$") 
            file_name = string.sub(file_name,1,-5)

            if req_ext == file_ext then
                if store_image_table[dir] == nil then
                    store_image_table[dir] = {}
                end
                store_image_table[dir][file_name] = imgui.CreateTextureFromFile(mainDir .. file)
            end
        end
        if doesDirectoryExist(mainDir .. "\\" .. file) and file ~= "." and file ~= ".." then
           module.LoadImages(mainDir .. "\\" .. file .. "\\",store_image_table,req_ext,file)
        end
    end
end

-- Used to read/write from/to memory
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
