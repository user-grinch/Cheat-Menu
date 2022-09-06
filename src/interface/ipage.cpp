#include "pch.h"
#include "ipage.h"
#include "utils/updater.h"
#include "utils/widget.h"
#include "imgui/imgui_internal.h"
#include "pages/welcome.h"

void PageHandler::AddPage(PagePtr page, size_t index)
{
    static size_t size = static_cast<size_t>(ePageID::COUNT);
    if (m_PageList.size() < size)
    {
        m_PageList.resize(size);
    }
    m_PageList[index] = page;
}

void PageHandler::SetCurrentPage(PagePtr page)
{
    m_pCurrentPage = page;
}

using IPageStatic = IPage<WelcomePage>; // dummy class
void PageHandler::DrawPages()
{
    ImVec2 size = Widget::CalcSize(3, false);
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    if (Updater::IsUpdateAvailable())
    {
        for (PagePtr ptr : m_PageList)
        {   
            IPageStatic* page = reinterpret_cast<IPageStatic*>(ptr);
            if (page->GetPageID() == ePageID::Update)
            {
                m_pCurrentPage = page;
                break;
            }
        }
    }
    
    // Check once if it's anniversary day
    static bool checked;
    if (!checked)
    {
        SYSTEMTIME st;
        GetSystemTime(&st);

        if (st.wMonth == 3 && st.wDay == 28)
        {
            /*
            *   We don't want to be annoying and
            *   show anniversary screen on every game start
            */
            bool flag = gConfig.Get("Window.AnniversaryShown", false);

            if (!flag)
            {
                gConfig.Set("Window.AnniversaryShown", true);

                for (void* ptr : m_PageList)
                {
                    IPageStatic* page = reinterpret_cast<IPageStatic*>(ptr);
                    if (page->GetPageID() == ePageID::Anniversary)
                    {
                        m_pCurrentPage = page;
                        break;
                    }
                }
            }
        }
        checked = true;
    }

    // Draw header buttons
    ImDrawList *pDrawList = ImGui::GetWindowDrawList();
    size_t count = 0;
    for (PagePtr ptr : m_PageList)
    {   
        IPageStatic* pg = reinterpret_cast<IPageStatic*>(ptr);
        if (!pg->HasHeaderButton())
        {
            continue;
        }

        std::string text = TEXT_S(pg->GetPageKey());
        ImVec4 color = (pg == m_pCurrentPage) ? style.Colors[ImGuiCol_ButtonActive] : style.Colors[ImGuiCol_Button];

        if (ImGui::InvisibleButton(text.c_str(), size))
        {
            m_pCurrentPage = pg;
            size_t id = static_cast<size_t>(pg->GetPageID());
            gConfig.Set("Window.CurrentPage", id);
            Updater::ResetUpdaterState();
        }

        if (ImGui::IsItemHovered())
        {
            color = style.Colors[ImGuiCol_ButtonHovered];
        }

        /*
        * Window rounding flags
        * TODO: hardcoded atm
        */
        ImDrawFlags flags = ImDrawFlags_RoundCornersNone;
        if (count == 0) flags = ImDrawFlags_RoundCornersTopLeft;
        if (count == 2) flags = ImDrawFlags_RoundCornersTopRight;
        if (count == 6) flags = ImDrawFlags_RoundCornersBottomLeft;
        if (count == 8) flags = ImDrawFlags_RoundCornersBottomRight;

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImVec2 size = ImGui::CalcTextSize(text.c_str());
        pDrawList->AddRectFilled(min, max, ImGui::GetColorU32(color), style.FrameRounding, flags);
        ImGui::RenderTextClipped(min + style.FramePadding, max - style.FramePadding, text.c_str(), NULL, &size, style.ButtonTextAlign);

        if (count % 3 != 2)
        {
            ImGui::SameLine();
        }
        ++count;
    }
    ImGui::PopStyleVar();
    ImGui::Spacing();
    ImGui::Dummy(ImVec2(0, 10));

    if (m_pCurrentPage != nullptr && ImGui::BeginChild("HEADERCONTENT"))
    {
        reinterpret_cast<IPageStatic*>(m_pCurrentPage)->Draw();
        ImGui::EndChild();
    }
}

PagePtr PageHandler::FindPagePtr(ePageID id)
{
    for (void* ptr : m_PageList)
    {
        IPageStatic* page = reinterpret_cast<IPageStatic*>(ptr);
        if (page->GetPageID() == id)
        {
            return reinterpret_cast<PagePtr>(page);
        }
    }
    return nullptr;
}