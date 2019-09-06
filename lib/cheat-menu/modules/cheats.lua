-- This module contains all the game original cheats

local module = {}

function CheatsEntry(func,names)
    
    local sizeX = (imgui.GetWindowWidth() - imgui.StyleVar.FramePadding - imgui.StyleVar.ItemSpacing)/2.93
    local sizeY = imgui.GetWindowHeight()/10
    fcommon.DropDownMenu(names[1],function()
        imgui.Spacing()

        for i = 1, #func do
            if imgui.Button(names[i+1],imgui.ImVec2(sizeX,sizeY)) then
                callFunction(func[i],1,1)
                fcommon.CheatActivated()
            end
            if i % 3 ~= 0 then
                imgui.SameLine()
            end
        end
    end)
end

function module.CheatsMain()
    if imgui.BeginChild("CheatsMain") then
        if imgui.BeginTabBar("Cheats list") then
            imgui.Spacing()
            if imgui.BeginTabItem(flanguage.GetText('cheats.Cheats')) then
                CheatsEntry({0x439110,0x439150,0x439190},{flanguage.GetText('cheats.Body'),flanguage.GetText('cheats.Fat'),flanguage.GetText('cheats.Muscle'),flanguage.GetText('cheats.Skinny')})
                CheatsEntry({0x438E40,0x438FF0},{flanguage.GetText('cheats.Fight'),flanguage.GetText('cheats.HESOYAM'),flanguage.GetText('cheats.Suicide')})
                CheatsEntry({0x438F90,0x438FC0},{flanguage.GetText('cheats.Gameplay'),flanguage.GetText('cheats.Faster'),flanguage.GetText('cheats.Slower')})
                CheatsEntry({0x439360,0x4393D0},{flanguage.GetText('cheats.Gangs'),flanguage.GetText('cheats.GangsEverywhere'),flanguage.GetText('cheats.GangLand')})
                CheatsEntry({0x4393F0,0x439710,0x439DD0,0x439C70,0x439B20},{flanguage.GetText('cheats.Peds'),flanguage.GetText('cheats.SlutMagnet'),flanguage.GetText('cheats.PedsRiot'),flanguage.GetText('cheats.AttackWithRocket'),flanguage.GetText('cheats.AttackPlayer'),flanguage.GetText('cheats.Mayhem')})
                CheatsEntry({0x439930,0x439940,0x4399D0},{flanguage.GetText('cheats.Skills'),flanguage.GetText('cheats.Stamina'),flanguage.GetText('cheats.Weapons'),flanguage.GetText('cheats.Vehicles')})
                CheatsEntry({0x4395B0,0x439600},{flanguage.GetText('cheats.Spawn'),flanguage.GetText('cheats.Parachute'),flanguage.GetText('cheats.Jetpack')})
                CheatsEntry({0x439230,0x439720,0x439E50},{flanguage.GetText('cheats.Themes'),flanguage.GetText('cheats.Beach'),flanguage.GetText('cheats.FunHouse'),flanguage.GetText('cheats.Ninja')})
                CheatsEntry({0x4390D0,0x4390F0,0x4394B0,0x4394E0},{flanguage.GetText('cheats.Traffic'),flanguage.GetText('cheats.Pink'),flanguage.GetText('cheats.Black'),flanguage.GetText('cheats.Cheap'),flanguage.GetText('cheats.Fast')})
                CheatsEntry({0x438F50,0x438F60,0x438F70,0x438F80,0x439570,0x439590},{flanguage.GetText('cheats.Weather'),flanguage.GetText('cheats.VerySunny'),flanguage.GetText('cheats.Overcast'),flanguage.GetText('cheats.Rainy'),flanguage.GetText('cheats.Foggy'),flanguage.GetText('cheats.Thunderstorm'),flanguage.GetText('cheats.Sandstorm')})
                CheatsEntry({0x439D80,0x4398D0},{flanguage.GetText('cheats.Vehicles'),flanguage.GetText('cheats.BlowUP'),flanguage.GetText('cheats.AimWhileDriving')})
                CheatsEntry({0x439540,0x4391D0,0x439F60,0x4395A0,0x439880},{flanguage.GetText('cheats.Misc'),flanguage.GetText('cheats.StopClock'),flanguage.GetText('cheats.ElvisEverywhere'),flanguage.GetText('cheats.CSInvasion'),flanguage.GetText('cheats.Predator'),flanguage.GetText('cheats.Adrenaline')})
                CheatsEntry({0x438E90,0x438F20,0x4396F0,0x4396C0},{flanguage.GetText('cheats.WantedLevel'),flanguage.GetText('cheats.+2Star'),flanguage.GetText('cheats.ClearStars'),flanguage.GetText('cheats.SixStar'),flanguage.GetText('cheats.NeverWanted')})
                CheatsEntry({0x4385B0,0x438890,0x438B30},{flanguage.GetText('cheats.Weapons'),flanguage.GetText('cheats.Set1'),flanguage.GetText('cheats.Set2'),flanguage.GetText('cheats.Set3')})
                imgui.EndTabItem()
            end
            imgui.EndTabBar()
        end
        imgui.EndChild()
    end
end

return module
