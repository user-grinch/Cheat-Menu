#pragma once
#include <vector>

enum class eMenuPage
{
    Anniversary, 
    Game, 
    Menu, 
    None, 
    Ped, 
    Player, 
    Scene, 
    Teleport, 
    Update, 
    Vehicle, 
    Visual, 
    Weapon, 
    Welcome
};

/*
    Interface class for pages
    Every page must inherit this 
*/
template<typename T>
class IPage
{
private:
    eMenuPage m_ePage;      // Menu page ID
    bool m_bHasHeader;      // Does the page has a header button
    std::string m_NameKey;  // A key to the page name string

public:
    static inline std::vector<IPage*> m_nPageList;                  // Contains list of the created pages 
    static inline IPage* m_pCurrentPage = nullptr;                  // Currently visible menu page

    IPage(m_ePage page, std::string&& key, bool header = true)
    : m_ePage(page), m_NameKey(key), m_bHasHeader(header)
    {
        m_nPageList.push_back(this);
    }

    // Page drawing code goes here
    virtual void Draw() = 0;

    static T *Get() final
    {
        static T _instance;
        return &_instance;
    }

    // Returns the ID of the page
    eMenuPage GetPageID() final
    {
        return m_ePage;
    }

    // Returns true if the page has a visible header button
    bool HasHeaderButton() final
    {
        return m_bHasHeader;
    }

    // Returns the page name key
    std::string GetKey() final
    {
        return m_NameKey;
    }
    
    // Process the menu pages & draw them
    static void Process() final;
};