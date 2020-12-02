-- This file is part of mimgui project
-- Licensed under the MIT License
-- Copyright (c) 2018, FYP <https://github.com/THE-FYP>

assert(getMoonloaderVersion() >= 025)

local imgui = require 'mimgui.imgui'
local DX9 = require 'mimgui.dx9'
local ffi = require 'ffi'
local winmsg = require 'windows.message'
local memory = require 'memory'
local mimgui = {}

local renderer = nil
local subscriptionsInitialize = {}
local subscriptionsNewFrame = {}
local eventsRegistered = false
local active = false
local cursorActive = false
local playerLocked = false
local iniFilePath = nil
local defaultGlyphRanges = nil

setmetatable(mimgui, {__index = imgui, __newindex = function(t, k, v)
    if imgui[k] then
        print('[mimgui] Warning! Overwriting existing key "'..k..'"!')
    end
    rawset(t, k, v)
end})

-- background "Shift" triggering fix
memory.fill(0x00531155, 0x90, 5, true)

local function ShowCursor(show)
    if show then
        showCursor(true)
    elseif cursorActive then
        showCursor(false)
    end
    cursorActive = show
end

local function LockPlayer(lock)
    if lock then
        lockPlayerControl(true)
    elseif playerLocked then
        lockPlayerControl(false)
    end
    playerLocked = lock
end

-- MoonLoader v.027
if not isCursorActive then
    isCursorActive = function() return cursorActive end
end

local function InitializeRenderer()
    -- init renderer
    local hwnd = ffi.cast('HWND', readMemory(0x00C8CF88, 4, false))
    local d3ddevice = ffi.cast('LPDIRECT3DDEVICE9', getD3DDevicePtr())
    renderer = assert(DX9.new(d3ddevice, hwnd))
    renderer:SwitchContext()
    
    -- configure imgui
    imgui.GetIO().ImeWindowHandle = nil -- default causes crash. TODO: why?
    imgui.GetIO().LogFilename = nil
    local confdir = getWorkingDirectory() .. [[\config\mimgui\]]
    if not doesDirectoryExist(confdir) then
        createDirectory(confdir)
    end
    iniFilePath = ffi.new('char[260]', confdir .. script.this.filename .. '.ini')
    imgui.GetIO().IniFilename = iniFilePath
    
    -- change font
    local fontFile = getFolderPath(0x14) .. '\\trebucbd.ttf'
    assert(doesFileExist(fontFile), '[mimgui] Font "' .. fontFile .. '" doesn\'t exist!')
    local builder = imgui.ImFontGlyphRangesBuilder()
    builder:AddRanges(imgui.GetIO().Fonts:GetGlyphRangesCyrillic())
    builder:AddText([[‚„…†‡€‰‹‘’“”•–—™›№]])
    defaultGlyphRanges = imgui.ImVector_ImWchar()
    builder:BuildRanges(defaultGlyphRanges)
    imgui.GetIO().Fonts:AddFontFromFileTTF(fontFile, 14, nil, defaultGlyphRanges[0].Data)

    -- invoke initializers
    for _, cb in ipairs(subscriptionsInitialize) do
        cb()
    end
end

local function RegisterEvents()
    addEventHandler('onD3DPresent', function()
        if active then
            if not renderer then
                InitializeRenderer()
            end
            if renderer and not renderer.lost then
                renderer:SwitchContext()
                for _, sub in ipairs(subscriptionsNewFrame) do
                    if sub._render and sub._before then
                        sub:_before()
                    end
                end
                renderer:NewFrame()
                local hideCursor = true
                for _, sub in ipairs(subscriptionsNewFrame) do
                    if sub._render then
                        sub:_draw()
                        hideCursor = hideCursor and sub.HideCursor
                    end
                end
                if hideCursor and not isCursorActive() then
                    imgui.SetMouseCursor(imgui.lib.ImGuiMouseCursor_None)
                end
                renderer:EndFrame()
            end
        end
    end)
    
    local keyState = {}
    local WM_MOUSEHWHEEL = 0x020E
    local mouseMsgs = {
        [WM_MOUSEHWHEEL]=true,
        [winmsg.WM_LBUTTONDOWN]=true,
        [winmsg.WM_LBUTTONDBLCLK]=true,
        [winmsg.WM_RBUTTONDOWN]=true,
        [winmsg.WM_RBUTTONDBLCLK]=true,
        [winmsg.WM_MBUTTONDOWN]=true,
        [winmsg.WM_MBUTTONDBLCLK]=true,
        [winmsg.WM_LBUTTONUP]=true,
        [winmsg.WM_RBUTTONUP]=true,
        [winmsg.WM_MBUTTONUP]=true,
        [winmsg.WM_MOUSEWHEEL]=true,
        [winmsg.WM_SETCURSOR]=true
    }
    local keyboardMsgs = {
        [winmsg.WM_KEYDOWN]=true,
        [winmsg.WM_SYSKEYDOWN]=true,
        [winmsg.WM_KEYUP]=true,
        [winmsg.WM_SYSKEYUP]=true,
        [winmsg.WM_CHAR]=true
    }
    addEventHandler('onWindowMessage', function(msg, wparam, lparam)
        if not renderer then
            return
        end
        
        if not mimgui.DisableInput then
            local keyboard = keyboardMsgs[msg]
            local mouse = mouseMsgs[msg]
            if active and (keyboard or mouse) then
                renderer:SwitchContext()
                local io = imgui.GetIO()
                renderer:WindowMessage(msg, wparam, lparam)
                if (keyboard and io.WantCaptureKeyboard) or (mouse and io.WantCaptureMouse) then
                    if msg == winmsg.WM_KEYDOWN or msg == winmsg.WM_SYSKEYDOWN then
                        keyState[wparam] = false
                        consumeWindowMessage(true, true, true)
                    elseif msg == winmsg.WM_KEYUP or msg == winmsg.WM_SYSKEYUP then
                        if not keyState[wparam] then
                            consumeWindowMessage(true, true, true)
                        end
                    else
                        consumeWindowMessage(true, true, true)
                    end
                end
            end
        end

        -- save key states to prevent key sticking
        if msg == winmsg.WM_KILLFOCUS then
            keyState = {}
        elseif wparam < 256 then
            if msg == winmsg.WM_KEYDOWN or msg == winmsg.WM_SYSKEYDOWN then
                keyState[wparam] = true
            elseif msg == winmsg.WM_KEYUP or msg == winmsg.WM_SYSKEYUP then
                keyState[wparam] = false
            end
        end
    end)
    
    addEventHandler('onD3DDeviceLost', function()
        if renderer and not renderer.lost then
            renderer:InvalidateDeviceObjects()
            renderer.lost = true
        end
    end)
    
    addEventHandler('onD3DDeviceReset', function()
        if renderer then
            renderer.lost = false
        end
    end)

    addEventHandler('onScriptTerminate', function(scr)
        if scr == script.this then
            ShowCursor(false)
            LockPlayer(false)
        end
    end)
    
    local updaterThread = lua_thread.create(function()
        while true do
            wait(0)
            local activate, hideCursor, lockPlayer = false, true, false
            if #subscriptionsNewFrame > 0 then
                for i, sub in ipairs(subscriptionsNewFrame) do
                    if type(sub.Condition) == 'function' then
                        sub._render = sub.Condition()
                    else
                        sub._render = sub.Condition and true
                    end
                    if sub._render then
                        hideCursor = hideCursor and sub.HideCursor
                        lockPlayer = lockPlayer or sub.LockPlayer
                    end
                    activate = activate or sub._render
                end
            end
            active = activate
            ShowCursor(active and not hideCursor)
            LockPlayer(active and lockPlayer)
        end
    end)
    updaterThread.work_in_pause = true
end

local function Unsubscribe(t, sub)
    for i, v in ipairs(t) do
        if v == sub then
            table.remove(t, i)
            return
        end
    end
end

local function ImGuiEnum(name)
    return setmetatable({__name = name}, {__index = function(t, k)
        return imgui.lib[t.__name .. k]
    end})
end

--- API ---
mimgui._VERSION = '1.7.0'
mimgui.DisableInput = false

mimgui.ComboFlags = ImGuiEnum('ImGuiComboFlags_')
mimgui.Dir = ImGuiEnum('ImGuiDir_')
mimgui.ColorEditFlags = ImGuiEnum('ImGuiColorEditFlags_')
mimgui.Col = ImGuiEnum('ImGuiCol_')
mimgui.WindowFlags = ImGuiEnum('ImGuiWindowFlags_')
mimgui.NavInput = ImGuiEnum('ImGuiNavInput_')
mimgui.FocusedFlags = ImGuiEnum('ImGuiFocusedFlags_')
mimgui.Cond = ImGuiEnum('ImGuiCond_')
mimgui.BackendFlags = ImGuiEnum('ImGuiBackendFlags_')
mimgui.TreeNodeFlags = ImGuiEnum('ImGuiTreeNodeFlags_')
mimgui.StyleVar = ImGuiEnum('ImGuiStyleVar_')
mimgui.DrawCornerFlags = ImGuiEnum('ImDrawCornerFlags_')
mimgui.DragDropFlags = ImGuiEnum('ImGuiDragDropFlags_')
mimgui.SelectableFlags = ImGuiEnum('ImGuiSelectableFlags_')
mimgui.InputTextFlags = ImGuiEnum('ImGuiInputTextFlags_')
mimgui.MouseCursor = ImGuiEnum('ImGuiMouseCursor_')
mimgui.FontAtlasFlags = ImGuiEnum('ImFontAtlasFlags_')
mimgui.HoveredFlags = ImGuiEnum('ImGuiHoveredFlags_')
mimgui.ConfigFlags = ImGuiEnum('ImGuiConfigFlags_')
mimgui.DrawListFlags = ImGuiEnum('ImDrawListFlags_')
mimgui.DataType = ImGuiEnum('ImGuiDataType_')
mimgui.Key = ImGuiEnum('ImGuiKey_')

function mimgui.OnInitialize(cb)
    assert(type(cb) == 'function')
    table.insert(subscriptionsInitialize, cb)
    return {Unsubscribe = function() Unsubscribe(subscriptionsInitialize, cb) end}
end

function mimgui.OnFrame(cond, cbBeforeFrame, cbDraw)
    assert(type(cond) == 'function')
    assert(type(cbBeforeFrame) == 'function')
    if cbDraw then assert(type(cbDraw) == 'function') end
    if not eventsRegistered then
        RegisterEvents()
        eventsRegistered = true
    end
    local sub = {
        Condition = cond,
        LockPlayer = false,
        HideCursor = false,
        _before = cbDraw and cbBeforeFrame or nil,
        _draw = cbDraw or cbBeforeFrame,
        _render = false,
    }
    function sub:Unsubscribe()
        Unsubscribe(subscriptionsNewFrame, self)
    end
    function sub:IsActive()
        return self._render
    end
    table.insert(subscriptionsNewFrame, sub)
    return sub
end

function mimgui.SwitchContext()
    return renderer:SwitchContext()
end

function mimgui.CreateTextureFromFile(path)
    return renderer:CreateTextureFromFile(path)
end

function mimgui.CreateTextureFromFileInMemory(src, size)
    return renderer:CreateTextureFromFileInMemory(src, size)
end

function mimgui.ReleaseTexture(tex)
    return renderer:ReleaseTexture(tex)
end

function mimgui.CreateFontsTexture()
    return renderer:CreateFontsTexture()
end

function mimgui.InvalidateFontsTexture()
    return renderer:InvalidateFontsTexture()
end

function mimgui.GetRenderer()
    return renderer
end

function mimgui.IsInitialized()
    return renderer ~= nil
end

function mimgui.StrCopy(dst, src, len)
    if len or tostring(ffi.typeof(dst)):find('*', 1, true) then
        ffi.copy(dst, src, len)
    else
        len = math.min(ffi.sizeof(dst) - 1, #src)
        ffi.copy(dst, src, len)
        dst[len] = 0
    end
end

local new = {}
setmetatable(new, {
    __index = function(self, key)
        local basetype = ffi.typeof(key)
        local mt = {
            __index = function(self, sz)
                return setmetatable({type = ffi.typeof('$[$]', self.type, sz)}, getmetatable(self))
            end,
            __call = function(self, ...)
                return self.type(...)
            end
        }
        return setmetatable({type = ffi.typeof('$[1]', basetype), basetype = basetype}, {
            __index = function(self, sz)
                return setmetatable({type = ffi.typeof('$[$]', self.basetype, sz)}, mt)
            end,
            __call = mt.__call
        })
    end,
    __call = function(self, t, ...)
        return ffi.new(t, ...)
    end
})
mimgui.new = new

return mimgui
