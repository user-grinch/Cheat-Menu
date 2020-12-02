#pragma once

typedef HRESULT(CALLBACK *f_Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(CALLBACK *f_Present9)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
typedef HRESULT(CALLBACK *f_Present11)(IDXGISwapChain*, UINT, UINT);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Hook
{
private:
	static WNDPROC oWndProc;
	static f_Reset oReset9;
	static f_Present11 oPresent11;
	static f_Present9 oPresent9;

	static HRESULT CALLBACK ResetDx9(IDirect3DDevice9 * pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static HRESULT CALLBACK PresentDx9(IDirect3DDevice9 *pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
	static HRESULT CALLBACK PresentDx11(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	static LRESULT CALLBACK InputProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	static bool disable_controls;
	static std::function<void()> window_func;

public:
	Hook();
	~Hook();
};


