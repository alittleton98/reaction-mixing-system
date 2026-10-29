#include "NexusClient.h"

// Initialize the client
bool NexusClient::InitializeClient( string ClientScriptPath )
{
	m_ClientScriptPath = ClientScriptPath;
	return true;
}
bool NexusClient::ShutdownClient()
{
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
	string moduleName = nexus_util::StripTypeKeyword( typeid( *this ).name() );
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