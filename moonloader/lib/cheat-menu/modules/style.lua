------------------------------------------------------------------------------
-- This specific module was created by randazzo <https://github.com/randazz0>
------------------------------------------------------------------------------

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

module.tstyle = 
{
    array          = nil,
    current_font   = "",
    fonts          = {},
    font_size      = fconfig.Get('tstyle.font_size',14),
    font_size_var  = imgui.new.int(fconfig.Get('tstyle.font_size',14)),
    list           = nil,
    name           = imgui.new.char[256]("Untitled"),
    preparetoapply = false,
    selected       = imgui.new.int(0),
    selected_name  = fconfig.Get('tstyle.selected_name',"Default"),
    status         = nil,
    styles_table   = {},
}

local _ImGuiStyle =
{
    'Alpha',                      
    'WindowPadding',              
    'WindowRounding',             
    'WindowBorderSize',           
    'WindowMinSize',              
    'WindowTitleAlign',           
    'WindowMenuButtonPosition',   
    'ChildRounding',              
    'ChildBorderSize',            
    'PopupRounding',              
    'PopupBorderSize',            
    'FramePadding',               
    'FrameRounding',              
    'FrameBorderSize',            
    'ItemSpacing',                
    'ItemInnerSpacing',           
    'TouchExtraPadding',          
    'IndentSpacing',              
    'ColumnsMinSpacing',          
    'ScrollbarSize',              
    'ScrollbarRounding',          
    'GrabMinSize',                
    'GrabRounding',               
    'TabRounding',                
    'TabBorderSize',              
    'ColorButtonPosition',        
    'ButtonTextAlign',            
    'SelectableTextAlign',        
    'DisplayWindowPadding',       
    'DisplaySafeAreaPadding',     
    'MouseCursorScale',           
    'AntiAliasedLines',           
    'AntiAliasedFill',            
    'CurveTessellationTol',       
    'Colors',
};

local _ImGuiCol =
{
    'Text',
    'TextDisabled',
    'WindowBg',             
    'ChildBg',              
    'PopupBg',              
    'Border',
    'BorderShadow',
    'FrameBg',              
    'FrameBgHovered',
    'FrameBgActive',
    'TitleBg',
    'TitleBgActive',
    'TitleBgCollapsed',
    'MenuBarBg',
    'ScrollbarBg',
    'ScrollbarGrab',
    'ScrollbarGrabHovered',
    'ScrollbarGrabActive',
    'CheckMark',
    'SliderGrab',
    'SliderGrabActive',
    'Button',
    'ButtonHovered',
    'ButtonActive',
    'Header',               
    'HeaderHovered',
    'HeaderActive',
    'Separator',
    'SeparatorHovered',
    'SeparatorActive',
    'ResizeGrip',
    'ResizeGripHovered',
    'ResizeGripActive',
    'Tab',
    'TabHovered',
    'TabActive',
    'TabUnfocused',
    'TabUnfocusedActive',
    'PlotLines',
    'PlotLinesHovered',
    'PlotHistogram',
    'PlotHistogramHovered',
    'TextSelectedBg',
    'DragDropTarget',
    'NavHighlight',         
    'NavWindowingHighlight',
    'NavWindowingDimBg',    
    'ModalWindowDimBg'      
};

local function split(str, delim, plain)
    local lines, pos, plain = {}, 1, not (plain == false) --[[ delimiter is plain text by default ]]
    repeat
      local npos, epos = string.find(str, delim, pos, plain)
      table.insert(lines, string.sub(str, pos, npos and npos - 1))
      pos = epos and epos + 1
    until not pos
    return lines
end

function module.getStyles()
    local tmp = {}
    for k in pairs(module.tstyle.styles_table) do 
        table.insert( tmp, k ) 
        module.tstyle.preparetoapply = true 
    end
    return module.tstyle.preparetoapply and tmp or {"No styles"}
end

function module.applyStyle(style, stylename)
    imgui.SwitchContext()
    if module.tstyle.preparetoapply and module.tstyle.styles_table[stylename] then
        for _,v in pairs(_ImGuiStyle) do
            if v == 'Colors' then
                for k, d in pairs(_ImGuiCol) do
                    style[v][k-1] = imgui.ColorConvertU32ToFloat4(tonumber(bit.tohex(module.tstyle.styles_table[stylename][d]), 16))
                end
                break
            end
            if v == 'Font' then
                break
            end
            if tostring(module.tstyle.styles_table[stylename][v]):find("(%d+) (%d+)") then
                local n = split(module.tstyle.styles_table[stylename][v], " ")
                style[v] = imgui.ImVec2(tonumber(n[1]), tonumber(n[2]))
            elseif tonumber(module.tstyle.styles_table[stylename][v]) then
                style[v] = tonumber(module.tstyle.styles_table[stylename][v])
            end
        end
        imgui.GetIO().FontDefault = fstyle.tstyle.fonts[module.tstyle.styles_table[stylename]["Font"]] or fstyle.tstyle.fonts["trebucbd.ttf"]
        fstyle.tstyle.current_font = module.tstyle.styles_table[stylename]["Font"]
        return true
    end
    return false
end

function module.loadStyles()
    module.tstyle.preparetoapply = false
    module.tstyle.styles_table = fcommon.LoadJson("styles")

    if module.tstyle.styles_table ~= nil then
        module.tstyle.preparetoapply = true
    end

    return module.tstyle.preparetoapply
end

function module.LoadFonts()
    local mask = tcheatmenu.dir .. "fonts//*.ttf"

    local handle, name = findFirstFile(mask)
    
    while handle and name do
        fstyle.tstyle.fonts[name] = imgui.GetIO().Fonts:AddFontFromFileTTF(string.format( "%sfonts//%s",tcheatmenu.dir,name), module.tstyle.font_size)
        name = findNextFile(handle)
    end
end

function StylerCheckbox(label,style)

    local var = imgui.new.bool((style > 0.0) and true or false)
    if imgui.Checkbox(label, var) then 
        style = var[0] and 1.0 or 0.0
    end

    return style
end

function StylerSliderFloat(label,style,min,max)

    local var = imgui.new.float(style)
    if imgui.SliderFloat(label, var, min, max, "%.0f") then
        style = var[0]
    end

    return style
end

function StylerSliderFloat2(label,style,min,max)

    local var = imgui.new.float[2](style.x,style.y)
    if imgui.SliderFloat2(label, var, min, max, "%.0f") then
        style = imgui.ImVec2(var[0],var[1])
    end

    return style
end

function StylerColorPicker(style,i)
    local name = imgui.GetStyleColorName(i)
    imgui.PushIDInt(i)

    local float = imgui.new.float[4](style.Colors[i].x,style.Colors[i].y,style.Colors[i].z,style.Colors[i].w)
    if imgui.ColorEdit4("##color", float,imgui.ColorEditFlags.AlphaBar) then
        style.Colors[i].x = float[0]
        style.Colors[i].y = float[1]
        style.Colors[i].z = float[2]
        style.Colors[i].w = float[3]
    end

    imgui.SameLine()
    imgui.TextUnformatted(name)
   
    if fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1] ~= nil
    and module.tstyle.styles_table[fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]][ffi.string(name)] ~= nil then
        local s = imgui.ColorConvertU32ToFloat4(tonumber(bit.tohex(module.tstyle.styles_table[fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]][ffi.string(name)]), 16))

        if float[0] ~= s.x or float[1] ~= s.y or float[2] ~= s.z or float[3] ~= s.w then
            imgui.SameLine(0.0, style.ItemInnerSpacing.x)
            if imgui.Button("Revert") then
                style.Colors[i].x = s.x
                style.Colors[i].y = s.y
                style.Colors[i].z = s.z
                style.Colors[i].w = s.w
            end
        end
    end
    imgui.PopID()
end
function module.StyleEditor()

    local style = imgui.GetStyle();

    fcommon.DropDownList("Select font",fstyle.tstyle.fonts,fstyle.tstyle.current_font,
    function(key,val)
        imgui.GetIO().FontDefault = val
        fstyle.tstyle.current_font = key
    end)

    imgui.Spacing()

    fcommon.Tabs("Style",{"Borders","Colors","Sizes"},{
        function()
            imgui.Columns(2,nil,false)

            style.ChildBorderSize = StylerCheckbox("Child border",style.ChildBorderSize)
            style.FrameBorderSize = StylerCheckbox("Frame border",style.FrameBorderSize)
        
            imgui.NextColumn()

            style.PopupBorderSize = StylerCheckbox("Popup border",style.PopupBorderSize)
            style.WindowBorderSize = StylerCheckbox("Window border",style.WindowBorderSize)

            imgui.Columns(1)
        end,
        function()

            imgui.BeginChild("##colors")
            imgui.PushItemWidth(-160)

            for i=0,35,1 do
                StylerColorPicker(style,i)
            end
            StylerColorPicker(style,42)
            imgui.PopItemWidth();
            imgui.EndChild();
        end,
        function()
            imgui.BeginChild("##sizes");
            imgui.PushItemWidth(imgui.GetWindowWidth() * 0.50);

            imgui.SliderInt("Font size", module.tstyle.font_size_var, 12, 48)
                
            style.GrabMinSize = StylerSliderFloat("Grab min size",style.GrabMinSize,0.0,20.0)
            style.IndentSpacing = StylerSliderFloat("Indent spacing",style.IndentSpacing,0.0,30.0)
            style.ItemInnerSpacing = StylerSliderFloat2("Item inner spacing",style.ItemInnerSpacing,0.0,20.0)
            style.ItemSpacing = StylerSliderFloat2("Item spacing",style.ItemSpacing,0.0,20.0)
            style.ScrollbarSize = StylerSliderFloat("Scrollbar size",style.ScrollbarSize,1.0,20.0)
            style.WindowPadding = StylerSliderFloat2("Window padding",style.WindowPadding,0.0,20.0)
            
            imgui.Dummy(imgui.ImVec2(0,10))

            style.ChildRounding = StylerSliderFloat("Child rounding",style.ChildRounding,0.0,12.0)
            style.FrameRounding = StylerSliderFloat("Frame rounding",style.FrameRounding,0.0,12.0)
            style.GrabRounding = StylerSliderFloat("Grab rounding",style.GrabRounding,0.0,12.0)
            style.PopupRounding = StylerSliderFloat("Popup rounding",style.PopupRounding,0.0,12.0)
            style.ScrollbarRounding = StylerSliderFloat("Scrollbar rounding",style.ScrollbarRounding,0.0,12.0)
            style.WindowRounding = StylerSliderFloat("Window rounding",style.WindowRounding,0.0,12.0)

            imgui.PopItemWidth()
            imgui.EndChild()
        end
    })
end



function module.saveStyles( style, stylename )
    module.tstyle.styles_table[stylename] = {}
    for _, v in pairs(_ImGuiStyle) do
        if v == 'Colors' then
            for k, d in pairs(_ImGuiCol) do
                module.tstyle.styles_table[stylename][d] = "0x"..bit.tohex(imgui.ColorConvertFloat4ToU32(style[v][k-1]))
            end
            break
        end
        module.tstyle.styles_table[stylename][v] = type(style[v]) == 'cdata' and (style[v].x.." "..style[v].y) or style[v]
    end
    module.tstyle.styles_table[stylename]["Font"] = (memory.tostring(imgui.GetFont():GetDebugName()):sub(1,-7))
    
    return fcommon.SaveJson("styles",module.tstyle.styles_table) and true or false
end

return module
