#pragma once
#ifndef REACTIONCOMMON_API
#define REACTIONCOMMON_API __declspec(dllexport)
#endif


extern "C" 
{
	bool REACTIONCOMMON_API __cdecl ReactionCommon_Initialize();
	bool REACTIONCOMMON_API __cdecl ReactionCommon_Deinitialize();
}