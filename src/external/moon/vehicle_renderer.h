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

#pragma once
#include "pool_object_extender.h"
class VehicleRenderer
{
	struct MaterialProperties
	{
		MaterialProperties() :
			_color{0, 0, 0, 0},
			_recolor(false),
			_retexture(false),
			_originalColor{0, 0, 0, 0},
			_originalTexture(nullptr),
			_originalGeometryFlags(0),
			_geometry(nullptr)
		{
		}

		RwRGBA _color;
		std::weak_ptr<RwTexture> _texture;
		bool _recolor;
		bool _retexture;
		RpGeometry* _geometry;
		RwRGBA _originalColor;
		RwTexture* _originalTexture;
		RwInt32 _originalGeometryFlags;
	};

	struct VehicleData
	{
		VehicleData(const CVehicle* veh)
		{
			primary_color = veh->m_nPrimaryColor;
			secondary_color = veh->m_nSecondaryColor;
		}

		// carcols color id
		uchar primary_color;
		uchar secondary_color;

		std::unordered_map<RpMaterial*, MaterialProperties> _materialProperties;
	};

public:
	void setMaterialColor(CVehicle* veh, RpMaterial* material, RpGeometry* geometry, RwRGBA color, bool filter_mat = false);
	void setMaterialTexture(CVehicle* veh, RpMaterial* material, std::shared_ptr<RwTexture> texture9, bool filter_mat = false);
	void resetMaterialColor(CVehicle* veh, RpMaterial* material);
	void resetMaterialTexture(CVehicle* veh, RpMaterial* material);
	void processRender(CVehicle* veh);
	void postRender(CVehicle* veh);
	bool isInitialized() const { return _vehicleData != nullptr; }

	static VehicleRenderer* get()
	{
		static std::unique_ptr<VehicleRenderer> instance = std::make_unique<VehicleRenderer>();
		return instance.get();
	}

private:
	auto& getVehicleMaterialProperties(CVehicle* veh);
	auto& getVehicleData(CVehicle* veh);

	std::unique_ptr<VehicleDataExtended<VehicleData>> _vehicleData;
};
