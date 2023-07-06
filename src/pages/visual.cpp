#include "pch.h"
#include "visual.h"
#include "game.h"
#include "vehicle.h"
#include "utils/widget.h"
#include "utils/util.h"
#include "utils/timecycle.h"
#include <CWorld.h>

#ifdef GTASA
#include <CHudColours.h>
#endif

#ifdef GTASA
#define TOTAL_WEATHERS 23

// taken from vHud (_AG)
static bool IsTouchingRect(CVector2D& point, CRect rect1, CRect rect2)
{
    float vroot;
    float v12;
    float v13;
    float v14;
    float v15;
    float w1;
    float h1;
    float w2;
    float v19;
    float h2;
    float w;
    float h;

    if (rect1.right == rect1.left && rect1.bottom == rect1.top || rect2.right == rect2.left && rect2.bottom == rect2.top)
    {
        return false;
    }

    if (rect2.left == rect1.left && rect2.top == rect1.top || rect2.left == rect1.right && rect2.top == rect1.bottom || rect2.right == rect1.left && rect2.bottom == rect1.top || rect2.right == rect1.right && rect2.bottom == rect1.bottom)
    {
        return false;
    }

    w1 = rect1.right - rect1.left;
    h1 = rect1.bottom - rect1.top;
    w2 = rect2.left - rect1.left;
    h2 = rect2.top - rect1.top;
    w = rect2.right - rect1.left;
    h = rect2.bottom - rect1.top;

    vroot = sqrtf(w1 * w1 + h1 * h1);
    v12 = h2 * (w1 / vroot) - w2 * (h1 / vroot);
    v19 = h1 / vroot * h2 + w1 / vroot * w2;
    v14 = h * (w1 / vroot) - w * (h1 / vroot);

    if (v12 < 0.0f && v14 < 0.0f || v12 >= 0.0f && v14 >= 0.0f)
    {
        return false;
    }

    v13 = h1 / vroot * h + w1 / vroot * w;
    v15 = v13 + (v19 - v13) * v14 / (v14 - v12);

    if (v15 < 0.0f)
    {
        return false;
    }

    if (v15 > vroot)
    {
        return false;
    }

    point.x = w1 / vroot * v15 + rect1.left;
    point.y = v15 * (h1 / vroot) + rect1.top;
    return true;
}

static float LimitRadarPoint(CVector2D& point)
{
    if (FrontEndMenuManager.m_bDrawRadarOrMap)
    {
        return point.Magnitude();
    }

    if (point.x >=  -1.0f && point.x <= 1.0f && point.y >= -1.0f && point.y <= 1.0f)
    {
        return 0.99f;
    }

    CVector2D temp;
    CRect rect[4] =
    {
        {-1.0f, 1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f, -1.0f}
    };

    for (int i = 0; i <= 4; i++)
    {
        if (IsTouchingRect(temp, rect[i], CRect(0.0f, 0.0f, point.x, point.y)))
        {
            point.x = temp.x;
            point.y = temp.y;
            break;
        }
    }

    return 1.1f;
}

#elif GTAVC
#define TOTAL_WEATHERS 7
#else
#define TOTAL_WEATHERS 4
#endif

// Timecyc stuff
static int m_nTimecycHour = 8;
static std::vector<std::string> m_WeatherNames
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

VisualPage& visualPage = VisualPage::Get();
VisualPage::VisualPage()
    : IPage<VisualPage>(ePageID::Visual, "Visual", true)
{
#ifdef GTASA

    if (GetModuleHandle("timecycle24.asi"))
    {
        m_nTimecycHour = 24;
    }
#endif

    Events::processScriptsEvent += [this]
    {
        // TODO: Needs improvement
        static short m_nBacWeatherType;
        if (m_bLockWeather)
        {
            CWeather::OldWeatherType = m_nBacWeatherType;
            CWeather::NewWeatherType = m_nBacWeatherType;
        }
        else
        {
            m_nBacWeatherType = CWeather::OldWeatherType;
        }
    };
}

template <typename T>
int GetTCVal(T* addr, int index)
{
#ifdef GTASA
    T* arr = static_cast<T*>(patch::GetPointer(int(addr)));
#else
    T* arr = static_cast<T*>(addr);
#endif
    return static_cast<int>(arr[index]);
}

static void GenerateTimecycFile()
{
#ifdef GTASA
    std::ofstream file;
    std::string buffer;
    if (m_nTimecycHour == 24)
    {
        file = std::ofstream("timecyc_24h.dat");
    }
    else
    {
        file = std::ofstream("timecyc.dat");
    }

    for (uint i = 0; i < m_WeatherNames.size(); ++i)
    {
        buffer += "\n// " + m_WeatherNames[i] + "\n";
        buffer += "//	Amb					Amb Obj 				Dir 					Sky top				Sky bot				SunCore					SunCorona			SunSz	SprSz	SprBght		Shdw	LightShd	PoleShd		FarClp		FogSt	LightOnGround	LowCloudsRGB	BottomCloudRGB		WaterRGBA				ARGB1					ARGB2			CloudAlpha		IntensityLimit		WaterFogAlpha	DirMult";

        file << buffer << std::endl;
        for (int j = 0; j < m_nTimecycHour; ++j)
        {
            if (m_nTimecycHour == 24)
            {
                if (j < 12)
                {
                    buffer = std::format("// {} AM\n", j);
                }
                else
                {
                    buffer = std::format("// {} PM\n", j);
                }
            }
            else
            {
                if (j == 0) buffer = "// Midnight\n";
                if (j == 1) buffer = "// 5 AM\n";
                if (j == 2) buffer = "// 6 AM\n";
                if (j == 3) buffer = "// 7 AM\n";
                if (j == 4) buffer = "// Midday\n";
                if (j == 5) buffer = "// 7 PM\n";
                if (j == 6) buffer = "// 8 PM\n";
                if (j == 7) buffer = "// 10 PM\n";
            }

            int val = TOTAL_WEATHERS * j + i;

            buffer += std::format("{} {} {}\t{} {} {}\t255 255 255\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {} {}\t{} {} {} {}\t{} {} {} {}\t{} {} {} {}",
                                  GetTCVal(m_nAmbientRed, val), GetTCVal(m_nAmbientGreen, val), GetTCVal(m_nAmbientBlue, val),
                                  GetTCVal(m_nAmbientRed_Obj, val), GetTCVal(m_nAmbientGreen_Obj, val), GetTCVal(m_nAmbientBlue_Obj, val),
                                  GetTCVal(m_nSkyTopRed, val), GetTCVal(m_nSkyTopGreen, val), GetTCVal(m_nSkyTopBlue, val),
                                  GetTCVal(m_nSkyBottomRed, val), GetTCVal(m_nSkyBottomGreen, val), GetTCVal(m_nSkyBottomBlue, val),
                                  GetTCVal(m_nSunCoreRed, val), GetTCVal(m_nSunCoreGreen, val), GetTCVal(m_nSunCoreBlue, val),
                                  GetTCVal(m_nSunCoronaRed, val), GetTCVal(m_nSunCoronaGreen, val), GetTCVal(m_nSunCoronaBlue, val),
                                  GetTCVal(m_fSunSize, val) / 10.0f, GetTCVal(m_fSpriteSize, val) / 10.0f, GetTCVal(m_fSpriteBrightness, val) / 10.0f,
                                  GetTCVal(m_nShadowStrength, val), GetTCVal(m_nLightShadowStrength, val), GetTCVal(m_nPoleShadowStrength, val),
                                  GetTCVal(m_fFarClip, val), GetTCVal(m_fFogStart, val), GetTCVal(m_fLightsOnGroundBrightness, val)/ 10.0f,
                                  GetTCVal(m_nLowCloudsRed, val), GetTCVal(m_nLowCloudsGreen, val), GetTCVal(m_nLowCloudsBlue, val),
                                  GetTCVal(m_nSkyBottomRed, val), GetTCVal(m_nSkyBottomGreen, val), GetTCVal(m_nSkyBottomBlue, val),
                                  GetTCVal(m_fWaterRed, val), GetTCVal(m_fWaterGreen, val), GetTCVal(m_fWaterBlue, val), GetTCVal(m_fWaterAlpha, val),
                                  GetTCVal(m_fPostFx1Red, val), GetTCVal(m_fPostFx1Green, val), GetTCVal(m_fPostFx1Blue, val), GetTCVal(m_fPostFx1Alpha, val),
                                  GetTCVal(m_fPostFx2Red, val), GetTCVal(m_fPostFx2Green, val), GetTCVal(m_fPostFx2Blue, val), GetTCVal(m_fPostFx2Alpha, val),
                                  GetTCVal(m_fCloudAlpha, val), GetTCVal(m_nHighLightMinIntensity, val), GetTCVal(m_nWaterFogAlpha, val), GetTCVal(m_nDirectionalMult, val) / 100.0
                                 );
            file << buffer << std::endl;
        }
    }
#else
    std::ofstream file = std::ofstream("timecyc.dat");

    for (uint i = 0; i < TOTAL_WEATHERS; ++i)
    {
        std::string buffer;
        switch(i)
        {
        case 0:
            buffer =  "\n// SUNNY\n";
            break;
        case 1:
            buffer =  "\n// CLOUDY\n";
            break;
        case 2:
            buffer =  "\n// RAINY\n";
            break;
        case 3:
            buffer =  "\n// FOGGY\n";
#ifdef GTAVC
        case 4:
            buffer =  "\n// EXTRA SUNNY\n";
        case 5:
            buffer =  "\n// HURRICANE\n";
        case 6:
            buffer =  "\n// EXTRA COLORS (INTERIORS)\n// These colours do not belong to a weather type but can be set by the level designers for interiors.\n";
#endif
        }

#ifdef GTA3
        buffer += "// Amb     Dir        Sky top	Sky bot			SunCore 		SunCorona   SunSz  SprSz	SprBght Shdw LightShd  TreeShd FarClp  FogSt	LightOnGround 	LowCloudsRGB 	TopCloudRGB 	BottomCloudRGB		PostFx";
#else
        buffer += "// Amb     Amb_Obj    Amb_bl     Amb_Obj_bl	   Dir          Sky top			Sky bot		SunCore 		SunCorona   SunSz  SprSz	SprBght Shdw LightShd  PoleShd 	FarClp 	 FogSt 	LightOnGround	LowCloudsRGB TopCloudRGB BottomCloudRGB		BlurRGB					WaterRGBA";
#endif
        file << buffer << std::endl;

        for (size_t j = 0; j < 24; ++j)
        {

#ifdef GTAVC
            if (i == 6) //EXTRA COLORS
            {
                buffer = "// Extra Color " + std::to_string(j);

                static std::string intNames[] =
                {
                    "Maibu Club", "Strip Club", "Hotel", "Bank", "Police HQ", "Mall", "Rifle Range", "Mansion", "Dirtring", "Blood ring",
                    "Hotring", "Concert Hall", "Auntie Poulets", "Intro at Docks", "Biker Bar", "Intro Cafe Dark Room", "Studio"
                };

                if (j < 18)
                {
                    buffer += "(" + intNames[j] + ")";
                }
            }
            else
            {
#endif
                buffer = "// " + std::to_string(j) + " ";
                if (j < 12)
                {
                    buffer += "AM\n";
                }
                else
                {
                    buffer += "PM\n";
                }
#ifdef GTAVC
            }
#endif

#ifdef GTA3
            size_t val = i + TOTAL_WEATHERS*j;
            buffer += std::format("{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {} {}",
                                  GetTCVal(m_nAmbientRed, val), GetTCVal(m_nAmbientGreen, val), GetTCVal(m_nAmbientBlue, val),
                                  GetTCVal(m_nDirRed, val), GetTCVal(m_nDirGreen, val), GetTCVal(m_nDirBlue, val),
                                  GetTCVal(m_nSkyTopRed, val), GetTCVal(m_nSkyTopGreen, val), GetTCVal(m_nSkyTopBlue, val),
                                  GetTCVal(m_nSkyBottomRed, val), GetTCVal(m_nSkyBottomGreen, val), GetTCVal(m_nSkyBottomBlue, val),
                                  GetTCVal(m_nSunCoreRed, val), GetTCVal(m_nSunCoreGreen, val), GetTCVal(m_nSunCoreBlue, val),
                                  GetTCVal(m_nSunCoronaRed, val), GetTCVal(m_nSunCoronaGreen, val), GetTCVal(m_nSunCoronaBlue, val),
                                  GetTCVal(m_fSunSize, val), GetTCVal(m_fSpriteSize, val), GetTCVal(m_fSpriteBrightness, val),
                                  GetTCVal(m_nShadowStrength, val), GetTCVal(m_nLightShadowStrength, val), GetTCVal(m_nTreeShadowStrength, val),
                                  GetTCVal(m_fFarClip, val), GetTCVal(m_fFogStart, val), GetTCVal(m_fLightsOnGroundBrightness, val),
                                  GetTCVal(m_nLowCloudsRed, val), GetTCVal(m_nLowCloudsGreen, val), GetTCVal(m_nLowCloudsBlue, val),
                                  GetTCVal(m_nTopCloudsRed, val), GetTCVal(m_nTopCloudsGreen, val), GetTCVal(m_nTopCloudsBlue, val),
                                  GetTCVal(m_nBottomCloudsRed, val), GetTCVal(m_nBottomCloudsGreen, val), GetTCVal(m_nBottomCloudsBlue, val),
                                  GetTCVal(m_fPostFxRed, val), GetTCVal(m_fPostFxGreen, val), GetTCVal(m_fPostFxBlue, val), GetTCVal(m_fPostFxAlpha, val)
                                 );
#elif GTAVC
            size_t val = TOTAL_WEATHERS * i + j;
            buffer += std::format("{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {}\t{} {} {} {}",
                                  GetTCVal(m_nAmbientRed, val), GetTCVal(m_nAmbientGreen, val), GetTCVal(m_nAmbientBlue, val),
                                  GetTCVal(m_nAmbientRed_Obj, val), GetTCVal(m_nAmbientGreen_Obj, val), GetTCVal(m_nAmbientBlue_Obj, val),
                                  GetTCVal(m_nAmbientBlRed, val), GetTCVal(m_nAmbientBlGreen, val), GetTCVal(m_nAmbientBlBlue, val),
                                  GetTCVal(m_nAmbientBlRed_Obj, val), GetTCVal(m_nAmbientBlGreen_Obj, val), GetTCVal(m_nAmbientBlBlue_Obj, val),
                                  GetTCVal(m_nDirRed, val), GetTCVal(m_nDirGreen, val), GetTCVal(m_nDirBlue, val),
                                  GetTCVal(m_nSkyTopRed, val), GetTCVal(m_nSkyTopGreen, val), GetTCVal(m_nSkyTopBlue, val),
                                  GetTCVal(m_nSkyBottomRed, val), GetTCVal(m_nSkyBottomGreen, val), GetTCVal(m_nSkyBottomBlue, val),
                                  GetTCVal(m_nSunCoreRed, val), GetTCVal(m_nSunCoreGreen, val), GetTCVal(m_nSunCoreBlue, val),
                                  GetTCVal(m_nSunCoronaRed, val), GetTCVal(m_nSunCoronaGreen, val), GetTCVal(m_nSunCoronaBlue, val),
                                  GetTCVal(m_fSunSize, val) / 10.0f, GetTCVal(m_fSpriteSize, val) / 10.0f, GetTCVal(m_fSpriteBrightness, val) / 10.0f,
                                  GetTCVal(m_nShadowStrength, val), GetTCVal(m_nLightShadowStrength, val), GetTCVal(m_nPoleShadowStrength, val),
                                  GetTCVal(m_fFarClip, val), GetTCVal(m_fFogStart, val), GetTCVal(m_fLightsOnGroundBrightness, val)/ 10.0f,
                                  GetTCVal(m_nLowCloudsRed, val), GetTCVal(m_nLowCloudsGreen, val), GetTCVal(m_nLowCloudsBlue, val),
                                  GetTCVal(m_nTopCloudsRed, val), GetTCVal(m_nTopCloudsGreen, val), GetTCVal(m_nTopCloudsBlue, val),
                                  GetTCVal(m_nBottomCloudsRed, val), GetTCVal(m_nBottomCloudsGreen, val), GetTCVal(m_nBottomCloudsBlue, val),
                                  GetTCVal(m_nBlurRed, val), GetTCVal(m_nBlurGreen, val), GetTCVal(m_nBlurBlue, val),
                                  GetTCVal(m_fWaterRed, val), GetTCVal(m_fWaterGreen, val), GetTCVal(m_fWaterBlue, val), GetTCVal(m_fWaterAlpha, val)
                                 );
#endif
            file << buffer << std::endl;
        }
    }
#endif
}

int CalcArrayIndex()
{
    int hour = CClock::ms_nGameClockHours;

#ifdef GTASA
    if (m_nTimecycHour == 24)
    {
        hour = hour;
    }
    else
    {
        if (hour < 5)
            hour = 0;
        if (hour == 5)
            hour = 1;
        if (hour == 6)
            hour = 2;
        if (7 <= hour && hour < 12)
            hour = 3;
        if (12 <= hour && hour < 19)
            hour = 4;
        if (hour == 19)
            hour = 5;
        if (hour == 20 || hour == 21)
            hour = 6;
        if (hour == 22 || hour == 23)
            hour = 7;
    }
#endif
    return TOTAL_WEATHERS * hour + CWeather::OldWeatherType;
}

template <typename T>
void TimecycSlider(const char* label, T* ptr, int min, int max)
{
    int val = CalcArrayIndex();
#ifdef GTASA
    // Compatable with 24h TimeCyc
    T* arr = static_cast<T*>(patch::GetPointer(int(ptr)));
#else
    T* arr = static_cast<T*>(ptr);
#endif
    int a = arr[val];

    if (ImGui::SliderInt(label, &a, min, max))
        arr[val] = static_cast<T>(a);
}

template<typename T>
bool VisualPage::TimeCycColorEdit3(const char* label, T* r, T* g, T* b, ImGuiColorEditFlags flags)
{
    bool rtn = false;
    int val = CalcArrayIndex();

#ifdef GTASA
    auto red = static_cast<T*>(patch::GetPointer(int(r)));
    auto green = static_cast<T*>(patch::GetPointer(int(g)));
    auto blue = static_cast<T*>(patch::GetPointer(int(b)));
#else
    auto red = static_cast<T*>(r);
    auto green = static_cast<T*>(g);
    auto blue = static_cast<T*>(b);
#endif

    float col[3] { red[val] / 255.0f, green[val] / 255.0f, blue[val] / 255.0f };

    if (ImGui::ColorEdit3(label, col, flags))
    {
        red[val] = col[0] * 255;
        green[val] = col[1] * 255;
        blue[val] = col[2] * 255;
        rtn = true;
    }

    return rtn;
}

template <typename T>
void VisualPage::TimecycSlider(const char* label, T* ptr, int min, int max)
{
    int val = CalcArrayIndex();
#ifdef GTASA
    // Compatable with 24h TimeCyc
    T* arr = static_cast<T*>(patch::GetPointer(int(ptr)));
#else
    T* arr = static_cast<T*>(ptr);
#endif
    int a = arr[val];

    if (ImGui::SliderInt(label, &a, min, max))
        arr[val] = static_cast<T>(a);
}

template <typename T>
bool VisualPage::TimeCycColorEdit4(const char* label, T* r, T* g, T* b, T* a, ImGuiColorEditFlags flags)
{
    bool rtn = false;
    int val = CalcArrayIndex();

#ifdef GTASA
    auto red = static_cast<T*>(patch::GetPointer(int(r)));
    auto green = static_cast<T*>(patch::GetPointer(int(g)));
    auto blue = static_cast<T*>(patch::GetPointer(int(b)));
    auto alpha = static_cast<T*>(patch::GetPointer(int(a)));
#else
    auto red = static_cast<T*>(r);
    auto green = static_cast<T*>(g);
    auto blue = static_cast<T*>(b);
    auto alpha = static_cast<T*>(a);
#endif

    float col[4] { red[val] / 255.0f, green[val] / 255.0f, blue[val] / 255.0f, alpha[val] / 255.0f };

    if (ImGui::ColorEdit4(label, col, flags))
    {
        red[val] = col[0] * 255;
        green[val] = col[1] * 255;
        blue[val] = col[2] * 255;
        alpha[val] = col[3] * 255;
        rtn = true;
    }

    return rtn;
}


static void ColorPickerAddr(const char* label, int addr, ImVec4&& default_color)
{
    if (ImGui::CollapsingHeader(label))
    {
        float cur_color[4];
        cur_color[0] = patch::Get<BYTE>(addr, false);
        cur_color[1] = patch::Get<BYTE>(addr + 1, false);
        cur_color[2] = patch::Get<BYTE>(addr + 2, false);
        cur_color[3] = patch::Get<BYTE>(addr + 3, false);

        // 0-255 -> 0-1
        cur_color[0] /= 255;
        cur_color[1] /= 255;
        cur_color[2] /= 255;
        cur_color[3] /= 255;

        if (ImGui::ColorPicker4(std::string("Pick color##" + std::string(label)).c_str(), cur_color))
        {
            // 0-1 -> 0-255
            cur_color[0] *= 255;
            cur_color[1] *= 255;
            cur_color[2] *= 255;
            cur_color[3] *= 255;

            patch::Set<BYTE>(addr, cur_color[0], false);
            patch::Set<BYTE>(addr + 1, cur_color[1], false);
            patch::Set<BYTE>(addr + 2, cur_color[2], false);
            patch::Set<BYTE>(addr + 3, cur_color[3], false);
        }
        ImGui::Spacing();

        if (ImGui::Button("Reset to default", Widget::CalcSize()))
        {
            patch::Set<BYTE>(addr, default_color.x, false);
            patch::Set<BYTE>(addr + 1, default_color.y, false);
            patch::Set<BYTE>(addr + 2, default_color.z, false);
            patch::Set<BYTE>(addr + 3, default_color.w, false);
        }

        ImGui::Spacing();
        ImGui::Separator();
    }
}

void VisualPage::PatchRadar()
{
#ifdef GTASA
    static float clockPosX = *(float*)*(int*)0x58EC16;
    static float clockPosY = *(float*)*(int*)0x58EC04;
    static float radarPosX = *(float*)*(int*)0x5834D4;
    static float radarPosY = *(float*)*(int*)0x583500;
    static float radarWidth = *(float*)*(int*)0x5834C2;
    static float radarHeight = *(float*)*(int*)0x5834F6;
    static CHudColour armourBar = HudColour.m_aColours[4];
    static CHudColour clockBar = HudColour.m_aColours[4];
    static CHudColour healthBar = HudColour.m_aColours[0];
    static CHudColour breathBar = HudColour.m_aColours[3];
    static CHudColour wantedBar = HudColour.m_aColours[6];
    static float moneyPosX = *(float*)*(int*)0x58F5FC;
    static float breathPosX = *(float*)*(int*)0x58F11F;
    static float breathPosY = *(float*)*(int*)0x58F100;
    static float weaponIconPosX = *(float*)*(int*)0x58F927;
    static float weaponIconPosY = *(float*)*(int*)0x58F913;
    static float weaponAmmoPosX = *(float*)*(int*)0x58FA02;
    static float weaponAmmoPosY = *(float*)*(int*)0x58F9E6;
    static float wantedPosX = *(float*)*(int*)0x58DD0F;

    patch::SetPointer(0x58EC16, &clockPosX);
    patch::SetPointer(0x58EC04, &clockPosY);
    patch::SetPointer(0x5834D4, &radarPosX);
    patch::SetPointer(0x583500, &radarPosY);
    patch::SetPointer(0x5834F6, &radarHeight);
    patch::SetPointer(0x5834C2, &radarWidth);

    patch::SetPointer(0x58A79B, &radarPosX);
    patch::SetPointer(0x58A7C7, &radarPosY);
    patch::SetPointer(0x58A801, &radarHeight);
    patch::SetPointer(0x58A7E9, &radarWidth);
    patch::SetPointer(0x58A836, &radarPosX);
    patch::SetPointer(0x58A868, &radarPosY);
    patch::SetPointer(0x58A8AB, &radarHeight);
    patch::SetPointer(0x58A840, &radarWidth);
    patch::SetPointer(0x58A8E9, &radarPosX);
    patch::SetPointer(0x58A913, &radarPosY);
    patch::SetPointer(0x58A921, &radarHeight);
    patch::SetPointer(0x58A943, &radarWidth);
    patch::SetPointer(0x58A98A, &radarPosX);
    patch::SetPointer(0x58A9C7, &radarPosY);
    patch::SetPointer(0x58A9D5, &radarHeight);
    patch::SetPointer(0x58A99D, &radarWidth);

    patch::SetPointer(0x5890FC, &armourBar);
    patch::SetChar(0x5890F5, 0);
    patch::SetPointer(0x589331, &healthBar);
    patch::SetPointer(0x5891EB, &breathBar);
    patch::SetChar(0x5891E4, 0);
    patch::SetPointer(0x58EBD1, &clockBar);
    patch::SetChar(0x58EBCA, 0);

    patch::SetPointer(0x58F5FC, &moneyPosX);
    patch::SetPointer(0x58F11F, &breathPosX);
    patch::SetPointer(0x58F100, &breathPosY);
    patch::SetPointer(0x58DD0F, &wantedPosX);
    patch::SetPointer(0x58F927, &weaponIconPosX);
    patch::SetPointer(0x58F913, &weaponIconPosY);
    patch::SetPointer(0x58FA02, &weaponAmmoPosX);
    patch::SetPointer(0x58F9E6, &weaponAmmoPosY);
#elif GTAVC
    // posY
    patch::SetUInt(0x55A972, 0x68FD34);
    patch::SetUInt(0x55AAB2, 0x68FD34);
    // scaleY
    patch::SetUInt(0x55A9C1, 0x68FD30);
    patch::SetUInt(0x55AAFA, 0x68FD30);
    // scaleX
    patch::SetUInt(0x55A9A8, 0x68FD24);
    patch::SetUInt(0x55AAE1, 0x68FD24);
#endif
}

void VisualPage::Draw()
{
    if (ImGui::BeginTabBar("Visual", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT( "Window.ToggleTab")))
        {
            ImGui::BeginChild("VisualCHild");
            ImGui::Spacing();
            ImGui::Columns(2, nullptr, false);
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Visual.ArmourBorder"), 0x589123);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.ArmourPercentage"), 0x589125);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.BreathBorder"), 0x589207);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.BreathPercentage"), 0x589209);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.CCTVEffect"), 0xC402C5);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.DarknessFilter"), 0xC402C4);
            if (Widget::Toggle(TEXT("Visual.DisableHydrant"), &m_bDisableHydrant))
            {
                if (m_bDisableHydrant)
                {
                    // don't call Fx_c::TriggerWaterHydrant
                    patch::Nop(0x4A0D70, 5);
                }
                else
                {
                    patch::SetRaw(0x4A0D70, (char*)"\xE9\x94\x3F\xF6\xFF", 5);
                }
            }
            Widget::ToggleAddr<int8_t>(TEXT("Visual.FogEffect"), 0xC402C6);
            if (Widget::Toggle(TEXT("Visual.FullscreenMap"), &m_bFullScreenMap, TEXT("Visual.FullscreenMapTip")))
            {
                if (m_bFullScreenMap)
                {
                    // NOP CSprite2d::DrawRect calls
                    patch::Nop(0x575BF6, 5);
                    patch::Nop(0x575C40, 5);
                    patch::Nop(0x575C84, 5);
                    patch::Nop(0x575CCE, 5);
                    patch::Nop(0x575D1F, 5);
                    patch::Nop(0x575D6F, 5);
                    patch::Nop(0x575DC2, 5);
                    patch::Nop(0x575E12, 5);

                    // Remove screen width height scaling
                    patch::Nop(0x5754EC, 6);
                    patch::Nop(0x575537, 6);
                    patch::Nop(0x575311, 6);
                    patch::Nop(0x575361, 6);
                }
                else
                {
                    // restore
                    patch::SetRaw(0x575BF6, (char*)"\xE8\x65\x1F\x1B\x00", 5);
                    patch::SetRaw(0x575C40, (char*)"\xE8\x1B\x1F\x1B\x00", 5);
                    patch::SetRaw(0x575C84, (char*)"\xE8\xD7\x1E\x1B\x00", 5);
                    patch::SetRaw(0x575CCE, (char*)"\xE8\x8D\x1E\x1B\x00", 5);
                    patch::SetRaw(0x575D1F, (char*)"\xE8\x3C\x1E\x1B\x00", 5);
                    patch::SetRaw(0x575D6F, (char*)"\xE8\xEC\x1D\x1B\x00", 5);
                    patch::SetRaw(0x575DC2, (char*)"\xE8\x99\x1D\x1B\x00", 5);
                    patch::SetRaw(0x575E12, (char*)"\xE8\x49\x1D\x1B\x00", 5);
                    patch::SetRaw(0x5754EC, (char*)"\xD8\x0D\x20\x95\x85\x00", 6);
                    patch::SetRaw(0x575537, (char*)"\xD8\x0D\x24\x95\x85\x00", 6);
                    patch::SetRaw(0x575311, (char*)"\xD8\x0D\x70\x53\x86\x00", 6);
                    patch::SetRaw(0x575361, (char*)"\xD8\x0D\x6C\x53\x86\x00", 6);
                }
            }
            Widget::ToggleAddr<int8_t>(TEXT("Visual.GrainEffect"), 0xC402B4);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.GrayRadar"), 0xA444A4);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.HealthBorder"), 0x589353);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.HealthPercentage"), 0x589355);

            Widget::ToggleAddr<int8_t>(TEXT("Visual.HeatHazeEffect"), 0xC402BA);

            if (Widget::Toggle(TEXT("Visual.HideAreaNames"), &CHud::bScriptDontDisplayAreaName))
            {
                Command<Commands::DISPLAY_ZONE_NAMES>(!CHud::bScriptDontDisplayAreaName);
            }

            ImGui::NextColumn();

            if (Widget::Toggle(TEXT("Visual.HideVehNames"), &CHud::bScriptDontDisplayVehicleName))
            {
                Command<Commands::DISPLAY_CAR_NAMES>(!CHud::bScriptDontDisplayVehicleName);
            }

            Widget::ToggleAddr<int8_t>(TEXT("Visual.HideWantedLevel"), 0x58DD1B, "", 0x90, 0x7E);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.InfraredVision"), 0xC402B9);
            if (Widget::Toggle(TEXT("Visual.InvisibleWater"), &m_bInvisibleWater))
            {
                if (!m_bNoWater)
                {
                    if (m_bInvisibleWater)
                    {
                        // don't call CWaterLevel::RenderWater()
                        patch::Nop(0x53E004, 5);
                        patch::Nop(0x53E142, 5);
                    }
                    else
                    {
                        // restore call CWaterLevel::RenderWater()
                        patch::SetRaw(0x53E004, (char*)"\xE8\x47\x16\x1B\x00", 5);
                        patch::SetRaw(0x53E142, (char*)"\xE8\x09\x15\x1B\x00", 5);
                    }
                }
            }
            Widget::Toggle(TEXT("Visual.LockWeather"), &m_bLockWeather);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.NightVision"), 0xC402B8);
            if (Widget::Toggle(TEXT("Visual.NoMoneyZeros"), &m_bNoMoneyZeros))
            {
                static const char *pos = "$%d", *neg = "-$%d";
                if(m_bNoMoneyZeros)
                {
                    patch::Set<const char*>(0x58F4C8, pos, true); //positive
                    patch::Set<const char*>(0x58F50A, neg, true); //negative
                }
                else
                {
                    patch::SetRaw(0x58F4C8, (void*)"\x94\x6C\x86\x00", 4);
                    patch::SetRaw(0x58F50A, (void*)"\x8C\x6C\x86\x00", 4);
                }
            }
            if (Widget::Toggle(TEXT("Visual.NoParticles"), &m_bNoPartciles))
            {
                if(m_bNoPartciles)
                {
                    patch::Set<uint32_t>(0x4AA440, 0x000020C2, true);
                }
                else
                {
                    patch::Set<uint32_t>(0x4AA440, 0x5608EC83, true);
                }
            }
            Widget::ToggleAddr<int8_t>(TEXT("Visual.NoPostFX"), 0xC402CF);
            if (Widget::Toggle(TEXT("Visual.NoRadarRot"), &m_bNoRadarRot))
            {
                // Credits: jeremii (bjeremii.blogspot.com)
                if (m_bNoRadarRot)
                {
                    patch::Set<float>(0xBA8310, 0.0);
                    patch::Set<float>(0xBA830C, 0.0);
                    patch::Set<float>(0xBA8308, 1.0);

                    // stop map rotation
                    patch::Nop(0x5837FB, 6);
                    patch::Nop(0x583805, 6);
                    patch::Nop(0x58380D, 6);
                    patch::Nop(0x5837D6, 6);
                    patch::Nop(0x5837D0, 6);
                    patch::Nop(0x5837C6, 8);
                }
                else
                {
                    patch::SetRaw(0x5837FB, (void*)"\xD9\x15\x10\x83\xBA\x00", 6);
                    patch::SetRaw(0x583805, (void*)"\xD9\x1D\x0C\x83\xBA\x00", 6);
                    patch::SetRaw(0x58380D, (void*)"\xD9\x1D\x08\x83\xBA\x00", 6);
                    patch::SetRaw(0x5837D6, (void*)"\xD9\x1D\x10\x83\xBA\x00", 6);
                    patch::SetRaw(0x5837D0, (void*)"\xD9\x1D\x08\x83\xBA\x00", 6);
                    patch::SetRaw(0x5837C6, (void*)"\xD9\x1D\x0C\x83\xBA\x00\xD9\xC0", 8);
                }
            }
            if (Widget::Toggle(TEXT("Visual.NoWater"), &m_bNoWater))
            {
                if (m_bNoWater)
                {
                    // don't call CWaterLevel::RenderWater()
                    patch::Nop(0x53E004, 5);
                    patch::Nop(0x53E142, 5);

                    // rtn CWaterLevel::GetWaterLevelNoWaves
                    patch::SetRaw(0x6E8580, (char*)"\x32\xC0\xC3", 3);
                }
                else
                {
                    // restore call CWaterLevel::RenderWater()
                    patch::SetRaw(0x53E004, (char*)"\xE8\x47\x16\x1B\x00", 5);
                    patch::SetRaw(0x53E142, (char*)"\xE8\x09\x15\x1B\x00", 5);

                    // restore CWaterLevel::GetWaterLevelNoWaves
                    patch::SetRaw(0x6E8580, (char*)"\x51\xD9\x44", 3);
                }
            }
            bool radar_state = (patch::Get<BYTE>(0xBA676C) != 2);
            if (Widget::Toggle(TEXT("Visual.ShowRadar"), &radar_state))
            {
                patch::Set<BYTE>(0xBA676C, radar_state == true ? 0 : 2);
            }

            Widget::ToggleAddr<int8_t>(TEXT("Visual.ShowHud"), 0xBA6769);
            if (Widget::Toggle(TEXT("Visual.SquareRadar"), &m_bSquareRadar))
            {
                if (m_bSquareRadar)
                {
                    static float var = 0.000001f;
                    static CSprite2d sprite;
                    sprite.m_pTexture =  gTextureList.FindRwTextureByName("radardisc");

                    // rediect to our texture
                    patch::Set(0x58A8C9, &sprite);
                    patch::Set(0x58A973, &sprite);
                    patch::Set(0x58AA21, &sprite);
                    patch::Set(0x58A81F, &sprite);

                    patch::Set(0x58585C, &var);
                    patch::ReplaceFunction(0x401EC5, &LimitRadarPoint);
                }
                else
                {
                    patch::Set(0x58A8C9, 0xBAB208);
                    patch::Set(0x58A973, 0xBAB208);
                    patch::Set(0x58AA21, 0xBAB208);
                    patch::Set(0x58A81F, 0xBAB208);
                    // Only has 1 image atm
                    patch::SetRaw(0x58585C, (void*)"\x1C\x8F\x85\x00", 4);
                    patch::SetRaw(0x401EC5, (void*)"\xA0\xA1\x67\xBA\x00", 5);
                }
            }

            Widget::ToggleAddr<int8_t>(TEXT("Visual.UnderwaterEffect"), 0xC402D3);
            Widget::ToggleAddr<int>(TEXT("Visual.UnfogMap"), 0xBA372C, TEXT("Visual.UnfogMapText") , 0x50, 0x0);
#elif GTAVC
            Widget::ToggleAddr<int8_t>(TEXT("Visual.HideRadar"), 0xA10AB6);
            Widget::Toggle(TEXT("Visual.LockWeather"), &m_bLockWeather);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.ShowHud"), 0x86963A);

            ImGui::NextColumn();

            Widget::ToggleAddr<int8_t>(TEXT("Visual.GreenScanlines"), 0xA10B69);
            Widget::ToggleAddr<int8_t>(TEXT("Visual.WhiteScanlines"), 0xA10B68);
#else
            static bool hideHud, hideRadar;
            if (Widget::Toggle(TEXT("Visual.HideHud"), &hideHud))
            {
                if (hideHud)
                {
                    patch::Nop(0x48E420, 5); // CHud::Draw
                }
                else
                {
                    patch::SetRaw(0x48E420, (char*)"\xE8\x7B\x6E\x07\x00", 5);
                }
            }
            if (Widget::Toggle(TEXT("Visual.HideRadar"), &hideRadar))
            {
                if (hideHud)
                {
                    patch::Nop(0x50838D, 5); // CRadar::Draw
                }
                else
                {
                    patch::SetRaw(0x50838D, (char*)"\xE8\x6E\xBE\xF9\xFF", 5);
                }
            }
            Widget::Toggle(TEXT("Visual.LockWeather"), &m_bLockWeather);
#endif
            ImGui::Columns(1);
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Window.MenusTab")))
        {
            static bool initPatches = false;
            if (!initPatches)
            {
                PatchRadar();
                initPatches = true;
            }

            ImGui::Spacing();
            ImGui::SameLine();
            ImGui::TextWrapped(TEXT("Visual.IncompatibleMods"));
            Widget::Tooltip(TEXT("Visual.IncompatibleModsText"));
            ImGui::Spacing();

            if (ImGui::BeginChild("VisualsChild"))
            {
#ifdef GTASA
                ColorPickerAddr(TEXT("Visual.ArmourbarColor"), *(int*)0x5890FC, ImVec4(225, 225, 225, 255));
                Widget::EditAddr<float>(TEXT("Visual.ArmourbarPosX"), 0x866B78, -999, 94, 999);
                Widget::EditAddr<float>(TEXT("Visual.ArmourbarPosY"), 0x862D38, -999, 48, 999);
                ColorPickerAddr(TEXT("Visual.BreathbarColor"), *(int*)0x5891EB, ImVec4(172, 203, 241, 255));
                Widget::EditAddr<float>(TEXT("Visual.BreathbarPosX"), *(int*)0x58F11F, -999, 94, 999);
                Widget::EditAddr<float>(TEXT("Visual.BreathbarPosY"), *(int*)0x58F100, -999, 62, 999);
                ColorPickerAddr(TEXT("Visual.ClockColor"), *(int*)0x58EBD1, ImVec4(255, 255, 255, 255));
                Widget::EditAddr<float>(TEXT("Visual.ClockPosX"), *(int*)0x58EC16, -999, 32, 999);
                Widget::EditAddr<float>(TEXT("Visual.ClockPosY"), *(int*)0x58EC04, -999, 22, 999);
                ColorPickerAddr(TEXT("Visual.HealthbarColor"), *(int*)0x589331, ImVec4(180, 25, 29, 255));
                Widget::EditAddr<float>(TEXT("Visual.HealthbarPosX"), 0x86535C, -999, 141, 999);
                Widget::EditAddr<float>(TEXT("Visual.HealthbarPosY"), 0x866CA8, -999, 77, 999);
                ColorPickerAddr(TEXT("Visual.DrawMenuTitle"), 0xBAB240, ImVec4(0, 0, 0, 255));
                ColorPickerAddr(TEXT("Visual.MoneyColor"), 0xBAB230, ImVec4(54, 104, 44, 255));
                Widget::EditAddr<float>(TEXT("Visual.MoneyPosX"), *(int*)0x58F5FC, -999, 32, 999);
                Widget::EditAddr<float>(TEXT("Visual.MoneyPosY"), 0x866C88, -999, 89, 999);
                static std::vector<Widget::BindInfo> font_outline
                {
                    {TEXT("Visual.NoOutline"), 0}, {TEXT("Visual.ThinOutline"), 1}, {TEXT("Visual.DefaultOutline"), 2}
                };
                Widget::EditRadioBtnAddr(TEXT("Visual.MoneyFontOutline"), 0x58F58D, font_outline);
                static std::vector<Widget::BindInfo> style
                {
                    {TEXT("Visual.Style1"), 1}, {TEXT("Visual.Style2"), 2}, {TEXT("Visual.DefaultStyle"), 3}
                };
                Widget::EditRadioBtnAddr(TEXT("Visual.MoneyFontStyle"), 0x58F57F, style);
                Widget::EditAddr<float>(TEXT("Visual.RadarHeight"), *(int*)0x5834F6, 0, 76, 999);
                Widget::EditAddr<float>(TEXT("Visual.RadarWidth"), *(int*)0x5834C2, 0, 94, 999);
                Widget::EditAddr<float>(TEXT("Visual.RadarPosX"), *(int*)0x5834D4, -999, 40, 999);
                Widget::EditAddr<float>(TEXT("Visual.RadarPosY"), *(int*)0x583500, -999, 104, 999);
                Widget::EditAddr<int>(TEXT("Visual.RadarZoom"), 0xA444A3, 0, 0, 170);
                ColorPickerAddr(TEXT("Visual.RadioStationColor"), 0xBAB24C, ImVec4(150, 150, 150, 255));

                static std::vector<Widget::BindInfo> star_border
                {
                    {TEXT("Visual.NoBorder"), 0}, {TEXT("Visual.DefaultBorder"), 1}, {TEXT("Visual.BoldBorder"), 2}
                };
                Widget::EditRadioBtnAddr(TEXT("Visual.WantedStarBorder"), 0x58DD41, star_border);
                Widget::EditAddr<float>(TEXT("Visual.WantedPosX"), *(int*)0x58DD0F, -999, 29, 999);
                Widget::EditAddr<float>(TEXT("Visual.WantedPosY"), *(int*)0x58DDFC, -999, 114, 999);
                Widget::EditAddr<float>(TEXT("Visual.WeaponAmmoPosX"), *(int*)0x58FA02, -999, 32, 999);
                Widget::EditAddr<float>(TEXT("Visual.WeaponAmmoPosY"), *(int*)0x58F9E6, -999, 43, 999);
                Widget::EditAddr<float>(TEXT("Visual.WeaponIconPosX"), *(int*)0x58F927, -999, 32, 999);
                Widget::EditAddr<float>(TEXT("Visual.WeaponIconPosY"), *(int*)0x58F913, -999, 20, 999);
#elif GTAVC
                static float discLeft = *(float*)0x55A956;
                static float discRight = *(float*)*(int*)0x55A9AE;
                static float &posX = *(float*)0x68FD2C;
                patch::SetPointer(0x55A9AE, &discRight);
                patch::SetPointer(0x55AAE7, &discRight);
                patch::SetFloat(0x55A956, discLeft);
                patch::SetFloat(0x55AA94, discLeft);

                float prevVal = posX;
                Widget::EditAddr<float>(TEXT("Visual.RadarPosX"), (uint)&posX, -999, 40, 999);
                float diff = posX - prevVal;
                discLeft += diff;
                discRight += diff;

                Widget::EditAddr<float>(TEXT("Visual.RadarPosY"), 0x68FD34, -999, 116, 999);
                Widget::EditAddr<float>(TEXT("Visual.RadarHeight"), 0x68FD30, -999, 76, 999);
                Widget::EditAddr<float>(TEXT("Visual.RadarWidth"), 0x68FD24, -999, 94, 999);
#endif

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }

#ifdef GTASA
        if (m_nTimecycHour == 8 ? ImGui::BeginTabItem(TEXT( "Visual.TimecycTab")) 
        : ImGui::BeginTabItem(TEXT( "Visual.Timecyc24hTab")))
#else
        if (ImGui::BeginTabItem(TEXT( "Visual.TimecycTab")))
#endif
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Visual.GenerateFile"), Widget::CalcSize(2)))
            {
                GenerateTimecycFile();
                Util::SetMessage(TEXT("Visual.FileGenerated"));
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Visual.ResetTimecyc"), Widget::CalcSize(2)))
            {
                CTimeCycle::Initialise();
                Util::SetMessage(TEXT("Visual.TimecycReset"));
            }
            ImGui::Spacing();

            ImGui::Spacing();
            if (ImGui::BeginTabBar("Timecyc subtab", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
            {
                if (ImGui::BeginTabItem(TEXT( "Visual.ColorsTab")))
                {
                    ImGui::BeginChild("TimecycColors");
                    ImGui::Spacing();

                    TimeCycColorEdit3(TEXT("Visual.Ambient"), m_nAmbientRed, m_nAmbientGreen, m_nAmbientBlue);
#ifndef GTA3
                    TimeCycColorEdit3(TEXT("Visual.AmbientObj"), m_nAmbientRed_Obj, m_nAmbientGreen_Obj, m_nAmbientBlue_Obj);
#endif

#ifdef GTASA
                    TimeCycColorEdit3(TEXT("Visual.FluffyClouds"), m_nFluffyCloudsBottomRed, m_nFluffyCloudsBottomGreen,
                                      m_nFluffyCloudsBottomBlue);
#elif GTAVC
                    TimeCycColorEdit3(TEXT("Visual.AmbientBl"), m_nAmbientBlRed, m_nAmbientBlGreen, m_nAmbientBlBlue);
                    TimeCycColorEdit3(TEXT("Visual.AmbientObjBl"), m_nAmbientBlRed_Obj, m_nAmbientBlGreen_Obj,
                                      m_nAmbientBlBlue_Obj);
                    TimeCycColorEdit3(TEXT("Visual.Blur"), m_nBlurRed, m_nBlurGreen, m_nBlurBlue);
#endif

#ifndef GTASA
                    TimeCycColorEdit3(TEXT("Visual.CloudsBottom"), m_nBottomCloudsRed, m_nBottomCloudsGreen, m_nBottomCloudsBlue);
                    TimeCycColorEdit3(TEXT("Visual.CloudsLow"), m_nLowCloudsRed, m_nLowCloudsGreen, m_nLowCloudsBlue);
                    TimeCycColorEdit3(TEXT("Visual.CloudsTop"), m_nTopCloudsRed, m_nTopCloudsGreen, m_nTopCloudsBlue);
                    TimeCycColorEdit3(TEXT("Visual.DirectionalLight"), m_nDirRed, m_nDirGreen, m_nDirBlue);
#endif

#ifdef GTASA
                    TimeCycColorEdit4(TEXT("Visual.PostFX1"), m_fPostFx1Red, m_fPostFx1Green, m_fPostFx1Blue, m_fPostFx1Alpha);
                    TimeCycColorEdit4(TEXT("Visual.PostFX2"), m_fPostFx2Red, m_fPostFx2Green, m_fPostFx2Blue, m_fPostFx1Alpha);
#elif GTA3
                    TimeCycColorEdit4(TEXT("Visual.PostFX1"), m_fPostFxRed, m_fPostFxGreen, m_fPostFxBlue, m_fPostFxAlpha);
#endif

                    TimeCycColorEdit3(TEXT("Visual.SkyBottom"), m_nSkyBottomRed, m_nSkyBottomGreen, m_nSkyBottomBlue);
                    TimeCycColorEdit3(TEXT("Visual.SunCore"), m_nSunCoreRed, m_nSunCoreGreen, m_nSunCoreBlue);
                    TimeCycColorEdit3(TEXT("Visual.SunCorona"), m_nSunCoronaRed, m_nSunCoronaGreen, m_nSunCoronaBlue);
                    TimeCycColorEdit3(TEXT("Visual.SkyTop"), m_nSkyTopRed, m_nSkyTopGreen, m_nSkyTopBlue);

#ifndef GTA3
                    TimeCycColorEdit4(TEXT("Visual.Water"), m_fWaterRed, m_fWaterGreen, m_fWaterBlue, m_fWaterAlpha);
#endif
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(TEXT( "Visual.Miscellaneous")))
                {
                    ImGui::BeginChild("TimecycMisc");
                    ImGui::Spacing();
                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);
                    int weather = CWeather::OldWeatherType;
                    if (Widget::ListBox(TEXT("Visual.CurrentWeather"), m_WeatherNames, weather))
                    {
                        CWeather::OldWeatherType = weather;
                    }

                    weather = CWeather::NewWeatherType;
                    if (Widget::ListBox(TEXT("Visual.NextWeather"), m_WeatherNames, weather))
                    {
                        CWeather::NewWeatherType = weather;
                    }

                    ImGui::Spacing();
                    int hour = CClock::ms_nGameClockHours;
                    int minute = CClock::ms_nGameClockMinutes;

                    if (gamePage.m_bSyncTime)
                    {
                        ImGui::BeginDisabled(gamePage.m_bSyncTime);
                    }

                    if (ImGui::InputInt(TEXT("Visual.Hour"), &hour) & !gamePage.m_bSyncTime)
                    {
                        if (hour < 0) hour = 23;
                        if (hour > 23) hour = 0;
                        CClock::ms_nGameClockHours = hour;
                    }

                    if (ImGui::InputInt(TEXT("Visual.Minute"), &minute) & !gamePage.m_bSyncTime)
                    {
                        if (minute < 0) minute = 59;
                        if (minute > 59) minute = 0;
                        CClock::ms_nGameClockMinutes = minute;
                    }

                    if (gamePage.m_bSyncTime)
                    {
                        ImGui::EndDisabled();
                        Widget::Tooltip(TEXT("Visual.SyncTimeEnabled"));
                    }

                    if (Widget::Toggle(TEXT("Visual.FreezeGameTime"), &gamePage.m_bFreezeTime))
                    {
                        if (gamePage.m_bFreezeTime)
                        {
                            patch::SetRaw(BY_GAME(0x52CF10, 0x487010, 0x473460), (char *)"\xEB\xEF", 2);
                        }
                        else
                        {
                            patch::SetRaw(BY_GAME(0x52CF10, 0x487010, 0x473460),
                                            (char *)BY_GAME("\x56\x8B", "\x6A\x01", "\x6A\x01"), 2);
                        }
                    }
                    ImGui::Spacing();
#ifdef GTASA
                    TimecycSlider(TEXT("Visual.CloudAlpha"), m_fCloudAlpha, 0, 255);
                    TimecycSlider(TEXT("Visual.DirectionalMult"), m_nDirectionalMult, 0, 255);
#endif
                    TimecycSlider(TEXT("Visual.FarClip"), m_fFarClip, 0, 5000);
                    TimecycSlider(TEXT("Visual.FogStart"), m_fFogStart, 0, 5000);
#ifdef GTASA
                    TimecycSlider(TEXT("Visual.LightIntensity"), m_nHighLightMinIntensity, 0, 255);
#endif
                    TimecycSlider(TEXT("Visual.LightBrightness"), m_fLightsOnGroundBrightness, 0, 255);
                    TimecycSlider(TEXT("Visual.LightShadowStrength"), m_nLightShadowStrength, 0, 255);

#ifndef GTA3
                    TimecycSlider(TEXT("Visual.PoleShadowStrength"), m_nPoleShadowStrength, 0, 255);
#endif
                    TimecycSlider(TEXT("Visual.ShadowStrength"), m_nShadowStrength, 0, 255);
                    TimecycSlider(TEXT("Visual.SpriteBrightness"), m_fSpriteBrightness, 0, 127);
                    TimecycSlider(TEXT("Visual.SpriteSize"), m_fSpriteSize, 0, 127);
                    TimecycSlider(TEXT("Visual.SunSize"), m_fSunSize, 0, 127);

#ifdef GTA3
                    TimecycSlider(TEXT("Visual.TreeShadowStrength"), m_nTreeShadowStrength, 0, 255);
#endif

#ifdef GTASA
                    TimecycSlider(TEXT("Visual.WaterFogAlpha"), m_nWaterFogAlpha, 0, 255);
#endif
                    ImGui::PopItemWidth();

                    ImGui::Spacing();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
