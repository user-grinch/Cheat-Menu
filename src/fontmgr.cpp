#include "fontmgr.h"
#include "pch.h"

ImFont* FontMgr::Get(const char* fontName)
{
    for (auto &data : m_vecFonts)
    {
        if (!strcmp(data.m_path.c_str(), fontName))
        {
            return data.m_pFont;
        }
    }

    return nullptr;
}

ImFont* FontMgr::Load(const char* fontName, float fontMul)
{
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;
    std::string fullPath = std::format("{}{}.ttf", PLUGIN_PATH((char*)"CheatMenu/fonts/"), fontName);
    ImFont *pFont = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), fontSize);
    
    m_vecFonts.push_back({pFont, fontSize, fontMul, std::string(fontName)});
    io.Fonts->Build();

    return pFont;
}

void FontMgr::UnloadAll()
{
    ImGui::GetIO().Fonts->Clear();
}

void FontMgr::ReloadAll()
{
    UnloadAll();

    ImGuiIO& io = ImGui::GetIO();
    for (auto &data : m_vecFonts)
    {
        size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * data.m_fMul;
        std::string fullPath = PLUGIN_PATH((char*)"CheatMenu/fonts/") + data.m_path + ".ttf";
        data.m_pFont = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), data.m_nSize);
    }
    io.FontDefault = Get("text");
    io.Fonts->Build();
}