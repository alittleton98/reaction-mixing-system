// Representation of a group of audio channels
// Mono uses the mono channel strip config. Stereo uses the stereo channel strip config. Larger channel counts use multimono 
// This class handles all VST signal processing for the audio buffer
#pragma once
#include "common.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "base/source/fdebug.h"
#include <atomic>
#include <cstddef>
#if NEXUS_PLATFORM_WINDOWS
#include <windows.h>
#endif

using namespace Steinberg;
using namespace VST3::Hosting;
class NexusSoundEngine;

struct FChannelStripEffect
{
	IPtr<Vst::IComponent> componentPtr;
	IPtr<Vst::IAudioProcessor> audioProcessorPtr;
	IPtr<Vst::IEditController> editControllerPtr;
	IPtr<Vst::PlugProvider> pluginProvider;
	ClassInfo classInfo;
};

static constexpr uint32_t SAMPLE_RING_BUFFER_MAGIC = 0xA510BEEF;
static constexpr uint32_t SAMPLE_RING_BUFFER_VERSION = 1;
#if NEXUS_PLATFORM_WINDOWS
static constexpr char SAMPLE_RING_BUFFER_MAPPING_BASE[] = "Global\\AsioSharedRingBuffer_v1_";
using NexusMemoryHandle = HANDLE;
#else
// POSIX shm_open names start with a single '/'
static constexpr char SAMPLE_RING_BUFFER_MAPPING_BASE[] = "/AsioSharedRingBuffer_v1_";
using NexusMemoryHandle = int;
#endif

// Ring Buffer header structure
// Layout is explicit (padding + alignas); the static_asserts below verify the offsets on every compiler
struct AudioSampleBufferHeader
{
	// immutable
	uint32_t magic;					// Sanity checker
	uint32_t version;
	uint32_t frameCapacity;			// Number of sample frames. Frame is all channels of interleaved audio. We use 1 right now, but leaving room to use the interleaved as we learn more
	uint32_t sampleRate;			// Samples per second
	uint32_t channelIndex;			// The channel of the audio device using this buffer
	uint32_t _pad[ 11 ];			// Pad the above to 64 bytes

	alignas( 64 ) std::atomic<uint64_t> writePointer; // Advanced by producer application
	alignas( 64 ) std::atomic<uint64_t> readPointer; // Advanced by the consumer

	AudioSampleBufferHeader() : magic( SAMPLE_RING_BUFFER_MAGIC ), version( SAMPLE_RING_BUFFER_VERSION ) {}
};

static_assert( offsetof( AudioSampleBufferHeader, writePointer ) == 64, "writePointer is out of alignment" );
static_assert( offsetof( AudioSampleBufferHeader, readPointer ) == 128, "readPointer is out of alignment" );

struct AudioMemoryPage
{
	AudioSampleBufferHeader* header = nullptr;
	std::string mappingName = SAMPLE_RING_BUFFER_MAPPING_BASE;
#if NEXUS_PLATFORM_WINDOWS
	NexusMemoryHandle memoryFile = nullptr;
#else
	NexusMemoryHandle memoryFile = -1;
#endif
};

// Get the total size of the buffer
inline size_t GetRingBufferSize( uint32_t frameCapacity )
{
	return sizeof( AudioSampleBufferHeader ) + ( static_cast< size_t >( frameCapacity ) * sizeof( float ) );
}

// Get the sample buffer by getting the header and the sample data that follows it
inline float* GetSampleBuffer( AudioSampleBufferHeader* header )
{
	return reinterpret_cast< float* >( reinterpret_cast< uint64_t* >( header ) + sizeof( AudioSampleBufferHeader ) );
}

#define MAX_NUMBER_GROUPED_FADERS 512

class NxsChannelStrip : public Vst::IComponentHandler
{
public:
	std::string Name = "Track";
	int Number = -1;
	std::vector<int> asioChannels;
	int BufferSize = 0;
	int64 Color;

	void SetModules( Module::Ptr inChannelStripModule, Module::Ptr inDynamicsModule, Module::Ptr inMeterModule );
	bool InitializeBus( NexusSoundEngine* inSoundEngine, bool bUseBusCompressor, bool bUseMeter );
	bool ProcessAudioForChannel();
	bool StartChannelStrip( PluginFactory inChannelStripFactory, NexusSoundEngine* inSoundEngine );

	bool CreateView();

	bool StartMeter( PluginFactory inMeterFactory, NexusSoundEngine* inSoundEngine );
	FChannelStripEffect ChannelStripEffect;
	FChannelStripEffect BusCompressorEffect;
	FChannelStripEffect MeterEffect;

	// Vst::IComponentHandler interface
	/** To be called before calling a performEdit (e.g. on mouse-click-down event).
	 * This must be called in the UI-Thread context!
	 * \note [UI-thread & Connected] */
	tresult beginEdit( Vst::ParamID id ) override
	{
		SMTG_DBPRT1( "beginEdit called (%d)\n", id );
		return kNotImplemented;
	}

	/** Called between beginEdit and endEdit to inform the handler that a given parameter has a new
	 * value. This must be called in the UI-Thread context!
	 * \note [UI-thread & Connected] */
	tresult  performEdit( Vst::ParamID id, Vst::ParamValue valueNormalized ) override
	{
		printf( "performEdit called (%d, %f)\n", id, valueNormalized );
		SMTG_DBPRT2( "performEdit called (%d, %f)\n", id, valueNormalized );
		return kNotImplemented;
	}

	/** To be called after calling a performEdit (e.g. on mouse-click-up event).
	 * This must be called in the UI-Thread context!
	 * \note [UI-thread & Connected] */
	tresult  endEdit( Vst::ParamID id ) override
	{
		SMTG_DBPRT1( "endEdit called (%d)\n", id );
		return kNotImplemented;
	}

	/** Instructs host to restart the component. This must be called in the UI-Thread context!
	 * @param[in] flags is a combination of RestartFlags
	 * \note [UI-thread & Connected] */
	tresult  restartComponent( int32 flags ) override
	{
		SMTG_DBPRT1( "restartComponent called (%d)\n", flags );
		return kNotImplemented;
	}

	tresult  queryInterface( const TUID /*_iid*/, void** /*obj*/ ) override
	{
		return kNoInterface;
	}

	// we do not care here of the ref-counting. A plug-in call of release should not destroy this
	// class!
	uint32  addRef() override { return 1000; }
	uint32  release() override { return 1000; }

private:
	VST3::Hosting::Module::Ptr channelStripModule;
	VST3::Hosting::Module::Ptr dynamicsModule;
	VST3::Hosting::Module::Ptr meterModule;
	std::vector<Vst::ProcessData> ChannelProcessData;
	Vst::SampleRate ChannelSampleRate;
	int ChannelBlockSize;

	AudioMemoryPage* memoryPage_ch1 = nullptr;
	AudioMemoryPage* memoryPage_ch2 = nullptr;

};