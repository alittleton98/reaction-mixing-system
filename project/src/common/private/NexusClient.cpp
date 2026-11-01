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
	// A GUI-subsystem executable has no std handles when Python starts, so sys.stdout/stderr come up as None.
	// Point them at the console so print() and flush() work.
	try
	{
		python::module_ pySys = python::module_::import( "sys" );
		python::module_ pyBuiltins = python::module_::import( "builtins" );
		for( const char* streamName : { "stdout", "stderr" } )
		{
			if( pySys.attr( streamName ).is_none() )
			{
				pySys.attr( streamName ) = pyBuiltins.attr( "open" )( "CONOUT$", "w", python::arg( "buffering" ) = 1, python::arg( "encoding" ) = "utf-8" );
			}
		}
	}
	catch( exception& e )
	{
		fprintf( stderr, "> Failed to attach Python stdout/stderr to the console: %s\n", e.what() );
	}
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
	python::module_ machinery;
	python::module_ util;
	python::module_ sys;
	try
	{
		machinery = python::module_::import( "importlib.machinery" );
		util = python::module_::import( "importlib.util" );
		sys = python::module_::import( "sys" );
	}
	catch( exception& exception )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Client" );
		return false;
	}

	if( machinery.is_none() || util.is_none() || sys.is_none() )
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
		m_SelfPythonModule = util.attr( "module_from_spec" )( spec );
		loader.attr( "exec_module" )( m_SelfPythonModule );
	}
	catch( exception& exception )
	{
		ERROR_LOG( exception.what() );
		return false;
	}

	try
	{
		if( !python::hasattr( m_SelfPythonModule, SCRIPT_SANITY_CHECK ) )
		{
			ERROR_LOG( "Client script module does not have the required OnSanityCheck function" );
			return false;
		}
		python::object result = m_SelfPythonModule.attr( SCRIPT_SANITY_CHECK )( );
		if( result.is_none() )
		{
			ERROR_LOG( "Failed to run sanity check for Client script module" );
			return false;
		}
		else if( !result.cast<bool>() )
		{
			WARNING_LOG( "Sanity check for Client script module returned false" );
			return false;
		}
	}
	catch( exception& exception )
	{
		ERROR_LOG( "Exception calling the OnSanityCheck function in the Client script module" );
		return false;
	}
	PEEK_PYTHON_LOG;
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
	if( m_ClientScriptPath.length() < 1 )
	{
		ERROR_LOG( "No Client script provided. This should at least be the default path" );
		return false;
	}

	START_PYTHON;
	python::module_ machinery;
	python::module_ util;
	python::module_ sys;
	try
	{
		machinery = python::module_::import( "importlib.machinery" );
		util = python::module_::import( "importlib.util" );
		sys = python::module_::import( "sys" );
	}
	catch( exception& exception )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Client" );
		return false;
	}

	if( machinery.is_none() || util.is_none() || sys.is_none() )
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
		m_SelfPythonModule = util.attr( "module_from_spec" )( spec );
		loader.attr( "exec_module" )( m_SelfPythonModule );
	}
	catch( exception& exception )
	{
		ERROR_LOG( exception.what() );
		return false;
	}

	return true;
}
}

void NexusClient::CallScriptFunction( string FunctionName )
{
	START_PYTHON;
	CompileClientScript();
	if( !python::hasattr( m_SelfPythonModule, FunctionName.c_str() ) )
	{
		WARNING_LOG( format( "Failed to run function {}() in Client script module", FunctionName.c_str() ).c_str() );
	}
	try
	{
		python::object result = m_SelfPythonModule.attr( FunctionName.c_str() )( );
		if( result.is_none() )
		{
			WARNING_LOG( "Module did not return a returnable object/value" );
		}
		else if( !result.cast<bool>() )
		{
			WARNING_LOG( format( "{} in Client script module returned false", FunctionName.c_str() ).c_str() );
		}
	}
	catch( exception& exception )
	{
		WARNING_LOG( format( "Exception calling function {}() in Client script module:{}", FunctionName.c_str(), exception.what() ).c_str() );
	}
	//PEEK_PYTHON_LOG;
	END_PYTHON;
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
SCRIPTING_API_START( nexusclient, "Embedded Python module for the Nexus Client" )
pythonModule.def( "initialize_client", &NexusClientAPI::NexusClient_InitializeClient, python::arg( "ClientScriptPath" ) );
SCRIPTING_API_END