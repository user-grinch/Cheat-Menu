#include "animation.h"

#include "pch.h"

#ifdef GTASA
#include "ui.h"
#include "util.h"

Animation::Animation()
{
    Events::processScriptsEvent += [this] {
        if (m_Cutscene::m_bRunning)
        {
            if (Command<Commands::HAS_CUTSCENE_FINISHED>())
            {
                CPlayerPed *pPlayer = FindPlayerPed();
                if (!pPlayer)
                {
                    return;
                }

                pPlayer->m_nAreaCode = m_Cutscene::m_nInterior;
                Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
                m_Cutscene::m_nInterior = 0;
                TheCamera.Fade(0, 1);
            }
        }
        else
        {
            if (m_Cutscene::m_SceneName != "" && Command<Commands::HAS_CUTSCENE_LOADED>())
            {
                Command<Commands::START_CUTSCENE>();
                m_Cutscene::m_bRunning = true;
            }
        }
    };
}

void Animation::PlayCutscene(std::string &rootKey, std::string &cutsceneId, std::string &interior)
{
    if (Util::IsOnCutscene())
    {
        SetHelpMessage("Another cutscene is running", false, false, false);
        return;
    }

    CPlayerPed *pPlayer = FindPlayerPed();
    if (!pPlayer)
    {
        return;
    }

    m_Cutscene::m_SceneName = cutsceneId;
    Command<Commands::LOAD_CUTSCENE>(cutsceneId.c_str());
    m_Cutscene::m_nInterior = pPlayer->m_nAreaCode;
    pPlayer->m_nAreaCode = std::stoi(interior);
    Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
}

void Animation::Draw()
{
    if (ImGui::BeginTabBar("Animation", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        ImGui::Spacing();
        CPlayerPed *pPlayer = FindPlayerPed();
        int hPlayer = CPools::GetPedRef(pPlayer);

        ImGui::Spacing();

        if (ImGui::BeginTabItem("Anims"))
        {
            ImGui::Spacing();
            if (ImGui::Button("Stop animation", Ui::GetSize()))
            {
                if (hPlayer)
                {
                    Command<Commands::CLEAR_CHAR_TASKS>(hPlayer);
                }
            }

            ImGui::Spacing();

            ImGui::Columns(2, nullptr, false);
            ImGui::Checkbox("Loop", &m_Loop);
            Ui::ShowTooltip("Keep playing the animation on repeat");
            ImGui::NextColumn();
            ImGui::Checkbox("Secondary", &m_bSecondary);
            Ui::ShowTooltip("Player can move while playing the animation");
            ImGui::Columns(1);
            ImGui::Spacing();

            if (ImGui::BeginChild("Anims Child"))
            {
                ImGui::Spacing();
                Ui::DrawJSON(m_AnimData, PlayAnimation, RemoveAnimation);
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Misc"))
        {
            ImGui::Spacing();
            if (Ui::ListBox("Fighting style", m_FightingStyleList, m_nFightingStyle))
            {
                Command<Commands::GIVE_MELEE_ATTACK_TO_CHAR>(hPlayer, m_nFightingStyle + 4, 6);
                SetHelpMessage("Fighting anim set", false, false, false);
            }
            if (Ui::ListBoxStr("Walking style", m_WalkingStyleList, m_nWalkingStyle))
            {
                if (m_nWalkingStyle == "default")
                {
                    patch::Set<DWORD>(0x609A4E, 0x4D48689);
                    patch::Set<WORD>(0x609A52, 0);
                }
                else
                {
                    patch::Nop(0x609A4E, 6);
                    Command<Commands::REQUEST_ANIMATION>(m_nWalkingStyle.c_str());
                    Command<Commands::LOAD_ALL_MODELS_NOW>();
                    Command<Commands::SET_ANIM_GROUP_FOR_CHAR>(hPlayer, m_nWalkingStyle.c_str());
                    Command<Commands::REMOVE_ANIMATION>(m_nWalkingStyle.c_str());
                }
                SetHelpMessage("Walking anim set", false, false, false);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Custom"))
        {
            ImGui::InputTextWithHint("IFP name", "ped", m_nIfpBuffer, INPUT_BUFFER_SIZE);
            ImGui::InputTextWithHint("Anim name", "cower", m_nAnimBuffer, INPUT_BUFFER_SIZE);
            ImGui::Spacing();
            if (ImGui::Button("Add animation", Ui::GetSize()))
            {
                m_AnimData.m_pJson->m_Data["Custom"][m_nAnimBuffer] = ("0, " + std::string(m_nIfpBuffer));
                m_AnimData.m_pJson->WriteToDisk();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Cutscene"))
        {
            ImGui::Spacing();
            if (ImGui::Button("Stop cutscene", Ui::GetSize()))
            {
                if (m_Cutscene::m_bRunning)
                {
                    Command<Commands::CLEAR_CUTSCENE>();
                    m_Cutscene::m_bRunning = false;
                    m_Cutscene::m_SceneName = "";
                    CPlayerPed *player = FindPlayerPed();
                    player->m_nAreaCode = m_Cutscene::m_nInterior;
                    Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
                    m_Cutscene::m_nInterior = 0;
                    TheCamera.Fade(0, 1);
                }
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("Cutscene Child"))
            {
                ImGui::Spacing();
                Ui::DrawJSON(m_Cutscene::m_Data, PlayCutscene, nullptr);
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void Animation::PlayAnimation(std::string &ifp, std::string &anim, std::string &ifpRepeat)
{
    int hplayer = CPools::GetPedRef(FindPlayerPed());

    if (ifp != "PED")
    {
        Command<Commands::REQUEST_ANIMATION>(ifp.c_str());
        Command<Commands::LOAD_ALL_MODELS_NOW>();
    }

    Command<Commands::CLEAR_CHAR_TASKS>(hplayer);
    if (m_bSecondary)
    {
        Command<Commands::TASK_PLAY_ANIM_SECONDARY>(hplayer, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
    }
    else
    {
        Command<Commands::TASK_PLAY_ANIM>(hplayer, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
    }

    if (ifp != "PED")
    {
        Command<Commands::REMOVE_ANIMATION>(ifp.c_str());
    }
}

void Animation::RemoveAnimation(std::string &ifp, std::string &anim, std::string &ifpRepeat)
{
    if (ifp == "Custom")
    {
        m_AnimData.m_pJson->m_Data["Custom"].erase(anim);
        m_AnimData.m_pJson->WriteToDisk();
        SetHelpMessage("Animation removed", false, false, false);
    }
    else
    {
        SetHelpMessage("You can only remove custom anims", false, false, false);
    }
}
#endif