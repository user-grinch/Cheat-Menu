#include "pch.h"
#include "Json.h"

CJson::CJson(const char* name)
{
	if (name == "")
		return;

	m_FilePath = PLUGIN_PATH((char*)"/CheatMenu/json/") + std::string(name) + ".json";

	if (fs::exists(m_FilePath))
	{
		try
		{
			std::ifstream file(m_FilePath);
			file >> m_Data;
			file.close();
		}
		catch (...)
		{
			flog << "Error trying to read " << m_FilePath << std::endl;
			m_Data = "{}"_json;
		}
	}
	else
	{
		m_Data = "{}"_json;

		if (m_FilePath.find("config"))
			flog << "Creating config.json file" << std::endl;
		else
			flog << "Failed to locate file " << m_FilePath << std::endl;
	}
}

void CJson::WriteToDisk()
{
	std::ofstream file(m_FilePath);
	file << m_Data.dump(4, ' ', false, nlohmann::json::error_handler_t::replace) << std::endl;
	file.close();
}

void CJson::LoadData(std::vector<std::string>& vec, std::string& selected) // Test
{
	for (auto element : m_Data.items())
		vec.push_back(element.key());
}
