
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ReactionCommon.h"

namespace rms::common
{
	const char* LibraryName()
	{
		return "rms_common";
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

extern "C"
{
	bool REACTIONCOMMON_API __cdecl ReactionCommon_Initialize()
	{
		// Initialization code for the library can be added here if needed
		return false; // Return true to indicate successful initialization	
	}
	bool REACTIONCOMMON_API __cdecl ReactionCommon_Deinitialize()
	{
		// Cleanup code for the library can be added here if needed
		return true; // Return true to indicate successful deinitialization
	}
}