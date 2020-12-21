#include "pch.h"
#include "Hook.h"
#include "external/kiero/kiero.h"
#include "external/kiero/minhook/include/MinHook.h"

WNDPROC Hook::oWndProc = NULL;
f_Reset Hook::oReset9 = NULL; 
f_Present11 Hook::oPresent11 = NULL;
f_Present9 Hook::oPresent9 = NULL;

bool Hook::mouse_visibility = false;
bool Hook::show_mouse = false;

std::function<void()> Hook::window_func = NULL;

LRESULT Hook::InputProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (ImGui::GetIO().WantTextInput)
	{
		Call<0x53F1E0>(); // CPad::ClearKeyboardHistory
	 	return 1;
	}
	else
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT Hook::ResetDx9(IDirect3DDevice9 * pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	return oReset9(pDevice, pPresentationParameters);
}

HRESULT Hook::PresentDx9(IDirect3DDevice9 *pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	ImGuiIO& io = ImGui::GetIO();

	if (Globals::init_done)
	{
		if (mouse_visibility != show_mouse)
		{
			Hook::ShowMouse(show_mouse);
			mouse_visibility = show_mouse;
		}

		// Change font size if the game resolution changes
		if (Globals::font_screen_size.x != screen::GetScreenWidth()
			&& Globals::font_screen_size.y != screen::GetScreenHeight())
		{
			int font_size = int(screen::GetScreenHeight() / 54.85); // manually tested

			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", font_size);
			io.Fonts->Build();
			ImGui_ImplDX9_InvalidateDeviceObjects();

			Globals::font_screen_size = ImVec2(screen::GetScreenWidth(), screen::GetScreenHeight());
		}

		static RwBool fullscreen = RsGlobal.ps->fullScreen;
		if (fullscreen != RsGlobal.ps->fullScreen)	
		{	
			flog << "LETS GO" << std::endl;
			fullscreen = RsGlobal.ps->fullScreen;	
			ImGui_ImplDX9_InvalidateDeviceObjects();
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (window_func != NULL)
			window_func();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	else
	{
		Globals::init_done = true;
		ImGui::CreateContext();

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui_ImplWin32_Init(RsGlobal.ps->window);
		ImGui_ImplDX9_Init(pDevice);
		ImGui_ImplWin32_EnableDpiAwareness();

		io.IniFilename = NULL;
		io.LogFilename = NULL;

		style.WindowTitleAlign = ImVec2(0.5, 0.5);
		oWndProc = (WNDPROC)SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LRESULT)InputProc);
	}

	return oPresent9(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);;
}

HRESULT Hook::PresentDx11(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	ImGuiIO& io = ImGui::GetIO();

	if (Globals::init_done)
	{
		HRESULT hr = pSwapChain->Present(1, 0);
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			ImGui_ImplDX11_InvalidateDeviceObjects();

		// Change font size if the game resolution changes
		if (Globals::font_screen_size.x != screen::GetScreenWidth()
			&& Globals::font_screen_size.y != screen::GetScreenHeight())
		{
			int font_size = int(screen::GetScreenHeight() / 54.85); // manually tested

			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", font_size);
			io.Fonts->Build();
			ImGui_ImplDX11_InvalidateDeviceObjects();

			Globals::font_screen_size = ImVec2(screen::GetScreenWidth(), screen::GetScreenHeight());
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (window_func != NULL)
			window_func();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}
	else
	{
		Globals::init_done = true;

		ImGuiStyle& style = ImGui::GetStyle();

		DXGI_SWAP_CHAIN_DESC desc;
		pSwapChain->GetDesc(&desc);

		pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Globals::device11);
		ID3D11DeviceContext* context;
		Globals::device11->GetImmediateContext(&context);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(desc.OutputWindow);
		ImGui_ImplDX11_Init(Globals::device11, context);
		ImGui_ImplWin32_EnableDpiAwareness();

		io.IniFilename = NULL;
		io.LogFilename = NULL;

		style.WindowTitleAlign = ImVec2(0.5, 0.5);

		oWndProc = (WNDPROC)SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LONG)InputProc);
	}

	return oPresent11(pSwapChain, SyncInterval, Flags);
}

void Hook::ShowMouse(bool state)
{
	if (state)
	{
		patch::SetUChar(0x6194A0, 0xC3);
		patch::Nop(0x53F417, 5); // don't call CPad__getMouseState
		patch::SetRaw(0x53F41F, "\x33\xC0\x0F\x84", 4); // disable camera mouse movement
	}
	else
	{
		patch::SetRaw(0x541DF5, "\xE8\x46\xF3\xFE\xFF", 5); // call CControllerConfigManager::AffectPadFromKeyBoard
		patch::SetRaw(0x53F417, "\xE8\xB4\x7A\x20\x00", 5); // call CPad__getMouseState
		patch::SetRaw(0x53F41F, "\x85\xC0\x0F\x8C", 4); // xor eax, eax -> test eax, eax , enable camera mouse movement
														// jz loc_53F526 -> jl loc_53F526
		patch::SetUChar(0x6194A0, 0xE9); // jmp setup
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
			if (kiero::bind(16, (void**)&oReset9, ResetDx9) == kiero::Status::Success
			&& kiero::bind(17, (void**)&oPresent9, PresentDx9) == kiero::Status::Success) 
			{
				Globals::renderer = Render_DirectX9;
				flog << "Successfully hooked dx9 device." << std::endl;
			}
		}
		else
		{
			// gtaRenderHook
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				if (kiero::bind(8, (void**)&oPresent11, PresentDx11) == kiero::Status::Success) 
				{
					Globals::renderer = Render_DirectX11;
					flog << "Successfully hooked dx11 device." << std::endl;
				}

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