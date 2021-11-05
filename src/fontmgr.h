#pragma once
#include "pch.h"

class FontMgr
{
private:
    struct FontInfo
    {
        ImFont *m_pFont;
        size_t fontSize;
        std::string m_path;
    };

public:
    static inline std::vector<FontInfo> m_vecFonts;
    
    FontMgr() = delete;
    FontMgr(FontMgr&) = delete;

    static ImFont* GetFont(const char* fontName);
    static ImFont* LoadFont(const char* fontName, float fontMul = 1.0f);
    static void ReinitFonts();
};


