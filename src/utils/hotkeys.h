#pragma once
#include <string>

const int VK_NONE = -1;
const size_t VK_KEY_0 = 0x30;
const size_t VK_KEY_1 = 0x31;
const size_t VK_KEY_2 = 0x32;
const size_t VK_KEY_3 = 0x33;
const size_t VK_KEY_4 = 0x34;
const size_t VK_KEY_5 = 0x35;
const size_t VK_KEY_6 = 0x36;
const size_t VK_KEY_7 = 0x37;
const size_t VK_KEY_8 = 0x38;
const size_t VK_KEY_9 = 0x39;
const size_t VK_KEY_A = 0x41;
const size_t VK_KEY_B = 0x42;
const size_t VK_KEY_C = 0x43;
const size_t VK_KEY_D = 0x44;
const size_t VK_KEY_E = 0x45;
const size_t VK_KEY_F = 0x46;
const size_t VK_KEY_G = 0x47;
const size_t VK_KEY_H = 0x48;
const size_t VK_KEY_I = 0x49;
const size_t VK_KEY_J = 0x4A;
const size_t VK_KEY_K = 0x4B;
const size_t VK_KEY_L = 0x4C;
const size_t VK_KEY_M = 0x4D;
const size_t VK_KEY_N = 0x4E;
const size_t VK_KEY_O = 0x4F;
const size_t VK_KEY_P = 0x50;
const size_t VK_KEY_Q = 0x51;
const size_t VK_KEY_R = 0x52;
const size_t VK_KEY_S = 0x53;
const size_t VK_KEY_T = 0x54;
const size_t VK_KEY_U = 0x55;
const size_t VK_KEY_V = 0x56;
const size_t VK_KEY_W = 0x57;
const size_t VK_KEY_X = 0x58;
const size_t VK_KEY_Y = 0x59;
const size_t VK_KEY_Z = 0x5A;

/*
    HotKey Implementation Class
    Handles events, save-load, name-fetch, ui etc.
*/
class Hotkey {
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

    // Returns true if the hotkeys are pressed in realtime
    bool PressedRealtime();
};
