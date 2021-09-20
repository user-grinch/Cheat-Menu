#pragma once
#include "pch.h"

using f_EndScene = HRESULT(CALLBACK*)(IDirect3DDevice9*);
using f_Present11 = HRESULT(CALLBACK*)(IDXGISwapChain*, UINT, UINT);
using f_Reset = HRESULT(CALLBACK*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Hook
{
private:
	static inline WNDPROC oWndProc;
	static inline f_Present11 oPresent11;
	static inline f_EndScene oEndScene;
	static inline f_Reset oReset;
	static inline bool m_bMouseVisibility;

	static void CALLBACK RenderFrame(void* ptr);
	static HRESULT CALLBACK Dx9Handler(IDirect3DDevice9* pDevice);
	static HRESULT CALLBACK Dx11Handler(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static HRESULT CALLBACK Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void ShowMouse(bool state);

public:
	static inline bool m_bShowMouse = false;
	static inline std::function<void()> windowCallback = nullptr;

#ifdef GTASA
	static void ApplyMouseFix();
#endif

	Hook();
	~Hook();
};
