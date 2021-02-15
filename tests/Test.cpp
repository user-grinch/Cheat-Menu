#include "plugin.h"
#include "CHud.h"

using namespace plugin;

void DoStuff();

class Test
{
public:
    
    Test()
    {   
        Events::processScriptsEvent += DoStuff;
    }
    ~Test()
    {   
        Events::processScriptsEvent -= DoStuff;
    }
} test;

void DoStuff()
{
    CHud::SetMessage((char*)"Test");

    if (KeyPressed(VK_TAB))
    {
        test.~Test();
    }
}

