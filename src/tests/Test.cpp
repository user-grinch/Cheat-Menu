#include "plugin.h"

using namespace plugin;

_asm
{
    xor     ebx, ebx
    cmp     eax, ebx
    jz      short loc_6D1CFD
    call    _ZN10CGameLogic17IsCoopGameGoingOnEv ; CGameLogic::IsCoopGameGoingOn(void)
    test    al, al
    jnz     short loc_6D1CC4
}

class Test
{
public:
    Test()
    {   
        Events::initGameEvent += [] 
        {
            patch::ReplaceFunctionCall(0x6D1C80);
        };
    }
} test;

