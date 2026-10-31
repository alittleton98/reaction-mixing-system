#include "NexusClient.h"
#include <filesystem>
#include <cstdio>
#include <iostream>

NexusClient NexusClient::globalClient;

// Initialize the client
bool NexusClient::InitializeClient( string ClientScriptPath )
{
	if( !filesystem::exists( ClientScriptPath ) )
	{
		WARNING_LOG( "No user client script provided. Loading default" );

		if( !filesystem::exists( DEFAULT_SCRIPT_PATH ) )
		{
			ERROR_LOG( "No default script available. Cannot initialize" );
			return false;
		}
		else
		{
			ClientScriptPath = DEFAULT_SCRIPT_PATH;
		}
	}
	m_ClientScriptPath = ClientScriptPath;
	python::initialize_interpreter();
	return true;
}
bool NexusClient::ShutdownClient()
{
	python::finalize_interpreter();

	return true;
}
// Load the user's script module that defines how the client behaves and its setup configuration
bool NexusClient::InitializeScriptModule()
{
	if( m_ClientScriptPath.length() < 1 )
	{
		ERROR_LOG( "No Client script provided. This should at least be the default path" );
		return false;
	}

	START_PYTHON;
	if( !PyGILState_Check() )
		PyGILState_Ensure();
	python::module_ machinery;
	python::module_ util;
	python::module_ sys;
	try
	{
		python::module_ machinery = python::module_::import( "importlib.machinery" );
		python::module_ util = python::module_::import( "importlib.util" );
		python::module_ sys = python::module_::import( "sys" );
	}
	catch( exception& exception )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Client" );
		return false;
	}

	if( machinery == python::none() || util == python::none() || sys == python::none() )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Client" );
		return false;
	}
	string moduleName = nexus_util::GetFileNameWithoutExtension( m_ClientScriptPath );
	string adjusted = nexus_util::to_lower( moduleName );
	const char* adjustedModuleName = adjusted.c_str();

	try
	{

		python::object loader = machinery.attr( "SourceFileLoader" )( adjustedModuleName, m_ClientScriptPath );
		python::object spec = util.attr( "spec_from_loader" )( adjustedModuleName, loader );
		m_ClientModule = util.attr( "module_from_spec" )( spec );
	}
	catch( exception& exception )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Client" );
		return false;
	}

	END_PYTHON;
	return true;
}
bool NexusClient::ShutdownScriptModule()
{
	return true;
}
// compile the for the client
bool NexusClient::CompileClientScript()
{


	return true;
}
bool NexusClient::InitializeInterpreter()
{
	return true;
}
bool NexusClient::FinalizeInterpreter()
{
	return true;
}

APPLICATION_API_START
bool NEXUSCLIENT_API NexusClient_InitializeClient( const char* pathName )
{
#if DEBUG_CONFIG
	DISPLAY_LOG( "Inititializing client" );
#endif

	NexusClient* client = NexusClient::Get();
	if( !client )
	{
		WARNING_LOG( "NexusClient::Get returned null!" );
		return false;
	}

	if( !client->InitializeClient( pathName ) )
	{
		WARNING_LOG( "NexusClient failed base initialization" );
		return false;
	}

	if( !client->InitializeScriptModule() )
	{
		WARNING_LOG( "NexusClient failed to initialize its script module" );
		return false;
	}
	return true;
}
bool NEXUSCLIENT_API NexusClient_ShutdownClient()
{
	return true;
}
bool NEXUSCLIENT_API NexusClient_CompileClientScript()
{
	return true;
}
void NEXUSCLIENT_API NexusClient_AttachConsoleOutput()
{
	FILE* f;
	freopen_s( &f, "CONOUT$", "w", stdout );
	freopen_s( &f, "CONOUT$", "w", stderr );

	// Without this, native output can show up late or out of order relative to C# output
	setvbuf( stdout, nullptr, _IONBF, 0 );
	setvbuf( stderr, nullptr, _IONBF, 0 );

	// Reset iostreams in case they were used before the console existed
	std::cout.clear();
	std::cerr.clear();
}
APPLICATION_API_END