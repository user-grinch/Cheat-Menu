#include "pch.h"
#include "hotkeys.h"

const size_t HOTKEY_START = 3;
const size_t HOTKEY_END = 135;

Hotkey::Hotkey(int key1, int key2, const std::string& configPath)
    : m_key1(key1), m_key2(key2), m_ConfigPath("Hotkeys." + configPath)
{
    if (m_ConfigPath != "")
    {
        m_key1 = gConfig.GetValue(m_ConfigPath + ".1", m_key1);
        m_key2 = gConfig.GetValue(m_ConfigPath + ".2", m_key2);
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
            gConfig.SetValue(m_ConfigPath + "1", m_key1);
            gConfig.SetValue(m_ConfigPath + "2", m_key2);
        }
    }


    // Reset hotkeys to NONE on right click
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        if (ImGui::IsItemHovered())
        {
            m_key1 = VK_NONE;
            m_key2 = VK_NONE;
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

bool Hotkey::PressedBasic()
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