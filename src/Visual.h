#pragma once
class Visual
{
private:
	inline static bool lock_weather = false;
	inline static int weather_type_backup = 0;

	// Timecyc
	inline static int timecyc_hour = 8;
	inline static std::vector<std::string> weather_names
	{
		"EXTRASUNNY LA","SUNNY LA","EXTRASUNNY SMOG LA","SUNNY SMOG LA","CLOUDY LA","SUNNY SF","EXTRASUNNY SF","CLOUDY SF","RAINY SF","FOGGY SF",
		"SUNNY VEGAS","EXTRASUNNY VEGAS","CLOUDY VEGAS","EXTRASUNNY COUNTRYSIDE","SUNNY COUNTRYSIDE","CLOUDY COUNTRYSIDE","RAINY COUNTRYSIDE",
		"EXTRASUNNY DESERT","SUNNY DESERT","SANDSTORM DESERT","UNDERWATER","EXTRACOLOURS 1","EXTRACOLOURS 2"
	};

	static void GenerateTimecycFile();
	static int GetCurrentHourTimeId();
	static bool TimeCycColorEdit3(const char* label, uchar* r, uchar* g, uchar* b, ImGuiColorEditFlags flags = 0);
	static bool TimeCycColorEdit4(const char* label, uchar* r, uchar* g, uchar* b, uchar* a, ImGuiColorEditFlags flags = 0);
	template<typename T>
	static void TimecycSlider(const char* label, T* data, int min, int max);
public:
	Visual();
	~Visual();
	static void Draw();
};

template<typename T>
void Visual::TimecycSlider(const char* label, T* ptr, int min, int max)
{
	int val = 23 * GetCurrentHourTimeId() + CWeather::OldWeatherType;
	T* arr = (T*)patch::GetPointer(int(ptr));
	int a = arr[val];

	if (ImGui::SliderInt(label, &a, min, max))
		arr[val] = (T)a;
}