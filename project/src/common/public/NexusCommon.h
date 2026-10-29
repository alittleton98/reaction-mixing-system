// Nexus Common is the module for loading and running the 
#pragma once
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "common.h"

namespace python = pybind11;

#define INITIALIZE_PYTHON_INTERPRETER python::initialize_interpreter()
#define FINALIZE_PYTHON_INTERPRETER python::finalize_interpreter()
#define START_PYTHON if (!Py_IsInitialized) { printf("%s: Python script section trying to execute without an interpreter running\n", __func__); } else { optional<python::scoped_interpreter> guard; python::gil_scoped_acquire gInterpreterLock;
#define END_PYTHON }; python::module_::import("sys").attr("stdout").attr("flush")();
#define PEEK_PYTHON_LOG python::module_::import("sys").attr("stdout").attr("flush")();
#define SCRIPT_SANITY_CHECK "OnSanityCheck"
#define MODULE_API_START(name) namespace name {
#define MODULE_API_END }
#define APPLICATION_API_START extern "C" {
#define APPLICATION_API_END }
#define SCRIPTING_API_START(name, description) PYBIND11_EMBEDDED_MODULE(name, pythonModule){ pythonModule.doc() = description;
#define SCRIPTING_API_END }

using namespace std;
class NexusScriptInterpreter;

class NexusScriptModule
{
public:
	NexusScriptModule() = default;
	virtual ~NexusScriptModule() = default;
	virtual bool InitializeScriptModule() = 0;
	virtual bool ShutdownScriptModule() = 0;
	template <typename T>
	bool LoadScriptModule( T* ScriptModule );
	template <typename T>
	bool GeneratePythonStubs( T* ScriptModule );
	template <typename T>
	bool CompileModule( T* ScriptModule );
	bool GetIsModuleImported() { return isModuleImported; }

private:
	bool isModuleImported = false;
	template <typename T>
	bool CanPythonImportModule( T* ScriptModuleObject );
	// The API Module imported by the users python script
	python::module_ m_SelfPythonModule;
	// The user's python script. These are called from the embedded interpreter
	python::module_ m_UserPythonModule;
};


inline python::object RunPythonEvent( const char* EventName, const python::module_ ScriptModule )
{
	if( !Py_IsInitialized() )
	{
		return python::none();
	}
	python::gil_scoped_acquire gInterpreterLock;
	if( !python::hasattr( ScriptModule, EventName ) )
	{
		return python::none();
	}
	python::object result;
	try
	{
		result = ScriptModule.attr( EventName );
	}
	catch( exception& exception )
	{
		std::cerr << "Error running python event '" << EventName << "': " << exception.what() << std::endl;
	}
	return result;
}
#define SCRIPT_EVENT(EventName, ScriptModule) RunPythonEvent(EventName, ScriptModule)

template<typename T>
inline T RunPythonEventWithReturnValue( const char* EventName, const python::module_ ScriptModule )
{
	T retValue = 0;
	if( !Py_IsInitialized() )
	{
		return retValue;
	}
	python::gil_scoped_acquire gInterpreterLock;
	if( !python::hasattr( ScriptModule, EventName ) )
	{
		return retValue;
	}
	python::object result = python::none();
	try
	{
		result = ScriptModule.attr( EventName );
		retValue = result.cast<T>();
	}
	catch( exception& exception )
	{
		std::cerr << "Error running python event '" << EventName << "': " << exception.what() << std::endl;
	}

	return retValue = result.cast<T>();
}
#define SCRIPT_EVENT_RET_VALUE(T, EventName, ScriptModule) RunPythonEventWithReturnValue<T>(EventName, ScriptModule)

inline python::object RunStartEventForFunction( const char* FunctionName, const python::module_ UserScriptModule )
{
	string eventName = string( "On_" ) + FunctionName + "_Start";
	return RunPythonEvent( eventName.c_str(), UserScriptModule );
}
#define SCRIPT_ON_FUNCTION_START(FunctionName, UserScriptModule) RunStartEventForFunction(FunctionName, UserScriptModule)

inline python::object RunEndEventForFunction( const char* FunctionName, const python::module_ UserScriptModule )
{
	string eventName = string( "On_" ) + FunctionName + "_End";
	return RunPythonEvent( eventName.c_str(), UserScriptModule );
}
#define SCRIPT_ON_FUNCTION_END(FunctionName, UserScriptModule) RunEndEventForFunction(FunctionName, UserScriptModule)

namespace nexus_util
{
	inline string to_lower( std::string s )
	{
		std::transform( s.begin(), s.end(), s.begin(),
						[] ( unsigned char c ) { return std::tolower( c ); } );
		return s;
	}

	inline string to_upper( std::string s )
	{
		std::transform( s.begin(), s.end(), s.begin(),
						[] ( unsigned char c ) { return std::toupper( c ); } );
		return s;
	}

	// MSVC's typeid().name() is prefixed with "class " / "struct "; drop it so only the type name remains
	inline string StripTypeKeyword( string typeName )
	{
		for( const char* keyword : { "class ", "struct " } )
		{
			if( typeName.starts_with( keyword ) )
			{
				typeName.erase( 0, strlen( keyword ) );
			}
		}
		return typeName;
	}
}
