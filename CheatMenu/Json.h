#pragma once
#include "external\json.hpp"

class CJson
{

private:
	std::string file_path;
public:
	nlohmann::json data;
	template <typename T>
	T GetValue(std::string&& key, T&& default_val)
	{
		try {
			std::stringstream ss(key);
			std::string line;

			nlohmann::json *json = &data;

			while (getline(ss, line, '.'))
				json = &((*json)[line]);

			// json library bugs with bool, using int instead
			if (typeid(T) == typeid(bool))
			{
				return ((json->get<int>() == 1) ? true : false);
			}
			else
				return json->get<T>();
		}
		catch (...) {
			return default_val;
		}
	}
	template <typename T>
	void SetValue(std::string&& key, T&& val)
	{
		std::stringstream ss(key);
		std::string line;

		nlohmann::json *json = &data;

		while (getline(ss, line, '.'))
			json = &((*json)[line]);

		// json library bugs with bool, using int instead
		if (typeid(T) == typeid(bool))
			*json = (val ? 1 : 0);
		else
			*json = val;
	}
	void LoadJsonData(std::vector<std::string>& vec, std::string& selected);
	CJson(const char* text, bool create_new = false);
	virtual ~CJson();
};


