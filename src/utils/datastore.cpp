#include "pch.h"
#include "datastore.h"

DataStore::DataStore(const char* fileName, bool isPathPredefined) noexcept
{
    if (isPathPredefined)
    {
        path = std::string(fileName) + fileExt;
    }
    else
    {
        path = PLUGIN_PATH((char*)FILE_NAME "/data/") + std::string(fileName) + fileExt;
    }

    if (std::filesystem::exists(path))
    {
        toml::parse_result result = toml::parse_file(path);

        if (result)
        {
            pTable = std::make_unique<toml::table>(std::move(result));
            return;
        }
    }

    if (pTable == nullptr)
    {
        pTable = std::make_unique<toml::table>();

        if (fileName == FILE_NAME)
        {
            Log::Print<eLogLevel::Info>("Creating {}{}", fileName, fileExt);
        }
        else
        {
            Log::Print<eLogLevel::Warn>("Error parsing {}{}", fileName, fileExt);
        }
    }
}

bool DataStore::Contains(const char* ele) noexcept
{
    if (pTable)
    {
        return pTable->contains(ele);
    }

    return false;
}

DataStore::Table& DataStore::Items() noexcept
{
    return pTable->as_table()->ref<DataStore::Table>();
}

void DataStore::RemoveTable(const char* key) noexcept
{
    if (pTable)
    {
        pTable->erase(key);
    }
}

void DataStore::RemoveKey(const char* key, const char* entry) noexcept
{
    if (pTable)
    {
        Table *tbl = pTable->at_path(key).as_table();
        if (tbl)
        {
            tbl->erase(entry);
        }
    }
}

void DataStore::Save() noexcept
{
    if (pTable)
    {
        std::ofstream file(path);
        if (file.good())
        {
            file << *pTable << std::endl;
            file.close();
        }
    }
}