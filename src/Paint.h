#pragma once
#include "vendor/moon/vehicle_renderer.h"

class Paint : VehicleRenderer
{
public:
	Paint();
	~Paint();

	struct veh_nodes
	{
		static std::vector<std::string> names_vec;
		static std::string selected;
	};
	static std::map<std::string, std::shared_ptr<RwTexture>> textures;

	static void UpdateNodeListRecursive(CVehicle* pVeh);
	static void NodeWrapperRecursive(RwFrame * frame, CVehicle* pVeh, std::function<void(RwFrame*)> func);
	static void SetNodeColor(CVehicle* pVeh, std::string node_name, CRGBA color, bool filter_mat = false);
	static void SetNodeTexture(CVehicle* pVeh, std::string node_name, std::string texturename, bool filter_mat = false);
	static void ResetNodeColor(CVehicle *veh, std::string node_name);
	static void ResetNodeTexture(CVehicle *pVeh, std::string node_name);
};

