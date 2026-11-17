#pragma once
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <format>
#include <typeinfo>

#if defined(__GNUG__)
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#endif

// ---------------------------------------------------------------------------------------------
// Platform / feature macros
// Modules always stay in the build and linked; features that a platform cannot support are
// compiled out with these macros (never by removing files from the build).
// ---------------------------------------------------------------------------------------------
#if defined(_WIN32)
#define NEXUS_PLATFORM_WINDOWS 1
#define NEXUS_PLATFORM_LINUX 0
#elif defined(__linux__)
#define NEXUS_PLATFORM_WINDOWS 0
#define NEXUS_PLATFORM_LINUX 1
#else
#error "Unsupported platform. Nexus Mixing System supports Windows and Linux"
#endif

// Symbol export for the shared libraries / the extern "C" application API
#if NEXUS_PLATFORM_WINDOWS
#define NEXUS_EXPORT __declspec(dllexport) __cdecl
#else
#define NEXUS_EXPORT __attribute__((visibility("default")))
#endif

// ASIO is a Windows only driver model
#ifndef NEXUS_WITH_ASIO
#define NEXUS_WITH_ASIO NEXUS_PLATFORM_WINDOWS
#endif
// The Win32 named shared memory (CreateFileMapping) used for the sample ring buffers
#ifndef NEXUS_WITH_WIN32_SHARED_MEMORY
#define NEXUS_WITH_WIN32_SHARED_MEMORY NEXUS_PLATFORM_WINDOWS
#endif


#if defined(DEBUG_CONFIG) && DEBUG_CONFIG
#define DISPLAY_LOG(string) std::fprintf(stdout, "> %s\n", string)
#define DISPLAY_LOG_Func(string) std::fprintf(stdout, "> %s | %s\n", __func__, string)
#define WARNING_LOG(string) std::fprintf(stdout, "> %s | WARNING | %s\n", __func__, string)
#else
#define DISPLAY_LOG(string) ((void)0)
#define WARNING_LOG(string) ((void)0)
#endif
#define ERROR_LOG(string) assert(false && string); fprintf(stderr, "> %s | ERROR | %s\n", __func__, string)

namespace nexus_util
{
	inline std::string to_lower( std::string s )
	{
		std::transform( s.begin(), s.end(), s.begin(),
						[] ( unsigned char c ) { return std::tolower( c ); } );
		return s;
	}

	inline std::string to_upper( std::string s )
	{
		std::transform( s.begin(), s.end(), s.begin(),
						[] ( unsigned char c ) { return std::toupper( c ); } );
		return s;
	}

	// MSVC's typeid().name() is prefixed with "class " / "struct "; drop it so only the type name remains.
	// GCC/Clang return the Itanium mangled name (e.g. "16NexusSoundEngine"), so demangle it first.
	inline std::string StripTypeKeyword( std::string typeName )
	{
#if defined(__GNUG__)
		int status = 0;
		std::unique_ptr<char, void( * )( void* )> demangled( abi::__cxa_demangle( typeName.c_str(), nullptr, nullptr, &status ), std::free );
		if( status == 0 && demangled )
		{
			typeName = demangled.get();
		}
#endif
		for( const char* keyword : { "class ", "struct " } )
		{
			if( typeName.starts_with( keyword ) )
			{
				typeName.erase( 0, strlen( keyword ) );
			}
		}
		return typeName;
	}

	// Returns the file name of a path without its directory or extension, e.g. "C:\dir\script.nxs" -> "script"
	inline std::string GetFileNameWithoutExtension( const std::string& path )
	{
		std::size_t nameStart = path.find_last_of( "/\\" );
		std::string fileName = ( nameStart == std::string::npos ) ? path : path.substr( nameStart + 1 );

		// A leading '.' (e.g. ".config") is part of the name, not an extension
		std::size_t extensionStart = fileName.find_last_of( '.' );
		if( extensionStart != std::string::npos && extensionStart > 0 )
		{
			fileName.erase( extensionStart );
		}
		return fileName;
	}
}
