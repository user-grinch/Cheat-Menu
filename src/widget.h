#pragma once
#include "pch.h"

/*
    Widgets Class
    Contains useful ui utilities 
*/
class Widget
{
private: 
    using VecStr = const std::vector<std::string>;

public:
    struct BindInfo
    {
        std::string name;
        uint val;
    };

    Widget() = delete;
    Widget(Widget&) = delete;

    // Calculates button size based on window width & spacing flags
    static ImVec2 CalcSize(short count = 1, bool spacing = true);

    // Regular checkbox with hint support
    static bool Checkbox(const char* label, bool* state, const char* hint = nullptr, 
                            bool is_disabled = false);
    
    // Checkbox for bool memory address
    static bool CheckboxAddr(const char* label, uint addr, const char* hint = nullptr);

    // Checkbox with raw memory input
    static bool CheckboxAddrRaw(const char* label, uint addr, uint size, const char* enabled, 
                                    const char* disabled, const char* hint = nullptr);
    
    // Checkbox for bit fields
    static bool CheckboxBits(const char* label, uint flag, const char* hint = nullptr);

    // Displays a button with specified color id
    static bool ColorBtn(int colorId, std::vector<float>& color, ImVec2 size);

    // Draws DataStore data in the interface
    static void DataList(ResourceStore& data, ArgCallback3 clickFunc, ArgCallback3 removeFunc, 
                            ArgCallbackNone addFunc = nullptr);
    
    // Draws a dropdown editor for memory address
    template <typename T>
    static void EditAddr(const char* label, uint address, int min = 0, int def = 0, int max = 100);
    static void EditAddr(const char* label, uint address, float min = 0.0f, float def = 0.0f, 
                            float max = 100.0f, float mul = 1, float change = 1.0f);

    // Draws a dropdown editor for memory bits
    static void EditBits(const char* label, int address, VecStr& names);

    // Draws radio button list from address
    static void EditRadioBtnAddr(const char* label, std::vector<BindInfo>& addrInfo);
    static void EditRadioBtnAddr(const char* label, uint addr, std::vector<BindInfo>& valInfo);

#ifdef GTASA
    // Draws a dropdown editor for editing stats, only sa
    static void EditStat(const char* label, int stat_id, int min = 0, int def = 0, int max = 1000);
#endif

    // ImGui::TextFilter with hint support
    static void Filter(const char* label, ImGuiTextFilter& filter, const char* hint);

    // Input widgets with increment & decrement buttons
    static bool InputFloat(const char* label, float *val, float change = 1.0f, float min = -1.0f, float max = -1.0f);
    static bool InputInt(const char* label, int *val, int change = 1, int min = -1, int max = -1);

    // Draws ResourceStore images in the interface
    static void ImageList(ResourceStore &store, ArgCallback clickFunc, ArgCallbackRtn getNameFunc, 
                            ArgCallbackRtnBool verifyFunc = nullptr, ArgCallbackNone addFunc = nullptr);

    // Draws a dropdown listbox
    static bool ListBox(const char* label, VecStr& allItems, int& selected);
    static bool ListBox(const char* label, VecStr& allItems, std::string& selected);

    // Text aligned to the center of the window
    static void TextCentered(const std::string& text);

    // Displays a popup with helpful text
    static void Tooltip(const char* text);
};

template <typename T>
void Widget::EditAddr(const char* label, uint address, int min, int def, int max)
{
    if (ImGui::CollapsingHeader(label))
    {
        int val = patch::Get<T>(address, false);

        int items = 3;

        if (min == def)
        {
            items = 2;
        }

        ImGui::Columns(items, nullptr, false);
        ImGui::Text(("Min: " + std::to_string(min)).c_str());

        if (items == 3)
        {
            ImGui::NextColumn();
            ImGui::Text(("Def: " + std::to_string(def)).c_str());
        }

        ImGui::NextColumn();
        ImGui::Text(("Max: " + std::to_string(max)).c_str());
        ImGui::Columns(1);

        ImGui::Spacing();

        if (ImGui::InputInt(("Set value##" + std::string(label)).c_str(), &val))
        {
            if (val < min)
            {
                val = min;
            }

            if (val > max)
            {
                val = max;
            }
            patch::Set<T>(address, val, false);
        }

        ImGui::Spacing();

        if (ImGui::Button(("Minimum##" + std::string(label)).c_str(), CalcSize(items)))
        {
            patch::Set<T>(address, min, false);
        }

        if (items == 3)
        {
            ImGui::SameLine();

            if (ImGui::Button(("Default##" + std::string(label)).c_str(), CalcSize(3)))
            {
                patch::Set<T>(address, def, false);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(("Maximum##" + std::string(label)).c_str(), CalcSize(items)))
        {
            patch::Set<T>(address, max, false);
        }

        ImGui::Spacing();
        ImGui::Separator();
    }
}