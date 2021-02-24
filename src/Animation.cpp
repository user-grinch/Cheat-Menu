#include "pch.h"
#include "Animation.h"
#include "Ui.h"
#include "Util.h"

bool Animation::loop = false;
bool Animation::secondary = false;
char Animation::ifp_buffer[INPUT_BUFFER_SIZE] = "";
char Animation::anim_buffer[INPUT_BUFFER_SIZE] = "";

ImGuiTextFilter Animation::filter = "";
std::vector<std::string> Animation::search_categories;
std::string Animation::selected_item = "All";

CJson Animation::json = CJson("animation");

std::vector<std::string> fighting_vec{ "Default","Boxing","Kung fu","Kick Boxing","Punch Kick" };
int fighting_selected = 0;

std::vector<std::string> walking_vec{ "default", "man", "shuffle", "oldman", "gang1", "gang2",
"oldfatman", "fatman", "jogger", "drunkman", "blindman", "swat", "woman", "shopping", "busywoman",
"sexywoman", "pro", "oldwoman", "fatwoman", "jogwoman", "oldfatwoman", "skate" };

std::string walking_selected = "default";

Animation::Animation()
{
	json.LoadData(search_categories, selected_item);
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
				Ui::DrawJSON(json, search_categories, selected_item, filter, &PlayAnimation, &RemoveAnimation);
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
				json.data["Custom"][anim_buffer] = ("0, " + std::string(ifp_buffer));
				json.WriteToDisk();
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
	flog << ifp << std::endl;
	if (ifp == "Custom")
	{
		json.data["Custom"].erase(anim);
		json.WriteToDisk();
		CHud::SetHelpMessage("Animation removed", false, false, false);
	}	
else CHud::SetHelpMessage("You can only remove custom anims", false, false, false);

}

