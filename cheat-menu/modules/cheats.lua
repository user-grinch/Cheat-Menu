-- This module contains all the game original cheats

local module = {}

function module.cheats_section()
    imgui.Spacing()
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()

    fcommon.buttons_menu({0x439110,0x439150,0x439190},{"Body","Fat","Muscle","Skinny"},150,50,"horizantal")
    fcommon.buttons_menu({0x438E40,0x438FF0},{"Fight","Health+Armour+Money","Suicide"},150,50,"horizantal")
    fcommon.buttons_menu({0x438F90,0x438FC0},{"Gameplay","Faster","Slower"},150,50,"horizantal")
    fcommon.buttons_menu({0x439360,0x4393D0},{"Gangs","Gangs Everywhere","Gang Land"},150,50,"horizantal")
    fcommon.buttons_menu({0x4393F0,0x439710,0x439DD0,0x439C70,0x439B20},{"Peds","Slut Magnet","Peds Riot","Peds attack with rockets","Peds attack player","Mayhem"},150,50,"horizantal")
    fcommon.buttons_menu({0x439930,0x439940,0x4399D0},{"Skills","Stamina","Weapons","Vehicles"},150,50,"horizantal")
    fcommon.buttons_menu({0x4395B0,0x439600},{"Spawn","Get Parachute","Get Jetpack"},150,50,"horizantal")
    fcommon.buttons_menu({0x439230,0x439720,0x439E50},{"Themes","Beach","Funhouse","Ninja"},150,50,"horizantal")
    fcommon.buttons_menu({0x4390D0,0x4390F0,0x4394B0,0x4394E0},{"Traffic","Pink","Black","Cheap","Fast"},150,50,"horizantal")
    fcommon.buttons_menu({0x438E90,0x438F20,0x4396F0,0x4396C0},{"Wanted Level","+2 wanted level","Clear wanted level","Six star wanted","Never wanted"},150,50,"horizantal")
    fcommon.buttons_menu({0x4385B0,0x438890,0x438B30},{"Weapons","Weapon Set 1","Weapon Set 2","Weapon Set 3"},150,50,"horizantal")
    fcommon.buttons_menu({0x438F50,0x438F60,0x438F70,0x438F80,0x439570,0x439590},{"Weather","Very Sunny","Overcast","Rainy","Foggy","Thunderstorm","Sandstorm"},150,50,"horizantal")
    fcommon.buttons_menu({0x439D80,0x4398D0},{"Vehicles","Blow up cars","Aim while driving"},150,50,"horizantal")
    fcommon.buttons_menu({0x439540,0x4391D0,0x439F60,0x4395A0,0x439880},{"Misc","Stop Clock","Elvis Everywhere","Countryside Invasion","Predator","Adrenaline"},150,50,"horizantal")
 
end

return module