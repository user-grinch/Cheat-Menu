#pragma once
#include "pch.h"

class Player
{
private:
    static inline bool m_bHealthRegen;
    static inline bool m_bGodMode;
    static inline bool m_bModloaderInstalled;
    struct m_KeepPosition
    {
        static inline bool m_bEnabled = false;
        static inline CVector m_fPos;
    };

#ifdef GTASA
    static inline bool m_bAimSkinChanger;
    static inline bool m_bDrunkEffect;
    static inline bool m_bFastSprint;
    static inline int m_nUiBodyState;
    static inline ResourceStore m_ClothData { "clothes", eResourceType::TYPE_IMAGE, ImVec2(70, 100)};
    struct m_CustomSkins
    {
        static inline std::string m_Path = paths::GetGameDirPathA() + std::string("\\modloader\\Custom Skins\\");;
        static inline ImGuiTextFilter m_Filter;
        static inline std::vector<std::string> m_List;
    };

    struct m_TopDownCamera
    {
        static inline bool m_bEnabled = false;
        static inline float m_fOffset = 40.0f;
    };

    static inline const char* clothNameList[18] =
    {
        "Shirts", "Heads", "Trousers", "Shoes", "Tattoos left lower arm", "Tattoos left upper arm",
        "Tattoos right upper arm", "Tattoos right lower arm", "Tattoos back", "Tattoos left chest",
        "Tattoos right chest", "Tattoos stomach", "Tattoos lower back", "Necklaces", "Watches",
        "Glasses", "Hats", "Extras"
    };
#else
    static inline ResourceStore skinData { BY_GAME(NULL, "skin", "ped"), eResourceType::TYPE_TEXT };
#endif

#ifdef GTASA
    static void ChangePlayerModel(std::string& model);
    static void ChangePlayerCloth(std::string& model);
    static void TopDownCameraView();
#else
    static void ChangePlayerModel(std::string& cat, std::string& name, std::string& id);
#endif

public:
    Player();
    static void ShowPage();
};
