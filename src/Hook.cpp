#include "Hook.h"
#include "kiero/kiero.h"
#include "kiero/minhook/MinHook.h"

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

HRESULT Hook::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	return oReset9(pDevice, pPresentationParameters);
}

void Hook::Present(void* ptr)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();

	if (Globals::init_done)
	{	
		Hook::ShowMouse(show_mouse);

		// handle window scaling here
		ImVec2 size(screen::GetScreenWidth(), screen::GetScreenHeight());
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
				Globals::menu_size.x += (size.x - Globals::screen_size.x) / 4.0f;
				Globals::menu_size.y += (size.y - Globals::screen_size.y) / 1.2f;
			}

			ImGuiStyle* style = &ImGui::GetStyle();
			float scale_x = size.x / 1366.0f;
			float scale_y = size.y / 768.0f;

			style->FramePadding = ImVec2(5 * scale_x, 3 * scale_y);
			style->ItemSpacing = ImVec2(8 * scale_x, 4 * scale_y);
			style->ScrollbarSize = 12 * scale_x;
			style->IndentSpacing = 20 * scale_x;
			style->ItemInnerSpacing = ImVec2(4 * scale_x, 4 * scale_y);

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
		patch::Nop(0x00531155, 5);

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
		oWndProc = (WNDPROC)SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LRESULT)WndProc);
	}
}

HRESULT Hook::PresentDx9Handler(IDirect3DDevice9* pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	Present(pDevice);
	return oPresent9(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
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
		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
		patch::SetUChar(0x6194A0, 0xC3);

		// Don't nop this, WindowedMode uses it
		// patch::Nop(0x53F417, 5); // don't call CPad__getMouseState
		patch::SetUChar(0x746ED0, 0xC3);

		patch::SetRaw(0x53F41F, (void*)"\x33\xC0\x0F\x84", 4); // disable camera mouse movement
	}
	else
	{
		if (mouse_visibility != show_mouse)
		{
			patch::SetUChar(0x6194A0, 0xE9); // jmp setup
			patch::SetUChar(0x746ED0, 0xA1);
			patch::SetRaw(0x53F41F, (void*)"\x85\xC0\x0F\x8C", 4); // xor eax, eax -> test eax, eax , enable camera mouse movement
															// jz loc_53F526 -> jl loc_53F526
		}
	}

	if (mouse_visibility != show_mouse)
	{
		CPad::ClearMouseHistory();
		CPad::UpdatePads();

		// TODO: Replace this with windows cursor
		ImGui::GetIO().MouseDrawCursor = state;

		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
		mouse_visibility = show_mouse;
	}
}

Hook::Hook()
{
	ImGui::CreateContext();

	// gtaRenderHook
	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		Globals::renderer = Render_DirectX11;
		kiero::bind(8, (void**)&oPresent11, PresentDx11Handler);
	}
	else
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			Globals::renderer = Render_DirectX9;
			kiero::bind(16, (void**)&oReset9, Reset);
			kiero::bind(17, (void**)&oPresent9, PresentDx9Handler);
		}
	}
}

Hook::~Hook()
{
	SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LRESULT)oWndProc);
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	kiero::shutdown();
}