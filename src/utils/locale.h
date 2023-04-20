#pragma once
#include <vector>
#include <string>
#include "datastore.h"

/*
    A custom i18n library
    Loads strings from a json file
    Requires the DataStore class
*/
class Locale
{
private:
    static inline std::vector<std::string> m_locales;
    static inline std::string m_path;
    static inline DataStore *m_pData = nullptr;
    static inline DataStore *m_pCallbackData = nullptr;
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
    static eReturnCodes Init(const char* path, const char* def = "English", const char* fallback = "");

    // Returns a vector of available languages
    static std::vector<std::string>& GetLocaleList();
    static size_t GetCurrentLocaleIndex();

    /*
        Returns the string for the given key for the language specified
        If the key is not found, returns the defaultValue

        You need to call SetLanguage once before calling this function
        By default, the language is set to "en"
    */
    static std::string GetText(std::string&& key, std::string&& defaultValue = "");

    /*
        Sets the language to use
        If the language is not found, the default language is used

        index is the index of the language in the GetLocaleList() list
    */
    static eReturnCodes SetLocale(size_t index);
    static eReturnCodes SetLocaleByName(const std::string &name);

    /*
        Sets the language back to default (aka English)
    */
    static void SetDefaultLocale();
};