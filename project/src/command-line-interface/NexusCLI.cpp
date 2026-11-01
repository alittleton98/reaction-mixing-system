
#include <windows.h>
#include "resources/resource.h"
#include "stdlib.h"
#include "common.h"
#include "common/public/NexusServer.h"
#include "soundengine/public/NexusSoundEngine.h"
#include "common/public/NexusClient.h"


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

	WARNING_LOG( "Nexus Mixing Desk Command Line Interface \nalittl3ton13\nVersion: %s\n===========================================\n\n", NMS_VERSION_STRING );

	NexusClient* client = NexusClient::Get();
	if( !client )
	{
		WARNING_LOG( "Failed to get NexusClient instance\n" );
		return -1;
	}
	if( !client->InitializeClient( "" ) )
	{
		WARNING_LOG( "Failed to initialize NexusClient\n" );
	}

	if( !client->InitializeScriptModule() )
	{
		WARNING_LOG( "Failed to initialize Client script module\n" );
		return -1;
	}

	string message = "";
	while( message != "exit" )
	{
		cout << "> ";
		getline( cin, message );

		string host = message.substr( 0, 6 ); // server or client. Same number of letters
		if( host == "server" )
		{
			// run server command 
		}
		else if( host == "client" )
		{
			client->CallScriptFunction( message.substr( 7 ) );
		}

	}
	DISPLAY_LOG( "Shutting down Mixing Desk" );
	this_thread::sleep_for( chrono::milliseconds( 500 ) );

	return 0;
}