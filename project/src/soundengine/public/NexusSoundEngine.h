#pragma once
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



