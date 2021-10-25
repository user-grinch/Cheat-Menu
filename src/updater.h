#pragma once
#include <string>

enum UPDATER_STATE
{
	UPDATER_IDLE,
	UPDATER_CHECKING,
	UPDATER_UPDATE_FOUND
};

class Updater
{
public:
	static inline UPDATER_STATE m_State = UPDATER_IDLE;
	static inline std::string m_LatestVersion;

	static void CheckForUpdate();
	static void ShowUpdateScreen();
};
