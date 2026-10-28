// Nexus Common is the module for loading and running the 
#pragma once
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

namespace python = pybind11;
using namespace std;

// Identifier form is needed by PYBIND11_EMBEDDED_MODULE; NEXUS_SCRIPT_MODULE_NAME is the quoted form
#ifndef NEXUS_SCRIPT_MODULE_ID
#define NEXUS_SCRIPT_MODULE_ID nms_common
#endif
#define NEXUS_STRINGIFY_IMPL( x ) #x
#define NEXUS_STRINGIFY( x ) NEXUS_STRINGIFY_IMPL( x )
#define NEXUS_SCRIPT_MODULE_NAME NEXUS_STRINGIFY( NEXUS_SCRIPT_MODULE_ID )

class NexusScriptModule
{
public:
	const char* GetModuleName() const
	{
		return NEXUS_SCRIPT_MODULE_NAME;
	}

	bool CanPythonImportModule()
	{
		python::scoped_interpreter guard{}; // Start the Python interpreter
		try
		{
			python::module_ pModule = python::module_::import( NEXUS_SCRIPT_MODULE_NAME );
		}
		catch ( const exception& e )
		{
			std::cerr << "Error importing module '" << NEXUS_SCRIPT_MODULE_NAME << "': " << e.what() << std::endl;
			return false;
		}
		return true;
	}

	bool LoadScriptModule()
	{
		return CanPythonImportModule();
	}


	virtual bool InitializeModule() = 0;
	virtual bool ShutdownModule() = 0;
};

