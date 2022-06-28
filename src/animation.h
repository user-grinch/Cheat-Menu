#pragma once
#include "pch.h"

#ifdef GTASA
class Cutscene
{
public:
    static inline ResourceStore m_Data{ "cutscenes", eResourceType::TYPE_TEXT };
    static inline std::string m_SceneName;
    static inline int m_nInterior; // player interior backup
    static inline bool m_bRunning;

    static void Play(std::string& rootKey, std::string& cutsceneId, std::string& interior);
};

class Particle
{
public:
    static inline ResourceStore m_Data{ "particles", eResourceType::TYPE_TEXT };
    static inline std::vector<int> m_nParticleList;

    static void Play(std::string& rootKey, std::string& particle, std::string& dummy);
    static void Remove(std::string& ifp, std::string& anim, std::string& ifpRepeat);
};
#endif

class Animation
{
private:
    static inline ResourceStore m_AnimData{ "animations", eResourceType::TYPE_TEXT };
    static inline bool m_Loop; // loop animation
    static inline bool m_bSecondary; // play animation as secondary
    static inline bool m_PedAnim;
    static inline CPed *m_pTarget = nullptr;

    static void Play(std::string& rootKey, std::string& anim, std::string& ifp);
    static void Remove(std::string& rootKey, std::string& anim, std::string& ifp);

public:
    Animation() = delete;
    Animation(const Animation&) = delete;

    static void Init();
    static void ShowPage();
};