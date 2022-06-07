#pragma once
#include <string>

/*
    HotKey Implementation Class
    Handles events, save-load, name-fetch, ui etc.
*/
class Hotkey
{
private:
    static inline std::string m_CurrentHotkey;
    bool m_bPressed;
    int m_key1, m_key2;
    std::string m_ConfigPath; // config save path

public:
    Hotkey(int key1 = -1, int key2 = -1, const std::string& configPath = "");

    // Draws ui to change the hotkeys from frontend
    bool DrawUI(const char* label);

    // Returns formatted name of the hotkeys
    std::string GetNameString();

    // Returns true if the hotkeys are pressed
    bool Pressed();

    // Returns true if the hotkeys are pressed, with no checks
    bool PressedBasic();
};
