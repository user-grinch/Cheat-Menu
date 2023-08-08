#include "slider.h"

namespace ImGuiExtras {

// A modified version of ImGui::SliderScalar
bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        const bool clicked = hovered && ImGui::IsMouseClicked(0, id);
        const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id);
        if (make_active && clicked)
            ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
        if (make_active && temp_input_allowed)
            if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                temp_input_is_active = true;

        if (make_active && !temp_input_is_active)
        {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }
    }

    if (temp_input_is_active)
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

   	// Slider behavior
    ImRect grab_bb;
    const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
	
    // ImGui::RenderNavHighlight(frame_bb, id);
	// Draw frame
	ImRect frame_sz = frame_bb;
	frame_sz.Min.y += 10.0f;
	frame_sz.Max.y -= 10.0f;

	ImRect temp = frame_sz;
	temp.Max.x = grab_bb.Max.x + 1;
    ImGui::RenderFrame(temp.Min, temp.Max, ImGui::GetColorU32(ImGuiCol_FrameBgActive), true, 30.0f);
	temp = frame_sz;
	temp.Min.x = grab_bb.Min.x - 1;
	ImGui::RenderFrame(temp.Min, temp.Max, ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, 30.0f);

    if (value_changed)
        ImGui::MarkItemEdited(id);

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x) {
		float radius = frame_sz.Max.y - frame_sz.Min.y + 3.0f;
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 center = ImVec2(grab_bb.Min.x + (grab_bb.Max.x - grab_bb.Min.x)/2, grab_bb.Max.y + (grab_bb.Min.y - grab_bb.Max.y)/2);
        window->DrawList->AddCircleFilled(center, radius, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), style.GrabRounding);
	}

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("{", "}");
	
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 pos = ImGui::GetMousePos();
        if (pos.x > grab_bb.Min.x && pos.x < grab_bb.Max.x) {
            ImVec2 sz = ImGui::CalcTextSize(value_buf, value_buf_end);
            sz.x += 2 * ImGui::GetStyle().WindowPadding.x;
            sz.y = ImGui::GetTextLineHeight() + 2 * ImGui::GetStyle().WindowPadding.y;

            ImVec2 tooltipPos = ImVec2(grab_bb.GetCenter().x - sz.x/2, grab_bb.GetCenter().y - sz.y / 2 - 30.0f);
            ImGui::SetNextWindowPos(tooltipPos);
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
            ImGui::Begin("Tooltip", NULL, flags);
            ImGui::Text(value_buf);
            ImGui::End();
        }
    } 
    

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
    return value_changed;
}

bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}
}