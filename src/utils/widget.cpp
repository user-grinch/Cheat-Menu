#include "pch.h"
#include "widget.h"
#include "pages/menu.h"
#include "savemgr.h"
#include "imgui/toggle/imgui_toggle.h"
#include "imgui/toggle/imgui_toggle_presets.h"

static struct {
    std::string root, key, val;
    bool show = false;
    bool added = false;
} contextMenu;

ImVec2 Widget::CalcSize(short count, bool spacing) {
    if (count == 1) {
        spacing = false;
    }

    // manually tested values
    float factor = ImGui::GetStyle().ItemSpacing.x / 2.0f;
    float x;

    if (count == 3) {
        factor = ImGui::GetStyle().ItemSpacing.x / 1.403f;
    }

    if (spacing) {
        x = ImGui::GetWindowContentRegionWidth() / count - factor;
    } else {
        x = ImGui::GetWindowContentRegionWidth() / count;
    }

    return ImVec2(x, ImGui::GetFrameHeight() * 1.3f);
}

ImVec2 Widget::CalcSizeFrame(const char* text) {
    return ImVec2(ImGui::CalcTextSize(text).x + 2 * ImGui::GetStyle().ItemSpacing.x , ImGui::GetFrameHeight());
}

void Widget::TextCentered(const std::string& text) {
    ImVec2 size = ImGui::CalcTextSize(text.c_str());
    ImGui::NewLine();
    float width = ImGui::GetWindowContentRegionWidth() - size.x;
    ImGui::SameLine(width/2);
    ImGui::Text(text.c_str());
}

void Widget::Tooltip(const char* text) {
    ImGui::SameLine();
    ImGui::TextDisabled("?");

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(text);
        ImGui::EndTooltip();
    }
}


void Widget::ColorPickerAddr(const char* label, int addr, ImVec4&& default_color) {
    float cur_color[4];
    cur_color[0] = patch::Get<BYTE>(addr, false);
    cur_color[1] = patch::Get<BYTE>(addr + 1, false);
    cur_color[2] = patch::Get<BYTE>(addr + 2, false);
    cur_color[3] = patch::Get<BYTE>(addr + 3, false);

    // 0-255 -> 0-1
    cur_color[0] /= 255;
    cur_color[1] /= 255;
    cur_color[2] /= 255;
    cur_color[3] /= 255;

    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() / MENU_WIDTH_FACTOR_X);
    if (ImGui::ColorEdit4(std::string("##Pick color" + std::string(label)).c_str(), cur_color)) {
        // 0-1 -> 0-255
        cur_color[0] *= 255;
        cur_color[1] *= 255;
        cur_color[2] *= 255;
        cur_color[3] *= 255;

        patch::Set<BYTE>(addr, cur_color[0], false);
        patch::Set<BYTE>(addr + 1, cur_color[1], false);
        patch::Set<BYTE>(addr + 2, cur_color[2], false);
        patch::Set<BYTE>(addr + 3, cur_color[3], false);
    }
    ImGui::SameLine();
    if (ImGui::Button((TEXT_S("Menu.ResetToDefault") + "##" + label).c_str(), Widget::CalcSizeFrame(TEXT("Menu.ResetToDefault")))) {
        patch::Set<BYTE>(addr, default_color.x, false);
        patch::Set<BYTE>(addr + 1, default_color.y, false);
        patch::Set<BYTE>(addr + 2, default_color.z, false);
        patch::Set<BYTE>(addr + 3, default_color.w, false);
    }
    ImGui::SameLine();
    ImGui::Text(label);
}

bool Widget::Filter(const char* label, ImGuiTextFilter& filter, const char* hint) {
    bool state = filter.Draw(label);
    if (strlen(filter.InputBuf) == 0) {
        ImDrawList* drawlist = ImGui::GetWindowDrawList();

        ImVec2 min = ImGui::GetItemRectMin();
        min.x += ImGui::GetStyle().FramePadding.x;
        min.y += ImGui::GetStyle().FramePadding.y;

        drawlist->AddText(min, ImGui::GetColorU32(ImGuiCol_TextDisabled), hint);
    }
    return state;
}

void DrawClippedList(ResourceStore& data, fArg3_t clickFunc, bool favourites, bool isEditItem, fArgNone_t contextOptionsFunc) {
    // Category box
    ImGui::PushItemWidth(favourites ? ImGui::GetWindowContentRegionWidth() :
                         (ImGui::GetWindowContentRegionWidth() - ImGui::GetStyle().ItemSpacing.x)/2);

    if (!favourites) {
        if (Widget::ListBox("##Categories", data.m_Categories, data.m_Selected)) {
            data.UpdateSearchList(favourites);
        }
        ImGui::SameLine();
    }

    if (Widget::Filter("##Filter", data.m_Filter, TEXT( "Window.Search"))) {
        data.UpdateSearchList(favourites);
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();
    if (favourites && data.m_pData->GetTable("Favourites")->size() == 0) {
        Widget::TextCentered(TEXT("Menu.FavouritesNone"));
    }
    ImGui::BeginChild("CliipingLIst");
    ImGuiListClipper clipper;
    clipper.Begin(data.m_nSearchList.size(), ImGui::GetTextLineHeight());
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            std::string &label = std::get<ListLookup>(data.m_nSearchList[i]).key;
            std::string &cat = std::get<ListLookup>(data.m_nSearchList[i]).cat;
            std::string &val =  std::get<ListLookup>(data.m_nSearchList[i]).val;
            if (isEditItem) {
#ifdef GTASA
                Widget::EditStat(label.c_str(), std::stoi(val));
#endif
            } else {
                if (ImGui::MenuItem(label.c_str()) && clickFunc != nullptr) {
                    clickFunc(cat, label, val);
                }
            }

            if (ImGui::IsItemClicked(1)) {
                contextMenu = {cat, label, val, true};
            }
        }
    }

    if (contextMenu.show) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (ImGui::BeginPopupContextWindow()) {
            ImGui::Text(contextMenu.key.c_str());
            ImGui::Separator();

            if (!favourites && ImGui::MenuItem(TEXT("Menu.Favourites"))) {
                data.m_pData->Set(std::format("Favourites.{}", contextMenu.key).c_str(), contextMenu.val);
                data.m_pData->Save();
            }
            if (ImGui::MenuItem(TEXT( "Menu.Remove"))) {
                if (favourites) {
                    data.m_pData->RemoveKey("Favourites", contextMenu.key.c_str());
                    data.m_pData->Save();
                    data.UpdateSearchList(true);
                } else {
                    if (contextMenu.root == "Custom" || data.m_bAllowRemoveAll) {
                        data.m_pData->RemoveKey(contextMenu.root.c_str(), contextMenu.key.c_str());
                        data.m_pData->RemoveKey("Favourites", contextMenu.key.c_str());
                        data.m_pData->Save();
                        data.UpdateSearchList();
                        Util::SetMessage(TEXT("Window.RemoveEntry"));
                    } else {
                        Util::SetMessage(TEXT("Window.CustomRemoveOnly"));
                    }
                }
            }
            if (contextOptionsFunc) {
                contextOptionsFunc();
            }

            if (ImGui::MenuItem(TEXT( "Menu.Close"))) {
                contextMenu.show = false;
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}

void Widget::DataList(ResourceStore& data, fArg3_t clickFunc, fArgNone_t addFunc, bool isEditItem, fArgNone_t contextOptionsFunc, fArgNone_t tabsFunc) {
    if (ImGui::IsMouseClicked(1)) {
        contextMenu.show = false;
    }

    // Drawing the list here
    if (ImGui::BeginTabBar("MYTABS")) {
        if (ImGui::BeginTabItem(TEXT( "Window.Search"))) {
            ImGui::Spacing();
            DrawClippedList(data, clickFunc, false, isEditItem, contextOptionsFunc);
            ImGui::EndTabItem();
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            data.UpdateSearchList();
        }
        if (ImGui::BeginTabItem(TEXT( "Window.FavouritesTab"))) {
            ImGui::Spacing();
            DrawClippedList(data, clickFunc, true, isEditItem, contextOptionsFunc);
            ImGui::EndTabItem();
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            data.UpdateSearchList(true);
        }
        if (addFunc) {
            if (ImGui::BeginTabItem(TEXT( "Window.AddNew"))) {
                ImGui::Spacing();
                ImGui::BeginChild("AddNew2");
                ImGui::TextWrapped(TEXT("Window.AddNewTip"));
                ImGui::Dummy(ImVec2(0, 5));
                addFunc();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
        }
        if (tabsFunc) {
            tabsFunc();
        }
        ImGui::EndTabBar();
    }
}

static bool CustomImgageButton(ImTextureID textureID, ImVec2& size, const char* hoverText, bool alwaysHovered = false) {
    // Default to using texture ID as ID. User can still push string/integer prefixes.

    // Creating a invisible button as placeholder
    ImGui::InvisibleButton(hoverText, size);
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    float rounding = ImGui::GetStyle().FrameRounding;
    drawList->AddImageRounded(textureID, min, max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), rounding);

    // Add selection overlay and stuff on hover
    bool isHovered = ImGui::IsItemHovered();
    if (isHovered || alwaysHovered) {
        if (isHovered) {
            drawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg), rounding);
        }

        // Calculating and drawing text over the image
        ImVec2 textSize = ImGui::CalcTextSize(hoverText);
        if (textSize.x < size.x) {
            float offsetX = (ImGui::GetItemRectSize().x - textSize.x) / 2;
            drawList->AddText(ImVec2(min.x + offsetX, min.y + 10), ImGui::GetColorU32(ImGuiCol_Text), hoverText);
        } else {
            std::string buf = "";
            std::stringstream ss(hoverText);
            short count = 1;

            while (ss >> buf) {
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

void DrawClippedImages(ResourceStore& data, ImVec2 imgSz, size_t imagesInRow, bool showImages,
                       bool favourites, fArg1_t clickFunc, fRtnArg1_t getNameFunc, fRtnBoolArg1_t verifyFunc, fArgNone_t contextOptionsFunc) {
    static IDirect3DTexture9 **pDefaultTex = BY_GAME(gTextureList.FindTextureByName("placeholder"), nullptr, nullptr);
    ImGuiStyle &style = ImGui::GetStyle();

    // Category box
    ImGui::PushItemWidth(favourites ? ImGui::GetWindowContentRegionWidth() :
                         (ImGui::GetWindowContentRegionWidth() - ImGui::GetStyle().ItemSpacing.x)/2);
    if (!favourites) {
        if (Widget::ListBox("##Categories", data.m_Categories, data.m_Selected)) {
            data.UpdateSearchList(favourites, getNameFunc, verifyFunc);
        }
        ImGui::SameLine();
    }
    if (Widget::Filter("##Filter", data.m_Filter, TEXT( "Window.Search"))) {
        data.UpdateSearchList(favourites, getNameFunc, verifyFunc);
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::BeginChild("CliipingImag");
    if (favourites && data.m_pData->GetTable("Favourites")->size() == 0) {
        Widget::TextCentered(TEXT("Menu.FavouritesNone"));
    }

    int imageCount = 1;
    size_t totalSz = data.m_nSearchList.size();
    ImGuiListClipper clipper;
    clipper.Begin((totalSz > imagesInRow ? totalSz/imagesInRow : totalSz), imgSz.y);
    while (clipper.Step()) {
        // hack to get clipper working with rowed items
        size_t start = clipper.DisplayStart * imagesInRow;
        size_t end = clipper.DisplayEnd * imagesInRow + imagesInRow;
        end = (end > totalSz) ? totalSz : end;

        for (size_t i = start; i < end; ++i) {
            std::string &text = std::get<ImageLookup>(data.m_nSearchList[i]).m_FileName;
            std::string &modelName = std::get<ImageLookup>(data.m_nSearchList[i]).m_ModelName;
            bool custom = std::get<ImageLookup>(data.m_nSearchList[i]).m_bCustom;
            void *pTexture = custom ? pDefaultTex : std::get<ImageLookup>(data.m_nSearchList[i]).m_pTexture;
            if (showImages ? CustomImgageButton(pTexture, imgSz, modelName.c_str(), custom)
                    : ImGui::MenuItem(modelName.c_str())
               ) {
                clickFunc(text);
            }

            // Right click popup
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                contextMenu.show = true;
                contextMenu.added = custom;
                contextMenu.val = std::string(text);
                contextMenu.key = std::string(modelName);
            }

            if (showImages) {
                if (imageCount % imagesInRow != 0) {
                    ImGui::SameLine(0.0, style.ItemInnerSpacing.x);
                }
            }
            imageCount++;
        }
    }
    if (contextMenu.show) {
        if (ImGui::BeginPopupContextWindow()) {
            ImGui::Text(contextMenu.key.c_str());
            ImGui::Separator();
            if (!favourites && ImGui::MenuItem(TEXT("Menu.Favourites"))) {
                data.m_pData->Set(std::format("Favourites.{}", contextMenu.key).c_str(), contextMenu.val);
                data.m_pData->Save();
            }
            if (!favourites && contextMenu.added && ImGui::MenuItem(TEXT("Menu.Remove"))) {
                data.m_pData->RemoveKey("Custom", contextMenu.key.c_str());
                data.m_pData->RemoveKey("Favourites", contextMenu.key.c_str());
                data.m_pData->Save();
                data.UpdateSearchList(false, getNameFunc, verifyFunc);
            }
            if (favourites && ImGui::MenuItem(TEXT("Menu.FavouritesRemove"))) {
                data.m_pData->RemoveKey("Favourites", contextMenu.key.c_str());
                data.m_pData->Save();
                data.UpdateSearchList(true, getNameFunc, verifyFunc);
            }
            if (contextOptionsFunc) {
                contextOptionsFunc();
            }
            if (ImGui::MenuItem(TEXT( "Menu.Close"))) {
                contextMenu.show = false;
            }

            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
}
/*
    Here we go again...
    This direly needs a refactor oof
*/
void Widget::ImageList(ResourceStore &store, fArg1_t clickFunc, fRtnArg1_t getNameFunc,
                       fRtnBoolArg1_t verifyFunc, fArgNone_t addFunc, fArgNone_t contextOptionsFunc, fArgNone_t tabsFunc) {
    ImGuiStyle& style =  ImGui::GetStyle();
    /*
    	Trying to scale images based on resolutions
    	Native 1366x768
    */
    ImVec2 m_ImageSize = store.m_ImageSize;
    m_ImageSize.x *= screen::GetScreenWidth() / 1366.0f;
    m_ImageSize.y *= screen::GetScreenHeight() / 768.0f;

    int imagesInRow = static_cast<int>(ImGui::GetWindowContentRegionWidth() / m_ImageSize.x);
    m_ImageSize.x = ImGui::GetWindowContentRegionWidth() - style.ItemSpacing.x * (imagesInRow-1);
    m_ImageSize.x /= imagesInRow;

    bool showImages = !menuPage.m_bTextOnlyMode;
    if (gRenderer == eRenderer::DirectX11) {
        showImages = false;
    }

    ImGui::Spacing();

    // Hide the popup if right clicked again
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        contextMenu.show = false;
    }

    if (showImages) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(3, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
    }

    if (store.m_bSearchListUpdateRequired) {
        store.UpdateSearchList(false, getNameFunc, verifyFunc);
        store.m_bSearchListUpdateRequired = false;
    }

    // Draw images here
    if (ImGui::BeginTabBar("MYTABS")) {
        if (ImGui::BeginTabItem(TEXT( "Window.Search"))) {
            ImGui::Spacing();
            DrawClippedImages(store, m_ImageSize, imagesInRow, showImages, false, clickFunc, getNameFunc, verifyFunc, contextOptionsFunc);
            ImGui::EndTabItem();
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            store.UpdateSearchList(false, getNameFunc, verifyFunc);
        }
        if (ImGui::BeginTabItem(TEXT( "Window.FavouritesTab"))) {
            ImGui::Spacing();
            DrawClippedImages(store, m_ImageSize, imagesInRow, showImages, true, clickFunc, getNameFunc, verifyFunc, contextOptionsFunc);
            ImGui::EndTabItem();
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            store.UpdateSearchList(true, getNameFunc, verifyFunc);
        }
        if (addFunc) {
            if (ImGui::BeginTabItem(TEXT( "Window.AddNew"))) {
                ImGui::Spacing();
                ImGui::TextWrapped(TEXT("Window.AddNewTip"));
                ImGui::Dummy(ImVec2(0, 5));

                ImGui::BeginChild("AddNew2");
                addFunc();

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                if (ImGui::CollapsingHeader(TEXT("Window.AddNewCustomImg"))) {
                    ImGui::Spacing();
                    ImGui::TextWrapped(std::vformat(TEXT("Window.AddNewTip2"), std::make_format_args(store.m_FileName)).c_str());
                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Window.DownloadMagicTxd"), Widget::CalcSize())) {
                        OPEN_LINK("https://gtaforums.com/topic/851436-relopensrc-magictxd/");
                    }
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
        }
        if (tabsFunc) {
            tabsFunc();
        }
        ImGui::EndTabBar();
    }

    if (showImages) {
        ImGui::PopStyleVar(4);
    }
}

bool Widget::ColorBtn(int colorId, std::vector<float>& color, ImVec2 size) {
    bool rtn = false;
    std::string label = "Color " + std::to_string(colorId);

    if (ImGui::ColorButton(label.c_str(), ImVec4(color[0], color[1], color[2], 1), 0, size)) {
        rtn = true;
    }

    if (ImGui::IsItemHovered()) {
        ImDrawList* drawlist = ImGui::GetWindowDrawList();
        drawlist->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
                                ImGui::GetColorU32(ImGuiCol_ModalWindowDimBg));
    }

    return rtn;
}


bool Widget::Toggle(const char* label, bool* v, const char* hint, bool is_disabled) {
    ImGuiToggleConfig config =ImGuiTogglePresets::MaterialStyle();
    config.Flags = ImGuiToggleFlags_Animated;
    bool pressed = ImGui::Toggle(label, v, config);

    // draw hint
    if (hint != nullptr) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* drawlist = ImGui::GetWindowDrawList();

        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        ImGui::InvisibleButton("?", ImGui::CalcTextSize("?", nullptr, true));
        ImVec2 min = ImGui::GetItemRectMin();
        drawlist->AddText(ImVec2(min.x, min.y + style.ItemInnerSpacing.y / 2),
                          ImGui::GetColorU32(ImGuiCol_TextDisabled), "?");

        if (ImGui::IsItemHovered() && !is_disabled) {
            ImGui::BeginTooltip();
            ImGui::Text(hint);
            ImGui::Spacing();
            ImGui::EndTooltip();
        }
    }

    return pressed;
}

bool Widget::ToggleAddrRaw(const char* label, uint addr, size_t size, const char* enabled, const char* disabled, const char* hint) {
    bool rtn = false;
    char* buf = new char[size+1];
    patch::GetRaw(addr, buf, size);
    buf[size] = '\0';
    bool state = !strcmp(buf, enabled);

    if (Toggle(label, &state, hint)) {
        if (state) {
            patch::SetRaw(addr, const_cast<char*>(enabled), size);
            SaveMgr::SaveData(label, addr, SaveMgr::eCheatState::Enabled, enabled, disabled);
        } else {
            patch::SetRaw(addr, const_cast<char*>(disabled), size);
            SaveMgr::SaveData(label, addr, SaveMgr::eCheatState::Disabled, 0, 0);
        }
        rtn = true;
    }
    delete buf;

    return rtn;
}

bool Widget::ToggleBits(const char* label, uint flag, const char* hint) {
    bool rtn = false;
    bool state = (flag == 1) ? true : false;
    if (Toggle(label, &state, hint)) {
        flag = state ? 1 : 0;
        rtn = true;
    }

    return rtn;
}

bool Widget::InputFloat(const char* label, float *val, float change, float min, float max) {
    bool state = false;
    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() / MENU_WIDTH_FACTOR_X -2 * (ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x));
    if (ImGui::InputFloat(std::format("##Input{}", label).c_str(), val)) {
        if (min != max) {
            *val = (*val < min) ? min : *val;
            *val = (*val > max) ? max : *val;
        }
        state = true;
    }
    ImGui::SameLine(0.0, 4.0);
    int size = ImGui::GetFrameHeight();
    if (ImGui::Button(std::format("-##Input{}", label).c_str(), ImVec2(size, size))) {
        if (min != max && (*val - change) > min) {
            *val -= change;
        }
        state = true;
    }
    ImGui::SameLine(0.0, 4.0);
    if (ImGui::Button(std::format("+##Input{}", label).c_str(), ImVec2(size, size))) {
        if (min != max && (*val + change) < max) {
            *val += change;
        }
        state = true;
    }
    return state;
}

bool Widget::InputInt(const char* label, int *val, int min, int max) {
    bool state = false;
    if (ImGui::InputInt(label, val)) {
        if (min != max) {
            *val = (*val < min) ? min : *val;
            *val = (*val > max) ? max : *val;
        }
        state = true;
    }
    return state;
}

void Widget::InputAddr(const char* label, uint address, float min, float def, float max, float mul, float change) {
    float val = patch::Get<float>(address) * mul;

    if (InputFloat(std::format("##Set{}", label).c_str(), &val, change, min, max)) {
        patch::SetFloat(address, val / mul);
        SaveMgr::SaveData(label, address, SaveMgr::eCheatState::Enabled, min / mul, 0.0f);
    }
    ImGui::SameLine();
    if (ImGui::Button((TEXT_S("Menu.ResetToDefault") + "##" + label).c_str(), Widget::CalcSizeFrame(TEXT("Menu.ResetToDefault")))) {
        patch::Set<float>(address, def / mul);
        SaveMgr::SaveData(label, 0.0f, SaveMgr::eCheatState::Disabled, 0.0f, 0.0f);
    }
    ImGui::SameLine();
    ImGui::Text(label);
}


void Widget::EditBits(const char* label, const int address, VecStr& names) {
    auto mem_val = (int*)address;
    ImGui::Columns(2, nullptr, false);

    for (int i = 0; i < 32; ++i) {
        int mask = 1 << i;
        bool state = *mem_val & mask;

        if (Toggle(names[i].c_str(), &state)) {
            *mem_val ^= mask;
        }

        if (i + 1 == 32 / 2) {
            ImGui::NextColumn();
        }
    }
    ImGui::Columns(1);
}

#ifdef GTASA
void Widget::EditStat(const char* label, const int stat_id, const int min, const int def, const int max) {
    int val = static_cast<int>(CStats::GetStatValue(stat_id));
    bool val_updated = false;
    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / MENU_WIDTH_FACTOR_X);
    if (ImGui::InputInt(("##Set" + std::string(label)).c_str(), &val)) {
        val_updated =  true;
    }
    ImGui::SameLine();
    if (ImGui::Button((TEXT_S("Menu.ResetToDefault") + "##" + label).c_str(), Widget::CalcSizeFrame(TEXT("Menu.ResetToDefault")))) {
        val_updated =  true;
        val = def;
    }
    ImGui::SameLine();
    ImGui::Text(label);

    if (val_updated) {
        float fval = static_cast<float>(val);
        CStats::SetStatValue(stat_id, fval);
        gConfig.Set(std::format("Stats.{}", stat_id).c_str(), val);
    }
}
#endif

bool Widget::ListBox(const char* label, VecStr& allItems, std::string& selected) {
    bool rtn = false;
    if (ImGui::BeginCombo(label, selected.c_str())) {
        for (std::string curItem : allItems) {
            if (ImGui::MenuItem(curItem.c_str())) {
                selected = curItem;
                rtn = true;
            }
        }
        ImGui::EndCombo();
    }

    return rtn;
}

bool Widget::ListBox(const char* label, VecStr& allItems, int& selected) {
    bool rtn = false;
    if (ImGui::BeginCombo(label, allItems[selected].c_str())) {
        for (size_t index = 0; index < allItems.size(); ++index) {
            if (ImGui::MenuItem(allItems[index].c_str())) {
                selected = index;
                rtn = true;
            }
        }
        ImGui::EndCombo();
    }

    return rtn;
}

void Widget::ComboBoxAddr(const char* label, const char* keys, std::vector<uint32_t> &addrs) {
    int curItem = 0;

    size_t temp = 1; // 0 is None
    for (auto addr : addrs) {
        if (patch::Get<bool>(addr, false)) {
            curItem = temp;
        }
        ++temp;
    }

    if (ImGui::Combo(label, &curItem, keys)) {
        for (auto addr : addrs) {
            patch::Set<bool>(addr, false);
        }
        if (curItem == 0) {
            SaveMgr::SaveData(label, 0, SaveMgr::eCheatState::Disabled, true, false);
        } else {
            patch::Set<bool>(addrs[curItem-1], true);
            SaveMgr::SaveData(label, addrs[curItem-1], SaveMgr::eCheatState::Enabled, true, false);
        }
    }
}

void Widget::ComboBoxAddr(const char* label, uint addr, const char* keys, std::vector<uint32_t> &values) {
    int memVal = patch::Get<int8_t>(addr);
    int curItem = -1;

    for (auto val : values) {
        ++ curItem;
        if (val == memVal) {
            break;
        }
    }

    if (ImGui::Combo(label, &curItem, keys)) {
        patch::Set<int8_t>(addr, values[curItem]);
        SaveMgr::SaveData(label, static_cast<int>(addr), SaveMgr::eCheatState::Enabled,
            static_cast<int8_t>(values[curItem]), static_cast<int8_t>(0));
    }
}