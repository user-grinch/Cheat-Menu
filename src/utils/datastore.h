#pragma once
#define TOML_EXCEPTIONS 0
#include "toml_addon.hpp"
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
    using Table = toml::table;
    using Array = toml::array;

    DataStore(const char* fileName, bool isPathPredefined = false) noexcept;

    // Returns data from store structure
    std::string Get(const char* key, const char* defaultVal, bool writeOnNone = false) noexcept
    {
        if (pTable)
        {
            std::optional<std::string> option = pTable->at_path(key).value<std::string>();

            if (option.has_value())
            {
                return option.value();
            }
        }

        if (writeOnNone)
        {
            Set(key, std::string(defaultVal));
        }
        return defaultVal;
    }

    template<typename T>
    T Get(const char* key, const T& defaultVal, bool writeOnNone = false) noexcept
    {
        if (pTable)
        {
            std::optional<T> option = pTable->at_path(key).value<T>();

            if (option.has_value())
            {
                return option.value();
            }
        }

        if (writeOnNone)
        {
            Set(key, defaultVal);
        }
        return defaultVal;
    }

    Array* GetArray(const char* key) noexcept
    {
        if (pTable)
        {
            Array *tbl = (*pTable).at_path(key).as_array();
            if (tbl)
            {
                return tbl;
            }
            else
            {
                pTable->insert(key, Table());
                return (*pTable).at_path(key).as_array();

            }
        }
        return nullptr;
    }

    Table* GetTable(const char* key) noexcept
    {
        if (pTable)
        {
            Table *tbl = (*pTable).at_path(key).as_table();
            if (tbl)
            {
                return tbl;
            }
            else
            {
                pTable->insert(key, Table());
                return (*pTable).at_path(key).as_table();

            }
        }
        return nullptr;
    }


    // Adds data to the structure
    template <typename T>
    void Set(const char* key, T&& value)
    {
        std::stringstream ss(key);
        std::vector<std::string> paths;

        while(ss.good())
        {
            std::string s1 = "";
            getline(ss, s1, '.');
            if (s1 != "")
            {
                paths.push_back(std::move(s1));
            }
        }

        // assign the value
        toml::table tbl;
        int startIndex = paths.size()-1;
        for (int i = startIndex; i >= 0; --i)
        {
            if (i == startIndex)
            {
                tbl.insert_or_assign(paths[i], std::move(value));
            }
            else
            {
                toml::table temp;
                temp.insert_or_assign(paths[i], std::move(tbl));
                tbl = std::move(temp);
            }
        }
        merge_left(*pTable, std::move(tbl));
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