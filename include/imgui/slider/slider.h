#pragma once
#include "../imgui.h"
#include "../imgui_internal.h"

namespace ImGuiExtras {
    bool SliderScalar(const char* label, float* v, float v_min, float v_max, const char* display_format, float power, ImVec2 size, ImVec4 color);

    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
}