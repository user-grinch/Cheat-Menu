#pragma once
#include "utils/resourcestore.h"
#include "interface/icheat.hpp"

/*
    Animation Player Class
    Plays animatins on player & peds
*/
class AnimationMgr : public ICheat<AnimationMgr>
{
private:
    CPed *m_pTarget = nullptr;    // target ped pointer

    friend class ICheat;
    AnimationMgr();
    AnimationMgr(const AnimationMgr&);

public:
    bool m_Loop;                  // keep playing animation in a loop
    bool m_bSecondary;            // play animation alongside another
    bool m_bPedAnim;              // play the animation on a target ped

    ResourceStore m_Data{ "animations", eResourceType::TYPE_TEXT };

    // Add a custom animation entry
    void AddNew();

    // Retuns true if a ped target is found
    bool IsTargetFound();

    // Play the animation
    void Play(std::string& cat, std::string& key, std::string& val);

    // Stops whatever action char is doing
    void StopImmediately(CPed *ped);
};

extern AnimationMgr& Animation;