#pragma once
#include "../depend/json.hpp"

/*
	Wrapper class for nlohmann::json
	Contains helper methods
*/
class CJson
{
private:
    std::string m_FilePath;

public:
    nlohmann::json m_Data;

    /*
    	Returns a value from json structure hierarchy using '.'
    	Example: "Menu.Window.X"
    */
    // specialize since typeid(std::string) doesn't work

    template <typename T>
    T inline GetValue(std::string&& key, const T& defaultVal)
    {
        try
        {
            std::stringstream ss(key);
            std::string line;

            nlohmann::json* json = &m_Data;

            while (getline(ss, line, '.'))
            {
                json = &((*json)[line]);
            }

            // json library bugs with bool, using int instead
            if (typeid(T) == typeid(bool))
            {
                return ((json->get<int>() == 1) ? true : false);
            }
            return json->get<T>();
        }
        catch (...)
        {
            return defaultVal;
        }
    }

    std::string inline GetValueStr(const std::string& key, const std::string& defaultVal)
    {
        try
        {
            std::stringstream ss(key);
            std::string line;

            nlohmann::json* json = &m_Data;

            while (getline(ss, line, '.'))
            {
                json = &((*json)[line]);
            }

            return json->get<std::string>();
        }
        catch (...)
        {
            return defaultVal;
        }
    }

    /*
    	Allows to save values in json hierarchy using '.'
    	Example: "Menu.Window.X"
    */
    template <typename T>
    void inline SetValue(std::string&& key, const T& val)
    {
        std::stringstream ss(key);
        std::string line;

        nlohmann::json* json = &m_Data;

        while (getline(ss, line, '.'))
        {
            json = &((*json)[line]);

        }

        // json library bugs with bool, using int instead
        if (typeid(T) == typeid(bool))
        {
            *json = (val ? 1 : 0);
        }
        else
        {
            *json = val;
        }
    }

    template <>
    void inline SetValue(std::string&& key, const std::string& val)
    {
        std::stringstream ss(key);
        std::string line;

        nlohmann::json* json = &m_Data;

        while (getline(ss, line, '.'))
        {
            json = &((*json)[line]);
        }

        *json = val;
    }

    /*
    	Saves json data to disk
    */
    void Save();
    CJson(const char* text, bool pathPredefined = false);
};
