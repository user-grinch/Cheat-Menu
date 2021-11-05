#include "fontmgr.h"
#include "pch.h"

ImFont* FontMgr::GetFont(const char* fontName)
{
    for (auto &data : m_vecFonts)
    {
       if (data.m_path == std::string(fontName))
       {
           return data.m_pFont;
       }
    }

    return nullptr;
}

ImFont* FontMgr::LoadFont(const char* fontName, float fontMul)
{
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;

    std::string fullPath = std::string(PLUGIN_PATH((char*)"CheatMenu/fonts/")) + fontName + ".ttf";
    m_vecFonts.push_back({io.Fonts->AddFontFromFileTTF(fullPath.c_str(), fontSize), fontSize,
                     std::string(fontName)});
    io.Fonts->Build();

    return m_vecFonts.back().m_pFont;
}

void FontMgr::ReinitFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    for (auto &data : m_vecFonts)
    {
        std::string fullPath = PLUGIN_PATH((char*)"CheatMenu/fonts/") + data.m_path + ".ttf";
        data.m_pFont = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), data.fontSize);
    }
    io.FontDefault = GetFont("text");
    io.Fonts->Build();
}