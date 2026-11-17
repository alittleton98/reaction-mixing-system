#pragma once
#include "NexusCommon.h"

#ifndef NEXUSCORE_API
#define NEXUSCORE_API NEXUS_EXPORT
#endif

// The core is the module loaded alongside the desktop application. 
class NexusCore : public NexusScriptModule
{
public:
	static NexusCore globalCore;
	static NexusCore* Get() { return &NexusCore::globalCore; };
public:

	bool InitializeCore( std::string CoreScriptPath );
	bool ShutdownCore();
	// Load the user's script module that defines how the core behaves and its setup configuration
	bool InitializeScriptModule() override;
	bool ShutdownScriptModule() override;
	// compile the for the core
	bool CompileCoreScript();
	bool IsInitialized() const
	{
		return m_bInitialized;
	}

	void CallScriptFunction( std::string FunctionName );
private:
	bool m_bInitialized = false;
	bool m_bInterpreterInitialized = false;
	std::string m_CoreScriptPath = "";
	python::module_ m_CoreModule;
};

MODULE_API_START( NexusCoreAPI )
APPLICATION_API_START
bool NEXUSCORE_API NexusCore_InitializeCore( const char* pathName );
bool NEXUSCORE_API NexusCore_ShutdownCore();
// Manually recompiles the Core script
bool NEXUSCORE_API NexusCore_CompileCoreScript();
void NEXUSCORE_API NexusCore_AttachConsoleOutput();
APPLICATION_API_END
MODULE_API_END