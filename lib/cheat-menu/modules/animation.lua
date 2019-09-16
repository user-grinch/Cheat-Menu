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

local tanimation =
{
    loop             = imgui.new.bool(fconfig.get('tanimation.loop',false)),
    secondary        = imgui.new.bool(fconfig.get('tanimation.secondary',false)),
    search_text      = imgui.new.char[20](),
    name             = imgui.new.char[20](),
    ifp_name         = imgui.new.char[20](),
    fighting =
    {
        selected = imgui.new.int(fconfig.get('tanimation.fighting.selected',0)),
        names    = {"Default","Boxing","Kung fu","Kick Boxing","Punch Kick"},
        list     = {},
    },
    walking =
    {
        selected = imgui.new.int(fconfig.get('tanimation.walking.selected',0)),
        names    = {"man","shuffle","oldman","gang1","gang2","oldfatman","fatman","jogger","drunkman","blindman","swat","woman","shopping","busywoman","sexywoman","pro","oldwoman","fatwoman","jogwoman","oldfatwoman","skate"},
        list     = {},
    },
}
module.tanimation = tanimation

tanimation.fighting.list = imgui.new['const char*'][#tanimation.fighting.names](tanimation.fighting.names)
tanimation.walking.list  = imgui.new['const char*'][#tanimation.walking.names](tanimation.walking.names)


function AnimationEntry(file,animation)
    if imgui.MenuItemBool(animation)then
        requestAnimation(file)
        loadAllModelsNow()
        
        if tplayer.animation.secondary[0] == true then
            taskPlayAnimSecondary(PLAYER_PED,animation,file,4.0,tplayer.animation.loop[0],0,0,0,-1)
        else
            taskPlayAnim(PLAYER_PED,animation,file,4.0,tplayer.animation.loop[0],0,0,0,-1)
        end
        removeAnimation(animation)
    end
end

function module.AnimationMain()
    imgui.Spacing()
    if imgui.Button("Stop animation",imgui.ImVec2(fcommon.GetSize(1))) then
        clearCharTasks(PLAYER_PED)
        fcommon.CheatActivated()
    end
    imgui.Spacing()
    imgui.Columns(2,nil,false)
    fcommon.CheckBox({name = "Loop",var = tanimation.loop})
    imgui.NextColumn()
    fcommon.CheckBox({name = "Secondary",var = tanimation.secondary})
    imgui.Columns(1)
    imgui.Spacing() 
    if imgui.BeginTabBar("Animation") then
        imgui.Spacing() 

        imgui.Spacing()
        if imgui.BeginTabItem("List") then
            if imgui.BeginChild("Stat Entries") then

                local menu_name = ""
                for key,value in ipairs(ftable.animation.table) do
                    local temp,_ = value:match("([^$]+)$([^$]+)")
                    if menu_name ~= temp then
                        menu_name = temp
                        fcommon.DropDownMenu(menu_name,function()
                            for key,value in pairs(ftable.animation.table) do
                                local file,animation = value:match("([^$]+)$([^$]+)")
                                if menu_name == file then
                                    AnimationEntry(file,animation)
                                end
                            end
                        end)
                    end
                end   
                                
                imgui.EndChild()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Search") then

            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText("Search",tanimation.search_text,ffi.sizeof(tanimation.search_text)) then end
            imgui.SameLine()

            imgui.Spacing()
            imgui.Text("Found entries :(" .. ffi.string(tanimation.search_text) .. ")")
            imgui.Separator()
            imgui.Spacing()
            if imgui.BeginChild("Stat Entries") then
                for key,value in pairs(ftable.animation.table) do
                    file, animation = value:match("([^$]+)$([^$]+)")
                    if (string.upper(animation):find(string.upper(ffi.string(tanimation.search_text)))) then
                        AnimationEntry(file,animation)
                    end
                end       
                imgui.Spacing()
                imgui.EndChild()
            end
            
            imgui.EndTabItem()
        end


        if imgui.BeginTabItem("Misc") then
            if imgui.Combo("Fighting animation", tanimation.fighting.selected,tanimation.fighting.list,#tanimation.fighting.names) then
                giveMeleeAttackToChar(PLAYER_PED,tanimation.fighting.selected[0]+4,6)
                fcommon.CheatActivated()
            end
            if imgui.Combo("Walking animation", tanimation.walking.selected,tanimation.walking.list,#tanimation.walking.names) then
                writeMemory(0x609A4E,4,-1869574000,true)
                writeMemory(0x609A52,2,37008,true)
                requestAnimation(tanimation.walking.names[tanimation.walking.selected[0]+1])
                loadAllModelsNow()
                setAnimGroupForChar(PLAYER_PED,tanimation.walking.names[tanimation.walking.selected[0]+1])
                removeAnimation(tanimation.walking.names[tanimationwalking.selected[0]+1])
                fcommon.CheatActivated()
            end
            imgui.EndTabItem()
        end

        if imgui.BeginTabItem("Custom") then
            if imgui.InputText("IFP name",tanimation.ifp_name,ffi.sizeof(tanimation.ifp_name)) then end
            if imgui.InputText("Animation name",tanimation.name,ffi.sizeof(tanimation.name)) then end
            imgui.Spacing()
            if imgui.Button("Play animation",imgui.ImVec2(fcommon.GetSize(1))) then
                PlayAnimation(ffi.string(tanimation.ifp_name),ffi.string(tanimation.name))
            end
            imgui.EndTabItem()
        end
            
        imgui.EndTabBar()
    end
end

return module
