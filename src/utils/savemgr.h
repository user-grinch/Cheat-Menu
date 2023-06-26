#pragma once
#include "string"
#include "datastore.h"
#include <Patch.h>
#include <type_traits>

extern DataStore gConfig;
/*
*	Save Data Manager Class
*	Handles saving & loading menu save data
*/
class SaveMgr
{
private:

public:
    // Defines the possible states of a cheat
    enum class eCheatState 
    {
        Disabled,
        Enabled,
        Unknown
    };

    enum class eAddrType
    {
        Bool,
        Int8,
        Int16,
        Int32,
        Float, 
        String,
        Unknown
    };

    SaveMgr() = delete;
    SaveMgr(SaveMgr&) = delete;

    // Initializes and loads save data from disk
    static void InitAndLoad()
    {
        for (auto [k, v] : gConfig.Items())
        {
            if (v.is_table())
            {
#ifdef GTASA
                if (k == "Stats")
                {
                    for (auto [i, j] : v.as_table()->ref<DataStore::Table>())
                    {
                        int stat_id = 0;
                        std::string_view str = i.str();
                        std::from_chars(str.data(), str.data() + str.size(), stat_id);
                        CStats::SetStatValue(stat_id, j.value_or<float>(0.0f));
                    }
                    continue;
                }
#endif    
                int addr = v.as_table()->at_path("Address").value_or<int>(0);
                eAddrType type = static_cast<eAddrType>(v.as_table()->at_path("Type").value_or<int>(0));
                eCheatState state = v.as_table()->at_path("State").value_or<int>(0) == 1 ? eCheatState::Enabled : eCheatState::Disabled;

                if (addr == 0 || state != eCheatState::Enabled)
                {
                    continue;
                }

                switch(type)
                {
                    case eAddrType::Bool:
                    {
                        injector::WriteMemory<bool>(addr, state == eCheatState::Enabled, true);
                        break;
                    }
                    case eAddrType::Int8:
                    {
                        int8_t val = v.as_table()->at_path("Enabled").value_or<int8_t>(0);
                        plugin::patch::Set<int8_t>(addr, val);  
                        break;
                    }
                    case eAddrType::Int32:
                    {
                        int val = v.as_table()->at_path("Enabled").value_or<int>(0);
                        plugin::patch::SetInt(addr, val);  
                        break;
                    }
                    case eAddrType::Float:
                    {
                        float val = v.as_table()->at_path("Enabled").value_or<float>(0.0f);
                        plugin::patch::SetFloat(addr, val);
                        break;
                    }
                    case eAddrType::String:
                    {
                        std::string val = v.as_table()->at_path("Enabled").value_or<std::string>("0");
                        patch::SetRaw(addr, const_cast<char*>(val.c_str()), val.size()-1);
                        break;
                    }
                    default:
                    {

                    }
                };
            }
        }
    }

    template<typename T>
    static void SaveData(std::string&& key, int addr, eCheatState state, T enabled, T disabled)
    {
        eAddrType type = eAddrType::Unknown;

        if (std::is_same<bool, T>::value) type = eAddrType::Bool;
        if (std::is_same<int8_t, T>::value) type = eAddrType::Int8;
        if (std::is_same<int16_t, T>::value) type = eAddrType::Int16;
        if (std::is_same<int32_t, T>::value) type = eAddrType::Int32;
        if (std::is_same<float, T>::value) type = eAddrType::Float;
        if (std::is_same<const char*, T>::value) type = eAddrType::String;

        gConfig.Set(std::string(key + ".Address").c_str(), addr);
        gConfig.Set(std::string(key + ".Type").c_str(), static_cast<int>(type));
        gConfig.Set(std::string(key + ".Enabled").c_str(), enabled);
        gConfig.Set(std::string(key + ".Disabled").c_str(), disabled);
        gConfig.Set(std::string(key + ".State").c_str(), state == eCheatState::Enabled);
    };
};