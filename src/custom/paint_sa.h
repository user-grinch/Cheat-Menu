#pragma once
#include "pch.h"
#include "interface/icheat.hpp"
/*
	Vehicle color & texturring implementation class for GTA: San Andreas

	TODO: Implement for VC & 3 too (maybe)
	Dunno how it'd work with the d3d8to9 wrapper
*/
class PaintMgr : public ICheat<PaintMgr>
{
private:
    struct PaintData
    {
        struct MatInfo
        {
            bool m_bRecolor = false;
            bool m_bRetexture = false;
            RwRGBA m_nColor = {0, 0, 0, 0};
            RwRGBA m_nOriginalColor = {0, 0, 0, 0};
            RwTexture* m_pTexture = nullptr;
            RwTexture* m_pOriginalTexture = nullptr;
            RpGeometry* m_pGeometry = nullptr;
            RwInt32 m_nOriginalGeometryFlags = 0;
        };
        uchar m_nCarColors[4];  // carcols color IDs (primary, secondary, tertiary, quaternary)
        std::string m_nTextureName = ""; // current applied texture name

        std::unordered_map<RpMaterial*, MatInfo> m_nMapInfoList;

        PaintData(CVehicle* pVeh)
        {
            m_nCarColors[0] = pVeh->m_nPrimaryColor;
            m_nCarColors[1] = pVeh->m_nSecondaryColor;
            m_nCarColors[2] = pVeh->m_nTertiaryColor;
            m_nCarColors[3] = pVeh->m_nQuaternaryColor;
        }

        // Resets applied material colors
        void ResetMatColor(RpMaterial* pMat);

        // Resets applied material textures
        void ResetMatTexture(RpMaterial* pMat);

        // Sets the material color to provided value
        void SetMatColor(RpMaterial* pMat, RpGeometry* pGeo, RwRGBA color);

        // Sets the material to provided texture
        void SetMatTexture(RpMaterial* pMat, RwTexture* pTex);
    };
    VehicleExtendedData<PaintData> m_VehPaint;

    friend class IFeature;
    PaintMgr();
    PaintMgr(PaintMgr&);

public:
    ResourceStore m_TextureData { "textures", eResourceType::TYPE_IMAGE_TEXT, ImVec2(100, 80) };

    // Returns internal data structure
    PaintData &GetData(CVehicle* pVeh);

    // Resets applied applied colors
    void ResetColor(CVehicle* pVeh);

    // Resets appllied textures
    void ResetTexture(CVehicle* pVeh);

    // Applies color to vehicle
    void SetColor(CVehicle* pVeh, CRGBA color);

    // Sets vehicle carcol colors
    void SetCarcols(CVehicle *pVeh, uint primary, uint secondary, uint tertiary, uint quaternary, bool reset = true);

    // Applies texture to vehicle
    void SetTexture(CVehicle* pVeh, std::string name);
};

extern PaintMgr& Paint;