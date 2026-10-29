#include "common/public/NexusServer.h"

PYBIND11_EMBEDDED_MODULE( nms_server, pyModule )
{}

NexusServer NexusServer::globalServer;

bool NexusServer::InitializeScriptModule()
{
	// Placeholder for server initialization logic
	// In a real implementation, this function would set up the server environment,
	// load necessary resources, and prepare the server for operation.
	m_bInitialized = true;
	return m_bInitialized;
}

bool NexusServer::ShutdownScriptModule()
{
	// Placeholder for server shutdown logic
	// In a real implementation, this function would clean up resources,
	// save state if necessary, and gracefully shut down the server.
	m_bInitialized = false;
	return !m_bInitialized;
}

bool NexusServer::CompileScripts()
{
	// Placeholder for script compilation logic
	// In a real implementation, this function would compile the necessary scripts
	// and return true if successful, or false if there was an error.
	return true;
}

bool NexusServer::InitializeInterpreter()
{
	if ( m_bInterpreterInitialized )
	{
		return true;
	}

	python::initialize_interpreter();
	m_bInterpreterInitialized = true;

	python::module_ sys = python::module_::import( "sys" );
	sys.attr( "path" ).attr( "insert" )( 0, SCRIPT_OUTPUT_DIRECTORY );
	return true;
}
bool NexusServer::FinalizeInterpreter()
{
	if ( m_bInterpreterInitialized )
	{
		python::finalize_interpreter();
		m_bInterpreterInitialized = false;
	}
	return true;
}
bool NexusServer::LoadUserScriptModule( const char* scriptName )
{
	bool bIsLoaded = false;
	python::module_ scriptModule;
	if ( !m_bInterpreterInitialized )
	{
		return false;
	}

	try
	{
		scriptModule = python::module_::import( scriptName );
	}
	catch ( exception& e )
	{
		std::cerr << "Error importing module '" << scriptName << "': " << e.what() << std::endl;
		return false;
	}

	try
	{
		python::object result = scriptModule.attr( "OnScriptModuleLoaded" )( );
		bIsLoaded = result.cast<bool>();
	}
	catch ( exception& e )
	{
		std::cerr << "Error calling OnScriptModuleLoaded in '" << scriptName << "': " << e.what() << std::endl;
		return false;
	}

	return bIsLoaded;
}

extern "C"
{
	bool NEXUSSERVER_API InitializeServer()
	{
		NexusServer* server = NexusServer::Get();
		return server->InitializeScriptModule() && server->InitializeInterpreter();
	}
	bool NEXUSSERVER_API ShutdownServer()
	{
		NexusServer* server = NexusServer::Get();
		if ( !server->IsInitialized() )
		{
			return false;
		}
		bool bSuccess = server->FinalizeInterpreter();
		bSuccess = server->ShutdownScriptModule() && bSuccess;
		return bSuccess;
	}
};
