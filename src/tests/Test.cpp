#include "plugin.h"
#include "../vendor/fla/IDaccess.h"
#include "CHud.h"

using namespace plugin;

class Test
{
public:
    Test()
    {   
        Events::initRwEvent += []
        {
            CFastman92limitAdjuster::Init();
        };

        Events::processScriptsEvent += [] 
        {
            CPlayerPed *player = FindPlayerPed();

            if(KeyPressed(VK_UP) && player && player->m_pVehicle)
            {
                uint8_replacement &primary_color = *(uint8_replacement *)(int(player->m_pVehicle) + 0x434);
                primary_color = 74;
                CHud::SetHelpMessage("Color changed",false,false,false);
            } 
            if(KeyPressed(VK_DOWN))
            {

            } 
        };
    }
} test;

