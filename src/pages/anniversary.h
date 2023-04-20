#pragma once
#include "interface/ipage.h"

// The anniversary celebration page
class AnniversaryPage : public IPage<AnniversaryPage>
{
private:
    friend class IFeature;
    AnniversaryPage() : IPage<AnniversaryPage>(ePageID::Anniversary, "Anniversary", false) {}
    AnniversaryPage(const AnniversaryPage&);

public:
    void Draw();
};

extern AnniversaryPage& anniversaryPage;