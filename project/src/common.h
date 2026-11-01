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

using namespace std;

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
	inline string to_lower( std::string s )
	{
		std::transform( s.begin(), s.end(), s.begin(),
						[] ( unsigned char c ) { return std::tolower( c ); } );
		return s;
	}

	inline string to_upper( std::string s )
	{
		std::transform( s.begin(), s.end(), s.begin(),
						[] ( unsigned char c ) { return std::toupper( c ); } );
		return s;
	}

	// MSVC's typeid().name() is prefixed with "class " / "struct "; drop it so only the type name remains
	inline string StripTypeKeyword( string typeName )
	{
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
	inline string GetFileNameWithoutExtension( const string& path )
	{
		size_t nameStart = path.find_last_of( "/\\" );
		string fileName = ( nameStart == string::npos ) ? path : path.substr( nameStart + 1 );

		// A leading '.' (e.g. ".config") is part of the name, not an extension
		size_t extensionStart = fileName.find_last_of( '.' );
		if( extensionStart != string::npos && extensionStart > 0 )
		{
			fileName.erase( extensionStart );
		}
		return fileName;
	}
}
