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

const ImWchar* FontMgr::GetGlyphRanges()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation

        // Chinease
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0xFFFD, 0xFFFD, // Invalid
        0x4e00, 0x9FAF, // CJK Ideograms

        // Russian
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0x2212, 0x2212, // Minus Sign
        0,
    };
    return &ranges[0];
}

ImFont* FontMgr::Load(const char* fontName, float fontMul)
{
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;
    std::string fullPath = std::format("{}{}.ttf", PLUGIN_PATH((char*)FILE_NAME "/fonts/"), fontName);
    ImFont *pFont = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), fontSize, NULL, GetGlyphRanges());
    
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
        std::string fullPath = PLUGIN_PATH((char*)FILE_NAME "/fonts/") + data.m_path + ".ttf";
        data.m_pFont = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), data.m_nSize, NULL, GetGlyphRanges());
    }
    io.FontDefault = Get("text");
    io.Fonts->Build();
}