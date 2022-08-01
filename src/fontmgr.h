#pragma once

/*
    Font Manager Class
    Handles loading, fetching, freeing & reloading fonts
*/
class FontMgr
{
private:
    enum class eStates
    {
        Idle,
        Downloading,
    };

    struct FontInfo
    {
        ImFont *m_pFont;
        size_t m_nSize;
        float m_fMul;
        std::string m_ID;
        std::string m_path;
    };
    static inline std::vector<FontInfo> m_vecFonts;
    static inline eStates curState = eStates::Idle;

public:
    FontMgr() = delete;
    FontMgr(FontMgr&) = delete;

    // Returns font pointer from name
    static ImFont* Get(const char* fontID);

    // Loads a font into memory
    static ImFont* Load(const char* fontID, const char* path = 0, float fontMul = 1.0f);

    // Get the glyph ranges for our needed fonts
    static const ImWchar* GetGlyphRanges();
    
    // Unloads all the loaded fonts from fontmgr
    // ImGui::GetIO().Default font must be loaded after unloading all fonts
    static void UnloadAll();

    // Handles font downloading
    static void Process();

    // Reloads all the fonts 
    static void ReloadAll();

    // Downloads optional font package
    static void StartOptionalFontDownload();
};


