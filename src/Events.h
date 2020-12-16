#pragma once

namespace plugin
{
	namespace Events
	{
		extern ThiscallEvent<AddressList<0x55332A, H_CALL>, PRIORITY_BEFORE, ArgPickN<CVehicle*, 0>, void(CVehicle*)> vehicleResetAfterRender;
		extern ThiscallEvent<AddressList<0x53EABF, H_CALL>, PRIORITY_BEFORE, ArgPickNone, void()> renderscence;
	}
}
