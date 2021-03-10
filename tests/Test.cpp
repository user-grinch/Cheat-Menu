#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
		MessageBox(NULL, "SilentPatch isn't installed. Exiting CheatMenu.", "CheatMenu", MB_ICONERROR);
	

	return TRUE;
}