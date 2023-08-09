#pragma once
#include "pch.h"
#include "savemgr.h"

/*
    Widgets Class
    Contains useful ui utilities
*/
class Widget {
  private:
    using VecStr = const std::vector<std::string>;

  public:
    struct AddrInfo {
        std::string name;
        uint val;
    };

    Widget() = delete;
    Widget(Widget&) = delete;

    // Calculates button size based on window width & spacing flags
    static ImVec2 CalcSize(short count = 1, bool spacing = true);

    // Calculates button size based on frame size
    static ImVec2 CalcSizeFrame(const char* text);

    // Color Picker from address
    static void ColorPickerAddr(const char* label, int addr, ImVec4&& default_color);

    // Regular checkbox with hint support
    static bool Toggle(const char* label, bool* state, const char* hint = nullptr,
                       bool is_disabled = false);

    // Checkbox for bool memory address
    template <typename T>
    static bool ToggleAddr(const char* label, uint addr, const char* hint = nullptr,
                           T enabled = 1, T disabled = 0);

    // Checkbox with raw memory input
    static bool ToggleAddrRaw(const char* label, uint addr, uint size, const char* enabled,
                              const char* disabled, const char* hint = nullptr);

    // Checkbox for bit fields
    static bool ToggleBits(const char* label, uint flag, const char* hint = nullptr);

    // Displays a button with specified color id
    static bool ColorBtn(int colorId, std::vector<float>& color, ImVec2 size);

    // ComboBox with addrs
    static void ComboBoxAddr(const char* label, const char* keys, std::vector<uint32_t> &addrs);
    static void ComboBoxAddr(const char* label, uint addr, const char* keys, std::vector<uint32_t> &values);

    // Draws DataStore data in the interface
    static void DataList(ResourceStore& data, fArg3_t clickFunc = nullptr, fArgNone_t addFunc = nullptr,
                         bool isEditItem = false, fArgNone_t contextOptionsFunc = nullptr, fArgNone_t tabsFunc = nullptr);

    // Draws a dropdown editor for memory address
    template <typename T>
    static void InputAddr(const char* label, uint address, int min = 0, int def = 0, int max = 100);
    static void InputAddr(const char* label, uint address, float min = 0.0f, float def = 0.0f,
                         float max = 100.0f, float mul = 1, float change = 1.0f);

    // Draws a dropdown editor for memory bits
    static void EditBits(const char* label, int address, VecStr& names);

#ifdef GTASA
    // Draws a dropdown editor for editing stats, only sa
    static void EditStat(const char* label, int stat_id, int min = 0, int def = 0, int max = 1000);
#endif

    // ImGui::TextFilter with hint support
    static bool Filter(const char* label, ImGuiTextFilter& filter, const char* hint);

    // Input widgets with increment & decrement buttons
    static bool InputFloat(const char* label, float *val, float change = 1.0f, float min = -1.0f, float max = -1.0f);
    static bool InputInt(const char* label, int *val, int min = -1, int max = -1);

    // Draws ResourceStore images in the interface
    static void ImageList(ResourceStore &store, fArg1_t clickFunc, fRtnArg1_t getNameFunc,
                          fRtnBoolArg1_t verifyFunc = nullptr, fArgNone_t addFunc = nullptr,
                          fArgNone_t contextOptionsFunc = nullptr, fArgNone_t tabsFunc = nullptr);

    // Draws a dropdown listbox
    static bool ListBox(const char* label, VecStr& allItems, int& selected);
    static bool ListBox(const char* label, VecStr& allItems, std::string& selected);

    // Text aligned to the center of the window
    static void TextCentered(const std::string& text);

    // Displays a popup with helpful text
    static void Tooltip(const char* text);
};

template <typename T>
bool Widget::ToggleAddr(const char* label, uint addr, const char* hint, T enabled, T disabled) {
    bool rtn = false;
    bool state = patch::Get<T>(addr) == enabled;

    if (Toggle(label, &state, hint)) {
        patch::Set<T>(addr, state ? enabled : disabled);
        SaveMgr::SaveData(label, addr, state ? SaveMgr::eCheatState::Enabled
                          : SaveMgr::eCheatState::Disabled, enabled, disabled);
        rtn = true;
    }

    return rtn;
}

template <typename T>
void Widget::InputAddr(const char* label, uint address, int min, int def, int max) {
    int val = patch::Get<T>(address, false);
    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() / MENU_WIDTH_FACTOR_X);
    if (ImGui::InputInt(("##SetValue" + std::string(label)).c_str(), &val)) {
        if (val < min) {
            val = min;
        }

        if (val > max) {
            val = max;
        }
        patch::Set<T>(address, val);
        SaveMgr::SaveData<T>(label, address, SaveMgr::eCheatState::Enabled, static_cast<T>(val), 0);
    }
    ImGui::SameLine();
    if (ImGui::Button((TEXT_S("Menu.ResetToDefault") + "##" + label).c_str(), Widget::CalcSizeFrame(TEXT("Menu.ResetToDefault")))) {
        patch::Set<T>(address, def);
        SaveMgr::SaveData<T>(label, address, SaveMgr::eCheatState::Disabled, static_cast<T>(def), 0);
    }
    ImGui::SameLine();
    ImGui::Text(label);
}