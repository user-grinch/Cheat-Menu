#pragma once
#include "interface/icheat.hpp"

/*
    TopDown for SA
    Activates/Disactivates cheats randomly
*/
class TopDownCamera : public ICheat<TopDownCamera>
{
private:

    friend class IFeature;
    TopDownCamera();
    TopDownCamera(const TopDownCamera&);

public:
    int m_nZoom = 40;

    // Process cheat each frame
    void DrawPages();
};

extern TopDownCamera& TopDownCam;