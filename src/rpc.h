#pragma once
#include "../depend/discord/discord.h"

class RPC
{
private:
    static inline discord::Core* pCore;
    static inline bool bInit = false;

public:
    RPC() = delete;
    RPC(RPC&) = delete;

    static void Init();
    static void Process();
    static void Shutdown();
};