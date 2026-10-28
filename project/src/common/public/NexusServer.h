#pragma once
#include "NexusCommon.h"


#ifndef NEXUSSERVER_API
#define NEXUSSERVER_API __declspec(dllexport) __cdecl
#endif

class NexusServer : public NexusScriptModule
{
public:

public:
	bool Initialize();
	bool Shutdown();
	bool CompileScripts();
	bool InitializeInterpreter();
	bool FinalizeInterpreter();
	bool IsInitialized() const
	{
		return m_bInitialized;
	}
private:
	bool m_bInitialized = false;
	bool m_bInterpreterInitialized = false;

};

extern "C"
{
	//
	bool NEXUSSERVER_API InitializeServer( NexusServer& Server );
	bool NEXUSSERVER_API CompileScripts( NexusServer* Server );
	bool NEXUSSERVER_API ShutdownServer( NexusServer* Server );
}

