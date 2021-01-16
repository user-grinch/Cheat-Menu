#include "plugin.h"
#include "CHud.h"

using namespace plugin;

class Test
{
public:
    Test()
    {   
        Events::processScriptsEvent += [] 
        {

        };
    }
} test;

