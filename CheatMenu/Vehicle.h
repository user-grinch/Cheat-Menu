#pragma once
#include "Neon.h"
#include "Paint.h"

class Vehicle : Paint, Neon
{
private:
	inline static bool m_bBikeFly;
	inline static bool m_bDontFallBike;
	inline static bool m_bVehHeavy;
	inline static bool m_bVehWatertight;
	inline static bool m_bNoDamage;
	inline static int m_nDoorMenuButton;
	inline static std::string m_DoorNames[6] =
		{"Hood", "Boot", "Front left door", "Front right door", "Rear left door", "Rear right door"};
	inline static int m_nVehRemoveRadius;
	inline static bool m_bLockSpeed;
	inline static float m_fLockSpeed;
	inline static std::map<int, std::string> m_VehicleIDE;
	inline static std::vector<std::vector<float>> m_CarcolsColorData;
	inline static std::map<std::string, std::vector<int>> m_CarcolsCarData;
	static struct
	{
		inline static bool m_bMatFilter = true;
		inline static int m_nRadioButton = 1;
		inline static bool bShowAll;
		inline static float m_fColorPicker[3]{0, 0, 0};
	} m_Color;
	static struct
	{
		inline static float m_fColorPicker[3]{0, 0, 0};
		inline static bool m_bRainbowEffect;
		inline static uint m_nRainbowTimer;
		inline static bool m_bApplyOnTraffic;
		inline static uint m_bTrafficTimer;
	} m_Neon;
	static struct
	{
		inline static SSearchData m_VehData;
		inline static bool m_bSpawnInside = true;
		inline static bool m_bSpawnInAir = true;
		inline static char m_nLicenseText[9];
	} m_Spawner;
	inline static SSearchData m_TextureData;
	inline static SSearchData m_TuneData;
	inline static bool m_bImagesLoaded;
	static struct
	{
		inline static bool m_bEnabled;
		inline static bool m_bCompAdded;
	} m_UnlimitedNitro;

	inline static std::vector<std::string> (m_HandlingFlagNames) = // 32 flags
	{
		"1G_BOOST", "2G_BOOST", "NPC_ANTI_ROLL", "NPC_NEUTRAL_HANDL", "NO_HANDBRAKE", "STEER_REARWHEELS",
		"HB_REARWHEEL_STEER", "ALT_STEER_OPT",
		"WHEEL_F_NARROW2", "WHEEL_F_NARROW", "WHEEL_F_WIDE", "WHEEL_F_WIDE2", "WHEEL_R_NARROW2", "WHEEL_R_NARROW",
		"WHEEL_R_WIDE", "WHEEL_R_WIDE2",
		"HYDRAULIC_GEOM", "HYDRAULIC_INST", "HYDRAULIC_NONE", "NOS_INST", "OFFROAD_ABILITY", "OFFROAD_ABILITY2",
		"HALOGEN_LIGHTS", "PROC_REARWHEEL_1ST",
		"USE_MAXSP_LIMIT", "LOW_RIDER", "STREET_RACER", "SWINGING_CHASSIS", "Unused 1", "Unused 2", "Unused 3",
		"Unused 4"
	};

	inline static std::vector<std::string> (m_ModelFlagNames) = // 32 flags
	{
		"IS_VAN", "IS_BUS", "IS_LOW", "IS_BIG", "REVERSE_BONNET", "HANGING_BOOT", "TAILGATE_BOOT", "NOSWING_BOOT",
		"NO_DOORS", "TANDEM_SEATS",
		"SIT_IN_BOAT", "CONVERTIBLE", "NO_EXHAUST", "DOUBLE_EXHAUST", "NO1FPS_LOOK_BEHIND", "FORCE_DOOR_CHECK",
		"AXLE_F_NOTILT", "AXLE_F_SOLID", "AXLE_F_MCPHERSON",
		"AXLE_F_REVERSE", "AXLE_R_NOTILT", "AXLE_R_SOLID", "AXLE_R_MCPHERSON", "AXLE_R_REVERSE", "IS_BIKE", "IS_HELI",
		"IS_PLANE", "IS_BOAT", "BOUNCE_PANELS",
		"DOUBLE_RWHEELS", "FORCE_GROUND_CLEARANCE", "IS_HATCHBAC1K"
	};

public:
	static void AddComponent(const std::string& component, bool display_message = true);
	static void RemoveComponent(const std::string& component, bool display_message = true);
	static std::string GetNameFromModel(int model);
	static int GetModelFromName(const char* name);
	static void SpawnVehicle(std::string& name);
	static int GetRandomTrainIdForModel(int model);
	static void ParseVehiclesIDE();
	static void ParseCarcolsDAT();
	static void GenerateHandlingDataFile(int phandling);
	Vehicle();
	~Vehicle();
	static void Draw();
};
