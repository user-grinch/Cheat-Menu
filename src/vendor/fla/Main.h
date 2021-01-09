/*
* Copyright (C) fastman92 <fastman92@gmail.com>, website: http://fastman92.com
* Licensed under the MIT License, see LICENSE at top level directory.
*
*/

#pragma once
#include "common/common.h"
#include "Library/Library.h"

#define SET_FUNCTION_ADDRESS(functionName) if(phModule)*(void**)&funcs.functionName = Library::GetSymbolAddress(phModule, TOSTRING(functionName)); else funcs.functionName = 0;

class CFastman92limitAdjuster
{
public:
	// hModule_of_the_FLA
	static tGenericLibraryModule hModule;

	// Initializes everything related to the FLA
	static void Init();
};