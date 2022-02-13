#pragma once
#include <vector>
#include <string>
#include "json.h"

/*
    A custom i18n library
    Loads strings from a json file
    Requires the CJson class
*/
class Locale
{
private:
    static inline std::vector<std::string> m_locales;
    static inline std::string m_path;
    static inline CJson *m_pJson = nullptr;
    static inline size_t localeIndex;

public:

    enum class eReturnCodes
    {
        DIR_NOT_FOUND = 0,          // Failed to find the language directory
        NO_LOCALE_FOUND = 1,        // Failed to find language files
        INVALID_INDEX = 2,          // Invalid langauge index for GetLocaleList()
        SUCCESS = 3,          
        DEF_LOCALE_NOT_FOUND = 3,          
    };

    Locale() = delete;
    Locale(Locale const&) = delete;
    void operator=(Locale const&) = delete;

    /* 
        Loads json files from the locale directory
        Calling it multiple times will unload previous data
    */
    static eReturnCodes Init(const char* path, const char* def = "English");

    // Returns a vector of available languages
    static std::vector<std::string>& GetLocaleList();
    static size_t GetCurrentLocaleIndex();

    /*
        Returns the string for the given key for the language specified
        If the key is not found, returns the defaultValue

        You need to call SetLanguage once before calling this function
        By default, the language is set to "en"
    */
    static inline std::string GetText(std::string&& key, std::string&& defaultValue = "")
    {
        if (m_pJson == nullptr)
        {
            return defaultValue;
        }

#ifdef _GTA_
        // Wanted to do this in the macro TEXT/ TEXT_S
        // But VS Code complains about it so..
        defaultValue += "##" + key;
#endif
        return m_pJson->GetValueStr(key, defaultValue);
    }

    /*
        Sets the language to use
        If the language is not found, the default language is used

        index is the index of the language in the GetLocaleList() list
    */
    static eReturnCodes SetLocale(int index);
};