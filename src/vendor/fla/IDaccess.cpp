/*
* Copyright (C) fastman92 <fastman92@gmail.com>, website: http://fastman92.com
* Licensed under the MIT License, see LICENSE at top level directory.
*
*/

#include "IDaccess.h"

// pointers of functions
tIDaccessFuncs IDaccess::funcs;

// Initializes
void IDaccess::Init()
{
	tGenericLibraryModule* phModule = &CFastman92limitAdjuster::hModule;

	SET_FUNCTION_ADDRESS(AreDifficultIDsExtended);
	SET_FUNCTION_ADDRESS(GetExtendedIDfrom16bitBefore);
	SET_FUNCTION_ADDRESS(SetExtendedIDfrom16bitBefore);

	SET_FUNCTION_ADDRESS(AreVehicleColourIDsextended);
	SET_FUNCTION_ADDRESS(GetExtendedIDfrom8bitBefore);
	SET_FUNCTION_ADDRESS(SetExtendedIDfrom8bitBefore);
}

// Tells if DFF/TXD 32-bit IDs are enabled.
bool IDaccess::AreDifficultIDsExtended()
{
	return funcs.AreDifficultIDsExtended ? funcs.AreDifficultIDsExtended() : false;
}

// Returns extended ID for the pointer, where an ID of 16-bit size was available before.
int32_t IDaccess::GetExtendedIDfrom16bitBefore(const void* basicPtr)
{
	if (funcs.GetExtendedIDfrom16bitBefore)
		return funcs.GetExtendedIDfrom16bitBefore(basicPtr);
	else
	{
		uint16_t uID = *(uint16_t*)basicPtr;

		if (uID > MAX_FILE_ID_UINT16)	// is it negative ID?
			return *(int16_t*)basicPtr;
		else
			return uID;
	}
}

// Sets extended ID for the pointer, where an ID of 16-bit size was available before.
void IDaccess::SetExtendedIDfrom16bitBefore(void* basicPtr, int32_t ID)
{
	if (funcs.SetExtendedIDfrom16bitBefore)
		funcs.SetExtendedIDfrom16bitBefore(basicPtr, ID);
	else
		*(uint16_t*)basicPtr = ID;
}

// Compares only a significiant part, uses for patches such as "cmp ax, 520" or so.
// Where only 16-bit would be used before, but now it may be 32-bit
int32_t IDaccess::CompareDifficultIDonlySignificiantPart(int32_t a, uint16_t b)
{
	if (IDaccess::AreDifficultIDsExtended())
		return  a == b;
	else
		return (uint16_t)a == b;
}

// Returns true if vehicles colour IDs extended.
bool IDaccess::AreVehicleColourIDsextended()
{
	return funcs.AreVehicleColourIDsextended
		? funcs.AreVehicleColourIDsextended() : false;
}

// Returns extended ID for the pointer, where an ID of 8-bit size was available before.
int32_t IDaccess::GetExtendedIDfrom8bitBefore(const void* basicPtr)
{
	if (funcs.GetExtendedIDfrom8bitBefore)
		return funcs.GetExtendedIDfrom8bitBefore(basicPtr);
	else
	{
		uint8_t uID = *(uint8_t*)basicPtr;

		if (uID == 0xFF)	// is it unset ID?
			return -1;
		else
			return uID;
	}
}

// Returns extended ID for the pointer, where an ID of 8-bit size was available before.
void IDaccess::SetExtendedIDfrom8bitBefore(void* basicPtr, int32_t ID)
{
	if (funcs.SetExtendedIDfrom8bitBefore)
		funcs.SetExtendedIDfrom8bitBefore(basicPtr, ID);
	else
		*(uint8_t*)basicPtr = ID;
}