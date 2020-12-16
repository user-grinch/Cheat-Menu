#include "pch.h"
#include "NeonAPI.h"

bool NeonAPI::api_installed = false;
HMODULE NeonAPI::hapi = false;

extern "C"
{
	static int(__cdecl *_installneon)(CVehicle*, uchar, uchar) = nullptr;
	static int(__cdecl *_setred)(CVehicle*, uchar) = nullptr;
	static int(__cdecl *_setgreen)(CVehicle*, uchar) = nullptr;
	static int(__cdecl *_setblue)(CVehicle*, uchar) = nullptr;
	static bool(__cdecl *_isneoninstalled)(CVehicle*) = nullptr;
	static bool(__cdecl *_removeneon)(CVehicle*) = nullptr;
}

template <class T>
static inline void InitFun(T &fun, HMODULE lib, const char *name)
{
	fun = (T)GetProcAddress(lib, name);
}

NeonAPI::NeonAPI()
{
	Events::initGameEvent += [this]
	{
		hapi = LoadLibraryW(L"neon_api.asi");
		if (hapi)
		{
			api_installed = true;

			InitFun(_installneon,hapi,"InstallNeon"); 
			InitFun(_setred, hapi, "SetRed");
			InitFun(_setgreen, hapi, "SetGreen");
			InitFun(_setblue, hapi, "SetBlue");
			InitFun(_isneoninstalled, hapi, "IsNeonInstalled");
			InitFun(_removeneon, hapi, "RemoveNeon");
		}
	};
}


NeonAPI::~NeonAPI()
{
}

bool NeonAPI::IsNeonInstalled(CVehicle *veh)
{
	return api_installed && _isneoninstalled(veh);
}

bool NeonAPI::InstallNeon(CVehicle *veh, int red, int green, int blue)
{
	if (api_installed)
	{
		_installneon(veh, 0, 0);
		_setred(veh, red);
		_setgreen(veh, green);
		_setblue(veh, blue);

		return true;
	}
	return false;
}

bool NeonAPI::RemoveNeon(CVehicle *veh)
{
	if (api_installed)
	{
		_removeneon(veh);

		return true;
	}
	return false;
}
