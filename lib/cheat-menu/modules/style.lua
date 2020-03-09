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
    alpha_flags    = imgui.new.int(0),
    fonts          = {},
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
    'Alpha',                      -- Global alpha applies to everything in Dear ImGui.
    'WindowPadding',              -- Padding within a window.
    'WindowRounding',             -- Radius of window corners rounding. Set to 0.0f to have rectangular windows.
    'WindowBorderSize',           -- Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    'WindowMinSize',              -- Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
    'WindowTitleAlign',           -- Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
    'WindowMenuButtonPosition',   -- Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
    'ChildRounding',              -- Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
    'ChildBorderSize',            -- Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    'PopupRounding',              -- Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
    'PopupBorderSize',            -- Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    'FramePadding',               -- Padding within a framed rectangle (used by most widgets).
    'FrameRounding',              -- Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    'FrameBorderSize',            -- Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    'ItemSpacing',                -- Horizontal and vertical spacing between widgets/lines.
    'ItemInnerSpacing',           -- Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    'TouchExtraPadding',          -- Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    'IndentSpacing',              -- Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    'ColumnsMinSpacing',          -- Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
    'ScrollbarSize',              -- Width of the vertical scrollbar, Height of the horizontal scrollbar.
    'ScrollbarRounding',          -- Radius of grab corners for scrollbar.
    'GrabMinSize',                -- Minimum width/height of a grab box for slider/scrollbar.
    'GrabRounding',               -- Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    'TabRounding',                -- Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    'TabBorderSize',              -- Thickness of border around tabs.
    'ColorButtonPosition',        -- Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
    'ButtonTextAlign',            -- Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
    'SelectableTextAlign',        -- Alignment of selectable text when selectable is larger than text. Defaults to (0.0f, 0.0f) (top-left aligned).
    'DisplayWindowPadding',       -- Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
    'DisplaySafeAreaPadding',     -- If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
    'MouseCursorScale',           -- Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    'AntiAliasedLines',           -- Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
    'AntiAliasedFill',            -- Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    'CurveTessellationTol',       -- Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    'Colors',
};

local _ImGuiCol =
{
    'Text',
    'TextDisabled',
    'WindowBg',              -- Background of normal windows
    'ChildBg',               -- Background of child windows
    'PopupBg',               -- Background of popups, menus, tooltips windows
    'Border',
    'BorderShadow',
    'FrameBg',               -- Background of checkbox, radio button, plot, slider, text input
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
    'Header',                -- Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
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
    'NavHighlight',          -- Gamepad/keyboard: current highlighted item
    'NavWindowingHighlight', -- Highlight window when using CTRL+TAB
    'NavWindowingDimBg',     -- Darken/colorize entire screen behind the CTRL+TAB window list, when active
    'ModalWindowDimBg'      -- Darken/colorize entire screen behind a modal window, when one is active
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

        imgui.GetIO().FontDefault = fstyle.tstyle.fonts[module.tstyle.styles_table[stylename]["Font"]]

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
        fstyle.tstyle.fonts[name] = imgui.GetIO().Fonts:AddFontFromFileTTF(string.format( "%sfonts//%s",tcheatmenu.dir,name), 14)
        name = findNextFile(handle)
    end
end

function module.StyleEditor()

    local style = imgui.GetStyle();
    local var = nil

    imgui.ShowFontSelector("Fonts##Selector")
    imgui.Spacing()

    fcommon.Tabs("Style",{"Borders","Colors","Sizes"},{
        function()
            imgui.Columns(2,nil,false)
            var = imgui.new.bool((style.ChildBorderSize > 0.0) or false)
            if imgui.Checkbox("Child border", var) then 
                style.ChildBorderSize = var[0] and 1.0 or 0.0
            end
        
            var = imgui.new.bool((style.FrameBorderSize > 0.0) or false)
            if imgui.Checkbox("Frame border", var) then 
                style.FrameBorderSize = var[0] and 1.0 or 0.0
            end
        
            imgui.NextColumn()

            var = imgui.new.bool((style.PopupBorderSize > 0.0) or false)
            if imgui.Checkbox("Popup border", var) then 
                style.PopupBorderSize = var[0] and 1.0 or 0.0
            end
        
            -- var = imgui.new.bool((style.TabBorderSize > 0.0) or false)
            -- if imgui.Checkbox("Tab border", var) then 
            --     style.TabBorderSize = var[0] and 1.0 or 0.0
            -- end
        
            var = imgui.new.bool((style.WindowBorderSize > 0.0) or false)
            if imgui.Checkbox("Window border", var) then
                style.WindowBorderSize = var[0] and 1.0 or 0.0
            end

            imgui.Columns(1)
        end,
        function()
            local int output_dest = 0;
            local bool output_only_modified = true;

            if imgui.RadioButtonIntPtr("Opaque", module.tstyle.alpha_flags,0) then                                    
                module.tstyle.alpha_flags[0] = 0
            end

            imgui.SameLine()

            if imgui.RadioButtonIntPtr("Alpha",  module.tstyle.alpha_flags,imgui.ColorEditFlags.AlphaPreview) then
                module.tstyle.alpha_flags[0] = imgui.ColorEditFlags.AlphaPreview
            end
            
            imgui.SameLine()

            if imgui.RadioButtonIntPtr("Both",  module.tstyle.alpha_flags,imgui.ColorEditFlags.AlphaPreviewHalf) then  
                module.tstyle.alpha_flags[0] = imgui.ColorEditFlags.AlphaPreviewHalf
            end

            fcommon.InformationTooltip("Left-click on colored square to open color picker\nRight-click to open edit options menu.")

            imgui.BeginChild("##colors")
            imgui.PushItemWidth(-160)
 
            for i=0,imgui.Col.COUNT-1,1 do
                local name = imgui.GetStyleColorName(i)
                imgui.PushIDInt(i)

                local float = imgui.new.float[4](style.Colors[i].x,style.Colors[i].y,style.Colors[i].z,style.Colors[i].w)
                if imgui.ColorEdit4("##color", float,module.tstyle.alpha_flags[0]) then
                    style.Colors[i].x = float[0]
                    style.Colors[i].y = float[1]
                    style.Colors[i].z = float[2]
                    style.Colors[i].w = float[3]
                end
               
                if module.tstyle.styles_table[fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]][ffi.string(name)] ~= nil then
                    local s = imgui.ColorConvertU32ToFloat4(tonumber(bit.tohex(module.tstyle.styles_table[fstyle.tstyle.list[fstyle.tstyle.selected[0] + 1]][ffi.string(name)]), 16))
                    
                    imgui.SameLine(0.0, style.ItemInnerSpacing.x)
                    imgui.TextUnformatted(name)

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
            imgui.PopItemWidth();
            imgui.EndChild();
        end,
        function()
            imgui.BeginChild("##sizes");
            imgui.PushItemWidth(imgui.GetWindowWidth() * 0.50);

            imgui.Spacing()

            local string = {"Left","Right"}
            local list   = imgui.new['const char*'][#string](string)
            var = imgui.new.int(style.ColorButtonPosition)

            if imgui.Combo("Color button position", var, list,#string) then
                style.ColorButtonPosition = var[0]
            end

            string = {"None","Left","Right"}
            list   = imgui.new['const char*'][#string](string)
            var = imgui.new.int(style.WindowMenuButtonPosition + 1)

            if imgui.Combo("Window menu button position", var, list,#string) then
                style.WindowMenuButtonPosition = var[0] - 1
            end

            imgui.Dummy(imgui.ImVec2(0,10))

            var = imgui.new.float[2](style.DisplaySafeAreaPadding.x,style.DisplaySafeAreaPadding.y)
            if imgui.SliderFloat2("Display safe area padding", var, 0.0, 30.0, "%.0f") then
                style.DisplaySafeAreaPadding = imgui.ImVec2(var[0],var[1])
            end
            fcommon.InformationTooltip("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).")

            var = imgui.new.float[2](style.FramePadding.x,style.FramePadding.y)
            if imgui.SliderFloat2("Frame padding", var, 0.0, 20.0, "%.0f") then
                style.FramePadding = imgui.ImVec2(var[0],var[1])
            end

            var = imgui.new.float(style.GrabMinSize)
            if imgui.SliderFloat("Grab min size", var, 1.0, 20.0, "%.0f") then
                style.GrabMinSize = var[0]
            end

            var = imgui.new.float(style.IndentSpacing)
            if imgui.SliderFloat("Indent spacing", var, 0.0, 30.0, "%.0f") then
                style.IndentSpacing = var[0]
            end

            var = imgui.new.float[2](style.ItemInnerSpacing.x,style.ItemInnerSpacing.y)
            if imgui.SliderFloat2("Item inner spacing", var, 0.0, 20.0, "%.0f") then
                style.ItemInnerSpacing = imgui.ImVec2(var[0],var[1])
            end

            var = imgui.new.float[2](style.ItemSpacing.x,style.ItemSpacing.y)
            if imgui.SliderFloat2("Item spacing", var, 0.0, 20.0, "%.0f") then
                style.ItemSpacing = imgui.ImVec2(var[0],var[1])
            end

            var = imgui.new.float(style.ScrollbarSize)
            if imgui.SliderFloat("Scrollbar size", var, 1.0, 20.0, "%.0f") then
                style.ScrollbarSize = var[0]
            end

            var = imgui.new.float[2](style.TouchExtraPadding.x,style.TouchExtraPadding.y)
            if imgui.SliderFloat2("Touch extra padding", var, 0.0, 10.0, "%.0f") then
                style.TouchExtraPadding = imgui.ImVec2(var[0],var[1])
            end

            var = imgui.new.float[2](style.WindowPadding.x,style.WindowPadding.y)
            if imgui.SliderFloat2("Window padding", var, 0.0, 20.0, "%.0f") then
                style.WindowPadding = imgui.ImVec2(var[0],var[1])
            end
            
            imgui.Dummy(imgui.ImVec2(0,10))

            var = imgui.new.float(style.ChildRounding)
            if imgui.SliderFloat("Child rounding", var, 0.0, 12.0, "%.0f") then
                style.ChildRounding = var[0]
            end

            var = imgui.new.float(style.FrameRounding)
            if imgui.SliderFloat("Frame rounding", var, 0.0, 12.0, "%.0f") then
                style.FrameRounding = var[0]
            end

            var = imgui.new.float(style.GrabRounding)
            if imgui.SliderFloat("Grab rounding", var, 0.0, 12.0, "%.0f") then
                style.GrabRounding = var[0]
            end

            var = imgui.new.float(style.PopupRounding)
            if imgui.SliderFloat("Popup rounding", var, 0.0, 12.0, "%.0f") then
                style.PopupRounding = var[0]
            end

            var = imgui.new.float(style.ScrollbarRounding)
            if imgui.SliderFloat("Scrollbar rounding", var, 0.0, 12.0, "%.0f") then
                style.ScrollbarRounding = var[0]
            end

            -- var = imgui.new.float(style.TabRounding)
            -- if imgui.SliderFloat("Tab rounding", var, 0.0, 12.0, "%.0f") then
            --     style.TabRounding = var[0]
            -- end

            var = imgui.new.float(style.WindowRounding)
            if imgui.SliderFloat("Window rounding", var, 0.0, 12.0, "%.0f") then
                style.WindowRounding = var[0]
            end

            imgui.Dummy(imgui.ImVec2(0,10))

            var = imgui.new.float[2](style.ButtonTextAlign.x,style.ButtonTextAlign.y)
            if imgui.SliderFloat2("Button text align", var, 0.0, 1.0, "%.2f") then
                style.ButtonTextAlign = imgui.ImVec2(var[0],var[1])
            end

            var = imgui.new.float[2](style.SelectableTextAlign.x,style.SelectableTextAlign.y)
            if imgui.SliderFloat2("Selectable text align", var, 0.0, 1.0, "%.2f") then
                style.SelectableTextAlign = imgui.ImVec2(var[0],var[1])
            end

            var = imgui.new.float[2](style.WindowTitleAlign.x,style.WindowTitleAlign.y)
            if imgui.SliderFloat2("Window title align", var, 0.0, 1.0, "%.2f") then
                style.WindowTitleAlign = imgui.ImVec2(var[0],var[1])
            end

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
    module.tstyle.styles_table[stylename]["Font"] = memory.tostring(imgui.GetFont():GetDebugName()):sub(1,-7)
    
    return fcommon.SaveJson("styles",module.tstyle.styles_table) and true or false
end

return module
