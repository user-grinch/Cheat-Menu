#include "json.h"
#include "pch.h"

CJson::CJson(const char *name)
{
    if (name == "" || !std::filesystem::is_directory(PLUGIN_PATH((char *)"CheatMenu")))
    {
        return;
    }

    m_FilePath = PLUGIN_PATH((char *)"/CheatMenu/json/") + std::string(name) + ".json";

    if (std::filesystem::exists(m_FilePath))
    {
        try
        {
            std::ifstream file(m_FilePath);
            file >> m_Data;
            file.close();
        }
        catch (...)
        {
            gLog << "Error trying to read " << m_FilePath << std::endl;
            m_Data = "{}"_json;
        }
    }
    else
    {
        m_Data = "{}"_json;

        if (m_FilePath.find("config"))
        {
            gLog << "Creating config.json file" << std::endl;
        }
        else
        {
            gLog << "Failed to locate file " << m_FilePath << std::endl;
        }
    }
}

void CJson::WriteToDisk()
{
    std::ofstream file(m_FilePath);
    file << m_Data.dump(4, ' ', false, nlohmann::json::error_handler_t::replace) << std::endl;
    file.close();
}
