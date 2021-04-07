/*
* Copyright (C) fastman92 <fastman92@gmail.com>, website: http://fastman92.com
* Licensed under the MIT License, see LICENSE at top level directory.
*
*/

#pragma once

// CValueExtension
template <typename from, typename to, to(*GetFunc)(const void* basicPtr), void SetFunc(void* basicPtr, to ID)> class CValueExtension
{
	from m_Value;

	// Constructor
	CValueExtension() = delete;

public:
	// Returns ID
	to Get() const
	{
		return GetFunc(&this->m_Value);
	}

	// Sets ID
	void Set(to ID)
	{
		SetFunc(&this->m_Value, ID);
	}
};

// CValueExtensionInt
template <typename from, typename to, to(*GetFunc)(const void* basicPtr), void SetFunc(void* basicPtr, to ID)> class CValueExtensionNum
	: public CValueExtension<from, to, GetFunc, SetFunc>
{
public:
	// Assignnment operators
	CValueExtensionNum& operator=(to dwModelIndex) { this->Set(dwModelIndex); return *this; }
	CValueExtensionNum& operator=(const CValueExtensionNum& dwModelIndex) { this->Set(dwModelIndex.Get()); return *this; }

	// Comparison operators
	bool operator==(to dwModelIndex) { return this->Get() == dwModelIndex; }
	bool operator==(const CValueExtensionNum& dwModelIndex) { return this->Get() == dwModelIndex.Get(); }
	bool operator>=(to dwModelIndex) { return this->Get() >= dwModelIndex; }
	bool operator>=(const CValueExtensionNum& dwModelIndex) { return this->Get() >= dwModelIndex.Get(); }
	bool operator<=(to dwModelIndex) { return this->Get() <= dwModelIndex; }
	bool operator<=(const CValueExtensionNum& dwModelIndex) { return this->Get() <= dwModelIndex.Get(); }
	bool operator>(to dwModelIndex) { return this->Get() > dwModelIndex; }
	bool operator>(const CValueExtensionNum& dwModelIndex) { return this->Get() > dwModelIndex.Get(); }
	bool operator<(to dwModelIndex) { return this->Get() < dwModelIndex; }
	bool operator<(const CValueExtensionNum& dwModelIndex) { return this->Get() < dwModelIndex.Get(); }
	bool operator!=(to dwModelIndex) { return this->Get() != dwModelIndex; }
	bool operator!=(const CValueExtensionNum& dwModelIndex) { return this->Get() != dwModelIndex.Get(); }

	// Conversion operators
	operator to() const { return this->Get(); }
};