/*
    VS Code extension doesn't work well with template classes
*/

#pragma once
#include <vector>
#include "CVehicle.h"

template <class T>
class VehExtender
{
public:
    VehExtender(){};
    VehExtender(const VehExtender&) = delete;
    
    T& Get(CVehicle *veh)
    {
        static std::vector<std::pair<CVehicle*,T>> data;

        for (auto it = data.begin(); it < data.end(); ++it)
        {
            if (it->first == veh)
                return it->second;
        }

        data.push_back({veh, T(veh)});
        return data.back().second;
    }
};