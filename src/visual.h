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
#endif

    // Timecyc stuff
    static inline int m_nTimecycHour = 8;
    static inline std::vector<std::string> m_WeatherNames
    {
#ifdef GTASA
        "EXTRASUNNY LA", "SUNNY LA", "EXTRASUNNY SMOG LA", "SUNNY SMOG LA", "CLOUDY LA", "SUNNY SF", "EXTRASUNNY SF",
        "CLOUDY SF", "RAINY SF", "FOGGY SF",
        "SUNNY VEGAS", "EXTRASUNNY VEGAS", "CLOUDY VEGAS", "EXTRASUNNY COUNTRYSIDE", "SUNNY COUNTRYSIDE",
        "CLOUDY COUNTRYSIDE", "RAINY COUNTRYSIDE",
        "EXTRASUNNY DESERT", "SUNNY DESERT", "SANDSTORM DESERT", "UNDERWATER", "EXTRACOLOURS 1", "EXTRACOLOURS 2"
#else
        "SUNNY", "CLOUDY", "RAINY", "FOGGY"
#ifdef GTAVC
        ,"EXTRA_SUNNY", "HURRICANE", "EXTRACOLORS"
#endif
#endif
    };

    static void GenerateTimecycFile();
    static int CalcArrayIndex();
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
};