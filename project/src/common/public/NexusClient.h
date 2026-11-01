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

	void CallScriptFunction( string FunctionName )
	{
		START_PYTHON
			if( !python::hasattr( m_UserPythonModule, FunctionName.c_str() ) )
			{
				WARNING_LOG( format( "Failed to run function %s in Client script module", FunctionName.c_str() ).c_str() );

			}
		python::object result = m_SelfPythonModule.attr( FunctionName.c_str() )( );
		if( result.is_none() )
		{
			WARNING_LOG( "Module did not return a returnable object/value" );
		}
		else if( !result.cast<bool>() )
		{
			WARNING_LOG( format( "%s in Client script module returned false", FunctionName.c_str() ).c_str() );
		}

		PEEK_PYTHON_LOG;
		END_PYTHON;
	}
private:
	bool m_bInitialized = false;
	bool m_bInterpreterInitialized = false;
	string m_ClientScriptPath = "";
	python::module_ m_ClientModule;
};

MODULE_API_START( NexusClientAPI )
APPLICATION_API_START
bool NEXUSCLIENT_API NexusClient_InitializeClient( const char* pathName );
bool NEXUSCLIENT_API NexusClient_ShutdownClient();
// Manually recompiles the Client script
bool NEXUSCLIENT_API NexusClient_CompileClientScript();
void NEXUSCLIENT_API NexusClient_AttachConsoleOutput();
APPLICATION_API_END
MODULE_API_END