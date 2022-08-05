#pragma once
#include "pch.h"

#ifdef GTASA
/*
    Cutscene Player Class for SA
    Plays mission cutscenes
*/
class Cutscene
{
private:
    static inline bool m_bRunning;          // is cutscene currently running

    // backup data
    static inline int m_nInterior;          // interior player was in 
    static inline CVehicle *m_pLastVeh;     // vehicle player was in
    static inline int m_nVehSeat;           // seat id of player vehicle

public:
    static inline ResourceStore m_Data{ "cutscenes", eResourceType::TYPE_TEXT }; // names

    Cutscene() = delete;
    Cutscene(const Cutscene&) = delete;

    // Initializes hooks, vars etc.
    static void Init();

    // Plays a cutscene 
    static void Play(std::string& cat, std::string& key, std::string& val);

    // Stops a running cutscene
    static void Stop();
};

/*
    Particle Player Class for SA
    Spawns particles in the world
*/
class Particle
{
private:
    static inline std::vector<int> m_nList;

public:
    static inline ResourceStore m_Data{ "particles", eResourceType::TYPE_TEXT }; // names

    Particle() = delete;
    Particle(const Particle&) = delete;

    // Plays a particle
    static void Play(std::string& cat, std::string& key, std::string& val);

    // Removes all spawned particles
    static void RemoveAll();

    // Removes the most recent spawned particle
    static void RemoveLatest();
};
#endif

/*
    Animation Player Class
    Plays animatins on player & peds
*/
class Animation
{
private:
    static inline ResourceStore m_AnimData{ "animations", eResourceType::TYPE_TEXT };
    static inline bool m_Loop;                  // keep playing animation in a loop
    static inline bool m_bSecondary;            // play animation alongside another
    static inline bool m_bPedAnim;              // play the animation on a target ped
    static inline CPed *m_pTarget = nullptr;    // target ped pointer

    // Plays a particular animation
    static void Play(std::string& cat, std::string& key, std::string& val);

public:
    Animation() = delete;
    Animation(const Animation&) = delete;

    // Initializes hooks & stuff 
    static void Init();

    // Displays animation menu page
    static void ShowPage();
};