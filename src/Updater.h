#pragma once

enum UPDATER_STATE
{
   UPDATER_IDLE,
   UPDATER_CHECKING,
   UPDATER_DOWNLOADING,
   UPDATER_DOWNLOADED,
   UPDATER_INSTALLING,
   UPDATER_NO_UPDATE,
   UPDATER_UPDATE_FOUND
};

class Updater 
{
public:
   inline static UPDATER_STATE state = UPDATER_IDLE;
   static void CheckForUpdates();
};
