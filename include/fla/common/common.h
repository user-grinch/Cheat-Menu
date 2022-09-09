/*
* Copyright (C) fastman92 <fastman92@gmail.com>, website: http://fastman92.com
* Licensed under the MIT License, see LICENSE at top level directory.
*
*/

#pragma once
#define F92_LA_CONVENTION

#define UNDEFINED_FILE_ID_UINT16 0xFFFF
#define UNDEFINED_FILE_ID_INT32 -1
#define UNKNOWN_MODEL_ID_MINUS_2 -2
#define MAX_FILE_ID_UINT16 0xFFFC
#define MAX_FILE_ID_INT32 0x7FFFFFFF

// Stringify
#if !defined(STRINGIFY) || defined(TOSTRING)
#define STRINGIFY(str) #str
#define TOSTRING(x) STRINGIFY(x)
#endif

// FLA library filename
#ifdef IS_PLATFORM_WIN_X86
#define FLA_FILENAME "$fastman92limitAdjuster.asi"
#elif defined(IS_PLATFORM_WIN_X64)
#define FLA_FILENAME "$fastman92limitAdjusterX64.asi"
#elif defined(IS_PLATFORM_ANDROID)
#define FLA_FILENAME "libplugin_fastman92limitAdjuster_" TOSTRING(SOLUTION_PLATFORM) ".so"
#endif
