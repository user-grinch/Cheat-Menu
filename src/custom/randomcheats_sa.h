#pragma once
#include "interface/icheat.hpp"
#include "pch.h"

/*
    RandomCheats for SA
    Activates/Disactivates cheats randomly
*/
class RandomCheatsMgr : public ICheat<RandomCheatsMgr>
{
private:
    std::string m_EnabledCheats[92][2];
    DataStore m_pData {"cheats"};

    friend class IFeature;
    RandomCheatsMgr();
    RandomCheatsMgr(const RandomCheatsMgr&);

    // Process the RandomCheat each frame & draw progress bar
    void DrawPages();

public:
    bool m_bProgressBar = true;
    int m_nInterval = 10;           // interval between cheat activation/deactivation

    // Draws a list of cheats for ui
    void DrawList();
};

extern RandomCheatsMgr& RandomCheats;