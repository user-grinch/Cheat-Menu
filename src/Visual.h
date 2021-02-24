#pragma once
class Visual
{
private:
	static bool lock_weather;
	static int weather_type_backup;

	// Timecyc
	static std::vector<std::string> weather_names;
	static int timecyc_hour;

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