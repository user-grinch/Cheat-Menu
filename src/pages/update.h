#pragma once
#include "interface/ipage.h"

// This page shows up when an update is found
class UpdatePage : public IPage<UpdatePage>
{
private:
    friend IPage;
    UpdatePage() : IPage<UpdatePage>(ePageID::Update, "Update", false){}
    UpdatePage(const UpdatePage&);

public:
    void Draw();
};

extern UpdatePage& updatePage;