#pragma once

/*
	Update class
	Checks for menu updates and provides a way to update the menu.
*/
class Updater
{
private:
    enum class States
    {
        IDLE,
        CHECKING,
        FOUND
    };
    static inline States curState = States::IDLE;
    static inline std::string latestVer;

public:

    Updater() = delete;
    Updater(const Updater&) = delete;

    /*
        Checks for update on GitHub
        This might freeze the current thread!
    */
    static void CheckUpdate();
    static std::string GetUpdateVersion();

    // Is updated version available from GitHub
    static bool IsUpdateAvailable();

    // Needs to run in it's own thread to prevent the game from freezing
    static void DrawPages();

    // Resets updater state to IDLE
    static void ResetUpdaterState();
};
