#include "pch.h"
#include "util.h"
#include "ui.h"
#include "../depend/imgui/imgui_internal.h"
#include "menuinfo.h"

// Really messy code, cleanup someday
bool Ui::DrawTitleBar()
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("#CLOSE");

	ImGui::PushFont(FontMgr::GetFont("title"));
	CenterdText(MENU_TITLE);

	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows 
		| ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		ImGui::PopFont();
		return false;
	}

	ImVec2 rectMin = ImGui::GetItemRectMin(); // get pos of title text
	ImGuiStyle& Style = ImGui::GetStyle();
	float framePadding = Style.FramePadding.x;
	float fontSize = ImGui::GetFontSize();
	ImRect title_bar_rect = window->TitleBarRect();
	ImVec2 pos = ImVec2(title_bar_rect.Max.x - framePadding*2 - fontSize, title_bar_rect.Min.y);

	// drawing the close button
    const ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
    ImRect bb_interact = bb;
    const float area_to_visible_ratio = window->OuterRectClipped.GetArea() / bb.GetArea();
	if (area_to_visible_ratio < 1.5f)
	{
		bb_interact.Expand(ImFloor(bb_interact.GetSize() * -0.25f));
	}

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb_interact, id, &hovered, &held);

	float cross_extent = (fontSize * 0.3f) - 1.0f;
	ImVec2 closePos = ImVec2(bb.GetCenter().x - cross_extent, rectMin.y);
    ImU32 closeCol = ImGui::GetColorU32(held || hovered ? ImVec4(0.80f, 0.0f, 0.0f, 1.0f) : ImVec4(0.80f, 0.80f, 0.80f, 1.00f));
	window->DrawList->AddText(closePos, closeCol, "X");
	ImGui::PopFont();

	return pressed;
}

bool Ui::RoundedImageButton(ImTextureID user_texture_id, ImVec2& size, const char* hover_text, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    ImGui::PushID((void*)(intptr_t)user_texture_id);
    const ImGuiID id = window->GetID("#image");
	ImGui::PopID();

    ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : g.Style.FramePadding;

	if (window->SkipItems)
		return false;

	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	// Render
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	ImGui::RenderNavHighlight(bb, id);
	ImGui::RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));
	if (bg_col.w > 0.0f)
		window->DrawList->AddRectFilled(bb.Min + padding, bb.Max - padding, ImGui::GetColorU32(bg_col));

	window->DrawList->AddImageRounded(user_texture_id, bb.Min + padding, bb.Max - padding, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(tint_col), 5.0f);

	if (ImGui::IsItemHovered())
	{
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		// Drawing selected overlay
		ImVec2 btnMin = ImGui::GetItemRectMin();
		ImVec2 btnMax = ImGui::GetItemRectMax();
		drawlist->AddRectFilled(btnMin, btnMax, ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg), 8.0f);

		// Calculating and drawing text over the image
		ImVec2 textSize = ImGui::CalcTextSize(hover_text);
		if (textSize.x < size.x)
		{
			float offsetX = (ImGui::GetItemRectSize().x - textSize.x) / 2;
			drawlist->AddText(ImVec2(btnMin.x + offsetX, btnMin.y + 10), ImGui::GetColorU32(ImGuiCol_Text), hover_text);
		}
		else
		{
			std::string buff = "";
			std::stringstream ss(hover_text);
			short count = 1;

			while (ss >> buff)
			{
				textSize = ImGui::CalcTextSize(buff.c_str());
				float offsetX = (ImGui::GetItemRectSize().x - textSize.x) / 2;
				drawlist->AddText(ImVec2(btnMin.x + offsetX, btnMin.y + 10 * count),
					ImGui::GetColorU32(ImGuiCol_Text), buff.c_str());
				++count;
			}
		}
	}

	return pressed;
}

bool Ui::ListBox(const char* label, std::vector<std::string>& all_items, int& selected)
{
	bool rtn = false;
	if (ImGui::BeginCombo(label, all_items[selected].c_str()))
	{
		for (size_t index = 0; index < all_items.size(); index++)
		{
			if (selected != index)
			{
				if (ImGui::MenuItem(all_items[index].c_str()))
				{
					selected = index;
					rtn = true;
				}
			}
		}
		ImGui::EndCombo();
	}
	return rtn;
}

bool Ui::ListBoxStr(const char* label, std::vector<std::string>& all_items, std::string& selected)
{
	bool rtn = false;
	if (ImGui::BeginCombo(label, selected.c_str()))
	{
		for (std::string current_item : all_items)
		{
			if (ImGui::MenuItem(current_item.c_str()))
			{
				selected = current_item;
				rtn = true;
			}
		}
		ImGui::EndCombo();
	}

	return rtn;
}

bool Ui::ListBoxCustomNames(const char* label, std::vector<std::string>& all_items, std::string& selected, const char* customNames[], size_t length)
{
	bool rtn = false;
	std::string display_selected = (selected == "All") ? selected : customNames[std::stoi(selected)];

	if (ImGui::BeginCombo(label, display_selected.c_str()))
	{
		if (ImGui::MenuItem("All"))
		{
			selected = "All";
			rtn = true;
		}

		for (size_t i = 0; i < length; ++i)
		{
			if (ImGui::MenuItem(customNames[i]))
			{
				selected = std::to_string(i);
				rtn = true;
				break;
			}
		}
		ImGui::EndCombo();
	}
	return rtn;
}

ImVec2 Ui::GetSize(short count, bool spacing)
{
	if (count == 1)
	{
		spacing = false;
	}

	float factor = ImGui::GetStyle().ItemSpacing.x / 2.0f;
	float x;

	if (count == 3)
	{
		factor = ImGui::GetStyle().ItemSpacing.x / 1.403f;
	}

	if (spacing)
	{
		x = ImGui::GetWindowContentRegionWidth() / count - factor;
	}
	else
	{
		x = ImGui::GetWindowContentRegionWidth() / count;
	}

	return ImVec2(x, ImGui::GetFrameHeight() * 1.3f);
}

void Ui::CenterdText(const std::string& text)
{
	ImVec2 size = ImGui::CalcTextSize(text.c_str());
	ImGui::NewLine();
	ImGui::SameLine(
		((ImGui::GetWindowContentRegionWidth() - size.x) / 2)
	);

	ImGui::Text(text.c_str());
}

void Ui::DrawHeaders(CallbackTable& data)
{
	static void* pCallback;
	ImVec2 size = GetSize(3, false);
	ImGuiStyle &style = ImGui::GetStyle();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::PushFont(FontMgr::GetFont("header"));

	ImDrawList *pDrawList = ImGui::GetWindowDrawList();
	for (int i = 0; i < data.size(); ++i)
	{
		const char* btn_text = data[i].first.c_str();

		ImVec4 color;
		if (i == m_HeaderId)
		{
			color = style.Colors[ImGuiCol_ButtonActive];
			pCallback = data[i].second;
		}
		else
		{
			color = style.Colors[ImGuiCol_Button];
		}

		if (ImGui::InvisibleButton(btn_text, size))
		{
			m_HeaderId = i;
			gConfig.SetValue("window.idnum", m_HeaderId);
			pCallback = data[i].second;
		}

		if (ImGui::IsItemHovered())
		{
			color = style.Colors[ImGuiCol_ButtonHovered];
		}

		// hardcoded
		ImDrawFlags flags = ImDrawFlags_RoundCornersNone;
		if (i == 0) flags = ImDrawFlags_RoundCornersTopLeft;
		if (i == 2) flags = ImDrawFlags_RoundCornersTopRight;
		if (i == 6) flags = ImDrawFlags_RoundCornersBottomLeft;
		if (i == 8) flags = ImDrawFlags_RoundCornersBottomRight;
		
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImVec2 size = ImGui::CalcTextSize(btn_text);
		pDrawList->AddRectFilled(min, max, ImGui::GetColorU32(color), style.FrameRounding, flags);
		ImGui::RenderTextClipped(min + style.FramePadding, max - style.FramePadding, btn_text, NULL, &size, style.ButtonTextAlign);

		if (i % 3 != 2)
		{
			ImGui::SameLine();
		}
	}
	ImGui::PopFont();
	ImGui::PopStyleVar();
	ImGui::Dummy(ImVec2(0, 10));

	if (m_HeaderId == -1)
	{
		// Show Welcome page
		ImGui::NewLine();

		CenterdText("Welcome to Cheat Menu");
		CenterdText("Author: Grinch_");

		ImGui::NewLine();
		ImGui::TextWrapped("Please ensure you have the latest version from GitHub.");
		ImGui::NewLine();
		if (ImGui::Button("Discord server", ImVec2(GetSize(2))))
		{
			ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
		}

		ImGui::SameLine();

		if (ImGui::Button("GitHub repo", ImVec2(GetSize(2))))
		{
			ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
		}

		ImGui::NewLine();
		ImGui::TextWrapped("If you find bugs or have suggestions, you can let me know on discord :)");
		ImGui::Dummy(ImVec2(0, 30));
		CenterdText("Copyright Grinch_ 2019-2022. All rights reserved.");
	}
	else
	{
		if (pCallback != nullptr && ImGui::BeginChild("TABSBAR"))
		{
			static_cast<void(*)()>(pCallback)();
			ImGui::EndChild();
		}
	}
}

void Ui::ShowTooltip(const char* text)
{
	ImGui::SameLine();
	ImGui::TextDisabled("?");

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text(text);
		ImGui::EndTooltip();
	}
}

bool Ui::CheckboxWithHint(const char* label, bool* v, const char* hint, bool is_disabled)
{
	// set things up
	bool pressed = false;
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 textSize = ImGui::CalcTextSize(label, nullptr, true);
	float square_sz = ImGui::GetFrameHeight();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	ImU32 color = ImGui::GetColorU32(ImGuiCol_FrameBg);
	std::string slabel = "##InvCheckboxBtn" + std::string(label);

	ImGui::BeginDisabled(is_disabled);
	
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
	drawlist->AddRectFilled(min, max, color, ImGui::GetStyle().FrameRounding);

	int pad = static_cast<int>(square_sz / 6.0);
	pad = (pad < 1) ? 1 : pad;

	if (*v)
	{
		// draw the checkmark
		float sz = (square_sz - pad * 2.0);
		float thickness = sz / 5.0;
		thickness = (thickness < 1.0) ? 1.0 : thickness;
		sz = sz - thickness * 0.5;

		auto pos = ImVec2(min.x + pad, min.y + pad);
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
	if (ImGui::InvisibleButton(label, ImVec2(ImGui::CalcTextSize(label, nullptr, true).x, square_sz)) && !is_disabled)
	{
		pressed = true;
		*v = !*v;
	}
	min = ImGui::GetItemRectMin();
	drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y / 2), ImGui::GetColorU32(ImGuiCol_Text), label);

	// draw hint
	if (hint != nullptr)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::InvisibleButton("?", ImGui::CalcTextSize("?", nullptr, true));
		min = ImGui::GetItemRectMin();
		drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y / 2), ImGui::GetColorU32(ImGuiCol_TextDisabled),
						  "?");

		if (ImGui::IsItemHovered() && !is_disabled)
		{
			ImGui::BeginTooltip();
			ImGui::Text(hint);
			ImGui::Spacing();
			ImGui::EndTooltip();
		}
	}

	ImGui::EndDisabled();

	return pressed;
}

bool Ui::CheckboxAddress(const char* label, const int addr, const char* hint)
{
	bool rtn = false;
	bool state = patch::Get<bool>(addr, false);

	if (CheckboxWithHint(label, &state, hint) && addr != NULL)
	{
		patch::Set<bool>(addr, state, false);
		rtn = true;
	}

	return rtn;
}

bool Ui::CheckboxAddressEx(const char* label, const int addr, int enabled_val, int disabled_val, const char* hint)
{
	bool rtn = false;

	bool state = false;
	int val = 0;
	patch::GetRaw(addr, &val, 1, false);

	if (val == enabled_val)
		state = true;

	if (CheckboxWithHint(label, &state, hint) && addr != NULL)
	{
		if (state)
			patch::SetRaw(addr, &enabled_val, 1, false);
		else
			patch::SetRaw(addr, &disabled_val, 1, false);
		rtn = true;
	}

	return rtn;
}

bool Ui::CheckboxAddressVar(const char* label, bool val, int addr, const char* hint)
{
	bool rtn = false;
	bool state = val;
	if (CheckboxWithHint(label, &state, hint))
	{
		patch::Set<bool>(addr, state, false);
		rtn = true;
	}

	return rtn;
}

bool Ui::CheckboxAddressVarEx(const char* label, bool val, int addr, int enabled_val, int disabled_val,
							  const char* hint)
{
	bool rtn = false;
	bool state = val;
	if (CheckboxWithHint(label, &state, hint))
	{
		if (state)
			patch::SetRaw(addr, &enabled_val, 1, false);
		else
			patch::SetRaw(addr, &disabled_val, 1, false);

		rtn = true;
	}

	return rtn;
}

bool Ui::CheckboxBitFlag(const char* label, uint flag, const char* hint)
{
	bool rtn = false;
	bool state = (flag == 1) ? true : false;
	if (CheckboxWithHint(label, &state, hint))
	{
		flag = state ? 1 : 0;
		rtn = true;
	}

	return rtn;
}

void Ui::DrawJSON(ResourceStore& data,
				std::function<void(std::string&, std::string&, std::string&)> func_left_click,
				std::function<void(std::string&, std::string&, std::string&)> func_right_click)
{
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2 - 5);
	ListBoxStr("##Categories", data.m_Categories, data.m_Selected);
	ImGui::SameLine();

	data.m_Filter.Draw("##Filter");
	if (strlen(data.m_Filter.InputBuf) == 0)
	{
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		ImVec2 min = ImGui::GetItemRectMin();
		min.x += ImGui::GetStyle().FramePadding.x;
		min.y += ImGui::GetStyle().FramePadding.y;

		drawlist->AddText(min, ImGui::GetColorU32(ImGuiCol_TextDisabled), "Search");
	}

	ImGui::PopItemWidth();

	ImGui::Spacing();

	if (ImGui::IsMouseClicked(1))
	{
		jsonPopup.function = nullptr;
	}


	ImGui::BeginChild(1);
	for (auto root : data.m_pJson->m_Data.items())
	{
		if (root.key() == data.m_Selected || data.m_Selected == "All")
		{
			for (auto _data : root.value().items())
			{
				std::string name = _data.key();
				if (data.m_Filter.PassFilter(name.c_str()))
				{
					if (ImGui::MenuItem(name.c_str()) && func_left_click != nullptr)
					{
						std::string root_key = root.key();
						std::string data_key = _data.key();
						std::string data_val = _data.value();

						func_left_click(root_key, data_key, data_val);
					}

					if (ImGui::IsItemClicked(1) && func_right_click != nullptr)
					{
						jsonPopup.function = func_right_click;
						jsonPopup.root = root.key();
						jsonPopup.key = name;
						jsonPopup.value = _data.value();
					}
				}
			}
		}
	}

	if (jsonPopup.function != nullptr)
	{
		if (ImGui::BeginPopupContextWindow())
		{
			ImGui::Text(jsonPopup.key.c_str());
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
				jsonPopup.function(jsonPopup.root, jsonPopup.key, jsonPopup.value);


			if (ImGui::MenuItem("Close"))
				jsonPopup.function = nullptr;

			ImGui::EndPopup();
		}
	}
	ImGui::EndChild();
}

#ifdef GTASA
void Ui::EditStat(const char* label, const int stat_id, const int min, const int def, const int max)
{
	if (ImGui::CollapsingHeader(label))
	{
		int val = static_cast<int>(CStats::GetStatValue(stat_id));

		ImGui::Columns(3, nullptr, false);
		ImGui::Text("Min: %d", min);
		ImGui::NextColumn();
		ImGui::Text("Def: %d", def);
		ImGui::NextColumn();
		ImGui::Text("Max: %d", max);
		ImGui::Columns(1);

		ImGui::Spacing();

		if (ImGui::InputInt(("Set value##" + std::string(label)).c_str(), &val))
			CStats::SetStatValue(stat_id, static_cast<float>(val));

		ImGui::Spacing();

		if (ImGui::Button(("Minimum##" + std::string(label)).c_str(), GetSize(3)))
			CStats::SetStatValue(stat_id, static_cast<float>(min));

		ImGui::SameLine();

		if (ImGui::Button(("Default##" + std::string(label)).c_str(), GetSize(3)))
			CStats::SetStatValue(stat_id, static_cast<float>(def));

		ImGui::SameLine();

		if (ImGui::Button(("Maximum##" + std::string(label)).c_str(), GetSize(3)))
			CStats::SetStatValue(stat_id, static_cast<float>(max));

		ImGui::Spacing();
		ImGui::Separator();
	}
}
#endif

void Ui::FilterWithHint(const char* label, ImGuiTextFilter& filter, const char* hint)
{
	filter.Draw(label);

	if (strlen(filter.InputBuf) == 0)
	{
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		ImVec2 min = ImGui::GetItemRectMin();
		min.x += ImGui::GetStyle().FramePadding.x;
		min.y += ImGui::GetStyle().FramePadding.y;

		drawlist->AddText(min, ImGui::GetColorU32(ImGuiCol_TextDisabled), hint);
	}
}

void Ui::DrawImages(ResourceStore &store, std::function<void(std::string&)> onLeftClick, std::function<void(std::string&)> onRightClick,
					std::function<std::string(std::string&)> getName, std::function<bool(std::string&)> verifyFunc,
					const char** customNames, size_t length)
{
	/*
		Trying to scale images based on resolutions
		Native 1366x768
	*/
	ImVec2 m_ImageSize = store.m_ImageSize;
	m_ImageSize.x *= screen::GetScreenWidth() / 1366.0f;
	m_ImageSize.y *= screen::GetScreenHeight() / 768.0f;

	int imageCount = 1;
	int imagesInRow = static_cast<int>(ImGui::GetWindowContentRegionWidth() / m_ImageSize.x);
	m_ImageSize.x = ImGui::GetWindowContentRegionWidth() / imagesInRow - static_cast<int>(ImGuiStyleVar_ItemSpacing) * 0.65f;

	ImGui::Spacing();

	// Hide the popup if right clicked again
	if (ImGui::IsMouseClicked(1))
	{
		imgPopup.function = nullptr;
	}

	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2 - 5);
	if (customNames)
	{
		ListBoxCustomNames("##Categories", store.m_Categories, store.m_Selected, customNames, length);
	}
	else
	{
		ListBoxStr("##Categories", store.m_Categories, store.m_Selected);
	}

	ImGui::SameLine();
	FilterWithHint("##Filter", store.m_Filter, "Search");

	ImGui::Spacing();

	ImGui::BeginChild("DrawImages");
	if (gRenderer == Render_DirectX9)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(3, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
	}

	for (uint i = 0; i < store.m_ImagesList.size(); ++i)
	{
		std::string text = store.m_ImagesList[i]->m_FileName;
		std::string modelName = getName(text);

		if (store.m_Filter.PassFilter(modelName.c_str())
			&& (store.m_ImagesList[i]->m_CategoryName == store.m_Selected || store.m_Selected == "All")
			&& (verifyFunc == nullptr || verifyFunc(text))
		)
		{
			/*
				Couldn't figure out how to laod images for Dx11
				Using texts for now
			*/
			if (gRenderer == Render_DirectX11)
			{
				if (ImGui::MenuItem(modelName.c_str()))
				{
					onLeftClick(text);
				}
			}
			else
			{
				if (Ui::RoundedImageButton(store.m_ImagesList[i]->m_pTexture, m_ImageSize, modelName.c_str(), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
				{
					onLeftClick(text);
				}
			}

			// Right click popup
			if (ImGui::IsItemClicked(1) && onRightClick != nullptr)
			{
				imgPopup.function = onRightClick;
				imgPopup.value = modelName;
			}

			if (gRenderer != Render_DirectX11)
			{
				if (imageCount % imagesInRow != 0)
				{
					ImGui::SameLine(0.0, ImGui::GetStyle().ItemInnerSpacing.x);
				}
			}
			imageCount++;
		}
	}

	if (gRenderer == Render_DirectX9)
	{
		ImGui::PopStyleVar(4);
	}

	// Draw popup code
	if (imgPopup.function != nullptr)
	{
		if (ImGui::BeginPopupContextWindow())
		{
			ImGui::Text(imgPopup.value.c_str());
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				imgPopup.function(imgPopup.value);
			}

			if (ImGui::MenuItem("Close"))
			{
				imgPopup.function = nullptr;
			}

			ImGui::EndPopup();
		}
	}
	ImGui::EndChild();
}

void Ui::RadioButtonAddress(const char* label, std::vector<NamedMemory>& named_mem)
{
	size_t btn_in_column = named_mem.size() / 2 - 1;

	ImGui::Text(label);
	ImGui::Columns(2, nullptr, false);

	bool state = true;

	for (size_t i = 0; i < named_mem.size(); i++)
	{
		if (patch::Get<bool>(named_mem[i].addr, false))
			state = false;
	}

	if (ImGui::RadioButton((std::string("None##") + label).c_str(), state))
	{
		for (size_t i = 0; i < named_mem.size(); i++)
			patch::Set<bool>(named_mem[i].addr, false);
	}

	for (size_t i = 0; i < named_mem.size(); i++)
	{
		state = patch::Get<bool>(named_mem[i].addr, false);

		if (ImGui::RadioButton(named_mem[i].name.c_str(), state))
		{
			for (size_t i = 0; i < named_mem.size(); i++)
				patch::Set<bool>(named_mem[i].addr, false);

			patch::Set<bool>(named_mem[i].addr, true);
		}

		if (i == btn_in_column)
			ImGui::NextColumn();
	}
	ImGui::Columns(1);
}

void Ui::RadioButtonAddressEx(const char* label, int addr, std::vector<NamedValue>& named_val)
{
	size_t btn_in_column = named_val.size() / 2;

	ImGui::Text(label);
	ImGui::Columns(2, nullptr, false);

	int mem_val = 0;
	patch::GetRaw(addr, &mem_val, 1, false);

	for (size_t i = 0; i < named_val.size(); i++)
	{
		if (ImGui::RadioButton(named_val[i].name.c_str(), &mem_val, named_val[i].value))
			patch::SetRaw(addr, &named_val[i].value, 1, false);

		if (i == btn_in_column)
			ImGui::NextColumn();
	}
	ImGui::Columns(1);
}

void Ui::EditRadioButtonAddress(const char* label, std::vector<NamedMemory>& named_mem)
{
	if (ImGui::CollapsingHeader(label))
	{
		RadioButtonAddress(label, named_mem);
		ImGui::Spacing();
		ImGui::Separator();
	}
}

void Ui::EditRadioButtonAddressEx(const char* label, int addr, std::vector<NamedValue>& named_val)
{
	if (ImGui::CollapsingHeader(label))
	{
		RadioButtonAddressEx(label, addr, named_val);
		ImGui::Spacing();
		ImGui::Separator();
	}
}

void Ui::ColorPickerAddress(const char* label, int base_addr, ImVec4&& default_color)
{
	if (ImGui::CollapsingHeader(label))
	{
		float cur_color[4];
		cur_color[0] = patch::Get<BYTE>(base_addr, false);
		cur_color[1] = patch::Get<BYTE>(base_addr + 1, false);
		cur_color[2] = patch::Get<BYTE>(base_addr + 2, false);
		cur_color[3] = patch::Get<BYTE>(base_addr + 3, false);

		// 0-255 -> 0-1
		cur_color[0] /= 255;
		cur_color[1] /= 255;
		cur_color[2] /= 255;
		cur_color[3] /= 255;

		if (ImGui::ColorPicker4(std::string("Pick color##" + std::string(label)).c_str(), cur_color))
		{
			// 0-1 -> 0-255
			cur_color[0] *= 255;
			cur_color[1] *= 255;
			cur_color[2] *= 255;
			cur_color[3] *= 255;

			patch::Set<BYTE>(base_addr, cur_color[0], false);
			patch::Set<BYTE>(base_addr + 1, cur_color[1], false);
			patch::Set<BYTE>(base_addr + 2, cur_color[2], false);
			patch::Set<BYTE>(base_addr + 3, cur_color[3], false);
		}
		ImGui::Spacing();

		if (ImGui::Button("Reset to default", GetSize()))
		{
			patch::Set<BYTE>(base_addr, default_color.x, false);
			patch::Set<BYTE>(base_addr + 1, default_color.y, false);
			patch::Set<BYTE>(base_addr + 2, default_color.z, false);
			patch::Set<BYTE>(base_addr + 3, default_color.w, false);
		}

		ImGui::Spacing();
		ImGui::Separator();
	}
}

void Ui::EditBits(const char* label, const int address, const std::vector<std::string>& names)
{
	auto mem_val = (int*)address;

	if (ImGui::CollapsingHeader(label))
	{
		ImGui::Columns(2, nullptr, false);

		for (int i = 0; i < 32; ++i)
		{
			int mask = 1 << i;
			bool state = *mem_val & mask;

			if (ImGui::Checkbox(names[i].c_str(), &state))
				*mem_val ^= mask;

			if (i + 1 == 32 / 2)
				ImGui::NextColumn();
		}
		ImGui::Columns(1);

		ImGui::Spacing();
		ImGui::Separator();
	}
}

void Ui::EditFloat(const char* label, const int address, const float min, const float def, const float max,
				   const float mul, const float change)
{
	if (ImGui::CollapsingHeader(label))
	{
		float val = patch::Get<float>(address, false) * mul;

		int items = 3;

		if (min == def)
			items = 2;

		ImGui::Columns(items, nullptr, false);

		ImGui::Text("Min: %f", min);

		if (items == 3)
		{
			ImGui::NextColumn();
			ImGui::Text("Def: %f", def);
		}

		ImGui::NextColumn();
		ImGui::Text("Max: %f", max);
		ImGui::Columns(1);

		ImGui::Spacing();

		int size = ImGui::GetFrameHeight();

		if (ImGui::InputFloat(("##" + std::string(label)).c_str(), &val))
			patch::SetFloat(address, val / mul, false);

		ImGui::SameLine(0.0, 4.0);
		if (ImGui::Button("-", ImVec2(size, size)) && (val - change) > min)
		{
			val -= change;
			patch::SetFloat(address, val / mul, false);
		}
		ImGui::SameLine(0.0, 4.0);
		if (ImGui::Button("+", ImVec2(size, size)) && (val + change) < max)
		{
			val += change;
			patch::SetFloat(address, val / mul, false);
		}
		ImGui::SameLine(0.0, 4.0);
		ImGui::Text("Set");


		ImGui::Spacing();

		if (ImGui::Button(("Minimum##" + std::string(label)).c_str(), GetSize(items)))
			patch::Set<float>(address, min / mul, false);

		if (items == 3)
		{
			ImGui::SameLine();

			if (ImGui::Button(("Default##" + std::string(label)).c_str(), GetSize(items)))
				patch::Set<float>(address, def / mul, false);
		}

		ImGui::SameLine();

		if (ImGui::Button(("Maximum##" + std::string(label)).c_str(), GetSize(items)))
			patch::Set<float>(address, max / mul, false);

		ImGui::Spacing();
		ImGui::Separator();
	}
}

bool Ui::ColorButton(int color_id, std::vector<float>& color, ImVec2 size)
{
	bool rtn = false;
	std::string label = "Color " + std::to_string(color_id);

	if (ImGui::ColorButton(label.c_str(), ImVec4(color[0], color[1], color[2], 1), 0, size))
	{
		rtn = true;
	}

	if (ImGui::IsItemHovered())
	{
		ImDrawList* drawlist = ImGui::GetWindowDrawList();
		drawlist->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
								ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg));
	}

	return rtn;
}
