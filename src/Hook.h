#pragma once
#include "pch.h"

typedef HRESULT(CALLBACK *f_Present9)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
typedef HRESULT(CALLBACK *f_Present11)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(CALLBACK *f_Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Hook
{
private:
	static WNDPROC oWndProc;
	static f_Present11 oPresent11;
	static f_Present9 oPresent9;
	static f_Reset oReset9;
	static bool mouse_visibility;

	static void CALLBACK Present(void *ptr);
	static HRESULT CALLBACK PresentDx9Handler(IDirect3DDevice9 *pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
	static HRESULT CALLBACK PresentDx11Handler(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static HRESULT CALLBACK Reset(IDirect3DDevice9 * pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters);
	static LRESULT CALLBACK InputProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void ShowMouse(bool state);

protected:
	static bool show_mouse;
	static std::function<void()> window_func;

	Hook();
	~Hook();
};


