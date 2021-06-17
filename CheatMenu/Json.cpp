#include "pch.h"
#include "Json.h"

CJson::CJson(const char* name)
{
	if (name == "")
		return;

	file_path = PLUGIN_PATH((char*)"/CheatMenu/json/") + std::string(name) + ".json";

	if (fs::exists(file_path))
	{
		try
		{
			std::ifstream file(file_path);
			file >> data;
			file.close();
		}
		catch (...)
		{
			flog << "Error trying to read " << file_path << std::endl;
			data = "{}"_json;
		}
	}
	else
	{
		data = "{}"_json;

		if (file_path.find("config"))
			flog << "Creating config.json file" << std::endl;
		else
			flog << "Failed to locate file " << file_path << std::endl;
	}
}

void CJson::WriteToDisk()
{
	std::ofstream file(file_path);
	file << data.dump(4, ' ', false, nlohmann::json::error_handler_t::replace) << std::endl;
	file.close();
}

void CJson::LoadData(std::vector<std::string>& vec, std::string& selected) // Test
{
	for (auto element : data.items())
		vec.push_back(element.key());
}