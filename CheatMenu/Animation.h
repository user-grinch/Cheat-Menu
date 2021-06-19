#pragma once
class Animation
{
private:
	inline static char m_nAnimBuffer[INPUT_BUFFER_SIZE];
	inline static SSearchData m_AnimData{ "animation" };
	struct m_Cutscene
	{
		inline static SSearchData m_Data{ "cutscene" };
		inline static std::string m_SceneName;
		inline static int m_nInterior;
		inline static bool m_bRunning;
	};
	inline static int m_nFightingStyle;
	inline static char m_nIfpBuffer[INPUT_BUFFER_SIZE];
	inline static bool m_Loop;
	inline static bool m_bSecondary;
	inline static std::string m_nWalkingStyle = "default";
	inline static std::vector<std::string> m_FightingStyleList = {"Default", "Boxing", "Kung fu", "Kick Boxing", "Punch Kick"};
	inline static std::vector<std::string> m_WalkingStyleList = 
	{
		"default", "man", "shuffle", "oldman", "gang1", "gang2", "oldfatman",
		"fatman", "jogger", "drunkman", "blindman", "swat", "woman", "shopping", "busywoman",
		"sexywoman", "pro", "oldwoman", "fatwoman", "jogwoman", "oldfatwoman", "skate"
	};

protected:
	Animation();

public:
	static void Draw();
	static void PlayAnimation(std::string& rootKey, std::string& anim, std::string& ifp);
	static void PlayCutscene(std::string& rootKey, std::string& cutsceneId, std::string& interior);
	static void RemoveAnimation(std::string& rootKey, std::string& anim, std::string& ifp);
};
