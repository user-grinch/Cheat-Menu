#include "pch.h"
#include "Visual.h"
#include "Ui.h"
#include "Util.h"
#include "Game.h"
#include "CHudColours.h"
#include "TimeCycle.h"

Visual::Visual()
{
	if (GetModuleHandle("timecycle24.asi"))
		m_nTimecycHour = 24;

	Events::processScriptsEvent += []
	{
		// TODO: Needs improvement
		if (m_bLockWeather)
		{
			CWeather::OldWeatherType = m_nBacWeatherType;
			CWeather::NewWeatherType = m_nBacWeatherType;
		}
	};
}

// Thanks to GuruGuru
int Visual::GetCurrentHourTimeId(int hour)
{
	if (hour == -1)
	{
		hour = CClock::ms_nGameClockHours;
	}

	if (m_nTimecycHour == 24)
		return hour;

	if (hour < 5) return 0;
	if (hour == 5) return 1;

	if (hour == 6) return 2;

	if (7 <= hour && hour < 12) return 3;
	if (12 <= hour && hour < 19) return 4;

	if (hour == 19) return 5;
	if (hour == 20 || hour == 21) return 6;
	if (hour == 22 || hour == 23) return 7;

	return NULL;
}

bool Visual::TimeCycColorEdit3(const char* label, uchar* r, uchar* g, uchar* b, ImGuiColorEditFlags flags)
{
	bool rtn = false;
	int val = 23 * GetCurrentHourTimeId() + CWeather::OldWeatherType;

	auto red = static_cast<uchar*>(patch::GetPointer(int(r)));
	auto green = static_cast<uchar*>(patch::GetPointer(int(g)));
	auto blue = static_cast<uchar*>(patch::GetPointer(int(b)));

	float col[3]{red[val] / 255.0f, green[val] / 255.0f, blue[val] / 255.0f};

	if (ImGui::ColorEdit3(label, col, flags))
	{
		red[val] = col[0] * 255;
		green[val] = col[1] * 255;
		blue[val] = col[2] * 255;
		rtn = true;
	}

	return rtn;
}

bool Visual::TimeCycColorEdit4(const char* label, uchar* r, uchar* g, uchar* b, uchar* a, ImGuiColorEditFlags flags)
{
	bool rtn = false;
	int val = 23 * GetCurrentHourTimeId() + CWeather::OldWeatherType;

	auto red = static_cast<uchar*>(patch::GetPointer(int(r)));
	auto green = static_cast<uchar*>(patch::GetPointer(int(g)));
	auto blue = static_cast<uchar*>(patch::GetPointer(int(b)));
	auto alpha = static_cast<uchar*>(patch::GetPointer(int(a)));

	float col[4]{red[val] / 255.0f, green[val] / 255.0f, blue[val] / 255.0f, alpha[val] / 255.0f};

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

template <typename T>
int GetTCVal(T* addr, int index)
{
	T* arr = static_cast<T*>(patch::GetPointer(int(addr)));
	return static_cast<int>(arr[index]);
}

void Visual::GenerateTimecycFile()
{
	std::ofstream file;
	if (m_nTimecycHour == 24)
		file = std::ofstream("timecyc_24h.dat");
	else
		file = std::ofstream("timecyc.dat");

	for (uint i = 0; i < m_WeatherNames.size(); ++i)
	{
		file << "\n\n//////////// " << m_WeatherNames[i] << "\n";
		file <<
			"//\tAmb\t\t\t\t\tAmb Obj \t\t\t\tDir \t\t\t\t\tSky top\t\t\t\tSky bot\t\t\t\tSunCore\t\t\t\t\tSunCorona\t\t\tSunSz\tSprSz\tSprBght\t\tShdw\tLightShd\tPoleShd\t\tFarClp\t\tFogSt\tLightOnGround\tLowCloudsRGB\tBottomCloudRGB\t\tWaterRGBA\t\t\t\tARGB1\t\t\t\t\tARGB2\t\t\tCloudAlpha\t\tIntensityLimit\t\tWaterFogAlpha\tDirMult \n\n";

		for (int j = 0; j < m_nTimecycHour; ++j)
		{
			if (m_nTimecycHour == 24)
			{
				if (j >= 12)
					file << "// " << j << " PM\n";
				else
					file << "// " << j << " AM\n";
			}
			else
			{
				if (j == 0) file << "// Midnight\n";
				if (j == 1) file << "// 5 AM\n";
				if (j == 2) file << "// 6 AM\n";
				if (j == 3) file << "// 7 AM\n";
				if (j == 4) file << "// Midday\n";
				if (j == 5) file << "// 7 PM\n";
				if (j == 6) file << "// 8 PM\n";
				if (j == 7) file << "// 10 PM\n";
			}

			int val = 23 * j + i;

			file << "\t" << GetTCVal(m_nAmbientRed, val) << " " << GetTCVal(m_nAmbientGreen, val) << " " << GetTCVal(
					m_nAmbientBlue, val) << " \t\t"

				<< "\t" << GetTCVal(m_nAmbientRed_Obj, val) << " " << GetTCVal(m_nAmbientGreen_Obj, val) << " " <<
				GetTCVal(m_nAmbientBlue_Obj, val) << " \t\t"

				<< "\t255 255 255\t\t" // unused

				<< "\t" << GetTCVal(m_nSkyTopRed, val) << " " << GetTCVal(m_nSkyTopGreen, val) << " " << GetTCVal(
					m_nSkyTopBlue, val) << " \t\t"

				<< "\t" << GetTCVal(m_nSkyBottomRed, val) << " " << GetTCVal(m_nSkyBottomGreen, val) << " " << GetTCVal(
					m_nSkyBottomBlue, val) << " \t\t"

				<< "\t" << GetTCVal(m_nSunCoreRed, val) << " " << GetTCVal(m_nSunCoreGreen, val) << " " << GetTCVal(
					m_nSunCoreBlue, val) << " \t\t"

				<< "\t" << GetTCVal(m_nSunCoronaRed, val) << " " << GetTCVal(m_nSunCoronaGreen, val) << " " << GetTCVal(
					m_nSunCoronaBlue, val) << " \t\t"

				<< "\t" << (GetTCVal(m_fSunSize, val) - 0.5f) / 10.0f << " " << (GetTCVal(m_fSpriteSize, val) - 0.5f) /
				10.0f << " " << (GetTCVal(m_fSpriteBrightness, val) - 0.5f) / 10.0f << " \t\t"

				<< "\t" << GetTCVal(m_nShadowStrength, val) << " " << GetTCVal(m_nLightShadowStrength, val) << " " <<
				GetTCVal(m_nPoleShadowStrength, val) << " \t\t"

				<< "\t" << GetTCVal(m_fFarClip, val) << " " << GetTCVal(m_fFogStart, val) << " " << (GetTCVal(
					m_fLightsOnGroundBrightness, val) - 0.5) / 10.0f << " \t\t"

				<< "\t" << GetTCVal(m_nLowCloudsRed, val) << " " << GetTCVal(m_nLowCloudsGreen, val) << " " << GetTCVal(
					m_nLowCloudsBlue, val) << " \t\t"

				<< "\t" << GetTCVal(m_nFluffyCloudsBottomRed, val) << " " << GetTCVal(m_nFluffyCloudsBottomGreen, val)
				<< " " << GetTCVal(m_nFluffyCloudsBottomBlue, val) << " \t\t"

				<< "\t" << GetTCVal(m_fWaterRed, val) << " " << GetTCVal(m_fWaterGreen, val) << " " <<
				GetTCVal(m_fWaterBlue, val) << " " << GetTCVal(m_fWaterAlpha, val) << " \t\t"

				<< "\t" << GetTCVal(m_fPostFx1Alpha, val) << " " << GetTCVal(m_fPostFx1Red, val) << " " <<
				GetTCVal(m_fPostFx1Green, val) << " " << GetTCVal(m_fPostFx1Blue, val) << " \t\t"

				<< "\t" << GetTCVal(m_fPostFx2Alpha, val) << " " << GetTCVal(m_fPostFx2Red, val) << " " <<
				GetTCVal(m_fPostFx2Green, val) << " " << GetTCVal(m_fPostFx2Blue, val) << " \t\t"

				<< "\t" << GetTCVal(m_fCloudAlpha, val) << " " << GetTCVal(m_nHighLightMinIntensity, val) << " " <<
				GetTCVal(m_nWaterFogAlpha, val) << " " << GetTCVal(m_nDirectionalMult, val) / 100.0 << " \t\t" <<
				std::endl;
		}
	}
}

void Visual::Draw()
{
	if (ImGui::BeginTabBar("Visual", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::Columns(2, nullptr, false);
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

			Ui::CheckboxAddressEx("Hide wanted level", 0x58DD1B, 0x90, 1);
			if (Ui::CheckboxWithHint("Lock weather", &m_bLockWeather))
				m_nBacWeatherType = CWeather::OldWeatherType;

			bool radar_state = (patch::Get<BYTE>(0xBA676C) != 2);
			if (Ui::CheckboxWithHint("Show radar", &radar_state))
			{
				patch::Set<BYTE>(0xBA676C, radar_state == true ? 0 : 2);
			}

			Ui::CheckboxAddress("Show hud", 0xBA6769);

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{
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

			if (ImGui::BeginChild("VisualsChild"))
			{
				ImGui::TextWrapped(
					"These options won't work if you got any mods that drastically changes the game hud. i.e. Mobile Hud, GTA 5 Hud etc.");
				ImGui::Spacing();
				Ui::ColorPickerAddress("Armourbar color", *(int*)0x5890FC, ImVec4(225, 225, 225, 255));
				Ui::EditAddress<float>("Armourbar posX", 0x866B78, -999, 94, 999);
				Ui::EditAddress<float>("Armourbar posY", 0x862D38, -999, 48, 999);
				Ui::ColorPickerAddress("Breathbar color", *(int*)0x5891EB, ImVec4(172, 203, 241, 255));
				Ui::EditAddress<float>("Breathbar posX", *(int*)0x58F11F, -999, 94, 999);
				Ui::EditAddress<float>("Breathbar posY", *(int*)0x58F100, -999, 62, 999);
				Ui::ColorPickerAddress("Clock color", *(int*)0x58EBD1, ImVec4(180, 25, 29, 255));
				Ui::EditAddress<float>("Clock posX", *(int*)0x58EC16, -999, 32, 999);
				Ui::EditAddress<float>("Clock posY", *(int*)0x58EC04, -999, 22, 999);
				Ui::ColorPickerAddress("Healthbar color", *(int*)0x589331, ImVec4(180, 25, 29, 255));
				Ui::EditAddress<float>("Healthbar posX", 0x86535C, -999, 141, 999);
				Ui::EditAddress<float>("Healthbar posY", 0x866CA8, -999, 77, 999);
				Ui::ColorPickerAddress("Draw menu title border color", 0xBAB240, ImVec4(0, 0, 0, 255));
				Ui::ColorPickerAddress("Money color", 0xBAB230, ImVec4(54, 104, 44, 255));
				Ui::EditAddress<float>("Money posX", *(int*)0x58F5FC, -999, 32, 999);
				Ui::EditAddress<float>("Money posY", 0x866C88, -999, 89, 999);
				static std::vector<Ui::NamedValue> font_outline{
					{"No outline", 0}, {"Thin outline", 1}, {"Default outline", 2}
				};
				Ui::EditRadioButtonAddressEx("Money font outline", 0x58F58D, font_outline);
				static std::vector<Ui::NamedValue> style{{"Style 1", 1}, {"Style 2", 2}, {"Default style", 3}};
				Ui::EditRadioButtonAddressEx("Money font style", 0x58F57F, style);
				Ui::EditAddress<float>("Radar Height", *(int*)0x5834F6, 0, 76, 999);
				Ui::EditAddress<float>("Radar Width", *(int*)0x5834C2, 0, 94, 999);
				Ui::EditAddress<float>("Radar posX", *(int*)0x5834D4, -999, 40, 999);
				Ui::EditAddress<float>("Radar posY", *(int*)0x583500, -999, 104, 999);
				Ui::EditAddress<int>("Radar zoom", 0xA444A3, 0, 0, 170);
				Ui::ColorPickerAddress("Radio station color", 0xBAB24C, ImVec4(150, 150, 150, 255));
				static std::vector<Ui::NamedValue> star_border{{"No border", 0}, {"Default", 1}, {"Bold border", 2}};
				Ui::EditRadioButtonAddressEx("Wanted star border", 0x58DD41, star_border);
				Ui::EditAddress<float>("Wanted posX", *(int*)0x58DD0F, -999, 29, 999);
				Ui::EditAddress<float>("Wanted posY", *(int*)0x58DDFC, -999, 114, 999);
				Ui::EditAddress<float>("Weapon ammo posX", *(int*)0x58FA02, -999, 32, 999);
				Ui::EditAddress<float>("Weapon ammo posY", *(int*)0x58F9E6, -999, 43, 999);
				Ui::EditAddress<float>("Weapon icon posX", *(int*)0x58F927, -999, 32, 999);
				Ui::EditAddress<float>("Weapon icon posY", *(int*)0x58F913, -999, 20, 999);

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}

		if (m_nTimecycHour == 8 ? ImGui::BeginTabItem("Timecyc") : ImGui::BeginTabItem("Timecyc 24h"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Generate timecyc file", Ui::GetSize(2)))
			{
				GenerateTimecycFile();
				SetHelpMessage("File generated", false, false, false);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset timecyc", Ui::GetSize(2)))
			{
				CTimeCycle::Initialise();
				SetHelpMessage("Timecyc reset", false, false, false);
			}
			ImGui::Spacing();

			int weather = CWeather::OldWeatherType;
			if (Ui::ListBox("Current weather", m_WeatherNames, weather))
				CWeather::OldWeatherType = weather;

			weather = CWeather::NewWeatherType;
			if (Ui::ListBox("Next weather", m_WeatherNames, weather))
				CWeather::NewWeatherType = weather;

			ImGui::Spacing();
			int hour = CClock::ms_nGameClockHours;
			int minute = CClock::ms_nGameClockMinutes;

			if (Game::m_bSyncTime)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::InputInt("Hour", &hour) & !Game::m_bSyncTime)
			{
				if (hour < 0) hour = 23;
				if (hour > 23) hour = 0;
				CClock::ms_nGameClockHours = hour;
			}

			if (ImGui::InputInt("Minute", &minute) & !Game::m_bSyncTime)
			{
				if (minute < 0) minute = 59;
				if (minute > 59) minute = 0;
				CClock::ms_nGameClockMinutes = minute;
			}

			if (Game::m_bSyncTime)
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
					TimeCycColorEdit3("Fluffy clouds", m_nFluffyCloudsBottomRed, m_nFluffyCloudsBottomGreen,
					                  m_nFluffyCloudsBottomBlue);
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
					TimecycSlider("Directional mult", m_nDirectionalMult, 0, 255);
					TimecycSlider("Far clip", m_fFarClip, 0, 2000);
					TimecycSlider("Fog start", m_fFogStart, 0, 1500);
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
