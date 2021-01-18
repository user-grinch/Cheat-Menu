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

#pragma once

class Paint
{

private:

	// store vehicle specific data
	struct VehData
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

		// carcols color id
		uchar primary_color = 0;
		uchar secondary_color = 0;
		std::unordered_map<RpMaterial*, MaterialProperties> materialProperties;

		VehData(CVehicle* veh)
		{
			primary_color = veh->m_nPrimaryColor;
			secondary_color = veh->m_nSecondaryColor;
		}

		void setMaterialColor(RpMaterial* material, RpGeometry* geometry, RwRGBA color, bool filter_mat = false);
		void setMaterialTexture(RpMaterial* material, std::shared_ptr<RwTexture> texture9, bool filter_mat = false);
		void resetMaterialColor(RpMaterial* material);
		void resetMaterialTexture(RpMaterial* material);
	};

	static VehicleExtendedData<VehData> vehdata;

protected:

	static std::map<std::string, std::shared_ptr<RwTexture>> textures;
	struct veh_nodes
	{
		static std::vector<std::string> names_vec;
		static std::string selected;
	};
	
	Paint();
	~Paint();
	static void UpdateNodeListRecursive(CVehicle* pVeh);
	static void NodeWrapperRecursive(RwFrame * frame, CVehicle* pVeh, std::function<void(RwFrame*)> func);
	static void SetNodeColor(CVehicle* pVeh, std::string node_name, CRGBA color, bool filter_mat = false);
	static void SetNodeTexture(CVehicle* pVeh, std::string node_name, std::string texturename, bool filter_mat = false);
	static void ResetNodeColor(CVehicle *veh, std::string node_name);
	static void ResetNodeTexture(CVehicle *pVeh, std::string node_name);
};

