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
        const unsigned int* m_pfunc;
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

    // Returns true if font needs to be reloaded
    static bool IsFontReloadRequired();

    // Loads a font from ttf
    static ImFont* LoadFont(const char* fontID, const unsigned int* func, unsigned int size, float fontMul = 1.0f);

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


