#pragma once
#include "pch.h"

class Animation
{
private:
    // Animation player
    static inline ResourceStore m_AnimData{ "animations", eResourceType::TYPE_TEXT };
    static inline char m_nAnimBuffer[INPUT_BUFFER_SIZE];
    static inline char m_nIfpBuffer[INPUT_BUFFER_SIZE];
    static inline bool m_Loop; // loop animation
    static inline bool m_bSecondary; // play animation as secondary
    static inline bool m_PedAnim;
    static inline CPed *m_pTarget = nullptr;

#ifdef GTASA
    // Cutscene player
    struct m_Cutscene
    {
        static inline ResourceStore m_Data{ "cutscenes", eResourceType::TYPE_TEXT };
        static inline std::string m_SceneName;
        static inline int m_nInterior; // player interior backup
        static inline bool m_bRunning;
    };

    static inline int m_nFightingStyle;
    static inline std::string m_nWalkingStyle = "default";
    static inline std::vector<std::string> m_FightingStyleList =
    { "Default", "Boxing", "Kung fu", "Kick Boxing", "Punch Kick" };
    static inline std::vector<std::string> m_WalkingStyleList =
    {
        "default", "man", "shuffle", "oldman", "gang1", "gang2", "oldfatman",
        "fatman", "jogger", "drunkman", "blindman", "swat", "woman", "shopping", "busywoman",
        "sexywoman", "pro", "oldwoman", "fatwoman", "jogwoman", "oldfatwoman", "skate"
    };

    // Particle player
    struct m_Particle
    {
        static inline ResourceStore m_Data{ "particles", eResourceType::TYPE_TEXT };
        static inline char m_NameBuffer[INPUT_BUFFER_SIZE];
        static inline std::vector<int> m_nParticleList;
    };
#endif

    static void PlayAnimation(std::string& rootKey, std::string& anim, std::string& ifp);
    static void RemoveAnimation(std::string& rootKey, std::string& anim, std::string& ifp);
    static void RemoveParticle(std::string& ifp, std::string& anim, std::string& ifpRepeat);
#ifdef GTASA
    static void PlayParticle(std::string& rootKey, std::string& particle, std::string& dummy);
    static void PlayCutscene(std::string& rootKey, std::string& cutsceneId, std::string& interior);
#elif GTAVC
    static bool _LoadAnimationBlock(const char* szBlockName);
#endif
#ifndef GTASA
    static void _PlayAnimation(RpClump* pClump, int animGroup, int animID, float blend);
#endif

public:
    Animation() = delete;
    Animation(const Animation&) = delete;

    static void Init();
    static void ShowPage();
};