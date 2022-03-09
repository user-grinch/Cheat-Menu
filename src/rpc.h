#pragma once
#include "../depend/discord/discord.h"

class RPC
{
private:
    static inline discord::Core* pCore;
    static inline bool bInit = false;
    static inline size_t totalLargeImages = 5;

public:
    RPC() = delete;
    RPC(RPC&) = delete;

    static void Init();
    static void Process();
    static void Shutdown();
};