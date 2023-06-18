#pragma once
#include "pch.h"
#include "interface/ipage.h"

class PlayerPage : public IPage<PlayerPage>
{
private:
    bool m_bGodMode;
    bool m_bPlayerRegen;
    struct
    {
        bool m_bEnabled = false;
        CVector m_fPos;
    } m_RespawnDieLoc;
    bool m_bKeepStuff;
    bool m_bFreezeWantedLevel;

#ifdef GTAVC
    bool m_bNoUndress;          // Tommy won't lose clothes after busted/ wasted
#endif

#ifdef GTASA
    bool m_bAimSkinChanger;
    bool m_bDrunkEffect;
    bool m_bFastSprint;
    ResourceStore m_ClothData { "clothes", eResourceType::TYPE_TEXT_IMAGE, ImVec2(70, 100)};

    void SetCloth(str& id);
    void RemoveClothesTab();
    void SetModel(str& id);
#else
    ResourceStore skinData { BY_GAME(NULL, "skins", "peds"), eResourceType::TYPE_TEXT };
    void ChangePlayerModel(str& cat, str& name, str& id);
#endif

    friend class IFeature;
    PlayerPage();
    PlayerPage(const PlayerPage&);

public:

    void Draw();
};

extern PlayerPage &playerPage;
