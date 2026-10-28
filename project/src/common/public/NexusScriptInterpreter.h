#pragma once
#include "NexusCommon.h"
#include <thread>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace python = pybind11;

thread* g_InterpreterThread = nullptr;
string g_LoadedScriptPath = "";
python::object* g_ScriptModule = nullptr;



static bool LoadScriptModule( const string& scriptPath )
{
	
}
