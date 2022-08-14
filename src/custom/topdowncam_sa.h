#pragma once
#include "interface/icheat.hpp"

/*
    TopDown for SA
    Activates/Disactivates cheats randomly
*/
class TopDownCamera : public ICheat<TopDownCamera>
{
private:

    friend ICheat;
    TopDownCamera();
    TopDownCamera(const TopDownCamera&);
    
public:
    int m_nZoom = 40; 

    // Process cheat each frame
    void Process();
};

extern TopDownCamera& TopDownCam;