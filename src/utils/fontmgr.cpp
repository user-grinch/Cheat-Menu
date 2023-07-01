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
        // 0x0980, 0x09FF, // Bengali
        0x2000, 0x206F, // General Punctuation

        // Chinease
        // 0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        // 0x31F0, 0x31FF, // Katakana Phonetic Extensions
        // 0xFF00, 0xFFEF, // Half-width characters
        // 0xFFFD, 0xFFFD, // Invalid
        // 0x4E00, 0x9FAF, // CJK Ideograms

        // // Russian
        // 0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        // 0x2DE0, 0x2DFF, // Cyrillic Extended-A
        // 0xA640, 0xA69F, // Cyrillic Extended-B

        // font awesome icons
        0x002b, // ICON_FA_PLUS,
        0xe068, // ICON_FA_PEOPLE_ARROWS, 
        0xe19b, // ICON_FA_GUN,
        0xe535, // ICON_FA_PEOPLE_PULLING,
        0xe54c, // ICON_FA_PERSON_MILITARY_TO_PERSON, 
        0xe54e, // ICON_FA_PERSON_RIFLE,
        0xf002, // ICON_FA_MAGNIFYING_GLASS,
        0xf005, // ICON_FA_STAR,
        0xf008, // ICON_FA_FILM, 
        0xf00d, // ICON_FA_XMARK
        0xf013, // ICON_FA_GEAR,
        0xf030, // ICON_FA_CAMERA 
        0xf03d, // ICON_FA_VIDEO,
        0xf07c, // ICON_FA_FOLDER_OPEN, 
        0xf0ae, // ICON_FA_LIST_CHECK, 
        0xf0c9, // ICON_FA_BARS,
        0xf0cb, // ICON_FA_LIST_OL 
        0xf0d7, // ICON_FA_CARET_DOWN,
        0xf11b, // ICON_FA_GAMEPAD
        0xf11c, // ICON_FA_KEYBOARD, 
        0xf120, // ICON_FA_TERMINAL, 
        0xf129, // ICON_FA_INFO,
        0xf205, // ICON_FA_TOGGLE_ON,
        0xf2d2, // ICON_FA_WINDOW_RESTORE, 
        0xf2ed, // ICON_FA_TRASH_CAN
        0xf2ed, // ICON_FA_TRASH_CAN,
        0xf390, // ICON_FA_DESKTOP,
        0xf533, // ICON_FA_PEOPLE_GROUP
        0xf53f, // ICON_FA_PALETTE ,
        0xf550, // ICON_FA_BARS_STAGGERED, 
        0xf553, // ICON_FA_SHIRT, 
        0xf554, // ICON_FA_PERSON_WALKING, 
        0xf5a0, // ICON_FA_MAP_LOCATION_DOT,
        0xf5bd, // ICON_FA_SPRAY_CAN, 
        0xf630, // ICON_FA_MASKS_THEATER, 
        0xf63c, // ICON_FA_TRUCK_PICKUP,
        0xf689, // ICON_FA_MAGNIFYING_GLASS_LOCATION,
        0xf70c, // ICON_FA_PERSON_RUNNING, 
        0xf783, // ICON_FA_CALENDAR_DAY,
        0,
    };
    return &ranges[0];
}

ImFont* FontMgr::LoadFont(const char* fontID, const unsigned int* func, unsigned int size, bool withIcons, float fontMul)
{
    ImFont* font;
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.OversampleH = 1;
    config.OversampleV = 1;
    config.PixelSnapH = 1;

    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;

    font = io.Fonts->AddFontFromMemoryCompressedTTF(func, fontSize, fontSize, &config, GetGlyphRanges());

    if (withIcons)
    {
        config.MergeMode = withIcons;
        io.Fonts->AddFontFromMemoryCompressedTTF(iconFont, iconFontSize, fontSize, &config, GetGlyphRanges());
    }

    m_vecFonts.push_back({font, fontSize, fontMul, std::string(fontID), func});
    io.Fonts->Build();

    return font; 
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