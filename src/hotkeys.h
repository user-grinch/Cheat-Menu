#pragma once
#include <string>

/*
    ImGui HotKey Implementation class
    Handles hotkeys and hotkey-related functions
*/
class Hotkey
{
private:
    static inline std::string m_CurrentHotkey;
    bool m_bPressed;

public:
    int m_key1;
    int m_key2;
    
    Hotkey(int key1 = -1, int key2 = -1)
    : m_key1(key1), m_key2(key2)
    {}

    // Draws ui to change the hotkeys from frontend
    bool DrawUI(const char* label);
    bool Pressed();
    std::string GetNameString();
};

extern Hotkey aimSkinChanger;
extern Hotkey freeCam;
extern Hotkey commandWindow;
extern Hotkey fixVeh;
extern Hotkey flipVeh;
extern Hotkey freeCamTeleportPlayer;
extern Hotkey godMode;
extern Hotkey menuOpen;
extern Hotkey quickSceenShot;
extern Hotkey quickTeleport;
extern Hotkey vehEngine;
extern Hotkey vehInstantStart;
extern Hotkey vehInstantStop;
