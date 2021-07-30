#include "pch.h"
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
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	if (Globals::m_bInit)
	{
		ShowMouse(m_bShowMouse);

		// handle window scaling here
		ImVec2 size(screen::GetScreenWidth(), screen::GetScreenHeight());
		if (Globals::m_fScreenSize.x != size.x && Globals::m_fScreenSize.y != size.y)
		{
			int fontSize = static_cast<int>(size.y / 54.85f); // manually tested

			io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", fontSize);
			io.Fonts->Build();

			if (Globals::renderer == Render_DirectX9)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
			}
			else
			{
				ImGui_ImplDX11_InvalidateDeviceObjects();
			}

			if (Globals::m_fScreenSize.x != -1 && Globals::m_fScreenSize.y != -1)
			{
				Globals::m_fMenuSize.x += (size.x - Globals::m_fScreenSize.x) / 4.0f;
				Globals::m_fMenuSize.y += (size.y - Globals::m_fScreenSize.y) / 1.2f;
			}

			ImGuiStyle* style = &ImGui::GetStyle();
			float scaleX = size.x / 1366.0f;
			float scaleY = size.y / 768.0f;

			style->FramePadding = ImVec2(5 * scaleX, 3 * scaleY);
			style->ItemSpacing = ImVec2(8 * scaleX, 4 * scaleY);
			style->ScrollbarSize = 12 * scaleX;
			style->IndentSpacing = 20 * scaleX;
			style->ItemInnerSpacing = ImVec2(4 * scaleX, 4 * scaleY);

			Globals::m_fScreenSize = size;
		}

		ImGui_ImplWin32_NewFrame();
		if (Globals::renderer == Render_DirectX9)
		{
			ImGui_ImplDX9_NewFrame();
		}
		else
		{
			ImGui_ImplDX11_NewFrame();
		}

		ImGui::NewFrame();

		if (windowCallback != nullptr)
		{
			windowCallback();
		}

		ImGui::EndFrame();
		ImGui::Render();

		if (Globals::renderer == Render_DirectX9)
		{
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		}
		else
		{
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
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

void Hook::ShowMouse(bool state)
{
	if (m_bMouseVisibility != m_bShowMouse)
	{
		CPad::ClearMouseHistory();
		CPad::UpdatePads();

		ImGui::GetIO().MouseDrawCursor = state;

		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
		m_bMouseVisibility = m_bShowMouse;
		Hook::ApplyMouseFix(); // Reapply the patches
	}
}

Hook::Hook()
{
	ImGui::CreateContext();

	// Nvidia Overlay crash fix
	if (init(kiero::RenderType::D3D9) == kiero::Status::Success)
	{
		Globals::renderer = Render_DirectX9;
		kiero::bind(16, (void**)&oReset, Reset);
		kiero::bind(42, (void**)&oEndScene, Dx9Handler);
	}
	else
	{
		// gtaRenderHook
		if (init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			Globals::renderer = Render_DirectX11;
			kiero::bind(8, (void**)&oPresent11, Dx11Handler);
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

struct Mouse
{
	unsigned int x, y;
	unsigned int wheelDelta;
	char k1, k2, k3, k4, k5;
};

struct MouseInfo
{
	int x, y, wheelDelta;
} mouseInfo;

static BOOL __stdcall _SetCursorPos(int X, int Y)
{
	if (Hook::m_bShowMouse || GetActiveWindow() != RsGlobal.ps->window)
		return 1;

	mouseInfo.x = X;
	mouseInfo.y = Y;

	return SetCursorPos(X, Y);
}

static LRESULT __stdcall _DispatchMessage(MSG* lpMsg)
{
	if (lpMsg->message == WM_MOUSEWHEEL && !Hook::m_bShowMouse)
	{
		mouseInfo.wheelDelta += *(int*)(&lpMsg->wParam);
	}

	return DispatchMessageA(lpMsg);
}

static int _cdecl _GetMouseState(Mouse* pMouse)
{
	if (Hook::m_bShowMouse)
		return -1;

	struct tagPOINT Point;

	pMouse->x = 0;
	pMouse->y = 0;
	pMouse->wheelDelta = mouseInfo.wheelDelta;
	GetCursorPos(&Point);

	if (mouseInfo.x >= 0)
	{
		pMouse->x = int(Point.x - mouseInfo.x);
	}

	if (mouseInfo.y >= 0)
	{
		pMouse->y = int(Point.y - mouseInfo.y);
	}

	mouseInfo.wheelDelta = 0;

	pMouse->k1 = (GetAsyncKeyState(1) >> 8);
	pMouse->k2 = (GetAsyncKeyState(2) >> 8);
	pMouse->k3 = (GetAsyncKeyState(4) >> 8);
	pMouse->k4 = (GetAsyncKeyState(5) >> 8);
	pMouse->k5 = (GetAsyncKeyState(6) >> 8);
	return 0;
}

void Hook::ApplyMouseFix()
{
	patch::ReplaceFunctionCall(0x53F417, _GetMouseState);
	patch::Nop(0x57C59B, 1);
	patch::ReplaceFunctionCall(0x57C59C, _SetCursorPos);
	patch::Nop(0x81E5D4, 1);
	patch::ReplaceFunctionCall(0x81E5D5, _SetCursorPos);
	patch::Nop(0x74542D, 1);
	patch::ReplaceFunctionCall(0x74542E, _SetCursorPos);
	patch::Nop(0x748A7C, 1);
	patch::ReplaceFunctionCall(0x748A7D, _DispatchMessage);
	patch::SetChar(0x746A08, 32); // diMouseOffset
	patch::SetChar(0x746A58, 32); // diDeviceoffset
}
