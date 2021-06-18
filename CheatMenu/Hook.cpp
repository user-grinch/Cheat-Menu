#include "Hook.h"
#include "../Depend/kiero/kiero.h"
#include "../Depend/kiero/minhook/MinHook.h"

LRESULT Hook::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (m_bShowMouse)
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

	return oReset(pDevice, pPresentationParameters);
}

void Hook::RenderFrame(void* ptr)
{
	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();

	if (Globals::m_bInit)
	{
		ShowMouse(m_bShowMouse);

		// handle window scaling here
		ImVec2 size(screen::GetScreenWidth(), screen::GetScreenHeight());
		if (Globals::m_fScreenSize.x != size.x && Globals::m_fScreenSize.y != size.y)
		{
			int font_size = static_cast<int>(size.y / 54.85f); // manually tested

			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", font_size);
			io.Fonts->Build();

			if (Globals::renderer == Render_DirectX9)
				ImGui_ImplDX9_InvalidateDeviceObjects();
			else
				ImGui_ImplDX11_InvalidateDeviceObjects();

			if (Globals::m_fScreenSize.x != -1 && Globals::m_fScreenSize.y != -1)
			{
				Globals::m_fMenuSize.x += (size.x - Globals::m_fScreenSize.x) / 4.0f;
				Globals::m_fMenuSize.y += (size.y - Globals::m_fScreenSize.y) / 1.2f;
			}

			ImGuiStyle* style = &ImGui::GetStyle();
			float scale_x = size.x / 1366.0f;
			float scale_y = size.y / 768.0f;

			style->FramePadding = ImVec2(5 * scale_x, 3 * scale_y);
			style->ItemSpacing = ImVec2(8 * scale_x, 4 * scale_y);
			style->ScrollbarSize = 12 * scale_x;
			style->IndentSpacing = 20 * scale_x;
			style->ItemInnerSpacing = ImVec2(4 * scale_x, 4 * scale_y);

			Globals::m_fScreenSize = size;
		}

		ImGui_ImplWin32_NewFrame();
		if (Globals::renderer == Render_DirectX9)
			ImGui_ImplDX9_NewFrame();
		else
			ImGui_ImplDX11_NewFrame();

		ImGui::NewFrame();

		if (window_callback != nullptr)
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
		Globals::m_bInit = true;
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
			reinterpret_cast<IDXGISwapChain*>(ptr)->GetDevice(__uuidof(ID3D11Device), &Globals::device);
			ID3D11DeviceContext* context;
			reinterpret_cast<ID3D11Device*>(Globals::device)->GetImmediateContext(&context);

			ImGui_ImplDX11_Init(reinterpret_cast<ID3D11Device*>(Globals::device), context);
		}

		ImGui_ImplWin32_EnableDpiAwareness();

		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad && ImGuiConfigFlags_NavEnableKeyboard;

		style.WindowTitleAlign = ImVec2(0.5, 0.5);
		oWndProc = (WNDPROC)SetWindowLongPtr(RsGlobal.ps->window, GWL_WNDPROC, (LRESULT)WndProc);
	}
}

HRESULT Hook::Dx9Handler(IDirect3DDevice9* pDevice)
{
	RenderFrame(pDevice);
	return oEndScene(pDevice);
}

HRESULT Hook::Dx11Handler(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	RenderFrame(pSwapChain);
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
		if (m_bMouseVisibility != m_bShowMouse)
		{
			patch::SetUChar(0x6194A0, 0xE9); // jmp setup
			patch::SetUChar(0x746ED0, 0xA1);
			patch::SetRaw(0x53F41F, (void*)"\x85\xC0\x0F\x8C", 4);
			// xor eax, eax -> test eax, eax , enable camera mouse movement
			// jz loc_53F526 -> jl loc_53F526
		}
	}

	if (m_bMouseVisibility != m_bShowMouse)
	{
		CPad::ClearMouseHistory();
		CPad::UpdatePads();

		// TODO: Replace this with windows cursor
		ImGui::GetIO().MouseDrawCursor = state;

		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
		m_bMouseVisibility = m_bShowMouse;
	}
}

Hook::Hook()
{
	ImGui::CreateContext();

	// gtaRenderHook
	if (init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		Globals::renderer = Render_DirectX11;
		kiero::bind(8, (void**)&oPresent11, Dx11Handler);
	}
	else
	{
		if (init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			Globals::renderer = Render_DirectX9;
			kiero::bind(16, (void**)&oReset, Reset);
			kiero::bind(42, (void**)&oEndScene, Dx9Handler);
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
