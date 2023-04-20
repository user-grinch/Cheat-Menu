#pragma once
#include "utils/resourcestore.h"
#include "interface/icheat.hpp"


/*
    AutoDriveMgr Class
    Automatically drives vehicle to marker/ coord
*/
class AutoDriveMgr : public ICheat<AutoDriveMgr>
{
private:

    friend class IFeature;
    AutoDriveMgr() {};
    AutoDriveMgr(const AutoDriveMgr&);

    void StartDrive(CVehicle *pVeh, const char *buf = nullptr);

public:

    // Draw custom skins handler code
    void Draw();
};

extern AutoDriveMgr& AutoDrive;