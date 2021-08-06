#include "pch.h"
#include "Util.h"
#include "Ui.h"

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

bool Ui::ListBoxCustomNames(const char* label, std::vector<std::string>& all_items, std::string& selected, const char* custom_names[], size_t length)
{
	bool rtn = false;
	std::string display_selected = (selected == "All") ? selected : custom_names[std::stoi(selected)];

	if (ImGui::BeginCombo(label, display_selected.c_str()))
	{
		if (ImGui::MenuItem("All"))
		{
			selected = "All";
			rtn = true;
		}

		for (size_t i = 0; i < length; ++i)
		{
			if (ImGui::MenuItem(custom_names[i]))
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
		spacing = false;

	float factor = ImGui::GetStyle().ItemSpacing.x / 2.0f;
	float x;

	if (count == 3)
		factor = ImGui::GetStyle().ItemSpacing.x / 1.403f;

	if (spacing)
		x = ImGui::GetWindowContentRegionWidth() / count - factor;
	else
		x = ImGui::GetWindowContentRegionWidth() / count;

	return ImVec2(x, ImGui::GetFrameHeight() * 1.3f);
}

void Ui::CenterdText(const std::string& text)
{
	float font_size = ImGui::GetFontSize() * text.size() / 2;
	ImGui::NewLine();
	ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 1.8));
	ImGui::Text(text.c_str());
}

void Ui::DrawHeaders(CallbackTable& data)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	short i = 1;
	auto colors = ImGui::GetStyle().Colors;
	ImVec4 btn_col = colors[ImGuiCol_Button];
	static void* func;
	for (auto it = data.begin(); it != data.end(); ++it)
	{
		const char* btn_text = it->first.c_str();

		if (btn_text == Globals::m_HeaderId)
		{
			colors[ImGuiCol_Button] = colors[ImGuiCol_ButtonActive];
			func = it->second;
		}


		if (ImGui::Button(btn_text, GetSize(3, false)))
		{
			Globals::m_HeaderId = btn_text;
			config.SetValue("window.id", Globals::m_HeaderId);
			func = it->second;
		}

		colors[ImGuiCol_Button] = btn_col;

		if (i % 3 != 0)
			ImGui::SameLine();
		i++;
	}
	ImGui::PopStyleVar();
	ImGui::Dummy(ImVec2(0, 10));

	if (Globals::m_HeaderId == "")
	{
		// Show Welcome page
		ImGui::NewLine();

		CenterdText("Welcome to Cheat Menu");
		CenterdText("Author: Grinch_");

		ImGui::NewLine();
		ImGui::TextWrapped("Please ensure you have the latest version from GitHub.");
		ImGui::NewLine();
		if (ImGui::Button("Discord server", ImVec2(GetSize(2))))
			ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);

		ImGui::SameLine();

		if (ImGui::Button("GitHub repo", ImVec2(GetSize(2))))
			ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);

		ImGui::NewLine();
		ImGui::TextWrapped("If you find bugs or have suggestions, you can let me know on discord :)");
		ImGui::NewLine();
		CenterdText("Copyright GPLv3 2019-2021");
	}
	else
	{
		if (func != nullptr && ImGui::BeginChild("TABSBAR"))
		{
			static_cast<void(*)()>(func)();
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
	const ImVec2 text_size = ImGui::CalcTextSize(label, nullptr, true);
	float square_sz = ImGui::GetFrameHeight();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
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
	drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

	// draw hint
	if (hint != nullptr)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::InvisibleButton("?", ImGui::CalcTextSize("?", nullptr, true));
		min = ImGui::GetItemRectMin();
		drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_TextDisabled),
						  "?");

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

void Ui::DrawJSON(CJson& json, std::vector<std::string>& combo_items, std::string& selected_item,
				ImGuiTextFilter& filter,
				std::function<void(std::string&, std::string&, std::string&)> func_left_click,
				std::function<void(std::string&, std::string&, std::string&)> func_right_click)
{
	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2 - 5);
	ListBoxStr("##Categories", combo_items, selected_item);
	ImGui::SameLine();

	filter.Draw("##Filter");
	if (strlen(filter.InputBuf) == 0)
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
		jsonPopup.function = nullptr;


	ImGui::BeginChild(1);
	for (auto root : json.m_Data.items())
	{
		if (root.key() == selected_item || selected_item == "All")
		{
			for (auto _data : root.value().items())
			{
				std::string name = _data.key();
				if (filter.PassFilter(name.c_str()))
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
						jsonPopup.rootKey = root.key();
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
				jsonPopup.function(jsonPopup.rootKey, jsonPopup.key, jsonPopup.value);


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

// clean up the code someday
void Ui::DrawImages(std::vector<std::unique_ptr<STextureStructure>>& img_vec, ImVec2 image_size,
					std::vector<std::string>& category_vec, std::string& selected_item, ImGuiTextFilter& filter,
					std::function<void(std::string&)> on_left_click, std::function<void(std::string&)> on_right_click,
					std::function<std::string(std::string&)> get_name_func,
					std::function<bool(std::string&)> verify_func, const char** custom_names, size_t length)
{
	// scale image size
	image_size.x *= screen::GetScreenWidth() / 1366.0f;
	image_size.y *= screen::GetScreenHeight() / 768.0f;

	int images_in_row = static_cast<int>(ImGui::GetWindowContentRegionWidth() / image_size.x);
	image_size.x = ImGui::GetWindowContentRegionWidth() / images_in_row - static_cast<int>(ImGuiStyleVar_ItemSpacing) *
		0.65f;

	int images_count = 1;

	ImGui::Spacing();

	if (ImGui::IsMouseClicked(1))
		imgPopup.function = nullptr;

	ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2 - 5);
	if (custom_names)
	{
		ListBoxCustomNames("##Categories", category_vec, selected_item, custom_names, length);
	}
	else
	{
		ListBoxStr("##Categories", category_vec, selected_item);
	}

	ImGui::SameLine();
	FilterWithHint("##Filter", filter, "Search");

	ImGui::Spacing();

	ImGui::BeginChild("DrawImages");
	for (uint i = 0; i < img_vec.size(); i++)
	{
		std::string text = img_vec[i]->m_FileName;
		std::string model_name = get_name_func(text);

		if (filter.PassFilter(model_name.c_str())
			&& (img_vec[i]->m_CategoryName == selected_item || selected_item == "All")
			&& (verify_func == nullptr || verify_func(text))
		)
		{
			if (Globals::renderer == Render_DirectX11)
			{
				if (ImGui::MenuItem(model_name.c_str()))
				{
					on_left_click(text);
				}
			}
			else
			{
				if (ImGui::ImageButton(img_vec[i]->m_pTexture, image_size, ImVec2(0, 0), ImVec2(1, 1), 1, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1)))
				{
					on_left_click(text);
				}
			}


			if (ImGui::IsItemClicked(1) && on_right_click != nullptr)
			{
				imgPopup.function = on_right_click;
				imgPopup.value = model_name;
			}

			if (Globals::renderer != Render_DirectX11)
			{
				if (ImGui::IsItemHovered())
				{
					ImDrawList* drawlist = ImGui::GetWindowDrawList();

					ImVec2 btn_min = ImGui::GetItemRectMin();
					ImVec2 btn_max = ImGui::GetItemRectMax();

					drawlist->AddRectFilled(btn_min, btn_max, ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg));

					ImVec2 text_size = ImGui::CalcTextSize(model_name.c_str());
					if (text_size.x < image_size.x)
					{
						float offsetX = (ImGui::GetItemRectSize().x - text_size.x) / 2;
						drawlist->AddText(ImVec2(btn_min.x + offsetX, btn_min.y + 10), ImGui::GetColorU32(ImGuiCol_Text),
										model_name.c_str());
					}
					else
					{
						std::string buff = "";

						std::stringstream ss(model_name);
						short count = 1;

						while (ss >> buff)
						{
							text_size = ImGui::CalcTextSize(buff.c_str());
							float offsetX = (ImGui::GetItemRectSize().x - text_size.x) / 2;
							drawlist->AddText(ImVec2(btn_min.x + offsetX, btn_min.y + 10 * count),
											ImGui::GetColorU32(ImGuiCol_Text), buff.c_str());
							++count;
						}
					}
				}

				if (images_count % images_in_row != 0)
				{
					ImGui::SameLine(0.0, ImGui::GetStyle().ItemInnerSpacing.x);
				}
			}
			images_count++;
		}
	}

	if (imgPopup.function != nullptr)
	{
		if (ImGui::BeginPopupContextWindow())
		{
			ImGui::Text(imgPopup.value.c_str());
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
				imgPopup.function(imgPopup.value);


			if (ImGui::MenuItem("Close"))
				imgPopup.function = nullptr;

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

bool Ui::HotKey(const char* label, HotKeyData& key_data)
{
	bool active = m_CurrentHotkey == label;
	bool state = false;

	if (active)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

		for (int key = 3; key != 135; ++key)
		{
			if (KeyPressed(key))
			{
				key_data.m_key1 = key;
				break;
			}
		}

		for (int key = 135; key != 3; --key)
		{
			if (KeyPressed(key))
			{
				key_data.m_key2 = key;
				break;
			}
		}
	}


	std::string text;

	if (key_data.m_key1 != VK_NONE)
		text = key_names[key_data.m_key1 - 1];
	else
		text = "None";

	if (key_data.m_key1 != key_data.m_key2)
		text += (" + " + key_names[key_data.m_key2 - 1]);

	if (ImGui::Button((text + std::string("##") + std::string(label)).c_str(),
		ImVec2(ImGui::GetWindowContentRegionWidth() / 3.5, ImGui::GetFrameHeight())))
		if (!active)
			m_CurrentHotkey = label;

	if (active && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		m_CurrentHotkey = "";
		state = true;
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		if (ImGui::IsItemHovered())
		{
			key_data.m_key1 = VK_NONE;
			key_data.m_key2 = VK_NONE;
		}
		else
			m_CurrentHotkey = "";

		state = true;
	}

	ImGui::SameLine();
	ImGui::Text(label);

	if (active)
		ImGui::PopStyleColor(2);

	return state;
}

bool Ui::HotKeyPressed(HotKeyData& hotkey)
{
	if (KeyPressed(hotkey.m_key1) && KeyPressed(hotkey.m_key2))
		hotkey.m_bPressed = true;
	else
	{
		if (hotkey.m_bPressed)
		{
			hotkey.m_bPressed = false;
			return m_CurrentHotkey == "";
		}
	}
	return false;
}

std::string Ui::GetHotKeyNameString(HotKeyData& hotkey)
{
	std::string text;

	if (hotkey.m_key1 != VK_NONE)
		text = key_names[hotkey.m_key1 - 1];
	else
		text = "None";

	if (hotkey.m_key1 != hotkey.m_key2)
		text += (" + " + key_names[hotkey.m_key2 - 1]);

	return text;
}

bool Ui::ColorButton(int color_id, std::vector<float>& color, ImVec2 size)
{
	bool rtn = false;
	std::string label = "Color " + std::to_string(color_id);

	if (ImGui::ColorButton(label.c_str(), ImVec4(color[0], color[1], color[2], 1), 0, size))
		rtn = true;

	if (ImGui::IsItemHovered())
	{
		ImDrawList* drawlist = ImGui::GetWindowDrawList();
		drawlist->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
								ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg));
	}

	return rtn;
}
