#pragma once
#include "pch.h"
#include "interface/ipage.h"

class VisualPage : public IPage<VisualPage>
{
private:
    bool m_bLockWeather;

#ifdef GTASA
    bool m_bInvisibleWater;
    bool m_bNoWater;
    bool m_bDisableHydrant;
    bool m_bNoMoneyZeros;
    bool m_bNoPartciles;
    bool m_bNoPostFX;
    bool m_bNoTextures;
    bool m_bFullScreenMap;
    bool m_bSquareRadar;
    bool m_bNoRadarRot;
#endif

    friend class IFeature;
    VisualPage();
    VisualPage(const VisualPage&);

    template<typename T>
    bool TimeCycColorEdit3(const char* label, T* r, T* g, T* b, ImGuiColorEditFlags flags = 0);
    template <typename T>
    bool TimeCycColorEdit4(const char* label, T* r, T* g, T* b, T* a, ImGuiColorEditFlags flags = 0);
    template <typename T>
    void TimecycSlider(const char* label, T* data, int min, int max);

public:
    void Draw();

    // Patches the radar for visual changes
    void PatchRadar();
};

extern VisualPage& visualPage;