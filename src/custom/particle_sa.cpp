#include "pch.h"
#include "particle_sa.h"
#include "utils/widget.h"

ParticleMgr& Particle = *ParticleMgr::Get();
void ParticleMgr::Play(std::string& cat, std::string& name, std::string& particle)
{
    CPlayerPed* pPlayer = FindPlayerPed();
    if (pPlayer)
    {
        CVector pos = pPlayer->GetPosition();
        int handle;
        Command<Commands::CREATE_FX_SYSTEM>(particle.c_str(), pos.x, pos.y, pos.z, 1, &handle);
        Command<Commands::PLAY_FX_SYSTEM>(handle);
        m_nList.push_back(handle);
    }
}

void ParticleMgr::RemoveAll()
{
    for (int& p : m_nList)
    {
        Command<Commands::KILL_FX_SYSTEM>(p);
    }
    m_nList.clear();
}

void ParticleMgr::RemoveLatest()
{
    Command<Commands::KILL_FX_SYSTEM>(m_nList.back()); // stop if anything is running
    m_nList.pop_back();
}

void ParticleMgr::AddNew()
{
    static char name[INPUT_BUFFER_SIZE], particle[INPUT_BUFFER_SIZE];
    ImGui::InputTextWithHint(TEXT("Scene.ParticleName"), "KKJJ fire particle", name, INPUT_BUFFER_SIZE);
    ImGui::InputTextWithHint(TEXT("Scene.ParticleID"), "kkjj_on_fire", particle, INPUT_BUFFER_SIZE);
    ImGui::Spacing();
    if (ImGui::Button(TEXT("Window.AddEntry"), Widget::CalcSize()))
    {
        std::string key = std::string("Custom.") + name;
        Particle.m_Data.m_pData->Set(key.c_str(), std::string(particle));
        Particle.m_Data.m_pData->Save();
        Util::SetMessage(TEXT("Window.AddEntryMSG"));
    }
}