
#include "common.h"
#if NEXUS_PLATFORM_WINDOWS
#include <windows.h>
#include "resources/resource.h"
#endif
#include <cstdlib>
#include "network/public/NexusServer.h"
#include "soundengine/public/NexusSoundEngine.h"
#include "common/public/NexusCore.h"


bool CompileScripts()
{
	// Placeholder for script compilation logic
	// In a real implementation, this function would compile the necessary scripts
	// and return true if successful, or false if there was an error.
	return true;
}

// Platform independent body of the CLI
static int RunNexusCLI()
{
	printf( "Nexus Mixing Desk Command Line Interface \nalittl3ton13\nVersion: %s\n===========================================\n", NMS_VERSION_STRING );

	// Init core
	NexusCore* core = NexusCore::Get();
	if( !core )
	{
		WARNING_LOG( "Failed to get NexusCore instance\n" );
		return -1;
	}
	if( !core->InitializeCore( "" ) )
	{
		WARNING_LOG( "Failed to initialize NexusCore\n" );
	}


	// Init other modules
	NexusSoundEngine* soundEngine = NexusSoundEngine::Get();
	if( !soundEngine )
	{
		ERROR_LOG( "Failed to get NexusSoundEngine instance\nRequired for core initialization" );
		return -1;
	}
	if( !soundEngine->LoadScriptModule<NexusSoundEngine>( soundEngine ) )
	{
		ERROR_LOG( "Failed to Load SoundEngine scripting Module NexusSoundEngine\nRequired for core initialization" );
		return -1;
	}
#if !NEXUS_WITH_ASIO
	// The module is linked on every platform but ASIO is compiled out here, so there is no audio device I/O
	printf( "Note: ASIO is not supported on this platform. SoundEngine audio device I/O is disabled\n" );
#endif

	// Init core script module. This must happen last because the script module relies on the others
	if( !core->InitializeScriptModule() )
	{
		WARNING_LOG( "Failed to initialize Core script module\n" );
		return -1;
	}

	std::string message = "";
	while( message != "exit" )
	{
		std::cout << "> ";
		if( !std::getline( std::cin, message ) )
		{
			break; // stdin closed (EOF)
		}

		std::string host = message.substr( 0, message.find( ' ' ) ); // server or core
		if( host == "server" )
		{
			// run server command 
		}
		else if( host == "core" )
		{
			core->CallScriptFunction( message.substr( message.find( ' ' ) + 1 ) );
		}

	}
	DISPLAY_LOG( "Shutting down Mixing Desk" );
	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	return 0;
}

#if NEXUS_PLATFORM_WINDOWS
int APIENTRY wWinMain( _In_ HINSTANCE /*instance*/, _In_opt_ HINSTANCE /*prevInstance*/,
					   _In_ LPWSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/ )
{
	AllocConsole();
	SetConsoleTitleW( L"Nexus Mixing System CLI" );
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );
	return RunNexusCLI();
}
#else
int main( int /*argc*/, char** /*argv*/ )
{
	return RunNexusCLI();
}
#endif
