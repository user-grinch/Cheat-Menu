#include "pch.h"
#include "paint_sa.h"
#include "utils/util.h"
#include "../../include/kiero/minhook/MinHook.h"

PaintMgr& Paint = PaintMgr::Get();

// This works with silentpatch
static auto oEntityRender = (void(__fastcall*)(CVehicle*))0x534310;
void __fastcall hkEntityRender(CVehicle* pVeh)
{
    if (!pVeh || pVeh->m_nType != ENTITY_TYPE_VEHICLE)
    {
        oEntityRender(pVeh);
        return;
    }
    auto& data = Paint.GetData(pVeh);

    /*
        Game colors won't ve visible over material colors
        So reset material colors & apply game colors here
    */
    if (pVeh->m_nPrimaryColor != data.m_nCarColors[0]
            || pVeh->m_nSecondaryColor != data.m_nCarColors[1])
    {
        for (auto& it : data.m_nMapInfoList)
        {
            data.ResetMatColor(it.first);
        }

        data.m_nCarColors[0] = pVeh->m_nPrimaryColor;
        data.m_nCarColors[1] = pVeh->m_nSecondaryColor;
        data.m_nCarColors[2] = pVeh->m_nTertiaryColor;
        data.m_nCarColors[3] = pVeh->m_nQuaternaryColor;
    }


    /*
        Applying our custom material colors here
    */
    for (auto& it : data.m_nMapInfoList)
    {
        if (it.second.m_bRecolor)
        {
            it.second.m_nOriginalColor = it.first->color;
            it.first->color = it.second.m_nColor;
            it.second.m_nOriginalGeometryFlags = it.second.m_pGeometry->flags;
            it.second.m_pGeometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
        }

        if (it.second.m_bRetexture)
        {
            auto tex = it.second.m_pTexture;
            if (tex)
            {
                it.second.m_pOriginalTexture = it.first->texture;
                it.first->texture = tex;
            }
            else
            {
                it.second.m_bRetexture = false;
            }
        }
    }
    oEntityRender(pVeh);
};

PaintMgr::PaintMgr()
{
    MH_Initialize();
    MH_CreateHook((void*)0x534310, hkEntityRender, (void**)&oEntityRender);
    MH_EnableHook((void*)0x534310);

    // This doesn't work for helicopters? SilentPatch?
    static ThiscallEvent<AddressList<0x55332A, H_CALL>, PRIORITY_BEFORE, ArgPickN<CVehicle*, 0>, void(CVehicle*)> vehicleResetAfterRender;
    vehicleResetAfterRender += [this](CVehicle* pVeh)
    {
        PaintData& data = m_VehPaint.Get(pVeh);
        for (auto& it : data.m_nMapInfoList)
        {
            if (it.second.m_bRecolor)
            {
                it.first->color = it.second.m_nOriginalColor;
                it.second.m_pGeometry->flags = it.second.m_nOriginalGeometryFlags;
            }
            if (it.second.m_bRetexture)
            {
                it.first->texture = it.second.m_pOriginalTexture;
            }
        }
    };
}

void PaintMgr::SetCarcols(CVehicle *pVeh, uint primary, uint secondary, uint tertiary, uint quaternary, bool reset)
{
    *(uint8_replacement*)(int(pVeh) + 0x433 + 1) = primary;
    *(uint8_replacement*)(int(pVeh) + 0x433 + 2) = secondary;
    *(uint8_replacement*)(int(pVeh) + 0x433 + 3) = tertiary;
    *(uint8_replacement*)(int(pVeh) + 0x433 + 4) = quaternary;

    // stop trigger reset
    if (!reset)
    {
        auto& data = Paint.GetData(pVeh);
        data.m_nCarColors[0] = *(uint8_replacement*)(int(pVeh) + 0x433 + 1);
        data.m_nCarColors[1] = *(uint8_replacement*)(int(pVeh) + 0x433 + 2);
        data.m_nCarColors[2] = *(uint8_replacement*)(int(pVeh) + 0x433 + 3);
        data.m_nCarColors[3] = *(uint8_replacement*)(int(pVeh) + 0x433 + 4);
    }

}

void PaintMgr::PaintData::SetMatColor(RpMaterial* material, RpGeometry* geometry, RwRGBA color)
{
    auto& matInfo = m_nMapInfoList[material];
    if ((material->color.red == 0x3C && material->color.green == 0xFF && material->color.blue == 0x00)
            || (material->color.red == 0xFF && material->color.green == 0x00 && material->color.blue == 0xAF))
    {
        matInfo.m_bRecolor = true;
        matInfo.m_nColor = color;
        matInfo.m_pGeometry = geometry;
    }
}

void PaintMgr::PaintData::SetMatTexture(RpMaterial* material, RwTexture* texture)
{
    auto& matInfo = m_nMapInfoList[material];
    if ((material->color.red == 0x3C && material->color.green == 0xFF && material->color.blue == 0x00)
            || (material->color.red == 0xFF && material->color.green == 0x00 && material->color.blue == 0xAF))
    {
        matInfo.m_bRetexture = true;
        matInfo.m_pTexture = texture;
    }
}

void PaintMgr::PaintData::ResetMatColor(RpMaterial* material)
{
    auto& matInfo = m_nMapInfoList[material];
    matInfo.m_bRecolor = false;
    matInfo.m_nColor = {0, 0, 0, 0};
}

void PaintMgr::PaintData::ResetMatTexture(RpMaterial* material)
{
    auto& matInfo = m_nMapInfoList[material];
    matInfo.m_bRetexture = false;
    matInfo.m_pTexture = nullptr;
}

PaintMgr::PaintData& PaintMgr::GetData(CVehicle *pVeh)
{
    return m_VehPaint.Get(pVeh);
}

static void NodeWrapperRecursive(RwFrame* frame, CVehicle* pVeh, std::function<void(RwFrame*)> func)
{
    if (frame)
    {
        func(frame);
        if (RwFrame* newFrame = frame->child)
        {
            NodeWrapperRecursive(newFrame, pVeh, func);
        }
        if (RwFrame* newFrame = frame->next)
        {
            NodeWrapperRecursive(newFrame, pVeh, func);
        }
    }
    return;
}

struct BindData
{
    void *pData;
    CRGBA color;
    RwTexture* pTexture;
};

void PaintMgr::SetColor(CVehicle* pVeh, CRGBA color)
{
    RwFrame* pFrame = (RwFrame*)pVeh->m_pRwClump->object.parent;
    NodeWrapperRecursive(pFrame, pVeh, [&](RwFrame* frame)
    {
        BindData bindData { &m_VehPaint.Get(pVeh), color };
        RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject*
        {
            if (object->type == rpATOMIC)
            {
                RpAtomic* atomic = reinterpret_cast<RpAtomic*>(object);

                BindData* bind = reinterpret_cast<BindData*>(data);
                CRGBA color = bind->color;
                PaintData* pData = reinterpret_cast<PaintData*>(bind->pData);

                for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
                {
                    pData->SetMatColor(atomic->geometry->matList.materials[i], atomic->geometry,
                    {color.r, color.g, color.b, color.a});
                }
            }
            return object;
        }, &bindData);
    });
}

void PaintMgr::SetTexture(CVehicle* pVeh, std::string name)
{
    RwFrame* pFrame = (RwFrame*)pVeh->m_pRwClump->object.parent;
    RwTexture* pTexture = nullptr;

    // find the texture
    for (auto const& tex : m_TextureData.m_ImagesList)
    {
        if (tex.get()->m_FileName == name)
        {
            pTexture = tex.get()->m_pRwTexture;
            break;
        }
    }

    if (!pTexture)
    {
        return;
    }

    m_VehPaint.Get(pVeh).m_nTextureName = name;
    NodeWrapperRecursive(pFrame, pVeh, [&](RwFrame* frame)
    {
        BindData bindData { &m_VehPaint.Get(pVeh), {}, pTexture };
        RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject*
        {
            if (object->type == rpATOMIC)
            {
                RpAtomic* atomic = reinterpret_cast<RpAtomic*>(object);

                BindData* bind = reinterpret_cast<BindData*>(data);
                RwTexture *pTex = bind->pTexture;
                PaintData* pData = reinterpret_cast<PaintData*>(bind->pData);

                for (int i = 0; i < atomic->geometry->matList.numMaterials; ++i)
                {
                    pData->SetMatTexture(atomic->geometry->matList.materials[i], pTex);
                }
            }
            return object;
        }, &bindData);
    });
}

void PaintMgr::ResetColor(CVehicle* pVeh)
{
    RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;
    NodeWrapperRecursive(frame, pVeh, [&](RwFrame* frame)
    {
        RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject*
        {
            if (object->type == rpATOMIC)
            {
                RpAtomic* pAtomic = reinterpret_cast<RpAtomic*>(object);
                PaintData* pData = reinterpret_cast<PaintData*>(data);

                for (int i = 0; i < pAtomic->geometry->matList.numMaterials; ++i)
                {
                    pData->ResetMatColor(pAtomic->geometry->matList.materials[i]);
                }
            }
            return object;
        }, &m_VehPaint.Get(pVeh));
    });
}

void PaintMgr::ResetTexture(CVehicle* pVeh)
{
    RwFrame* frame = (RwFrame*)pVeh->m_pRwClump->object.parent;
    NodeWrapperRecursive(frame, pVeh, [&](RwFrame* frame)
    {
        RwFrameForAllObjects(frame, [](RwObject* object, void* data) -> RwObject*
        {
            if (object->type == rpATOMIC)
            {
                RpAtomic* pAtomic = reinterpret_cast<RpAtomic*>(object);
                PaintData* pData = reinterpret_cast<PaintData*>(data);

                for (int i = 0; i < pAtomic->geometry->matList.numMaterials; ++i)
                {
                    pData->ResetMatTexture(pAtomic->geometry->matList.materials[i]);
                }
            }
            return object;
        }, &m_VehPaint.Get(pVeh));
    });
    m_VehPaint.Get(pVeh).m_nTextureName = "";
}