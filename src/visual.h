#pragma once
#include "pch.h"

class Visual
{
private:
    static inline bool m_bLockWeather;

#ifdef GTASA
    static inline bool m_bInvisibleWater;
    static inline bool m_bNoWater;
    static inline bool m_bDisableHydrant;
    static inline bool m_bNoMoneyZeros;
    static inline bool m_bNoPartciles;
    static inline bool m_bNoPostFX;
    static inline bool m_bNoTextures;
#endif

public:
    Visual() = delete;
    Visual(const Visual&) = delete;

    static void Init();
    static void ShowPage();
};