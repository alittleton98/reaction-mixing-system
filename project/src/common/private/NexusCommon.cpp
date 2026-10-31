
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "NexusCommon.h"
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <optional>




template <typename T>
bool NexusScriptModule::CanPythonImportModule( T* ScriptModuleObject )
{
	string moduleName = StripTypeKeyword( typeid( *ScriptModuleObject ).name() );
	string adjusted = nexus_util::to_lower( moduleName );
	const char* adjustedModuleName = adjusted.c_str();

	try
	{
		python::module_ pModule = python::module_::import( adjustedModuleName );
	}
	catch( const exception& e )
	{
		std::cerr << "Error importing module '" << adjustedModuleName << "': " << e.what() << std::endl;
		return false;
	}
	return true;
}

template <typename T>
bool NexusScriptModule::LoadScriptModule( T* ScriptModuleObject )
{
	isModuleImported = CanPythonImportModule( ScriptModuleObject );
	return isModuleImported;
}

template <typename T>
bool NexusScriptModule::GeneratePythonStubs( T* ScriptModuleObject )
{
	// Type should be child of NexusScriptModule but we don't use it so we can use the name from the child class
	NexusScriptModule* sanity = static_cast< NexusScriptModule* >( ScriptModuleObject );
	if( !sanity )
	{
		return false;
	}

	string moduleName = StripTypeKeyword( typeid( *ScriptModuleObject ).name() );
	string adjustedModuleName = nexus_util::to_lower( moduleName );

	START_PYTHON;
	python::module_ stubgen;
	try
	{
		stubgen = python::module_::import( "pybind11_stubgen" );
		python::list argv;
		argv.append( adjustedModuleName );
		argv.append( "-o" );
		argv.append( SCRIPT_OUTPUT_DIRECTORY );
		stubgen.attr( "main" )( argv );
	}
	catch( exception& exception )
	{
		std::cerr << "Error generating stubs '" << adjustedModuleName << "': " << exception.what() << std::endl;
		return false;
	}
	END_PYTHON;
	return true;
}

template <typename T>
bool NexusScriptModule::CompileModule( T* ScriptModule )
{
// Type should be child of NexusScriptModule but we don't use it so we can use the name from the child class
	NexusScriptModule* sanity = static_cast< NexusScriptModule* >( ScriptModule );
	if( !sanity )
	{
		return false;
	}

	string moduleName = StripTypeKeyword( typeid( *ScriptModule ).name() );
	string adjustedModuleName = nexus_util::to_lower( moduleName );

	START_PYTHON;
	try
	{
		if( m_SelfPythonModule == python::none() )
		{
			m_SelfPythonModule = python::module_::import( adjustedModuleName );
		}
		if( m_SelfPythonModule == python::none() )
		{
			return false;
		}

		python::object result;
		result = m_SelfPythonModule.attr( SCRIPT_SANITY_CHECK );
	}
	catch( exception& exception )
	{
		std::cerr << "Error generating stubs '" << adjustedModuleName << "': " << exception.what() << std::endl;
		return false;
	}
	END_PYTHON;
}


BOOL APIENTRY DllMain( HMODULE /*module*/, DWORD reason, LPVOID /*reserved*/ )
{
	switch( reason )
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


