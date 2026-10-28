
#include <windows.h>
#include "resources/resource.h"
#include "stdlib.h"
#include "common.h"
#include "common/public/NexusServer.h"


bool CompileScripts()
{
	// Placeholder for script compilation logic
	// In a real implementation, this function would compile the necessary scripts
	// and return true if successful, or false if there was an error.
	return true;
}

int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
	AllocConsole();
	SetConsoleTitleW( L"Nexus Mixing System CLI" );
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );
	HWND consoleWindow;
	consoleWindow = GetConsoleWindow();
	int argc = __argc;
	wchar_t** argv = __wargv;   // includes the program path as argv[0]

	printf( "Nexus Mixing Desk Command Line Interface \nalittl3ton13\nVersion: %s\n===========================================\n\n", NMS_VERSION_STRING );


	string message = "";
	while ( message != "exit" )
	{
		printf( "> " );
		getline( cin, message );
	}
	printf( "Shutting down Mixing Desk" );
	this_thread::sleep_for( chrono::milliseconds( 500 ) );

	return 0;
}