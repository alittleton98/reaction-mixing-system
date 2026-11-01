
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NexusCommon.h"
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <optional>


BOOL APIENTRY DllMain( HMODULE /*module*/, DWORD reason, LPVOID /*reserved*/ )
{
	switch( reason )
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


