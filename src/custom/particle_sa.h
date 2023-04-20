#pragma once
#include "utils/resourcestore.h"
#include "interface/icheat.hpp"

/*
    Particle Player Class for SA
    Spawns particles in the world
*/
class ParticleMgr : public ICheat<ParticleMgr>
{
private:
    std::vector<int> m_nList;

    friend class IFeature;
    ParticleMgr() {};
    ParticleMgr(const ParticleMgr&);

public:
    ResourceStore m_Data{ "particles", eResourceType::TYPE_TEXT }; // names

    // Plays a particle
    void Play(std::string& cat, std::string& key, std::string& val);

    // Adds new particle to data
    void AddNew();

    // Removes all spawned particles
    void RemoveAll();

    // Removes the most recent spawned particle
    void RemoveLatest();
};

extern ParticleMgr& Particle;