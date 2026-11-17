#include "NexusCore.h"
#include <filesystem>
#include <cstdio>
#include <iostream>


NexusCore NexusCore::globalCore;

// Initialize the core
bool NexusCore::InitializeCore( std::string CoreScriptPath )
{
	if( !std::filesystem::exists( CoreScriptPath ) )
	{
		WARNING_LOG( "No user core script provided. Loading default" );

		if( !std::filesystem::exists( DEFAULT_SCRIPT_PATH ) )
		{
			ERROR_LOG( "No default script available. Cannot initialize" );
			return false;
		}
		else
		{
			CoreScriptPath = DEFAULT_SCRIPT_PATH;
		}
	}
	m_CoreScriptPath = CoreScriptPath;
	python::initialize_interpreter();
#if NEXUS_PLATFORM_WINDOWS
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
	catch( std::exception& e )
	{
		fprintf( stderr, "> Failed to attach Python stdout/stderr to the console: %s\n", e.what() );
	}
#endif // NEXUS_PLATFORM_WINDOWS
	return true;
}
bool NexusCore::ShutdownCore()
{
	python::finalize_interpreter();

	return true;
}
// Load the user's script module that defines how the core behaves and its setup configuration
bool NexusCore::InitializeScriptModule()
{
	if( m_CoreScriptPath.length() < 1 )
	{
		ERROR_LOG( "No Core script provided. This should at least be the default path" );
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
	catch( std::exception& exception )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Core" );
		return false;
	}

	if( machinery.is_none() || util.is_none() || sys.is_none() )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Core" );
		return false;
	}
	std::string moduleName = nexus_util::GetFileNameWithoutExtension( m_CoreScriptPath );
	std::string adjusted = nexus_util::to_lower( moduleName );
	const char* adjustedModuleName = adjusted.c_str();

	try
	{

		python::object loader = machinery.attr( "SourceFileLoader" )( adjustedModuleName, m_CoreScriptPath );
		python::object spec = util.attr( "spec_from_loader" )( adjustedModuleName, loader );
		m_SelfPythonModule = util.attr( "module_from_spec" )( spec );
		loader.attr( "exec_module" )( m_SelfPythonModule );
	}
	catch( std::exception& exception )
	{
		ERROR_LOG( exception.what() );
		return false;
	}

	try
	{
		if( !python::hasattr( m_SelfPythonModule, SCRIPT_SANITY_CHECK ) )
		{
			ERROR_LOG( "Core script module does not have the required OnSanityCheck function" );
			return false;
		}
		python::object result = m_SelfPythonModule.attr( SCRIPT_SANITY_CHECK )( );
		if( result.is_none() )
		{
			ERROR_LOG( "Failed to run sanity check for Core script module" );
			return false;
		}
		else if( !result.cast<bool>() )
		{
			WARNING_LOG( "Sanity check for Core script module returned false" );
			return false;
		}
	}
	catch( std::exception& exception )
	{
		ERROR_LOG( "Exception calling the OnSanityCheck function in the Core script module" );
		return false;
	}
	PEEK_PYTHON_LOG;
	END_PYTHON;
	return true;
}
bool NexusCore::ShutdownScriptModule()
{
	return true;
}
// compile the for the core
bool NexusCore::CompileCoreScript()
{
	if( m_CoreScriptPath.length() < 1 )
	{
		ERROR_LOG( "No Core script provided. This should at least be the default path" );
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
	catch( std::exception& exception )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Core" );
		return false;
	}

	if( machinery.is_none() || util.is_none() || sys.is_none() )
	{
		ERROR_LOG( "Failed to load import python modules. Cannot Initialize Script module for Core" );
		return false;
	}
	std::string moduleName = nexus_util::GetFileNameWithoutExtension( m_CoreScriptPath );
	std::string adjusted = nexus_util::to_lower( moduleName );
	const char* adjustedModuleName = adjusted.c_str();

	try
	{

		python::object loader = machinery.attr( "SourceFileLoader" )( adjustedModuleName, m_CoreScriptPath );
		python::object spec = util.attr( "spec_from_loader" )( adjustedModuleName, loader );
		m_SelfPythonModule = util.attr( "module_from_spec" )( spec );
		loader.attr( "exec_module" )( m_SelfPythonModule );
	}
	catch( std::exception& exception )
	{
		ERROR_LOG( exception.what() );
		return false;
	}

	return true;
	END_PYTHON
	return false;
}

void NexusCore::CallScriptFunction( std::string FunctionName )
{
	START_PYTHON;
	CompileCoreScript();
	if( !python::hasattr( m_SelfPythonModule, FunctionName.c_str() ) )
	{
		WARNING_LOG( std::format( "Failed to run function {}() in Core script module", FunctionName.c_str() ).c_str() );
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
			WARNING_LOG( std::format( "{} in Core script module returned false", FunctionName.c_str() ).c_str() );
		}
	}
	catch( std::exception& exception )
	{
		WARNING_LOG( std::format( "Exception calling function {}() in Core script module:{}", FunctionName.c_str(), exception.what() ).c_str() );
	}
	//PEEK_PYTHON_LOG;
	END_PYTHON;
}

APPLICATION_API_START
bool NEXUSCORE_API NexusCore_InitializeCore( const char* pathName )
{
#if DEBUG_CONFIG
	DISPLAY_LOG( "Inititializing core" );
#endif

	NexusCore* core = NexusCore::Get();
	if( !core )
	{
		WARNING_LOG( "NexusCore::Get returned null!" );
		return false;
	}

	if( !core->InitializeCore( pathName ) )
	{
		WARNING_LOG( "NexusCore failed base initialization" );
		return false;
	}

	if( !core->InitializeScriptModule() )
	{
		WARNING_LOG( "NexusCore failed to initialize its script module" );
		return false;
	}
	return true;
}
bool NEXUSCORE_API NexusCore_ShutdownCore()
{
	return true;
}
bool NEXUSCORE_API NexusCore_CompileCoreScript()
{
	return true;
}
void NEXUSCORE_API NexusCore_AttachConsoleOutput()
{
#if NEXUS_PLATFORM_WINDOWS
	FILE* f;
	freopen_s( &f, "CONOUT$", "w", stdout );
	freopen_s( &f, "CONOUT$", "w", stderr );
#endif

	// Without this, native output can show up late or out of order relative to C# output
	setvbuf( stdout, nullptr, _IONBF, 0 );
	setvbuf( stderr, nullptr, _IONBF, 0 );

	// Reset iostreams in case they were used before the console existed
	std::cout.clear();
	std::cerr.clear();
}
APPLICATION_API_END
SCRIPTING_API_START( nexuscore, "Embedded Python module for the Nexus Core" )
pythonModule.def( "initialize_core", &NexusCoreAPI::NexusCore_InitializeCore, python::arg( "CoreScriptPath" ) );
SCRIPTING_API_END