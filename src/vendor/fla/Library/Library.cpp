#include "Library.h"

// Returns symbol address
void* Library::GetSymbolAddress(const tGenericLibraryModule* pModule, const char* symbolName)
{
#ifdef IS_PLATFORM_WIN
	return GetProcAddress(pModule->windows, symbolName);
#elif defined(IS_PLATFORM_ANDROID)
	return dlsym(pModule->linux_os, symbolName);
#endif
}