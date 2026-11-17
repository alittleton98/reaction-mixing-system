#include "NxsChannelStrip.h"

void NxsChannelStrip::SetModules( Module::Ptr inChannelStripModule, Module::Ptr inDynamicsModule, Module::Ptr inMeterModule )
{
}
bool NxsChannelStrip::InitializeBus( NexusSoundEngine* inSoundEngine, bool bUseBusCompressor, bool bUseMeter )
{
	return false;
}
bool NxsChannelStrip::ProcessAudioForChannel()
{
	return false;
}
bool NxsChannelStrip::StartChannelStrip( PluginFactory inChannelStripFactory, NexusSoundEngine* inSoundEngine )
{
	return false;
}

bool NxsChannelStrip::CreateView()
{
	return false;
}

bool NxsChannelStrip::StartMeter( PluginFactory inMeterFactory, NexusSoundEngine* inSoundEngine )
{
	return false;
}
