#include "pch.h"
#include "scene.h"
#include "utils/widget.h"
#include "utils/util.h"
#include "custom/animation.h"

#ifdef GTASA
#include "utils/overlay.h"
#include "custom/cutscene_sa.h"
#include "custom/particle_sa.h"
#endif

ScenePage& scenePage = ScenePage::Get();

void ScenePage::Draw()
{
    if (ImGui::BeginTabBar("Animation", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        ImGui::Spacing();
        CPlayerPed* pPlayer = FindPlayerPed();
        int hPlayer = CPools::GetPedRef(pPlayer);

        if (ImGui::BeginTabItem(TEXT( "Scene.AnimationTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Scene.StopAnimation"), Widget::CalcSize()))
            {
                Animation.StopImmediately(pPlayer);
            }
            ImGui::Spacing();

            ImGui::Columns(BY_GAME(3, 2, 2), nullptr, false);
            Widget::Toggle(TEXT("Scene.LoopCheckbox"), &Animation.m_Loop, TEXT("Scene.LoopCheckboxText"));
            ImGui::NextColumn();
            Widget::Toggle(TEXT("Scene.SecondaryCheckbox"), &Animation.m_bSecondary, TEXT("Scene.SecondaryCheckboxText"));
#ifdef GTASA
            ImGui::NextColumn();
            Widget::Toggle(TEXT("Scene.PedAnim"), &Animation.m_bPedAnim, TEXT("Scene.PedAnimText"));
#endif
            ImGui::Columns(1);
            ImGui::Spacing();

            if (Animation.IsTargetFound())
            {
                ImGui::TextWrapped(TEXT("Scene.NoTarget"));
            }
            else
            {
                ImGui::Spacing();

                if (ImGui::BeginChild("Anims Child"))
                {
                    ImGui::Spacing();
                    Widget::DataList(Animation.m_Data, fArg3Wrapper(Animation.Play), fArgNoneWrapper(Animation.AddNew));
                    ImGui::EndChild();
                }
            }
            ImGui::EndTabItem();
        }
#ifdef GTASA
        if (ImGui::BeginTabItem(TEXT( "Scene.Tasks")))
        {
            ImGui::Spacing();
            Widget::Toggle(TEXT("Menu.ShowPedTasks"), &Overlay::m_bPedTasks);
            ImGui::Spacing();
            if (pPlayer)
            {
                ImGui::BeginChild("TasksList");
                ImGui::Text(TEXT("Scene.PrimaryTasks"));
                ImGui::Separator();
                for (size_t i = 0; i != TASK_PRIMARY_MAX; ++i)
                {
                    CTask *pTask = pPlayer->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[i];
                    if (pTask)
                    {
                        const char *name = taskNames[pTask->GetId()];
                        if (ImGui::MenuItem(name))
                        {
                            ImGui::SetClipboardText(name);
                            Util::SetMessage(TEXT("Window.CopiedToClipboard"));
                        }
                    }
                }

                ImGui::Dummy(ImVec2(0, 25));

                ImGui::Text(TEXT("Scene.SecondaryTasks"));
                ImGui::Separator();
                for (size_t i = 0; i != TASK_SECONDARY_MAX; ++i)
                {
                    CTask *pTask = pPlayer->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[i];
                    if (pTask)
                    {
                        const char *name = taskNames[pTask->GetId()];
                        if (ImGui::MenuItem(name))
                        {
                            ImGui::SetClipboardText(name);
                            Util::SetMessage(TEXT("Window.CopiedToClipboard"));
                        }
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Scene.CutsceneTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Scene.StopCutscene"), Widget::CalcSize()))
            {
                Cutscene.Stop();
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("Cutscene Child"))
            {
                ImGui::Spacing();
                Widget::DataList(Cutscene.m_Data, fArg3Wrapper(Cutscene.Play));
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Scene.ParticleTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Scene.RemoveAll"), Widget::CalcSize(2)))
            {
                Particle.RemoveAll();
            }
            ImGui::SameLine();
            if (ImGui::Button(TEXT("Scene.RemoveLatest"), Widget::CalcSize(2)))
            {
                Particle.RemoveLatest();
            }
            ImGui::Spacing();
            if (Widget::ToggleBits(TEXT("Scene.InvisiblePlayer"), pPlayer->m_nPedFlags.bDontRender))
            {
                pPlayer->m_nPedFlags.bDontRender = (pPlayer->m_nPedFlags.bDontRender == 1) ? 0 : 1;
            }
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::BeginChild("Anims Child"))
            {
                ImGui::Spacing();
                Widget::DataList(Particle.m_Data, fArg3Wrapper(Particle.Play), fArgNoneWrapper(Particle.AddNew));
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Scene.Styles")))
        {
            ImGui::Spacing();

            static int fightStyle;
            static const char* fightStyles = "Default\0Boxing\0Kung Fu\0Kick Boxing\0Punch Kick\0";
            static std::string walkStyle = "default";
            static std::vector<std::string> walkStyles =
            {
                "default", "man", "shuffle", "oldman", "gang1", "gang2", "oldfatman",
                "fatman", "jogger", "drunkman", "blindman", "swat", "woman", "shopping", "busywoman",
                "sexywoman", "pro", "oldwoman", "fatwoman", "jogwoman", "oldfatwoman", "skate"
            };

            if (ImGui::Combo(TEXT("Scene.FightingStyle"), &fightStyle, fightStyles))
            {
                Command<Commands::GIVE_MELEE_ATTACK_TO_CHAR>(hPlayer, fightStyle + 4, 6);
                Util::SetMessage(TEXT("Scene.FightingStyleSet"));
            }
            if (Widget::ListBox(TEXT("Scene.WalkingStyle"), walkStyles, walkStyle))
            {
                if (walkStyle == "default")
                {
                    patch::Set<DWORD>(0x609A4E, 0x4D48689);
                    patch::Set<WORD>(0x609A52, 0);
                }
                else
                {
                    patch::Nop(0x609A4E, 6);
                    Command<Commands::REQUEST_ANIMATION>(walkStyle.c_str());
                    Command<Commands::LOAD_ALL_MODELS_NOW>();
                    Command<Commands::SET_ANIM_GROUP_FOR_CHAR>(hPlayer, walkStyle.c_str());
                    Command<Commands::REMOVE_ANIMATION>(walkStyle.c_str());
                }
                Util::SetMessage(TEXT("Scene.WalkingStyleSet"));
            }
            ImGui::EndTabItem();
        }
#endif
        ImGui::EndTabBar();
    }
}