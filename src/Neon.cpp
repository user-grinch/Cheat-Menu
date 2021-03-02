#include "pch.h"
#include "Neon.h"
#include "Util.h"

void Neon::RenderEvent(CVehicle *pVeh)
{
	NeonData* data = &VehNeon.Get(pVeh);
	if (data->neon_installed && !pVeh->IsUpsideDown())
	{
		CVector Pos = CModelInfo::GetModelInfo(pVeh->m_nModelIndex)->m_pColModel->m_boundBox.m_vecMin;
		CVector center = pVeh->TransformFromObjectSpace(CVector(0.0f, 0.0f, 0.0f));
		CVector up = pVeh->TransformFromObjectSpace(CVector(0.0f, -Pos.y - data->val, 0.0f)) - center;
		CVector right = pVeh->TransformFromObjectSpace(CVector(Pos.x + data->val, 0.0f, 0.0f)) - center;
		CShadows::StoreShadowToBeRendered(5, neon_texture, &center, up.x, up.y, right.x, right.y, 180, data->color.r, data->color.g, data->color.b, 2.0f, false, 1.0f, 0, true);

		if (CTimer::m_snTimeInMilliseconds - data->timer > 150)
		{
			data->timer = CTimer::m_snTimeInMilliseconds;

			if (data->pulsing)
			{
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
	}
}

Neon::Neon()
{
	neon_texture = Util::LoadTextureFromPngFile(PLUGIN_PATH((char*)"CheatMenu\\vehicles\\neon_mask.png"));
	if (!neon_texture)
		flog << "Failed to load neon mask" << std::endl;

	Events::vehicleRenderEvent += RenderEvent;
}

Neon::~Neon()
{	
	Events::vehicleRenderEvent -= RenderEvent;
	RwTextureDestroy(neon_texture);
}

bool Neon::IsNeonInstalled(CVehicle* pVeh)
{
	return VehNeon.Get(pVeh).neon_installed;
}

bool Neon::IsPulsingEnabled(CVehicle* pVeh)
{
	return VehNeon.Get(pVeh).pulsing;
}

void Neon::SetPulsing(CVehicle* pVeh, bool state)
{
	VehNeon.Get(pVeh).pulsing = state;
}

void Neon::InstallNeon(CVehicle* pVeh, int red, int green, int blue)
{
	CRGBA& color = VehNeon.Get(pVeh).color;

	color.r = red;
	color.g = green;
	color.b = blue;
	color.a = 255;

	VehNeon.Get(pVeh).neon_installed = true;
}

void Neon::RemoveNeon(CVehicle* pVeh)
{
	VehNeon.Get(pVeh).neon_installed = false;
}
