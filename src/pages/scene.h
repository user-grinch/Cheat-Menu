#pragma once
#include "interface/ipage.h"

/*
    ScenePage Class
    Contains animations, cutscenes, particles, tasks etc
*/
class ScenePage : public IPage<ScenePage>
{
private:
    friend class IFeature;
    ScenePage() : IPage<ScenePage>(ePageID::Scene, "Scene", true) {}
    ScenePage(const ScenePage&);

public:
    void Draw();
};

extern ScenePage& scenePage;