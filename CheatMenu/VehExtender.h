/*
    VS Code extension doesn't work well with template classes
*/

#pragma once
#include <vector>
#include "CVehicle.h"
#include "Events.h"

template <class T>
class VehExtender
{
private:
    inline static std::vector<std::pair<CVehicle*,T>> data{};

public:
    static void RemoveVehEntry(CVehicle *pVeh)
    {
        for (auto it = data.begin(); it != data.end(); it++)
        {
            if (it->first == pVeh)
            {
                data.erase(it);
                break;
            }
        }
    }

    VehExtender()
    {
        plugin::Events::vehicleDtorEvent.before += RemoveVehEntry;
    }
    ~VehExtender()
    {
        plugin::Events::vehicleDtorEvent.before -= RemoveVehEntry;
    }
    VehExtender(const VehExtender&) = delete;
    
    T& Get(CVehicle *pVeh)
    {
        for (auto it = data.begin(); it < data.end(); ++it)
        {
            if (it->first == pVeh)
                return it->second;
        }

        data.push_back({pVeh, T(pVeh)});
        return data.back().second;
    }
};