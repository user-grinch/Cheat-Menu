#include "pch.h"
#include "Neon.h"
#include "Util.h"

void Neon::RenderEvent(CVehicle* pVeh)
{
	NeonData* data = &m_VehNeon.Get(pVeh);
	if (data->m_bNeonInstalled && !pVeh->IsUpsideDown())
	{
		CVector Pos = CModelInfo::GetModelInfo(pVeh->m_nModelIndex)->m_pColModel->m_boundBox.m_vecMin;
		CVector center = pVeh->TransformFromObjectSpace(CVector(0.0f, 0.0f, 0.0f));
		CVector up = pVeh->TransformFromObjectSpace(CVector(0.0f, -Pos.y - data->m_fVal, 0.0f)) - center;
		CVector right = pVeh->TransformFromObjectSpace(CVector(Pos.x + data->m_fVal, 0.0f, 0.0f)) - center;
		CShadows::StoreShadowToBeRendered(5, m_pNeonTexture, &center, up.x, up.y, right.x, right.y, 180, data->m_Color.r,
		                                  data->m_Color.g, data->m_Color.b, 2.0f, false, 1.0f, 0, true);

		if (CTimer::m_snTimeInMilliseconds - data->m_nTimer > 150)
		{
			data->m_nTimer = CTimer::m_snTimeInMilliseconds;

			if (data->m_bPulsing)
			{
				if (data->m_fVal < 0.0f)
					data->m_bIncrement = true;

				if (data->m_fVal > 0.3f)
					data->m_bIncrement = false;

				if (data->m_bIncrement)
					data->m_fVal += 0.1f;
				else
					data->m_fVal -= 0.1f;
			}
		}
	}
}

Neon::Neon()
{
	Events::processScriptsEvent += [this]
	{
		if (!m_bMaskLoaded)
		{
			m_pNeonTexture = Util::LoadTextureFromPngFile(PLUGIN_PATH((char*)"CheatMenu\\vehicles\\neon_mask.png"));
			if (!m_pNeonTexture)
				flog << "Failed to load neon mask" << std::endl;

			m_bMaskLoaded = true;
		}
	};

	Events::vehicleRenderEvent += RenderEvent;
}

Neon::~Neon()
{
	Events::vehicleRenderEvent -= RenderEvent;

	if (m_pNeonTexture)
	{
		RwTextureDestroy(m_pNeonTexture);
		m_pNeonTexture = nullptr;
	}
}

bool Neon::IsNeonInstalled(CVehicle* pVeh)
{
	return m_VehNeon.Get(pVeh).m_bNeonInstalled;
}

bool Neon::IsPulsingEnabled(CVehicle* pVeh)
{
	return m_VehNeon.Get(pVeh).m_bPulsing;
}

void Neon::SetPulsing(CVehicle* pVeh, bool state)
{
	m_VehNeon.Get(pVeh).m_bPulsing = state;
}

void Neon::InstallNeon(CVehicle* pVeh, int red, int green, int blue)
{
	CRGBA& color = m_VehNeon.Get(pVeh).m_Color;

	color.r = red;
	color.g = green;
	color.b = blue;
	color.a = 255;

	m_VehNeon.Get(pVeh).m_bNeonInstalled = true;
}

void Neon::RemoveNeon(CVehicle* pVeh)
{
	m_VehNeon.Get(pVeh).m_bNeonInstalled = false;
}
