/*
* Copyright (C) fastman92 <fastman92@gmail.com>, website: http://fastman92.com
* Licensed under the MIT License, see LICENSE at top level directory.
*
*/

#pragma once
#include <stdint.h>

#define IMG_FASTMAN92_GTASA_MAX_FILENAME_LENGTH 39

#pragma pack(push, 1)
class tImgFastman92GTASAdirectoryEntry
{
public:
	uint32_t PositionInSectors;
	uint16_t OriginalSizeInSectors;
	uint16_t NumberOfPaddedBytesInAlignedOriginalSize;
	uint16_t PackedSizeInSectors;
	uint16_t NumberOfPaddedBytesInAlignedPackedSize;
	uint32_t Flags;
	char Name[IMG_FASTMAN92_GTASA_MAX_FILENAME_LENGTH + 1];
	char Reserved[8];	// should have NULL values when reserved.
};
#pragma pack(pop)