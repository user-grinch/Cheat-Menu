#pragma once
#include "interface/ipage.h"

// The default page for new install
class WelcomePage : public IPage<WelcomePage>
{
private:
    friend class IFeature;
    WelcomePage() : IPage<WelcomePage>(ePageID::Welcome, "Welcome", false)
    {
        Events::initGameEvent.after += [this]()
        {
            ePageID pageID = static_cast<ePageID>(gConfig.Get("Window.CurrentPage", static_cast<size_t>(ePageID::Welcome)));
            PagePtr ptr = PageHandler::FindPagePtr(pageID);
            if (ptr)
            {
                PageHandler::SetCurrentPage(ptr);
            }
        };
    }
    WelcomePage(const WelcomePage&);

public:
    void Draw();
};

extern WelcomePage& welcomePage;