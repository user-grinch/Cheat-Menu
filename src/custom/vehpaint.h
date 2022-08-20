#pragma once
#include "interface/ifeature.hpp"

/*
    VehPaintMgr Class 
    Handles neon, textures for vehicle
*/
class VehPaintMgr : public IFeature<VehPaintMgr>
{
private:
    std::vector<std::vector<float>> m_ColorData;    // vehicle color data from carcols.dat

    struct
    {
        float m_fColorPicker[3] { 0, 0, 0 };
        bool m_bRainbowEffect;
        unsigned int m_nRainbowTimer;
        bool m_bApplyOnTraffic;
        unsigned int m_nTrafficTimer;
    } m_Neon;

    struct
    {
        bool m_bMatFilter = true;
        int m_nRadioButton = 1;
        float m_fColorPicker[3] { 0, 0, 0 };
        std::vector<std::string> m_vecNames{"Default"};
        std::string m_Selected = "Default";
    } m_Paint;


    friend class IFeature;
    VehPaintMgr();
    VehPaintMgr(const VehPaintMgr&);

public:

    // Draw color, neon & texture tabs
    void Draw();
};

extern VehPaintMgr& VehPaint;