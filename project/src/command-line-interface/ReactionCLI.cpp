
#include <windows.h>
#include "resources/resource.h"
#include "common.h"

int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
	AllocConsole();
	SetConsoleTitleW( L"Reaction Mixing Server CLI" );
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );
	HWND consoleWindow;
	consoleWindow = GetConsoleWindow();

	printf( "Reaction Mixing Desk Command Line Interface \nalittl3ton13\nVersion: %s\n===========================================\n\n", RMD_VERSION_STRING );

	

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