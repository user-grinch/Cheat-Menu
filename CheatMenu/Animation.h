#pragma once
class Animation
{
private:
	inline static bool loop = false;
	inline static bool secondary = false;

	inline static SearchData anim_data{"animation"};

	struct Cutscene
	{
		inline static SearchData data{ "cutscene" };
		inline static std::string scene_name = "";
		inline static int interior = 0;
		inline static bool running = false;
	};
	
	inline static char anim_buffer[INPUT_BUFFER_SIZE] = "";
	inline static char ifp_buffer[INPUT_BUFFER_SIZE] = "";

	inline static std::vector<std::string> fighting_vec{ "Default","Boxing","Kung fu","Kick Boxing","Punch Kick" };
	inline static int fighting_selected = 0;
	inline static std::vector<std::string> walking_vec{ "default", "man", "shuffle", "oldman", "gang1", "gang2",
	"oldfatman", "fatman", "jogger", "drunkman", "blindman", "swat", "woman", "shopping", "busywoman",
	"sexywoman", "pro", "oldwoman", "fatwoman", "jogwoman", "oldfatwoman", "skate" };
	inline static std::string walking_selected = "default";

protected:
	Animation();

public:
	static void Draw();
	static void PlayAnimation(std::string& rootkey, std::string& anim, std::string& ifp);
	static void PlayCutscene(std::string& rootkey, std::string& cutscene_str, std::string& interior);
	static void RemoveAnimation(std::string& rootkey, std::string& anim, std::string& ifp);
};

