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
    // ------------------------------ Left Side ------------------------------ 
    // Pages

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
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1294f, 0.1333f, 0.1765f, 0.8431f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

    float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    float height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
    float child_width = width/9;
    ImGui::BeginChild("PagesChild", ImVec2(child_width, height));
    height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
    float btn_height = height/GetPageCount();
    float btn_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

    // Draw header buttons
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
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            flag = true;
        }
        
        if (ImGui::Button(pg->GetPageKey().c_str(), ImVec2(btn_width, btn_height)))
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
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
    ImGui::SameLine();
    // ------------------------------ Right Side ------------------------------ 
    ImGui::BeginGroup();

    // Title
    ImGui::PushFont(FontMgr::Get("title"));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    float rs_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x 
                        - btn_width - ImGui::GetStyle().ItemSpacing.x;

    ImVec2 child_sz = {rs_width + ImGui::GetStyle().ItemSpacing.x - btn_height, btn_height};
    ImGui::BeginChild("TitleChild", child_sz);
    Widget::TextCentered(MENU_TITLE);
    ImGui::EndChild();
    
    ImGui::SameLine();
    float close_sz = ImGui::GetTextLineHeightWithSpacing();
    if (ImGui::Button("X", ImVec2(close_sz, close_sz)))
    {
        rtn = true;
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopFont();
    
    float content_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y 
                        - btn_height - ImGui::GetStyle().ItemSpacing.y;
    
    ImGui::BeginChild("ContentChild", ImVec2(rs_width, content_height));
    if (m_pCurrentPage != nullptr)
    {
        reinterpret_cast<IPageStatic*>(m_pCurrentPage)->Draw();
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