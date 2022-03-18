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

    static void CheckUpdate();
    static std::string GetUpdateVersion();
    static bool IsUpdateAvailable();

    /*
        Just downloading counter.info file from GitHub repository
        Initial plan was to add google analytics but people will freak 
        out about that (though every site has it in some form)
        
        Pretty barebones but this shouldn't raise any concerns (i hope)
    */
    static void IncrementDailyUsageCounter();

    // Needs to run in it's own thread to prevent the game from freezing
    static void Process();
    static void ResetUpdaterState();
};
