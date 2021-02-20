#include "Hook.h"
#include "kiero/kiero.h"
#include "kiero/minhook/MinHook.h"
// #include "vulkan/vulkan.h"

WNDPROC Hook::oWndProc = NULL;
f_Present11 Hook::oPresent11 = NULL;
f_Present9 Hook::oPresent9 = NULL;
f_Reset Hook::oReset9 = NULL; 

bool Hook::mouse_visibility = false;
bool Hook::show_mouse = false;

std::function<void()> Hook::window_callback = NULL;

LRESULT Hook::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (Hook::show_mouse)
	{
		patch::Nop(0x4EB9F4, 5); //  disable radio scroll
		CPad::ClearMouseHistory();

		if (uMsg == WM_MOUSEWHEEL)
			return 1;
	}
	else
		patch::SetRaw(0x4EB9F4, (void*)"\xE8\x67\xFC\xFF\xFF", 5); // enable radio scroll

	if (ImGui::GetIO().WantTextInput)
	{
		Call<0x53F1E0>(); // CPad::ClearKeyboardHistory
	 	return 1;
	}
	
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT Hook::Reset(IDirect3DDevice9 * pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	return oReset9(pDevice, pPresentationParameters);
}

void Hook::Present(void *ptr)
{
	ImGuiIO& io = ImGui::GetIO();

	if (Globals::init_done)
	{
		Hook::ShowMouse(show_mouse);

		// handle window scaling here
		ImVec2 size(screen::GetScreenWidth(),screen::GetScreenHeight());
		if (Globals::screen_size.x != size.x && Globals::screen_size.y != size.y)
		{
			int font_size = int(size.y / 54.85f); // manually tested

			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", font_size);
			io.Fonts->Build();

			if (Globals::renderer == Render_DirectX9)
				ImGui_ImplDX9_InvalidateDeviceObjects();
			else
				ImGui_ImplDX11_InvalidateDeviceObjects();

			if (Globals::screen_size.x != -1 && Globals::screen_size.y != -1)
			{
				Globals::menu_size.x += (size.x-Globals::screen_size.x) / 4.0f;
				Globals::menu_size.y += (size.y-Globals::screen_size.y) / 1.2f;
			}

			ImGuiStyle* style = &ImGui::GetStyle();
			float scale_x = size.x / 1366.0f;
			float scale_y = size.y / 768.0f;

			style->FramePadding = ImVec2(5 * scale_x, 3 * scale_y);
			style->ItemSpacing = ImVec2(8 * scale_x, 4 * scale_y);
			style->ScrollbarSize = 12 * scale_x;
			style->IndentSpacing = 20 * scale_x;
			style->ItemInnerSpacing  = ImVec2(4 * scale_x,4 * scale_y);

			Globals::screen_size = size;
		}

		ImGui_ImplWin32_NewFrame();
		if (Globals::renderer == Render_DirectX9)
			ImGui_ImplDX9_NewFrame();
		else
			ImGui_ImplDX11_NewFrame();

		ImGui::NewFrame();

		if (window_callback != NULL)
			window_callback();

		ImGui::EndFrame();
		ImGui::Render();

		if (Globals::renderer == Render_DirectX9)
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		else
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	else
	{
		Globals::init_done = true;
		ImGui::CreateContext();

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui_ImplWin32_Init(RsGlobal.ps->window);
		
		// shift trigger fix
		patch::Nop(0x00531155,5);

		if (Globals::renderer == Render_DirectX9)
		{
			Globals::device = ptr;
			ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(Globals::device));
		}
		else
		{
			// for dx11 device ptr is swapchain
			reinterpret_cast<IDXGISwapChain*>(ptr)->GetDevice(__uuidof(ID3D11Device), (void**)&Globals::device);
			ID3D11DeviceContext* context;
			reinterpret_cast<ID3D11Device*>(Globals::device)->GetImmediateContext(&context);

			ImGui_ImplDX11_Init(reinterpret_cast<ID3D11Device*>(Globals::device), context);

			//ImGui_ImplVulkan_Init()
		}

		ImGui_ImplWin32_EnableDpiAwareness();

		io.IniFilename = NULL;
		io.LogFilename = NULL;

		style.WindowTitleAlign = ImVec2(0.5, 0.5);
		oWndProc = (WNDPROC)SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LRESULT)WndProc);
	}
}

HRESULT Hook::PresentDx9Handler(IDirect3DDevice9 *pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	Present(pDevice);
	return oPresent9(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);;
}

HRESULT Hook::PresentDx11Handler(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	Present(pSwapChain);
	return oPresent11(pSwapChain, SyncInterval, Flags);
}

// typedef void(*func_vkCmdDrawIndexed_t) (VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
// func_vkCmdDrawIndexed_t ovkCmdDrawIndexed;

// void hvkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
// {

// 	ovkCmdDrawIndexed(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
// }

// Thanks imring
void Hook::ShowMouse(bool state)
{
	if (state)
	{
		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
		patch::SetUChar(0x6194A0, 0xC3);

		// Since Windowed mode by ThirteenAG hooks this too 
		// patch::Nop(0x53F417, 5); // don't call CPad__getMouseState
		patch::SetUChar(0x746ED0, 0xC3);

		patch::SetRaw(0x53F41F, (void*)"\x33\xC0\x0F\x84", 4); // disable camera mouse movement
	}
	else
	{
		if (mouse_visibility != show_mouse)
		{
			patch::SetRaw(0x541DF5, (void*)"\xE8\x46\xF3\xFE\xFF", 5); // call CControllerConfigManager::AffectPadFromKeyBoard
			patch::SetUChar(0x746ED0, 0xA1);
			patch::SetRaw(0x53F41F, (void*)"\x85\xC0\x0F\x8C", 4); // xor eax, eax -> test eax, eax , enable camera mouse movement
															// jz loc_53F526 -> jl loc_53F526
			patch::SetUChar(0x6194A0, 0xE9); // jmp setup
		}
	}

	if (mouse_visibility != show_mouse)
	{
		CPad::ClearMouseHistory();
		CPad::UpdatePads();

		ImGui::GetIO().MouseDrawCursor = state;
		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
		mouse_visibility = show_mouse;
	}
}

Hook::Hook()
{
	ImGui::CreateContext();
	Events::initRwEvent += []()
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			Globals::renderer = Render_DirectX9;
			kiero::bind(16, (void**)&oReset9, Reset);
			kiero::bind(17, (void**)&oPresent9, PresentDx9Handler);
		}
		else
		{
			// gtaRenderHook
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				Globals::renderer = Render_DirectX11;
				kiero::bind(8, (void**)&oPresent11, PresentDx11Handler);
			}

			// if (kiero::init(kiero::RenderType::Vulkan) == kiero::Status::Success)
			// {
			// 	Globals::renderer = Render_Vulkan;
			// 	flog << "Vulkan detected!" << std::endl;
			// 	kiero::bind(105, (void**)&hvkCmdDraw, PresentDx11Handler);
			// }
		}
	};
}

Hook::~Hook()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	kiero::shutdown();
}