#include "pch.h"
#include "Json.h"

CJson::CJson(const char* name)
{
	file_path = "./CheatMenu/json/"+ std::string(name) +".json";
	
	if (std::experimental::filesystem::exists(file_path))
	{
		try
		{
			std::ifstream file(file_path);
			file >> data;
			file.close();
		}
		catch (...)
		{
			flog << "Error occured trying to read " << file_path << std::endl;
			data = "{}"_json;
		}
	}
	else
	{
		data = "{}"_json;
		flog << "File doesn't exist " << file_path << std::endl;
	}
}

void CJson::WriteToDisk()
{
	std::ofstream file(file_path);
	file << data.dump(4,' ',false, nlohmann::json::error_handler_t::replace) << std::endl;
	file.close();
}

void CJson::LoadData(std::vector<std::string>& vec, std::string& selected) // Test
{
	for (auto element : data.items())
		vec.push_back(element.key());
}

std::string CJson::GetValueStr(std::string&& key, std::string&& default_val)
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

void CJson::SetValueStr(std::string&& key, std::string& val)
{
	std::stringstream ss(key);
	std::string line;

	nlohmann::json *json = &data;

	while (getline(ss, line, '.'))
		json = &((*json)[line]);

	*json = val;
}

CJson::~CJson()
{
	// Saving here won't work on crash
}
