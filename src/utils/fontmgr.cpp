#include "pch.h"
#include "fontmgr.h"

ImFont* FontMgr::Get(const char* fontID)
{
    for (auto &data : m_vecFonts)
    {
        if (!strcmp(data.m_ID.c_str(), fontID))
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
        0x0980, 0x09FF, // Bengali
        0x2000, 0x206F, // General Punctuation

        // Chinease
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0xFFFD, 0xFFFD, // Invalid
        0x4E00, 0x9FAF, // CJK Ideograms

        // Russian
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

const ImWchar* FontMgr::GetIconGlyphRanges()
{
    static const ImWchar ranges[] =
    {
        // FontAwesome icons
        ICON_MIN_FA, ICON_MAX_FA,
        0,
    };
    return &ranges[0];
}

ImFont* FontMgr::LoadFromFile(const char* fontID, const char* path, float fontMul)
{
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;
    ImFont *pFont = io.Fonts->AddFontFromFileTTF(path, fontSize, NULL, GetGlyphRanges());

    m_vecFonts.push_back({pFont, eFontType::File, fontSize, fontMul, std::string(fontID), 
                            std::string(path), NULL});
    io.Fonts->Build();
    return pFont; 
}

ImFont* FontMgr::LoadFromMemory(const char* fontID, const unsigned char *func, float fontMul)
{
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;
    
    ImFontConfig config;
    config.MergeMode = false;
    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    ImFont *pFont = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(static_cast<const void*>(func)),
                     fontSize, fontSize, &config, GetIconGlyphRanges());

    m_vecFonts.push_back({pFont, eFontType::Memory, fontSize, fontMul, std::string(fontID), "", 
                const_cast<void*>(static_cast<const void*>(func))});
    io.Fonts->Build();
    return pFont;
}

void FontMgr::UnloadAll()
{
    ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->Clear();
    io.Fonts->ClearFonts();
}

void FontMgr::ReloadAll()
{
    UnloadAll();

    ImGuiIO& io = ImGui::GetIO();
    for (auto &data : m_vecFonts)
    {
        size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * data.m_fMul;

        if (data.m_nType == eFontType::File)
        {
            data.m_pFont = io.Fonts->AddFontFromFileTTF(data.m_path.c_str(), data.m_nSize, NULL, 
                            GetGlyphRanges());
        }
        else
        {
            ImFontConfig config;
            config.MergeMode = false;
            config.GlyphMinAdvanceX = data.m_nSize; // Use if you want to make the icon monospaced
            data.m_pFont = io.Fonts->AddFontFromMemoryTTF(data.m_pFontData, data.m_nSize, data.m_nSize,
                             &config, GetIconGlyphRanges());
        }
    }
    io.FontDefault = Get("text");
    io.Fonts->Build();
    m_bFontReloadRequired = false;
}

void FontMgr::Process()
{
    if (curState == eStates::Idle)
    {
        return;
    }

    const char* link = "https://github.com/user-grinch/Cheat-Menu/raw/master/resource/addon/text.ttf";
    HRESULT res = URLDownloadToFile(NULL, link, MENU_DATA_PATH("fonts/text.ttf"), 0, NULL);

    if (res == E_OUTOFMEMORY || res == INET_E_DOWNLOAD_FAILURE)
    {
        Util::SetMessage(TEXT("Updater.Failed"));
        return;
    }

    m_bFontReloadRequired = true;
    curState = eStates::Idle;
}

bool FontMgr::IsSupportPackageInstalled()
{
    return std::filesystem::file_size(MENU_DATA_PATH("fonts/text.ttf")) > 1000000; // 1 MB
}

bool FontMgr::IsFontReloadRequired()
{
    return m_bFontReloadRequired;
}

void FontMgr::StartOptionalFontDownload()
{
    if (curState == eStates::Idle)
    {
        curState = eStates::Downloading;
    }
}

void FontMgr::SetFontReloadRequired(bool state)
{
    m_bFontReloadRequired = state;
}