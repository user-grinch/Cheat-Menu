#include "pch.h"
#include "visual.h"
#include "ui.h"
#include "widget.h"
#include "util.h"
#include "game.h"
#include "timecycle.h"

#ifdef GTASA
#include "CHudColours.h"
#endif

#ifdef GTASA
#define TOTAL_WEATHERS 23
#elif GTAVC
#define TOTAL_WEATHERS 7
#else
#define TOTAL_WEATHERS 4
#endif

void Visual::Init()
{
#ifdef GTASA
    if (GetModuleHandle("timecycle24.asi"))
    {
        m_nTimecycHour = 24;
    }
#endif

    Events::processScriptsEvent += []
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

void Visual::GenerateTimecycFile()
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

int Visual::CalcArrayIndex()
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


template<typename T>
bool Visual::TimeCycColorEdit3(const char* label, T* r, T* g, T* b, ImGuiColorEditFlags flags)
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
void Visual::TimecycSlider(const char* label, T* ptr, int min, int max)
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
bool Visual::TimeCycColorEdit4(const char* label, T* r, T* g, T* b, T* a, ImGuiColorEditFlags flags)
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

void Visual::ShowPage()
{
    if (ImGui::BeginTabBar("Visual", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::Spacing();
            ImGui::Columns(2, nullptr, false);

#ifdef GTASA
            Ui::CheckboxAddress(TEXT("Visual.ArmourBorder"), 0x589123);
            Ui::CheckboxAddress(TEXT("Visual.ArmourPercentage"), 0x589125);
            Ui::CheckboxAddress(TEXT("Visual.BreathBorder"), 0x589207);
            Ui::CheckboxAddress(TEXT("Visual.BreathPercentage"), 0x589209);
            Ui::CheckboxAddress(TEXT("Visual.CCTVEffect"), 0xC402C5);
            Ui::CheckboxAddress(TEXT("Visual.DarknessFilter"), 0xC402C4);
            if (Ui::CheckboxWithHint(TEXT("Visual.DisableHydrant"), &m_bDisableHydrant))
            {
                if (m_bDisableHydrant)
                {
                    // don't call Fx_c::TriggerWaterHydrant
                    plugin::patch::Nop(0x4A0D70, 5);
                }
                else
                {
                    plugin::patch::SetRaw(0x4A0D70, (char*)"\xE9\x94\x3F\xF6\xFF", 5);
                }
            }
            Ui::CheckboxAddress(TEXT("Visual.FogEffect"), 0xC402C6);
            Ui::CheckboxAddress(TEXT("Visual.GrainEffect"), 0xC402B4);
            Ui::CheckboxAddress(TEXT("Visual.GrayRadar"), 0xA444A4);
            Ui::CheckboxAddress(TEXT("Visual.HealthBorder"), 0x589353);
            Ui::CheckboxAddress(TEXT("Visual.HealthPercentage"), 0x589355);

            Ui::CheckboxAddress(TEXT("Visual.HeatHazeEffect"), 0xC402BA);

            if (Ui::CheckboxWithHint(TEXT("Visual.HideAreaNames"), &CHud::bScriptDontDisplayAreaName))
            {
                Command<Commands::DISPLAY_ZONE_NAMES>(!CHud::bScriptDontDisplayAreaName);
            }

            ImGui::NextColumn();

            if (Ui::CheckboxWithHint(TEXT("Visual.HideVehNames"), &CHud::bScriptDontDisplayVehicleName))
            {
                Command<Commands::DISPLAY_CAR_NAMES>(!CHud::bScriptDontDisplayVehicleName);
            }

            Ui::CheckboxAddressEx(TEXT("Visual.HideWantedLevel"), 0x58DD1B, 0x90, 1);
            Ui::CheckboxAddress(TEXT("Visual.InfraredVision"), 0xC402B9);
            if (Ui::CheckboxWithHint(TEXT("Visual.InvisibleWater"), &m_bInvisibleWater))
            {
                if (!m_bNoWater)
                {
                    if (m_bInvisibleWater)
                    {
                        // don't call CWaterLevel::RenderWater()
                        plugin::patch::Nop(0x53E004, 5);
                        plugin::patch::Nop(0x53E142, 5);
                    }
                    else
                    {
                        // restore call CWaterLevel::RenderWater()
                        plugin::patch::SetRaw(0x53E004, (char*)"\xE8\x47\x16\x1B\x00", 5);
                        plugin::patch::SetRaw(0x53E142, (char*)"\xE8\x09\x15\x1B\x00", 5);
                    }
                }
            }
            Ui::CheckboxWithHint(TEXT("Visual.LockWeather"), &m_bLockWeather);
            Ui::CheckboxAddress(TEXT("Visual.NightVision"), 0xC402B8);
            if (Ui::CheckboxWithHint(TEXT("Visual.NoMoneyZeros"), &m_bNoMoneyZeros))
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
            if (Ui::CheckboxWithHint(TEXT("Visual.NoParticles"), &m_bNoPartciles))
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
            Ui::CheckboxAddress(TEXT("Visual.NoPostFX"), 0xC402CF);
           
            if (Ui::CheckboxWithHint(TEXT("Visual.NoWater"), &m_bNoWater))
            {
                if (m_bNoWater)
                {
                    // don't call CWaterLevel::RenderWater()
                    plugin::patch::Nop(0x53E004, 5);
                    plugin::patch::Nop(0x53E142, 5);

                    // rtn CWaterLevel::GetWaterLevelNoWaves
                    plugin::patch::SetRaw(0x6E8580, (char*)"\x32\xC0\xC3", 3);
                }
                else
                {
                    // restore call CWaterLevel::RenderWater()
                    plugin::patch::SetRaw(0x53E004, (char*)"\xE8\x47\x16\x1B\x00", 5);
                    plugin::patch::SetRaw(0x53E142, (char*)"\xE8\x09\x15\x1B\x00", 5);

                    // restore CWaterLevel::GetWaterLevelNoWaves
                    plugin::patch::SetRaw(0x6E8580, (char*)"\x51\xD9\x44", 3);
                }
            }

            bool radar_state = (patch::Get<BYTE>(0xBA676C) != 2);
            if (Ui::CheckboxWithHint(TEXT("Visual.ShowRadar"), &radar_state))
            {
                patch::Set<BYTE>(0xBA676C, radar_state == true ? 0 : 2);
            }

            Ui::CheckboxAddress(TEXT("Visual.ShowHud"), 0xBA6769);
            Ui::CheckboxAddress(TEXT("Visual.UnderwaterEffect"), 0xC402D3);
            Ui::CheckboxAddressEx(TEXT("Visual.UnfogMap"), 0xBA372C, 0x50, 0x0, TEXT("Visual.UnfogMapText"));
#elif GTAVC
            Ui::CheckboxAddress(TEXT("Visual.HideRadar"), 0xA10AB6);
            Ui::CheckboxWithHint(TEXT("Visual.Lockweather"), &m_bLockWeather);
            Ui::CheckboxAddress(TEXT("Visual.ShowHud"), 0x86963A);

            ImGui::NextColumn();

            Ui::CheckboxAddress(TEXT("Visual.GreenScanlines"), 0xA10B69);
            Ui::CheckboxAddress(TEXT("Visual.WhiteScanlines"), 0xA10B68);
#else
            static bool hideHud, hideRadar;
            if (Ui::CheckboxWithHint(TEXT("Visual.HideHud"), &hideHud))
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
            if (Ui::CheckboxWithHint(TEXT("Visual.HideRadar"), &hideRadar))
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
            Ui::CheckboxWithHint(TEXT("Visual.LockWeather"), &m_bLockWeather);
#endif
            ImGui::Columns(1);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
        {
#ifdef GTASA
            static bool init_patches = false;
            static float clock_posX = *(float*)*(int*)0x58EC16;
            static float clock_posY = *(float*)*(int*)0x58EC04;
            static float radar_posX = *(float*)*(int*)0x5834D4;
            static float radar_posY = *(float*)*(int*)0x583500;
            static float radar_width = *(float*)*(int*)0x5834C2;
            static float radar_height = *(float*)*(int*)0x5834F6;
            static CHudColour armour_bar = HudColour.m_aColours[4];
            static CHudColour clock_bar = HudColour.m_aColours[4];
            static CHudColour health_bar = HudColour.m_aColours[0];
            static CHudColour breath_bar = HudColour.m_aColours[3];
            static CHudColour wanted_bar = HudColour.m_aColours[6];
            static float money_posX = *(float*)*(int*)0x58F5FC;
            static float breath_posX = *(float*)*(int*)0x58F11F;
            static float breath_posY = *(float*)*(int*)0x58F100;
            static float weapon_icon_posX = *(float*)*(int*)0x58F927;
            static float weapon_icon_posY = *(float*)*(int*)0x58F913;
            static float weapon_ammo_posX = *(float*)*(int*)0x58FA02;
            static float weapon_ammo_posY = *(float*)*(int*)0x58F9E6;
            static float wanted_posX = *(float*)*(int*)0x58DD0F;

            if (!init_patches)
            {
                patch::SetPointer(0x58EC16, &clock_posX);
                patch::SetPointer(0x58EC04, &clock_posY);
                patch::SetPointer(0x5834D4, &radar_posX);
                patch::SetPointer(0x583500, &radar_posY);
                patch::SetPointer(0x5834F6, &radar_height);
                patch::SetPointer(0x5834C2, &radar_width);

                patch::SetPointer(0x58A79B, &radar_posX);
                patch::SetPointer(0x58A7C7, &radar_posY);
                patch::SetPointer(0x58A801, &radar_height);
                patch::SetPointer(0x58A7E9, &radar_width);
                patch::SetPointer(0x58A836, &radar_posX);
                patch::SetPointer(0x58A868, &radar_posY);
                patch::SetPointer(0x58A8AB, &radar_height);
                patch::SetPointer(0x58A840, &radar_width);
                patch::SetPointer(0x58A8E9, &radar_posX);
                patch::SetPointer(0x58A913, &radar_posY);
                patch::SetPointer(0x58A921, &radar_height);
                patch::SetPointer(0x58A943, &radar_width);
                patch::SetPointer(0x58A98A, &radar_posX);
                patch::SetPointer(0x58A9C7, &radar_posY);
                patch::SetPointer(0x58A9D5, &radar_height);
                patch::SetPointer(0x58A99D, &radar_width);

                patch::SetPointer(0x5890FC, &armour_bar);
                patch::SetChar(0x5890F5, 0);
                patch::SetPointer(0x589331, &health_bar);
                patch::SetPointer(0x5891EB, &breath_bar);
                patch::SetChar(0x5891E4, 0);
                patch::SetPointer(0x58EBD1, &clock_bar);
                patch::SetChar(0x58EBCA, 0);

                patch::SetPointer(0x58F5FC, &money_posX);
                patch::SetPointer(0x58F11F, &breath_posX);
                patch::SetPointer(0x58F100, &breath_posY);
                patch::SetPointer(0x58DD0F, &wanted_posX);
                patch::SetPointer(0x58F927, &weapon_icon_posX);
                patch::SetPointer(0x58F913, &weapon_icon_posY);
                patch::SetPointer(0x58FA02, &weapon_ammo_posX);
                patch::SetPointer(0x58F9E6, &weapon_ammo_posY);

                init_patches = true;
            }
#endif
            if (ImGui::BeginChild("VisualsChild"))
            {
#ifdef GTASA
                ImGui::Spacing();
                ImGui::SameLine();
                ImGui::TextWrapped(TEXT("Visual.IncompatibleMods"));
                Widget::Tooltip(TEXT("Visual.IncompatibleModsText"));
                ImGui::Spacing();
                Ui::ColorPickerAddress(TEXT("Visual.ArmourbarColor"), *(int*)0x5890FC, ImVec4(225, 225, 225, 255));
                Ui::EditAddress<float>(TEXT("Visual.ArmourbarPosX"), 0x866B78, -999, 94, 999);
                Ui::EditAddress<float>(TEXT("Visual.ArmourbarPosY"), 0x862D38, -999, 48, 999);
                Ui::ColorPickerAddress(TEXT("Visual.BreathbarColor"), *(int*)0x5891EB, ImVec4(172, 203, 241, 255));
                Ui::EditAddress<float>(TEXT("Visual.BreathbarPosX"), *(int*)0x58F11F, -999, 94, 999);
                Ui::EditAddress<float>(TEXT("Visual.BreathbarPosY"), *(int*)0x58F100, -999, 62, 999);
                Ui::ColorPickerAddress(TEXT("Visual.ClockColor"), *(int*)0x58EBD1, ImVec4(255, 255, 255, 255));
                Ui::EditAddress<float>(TEXT("Visual.ClockPosX"), *(int*)0x58EC16, -999, 32, 999);
                Ui::EditAddress<float>(TEXT("Visual.ClockPosY"), *(int*)0x58EC04, -999, 22, 999);
                Ui::ColorPickerAddress(TEXT("Visual.HealthbarColor"), *(int*)0x589331, ImVec4(180, 25, 29, 255));
                Ui::EditAddress<float>(TEXT("Visual.HealthbarPosX"), 0x86535C, -999, 141, 999);
                Ui::EditAddress<float>(TEXT("Visual.HealthbarPosY"), 0x866CA8, -999, 77, 999);
                Ui::ColorPickerAddress(TEXT("Visual.DrawMenuTitle"), 0xBAB240, ImVec4(0, 0, 0, 255));
                Ui::ColorPickerAddress(TEXT("Visual.MoneyColor"), 0xBAB230, ImVec4(54, 104, 44, 255));
                Ui::EditAddress<float>(TEXT("Visual.MoneyPosX"), *(int*)0x58F5FC, -999, 32, 999);
                Ui::EditAddress<float>(TEXT("Visual.MoneyPosY"), 0x866C88, -999, 89, 999);
                static std::vector<Ui::NamedValue> font_outline
                {
                    {TEXT("Visual.NoOutline"), 0}, {TEXT("Visual.ThinOutline"), 1}, {TEXT("Visual.DefaultOutline"), 2}
                };
                Ui::EditRadioButtonAddressEx(TEXT("Visual.MoneyFontOutline"), 0x58F58D, font_outline);
                static std::vector<Ui::NamedValue> style
                { 
                    {TEXT("Visual.Style1"), 1}, {TEXT("Visual.Style2"), 2}, {TEXT("Visual.DefaultStyle"), 3} 
                };
                Ui::EditRadioButtonAddressEx(TEXT("Visual.MoneyFontStyle"), 0x58F57F, style);
                Ui::EditAddress<float>(TEXT("Visual.RadarHeight"), *(int*)0x5834F6, 0, 76, 999);
                Ui::EditAddress<float>(TEXT("Visual.RadarWidth"), *(int*)0x5834C2, 0, 94, 999);
                Ui::EditAddress<float>(TEXT("Visual.RadarPosX"), *(int*)0x5834D4, -999, 40, 999);
                Ui::EditAddress<float>(TEXT("Visual.RadarPosY"), *(int*)0x583500, -999, 104, 999);
                Ui::EditAddress<int>(TEXT("Visual.RadarZoom"), 0xA444A3, 0, 0, 170);
                Ui::ColorPickerAddress(TEXT("Visual.RadioStationColor"), 0xBAB24C, ImVec4(150, 150, 150, 255));
                static std::vector<Ui::NamedValue> star_border
                { 
                    {TEXT("Visual.NoBorder"), 0}, {TEXT("Visual.DefaultBorder"), 1}, {TEXT("Visual.BoldBorder"), 2} 
                };
                Ui::EditRadioButtonAddressEx(TEXT("Visual.WantedStarBorder"), 0x58DD41, star_border);
                Ui::EditAddress<float>(TEXT("Visual.WantedPosX"), *(int*)0x58DD0F, -999, 29, 999);
                Ui::EditAddress<float>(TEXT("Visual.WantedPosY"), *(int*)0x58DDFC, -999, 114, 999);
                Ui::EditAddress<float>(TEXT("Visual.WeaponAmmoPosX"), *(int*)0x58FA02, -999, 32, 999);
                Ui::EditAddress<float>(TEXT("Visual.WeaponAmmoPosY"), *(int*)0x58F9E6, -999, 43, 999);
                Ui::EditAddress<float>(TEXT("Visual.WeaponIconPosX"), *(int*)0x58F927, -999, 32, 999);
                Ui::EditAddress<float>(TEXT("Visual.WeaponIconPosY"), *(int*)0x58F913, -999, 20, 999);
#elif GTAVC
                Ui::EditAddress<float>(TEXT("Visual.RadarPosX"), 0x68FD2C, -999, 40, 999);
                Ui::EditAddress<float>(TEXT("Visual.RadarPosY"), 0x68FD34, -999, 104, 999);
                Ui::EditAddress<BYTE>(TEXT("Visual.RadarWidth"), 0x68FD28, -999, 20, 999);
#endif

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }

#ifdef GTASA
        if (m_nTimecycHour == 8 ? ImGui::BeginTabItem(TEXT("Visual.TimecycTab")) : ImGui::BeginTabItem(TEXT("Visual.Timecyc24hTab")))
#else
        if (ImGui::BeginTabItem(TEXT("Visual.TimecycTab")))
#endif
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Visual.GenerateFile"), Widget::CalcSize(2)))
            {
                GenerateTimecycFile();
                SetHelpMessage(TEXT("Visual.FileGenerated"));
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Visual.ResetTimecyc"), Widget::CalcSize(2)))
            {
                CTimeCycle::Initialise();
                SetHelpMessage(TEXT("Visual.TimecycReset"));
            }
            ImGui::Spacing();

            int weather = CWeather::OldWeatherType;
            if (Ui::ListBox(TEXT("Visual.CurrentWeather"), m_WeatherNames, weather))
            {
                CWeather::OldWeatherType = weather;
            }

            weather = CWeather::NewWeatherType;
            if (Ui::ListBox(TEXT("Visual.NextWeather"), m_WeatherNames, weather))
            {
                CWeather::NewWeatherType = weather;
            }

            ImGui::Spacing();
            int hour = CClock::ms_nGameClockHours;
            int minute = CClock::ms_nGameClockMinutes;

            if (Game::m_bSyncTime)
            {
                ImGui::BeginDisabled(Game::m_bSyncTime);
            }

            if (ImGui::InputInt(TEXT("Visual.Hour"), &hour) & !Game::m_bSyncTime)
            {
                if (hour < 0) hour = 23;
                if (hour > 23) hour = 0;
                CClock::ms_nGameClockHours = hour;
            }

            if (ImGui::InputInt(TEXT("Visual.Minute"), &minute) & !Game::m_bSyncTime)
            {
                if (minute < 0) minute = 59;
                if (minute > 59) minute = 0;
                CClock::ms_nGameClockMinutes = minute;
            }

            if (Game::m_bSyncTime)
            {
                ImGui::EndDisabled();
                Widget::Tooltip(TEXT("Visual.SyncTimeEnabled"));
            }

            if (ImGui::Checkbox(TEXT("Visual.FreezeGameTime"), &Game::m_bFreezeTime))
            {
                if (Game::m_bFreezeTime)
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
            if (ImGui::BeginTabBar("Timecyc subtab", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
            {
                if (ImGui::BeginTabItem(TEXT("Visual.ColorsTab")))
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
                if (ImGui::BeginTabItem("Misc"))
                {
                    ImGui::BeginChild("TimecycMisc");
                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);

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
