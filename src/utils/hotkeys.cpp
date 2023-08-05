#include "pch.h"
#include "hotkeys.h"

const size_t HOTKEY_START = 3;
const size_t HOTKEY_END = 135;

static std::string key_names[]
{
    "LMB",
    "RMB",
    "Cancel",
    "MMB",
    "X1MB",
    "X2MB",
    "Unknown1",
    "Back",
    "Tab",
    "Reserved1",
    "Reserved2",
    "Clear",
    "Enter",
    "Unknown2",
    "Unknown3",
    "Shift",
    "Ctrl",
    "Alt",
    "Pause",
    "Capslock",
    "IME",
    "IME2",
    "IME3",
    "Unknown4",
    "IME4",
    "Unknown5",
    "Esc",
    "IME5"
    "IME6",
    "IME7",
    "IME8",
    "IME9",
    "Space",
    "Pup",
    "Pdown",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "Select",
    "Print",
    "Execute",
    "Print Screen",
    "INS",
    "Del",
    "Help",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "Unknown6",
    "Unknown7",
    "Unknown8",
    "Unknown9",
    "Unknown10",
    "Unknown11",
    "Unknown12",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "LWin",
    "RWin",
    "Apps",
    "Unknown11",
    "Sleep",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Multiply",
    "Add",
    "Separator",
    "Substract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Unknown12",
    "Unknown13",
    "Unknown14",
    "Unknown15",
    "Unknown16",
    "Unknown17",
    "Unknown18",
    "Numlock",
    "Scroll",
    "Unknown19",
    "Unknown20",
    "Unknown21",
    "Unknown22",
    "Unknown23",
    "Unknown24",
    "Unknown25",
    "Unknown26",
    "Unknown27",
    "Unknown28",
    "Unknown29",
    "Unknown30",
    "Unknown31",
    "Unknown32",
    "Unknown33",
    "LShift",
    "RShift",
    "LCtrl",
    "RCtrl",
    "LMenu",
    "RMenu"
};

Hotkey::Hotkey(int key1, int key2, const std::string& configPath)
    : m_key1(key1), m_key2(key2), m_ConfigPath("Hotkeys." + configPath)
{
    if (m_ConfigPath != "")
    {
        m_key1 = gConfig.Get((m_ConfigPath + ".Key1").c_str(), m_key1);
        m_key2 = gConfig.Get((m_ConfigPath + ".Key2").c_str(), m_key2);
    }
}

bool Hotkey::DrawUI(const char* label)
{
    bool active = (m_CurrentHotkey == label);
    bool state = false;

    // Check for pressed keys
    if (active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

        for (size_t key = HOTKEY_START; key != HOTKEY_END; ++key)
        {
            if (KeyPressed(key))
            {
                m_key1 = key;
                break;
            }
        }

        for (size_t key = HOTKEY_END; key != HOTKEY_START; --key)
        {
            if (KeyPressed(key))
            {
                m_key2 = key;
                break;
            }
        }
    }

    std::string btnText = GetNameString();
    ImVec2 size {ImGui::GetWindowContentRegionWidth() / 3.5f, ImGui::GetFrameHeight()*1.35f};
    if (ImGui::Button(std::format("{}##{}", btnText, label).c_str(), size))
    {
        if (!active)
        {
            m_CurrentHotkey = label;
        }
    }

    if (active && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        m_CurrentHotkey = "";
        state = true;

        // Save the hotkeys in config file
        if (m_ConfigPath != "")
        {
            gConfig.Set((m_ConfigPath + ".Key1").c_str(), m_key1);
            gConfig.Set((m_ConfigPath + ".Key2").c_str(), m_key2);
        }
    }


    // Reset hotkeys to NONE on right click
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        if (ImGui::IsItemHovered())
        {
            m_key1 = VK_NONE;
            m_key2 = VK_NONE;
            
            if (m_ConfigPath != "")
            {
                gConfig.Set((m_ConfigPath + ".Key1").c_str(), m_key1);
                gConfig.Set((m_ConfigPath + ".Key2").c_str(), m_key2);
            }
        }
        else
        {
            m_CurrentHotkey = "";
        }

        state = true;
    }

    ImGui::SameLine();
    ImGui::Text(label);

    if (active)
    {
        ImGui::PopStyleColor(2);
    }

    return state;
}

bool Hotkey::Pressed()
{
    if (KeyPressed(m_key1) && KeyPressed(m_key2))
    {
        m_bPressed = true;
    }
    else
    {
        if (m_bPressed)
        {
            m_bPressed = false;
            return (m_CurrentHotkey == "");
        }
    }
    return false;
}

bool Hotkey::PressedRealtime()
{
    return (m_CurrentHotkey == "") ? KeyPressed(m_key1) && KeyPressed(m_key2) : false;
}

std::string Hotkey::GetNameString()
{
    std::string text = (m_key1 == VK_NONE) ? "None" : key_names[m_key1 - 1];
    if (m_key1 != m_key2)
    {
        text += (" + " + key_names[m_key2 - 1]);
    }
    return text;
}