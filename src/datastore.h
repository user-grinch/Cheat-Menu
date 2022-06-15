#pragma once
#define TOML_EXCEPTIONS 0
#include "../depend/toml.hpp"
#include <memory>

/*
    DataStore Class
    Stores & loads data from disk
    TOML format
*/
class DataStore 
{
private:
    static inline const char* fileExt = ".toml";
    std::unique_ptr<toml::table> pTable;
    std::string path;

public:
    typedef toml::table Table;

    DataStore(const char* fileName, bool isConfig = false) noexcept;

    // Returns data from store structure
    template<typename T>
    T Get(const char* key, const T& defaultVal) noexcept
    {
        if (pTable)
        {
            return (*pTable)[key].value_or(defaultVal);
        }
        return defaultVal;
    }

    std::string Get(const char* key, std::string&& defaultVal) noexcept
    {
        if (pTable)
        {
            return (*pTable)[key].value_or(defaultVal);
        }
        return defaultVal;
    }

    // Sets data in store structure
    template<typename T>
    void Set(const char* key, const T& value) noexcept
    {
        if (pTable)
        {
            (*pTable)[key] = value;
        }
    }

    void Set(const char* key, std::string&& value) noexcept
    {
        if (pTable)
        {
            (*pTable)[key].ref<std::string>() = value;
        }
    }

    // If store contains element
    bool Contains(const char*) noexcept;

    // Provides access to internal structure elements
    Table& Items() noexcept;

    // Removes a table, it's keys & data
    void RemoveTable(const char* key) noexcept;

    // Removes a key and it's data
    void RemoveKey(const char* key, const char* entry) noexcept;

    // Saves data to disk
    void Save() noexcept;
};