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
    static inline std::vector<std::string> m_WeatherNames{"EXTRASUNNY LA",      "SUNNY LA",
                                                          "EXTRASUNNY SMOG LA", "SUNNY SMOG LA",
                                                          "CLOUDY LA",          "SUNNY SF",
                                                          "EXTRASUNNY SF",      "CLOUDY SF",
                                                          "RAINY SF",           "FOGGY SF",
                                                          "SUNNY VEGAS",        "EXTRASUNNY VEGAS",
                                                          "CLOUDY VEGAS",       "EXTRASUNNY COUNTRYSIDE",
                                                          "SUNNY COUNTRYSIDE",  "CLOUDY COUNTRYSIDE",
                                                          "RAINY COUNTRYSIDE",  "EXTRASUNNY DESERT",
                                                          "SUNNY DESERT",       "SANDSTORM DESERT",
                                                          "UNDERWATER",         "EXTRACOLOURS 1",
                                                          "EXTRACOLOURS 2"};

    static void GenerateTimecycFile();
    static int CalcArrayIndex();
    static bool TimeCycColorEdit3(const char *label, uchar *r, uchar *g, uchar *b, ImGuiColorEditFlags flags = 0);
    static bool TimeCycColorEdit4(const char *label, uchar *r, uchar *g, uchar *b, uchar *a,
                                  ImGuiColorEditFlags flags = 0);
    template <typename T> static void TimecycSlider(const char *label, T *data, int min, int max);

public:
    Visual();
    static void Draw();
};

template <typename T> void Visual::TimecycSlider(const char *label, T *ptr, int min, int max)
{
    int val = CalcArrayIndex();
#ifdef GTASA
    // Compatable with 24h TimeCyc
    T *arr = static_cast<T *>(patch::GetPointer(int(ptr)));
#else // GTA3 & GTAVC
    T *arr = static_cast<T *>(ptr);
#endif
    int a = arr[val];

    if (ImGui::SliderInt(label, &a, min, max))
        arr[val] = static_cast<T>(a);
}
