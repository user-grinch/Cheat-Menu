#pragma once

/*
    Font Manager class
    Handles loading, fetching, freeing & reloading fonts
*/
class FontMgr
{
private:
    struct FontInfo
    {
        ImFont *m_pFont;
        size_t m_nSize;
        float m_fMul;
        std::string m_path;
    };
    static inline std::vector<FontInfo> m_vecFonts;

public:
    FontMgr() = delete;
    FontMgr(FontMgr&) = delete;

    static ImFont* GetFont(const char* fontName);
    static ImFont* LoadFont(const char* fontName, float fontMul = 1.0f);
    
    // ImGui::GetIO().Default font must be loaded after unloading all fonts
    static void UnloadFonts();
    static void ReloadFonts();
};


