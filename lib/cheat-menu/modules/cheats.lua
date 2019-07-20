-- This module contains all the game original cheats

local module = {}

function CheatsEntry(func,names,sizeY,style)
    fcommon.DropDownMenu(names[1],function()
        imgui.Spacing()

        for i = 1, #func do
            if imgui.Button(names[i+1],imgui.ImVec2((imgui.GetWindowWidth()/#func) - 2*#func,sizeY)) then
                callFunction(func[i],1,1)
                fcommon.CheatActivated()
            end
            if not (i % 4 == 0) and style == "horizantal" then
                imgui.SameLine()
            end
        end
    end)
end

function module.CheatsMain()
    if imgui.BeginChild("CheatsMain") then
        if imgui.BeginTabBar("Cheats list") then
            imgui.Spacing()
            if imgui.BeginTabItem("Cheats") then
                CheatsEntry({0x439110,0x439150,0x439190},{"Body","Fat","Muscle","Skinny"},50,"horizantal")
                CheatsEntry({0x438E40,0x438FF0},{"Fight","Hesoyam","Suicide"},50,"horizantal")
                CheatsEntry({0x438F90,0x438FC0},{"Gameplay","Faster","Slower"},50,"horizantal")
                CheatsEntry({0x439360,0x4393D0},{"Gangs","Gangs Everywhere","Gang Land"},50,"horizantal")
                CheatsEntry({0x4393F0,0x439710,0x439DD0,0x439C70,0x439B20},{"Peds","Slut Magnet","Peds Riot","Attack with rocket","Attack player","Mayhem"},50,"horizantal")
                CheatsEntry({0x439930,0x439940,0x4399D0},{"Skills","Stamina","Weapons","Vehicles"},50,"horizantal")
                CheatsEntry({0x4395B0,0x439600},{"Spawn","Parachute","Jetpack"},50,"horizantal")
                CheatsEntry({0x439230,0x439720,0x439E50},{"Themes","Beach","Funhouse","Ninja"},50,"horizantal")
                CheatsEntry({0x4390D0,0x4390F0,0x4394B0,0x4394E0},{"Traffic","Pink","Black","Cheap","Fast"},50,"horizantal")
                CheatsEntry({0x438E90,0x438F20,0x4396F0,0x4396C0},{"Wanted Level","+2 Star","Clear","6 Star","Never Wanted"},50,"horizantal")
                CheatsEntry({0x4385B0,0x438890,0x438B30},{"Weapons","Set 1","Set 2","Set 3"},50,"horizantal")
                CheatsEntry({0x438F50,0x438F60,0x438F70,0x438F80,0x439570,0x439590},{"Weather","Very Sunny","Overcast","Rainy","Foggy","Thunderstorm","Sandstorm"},50,"horizantal")
                CheatsEntry({0x439D80,0x4398D0},{"Vehicles","Blow up","Aim while driving"},50,"horizantal")
                CheatsEntry({0x439540,0x4391D0,0x439F60,0x4395A0,0x439880},{"Misc","Stop Clock","Elvis Everywhere","CS Invasion","Predator","Adrenaline"},50,"horizantal")
                imgui.EndTabItem()
            end
            imgui.EndTabBar()
        end
        imgui.EndChild()
    end
end

return module
