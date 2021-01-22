#include "Hook.h"
#include "vendor/kiero/kiero.h"
#include "vendor/kiero/minhook/MinHook.h"

WNDPROC Hook::oWndProc = NULL;
f_Present11 Hook::oPresent11 = NULL;
f_Present9 Hook::oPresent9 = NULL;
f_Reset Hook::oReset9 = NULL; 

bool Hook::mouse_visibility = false;
bool Hook::show_mouse = false;

std::function<void()> Hook::window_func = NULL;

LRESULT Hook::InputProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (Hook::show_mouse)
	{
		patch::Nop(0x4EB9F4, 5); //  disable radio scroll
		Call<0x541BD0>(); // CPad::ClearMouseHistory
	}
	else
		patch::SetRaw(0x4EB9F4, (void*)"\xE8\x67\xFC\xFF\xFF", 5); // enable radio scroll

	if (ImGui::GetIO().WantTextInput)
	{
		Call<0x53F1E0>(); // CPad::ClearKeyboardHistory
	 	return 1;
	}
	else
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

		// Change font size if the game resolution changes
		if (Globals::screen_size.x != screen::GetScreenWidth()
			&& Globals::screen_size.y != screen::GetScreenHeight())
		{
			int font_size = int(screen::GetScreenHeight() / 54.85); // manually tested

			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", font_size);
			io.Fonts->Build();

			if (Globals::renderer == Render_DirectX9)
				ImGui_ImplDX9_InvalidateDeviceObjects();
			else
				ImGui_ImplDX11_InvalidateDeviceObjects();

			Globals::screen_size = ImVec2(screen::GetScreenWidth(), screen::GetScreenHeight());
		}

		ImGui_ImplWin32_NewFrame();
		if (Globals::renderer == Render_DirectX9)
			ImGui_ImplDX9_NewFrame();
		else
			ImGui_ImplDX11_NewFrame();

		ImGui::NewFrame();

		if (window_func != NULL)
			window_func();

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
		}

		ImGui_ImplWin32_EnableDpiAwareness();

		io.IniFilename = NULL;
		io.LogFilename = NULL;

		style.WindowTitleAlign = ImVec2(0.5, 0.5);
		oWndProc = (WNDPROC)SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LRESULT)InputProc);
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

// Thanks imring
void Hook::ShowMouse(bool state)
{
	if (state)
	{
		patch::SetUChar(0x6194A0, 0xC3);
		patch::Nop(0x53F417, 5); // don't call CPad__getMouseState
		patch::SetRaw(0x53F41F, (void*)"\x33\xC0\x0F\x84", 4); // disable camera mouse movement
	}
	else
	{
		if (mouse_visibility != show_mouse)
		{
			patch::SetRaw(0x541DF5, (void*)"\xE8\x46\xF3\xFE\xFF", 5); // call CControllerConfigManager::AffectPadFromKeyBoard
			patch::SetRaw(0x53F417, (void*)"\xE8\xB4\x7A\x20\x00", 5); // call CPad__getMouseState
			patch::SetRaw(0x53F41F, (void*)"\x85\xC0\x0F\x8C", 4); // xor eax, eax -> test eax, eax , enable camera mouse movement
															// jz loc_53F526 -> jl loc_53F526
			patch::SetUChar(0x6194A0, 0xE9); // jmp setup
			mouse_visibility = show_mouse;
		}
	}

	ImGui::GetIO().MouseDrawCursor = state;
	CPad::NewMouseControllerState.X = 0;
	CPad::NewMouseControllerState.Y = 0;

	// Broken in psdk
	Call<0x541BD0>(); // CPad::ClearMouseHistory
	Call<0x541DD0>(); // CPad::UpdatePads
}

Hook::Hook()
{
	ImGui::CreateContext();
	Events::initRwEvent += []()
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			Globals::renderer = Render_DirectX9;
			if (kiero::bind(16, (void**)&oReset9, Reset) == kiero::Status::Success
			&& kiero::bind(17, (void**)&oPresent9, PresentDx9Handler) == kiero::Status::Success) 
				flog << "Successfully hooked dx9 device." << std::endl;
		}
		else
		{
			// gtaRenderHook
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				Globals::renderer = Render_DirectX11;
				if (kiero::bind(8, (void**)&oPresent11, PresentDx11Handler) == kiero::Status::Success) 
					flog << "Successfully hooked dx11 device." << std::endl;
			}
			else
			{
				flog << "Failed to hook device" << std::endl;
			}
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