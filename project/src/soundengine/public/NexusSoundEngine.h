#pragma once
//#pragma region WWISE_INCLUDES
//#include "AK/SoundEngine/Common/AkSoundEngine.h"
//#include "AK/SoundEngine/Common/AkMemoryMgr.h"
//#include "AK/SoundEngine/Common/AkMemoryMgrModule.h"
//#include "AK/SoundEngine/Common/IAkStreamMgr.h"
//#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
//#include "AK/Comm/AkCommunication.h"
//#include "AK/IBytes.h"
//#if WIN32
//#include "SoundEngine/Win32/AkDefaultIOHookDeferred.h"
//#else
//#include "SoundEngine/POSIX/AkDefaultIOHookDeferred.h"
//#endif
//#pragma endregion

#pragma region VST3_SDK_INCLUDES
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include "pluginterfaces/vst/vsttypes.h"
#pragma endregion

#pragma region ASIO_INCLUDES
#include "asio.h"
#include "asiosys.h"
#include "asiodrivers.h"
#pragma endregion

#include "RtMidi.h"

#include "common/public/NexusCommon.h"
#include "common.h"

#ifndef NEXUSSOUNDENGINE_API
#define NEXUSSOUNDENGINE_API __declspec(dllexport) __cdecl
#endif

using namespace Steinberg;

namespace NexusSoundEngineCallbacks
{
	void OnSampleRateChange( ASIOSampleRate SampleRate );
	size_t GetBytesPerSample( ASIOSampleType sampleType );
	ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess );
	void OnBufferSwitch( long doubleBufferIndex, ASIOBool directProcess );
	long OnAsioMessage( long Selector, long Value, void* Message, double* opt );
}

#define MAX_INPUT_CHANNELS 512
#define MAX_OUTPUT_CHANNELS 512
#define DEFAULT_SAMPLERATE 48000
#define ASIO_DEVICE_DEFAULT_CHANNELS 512
#define M_PI 3.14159265358979323846

enum class EChannelConfiguration : unsigned short
{
	MONO = 1,
	STEREO = 2,
	QUAD = 4,
	SURROUND = 6,
	SURROUND_REAR = 8,
	ATMOS = 12
};

namespace ChannelStripColors
{
	const int64_t Default_Blue = 0xFF29A3CC;
	const int64_t Default_Green = 0xFF29CC29;
	const int64_t Default_Red = 0xFFCC2929;
	const int64_t Default_Yellow = 0xFFCCC729;
	const int64_t Default_Orange = 0xFFCC6D29;
	const int64_t Default_Purple = 0xFF8E29CC;
}

class NexusSoundEngine : public Vst::IHostApplication, NexusScriptModule
{
public:
	NexusSoundEngine();
	static NexusSoundEngine globalSoundEngine;
	static NexusSoundEngine* Get() { return &NexusSoundEngine::globalSoundEngine; };

public:
	bool InitializeSoundEngine();
	bool ShutdownSoundEngine();

	bool InitializeScriptModule() override;
	bool ShutdownScriptModule() override;

	// VST Host Interface
	tresult PLUGIN_API getName( Vst::String128 name ) override;
	tresult PLUGIN_API createInstance( TUID cid, TUID _iid, void** obj ) override;

	DECLARE_FUNKNOWN_METHODS

	// Audio Signal 
	bool InitializeASIODevice();
	long InitializeAsioStaticData();
	ASIOError CreateAsioBuffers();
	long GetAsioBufferSize() { return mAsioBufferPreferredSize; };
	ASIOSampleRate GetAsioSampleRate() { return mSampleRate; };
	long GetAsioInputLatency() { return mInputLatency; };
	long GetAsioOutputLatency() { return mOutputLatency; };
	bool LoadSignalChainPlugins();
	/*bool SetupChannels();
	bool SetupMaster();*/

	// Sound Engine Loop
	void SoundEngineLoopFunc( float deltaTime );

// API Controls
protected:
public:
public:
	bool bAsioDeviceInitialized = false;
	bool bSignalChainInitialized = false;

	// ASIO - Primary hardware I/O
	ASIODriverInfo mDriverInfo;
	ASIOCallbacks mAsioCallbacks;
	AsioDrivers* mAsioDriver = 0;
	long mInputChannels = 0;
	long mOutputChannels = 0;
	// ASIOGetBufferSize()
	long mAsioBufferMinSize = 0;
	long mAsioBufferMaxSize = 0;
	long mAsioBufferPreferredSize = 0;
	long mAsioBufferGranularity = 0;
	// ASIOGetSampleRate()
	ASIOSampleRate mSampleRate;
	// ASIOOutputReady()
	bool mOutputReady = false;
	// ASIOGetLatencies ()
	long mInputLatency = 0;
	long mOutputLatency = 0;
	// ASIOCreateBuffers ()
	long mInputBuffers = 0;	// becomes number of actual created input buffers
	long mOutputBuffers = 0;	// becomes number of actual created output buffers
	ASIOBufferInfo mBufferInfos[ MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS ]; // buffer info's
	// ASIOGetChannelInfo()
	ASIOChannelInfo mChannelInfos[ MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS ]; // channel info's
	// The above two arrays share the same indexing, as the data in them are linked together
	// Information from ASIOGetSamplePosition()
	// data is converted to double floats for easier use, however 64 bit integer can be used, too
	double nanoSeconds = 0;
	double samples = 0;
	double tcSamples = 0;	// time code samples

	// bufferSwitchTimeInfo()
	ASIOTime tInfo;			// time info state
	unsigned long sysRefTime = 0;      // system reference time, when bufferSwitch() was called

	// Signal Chain - intermediate signal processing
	VST3::Hosting::Module::Ptr vstModule_ChannelStrip;
	VST3::Hosting::Module::Ptr vstModule_Mastering;
	VST3::Hosting::Module::Ptr vstModule_Meter;
	string ChannelStripVSTPath = "";
	string MasteringVSTPath = "";
	string MeterVSTPath = "";

	Vst::HostApplication context;

};

MODULE_API_START( NexusSoundEngineAPI );
APPLICATION_API_START;
// Application will init the sound engine
bool NEXUSSOUNDENGINE_API NexusSoundEngine_InitializeSoundEngine();
// Application will tear down the sound engine
bool NEXUSSOUNDENGINE_API NexusSoundEngine_FinalizeSoundEngine();
void NEXUSSOUNDENGINE_API NexusSoundEngine_SetChannelStripPlugin( string PluginPath );
void NEXUSSOUNDENGINE_API NexusSoundEngine_SetMasteringPlugin( string PluginPath );
void NEXUSSOUNDENGINE_API NexusSoundEngine_SetMeterPlugin( string PluginPath );
APPLICATION_API_END;
SCRIPTING_API_START( NexusSoundEngineAPI, "API for the SoundEngine module of the Nexus Mixing System" )
SCRIPTING_API_END
MODULE_API_END;