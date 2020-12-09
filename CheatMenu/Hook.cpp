#include "pch.h"
#include "Hook.h"
#include "external/kiero/kiero.h"
#include "external/kiero/minhook/include/MinHook.h"

WNDPROC Hook::oWndProc = NULL;
f_Reset Hook::oReset9 = NULL; 
f_Present11 Hook::oPresent11 = NULL;
f_Present9 Hook::oPresent9 = NULL;

bool Hook::disable_controls = false;
std::function<void()> Hook::window_func = NULL;

LRESULT Hook::InputProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (disable_controls)
		return 1;
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

Hook::Hook()
{
	ImGui::CreateContext();
	Events::initRwEvent += []()
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			assert(kiero::bind(16, (void**)&oReset9, ResetDx9) == kiero::Status::Success);
			assert(kiero::bind(17, (void**)&oPresent9, PresentDx9) == kiero::Status::Success);
			Globals::renderer = Render_DirectX9;

			flog << "Successfully hooked dx9 device." << std::endl;
		}
		else
		{
			// gtaRenderHook
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				kiero::bind(8, (void**)&oPresent11, PresentDx11);
				Globals::renderer = Render_DirectX11;

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