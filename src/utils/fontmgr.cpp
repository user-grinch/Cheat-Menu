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
        0x2000, 0x206F, // General Punctuation
        0,
    };
    return &ranges[0];
}

ImFont* FontMgr::LoadFont(const char* fontID, const unsigned int* func, unsigned int size, float fontMul)
{
    ImFont* font;
    ImGuiIO& io = ImGui::GetIO();
    size_t fontSize = static_cast<int>(screen::GetScreenHeight() / 54.85f) * fontMul;

    font = io.Fonts->AddFontFromMemoryCompressedTTF(func, fontSize, fontSize, NULL, GetGlyphRanges());
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