  
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

module.tanimation =
{
    fighting      =
    {
        selected  = imgui.new.int(fconfig.Get('tanimation.fighting.selected',0)),
        names     = {"Default","Boxing","Kung fu","Kick Boxing","Punch Kick"},
        array     = {},
    }, 
    filter = imgui.ImGuiTextFilter(),
    ifp_name      = imgui.new.char[20](),
    list          = fcommon.LoadJson("animation"),
    loop          = imgui.new.bool(fconfig.Get('tanimation.loop',false)),
    name          = imgui.new.char[20](),
    ped           = imgui.new.bool(fconfig.Get('tanimation.ped',false)),
    secondary     = imgui.new.bool(fconfig.Get('tanimation.secondary',false)),
    walking       = 
    { 
        selected  = imgui.new.int(fconfig.Get('tanimation.walking.selected',0)),
        names     = {"man","shuffle","oldman","gang1","gang2","oldfatman","fatman","jogger","drunkman","blindman","swat","woman","shopping","busywoman","sexywoman","pro","oldwoman","fatwoman","jogwoman","oldfatwoman","skate"},
        array     = {},
    },
}

module.tanimation.fighting.array = imgui.new['const char*'][#module.tanimation.fighting.names](module.tanimation.fighting.names)
module.tanimation.walking.array  = imgui.new['const char*'][#module.tanimation.walking.names](module.tanimation.walking.names)


function AnimationEntry(file,animation)
    if imgui.MenuItemBool(animation)then
        local char = nil
        if file ~= "PED" then   -- don't request if animation is from ped.ifp
            requestAnimation(file)
            loadAllModelsNow()
        end
        PlayAnimation(file,animation)
    end
    if imgui.IsItemClicked(1) then
		module.tanimation.list[file .. "$" .. animation] = nil
		fcommon.SaveJson("animation",module.tanimation.list)
		module.tanimation.list = fcommon.LoadJson("animation")
		printHelpString("Animation ~r~removed")
	end
end

function PlayAnimation(file,animation)
    if module.tanimation.ped[0] == true then
        if fped.tped.selected ~=  nil then
            char = fped.tped.selected
        else
            printHelpString("~r~No~w~ ped selected")
            return
        end
    else
        char = PLAYER_PED
    end

    if module.tanimation.secondary[0] == true then
        taskPlayAnimSecondary(char,animation,file,4.0,module.tanimation.loop[0],0,0,0,-1)  
    else
        taskPlayAnim(char,animation,file,4.0,module.tanimation.loop[0],0,0,0,-1)
    end
    fcommon.CheatActivated()
    if file ~= "PED" then   -- don't remove if animation is from ped.ifp
        removeAnimation(animation)  
    end
end

-- Main function
function module.AnimationMain()
    imgui.Spacing()
    if imgui.Button("Stop animation",imgui.ImVec2(fcommon.GetSize(1))) then
        local char = nil
        if module.tanimation.ped[0] == true then
            if fped.tped.selected ~=  nil then
                char = fped.tped.selected
            else
                printHelpString("~r~No~w~ ped selected")
                return
            end
        else
            char = PLAYER_PED
        end
        clearCharTasks(char)
        fcommon.CheatActivated()
    end
    imgui.Spacing()

    -- Checkboxes
    imgui.Columns(2,nil,false)
    fcommon.CheckBoxVar("Loop",module.tanimation.loop)
    fcommon.CheckBoxVar("Ped ##Animation",module.tanimation.ped,"Play animation on ped.Aim with a gun to select.")
    imgui.NextColumn()
    fcommon.CheckBoxVar("Secondary",module.tanimation.secondary)
    imgui.Columns(1)

    imgui.Spacing() 
    fcommon.Tabs("Animation",{"List","Search","Misc","Custom"},{
        function()
            local menus_shown = {}
            for key,value in fcommon.spairs(module.tanimation.list) do
                local temp,_ = key:match("([^$]+)$([^$]+)")
                
                local show_menu = true
                for i=1,#menus_shown,1 do
                    if menus_shown[i] == temp then
                        show_menu = false
                    end
                end

                if show_menu then
                    fcommon.DropDownMenu(temp,function()
                        for key,value in fcommon.spairs(module.tanimation.list) do
                            local file,animation = key:match("([^$]+)$([^$]+)")
                            if temp == file then
                                AnimationEntry(file,animation)
                            end
                        end
                    end)
                    table.insert(menus_shown,temp)
                end
            end   
        end,
        function()
            module.tanimation.filter:Draw("Filter")
            imgui.Spacing()

            if imgui.BeginChild("Stat Entries") then
                for key,value in pairs(module.tanimation.list) do
                    file, animation = key:match("([^$]+)$([^$]+)")
                    if module.tanimation.filter:PassFilter(animation) then
                        AnimationEntry(file,animation)
                    end
                end       
                imgui.Spacing()
                imgui.EndChild()
            end
        end,
        function()
            if imgui.Combo("Fighting", module.tanimation.fighting.selected,module.tanimation.fighting.array,#module.tanimation.fighting.names) then
                giveMeleeAttackToChar(PLAYER_PED,module.tanimation.fighting.selected[0]+4,6)
                fcommon.CheatActivated()
            end
            if imgui.Combo("Walking", module.tanimation.walking.selected,module.tanimation.walking.array,#module.tanimation.walking.names) then
                writeMemory(0x609A4E,4,-1869574000,true)
                writeMemory(0x609A52,2,37008,true)
                requestAnimation(module.tanimation.walking.names[module.tanimation.walking.selected[0]+1])
                loadAllModelsNow()
                setAnimGroupForChar(PLAYER_PED,module.tanimation.walking.names[module.tanimation.walking.selected[0]+1])
                removeAnimation(module.tanimation.walking.names[module.tanimation.walking.selected[0]+1])
                fcommon.CheatActivated()
            end
        end,
        function()
            if imgui.InputText("IFP name",module.tanimation.ifp_name,ffi.sizeof(module.tanimation.ifp_name)) then end
            if imgui.InputText("Animation name",module.tanimation.name,ffi.sizeof(module.tanimation.name)) then end
            imgui.Spacing()
            if imgui.Button("Add animation",imgui.ImVec2(fcommon.GetSize(1))) then
                module.tanimation.list[ffi.string(module.tanimation.ifp_name) .. "$" .. ffi.string(module.tanimation.name)] = "Animation"
                fcommon.SaveJson("animation",module.tanimation.list)
                module.tanimation.list = fcommon.LoadJson("animation")
                printHelpString("Animation ~g~added")
            end
        end
    })
end

return module