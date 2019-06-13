-- This module contains all the functions used in peds

local module = {}
local tpeds = 
{
    path = getGameDirectory() .. "\\moonloader\\cheat-menu\\peds\\",
    list = {},
    special = 
    {
        -- list of SA special peds
        [1]   = "TRUTH",
        [2]   = "MACCER",
        [3]   = "ANDRE",
        [4]   = "BBTHIN",
        [5]   = "BB",
        [6]   = "EMMET",
        [8]   = "JANITOR",
        [42]  = "JETHRO",
        [65]  = "KENDL",
        [86]  = "RYDER1",
        [119] = "SINDACO",
        [149] = "SMOKEV",
        [190] = "COPGRL1",
        [191] = "GUNGRL1",
        [192] = "MECGRL1",
        [193] = "NURGRL1",
        [194] = "CROGRL1",
        [195] = "GANGRL1",
        [208] = "SUZIE",
        [265] = "TENPEN",
        [266] = "PULASKI",
        [267] = "HERN",
        [268] = "DWAYNE",
        [269] = "SMOKE",  
        [270] = "SWEET",
        [272] = "FORELLI",
        [273] = "TBONE",
        [289] = "ZERO",
        [290] = "ROSE", 
        [291] = "PAUL",
        [292] = "CESAR",
        [293] = "OGLOC",
        [294] = "WUZIMU",
        [295] = "TORINO",
        [296] = "JIZZY",
        [297] = "MADDOGG",
        [298] = "CAT",
        [299] = "CLAUDE",

    },
}

module.tpeds = tpeds

give_ped_to_player = function(model)

    if fpeds.tpeds.special[model] == nil then      
        if isModelAvailable(model) then 
            requestModel(model)
            loadAllModelsNow()
            x,y,z = getCharCoordinates(PLAYER_PED)
            createChar(4,model,x,y,z)
            markModelAsNoLongerNeeded(model)
            printHelpString("Ped ~g~Spawned")
        end
    else
        if not hasSpecialCharacterLoaded(290) then 
            loadSpecialCharacter(fpeds.tpeds.special[model],1)
            loadAllModelsNow()
        end	
        x,y,z = getCharCoordinates(PLAYER_PED)
        createChar(1,290,x,y,z)
        unloadSpecialCharacter(290)
        printHelpString("Ped ~g~Spawned")
    end
end

function module.peds_section()
    imgui.Text("Options")
    imgui.Separator()
    imgui.Spacing()
    fcommon.radio_menu("Recruit Anyone",{"9mm","AK47","Rockets"},{0x96917C,0x96917D,0x96917E},"horizantal")
    imgui.Columns(2)
    fcommon.check_box({ address = 0x969175,name = "Peds Riot"})
    fcommon.check_box({ address = 0x969140,name = "Everyone is armed"})
    fcommon.check_box({ address = 0x96913F,name = "Have bounty on head"})
    imgui.NextColumn()
    fcommon.check_box({ address = 0x96915D,name = "Slut magnet"})
    fcommon.check_box({ address = 0x969157,name = "Elvis everywhere"})
    fcommon.check_box({ address = 0x969158,name = "Peds attack with rockets"})
    imgui.Columns(1)
    imgui.Text("Menus")
    imgui.Separator()
    imgui.Spacing()
    fcommon.popup_menu({name = "Pedestrian Density",address = 0x8D2530,size = 4,max = 100,help_text = "Sets the pedestrain density in current zone."})
    if imgui.BeginMenu("Ped Spawner",true) then
        imgui.Text("Choose Ped")
        imgui.Separator()
        imgui.Spacing()
        fcommon.list_ped_images(give_ped_to_player)
        imgui.EndMenu()
    end
end
return module