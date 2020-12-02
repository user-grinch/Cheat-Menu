// This source is taken from MoonAdditions https://github.com/THE-FYP/MoonAdditions
// MIT License

// Copyright (c) 2012 DK22Pac
// Copyright (c) 2017 FYP

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
#include "plugin.h"
#include "vehicle_renderer.h"

auto& VehicleRenderer::getVehicleData(CVehicle* veh)
{
	if (!_vehicleData)
		_vehicleData = std::make_unique<VehicleDataExtended<VehicleData>>();
	return _vehicleData->get(veh);
}

auto& VehicleRenderer::getVehicleMaterialProperties(CVehicle* veh)
{
	return getVehicleData(veh)._materialProperties;
}

void VehicleRenderer::setMaterialColor(CVehicle* veh, RpMaterial* material, RpGeometry* geometry, RwRGBA color, bool filter_mat)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	
	if ( !filter_mat 
		|| (material->color.red == 0x3C && material->color.green == 0xFF && material->color.blue == 0x00)
		|| (material->color.red == 0xFF && material->color.green == 0x00 && material->color.blue == 0xAF))
	{
		matProps._recolor = true;
		matProps._color = color;
		matProps._geometry = geometry;
	}
}

void VehicleRenderer::setMaterialTexture(CVehicle* veh, RpMaterial* material, std::shared_ptr<RwTexture> texture, bool filter_mat)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];

	if (!filter_mat
		|| (material->color.red == 0x3C && material->color.green == 0xFF && material->color.blue == 0x00)
		|| (material->color.red == 0xFF && material->color.green == 0x00 && material->color.blue == 0xAF))
	{
		matProps._retexture = true;
		matProps._texture = texture;
	}

}

void VehicleRenderer::resetMaterialColor(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& matProps = getVehicleMaterialProperties(veh)[material];
		matProps._recolor = false;
		matProps._color = {0, 0, 0, 0};
	}
}

void VehicleRenderer::resetMaterialTexture(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& matProps = getVehicleMaterialProperties(veh)[material];
		matProps._retexture = false;
		matProps._texture.reset();
	}
}

void VehicleRenderer::processRender(CVehicle* veh)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		// reset custom color if color id changed
		if (veh->m_nPrimaryColor != getVehicleData(veh).primary_color
			|| veh->m_nSecondaryColor != getVehicleData(veh).secondary_color)
		{
			for (auto& it : _vehicleData->get(veh)._materialProperties)
				resetMaterialColor(veh, it.first);

			getVehicleData(veh).primary_color = veh->m_nPrimaryColor;
			getVehicleData(veh).secondary_color = veh->m_nSecondaryColor;
		}

		for (auto& it : _vehicleData->get(veh)._materialProperties)
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
}

void VehicleRenderer::postRender(CVehicle* veh)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		for (auto& it : _vehicleData->get(veh)._materialProperties)
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
}
