#pragma once
class Animation
{
private:
	static bool loop;
	static bool secondary;

	static CJson json;
	static ImGuiTextFilter filter;
	static std::vector<std::string> search_categories;
	static std::string selected_item;

	static char ifp_buffer[INPUT_BUFFER_SIZE];
	static char anim_buffer[INPUT_BUFFER_SIZE];

protected:
	Animation();
	~Animation() {};
public:
	static void Main();
	static void PlayAnimation(std::string& rootkey, std::string& anim, std::string& ifp);

	static void RemoveEntry(std::string& rootkey, std::string& anim, std::string& ifp);
};

