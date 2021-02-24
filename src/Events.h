#pragma once

namespace plugin
{
	namespace Events
	{
		extern ThiscallEvent<AddressList<0x55332A, H_CALL>, PRIORITY_BEFORE, ArgPickN<CVehicle*, 0>, void(CVehicle*)> vehicleResetAfterRender;
	}
}
