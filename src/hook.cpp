#include "pch.h"
#include "hook.h"
#include "../depend/kiero/kiero.h"
#include "../depend/kiero/minhook/MinHook.h"
#include "../depend/imgui/imgui_impl_dx9.h"
#include "../depend/imgui/imgui_impl_dx11.h"
#include "../depend/imgui/imgui_impl_win32.h"
#include <dinput.h>

#define DIMOUSE ((LPDIRECTINPUTDEVICE8)(RsGlobal.ps->diMouse))

LRESULT Hook::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (ImGui::GetIO().WantTextInput)
	{
#ifdef GTASA
		Call<0x53F1E0>(); // CPad::ClearKeyboardHistory
#endif
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
		ImGui::CreateContext();
	}

	ImGuiIO& io = ImGui::GetIO();
	static bool bInit = false;

	if (bInit)
	{
		ShowMouse(m_bShowMouse);

		// Scale the menu if game resolution changed
		static ImVec2 fScreenSize = ImVec2(-1, -1);
		ImVec2 size(screen::GetScreenWidth(), screen::GetScreenHeight());
		if (fScreenSize.x != size.x && fScreenSize.y != size.y)
		{
			FontMgr::ReinitFonts();

			if (gRenderer == Render_DirectX9)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
			}
			else
			{
				ImGui_ImplDX11_InvalidateDeviceObjects();
			}

			ImGuiStyle* style = &ImGui::GetStyle();
			float scaleX = size.x / 1366.0f;
			float scaleY = size.y / 768.0f;

			style->FramePadding = ImVec2(5 * scaleX, 5 * scaleY);
			style->ItemSpacing = ImVec2(8 * scaleX, 4 * scaleY);
			style->ScrollbarSize = 12 * scaleX;
			style->IndentSpacing = 20 * scaleX;
			style->ItemInnerSpacing = ImVec2(5 * scaleX, 5 * scaleY);

			fScreenSize = size;
		}

		ImGui_ImplWin32_NewFrame();
		if (gRenderer == Render_DirectX9)
		{
			ImGui_ImplDX9_NewFrame();
		}
		else
		{
			ImGui_ImplDX11_NewFrame();
		}

		ImGui::NewFrame();

		if (pCallbackFunc != nullptr)
		{
			pCallbackFunc();
		}

		ImGui::EndFrame();
		ImGui::Render();

		if (gRenderer == Render_DirectX9)
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
		bInit = true;
		ImGui_ImplWin32_Init(RsGlobal.ps->window);

#ifdef GTASA
		// shift trigger fix
		patch::Nop(0x00531155, 5);
#endif

		if (gRenderer == Render_DirectX9)
		{
			ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(ptr));
		}
		else
		{
			// for dx11 device ptr is swapchain
			reinterpret_cast<IDXGISwapChain*>(ptr)->GetDevice(__uuidof(ID3D11Device), &ptr);
			ID3D11DeviceContext* context;
			reinterpret_cast<ID3D11Device*>(ptr)->GetImmediateContext(&context);

			ImGui_ImplDX11_Init(reinterpret_cast<ID3D11Device*>(ptr), context);
		}

		ImGui_ImplWin32_EnableDpiAwareness();

		// Loading fonts
		io.FontDefault = FontMgr::LoadFont("text", 1.0f);
		FontMgr::LoadFont("title", 2.0f);
		FontMgr::LoadFont("header", 1.25f);
		
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

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
	// Disable player controls for controllers
	bool bMouseDisabled = false;
	bool isController;

#ifdef GTA3
	isController =  !patch::Get<BYTE>(0x5F03D8);
#elif GTAVC
	isController =  patch::Get<BYTE>(0x86968B);
#else // GTASA
	isController =  patch::Get<BYTE>(0xBA6818);
#endif


	if (isController && (m_bShowMouse || bMouseDisabled))
	{

#ifdef GTASA
		CPlayerPed *player = FindPlayerPed();
		CPad *pad = player ? player->GetPadFromPlayer() : NULL;
#else
		CPad *pad = CPad::GetPad(0);
#endif

		if (pad)
		{
			if (m_bShowMouse)
			{
				bMouseDisabled = true;
#ifdef GTA3
				pad->m_bDisablePlayerControls = true;
#else //GTAVC & GTASA
				pad->DisablePlayerControls = true;
#endif
			}
			else
			{
				bMouseDisabled = false;
#ifdef GTA3
				pad->m_bDisablePlayerControls = false;
#else //GTAVC & GTASA
				pad->DisablePlayerControls = false;
#endif
			}
		}
	}

	if (m_bMouseVisibility != m_bShowMouse)
	{
		ImGui::GetIO().MouseDrawCursor = state;

#ifdef GTASA
		Hook::ApplyMouseFix(); // Reapply the patches
#else
		if (m_bShowMouse)
		{
			
			patch::SetUChar(BY_GAME(0, 0x6020A0, 0x580D20), 0xC3); // psSetMousePos
			patch::Nop(BY_GAME(0, 0x4AB6CA, 0x49272F), 5); // don't call CPad::UpdateMouse()
		}
		else
		{
			
			patch::SetUChar(BY_GAME(0, 0x6020A0, 0x580D20), 0x53);
#ifdef GTAVC
			patch::SetRaw(0x4AB6CA, (char*)"\xE8\x51\x21\x00\x00", 5);
#else // GTA3
			patch::SetRaw(0x49272F, (char*)"\xE8\x6C\xF5\xFF\xFF", 5);
#endif
		}
#endif

		CPad::NewMouseControllerState.X = 0;
		CPad::NewMouseControllerState.Y = 0;
#ifdef GTA3
		CPad::GetPad(0)->ClearMouseHistory();
#else // GTAVC & GTASA
		CPad::ClearMouseHistory();
#endif
		CPad::UpdatePads();
		m_bMouseVisibility = m_bShowMouse;
	}
}

Hook::Hook()
{

	// Nvidia Overlay crash fix
	if (init(kiero::RenderType::D3D9) == kiero::Status::Success)
	{
		gRenderer = Render_DirectX9;
		kiero::bind(16, (void**)&oReset, Reset);
		kiero::bind(42, (void**)&oEndScene, Dx9Handler);
	}
	else
	{
		// gtaRenderHook
		if (init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			gRenderer = Render_DirectX11;
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

#ifdef GTASA
struct Mouse
{
	unsigned int x, y;
	unsigned int wheelDelta;
	unsigned char buttons[8];
};

struct MouseInfo
{
	int x, y, wheelDelta;
} mouseInfo;

static BOOL __stdcall _SetCursorPos(int X, int Y)
{
	if (Hook::m_bShowMouse || GetActiveWindow() != RsGlobal.ps->window)
	{
		return 1;
	}

	mouseInfo.x = X;
	mouseInfo.y = Y;

	return SetCursorPos(X, Y);
}

static LRESULT __stdcall _DispatchMessage(MSG* lpMsg)
{
	if (lpMsg->message == WM_MOUSEWHEEL && !Hook::m_bShowMouse)
	{
		mouseInfo.wheelDelta += GET_WHEEL_DELTA_WPARAM(lpMsg->wParam);
	}

	return DispatchMessageA(lpMsg);
}

static int _cdecl _GetMouseState(Mouse* pMouse)
{
	if (Hook::m_bShowMouse || !RsGlobal.ps->diMouse)
	{
		DIMOUSE->Unacquire();
		return -1;
	}
	
	if (DIMOUSE->GetDeviceState(sizeof(Mouse), pMouse) < 0)
	{
		if (DIMOUSE->Acquire() == DIERR_NOTINITIALIZED)
		{
			while (DIMOUSE->Acquire() == DIERR_NOTINITIALIZED);
		}
	}

	pMouse->wheelDelta = mouseInfo.wheelDelta;
	mouseInfo.wheelDelta = 0;
	pMouse->buttons[0] = (GetAsyncKeyState(1) >> 8);
	pMouse->buttons[1] = (GetAsyncKeyState(2) >> 8);
	pMouse->buttons[2] = (GetAsyncKeyState(4) >> 8);
	pMouse->buttons[3] = (GetAsyncKeyState(5) >> 8);
	pMouse->buttons[4] = (GetAsyncKeyState(6) >> 8);
	
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
#endif