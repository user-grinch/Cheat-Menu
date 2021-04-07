// Portion of this source is taken from MoonAdditions https://github.com/THE-FYP/MoonAdditions
// To keep the licensing simple this file would go under MIT License, GPLv3 won't apply

// Copyright (c) 2012 DK22Pac
// Copyright (c) 2017 FYP
// Copyright (c) 2021 Grinch_

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pch.h"
#include "Paint.h"
#include "NodeName.h"
#include "Util.h"

void Paint::RenderEvent(CVehicle* pVeh)
{
	VehData& data = vehdata.Get(pVeh);

	// reset custom color if color id changed
	if (pVeh->m_nPrimaryColor != data.primary_color
		|| pVeh->m_nSecondaryColor != data.secondary_color)
	{
		for (auto& it : data.materialProperties)
			data.resetMaterialColor(it.first);

		data.primary_color = pVeh->m_nPrimaryColor;
		data.secondary_color = pVeh->m_nSecondaryColor;
	}

	for (auto& it : data.materialProperties)
	{
		if (it.second._recolor)
		{
			it.second._originalColor = it.first->color;
			it.first->color = it.second._color;
			it.second._originalGeometryFlags = it.second._geometry->flags;
			it.second._geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
		}
		if (it.second._retexture)
		{
			auto tex = it.second._texture.lock();
			if (tex)
			{
				it.second._originalTexture = it.first->texture;
				it.first->texture = tex.get();
			}
			else
			{
				it.second._retexture = false;
			}
		}
	}
}

void Paint::ResetAfterRenderEvent(CVehicle* pVeh)
{
	for (auto& it : vehdata.Get(pVeh).materialProperties)
	{
		if (it.second._recolor)
		{
			it.first->color = it.second._originalColor;
			it.second._geometry->flags = it.second._originalGeometryFlags;
		}
		if (it.second._retexture)
		{
			it.first->texture = it.second._originalTexture;
		}
	}
}

Paint::Paint()
{
	Events::processScriptsEvent += [this]
	{
		if (!images_loaded)
		{
			for (auto& p : fs::recursive_directory_iterator(PLUGIN_PATH((char*)"\\CheatMenu\\vehicles\\paintjobs\\")))
			{
				if (p.path().extension() == ".png")
				{
					std::string file_name = p.path().stem().string();
					textures[file_name] = std::make_shared<RwTexture>(*(Util::LoadTextureFromPngFile(p.path())));
				}
			}
			images_loaded = true;
		}
	};

	Events::vehicleRenderEvent.before += RenderEvent;
	Events::vehicleResetAfterRender += ResetAfterRenderEvent;
}

Paint::~Paint()
{
	Events::vehicleRenderEvent.before -= RenderEvent;
	Events::vehicleResetAfterRender -= ResetAfterRenderEvent;
}

void Paint::VehData::setMaterialColor(RpMaterial* material, RpGeometry* geometry, RwRGBA color, bool filter_mat)
{
	auto& matProps = materialProperties[material];

	if (!filter_mat
		|| (material->color.red == 0x3C && material->color.green == 0xFF && material->color.blue == 0x00)
		|| (material->color.red == 0xFF && material->color.green == 0x00 && material->color.blue == 0xAF))
	{
		matProps._recolor = true;
		matProps._color = color;
		matProps._geometry = geometry;
	}
}

void Paint::VehData::setMaterialTexture(RpMaterial* material, std::shared_ptr<RwTexture> texture, bool filter_mat)
{
	auto& matProps = materialProperties[material];

	if (!filter_mat
		|| (material->color.red == 0x3C && material->color.green == 0xFF && material->color.blue == 0x00)
		|| (material->color.red == 0xFF && material->color.green == 0x00 && material->color.blue == 0xAF))
	{
		matProps._retexture = true;
		matProps._texture = texture;
	}
}

void Paint::VehData::resetMaterialColor(RpMaterial* material)
{
	auto& matProps = materialProperties[material];
	matProps._recolor = false;
	matProps._color = { 0, 0, 0, 0 };
}

void Paint::VehData::resetMaterialTexture(RpMaterial* material)
{
	auto& matProps = materialProperties[material];
	matProps._retexture = false;
	matProps._texture.reset();
}

void Paint::NodeWrapperRecursive(RwFrame* frame, CVehicle* pVeh, std::function<void(RwFrame*)> func)
{
	if (frame)
	{
		func(frame);

		if (RwFrame* newFrame = frame->child)
			NodeWrapperRecursive(newFrame, pVeh, func);
		if (RwFrame* newFrame = frame->next)
			NodeWrapperRecursive(newFrame, pVeh, func);
	}
	return;
}

void Paint::UpdateNodeListRecursive(CVehicle* pVeh)
{
	RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [](RwFrame* frame)
	{
		const std::string name = GetFrameNodeName(frame);

		if (!(std::find(veh_nodes::names_vec.begin(), veh_nodes::names_vec.end(), name) != veh_nodes::names_vec.end()))
			veh_nodes::names_vec.push_back(name);
	});
}

void Paint::SetNodeColor(CVehicle* pVeh, std::string node_name, CRGBA color, bool filter_mat)
{
	RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [&](RwFrame* frame)
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
					RpAtomic* atomic = reinterpret_cast<RpAtomic*>(object);

					ST* st = reinterpret_cast<ST*>(data);
					CRGBA* color = &st->_color;
					bool filter_mat = st->_filter;

					VehData& data = vehdata.Get(FindPlayerPed()->m_pVehicle);

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						data.setMaterialColor(atomic->geometry->matList.materials[i], atomic->geometry, { color->r,color->g,color->b, 255 }, filter_mat);
				}
				return object;
			}, &st);
		}
	});
}


void Paint::SetNodeTexture(CVehicle* pVeh, std::string node_name, std::string texturename, bool filter_mat)
{
	RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;
	NodeWrapperRecursive(frame, pVeh, [&](RwFrame* frame)
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
					RpAtomic* atomic = reinterpret_cast<RpAtomic*>(object);

					ST* st = reinterpret_cast<ST*>(data);
					VehData& data = vehdata.Get(FindPlayerPed()->m_pVehicle);

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						data.setMaterialTexture(atomic->geometry->matList.materials[i], st->_textures[st->_texturename], st->_filter);
				}
				return object;
			}, &st);
		}
	});
}

void Paint::ResetNodeColor(CVehicle* pVeh, std::string node_name)
{
	RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [&](RwFrame* frame)
	{
		const std::string name = GetFrameNodeName(frame);

		if (node_name == "Default" || node_name == name)
		{
			RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {
				if (object->type == rpATOMIC)
				{
					RpAtomic* atomic = reinterpret_cast<RpAtomic*>(object);
					VehData& data = vehdata.Get(FindPlayerPed()->m_pVehicle);

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						data.resetMaterialColor(atomic->geometry->matList.materials[i]);
				}
				return object;
			}, nullptr);
		}
	});
}

void Paint::ResetNodeTexture(CVehicle* pVeh, std::string node_name)
{
	RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;

	NodeWrapperRecursive(frame, pVeh, [&](RwFrame* frame)
	{
		const std::string name = GetFrameNodeName(frame);

		if (node_name == "Default" || node_name == name)
		{
			RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject* {

				if (object->type == rpATOMIC)
				{
					RpAtomic* atomic = reinterpret_cast<RpAtomic*>(object);

					VehData& data = vehdata.Get(FindPlayerPed()->m_pVehicle);

					for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
						data.resetMaterialTexture(atomic->geometry->matList.materials[i]);
				}
				return object;
			}, nullptr);
		}
	});
}