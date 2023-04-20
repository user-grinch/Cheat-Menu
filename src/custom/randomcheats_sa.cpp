#include "pch.h"
#include "randomcheats_sa.h"

RandomCheatsMgr& RandomCheats = RandomCheatsMgr::Get();

void RandomCheatsMgr::DrawPages()
{
    uint timer = CTimer::m_snTimeInMilliseconds;
    static uint m_nTimer = 0;

    if ((timer - m_nTimer) > (static_cast<uint>(m_nInterval) * 1000))
    {
        int id = Random(0U, 91U);

        for (int i = 0; i < 92; i++)
        {
            if (i == id)
            {
                if (m_EnabledCheats[i][1] == "true")
                {
                    Call<0x438370>(id); // cheatEnableLegimate(int CheatID)
                    Call<0x69F0B0>((char*)m_EnabledCheats[i][0].c_str(), 2000, 0, false); // CMessages::AddMessage
                    m_nTimer = timer;
                }
                break;
            }
        }
    }

    if (m_bProgressBar)
    {
        // Next cheat timer bar
        uint screenWidth = screen::GetScreenWidth();
        uint screenHeight = screen::GetScreenHeight();
        uint timer = CTimer::m_snTimeInMilliseconds;
        uint totalTime = m_nInterval;
        float progress = (totalTime - (timer - m_nTimer) / 1000.0f) / totalTime;

        CRect sizeBox = CRect(0,0, screenWidth, screenHeight/50);
        CRect sizeProgress = CRect(0,0, screenWidth*progress, screenHeight/50);
        CRGBA colorBG = CRGBA(24, 99, 44, 255);
        CRGBA colorProgress = CRGBA(33, 145, 63, 255);

        CSprite2d::DrawRect(sizeBox, colorBG);
        CSprite2d::DrawRect(sizeProgress, colorProgress);
    }
}

void RandomCheatsMgr::DrawList()
{
    for (std::string* element : m_EnabledCheats)
    {
        bool selected = (element[1] == "true") ? true : false;
        if (ImGui::MenuItem(element[0].c_str(), nullptr, selected))
        {
            element[1] = selected ? "false" : "true";
        }
    }
}

RandomCheatsMgr::RandomCheatsMgr()
{
    // Generate enabled cheats vector
    for (auto [k, v] : m_pData.Items())
    {
        std::string key { k.str() };
        m_EnabledCheats[std::stoi(key)][0] = v.value_or<std::string>("Unknown");
        m_EnabledCheats[std::stoi(key)][1] = "true";
    }

    Events::drawingEvent += [this]()
    {
        if (m_bEnabled)
        {
            DrawPages();
        }
    };
}