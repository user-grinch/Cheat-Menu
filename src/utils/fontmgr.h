#pragma once

/*
    Font Manager Class
    Handles loading, fetching, freeing & reloading fonts
*/
class FontMgr
{
public:
    enum class eFontMode 
    {
        Text,
        Icon,
        Merge,
    };

private:
    enum class eStates
    {
        Idle,
        Downloading,
    };

    struct FontInfo
    {
        ImFont *m_pFont;
        eFontMode mode;
        size_t m_nSize;
        float m_fMul;
        std::string m_ID;
        std::string m_path;
    };
    static inline std::vector<FontInfo> m_vecFonts;
    static inline eStates curState = eStates::Idle;
    static inline bool m_bFontReloadRequired = false;

public:
    FontMgr() = delete;
    FontMgr(FontMgr&) = delete;

    // Returns font pointer from name
    static ImFont* Get(const char* fontID);

    // Get the glyph ranges for our needed fonts
    static const ImWchar* GetGlyphRanges();
    static const ImWchar* GetIconGlyphRanges();

    // Returns true if font needs to be reloaded
    static bool IsFontReloadRequired();

    // Returns true if font support package is already installed
    static bool IsSupportPackageInstalled();

    // Loads a font from ttf
    static ImFont* LoadFont(const char* fontID, eFontMode mode, const char* path = 0, float fontMul = 1.0f);

    // Handles font downloading
    static void Process();

    // Reloads all the fonts
    static void ReloadAll();

    // Sets if the font reloading is requried
    static void SetFontReloadRequired(bool state);

    // Downloads optional font package from GitHub reository
    static void StartOptionalFontDownload();

    // Unloads all the loaded fonts from fontmgr
    // ImGui::GetIO().Default font must be loaded after unloading all fonts
    static void UnloadAll();
};


