#include "pch.h"
#include "Json.h"

CJson::CJson(const char* name,bool create_new)
{
	file_path = "./CheatMenu/json/"+ std::string(name) +".json";

	if (std::experimental::filesystem::exists(file_path))
	{
		std::ifstream file(file_path);
		file >> data;
		file.close();
	}
	else
	{
		if (create_new)
		{
			std::fstream new_file(file_path,std::fstream::out);
			data = "{}"_json;
			new_file.close();
		}
		else flog << "File doesn't exist " << file_path << std::endl;
	}
}

void CJson::LoadJsonData(std::vector<std::string>& vec, std::string& selected) // Test
{
	for (auto element : data.items())
		vec.push_back(element.key());
}

CJson::~CJson()
{
	std::ofstream file(file_path);
	file << data.dump(4,' ',false, nlohmann::json::error_handler_t::replace) << std::endl;
	file.close();
}
