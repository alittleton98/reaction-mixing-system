
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

const char* SanityCheck = "E:\\Dev\\reaction-mixing-desk\\project\\src\\common\\public\\Sanity.nxs";

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


