#pragma once
#include "interface/ipage.h"

// The anniversary celebration page
class AnniversaryPage : public IPage<AnniversaryPage>
{
private:
    friend IPage;
    AnniversaryPage() : IPage<AnniversaryPage>(ePageID::Anniversary, "Anniversary", false){}
    AnniversaryPage(const AnniversaryPage&);

public:
    void Draw();
};

extern AnniversaryPage& anniversaryPage;