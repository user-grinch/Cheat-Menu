#pragma once
#include "utils/resourcestore.h"
#include "interface/icheat.hpp"


/*
    Cutscene Player Class for SA
    Plays mission cutscenes
*/
class CutsceneMgr : public ICheat<CutsceneMgr>
{
private:
    bool m_bRunning;          // is cutscene currently running

    // backup data
    int m_nInterior;          // interior player was in
    CVehicle *m_pLastVeh;     // vehicle player was in
    int m_nVehSeat;           // seat id of player vehicle

    friend class IFeature;
    CutsceneMgr();
    CutsceneMgr(const CutsceneMgr&);

public:
    ResourceStore m_Data{ "cutscenes", eResourceType::TYPE_TEXT }; // names

    // Plays a cutscene
    void Play(std::string& cat, std::string& key, std::string& val);

    // Stops a running cutscene
    void Stop();
};

extern CutsceneMgr& Cutscene;