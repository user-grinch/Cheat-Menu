#include "plugin.h"

using namespace plugin;

class Test
{
public:
    Test()
    {
        Events::processScriptsEvent += [] 
        {
           if(KeyPressed(VK_UP))
           {
                patch::Nop(0x4EB9F4, 5);    //  disable
           } 
           if(KeyPressed(VK_DOWN))
           {
                patch::SetRaw(0x4EB9F4, (void*)"\xE8\x67\xFC\xFF\xFF", 5);    //  enable
           } 
        };
    }
} test;

