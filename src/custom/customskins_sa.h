#pragma once
#include "utils/resourcestore.h"
#include "interface/icheat.hpp"


/*
    CustomSkins Class for SA
    Adds ability to use skins without replacing
    Requires modloader installed

    There are certain limitations,
        1. Skin names can't be longer than 8 chars
        2. Skins can't be removed while the game is running
*/
class CustomSkinsMgr : public ICheat<CustomSkinsMgr> {
  private:
    ImGuiTextFilter m_Filter;
    std::vector<std::string> m_List;
    bool m_bInit;

    friend class IFeature;
    CustomSkinsMgr();
    CustomSkinsMgr(const CustomSkinsMgr&);

    // Change player to custom skin
    void SetSkin(const std::string& name);
  public:

    // Draw custom skins handler code
    void Draw();
};

extern CustomSkinsMgr& CustomSkins;