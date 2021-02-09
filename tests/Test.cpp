#include "plugin.h"

using namespace plugin;

class Test
{
public:
    Test()
    {   
        Events::initGameEvent += [] 
        {
            patch::PutRetn0(0x4B35A4);
        };
    }
} test;

