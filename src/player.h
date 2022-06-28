#pragma once
#include "pch.h"

class Player
{
private:
    static inline bool m_bHealthRegen;
    static inline bool m_bGodMode;
    static inline bool m_bModloaderInstalled;
    struct KeepPosition
    {
        static inline bool m_bEnabled = false;
        static inline CVector m_fPos;
    };
    static inline bool m_bFreezeWantedLevel;

#ifdef GTASA
    static inline bool m_bAimSkinChanger;
    static inline bool m_bDrunkEffect;
    static inline bool m_bFastSprint;
    static inline ResourceStore m_ClothData { "clothes", eResourceType::TYPE_IMAGE, ImVec2(70, 100)};
    struct CustomSkins
    {
        static inline ImGuiTextFilter m_Filter;
        static inline std::vector<std::string> m_List;
    };

    struct TopDownCamera
    {
    public:
        static inline bool m_bEnabled = false;
        static inline float m_fOffset = 40.0f;

        static void Process();
    };

    static void ChangePlayerModel(std::string& model);
    static void ChangePlayerCloth(std::string& model);
#else
    static inline ResourceStore skinData { BY_GAME(NULL, "skins", "peds"), eResourceType::TYPE_TEXT };
    static void ChangePlayerModel(std::string& cat, std::string& name, std::string& id);
#endif

public:
    Player() = delete;
    Player(const Player&) = delete;

    static void Init();
    static void ShowPage();
};
