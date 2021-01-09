#pragma once

#ifdef IS_PLATFORM_WIN
#include <Windows.h>
#endif

#ifdef IS_PLATFORM_ANDROID
#include <dlfcn.h>
#endif

union tGenericLibraryModule
{
#ifdef IS_PLATFORM_WIN
	HMODULE windows;
#elif defined(IS_PLATFORM_ANDROID)
	void* linux_os;
#endif
};

class Library
{
public:
	// Returns symbol address
	static void* GetSymbolAddress(const tGenericLibraryModule* pModule, const char* symbolName);
};