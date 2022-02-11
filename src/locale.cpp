#include "pch.h"
#include "locale.h"
#include <filesystem>

Locale::eReturnCodes Locale::Init(const char* path)
{
    std::string localePath = path;
    if (localePath.back() != '/')
    {
        localePath += '/';
    }

#ifdef _GTA_
    m_path = PLUGIN_PATH((char*)localePath.c_str());
#else
    m_path = localePath;
#endif

    if (!std::filesystem::exists(m_path))
    {
#ifdef _GTA_
        gLog << "Locale directory doesn't exist" << std::endl;
#endif
        return eReturnCodes::DIR_NOT_FOUND;
    }


    /*
        Get the list of available languages
        We won't load them here, we'll load them when we need them
    */
#ifdef _GTA_
    gLog << "Loading languages..." << std::endl;
#endif
    for (auto& entry : std::filesystem::directory_iterator(m_path))
    {
        if (entry.path().extension() == ".json")
        {
#ifdef _GTA_
            gLog << "Found locale: " << entry.path().stem().string() << std::endl;
#endif
            m_locales.push_back(entry.path().stem().string());
        }
    }

    if (sizeof(m_locales) == 0)
    {
#ifdef _GTA_
        gLog << "No language files found" << std::endl;
#endif
        return eReturnCodes::NO_LOCALE_FOUND;
    }

    return eReturnCodes::SUCCESS;
}

std::vector<std::string>& Locale::GetLocaleList()
{
    return m_locales;
}

std::string Locale::GetText(std::string&& key, std::string&& defaultValue)
{
    if (m_pJson == nullptr)
    {
        return defaultValue;
    }

    return m_pJson->GetValueStr(key, defaultValue);
}

Locale::eReturnCodes Locale::SetLocale(int index)
{
    if(m_pJson)
    {
        delete m_pJson;
        m_pJson = nullptr;
    }

    if (index < 0 || index >= m_locales.size())
    {
        return eReturnCodes::INVALID_INDEX;
    }

    std::string localeFile = m_locales[index];
    localeFile += ".json";
    std::string localePath = m_path + localeFile;
    m_pJson = new CJson(localePath.c_str());
    return eReturnCodes::SUCCESS;
}

