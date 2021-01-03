#pragma once
#include "vendor\json.hpp"

class CJson
{

private:
	std::string file_path;
public:
	nlohmann::json data;

	/*
		Returns a value from json structure hierarchy using '.' 
		Example: "Menu.Window.X"
	*/
	// specialize since typeid(std::string) doesn't work

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
	
	template<>
	std::string GetValue(std::string&& key, std::string&& default_val)
	{
		try {
			std::stringstream ss(key);
			std::string line;

			nlohmann::json *json = &data;

			while (getline(ss, line, '.'))
				json = &((*json)[line]);

			return json->get<std::string>();
		}
		catch (...) {
			return default_val;
		}
	}

	/*
		Allows to save values in json hierarchy using '.' 
		Example: "Menu.Window.X"
	*/

	template <typename T>
	void SetValue(std::string&& key, T& val)
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

	template<>
	void SetValue(std::string&& key, std::string& val)
	{
		std::stringstream ss(key);
		std::string line;

		nlohmann::json *json = &data;

		while (getline(ss, line, '.'))
			json = &((*json)[line]);

		*json = val;
	}
	/*
		Loads the section names into a category vector. 
		Used to create drop down category menus
	*/
	void LoadData(std::vector<std::string>& vec, std::string& selected);

	/*
		Saves json data to disk
	*/
	void WriteToDisk();
	CJson(const char* text);
	~CJson();
};


