#pragma once
#include "pch.h"

typedef HRESULT(CALLBACK* f_EndScene)(IDirect3DDevice9*);
typedef HRESULT(CALLBACK* f_Present11)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(CALLBACK* f_Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Hook
{
private:
	inline static WNDPROC oWndProc = NULL;
	inline static f_Present11 oPresent11 = NULL;
	inline static f_EndScene oEndScene = NULL;
	inline static f_Reset oReset = NULL;
	inline static bool mouse_visibility = false;

	static void CALLBACK RenderFrame(void* ptr);
	static HRESULT CALLBACK Dx9Handler(IDirect3DDevice9* pDevice);
	static HRESULT CALLBACK Dx11Handler(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static HRESULT CALLBACK Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static LRESULT CALLBACK WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void ShowMouse(bool state);

protected:
	inline static bool show_mouse = false;
	inline static std::function<void()> window_callback = NULL;
	Hook();
	~Hook();
};


