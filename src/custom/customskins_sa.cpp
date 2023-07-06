#include "pch.h"
#include "customskins_sa.h"
#include "utils/widget.h"

CustomSkinsMgr& CustomSkins = CustomSkinsMgr::Get();
CustomSkinsMgr::CustomSkinsMgr()
{
    Events::initRwEvent += [this]()
    {
        std::string path = GAME_PATH((char*)"modloader/");
        if (GetModuleHandle("modloader.asi") && std::filesystem::is_directory(path))
        {
            path += "CustomSkinsLoader/";
            if (std::filesystem::is_directory(path))
            {
                for (auto& p : std::filesystem::recursive_directory_iterator(path))
                {
                    if (p.path().extension() == ".dff")
                    {
                        std::string file_name = p.path().stem().string();

                        if (file_name.size() < 9)
                        {
                            m_List.push_back(file_name);
                        }
                        else
                        {
                            Log::Print<eLogLevel::Error>("Custom Skin longer than 8 characters {}", file_name);
                        }
                    }
                }
            }
            else
            {
                std::filesystem::create_directory(path);
            }
            m_bInit = true;
        }
    };
}

void CustomSkinsMgr::SetSkin(const std::string& name)
{
    CStreaming::RequestSpecialChar(1, name.c_str(), PRIORITY_REQUEST);
    CStreaming::LoadAllRequestedModels(true);
    FindPlayerPed()->SetModelIndex(291);
    CStreaming::SetSpecialCharIsDeletable(291);
}

void CustomSkinsMgr::Draw()
{
    if (m_bInit)
    {
        Widget::Filter(TEXT( "Window.Search"), m_Filter, std::format("{}{}", TEXT("Player.TotalSkins"), m_List.size()).c_str());
        Widget::Tooltip(TEXT("Player.CustomSkinsDirTip"));
        ImGui::Spacing();
        ImGui::TextWrapped(TEXT("Player.CustomSkinsTip"));
        ImGui::Spacing();
        for (std::string name : m_List)
        {
            if (m_Filter.PassFilter(name.c_str()))
            {
                if (ImGui::MenuItem(name.c_str()))
                {
                    SetSkin(name);
                }
            }
        }
    }
    else
    {
        ImGui::TextWrapped(TEXT("Player.CustomSkinTutorial"));
        ImGui::Spacing();
        if (ImGui::Button(TEXT("Player.DownloadModloader"), ImVec2(Widget::CalcSize(1))))
        {
            OPEN_LINK("https://gtaforums.com/topic/669520-mod-loader/");
        }
    }
}