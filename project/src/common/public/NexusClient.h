#pragma once
#include "NexusCommon.h"

#ifndef NEXUSCLIENT_API
#define NEXUSCLIENT_API __declspec(dllexport) __cdecl
#endif

// The client is the module loaded alongside the desktop application. 
class NexusClient : public NexusScriptModule
{
public:
	static NexusClient globalClient;
	static NexusClient* Get() { return &NexusClient::globalClient; };
public:
	// Initialize the client
	bool InitializeClient( string ClientScriptPath );
	bool ShutdownClient();
	// Load the user's script module that defines how the client behaves and its setup configuration
	bool InitializeScriptModule() override;
	bool ShutdownScriptModule() override;
	// compile the for the client
	bool CompileClientScript();
	bool InitializeInterpreter();
	bool FinalizeInterpreter();
	bool IsInitialized() const
	{
		return m_bInitialized;
	}

private:
	bool m_bInitialized = false;
	bool m_bInterpreterInitialized = false;
	string m_ClientScriptPath = "";
	python::module_ m_ClientModule;
};

MODULE_API_START( NexusClientAPI )
APPLICATION_API_START
bool NexusClient_InitializeClient( string ClientScriptPath );
bool NexusClient_ShutdownClient();
bool NexusClient_CompileClientScript();
APPLICATION_API_END
MODULE_API_END