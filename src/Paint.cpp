#include "pch.h"
#include "Paint.h"
#include "NodeName.h"
#include "Util.h"

std::vector<std::string> Paint::veh_nodes::names_vec{ "Default" };
std::string Paint::veh_nodes::selected = "Default";

std::map<std::string, std::shared_ptr<RwTexture>> Paint::textures;

Paint::Paint()
{

	Events::initGameEvent += []
	{
		for (auto &p : fs::recursive_directory_iterator(PLUGIN_PATH((char*)"\\CheatMenu\\vehicles\\paintjobs\\")))
		{
			if (p.path().extension() == ".png")
			{
				std::string file_name = p.path().stem().string();
				textures[file_name]= std::make_shared<RwTexture>(*(Util::LoadTextureFromPngFile(p.path())));
			}
		}
	};
	
	Events::vehicleRenderEvent.before += [](CVehicle* veh) {
		if (veh)
			VehicleRenderer::get()->processRender(veh);
	};

	Events::vehicleResetAfterRender += [](CVehicle* veh) {
		if (veh)
			VehicleRenderer::get()->postRender(veh);
	};
}

Paint::~Paint()
{
}

void Paint::NodeWrapperRecursive(RwFrame *frame, CVehicle* pVeh, std::function<void(RwFrame*)> func)
{
	if (frame)
	{
		func(frame);

		if (RwFrame * newFrame = frame->child)
			NodeWrapperRecursive(newFrame, pVeh, func);
		if (RwFrame * newFrame = frame->next)
			NodeWrapperRecursive(newFrame, pVeh, func);
	}
	return;
}

void Paint::UpdateNodeListRecursive(CVehicle* pVeh)
{
	RwFrame *frame = (RwFrame *)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [](RwFrame *frame)
	{
		const std::string name = GetFrameNodeName(frame);

		if (!(std::find(veh_nodes::names_vec.begin(), veh_nodes::names_vec.end(), name) != veh_nodes::names_vec.end()))
			veh_nodes::names_vec.push_back(name);
	});
}

void Paint::SetNodeColor(CVehicle* pVeh, std::string node_name, CRGBA color, bool filter_mat)
{
	RwFrame *frame = (RwFrame *)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [&](RwFrame *frame)
	{
		const std::string name = GetFrameNodeName(frame);

		struct ST
		{
			CRGBA _color;
			bool _filter;
		} st;

		st._color = color;
		st._filter = filter_mat;

		if (node_name == "Default" || node_name == name)
		{
			RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {
				if (object->type == rpATOMIC)
				{
					RpAtomic *atomic = reinterpret_cast<RpAtomic*>(object);

					ST* st = reinterpret_cast<ST*>(data);
					CRGBA* color = &st->_color;
					bool filter_mat = st->_filter;

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						VehicleRenderer::get()->setMaterialColor(FindPlayerPed()->m_pVehicle, atomic->geometry->matList.materials[i], atomic->geometry, { color->r,color->g,color->b, 255 }, filter_mat);
				}
				return object;
			}, &st);
		}
	});
}


void Paint::SetNodeTexture(CVehicle* pVeh, std::string node_name, std::string texturename, bool filter_mat)
{
	RwFrame *frame = (RwFrame *)pVeh->m_pRwClump->object.parent;
	NodeWrapperRecursive(frame, pVeh, [&](RwFrame *frame)
	{
		const std::string name = GetFrameNodeName(frame);

		struct ST
		{
			std::string _texturename;
			std::map<std::string, std::shared_ptr<RwTexture>> _textures;
			bool _filter;
		} st;

		st._textures = textures;
		st._texturename = texturename;
		st._filter = filter_mat;

		if (node_name == "Default" || node_name == name)
		{
			RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {

				if (object->type == rpATOMIC)
				{
					RpAtomic *atomic = reinterpret_cast<RpAtomic*>(object);

					ST* st = reinterpret_cast<ST*>(data);
					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						VehicleRenderer::get()->setMaterialTexture(FindPlayerPed()->m_pVehicle, atomic->geometry->matList.materials[i], st->_textures[st->_texturename], st->_filter);
				}
				return object;
			}, &st);
		}
	});
}

void Paint::ResetNodeColor(CVehicle *pVeh, std::string node_name)
{
	RwFrame *frame = (RwFrame *)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [&](RwFrame *frame)
	{
		const std::string name = GetFrameNodeName(frame);

		if (node_name == "Default" || node_name == name)
		{
			RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {
				if (object->type == rpATOMIC)
				{
					RpAtomic *atomic = reinterpret_cast<RpAtomic*>(object);

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						VehicleRenderer::get()->resetMaterialColor(FindPlayerPed()->m_pVehicle, atomic->geometry->matList.materials[i]);
				}
				return object;
			}, nullptr);
		}
	});
}

void Paint::ResetNodeTexture(CVehicle *pVeh, std::string node_name)
{
	RwFrame *frame = (RwFrame *)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [&](RwFrame *frame)
	{
		const std::string name = GetFrameNodeName(frame);

		if (node_name == "Default" || node_name == name)
		{
			RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {

				if (object->type == rpATOMIC)
				{
					RpAtomic *atomic = reinterpret_cast<RpAtomic*>(object);

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						VehicleRenderer::get()->resetMaterialTexture(FindPlayerPed()->m_pVehicle, atomic->geometry->matList.materials[i]);
				}
				return object;
			}, nullptr);
		}
	});
}