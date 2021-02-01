#include "pch.h"
#include "Visual.h"
#include "Ui.h"
#include "Util.h"
#include "Game.h"
#include "CHudColours.h"
#include "TimeCycle.h"


bool Visual::lock_weather = false;
int Visual::weather_type_backup = 0;
int Visual::timecyc_hour = 8;

std::vector<std::string> Visual::weather_names{
"EXTRASUNNY LA","SUNNY LA","EXTRASUNNY SMOG LA","SUNNY SMOG LA","CLOUDY LA","SUNNY SF","EXTRASUNNY SF","CLOUDY SF","RAINY SF","FOGGY SF",
"SUNNY VEGAS","EXTRASUNNY VEGAS","CLOUDY VEGAS","EXTRASUNNY COUNTRYSIDE","SUNNY COUNTRYSIDE","CLOUDY COUNTRYSIDE","RAINY COUNTRYSIDE",
"EXTRASUNNY DESERT","SUNNY DESERT","SANDSTORM DESERT","UNDERWATER","EXTRACOLOURS 1","EXTRACOLOURS 2"
};

// Let's just use our own variables
static float radar_posX;
static float radar_posY;
static float radar_width = 76.0f;
static float radar_height = 104.0f;
static CHudColour health_bar;
//static CHudColour armour_bar;
static bool init_patches = false;

Visual::Visual()
{
	Events::initGameEvent += []
	{
		if (LoadLibraryW(L"timecycle24.asi"))
			timecyc_hour = 24;
	};

	Events::processScriptsEvent += []
	{
		// Improve this later
		if (lock_weather)
		{
			CWeather::OldWeatherType = weather_type_backup;
			CWeather::NewWeatherType = weather_type_backup;
		}
	};
}


Visual::~Visual()
{
}

// Thanks to GuruGuru
int Visual::GetCurrentHourTimeId()
{
	int hour = CClock::ms_nGameClockHours;

	if (timecyc_hour == 24)
		return hour;

	if (hour < 5) return 0;
	if (hour == 5) return 1;

	if (hour == 6) return 2;

	if (7 <= hour && hour < 12) return 3;
	if (12 <= hour && hour < 19)  return 4;

	if (hour == 19)  return 5; 
	if (hour == 20 || hour == 21) return 6;
	if (hour == 22 || hour == 23) return 7;

	return NULL; 
}

bool Visual::TimeCycColorEdit3(const char* label, uchar *r, uchar *g, uchar *b, ImGuiColorEditFlags flags)
{
	bool rtn = false;
	int val = 23 * GetCurrentHourTimeId() + CWeather::OldWeatherType;

	float col[3]{ r[val] / 255.0f, g[val] / 255.0f, b[val] / 255.0f };

	if (ImGui::ColorEdit3(label, col, flags))
	{
		r[val] = col[0] * 255;
		g[val] = col[1] * 255;
		b[val] = col[2] * 255;
		rtn = true;
	}

	return rtn;
}

bool Visual::TimeCycColorEdit4(const char* label, uchar *r, uchar *g, uchar *b, uchar *a, ImGuiColorEditFlags flags)
{
	bool rtn = false;
	int val = 23 * GetCurrentHourTimeId() + CWeather::OldWeatherType;

	float col[4]{ r[val] / 255.0f, g[val] / 255.0f, b[val] / 255.0f, a[val] / 255.0f };

	if (ImGui::ColorEdit4(label, col, flags))
	{
		r[val] = col[0] * 255;
		g[val] = col[1] * 255;
		b[val] = col[2] * 255;
		a[val] = col[3] * 255;
		rtn = true;
	}

	return rtn;
}

void Visual::GenerateTimecycFile()
{
	std::ofstream file;
	if (timecyc_hour == 24)
		file = std::ofstream("timecyc_24h.dat");
	else
		file = std::ofstream("timecyc.dat");

	for (uint i = 0; i < weather_names.size(); ++i)
	{
		file << "\n\n//////////// " << weather_names[i] << "\n";
		file << "//\tAmb\t\t\t\t\tAmb Obj \t\t\t\tDir \t\t\t\t\tSky top\t\t\t\tSky bot\t\t\t\tSunCore\t\t\t\t\tSunCorona\t\t\tSunSz\tSprSz\tSprBght\t\tShdw\tLightShd\tPoleShd\t\tFarClp\t\tFogSt\tLightOnGround\tLowCloudsRGB\tBottomCloudRGB\t\tWaterRGBA\t\t\t\tARGB1\t\t\t\t\tARGB2\t\t\tCloudAlpha\t\tIntensityLimit\t\tWaterFogAlpha\tDirMult \n\n";

		for (int j = 0; j < timecyc_hour; ++j)
		{
			if (timecyc_hour == 24)
			{
				if (j >= 12)
					file << "// " << j << " PM\n";
				else
					file << "// " << j << " AM\n";
			}
			else
			{
				if (j == 0)  file << "// Midnight\n";
				if (j == 1)  file << "// 5 AM\n";
				if (j == 2)  file << "// 6 AM\n";
				if (j == 3)  file << "// 7 AM\n";
				if (j == 4)  file << "// Midday\n";
				if (j == 5)  file << "// 7 PM\n";
				if (j == 6)  file << "// 8 PM\n";
				if (j == 7)  file << "// 10 PM\n";
			}

			int val = 23 * j + i;

			file << "\t" << +m_nAmbientRed[val] << " " << +m_nAmbientGreen[val] << " " << +m_nAmbientBlue[val] << " \t\t"

				<< "\t" << +m_nAmbientRed_Obj[val] << " " << +m_nAmbientGreen_Obj[val] << " " << +m_nAmbientBlue_Obj[val] << " \t\t"

				<< "\t255 255 255\t\t" // unused

				<< "\t" << +m_nSkyTopRed[val] << " " << +m_nSkyTopGreen[val] << " " << +m_nSkyTopBlue[val] << " \t\t"

				<< "\t" << +m_nSkyBottomRed[val] << " " << +m_nSkyBottomGreen[val] << " " << +m_nSkyBottomBlue[val] << " \t\t"

				<< "\t" << +m_nSunCoreRed[val] << " " << +m_nSunCoreGreen[val] << " " << +m_nSunCoreBlue[val] << " \t\t"

				<< "\t" << +m_nSunCoronaRed[val] << " " << +m_nSunCoronaGreen[val] << " " << +m_nSunCoronaBlue[val] << " \t\t"

				<< "\t" << (+m_fSunSize[val] - 0.5f) / 10.0f << " " << (+m_fSpriteSize[val] - 0.5f) / 10.0f << " " << (+m_fSpriteBrightness[val] - 0.5f) / 10.0f << " \t\t"

				<< "\t" << +m_nShadowStrength[val] << " " << +m_nLightShadowStrength[val] << " " << +m_nPoleShadowStrength[val] << " \t\t"

				<< "\t" << +m_fFarClip[val] << " " << +m_fFogStart[val] << " " << (+m_fLightsOnGroundBrightness[val] - 0.5) / 10.0f << " \t\t"

				<< "\t" << +m_nLowCloudsRed[val] << " " << +m_nLowCloudsGreen[val] << " " << +m_nLowCloudsBlue[val] << " \t\t"

				<< "\t" << +m_nFluffyCloudsBottomRed[val] << " " << +m_nFluffyCloudsBottomGreen[val] << " " << +m_nFluffyCloudsBottomBlue[val] << " \t\t"

				<< "\t" << +m_fWaterRed[val] << " " << +m_fWaterGreen[val] << " " << +m_fWaterBlue[val] << " " << +m_fWaterAlpha[val] << " \t\t"

				<< "\t" << +m_fPostFx1Alpha[val] << " " << +m_fPostFx1Red[val] << " " << +m_fPostFx1Green[val] << " " << +m_fPostFx1Blue[val] << " \t\t"

				<< "\t" << +m_fPostFx2Alpha[val] << " " << +m_fPostFx2Red[val] << " " << +m_fPostFx2Green[val] << " " << +m_fPostFx2Blue[val] << " \t\t"

				<< "\t" << +m_fCloudAlpha[val] << " " << +m_nHighLightMinIntensity[val] << " " << +m_nWaterFogAlpha[val] << " " << +m_nDirectionalMult[val] / 100.0 << " \t\t" << std::endl;
		}
	}
}

void Visual::Main()
{
	if (ImGui::BeginTabBar("Visual", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::Columns(2, 0, false);
			Ui::CheckboxAddress("Armour border", 0x589123);
			Ui::CheckboxAddress("Armour percentage", 0x589125);
			Ui::CheckboxAddress("Breath border", 0x589207);
			Ui::CheckboxAddress("Breath percentage", 0x589209);
			Ui::CheckboxAddress("Gray radar", 0xA444A4);
			Ui::CheckboxAddress("Health border", 0x589353);
			Ui::CheckboxAddress("Health percentage", 0x589355);

			ImGui::NextColumn();

			if (Ui::CheckboxWithHint("Hide area names", &CHud::bScriptDontDisplayAreaName))
				Command<Commands::DISPLAY_ZONE_NAMES>(!CHud::bScriptDontDisplayAreaName);

			if (Ui::CheckboxWithHint("Hide veh names", &CHud::bScriptDontDisplayVehicleName))
				Command<Commands::DISPLAY_CAR_NAMES>(!CHud::bScriptDontDisplayVehicleName);

			if (Ui::CheckboxWithHint("Hide radar", &CHud::bScriptDontDisplayRadar))
				Command<Commands::DISPLAY_RADAR>(!CHud::bScriptDontDisplayRadar);

			Ui::CheckboxAddressEx("Hide wanted level", 0x58DD1B, 0x90, 1);
			if (Ui::CheckboxWithHint("Lock weather", &lock_weather))
				weather_type_backup = CWeather::OldWeatherType;

			if (Ui::CheckboxWithHint("Show hud", &CHud::m_Wants_To_Draw_Hud))
				Command<Commands::DISPLAY_HUD>(CHud::m_Wants_To_Draw_Hud);

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{

			if (!init_patches)
			{
				// read those values from game
				health_bar = HudColour.m_aColours[0];
				//armour_bar = HudColour.m_aColours[4];

				radar_posX = *(float*)*(int*)0x5834D4;
				radar_posY = *(float*)*(int*)0x583500;
				radar_height = *(float*)*(int*)0x5834F6;
				radar_width = *(float*)*(int*)0x5834C2;
				
				// patch radar stuff oof
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
 
				patch::SetPointer(0x589331, &health_bar);
				//patch::SetPointer(0x5890FC,&armour_bar);
				init_patches = true;
			}

			Ui::ColorPickerAddress("Armour bar", *(int*)0x5890FC, ImVec4(180,25,29,255));
			Ui::ColorPickerAddress("Health bar", *(int*)0x589331, ImVec4(180,25,29,255));
			Ui::ColorPickerAddress("Main menu title border color", 0xBAB240, ImVec4(0,0,0,255));
			Ui::ColorPickerAddress("Money color", 0xBAB230, ImVec4(54,104,44,255));	
			static std::vector<Ui::NamedValue> font_outline{{ "No outline", 0 }, { "Thin outline" ,1 }, { "Default outline" ,2 }};
			Ui::EditRadioButtonAddressEx("Money font outline", 0x58F58D, font_outline);
			static std::vector<Ui::NamedValue> style{ { "Style 1", 1 }, { "Style 2" ,2 }, { "Default style" ,3 }};
			Ui::EditRadioButtonAddressEx("Money font style", 0x58F57F, style);
			Ui::EditAddress<float>("Radar Height", *(int*)0x5834F6, 0, 76, 999);
			Ui::EditAddress<float>("Radar Width", *(int*)0x5834C2, 0, 94, 999);
			Ui::EditAddress<float>("Radar X position", *(int*)0x5834D4, -999, 40, 999);
			Ui::EditAddress<float>("Radar Y position", *(int*)0x583500, -999, 104, 999);
			Ui::EditAddress<int>("Radar zoom", 0xA444A3, 0, 0, 170);
			Ui::ColorPickerAddress("Radio station color", 0xBAB24C, ImVec4(150,150,150,255));
			static std::vector<Ui::NamedValue> star_border{ { "No border", 0 }, { "Default" ,1 }, { "Bold border" ,2 }};
			Ui::EditRadioButtonAddressEx("Wanted star border", 0x58DD41, star_border);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Timecyc"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Generate timecyc file", Ui::GetSize(2)))
			{
				GenerateTimecycFile();
				CHud::SetHelpMessage("File generated", false, false, false);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset timecyc", Ui::GetSize(2)))
			{
				CTimeCycle::Initialise();
				CHud::SetHelpMessage("Timecyc reset", false, false, false);
			}
			ImGui::Spacing();

			int weather = CWeather::OldWeatherType;
			if (Ui::ListBox("Current weather", weather_names, weather))
				CWeather::OldWeatherType = weather;

			weather = CWeather::NewWeatherType;
			if (Ui::ListBox("Next weather", weather_names, weather))
				CWeather::NewWeatherType = weather;

			ImGui::Spacing();
			int hour = CClock::ms_nGameClockHours;
			int minute = CClock::ms_nGameClockMinutes;

			if (Game::sync_time)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			
			if (ImGui::InputInt("Hour", &hour) & !Game::sync_time)
			{
				if (hour < 0) hour = 23;
				if (hour > 23) hour = 0;
				CClock::ms_nGameClockHours = hour;
			}

			if (ImGui::InputInt("Minute", &minute) & !Game::sync_time)
			{
				if (minute < 0) minute = 59;
				if (minute > 59) minute = 0;
				CClock::ms_nGameClockMinutes = minute;
			}

			if (Game::sync_time)
			{
				ImGui::PopStyleVar();
				ImGui::PopItemFlag();
				Ui::ShowTooltip("Sync system time is enabled.\n(Game/Sync system time)");
			}

			ImGui::Spacing();
			if (ImGui::BeginTabBar("Timecyc subtab", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
			{
				if (ImGui::BeginTabItem("Colors"))
				{
					ImGui::BeginChild("TimecycColors");
					ImGui::Spacing();

					TimeCycColorEdit3("Ambient", m_nAmbientRed, m_nAmbientGreen, m_nAmbientBlue);
					TimeCycColorEdit3("Ambient obj", m_nAmbientRed_Obj, m_nAmbientGreen_Obj, m_nAmbientBlue_Obj);
					TimeCycColorEdit3("Fluffy clouds", m_nFluffyCloudsBottomRed , m_nFluffyCloudsBottomGreen, m_nFluffyCloudsBottomBlue);
					TimeCycColorEdit3("Low clouds", m_nLowCloudsRed, m_nLowCloudsGreen, m_nLowCloudsBlue);
					
					TimeCycColorEdit4("Postfx 1", m_fPostFx1Red, m_fPostFx1Green, m_fPostFx1Blue, m_fPostFx1Alpha);
					TimeCycColorEdit4("Postfx 2", m_fPostFx2Red, m_fPostFx2Green, m_fPostFx2Blue, m_fPostFx1Alpha);
					
					TimeCycColorEdit3("Sky bottom", m_nSkyBottomRed, m_nSkyBottomGreen, m_nSkyBottomBlue);
					TimeCycColorEdit3("Sun core", m_nSunCoreRed, m_nSunCoreGreen, m_nSunCoreBlue);
					TimeCycColorEdit3("Sun corona", m_nSunCoronaRed, m_nSunCoronaGreen, m_nSunCoronaBlue);
					TimeCycColorEdit3("Sky top", m_nSkyTopRed, m_nSkyTopGreen, m_nSkyTopBlue);

					TimeCycColorEdit4("Water", m_fWaterRed, m_fWaterGreen, m_fWaterBlue, m_fWaterAlpha);

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Misc"))
				{
					ImGui::BeginChild("TimecycMisc");
					ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 2);
					TimecycSlider("Cloud alpha", m_fCloudAlpha, 0, 255);
					TimecycSlider("Directional mult", m_nDirectionalMult , 0, 255);
					TimecycSlider("Far clip", m_fFarClip, 0, 2000);
					TimecycSlider("Fog start", m_fFogStart, 0, 2000);
					TimecycSlider("High light min intensity", m_nHighLightMinIntensity, 0, 255);
					TimecycSlider("Light on ground brightness", m_fLightsOnGroundBrightness, 0, 255);
					TimecycSlider("Light shadow strength", m_nLightShadowStrength, 0, 255);
					TimecycSlider("Pole shadow strength", m_nPoleShadowStrength, 0, 255);
					TimecycSlider("Shadow strength", m_nShadowStrength, 0, 255);
					TimecycSlider("Sprite brightness", m_fSpriteBrightness, 0, 127);
					TimecycSlider("Sprite size", m_fSpriteSize, 0, 127);
					TimecycSlider("Sun size", m_fSunSize, 0, 127);
					TimecycSlider("Water fog alpha", m_nWaterFogAlpha, 0, 255);
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
