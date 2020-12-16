#include "pch.h"
#include "CustomWidgets.h"

// Checkbox with support for hints, disable
bool CustomWidgets::Checkbox(const char * label, bool *v, const char * hint, bool is_disabled)
{
	// set things up
	bool pressed = false;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 text_size = ImGui::CalcTextSize(label, NULL, true);
	float square_sz = ImGui::GetFrameHeight();
	ImDrawList *drawlist = ImGui::GetWindowDrawList();
	ImU32 color = ImGui::GetColorU32(ImGuiCol_FrameBg);
	std::string slabel = "##InvCheckboxBtn" + std::string(label);

	if (is_disabled)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

	// process the button states
	if (ImGui::InvisibleButton(slabel.c_str(), ImVec2(square_sz, square_sz)) && !is_disabled)
	{
		pressed = true;
		*v = !*v;
	}

	if (ImGui::IsItemHovered() && !is_disabled)
		color = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);


	// draw the button
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	drawlist->AddRectFilled(min, max, color);

	int pad = int(square_sz / 6.0);
	pad = (pad < 1) ? 1 : pad;

	if (*v)
	{   // draw the checkmark
		float sz = (square_sz - pad * 2.0);
		float thickness = sz / 5.0;
		thickness = (thickness < 1.0) ? 1.0 : thickness;
		sz = sz - thickness * 0.5;

		ImVec2 pos = ImVec2(min.x + pad, min.y + pad);
		pos.x = pos.x + thickness * 0.25;
		pos.y = pos.y + thickness * 0.25;

		float third = sz / 3.0;
		float bx = pos.x + third;
		float by = pos.y + sz - third * 0.5;

		drawlist->PathLineTo(ImVec2(bx - third, by - third));
		drawlist->PathLineTo(ImVec2(bx, by));
		drawlist->PathLineTo(ImVec2(bx + third * 2.0, by - third * 2.0));
		drawlist->PathStroke(ImGui::GetColorU32(ImGuiCol_CheckMark), false, thickness);
	}

	// draw label
	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	if (ImGui::InvisibleButton(label, ImVec2(ImGui::CalcTextSize(label, NULL, true).x, square_sz)) && !is_disabled)
	{
		pressed = true;
		*v = !*v;
	}
	min = ImGui::GetItemRectMin();
	drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

	// draw hint
	if (hint != nullptr)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::InvisibleButton("?", ImGui::CalcTextSize("?", NULL, true));
		min = ImGui::GetItemRectMin();
		drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_TextDisabled), "?");

		if (ImGui::IsItemHovered() && !is_disabled)
		{
			ImGui::BeginTooltip();
			ImGui::Text(hint);
			ImGui::Spacing();
			ImGui::EndTooltip();
		}
	}

	if (is_disabled)
		ImGui::PopStyleVar();

	return pressed;
}
