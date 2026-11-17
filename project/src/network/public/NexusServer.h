#pragma once
#include "common/public/NexusCommon.h"

#ifndef NEXUSSERVER_API
#define NEXUSSERVER_API NEXUS_EXPORT
#endif

struct NxsRemoteClient
{
	int ClientID;
	std::string ClientName;
	std::string ClientIP;
	int ClientPort;
};

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

	bool ConnectToMasterClient( const char* MasterClientIP, int MasterClientPort );


private:
	bool m_bInitialized = false;
	bool m_bInterpreterInitialized = false;
	NxsRemoteClient* m_MasterClient;
	std::vector<NxsRemoteClient*> m_RemoteClients;
	std::thread m_ConnectionLobbyThread;
};
