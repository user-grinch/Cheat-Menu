#include "pch.h"
#include "widget.h"
#include "ui.h"
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

void Widget::FilterWithHint(const char* label, ImGuiTextFilter& filter, const char* hint)
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
            Ui::ListBoxStr("##Categories", data.m_Categories, data.m_Selected);
            ImGui::SameLine();
            FilterWithHint("##Filter", data.m_Filter, TEXT("Window.Search"));
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
            FilterWithHint("##Filter", data.m_Filter, TEXT("Window.Search"));
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
    Ui::ListBoxStr("##Categories", store.m_Categories, store.m_Selected);
    ImGui::SameLine();
    FilterWithHint("##Filter", store.m_Filter, "Search");

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