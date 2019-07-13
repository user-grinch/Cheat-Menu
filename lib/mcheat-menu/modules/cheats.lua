-- This module contains all the game original cheats

local module = {}

function module.cheats_section()
    imgui.Spacing()
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()

    fcommon.buttons_menu({0x439110,0x439150,0x439190},{"Body","Fat","Muscle","Skinny"},110,50,"horizantal")
    fcommon.buttons_menu({0x438E40,0x438FF0},{"Fight","Hesoyam","Suicide"},110,50,"horizantal")
    fcommon.buttons_menu({0x438F90,0x438FC0},{"Gameplay","Faster","Slower"},110,50,"horizantal")
    fcommon.buttons_menu({0x439360,0x4393D0},{"Gangs","Gangs Everywhere","Gang Land"},110,50,"horizantal")
    fcommon.buttons_menu({0x4393F0,0x439710,0x439DD0,0x439C70,0x439B20},{"Peds","Slut Magnet","Peds Riot","Attack with rocket","Attack player","Mayhem"},110,50,"horizantal")
    fcommon.buttons_menu({0x439930,0x439940,0x4399D0},{"Skills","Stamina","Weapons","Vehicles"},110,50,"horizantal")
    fcommon.buttons_menu({0x4395B0,0x439600},{"Spawn","Parachute","Jetpack"},110,50,"horizantal")
    fcommon.buttons_menu({0x439230,0x439720,0x439E50},{"Themes","Beach","Funhouse","Ninja"},110,50,"horizantal")
    fcommon.buttons_menu({0x4390D0,0x4390F0,0x4394B0,0x4394E0},{"Traffic","Pink","Black","Cheap","Fast"},110,50,"horizantal")
    fcommon.buttons_menu({0x438E90,0x438F20,0x4396F0,0x4396C0},{"Wanted Level","+2 Star","Clear","6 Star","Never Wanted"},110,50,"horizantal")
    fcommon.buttons_menu({0x4385B0,0x438890,0x438B30},{"Weapons","Set 1","Set 2","Set 3"},110,50,"horizantal")
    fcommon.buttons_menu({0x438F50,0x438F60,0x438F70,0x438F80,0x439570,0x439590},{"Weather","Very Sunny","Overcast","Rainy","Foggy","Thunderstorm","Sandstorm"},110,50,"horizantal")
    fcommon.buttons_menu({0x439D80,0x4398D0},{"Vehicles","Blow up","Aim while driving"},110,50,"horizantal")
    fcommon.buttons_menu({0x439540,0x4391D0,0x439F60,0x4395A0,0x439880},{"Misc","Stop Clock","Elvis Everywhere","CS Invasion","Predator","Adrenaline"},110,50,"horizantal")
 
end

return module