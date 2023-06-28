#include "pch.h"
#include "ipage.h"
#include "utils/updater.h"
#include "utils/widget.h"
#include "imgui/imgui_internal.h"
#include "pages/welcome.h"

void PageHandler::AddPage(PagePtr page, size_t index, bool headers)
{
    static size_t size = static_cast<size_t>(ePageID::COUNT);
    if (m_PageList.size() < size)
    {
        m_PageList.resize(size);
    }
    if (headers)
    {
        ++m_nPagesWithHeaders;
    }
    m_PageList[index] = page;
}

void PageHandler::SetCurrentPage(PagePtr page)
{
    m_pCurrentPage = page;
}


uint PageHandler::GetPageCount()
{
    return m_nPagesWithHeaders;
}

using IPageStatic = IPage<WelcomePage>; // dummy class
bool PageHandler::DrawPages()
{
    bool rtn = false;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    // ------------------------------ Left Side ------------------------------ 
    // Pages

    ImVec2 size = Widget::CalcSize(3, false);
    ImGuiStyle &style = ImGui::GetStyle();

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

    // Button styling
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1294f, 0.1333f, 0.1765f, 0.8431f));
    float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    float height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
    uint p = GetPageCount();
    float btn_sz = (height - ImGui::GetStyle().ItemSpacing.y * (p-1)) / p;
    ImGui::BeginChild("PagesChild", ImVec2(btn_sz, height));

    // Draw header buttons
    ImGui::PushFont(FontMgr::Get("icon"));
    ImDrawList *pDrawList = ImGui::GetWindowDrawList();
    for (PagePtr ptr : m_PageList)
    {
        IPageStatic* pg = reinterpret_cast<IPageStatic*>(ptr);
        if (!pg->HasHeaderButton())
        {
            continue;
        }

        bool flag = false;
        if (pg == m_pCurrentPage)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            flag = true;
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        }
        
        if (ImGui::Button(pg->GetPageKey().c_str(), ImVec2(btn_sz, btn_sz)))
        {
            m_pCurrentPage = pg;
            size_t id = static_cast<size_t>(pg->GetPageID());
            gConfig.Set("Window.CurrentPage", id);
            Updater::ResetUpdaterState();
        }

        if (flag)
        {
            ImGui::PopStyleColor(2);
        }

    }
    ImGui::PopFont();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::SameLine();

    // ------------------------------ Right Side ------------------------------ 
    ImGui::BeginGroup();

    // Title
    float rs_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x 
                        - btn_sz - ImGui::GetStyle().ItemSpacing.x;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::BeginChild("TitleChild", ImVec2(rs_width - ImGui::GetStyle().ItemSpacing.x - btn_sz, btn_sz));
    ImGui::PushFont(FontMgr::Get("title"));
    Widget::TextCentered(MENU_TITLE);
    ImGui::PopFont();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    
    ImGui::SameLine();

    // draw close btn
    ImGui::PushFont(FontMgr::Get("icon"));
    if (ImGui::Button(ICON_FA_TIMES, ImVec2(btn_sz, btn_sz)))
    {
        rtn = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleVar(2);
    float content_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y 
                        - btn_sz - ImGui::GetStyle().ItemSpacing.y;
    
    ImGui::BeginChild("ContentChild", ImVec2(rs_width, content_height));
    if (m_pCurrentPage != nullptr && ImGui::BeginChild("HEADERCONTENT"))
    {
        reinterpret_cast<IPageStatic*>(m_pCurrentPage)->Draw();
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::EndGroup();

    return rtn;
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