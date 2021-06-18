#include "pch.h"
#include "Animation.h"
#include "Ui.h"
#include "Util.h"

CAnimation::CAnimation()
{
	m_AnimData.m_Json.LoadData(m_AnimData.m_Categories, m_AnimData.m_Selected);
	m_Cutscene.m_Data.m_Json.LoadData(m_Cutscene.m_Data.m_Categories, m_Cutscene.m_Data.m_Selected);

	Events::processScriptsEvent += [this]
	{
		if (m_Cutscene.m_bRunning)
		{
			if (Command<Commands::HAS_CUTSCENE_FINISHED>())
			{
				Command<Commands::CLEAR_CUTSCENE>();
				m_Cutscene.m_bRunning = false;
				m_Cutscene.m_SceneName = "";
				CPlayerPed* player = FindPlayerPed();
				player->m_nAreaCode = m_Cutscene.m_nInterior;
				Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
				m_Cutscene.m_nInterior = 0;
				TheCamera.Fade(0, 1);
			}
		}
		else
		{
			if (m_Cutscene.m_SceneName != "" && Command<Commands::HAS_CUTSCENE_LOADED>())
			{
				Command<Commands::START_CUTSCENE>();
				m_Cutscene.m_bRunning = true;
			}
		}
	};
}

void CAnimation::PlayCutscene(std::string& rootKey, std::string& cutsceneId, std::string& interior)
{
	if (Util::IsOnCutscene())
	{
		CHud::SetHelpMessage("Another cutscene is running", false, false, false);
		return;
	}

	m_Cutscene.m_SceneName = cutsceneId;
	Command<Commands::LOAD_CUTSCENE>(cutsceneId.c_str());
	CPlayerPed* pPlayer = FindPlayerPed();
	m_Cutscene.m_nInterior = pPlayer->m_nAreaCode;
	pPlayer->m_nAreaCode = std::stoi(interior);
	Command<Commands::SET_AREA_VISIBLE>(pPlayer->m_nAreaCode);
}

void CAnimation::Draw()
{
	if (ImGui::BeginTabBar("Animation", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		ImGui::Spacing();
		int hPlayer = CPools::GetPedRef(FindPlayerPed());

		ImGui::Spacing();

		if (ImGui::BeginTabItem("Anims"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Stop animation", Ui::GetSize()))
			{
				if (hPlayer)
					Command<Commands::CLEAR_CHAR_TASKS>(hPlayer);
			}

			ImGui::Spacing();

			ImGui::Columns(2, nullptr, false);
			ImGui::Checkbox("Loop", &m_Loop);
			ImGui::NextColumn();
			ImGui::Checkbox("Secondary", &m_bSecondary);
			ImGui::Columns(1);
			ImGui::Spacing();

			if (ImGui::BeginChild("Anims Child"))
			{
				ImGui::Spacing();
				Ui::DrawJSON(m_AnimData.m_Json, m_AnimData.m_Categories, m_AnimData.m_Selected, m_AnimData.m_Filter, &PlayAnimation,
				             &RemoveAnimation);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Misc"))
		{
			ImGui::Spacing();
			if (Ui::ListBox("Fighting", m_FightingStyleList, m_nFightingStyle))
			{
				Command<Commands::GIVE_MELEE_ATTACK_TO_CHAR>(hPlayer, m_nFightingStyle + 4, 6);
				CHud::SetHelpMessage("Fighting anim set", false, false, false);
			}
			if (Ui::ListBoxStr("Walking", m_WalkingStyleList, m_nWalkingStyle))
			{
				if (m_nWalkingStyle == "default")
				{
					patch::Set<DWORD>(0x609A4E, 0x4D48689, false);
					patch::Set<WORD>(0x609A52, 0, false);
				}
				else
				{
					patch::Set<DWORD>(0x609A4E, -0x6F6F6F70, false);
					patch::Nop(0x609A52, 2, false);

					Command<Commands::REQUEST_ANIMATION>(m_nWalkingStyle.c_str());
					Command<Commands::LOAD_ALL_MODELS_NOW>();
					Command<Commands::SET_ANIM_GROUP_FOR_CHAR>(hPlayer, m_nWalkingStyle.c_str());
					Command<Commands::REMOVE_ANIMATION>(m_nWalkingStyle.c_str());
				}
				CHud::SetHelpMessage("Walking anim set", false, false, false);
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
				m_AnimData.m_Json.m_Data["Custom"][m_nAnimBuffer] = ("0, " + std::string(m_nIfpBuffer));
				m_AnimData.m_Json.WriteToDisk();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Cutscene"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Stop cutscene", Ui::GetSize()))
			{
				if (m_Cutscene.m_bRunning)
				{
					Command<Commands::CLEAR_CUTSCENE>();
					m_Cutscene.m_bRunning = false;
					m_Cutscene.m_SceneName = "";
					CPlayerPed* player = FindPlayerPed();
					player->m_nAreaCode = m_Cutscene.m_nInterior;
					Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
					m_Cutscene.m_nInterior = 0;
					TheCamera.Fade(0, 1);
				}
			}
			ImGui::Spacing();

			if (ImGui::BeginChild("Cutscene Child"))
			{
				ImGui::Spacing();
				Ui::DrawJSON(m_Cutscene.m_Data.m_Json, m_Cutscene.m_Data.m_Categories, m_Cutscene.m_Data.m_Selected,
				             m_Cutscene.m_Data.m_Filter, &PlayCutscene, nullptr);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void CAnimation::PlayAnimation(std::string& ifp, std::string& anim, std::string& ifpRepeat)
{
	int hplayer = CPools::GetPedRef(FindPlayerPed());

	if (ifp != "PED")
	{
		Command<Commands::REQUEST_ANIMATION>(ifp.c_str());
		Command<Commands::LOAD_ALL_MODELS_NOW>();
	}

	Command<Commands::CLEAR_CHAR_TASKS>(hplayer);
	if (m_bSecondary)
		Command<Commands::TASK_PLAY_ANIM_SECONDARY>(hplayer, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);
	else
		Command<Commands::TASK_PLAY_ANIM>(hplayer, anim.c_str(), ifp.c_str(), 4.0, m_Loop, 0, 0, 0, -1);

	if (ifp != "PED")
		Command<Commands::REMOVE_ANIMATION>(ifp.c_str());
}

void CAnimation::RemoveAnimation(std::string& ifp, std::string& anim, std::string& ifpRepeat)
{
	if (ifp == "Custom")
	{
		m_AnimData.m_Json.m_Data["Custom"].erase(anim);
		m_AnimData.m_Json.WriteToDisk();
		CHud::SetHelpMessage("Animation removed", false, false, false);
	}
	else
		CHud::SetHelpMessage("You can only remove custom anims", false, false, false);
}
