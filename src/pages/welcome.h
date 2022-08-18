#pragma once
#include "interface/ipage.h"

// The default page for new install
class WelcomePage : public IPage<WelcomePage>
{
private:
    friend IPage;
    WelcomePage() : IPage<WelcomePage>(ePageID::Welcome, "Welcome", false)
    {
        PageHandler::SetCurrentPage(this);
    }
    WelcomePage(const WelcomePage&);

public:
    void Draw();
};

extern WelcomePage& welcomePage;