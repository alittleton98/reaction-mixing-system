#define WIN32_LEAN_AND_MEAN

#include "ReactionSoundEngine.h"
#include <windows.h>

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

namespace rms::soundengine
{
	const char* LibraryName()
	{
		return "rms_soundengine";
	}
}
