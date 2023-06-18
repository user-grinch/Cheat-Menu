#include "pch.h"
#include "locale.h"
#include <filesystem>

Locale::eReturnCodes Locale::Init(const char* path, const char* def, const char* fallback)
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
        Log::Print<eLogLevel::Error>("Locale directory doesn't exist");
#endif
        return eReturnCodes::DIR_NOT_FOUND;
    }


    /*
        Get the list of available languages
        We won't load them here, we'll load them when we need them
    */
#ifdef _GTA_
    Log::Print<eLogLevel::Info>("Loading languages...");
#endif
    for (auto& entry : std::filesystem::directory_iterator(m_path))
    {
        if (entry.path().extension() == ".toml")
        {
            std::string fileName = entry.path().stem().string();
#ifdef _GTA_
            Log::Print<eLogLevel::Info>("Found locale: {}", fileName);
#endif
            m_locales.push_back(fileName);

            if (!strcmp(fallback, fileName.c_str()))
            {
                std::string localePath = m_path + fileName;

                if(m_pCallbackData)
                {
                    delete m_pCallbackData;
                    m_pCallbackData = nullptr;
                }
                m_pCallbackData = new DataStore(localePath.c_str(), true);
            }
        }
    }

    if (sizeof(m_locales) == 0)
    {
#ifdef _GTA_
        Log::Print<eLogLevel::Error>("No language files found");
#endif
        return eReturnCodes::NO_LOCALE_FOUND;
    }


    // Look for default language and set it
    std::vector<std::string>& vec = Locale::GetLocaleList();

    size_t index = 0;
    for (std::string& locale : vec)
    {
        if (locale == def)
        {
            Locale::SetLocale(index);
            break;
        }

        index++;
    }

    if(!m_pData)
    {
#ifdef _GTA_
        Log::Print<eLogLevel::Error>("Failed to load default language.");
#endif
        return eReturnCodes::DEF_LOCALE_NOT_FOUND;
    }

    return eReturnCodes::SUCCESS;
}

std::vector<std::string>& Locale::GetLocaleList()
{
    return m_locales;
}

size_t Locale::GetCurrentLocaleIndex()
{
    return localeIndex;
}

void Locale::SetDefaultLocale()
{
    SetLocaleByName("English");
}

Locale::eReturnCodes Locale::SetLocale(size_t index)
{
    if(m_pData)
    {
        delete m_pData;
        m_pData = nullptr;
    }

    if (index < 0 || index >= m_locales.size())
    {
        return eReturnCodes::INVALID_INDEX;
    }
    std::string localePath = m_path + m_locales[index];
    m_pData = new DataStore(localePath.c_str(), true);
    localeIndex = index;
    return eReturnCodes::SUCCESS;
}

Locale::eReturnCodes Locale::SetLocaleByName(const std::string& name)
{
    for (size_t i = 0; i < m_locales.size(); ++i)
    {
        if (m_locales[i] == name)
        {
            return SetLocale(i);
        }
    }

    return eReturnCodes::NO_LOCALE_FOUND;
}

std::string Locale::GetText(std::string&& key, std::string&& defaultValue)
{
    if (m_pData == nullptr)
    {
        return defaultValue;
    }

    // Return keyname if no default value is provided
    if (defaultValue == "")
    {
        defaultValue = "#" + key;
    }
    
    std::string rtn = m_pData->Get(key.c_str(), defaultValue);

    if (rtn == defaultValue)
    {
        return m_pCallbackData->Get(key.c_str(), defaultValue);
    }

    return rtn;
}

