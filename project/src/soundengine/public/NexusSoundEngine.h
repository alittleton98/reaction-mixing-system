#pragma once
#pragma region WWISE_INCLUDES
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "AK/SoundEngine/Common/AkMemoryMgrModule.h"
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/Comm/AkCommunication.h"
#include "AK/IBytes.h"
#if WIN32
#include "SoundEngine/Win32/AkDefaultIOHookDeferred.h"
#else
#include "SoundEngine/POSIX/AkDefaultIOHookDeferred.h"
#endif
#pragma endregion

#pragma region VST3_SDK_INCLUDES
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include "pluginterfaces/vst/vsttypes.h"
#pragma endregion

#include "common.h"

#if NEXUS_WITH_ASIO
#pragma region ASIO_INCLUDES
#include "asio.h"
#include "asiosys.h"
#include "asiodrivers.h"
#pragma endregion
#endif // NEXUS_WITH_ASIO

#include "RtMidi.h"

#include "common/public/NexusCommon.h"
#include "NxsChannelStrip.h"
#include "common.h"

#ifndef NEXUSSOUNDENGINE_API
#define NEXUSSOUNDENGINE_API NEXUS_EXPORT
#endif

using namespace Steinberg;

#if NEXUS_WITH_ASIO
namespace NexusSoundEngineCallbacks
{
	void OnSampleRateChange( ASIOSampleRate SampleRate );
	size_t GetBytesPerSample( ASIOSampleType sampleType );
	ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess );
	void OnBufferSwitch( long doubleBufferIndex, ASIOBool directProcess );
	long OnAsioMessage( long Selector, long Value, void* Message, double* opt );
}
#endif // NEXUS_WITH_ASIO

#define MAX_INPUT_CHANNELS 512
#define MAX_OUTPUT_CHANNELS 512
#define DEFAULT_SAMPLERATE 48000
#define ASIO_DEVICE_DEFAULT_CHANNELS 512
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

// Sound Engine class that serves as the host for the VST3 plugins, controller for the ASIO device, and Wwise Sound Engine API
class NexusSoundEngine : public Vst::IHostApplication, public NexusScriptModule
{
public:
	NexusSoundEngine();
	static NexusSoundEngine globalSoundEngine;
	static NexusSoundEngine* Get() { return &NexusSoundEngine::globalSoundEngine; };

public:
	// Initialize engine and subsystems. Returns true if successful, false if failed
	bool InitializeSoundEngine();
	bool ShutdownSoundEngine();

	// Initialize the script module. Returns true if successful, false if failed
	bool InitializeScriptModule() override;
	bool ShutdownScriptModule() override;

	// Initialize the Wwise Sound Engine. Returns true if successful, false if failed
	bool InitializeInteractiveSoundEngine();
	bool ShutdownInteractiveSoundEngine();

	// VST Host Interface
	tresult PLUGIN_API getName( Vst::String128 name ) override;
	tresult PLUGIN_API createInstance( TUID cid, TUID _iid, void** obj ) override;

	DECLARE_FUNKNOWN_METHODS

	// Audio Signal 
	// Always declared. Without ASIO support (non-Windows) it logs and returns false
	bool InitializeASIODevice();
#if NEXUS_WITH_ASIO
	long InitializeAsioStaticData();
	ASIOError CreateAsioBuffers();
	long GetAsioBufferSize() { return mAsioBufferPreferredSize; };
	ASIOSampleRate GetAsioSampleRate() { return mSampleRate; };
	long GetAsioInputLatency() { return mInputLatency; };
	long GetAsioOutputLatency() { return mOutputLatency; };
#endif // NEXUS_WITH_ASIO
	bool LoadSignalChainPlugins();

	// Sound Engine Loop
	void SoundEngineLoopFunc( float deltaTime );

// API Controls
public:
	// Default behavior is a stero channel strip added at the end
	bool AddChannelToDesk( std::string TrackName = "", int ChannelCount = 2, int Index = -1 );
	bool RemoveChannelFromDesk( int Index );
	bool SetupMasteringBus( int ChannelCount = 2 );

	// Wwise Sound Engine and Spatial Audio Environment Setup and Teardown
	bool SetupWwiseSoundEngine();
	bool SetGeometry();
	bool SetGeometryInstance();
	bool RemoveGeometrySet();
	bool RemoveGeometryInstance();
	bool AddRoom();
	bool RemoveRoom();
	bool SetImageSource();
	bool RemoveImageSource();
	bool ClearImageSources();

	// Wwise Runtime Emitter controls
	bool AddDefaultListener();
	bool RemoveDefaultListener();
	bool LoadWwiseSoundBank();
	bool RegisterGameObject();
	bool UnregisterGameObject();
	bool SetListenerPosition();
	bool SetGameObjectPosition();
	bool SetGameObjectMultiposition();
	bool SetGameObjectInSpatialAudioRoom();
	bool SetAuxSends();
	bool SetDistanceProbe();
	bool SetAttenuationScalingFactor();

	// Wwise Runtime Playback and Parameter Controls
	bool PostAudioEvent();
	bool StopPlayingID();
	bool PausePlayingID();
	bool StopAllSound();
	bool StopGameObject();
	bool PauseGameObject();
	bool SetRealTimeParameterValue();
	bool SetRealTimeParameterValueOnPlayingID();
	bool SetSwitchValue();
	bool SetStatevalue();
	bool SeekOnEvent();
	bool PostTrigger();



	// Wwise Comms and Information
	bool SendSoundEngineMessage();

public:
	bool bAsioDeviceInitialized = false;
	bool bSignalChainInitialized = false;

#if NEXUS_WITH_ASIO
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
#endif // NEXUS_WITH_ASIO



	// Signal Chain - intermediate signal processing
	VST3::Hosting::Module::Ptr vstModule_ChannelStrip;
	VST3::Hosting::Module::Ptr vstModule_Mastering;
	VST3::Hosting::Module::Ptr vstModule_Meter;
	std::string ChannelStripVSTPath = "";
	std::string MasteringVSTPath = "";
	std::string MeterVSTPath = "";

	Vst::HostApplication context;
	std::vector<NxsChannelStrip*> Channels;
	NxsChannelStrip* MasteringBus = nullptr;
};

MODULE_API_START( NexusSoundEngineAPI );
APPLICATION_API_START;
// Application will init the sound engine
bool NEXUSSOUNDENGINE_API NexusSoundEngine_InitializeSoundEngine();
// Application will tear down the sound engine
bool NEXUSSOUNDENGINE_API NexusSoundEngine_FinalizeSoundEngine();
void NEXUSSOUNDENGINE_API NexusSoundEngine_SetChannelStripPlugin( std::string PluginPath );
void NEXUSSOUNDENGINE_API NexusSoundEngine_SetMasteringPlugin( std::string PluginPath );
void NEXUSSOUNDENGINE_API NexusSoundEngine_SetMeterPlugin( std::string PluginPath );
APPLICATION_API_END;
MODULE_API_END;