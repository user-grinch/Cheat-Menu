#pragma once
#include <vector>

enum class ePageID
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
    Handles all pages
    Draws the page code too
*/
using PagePtr = void*;
class PageHandler
{
private:
    static inline std::vector<PagePtr> m_PageList;  // Contains list of the created pages 
    static inline PagePtr m_pCurrentPage = nullptr;  // Currently visible menu page

public:

    PageHandler() = delete;
    PageHandler(const PageHandler&) = delete;

    // Process the menu pages & draw them
    static void DrawPages();

    // Add a new page
    static void AddPage(PagePtr page);
    static void SetCurrentPage(PagePtr page);
};

/*
    Interface class for pages
    Every page must inherit this 
*/
template<typename T>
class IPage
{
private:
    ePageID m_eID;          // Menu page ID
    bool m_bHasHeader;      // Does the page has a header button
    std::string m_NameKey;  // A key to the page name string

public:
    IPage(ePageID page, const std::string& key, bool header)
    : m_eID(page), m_NameKey(key), m_bHasHeader(header)
    {
        PageHandler::AddPage(reinterpret_cast<PagePtr>(this));
    }

    // Page drawing code goes here
    virtual void Draw() = 0;

    static T &Get()
    {
        static T _instance;
        return _instance;
    }

    // Returns the ID of the page
    virtual ePageID GetPageID() final
    {
        return m_eID;
    }

    // Returns true if the page has a visible header button
    virtual bool HasHeaderButton() final
    {
        return m_bHasHeader;
    }

    // Returns the page name key
    virtual std::string GetPageKey() final
    {
        return m_NameKey;
    }
};