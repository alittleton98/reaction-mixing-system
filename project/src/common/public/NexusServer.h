#pragma once
#include "NexusCommon.h"

#ifndef NEXUSSERVER_API
#define NEXUSSERVER_API __declspec(dllexport) __cdecl
#endif

// The server instance is the data manag
class NexusServer : public NexusScriptModule
{
public:
	static NexusServer globalServer;
	static NexusServer* Get() { return &NexusServer::globalServer; };

public:
	bool InitializeServer();
	bool ShutdownServer();
	bool InitializeScriptModule() override;
	bool ShutdownScriptModule() override;
	bool CompileScripts();
	bool InitializeInterpreter();
	bool FinalizeInterpreter();
	bool LoadUserScriptModule( const char* scriptName );
	bool IsInitialized() const
	{
		return m_bInitialized;
	}
private:
	bool m_bInitialized = false;
	bool m_bInterpreterInitialized = false;
};
