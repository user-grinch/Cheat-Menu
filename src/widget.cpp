#include "pch.h"
#include "widget.h"
#include "menu.h"

static struct
{
    std::string root, key, val;
    void* func = nullptr;
} contextMenu;

ImVec2 Widget::CalcSize(short count, bool spacing)
{
    if (count == 1)
    {
        spacing = false;
    }

    // manually tested values
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

void Widget::TextCentered(const std::string& text)
{
    ImVec2 size = ImGui::CalcTextSize(text.c_str());
    ImGui::NewLine();
    float width = ImGui::GetWindowContentRegionWidth() - size.x;
    ImGui::SameLine(width/2);
    ImGui::Text(text.c_str());
}

void Widget::Tooltip(const char* text)
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

void Widget::Filter(const char* label, ImGuiTextFilter& filter, const char* hint)
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

void Widget::DataList(ResourceStore& data, ArgCallback3 clickFunc, ArgCallback3 removeFunc)
{
    if (ImGui::IsMouseClicked(1))
    {
        contextMenu.func = nullptr;
    }

    // Drawing the list here
    if (ImGui::BeginTabBar("MYTABS"))
    {
        if (ImGui::BeginTabItem(TEXT("Window.Search")))
        {
            ImGui::Spacing();
            // Category box
            ImGui::PushItemWidth((ImGui::GetWindowContentRegionWidth() - ImGui::GetStyle().ItemSpacing.x)/2);
            Widget::ListBox("##Categories", data.m_Categories, data.m_Selected);
            ImGui::SameLine();
            Filter("##Filter", data.m_Filter, TEXT("Window.Search"));
            ImGui::PopItemWidth();

            ImGui::Spacing();
            ImGui::BeginChild(1);
            for (auto [k, v] : data.m_pData->Items())
            {
                if (k.str() == data.m_Selected || data.m_Selected == "All")
                {
                    for (auto [k2, v2] : v.as_table()->ref<DataStore::Table>())
                    {
                        std::string key = std::string(k2.str());
                        if (data.m_Filter.PassFilter(key.c_str()))
                        {
                            std::string root = std::string(k.str());
                            std::string val = v2.value_or<std::string>("Unkonwn");
                            if (ImGui::MenuItem(key.c_str()) && clickFunc != nullptr)
                            {
                                clickFunc(root, key, val);
                            }

                            if (ImGui::IsItemClicked(1))
                            {
                                contextMenu = {root, key, val, removeFunc};
                            }
                        }
                    }
                }
            }
            if (contextMenu.func != nullptr)
            {
                if (ImGui::BeginPopupContextWindow())
                {
                    ImGui::Text(contextMenu.key.c_str());
                    ImGui::Separator();
                    if (ImGui::MenuItem(TEXT("Menu.Favourites")))
                    {
                        data.m_pData->Set(std::format("Favourites.{}", contextMenu.key).c_str(), contextMenu.val);
                        data.m_pData->Save();
                        SetHelpMessage(TEXT("Menu.FavouritesText"));
                    }
                    if (ImGui::MenuItem(TEXT("Menu.Remove")))
                    {
                        static_cast<ArgCallback3>(contextMenu.func)(contextMenu.root, contextMenu.key, contextMenu.val);
                    }
                    if (ImGui::MenuItem(TEXT("Menu.Close")))
                    {
                        contextMenu.func = nullptr;
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.FavouritesTab")))
        {
            ImGui::Spacing();
            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
            Filter("##Filter", data.m_Filter, TEXT("Window.Search"));
            ImGui::PopItemWidth();
            ImGui::Spacing();
            ImGui::BeginChild(1);
            size_t count = 0;
            for (auto [k, v] : *data.m_pData->GetTable("Favourites"))
            {
                std::string key = std::string(k.str());
                if (data.m_Filter.PassFilter(key.c_str()))
                {
                    std::string val = v.value_or<std::string>("Unkonwn");
                    if (ImGui::MenuItem(key.c_str()) && clickFunc != nullptr)
                    {
                        std::string str = "Favourites";
                        clickFunc(str, key, val);
                    }

                    if (ImGui::IsItemClicked(1))
                    {
                        contextMenu = {std::string("Favourites"), key, val, removeFunc};
                    }
                }
                ++count;
            }
            if (count == 0)
            {
                Widget::TextCentered(TEXT("Menu.FavouritesNone"));
            }
            if (contextMenu.func != nullptr)
            {
                if (ImGui::BeginPopupContextWindow())
                {
                    ImGui::Text(contextMenu.key.c_str());
                    ImGui::Separator();
                    if (ImGui::MenuItem(TEXT("Menu.FavouritesRemove")))
                    {
                        data.m_pData->RemoveKey("Favourites", contextMenu.key.c_str());
                        data.m_pData->Save();
                        SetHelpMessage(TEXT("Menu.FavouritesRemoveText"));
                    }
                    if (ImGui::MenuItem(TEXT("Menu.Close")))
                    {
                        contextMenu.func = nullptr;
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

static bool RoundedImageButton(ImTextureID user_texture_id, ImVec2& size, const char* hover_text)
{
    // Default to using texture ID as ID. User can still push string/integer prefixes.

    // Creating a invisible button as placeholder
    ImGui::InvisibleButton(hover_text, size);
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    drawList->AddImageRounded(user_texture_id, min, max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 5.0f);
    
    // Add selection overlay and stuff on hover
    if (ImGui::IsItemHovered())
    {
        drawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg), 5.6f);

        // Calculating and drawing text over the image
        ImVec2 textSize = ImGui::CalcTextSize(hover_text);
        if (textSize.x < size.x)
        {
            float offsetX = (ImGui::GetItemRectSize().x - textSize.x) / 2;
            drawList->AddText(ImVec2(min.x + offsetX, min.y + 10), ImGui::GetColorU32(ImGuiCol_Text), hover_text);
        }
        else
        {
            std::string buf = "";
            std::stringstream ss(hover_text);
            short count = 1;

            while (ss >> buf)
            {
                textSize = ImGui::CalcTextSize(buf.c_str());
                float offsetX = (ImGui::GetItemRectSize().x - textSize.x) / 2;
                drawList->AddText(ImVec2(min.x + offsetX, min.y + 10 * count),
                                  ImGui::GetColorU32(ImGuiCol_Text), buf.c_str());
                ++count;
            }
        }
    }

    return ImGui::IsItemClicked(0);
}

void Widget::ImageList(ResourceStore &store, ArgCallback leftClickFunc, ArgCallback rightClickFunc,
                    ArgCallbackRtn getNameFunc, ArgCallbackRtnBool verifyFunc)
{
    ImGuiStyle& style =  ImGui::GetStyle();
    /*
    	Trying to scale images based on resolutions
    	Native 1366x768
    */
    ImVec2 m_ImageSize = store.m_ImageSize;
    m_ImageSize.x *= screen::GetScreenWidth() / 1366.0f;
    m_ImageSize.y *= screen::GetScreenHeight() / 768.0f;

    int imageCount = 1;
    int imagesInRow = static_cast<int>(ImGui::GetWindowContentRegionWidth() / m_ImageSize.x);
    m_ImageSize.x = ImGui::GetWindowContentRegionWidth() - style.ItemSpacing.x * (imagesInRow-1);
    m_ImageSize.x /= imagesInRow;

    bool showImages = !Menu::m_bTextOnlyMode;
    if (gRenderer == Render_DirectX11)
    {
        showImages = false;
    }

    ImGui::Spacing();

    // Hide the popup if right clicked again
    if (ImGui::IsMouseClicked(1))
    {
        contextMenu.func = nullptr;
    }

    ImGui::PushItemWidth((ImGui::GetWindowContentRegionWidth() - style.ItemSpacing.x)/2);
    Widget::ListBox("##Categories", store.m_Categories, store.m_Selected);
    ImGui::SameLine();
    Filter("##Filter", store.m_Filter, "Search");

    ImGui::Spacing();

    ImGui::BeginChild("DrawImages");
    if (showImages)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(3, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
    }

    // Draw images here
    for (uint i = 0; i < store.m_ImagesList.size(); ++i)
    {
        std::string text = store.m_ImagesList[i]->m_FileName;
        std::string modelName = getNameFunc(text);

        if (store.m_Filter.PassFilter(modelName.c_str())
            && (store.m_ImagesList[i]->m_CategoryName == store.m_Selected || store.m_Selected == "All")
            && (verifyFunc == nullptr || verifyFunc(text))
           )
        {
            /*
            	Couldn't figure out how to laod images for Dx11
            	Using texts for now
            */
            if (showImages)
            {
                if (RoundedImageButton(store.m_ImagesList[i]->m_pTexture, m_ImageSize, modelName.c_str()))
                {
                    leftClickFunc(text);
                }
                
            }
            else
            {
                if (ImGui::MenuItem(modelName.c_str()))
                {
                    leftClickFunc(text);
                }
            }

            // Right click popup
            if (ImGui::IsItemClicked(1) && rightClickFunc != nullptr)
            {
                contextMenu.func = rightClickFunc;
                contextMenu.val = modelName;
            }

            if (showImages)
            {
                if (imageCount % imagesInRow != 0)
                {
                    ImGui::SameLine(0.0, style.ItemInnerSpacing.x);
                }
            }
            imageCount++;
        }
    }

    if (showImages)
    {
        ImGui::PopStyleVar(4);
    }

    // Draw popup code
    if (contextMenu.func != nullptr)
    {
        if (ImGui::BeginPopupContextWindow())
        {
            ImGui::Text(contextMenu.val.c_str());
            ImGui::Separator();
            if (ImGui::MenuItem("Remove"))
            {
                static_cast<ArgCallback>(contextMenu.func)(contextMenu.val);
            }

            if (ImGui::MenuItem("Close"))
            {
                contextMenu.func = nullptr;
            }

            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
}

bool Widget::ColorBtn(int colorId, std::vector<float>& color, ImVec2 size)
{
    bool rtn = false;
    std::string label = "Color " + std::to_string(colorId);

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


bool Widget::Checkbox(const char* label, bool* v, const char* hint, bool is_disabled)
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
    {
        color = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
    }

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

bool Widget::CheckboxAddr(const char* label, uint addr, const char* hint)
{
    bool rtn = false;
    bool state = patch::Get<bool>(addr, false);

    if (Checkbox(label, &state, hint) && addr != NULL)
    {
        patch::Set<bool>(addr, state, false);
        rtn = true;
    }

    return rtn;
}

bool Widget::CheckboxAddrRaw(const char* label, uint addr, size_t size, const char* enabled, const char* disabled, const char* hint)
{
    bool rtn = false;
    char* buf = new char[size+1];
    patch::GetRaw(addr, buf, size);
    buf[size] = '\0';
    bool state = !strcmp(buf, enabled);

    if (Checkbox(label, &state, hint))
    {
        if (state)
        {
            patch::SetRaw(addr, const_cast<char*>(enabled), size);
        }
        else
        {
            patch::SetRaw(addr, const_cast<char*>(disabled), size);
        }
        rtn = true;
    }
    delete buf;

    return rtn;
}

bool Widget::CheckboxBits(const char* label, uint flag, const char* hint)
{
    bool rtn = false;
    bool state = (flag == 1) ? true : false;
    if (Checkbox(label, &state, hint))
    {
        flag = state ? 1 : 0;
        rtn = true;
    }

    return rtn;
}

void Widget::EditAddr(const char* label, uint address, float min, float def, float max, float mul, float change)
{
    if (ImGui::CollapsingHeader(label))
    {
        float val = patch::Get<float>(address, false) * mul;

        int items = 3;

        if (min == def)
        {
            items = 2;
        }

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
        {
            patch::SetFloat(address, val / mul, false);
        }

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

        if (ImGui::Button(("Minimum##" + std::string(label)).c_str(), CalcSize(items)))
        {
            patch::Set<float>(address, min / mul, false);
        }

        if (items == 3)
        {
            ImGui::SameLine();

            if (ImGui::Button(("Default##" + std::string(label)).c_str(), CalcSize(items)))
            {
                patch::Set<float>(address, def / mul, false);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(("Maximum##" + std::string(label)).c_str(), CalcSize(items)))
        {
            patch::Set<float>(address, max / mul, false);
        }

        ImGui::Spacing();
        ImGui::Separator();
    }
}


void Widget::EditBits(const char* label, const int address, VecStr& names)
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
            {
                *mem_val ^= mask;
            }

            if (i + 1 == 32 / 2)
            {
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
    }
}

#ifdef GTASA
void Widget::EditStat(const char* label, const int stat_id, const int min, const int def, const int max)
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
        {
            CStats::SetStatValue(stat_id, static_cast<float>(val));
        }

        ImGui::Spacing();

        if (ImGui::Button(("Minimum##" + std::string(label)).c_str(), CalcSize(3)))
        {
            CStats::SetStatValue(stat_id, static_cast<float>(min));
        }

        ImGui::SameLine();

        if (ImGui::Button(("Default##" + std::string(label)).c_str(), CalcSize(3)))
        {
            CStats::SetStatValue(stat_id, static_cast<float>(def));
        }

        ImGui::SameLine();

        if (ImGui::Button(("Maximum##" + std::string(label)).c_str(), CalcSize(3)))
        {
            CStats::SetStatValue(stat_id, static_cast<float>(max));
        }

        ImGui::Spacing();
        ImGui::Separator();
    }
}
#endif

bool Widget::ListBox(const char* label, VecStr& allItems, std::string& selected)
{
    bool rtn = false;
    if (ImGui::BeginCombo(label, selected.c_str()))
    {
        for (std::string curItem : allItems)
        {
            if (ImGui::MenuItem(curItem.c_str()))
            {
                selected = curItem;
                rtn = true;
            }
        }
        ImGui::EndCombo();
    }

    return rtn;
}

bool Widget::ListBox(const char* label, VecStr& allItems, int& selected)
{
    bool rtn = false;
    if (ImGui::BeginCombo(label, std::to_string(selected).c_str()))
    {
        for (size_t index = 0; index < allItems.size(); ++index)
        {
            if (ImGui::MenuItem(allItems[index].c_str()))
            {
                selected = index;
                rtn = true;
            }
        }
        ImGui::EndCombo();
    }

    return rtn;
}

void Widget::EditRadioBtnAddr(const char* label, std::vector<BindInfo>& addrInfo)
{
    if (ImGui::CollapsingHeader(label))
    {
        size_t btnsInColumn = addrInfo.size() / 2 - 1;

        ImGui::Columns(2, nullptr, false);

        bool state = true;

        for (size_t i = 0; i < addrInfo.size(); i++)
        {
            if (patch::Get<bool>(addrInfo[i].val, false))
            {
                state = false;
            }
        }

        if (ImGui::RadioButton((std::string("None##") + label).c_str(), state))
        {
            for (size_t i = 0; i < addrInfo.size(); i++)
            {
                patch::Set<bool>(addrInfo[i].val, false);
            }
        }

        for (size_t i = 0; i < addrInfo.size(); i++)
        {
            state = patch::Get<bool>(addrInfo[i].val, false);

            if (ImGui::RadioButton(addrInfo[i].name.c_str(), state))
            {
                for (size_t i = 0; i < addrInfo.size(); i++)
                {
                    patch::Set<bool>(addrInfo[i].val, false);
                }

                patch::Set<bool>(addrInfo[i].val, true);
            }

            if (i == btnsInColumn)
            {
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::Spacing();
        ImGui::Separator();
    }
}

void Widget::EditRadioBtnAddr(const char* label, uint addr, std::vector<BindInfo>& valInfo)
{
    if (ImGui::CollapsingHeader(label))
    {
        size_t btnsInColumn = valInfo.size() / 2;
        ImGui::Columns(2, nullptr, false);

        int mem_val = 0;
        patch::GetRaw(addr, &mem_val, 1, false);

        for (size_t i = 0; i < valInfo.size(); i++)
        {
            if (ImGui::RadioButton(valInfo[i].name.c_str(), &mem_val, valInfo[i].val))
            {
                patch::SetRaw(addr, &valInfo[i].val, 1, false);
            }

            if (i == btnsInColumn)
            {
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::Spacing();
        ImGui::Separator();
    }
}