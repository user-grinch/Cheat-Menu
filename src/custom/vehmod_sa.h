#pragma once
#include "pch.h"
#include "interface/ifeature.hpp"

/*
    VehModMgr Class 
    Handles tunes & handling
*/
class VehModMgr : public IFeature<VehModMgr>
{
private:
    ResourceStore m_TuneData { "components", eResourceType::TYPE_IMAGE_TEXT, ImVec2(100, 80) };
    std::map<int, std::string> m_VehicleIDE;

    // Add/ Remove vehicle mod
    void AddComponent(const std::string& component, bool displayMessage = true);
    void RemoveComponent(const std::string& component, bool displayMessage = true);

    friend class IFeature;
    VehModMgr();
    VehModMgr(const VehModMgr&);

public:
    struct
    {
        bool m_bEnabled;
        bool m_bCompAdded;
    } m_Nitro;
    
    // Draw custom skins handler code
    void Draw();
};

extern VehModMgr& VehMod;