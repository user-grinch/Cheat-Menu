#include "pch.h"
#include "NeonAPI.h"

VehicleExtendedData<NeonAPI::NeonData> NeonAPI::VehNeon;
RwTexture* NeonAPI::neon_texture = nullptr;

NeonAPI::NeonAPI()
{
	Events::initGameEvent += [this]
	{
        neon_texture = Util::LoadTextureFromPngFile(PLUGIN_PATH((char*)"CheatMenu\\vehicles\\neon_mask.png"));

		if (!neon_texture)
			flog << "Failed to load neon mask" << std::endl;
	};

	Events::vehicleRenderEvent += [](CVehicle *pVeh)
	{
		NeonData *data = &VehNeon.Get(pVeh);
		if (data->neon_installed && !pVeh->IsUpsideDown())
		{
			CVector Pos = CModelInfo::ms_modelInfoPtrs[pVeh->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMin;
        	CVector center = pVeh->TransformFromObjectSpace(CVector(0.0f, 0.0f, 0.0f));
			CVector up = pVeh->TransformFromObjectSpace(CVector(0.0f, -Pos.y - data->val, 0.0f)) - center;
        	CVector right = pVeh->TransformFromObjectSpace(CVector(Pos.x + data->val, 0.0f, 0.0f)) - center;
        	CShadows::StoreShadowToBeRendered(5, neon_texture, &center, up.x, up.y, right.x, right.y, 180, data->color.r, data->color.g, data->color.b, 2.0f, false, 1.0f, 0, true);

			if (CTimer::m_snTimeInMilliseconds - data->timer > 150)
			{
				data->timer = CTimer::m_snTimeInMilliseconds;

				if (data->val < 0.0f)
					data->increment = true;

				if (data->val > 0.3f)
					data->increment = false;

				if (data->increment)
					data->val += 0.1f;
				else
					data->val -= 0.1f;

			}
		}
	};
}

NeonAPI::~NeonAPI()
{
	delete neon_texture;
}

bool NeonAPI::IsNeonInstalled(CVehicle *pVeh)
{
	return VehNeon.Get(pVeh).neon_installed;
}

void NeonAPI::InstallNeon(CVehicle *pVeh, int red, int green, int blue)
{
	CRGBA &color = VehNeon.Get(pVeh).color;

	color.r = red;
	color.g = green;
	color.b = blue;
	color.a = 255;

	VehNeon.Get(pVeh).neon_installed = true;
}

void NeonAPI::RemoveNeon(CVehicle *pVeh)
{
	VehNeon.Get(pVeh).neon_installed = false;
}
