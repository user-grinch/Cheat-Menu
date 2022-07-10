#pragma once
#include "pch.h"

struct ShowModelInfo
{
private:
    static inline std::vector<CEntity*> m_EntityList;

public:
    static inline bool m_bEnable;
    static inline int m_nDistance = 50.0f;

    static void Init();
    static void Draw();
};

class Visual
{
private:
    static inline bool m_bLockWeather;
    static inline bool m_bModelInfo;

#ifdef GTASA
    static inline bool m_bInvisibleWater;
    static inline bool m_bNoWater;
    static inline bool m_bDisableHydrant;
    static inline bool m_bNoMoneyZeros;
    static inline bool m_bNoPartciles;
    static inline bool m_bNoPostFX;
    static inline bool m_bNoTextures;
    static inline bool m_bFullScreenMap;
    static inline bool m_bSquareRadar;
#endif

    template<typename T>
    static bool TimeCycColorEdit3(const char* label, T* r, T* g, T* b, ImGuiColorEditFlags flags = 0);
    template <typename T>
    static bool TimeCycColorEdit4(const char* label, T* r, T* g, T* b, T* a, ImGuiColorEditFlags flags = 0);
    template <typename T>
    static void TimecycSlider(const char* label, T* data, int min, int max);
    
public:
    Visual() = delete;
    Visual(const Visual&) = delete;

    static void Init();
    static void ShowPage();
    static void DrawModelInfo();
};