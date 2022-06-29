#pragma once
#include "pch.h"

/*
    Widgets Class
    Contains useful ui utilities 

TODO:
    Replace the horrible Ui class
*/
class Widget
{
public:
    Widget() = delete;
    Widget(Widget&) = delete;

    // Calculates button size based on window width & spacing flags
    static ImVec2 CalcSize(short count = 1, bool spacing = true);

    // Draws DataStore data in the interface
    static void DataList(ResourceStore& data, ArgCallback3 clickFunc, ArgCallback3 removeFunc);

    // ImGui::TextFilter with hint support
    static void FilterWithHint(const char* label, ImGuiTextFilter& filter, const char* hint);

    // Draws ResourceStore images in the interface
    static void ImageList(ResourceStore &store, ArgCallback leftClickFunc, ArgCallback rightClickFunc,
                           ArgCallbackRtn getNameFunc, ArgCallbackRtnBool verifyFunc = nullptr);

    // Text aligned to the center of the window
    static void TextCentered(const std::string& text);

    // Displays a popup with helpful text
    static void Tooltip(const char* text);
};