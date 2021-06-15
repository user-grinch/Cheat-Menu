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
   inline static UPDATER_STATE state = UPDATER_IDLE;
   inline static std::string latest_version = "";
   
   static void CheckForUpdate();
   static void ShowUpdateScreen();
};
