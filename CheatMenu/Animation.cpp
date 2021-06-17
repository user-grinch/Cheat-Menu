#include "pch.h"
#include "Animation.h"
#include "Ui.h"
#include "Util.h"

Animation::Animation()
{
	anim_data.json.LoadData(anim_data.categories, anim_data.selected);
	Cutscene::data.json.LoadData(Cutscene::data.categories, Cutscene::data.selected);

	Events::processScriptsEvent += []
	{
		if (Cutscene::running)
		{
			if (Command<Commands::HAS_CUTSCENE_FINISHED>())
			{
				Command<Commands::CLEAR_CUTSCENE>();
				Cutscene::running = false;
				Cutscene::scene_name = "";
				CPlayerPed* player = FindPlayerPed();
				player->m_nAreaCode = Cutscene::interior;
				Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
				Cutscene::interior = 0;
				TheCamera.Fade(0, 1);
			}
		}
		else
		{
			if (Cutscene::scene_name != "" && Command<Commands::HAS_CUTSCENE_LOADED>())
			{
				Command<Commands::START_CUTSCENE>();
				Cutscene::running = true;
			}
		}
	};
}

void Animation::PlayCutscene(std::string& rootkey, std::string& cutscene_str, std::string& interior)
{
	if (Util::IsOnCutscene())
	{
		CHud::SetHelpMessage("Another cutscene is running", false, false, false);
		return;
	}

	Cutscene::scene_name = cutscene_str;
	Command<Commands::LOAD_CUTSCENE>(cutscene_str.c_str());
	CPlayerPed *player =FindPlayerPed();
	Cutscene::interior = player->m_nAreaCode;
	player->m_nAreaCode = std::stoi(interior); 
	Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
}

void Animation::Draw()
{
	if (ImGui::BeginTabBar("Animation", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		ImGui::Spacing();
		int hplayer = CPools::GetPedRef(FindPlayerPed());

		ImGui::Spacing();

		if (ImGui::BeginTabItem("Anims"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Stop animation", Ui::GetSize()))
			{
				if (hplayer)
					Command<Commands::CLEAR_CHAR_TASKS>(hplayer);
			}

			ImGui::Spacing();

			ImGui::Columns(2, 0, false);
			ImGui::Checkbox("Loop", &loop);
			ImGui::NextColumn();
			ImGui::Checkbox("Secondary", &secondary);
			ImGui::Columns(1);
			ImGui::Spacing();

			if (ImGui::BeginChild("Anims Child"))
			{
				ImGui::Spacing();
				Ui::DrawJSON(anim_data.json, anim_data.categories, anim_data.selected, anim_data.filter, &PlayAnimation, &RemoveAnimation);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Misc"))
		{
			ImGui::Spacing();
			if (Ui::ListBox("Fighting", fighting_vec, fighting_selected))
			{
				Command<Commands::GIVE_MELEE_ATTACK_TO_CHAR>(hplayer, fighting_selected + 4, 6);
				CHud::SetHelpMessage("Fighting anim set", false, false, false);
			}
			if (Ui::ListBoxStr("Walking", walking_vec, walking_selected))
			{
				if (walking_selected == "default")
				{
					patch::Set<DWORD>(0x609A4E, 0x4D48689, false);
					patch::Set<WORD>(0x609A52, 0, false);
				}
				else
				{
					const char* cwalking_selected = walking_selected.c_str();

					patch::Set<DWORD>(0x609A4E, -0x6F6F6F70, false);
					patch::Nop(0x609A52, 2, false);

					Command<Commands::REQUEST_ANIMATION>(cwalking_selected);
					Command<Commands::LOAD_ALL_MODELS_NOW>();
					Command<Commands::SET_ANIM_GROUP_FOR_CHAR>(hplayer, cwalking_selected);
					Command<Commands::REMOVE_ANIMATION>(cwalking_selected);
				}
				CHud::SetHelpMessage("Walking anim set", false, false, false);
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Custom"))
		{
			ImGui::InputTextWithHint("IFP name", "ped", ifp_buffer, INPUT_BUFFER_SIZE);
			ImGui::InputTextWithHint("Anim name", "cower", anim_buffer, INPUT_BUFFER_SIZE);
			ImGui::Spacing();
			if (ImGui::Button("Add animation", Ui::GetSize()))
			{
				anim_data.json.data["Custom"][anim_buffer] = ("0, " + std::string(ifp_buffer));
				anim_data.json.WriteToDisk();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Cutscene"))
		{
			ImGui::Spacing();
			if (ImGui::Button("Stop cutscene", Ui::GetSize()))
			{
				if (Cutscene::running)
				{
					Command<Commands::CLEAR_CUTSCENE>();
					Cutscene::running = false;
					Cutscene::scene_name = "";
					CPlayerPed* player = FindPlayerPed();
					player->m_nAreaCode = Cutscene::interior;
					Command<Commands::SET_AREA_VISIBLE>(player->m_nAreaCode);
					Cutscene::interior = 0;
					TheCamera.Fade(0, 1);
				}
			}
			ImGui::Spacing();

			if (ImGui::BeginChild("Cutscene Child"))
			{
				ImGui::Spacing();
				Ui::DrawJSON(Cutscene::data.json, Cutscene::data.categories, Cutscene::data.selected, Cutscene::data.filter, &PlayCutscene, nullptr);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Animation::PlayAnimation(std::string& ifp, std::string& anim, std::string& ifp_repeat)
{
	int hplayer = CPools::GetPedRef(FindPlayerPed());

	if (ifp != "PED")
	{
		Command<Commands::REQUEST_ANIMATION>(ifp.c_str());
		Command<Commands::LOAD_ALL_MODELS_NOW>();
	}

	Command<Commands::CLEAR_CHAR_TASKS>(hplayer);
	if (secondary)
		Command<Commands::TASK_PLAY_ANIM_SECONDARY>(hplayer, anim.c_str(), ifp.c_str(), 4.0, loop, 0, 0, 0, -1);
	else
		Command<Commands::TASK_PLAY_ANIM>(hplayer, anim.c_str(), ifp.c_str(), 4.0, loop, 0, 0, 0, -1);

	if (ifp != "PED")
		Command<Commands::REMOVE_ANIMATION>(ifp.c_str());
}

void Animation::RemoveAnimation(std::string& ifp, std::string& anim, std::string& ifp_repeat)
{
	if (ifp == "Custom")
	{
		anim_data.json.data["Custom"].erase(anim);
		anim_data.json.WriteToDisk();
		CHud::SetHelpMessage("Animation removed", false, false, false);
	}	
	else 
		CHud::SetHelpMessage("You can only remove custom anims", false, false, false);

}

