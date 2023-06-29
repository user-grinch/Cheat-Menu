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

ImFont* FontMgr::LoadFont(const char* fontID, eFontMode mode, const char* path, float fontMul)
{
    FontInfo data;
    bool new_font = true;
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;

    ImFontConfig config;
    config.MergeMode = false;
    config.GlyphMinAdvanceX = fontSize; // Use if you want to make the icon monospaced

    for (FontInfo info : m_vecFonts)
    {
        if (!strcmp(info.m_ID.c_str(), fontID))
        {
            data = info;
            new_font = false;
            break;
        }
    }

    if (mode == eFontMode::Text || mode == eFontMode::Merge)
    {
        data.m_pFont = io.Fonts->AddFontFromFileTTF(path, fontSize, NULL, GetGlyphRanges());
    }
    if (mode == eFontMode::Icon)
    {
        data.m_pFont = io.Fonts->AddFontFromFileTTF(path, fontSize, &config, GetIconGlyphRanges());
    }

    if (mode == eFontMode::Merge)
    {
        config.MergeMode = true;
        io.Fonts->AddFontFromFileTTF(path, fontSize, &config, GetIconGlyphRanges());
    }

    if (new_font)
    {
        data.mode = mode;
        data.m_nSize = fontSize;
        data. m_fMul = fontMul;
        data.m_ID = std::string(fontID);
        data.m_path = std::string(path);
        m_vecFonts.push_back(data);
    }
    return data.m_pFont; 
}

void FontMgr::UnloadAll()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->ClearFonts();
}

void FontMgr::ReloadAll()
{
    // UnloadAll();

    // ImGuiIO& io = ImGui::GetIO();
    // for (auto &data : m_vecFonts)
    // {
    //     LoadFont(data.m_ID.c_str(), data.mode, data.m_path.c_str(), data.m_fMul);
    // }
    // io.FontDefault = Get("text");
    // m_bFontReloadRequired = false;
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