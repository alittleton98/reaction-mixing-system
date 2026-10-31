#include "NexusNetwork.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace nms::network
{
	const char* LibraryName()
	{
		return "nms_network";
	}
}

BOOL APIENTRY DllMain( HMODULE /*module*/, DWORD reason, LPVOID /*reserved*/ )
{
	switch ( reason )
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
