# Nexus Mixing System CMake configuration file
#
cmake_minimum_required (VERSION 3.26)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(FetchContent)
# Don't hit the network on every reconfigure once dependencies are downloaded
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

project ("nexus-mixing-system")

# Platform feature switches. Every module is built and linked on every platform; features a platform
# cannot support (ASIO, the Win32 shared memory, the WPF app) are compiled out by macro in source
# (see NEXUS_WITH_* in src/common.h), so the same module list is always produced.
if (WIN32)
	set(NMS_WITH_ASIO ON)
else()
	set(NMS_WITH_ASIO OFF)
	if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
		message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}. Use Windows or Linux")
	endif()
endif()
add_compile_definitions(NEXUS_WITH_ASIO=$<BOOL:${NMS_WITH_ASIO}>)
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
	execute_process(
        COMMAND ${GIT_EXECUTABLE}  rev-list --count HEAD -- "${CMAKE_CURRENT_SOURCE_DIR}/file-formats-in.json"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NMS_FILEFORMAT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    message(FATAL_ERROR "Git is not found. This should not be the case")
endif()

if (DEFINED ENV{WWISEVERSION})
  set (WWISE_SDK_PATH "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
  set (WWISE_INCLUDE_PATH "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK/include") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
  set (WWISE_SAMPLE_PATH "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK/samples") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
  set (WWISE_LIB_PATH_WIN "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK/x64_vc180") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
  set (WWISE_LIB_PATH_LNX "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK/Linux_x64") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
  set (WWISE_LIB_PATH_MAC "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK/Mac_Xcode2700") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
else()
  message(FATAL_ERROR "Environment variable WWISEVERSION is required")
endif()

file(GLOB_RECURSE WWISE_INTEGRATION_SOURCES
    "${WWISE_SDK_PATH}/samples/SoundEngine/Common/*.cpp"
)
list(REMOVE_ITEM WWISE_INTEGRATION_SOURCES
    "${WWISE_SAMPLE_PATH}/SoundEngine/Common/AkPlatformProfilerHookStubs.cpp"
)

if (WIN32)
file(GLOB_RECURSE WWISE_PLATFORM_SOURCES
    "${WWISE_SDK_PATH}/samples/SoundEngine/Win32/*.cpp"  # or Mac, depending on platform
)
else()
file(GLOB_RECURSE WWISE_PLATFORM_SOURCES
	"${WWISE_SDK_PATH}/samples/SoundEngine/POSIX/*.cpp"  # or Mac, depending on platform
)
endif()

set(NMS_MAJOR_NUMBER "0")
set(NMS_MINOR_NUMBER "1")
set(NMS_BUILD_NUMBER ${GIT_COMMIT_COUNT})


add_compile_definitions(NMS_MAJOR_NUMBER=0)
add_compile_definitions(NMS_MINOR_NUMBER=1)
add_compile_definitions(NMS_BUILD_NUMBER=${GIT_COMMIT_COUNT})
add_compile_definitions(NMS_VERSION_STRING="${NMS_MAJOR_NUMBER}.${NMS_MINOR_NUMBER}.${NMS_BUILD_NUMBER}")
add_compile_definitions(ASIO_DEVICE_NAME="VB-Matrix VASIO-512")
add_compile_definitions(WWISE_VERSION="${WWISEVERSION}")

# Build the libraries into the same directory as the C# exe (out/build/<preset>)
# Set after the FetchContent dependencies so they keep their default locations.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(SCRIPT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/script/")
set(SCRIPT_DEFAULT_NAME "default-nmsconfig")
set(SCRIPT_FILE_EXTENSION ".nxs")

# File format hash: SHA-256 of the unsubstituted template, truncated to 32 bits so it is a valid
# JSON number (and fits a uint32). The substituted copy is written to the build dir.
set(NMS_FILEFORMAT_JSON "${CMAKE_CURRENT_SOURCE_DIR}/file-formats-in.json")

# Project file field defaults, substituted into file-formats-in.json as @<name>_JSON@ alongside the
# version. Cached so they can be overridden at configure time, e.g. -DNMS_DEFAULT_METER_VST3=C:/...
# kind is BOOL, STRING, FILEPATH or NUMBER; strings are escaped and quoted for JSON.
function(nms_fileformat_default name kind value doc)
    if (kind STREQUAL "NUMBER")
        set(${name} "${value}" CACHE STRING "${doc}")
        set(json "${${name}}")
    else()
        set(${name} "${value}" CACHE ${kind} "${doc}")
        if (kind STREQUAL "BOOL")
            if (${name})
                set(json "true")
            else()
                set(json "false")
            endif()
        else()
            string(REPLACE "\\" "\\\\" json "${${name}}")
            string(REPLACE "\"" "\\\"" json "${json}")
            set(json "\"${json}\"")
        endif()
    endif()
    set(${name}_JSON "${json}" PARENT_SCOPE)
endfunction()

nms_fileformat_default(NMS_DEFAULT_CHANNEL_STRIP_VST3 FILEPATH "" "Default: path to the channel strip VST3")
nms_fileformat_default(NMS_DEFAULT_BUS_COMPRESSOR_VST3 FILEPATH "" "Default: path to the bus compressor VST3")
nms_fileformat_default(NMS_DEFAULT_METER_VST3 FILEPATH "" "Default: path to the meter VST3")
nms_fileformat_default(NMS_DEFAULT_ROUTING_MATRIX NUMBER 0.0 "Default: gain every routing matrix element is filled with")

configure_file("${NMS_FILEFORMAT_JSON}" "${CMAKE_CURRENT_SOURCE_DIR}/src/file-formats.json" @ONLY NEWLINE_STYLE LF)

# Mirror the global compile definitions into the C# project. DefineConstants can't carry values,
# so write them as constants; file(CONFIGURE) only rewrites the file when the content changes.
set(NMS_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/app")
if (WIN32)
file(MAKE_DIRECTORY "${NMS_APP_DIR}")
string(REPLACE "\\" "\\\\" NMS_CS_ASIO_DEVICE_NAME "$ENV{NMSASIODEVICENAME}")
string(REPLACE "\"" "\\\"" NMS_CS_ASIO_DEVICE_NAME "${NMS_CS_ASIO_DEVICE_NAME}")
file(CONFIGURE OUTPUT "${NMS_APP_DIR}/BuildDefines.g.cs" NEWLINE_STYLE CRLF CONTENT
"// <auto-generated> by CMakeLists.txt; do not edit. Mirrors the global add_compile_definitions().
namespace NexusMixingSystem;

internal static class BuildDefines
{
    public const int NmsMajorNumber = ${NMS_MAJOR_NUMBER};
    public const int NmsMinorNumber = ${NMS_MINOR_NUMBER};
    public const int NmsBuildNumber = ${NMS_BUILD_NUMBER};
    public const string NmsVersionString = \"${NMS_MAJOR_NUMBER}.${NMS_MINOR_NUMBER}.${NMS_BUILD_NUMBER}\";
    public const string AsioDeviceName = \"${NMS_CS_ASIO_DEVICE_NAME}\";
	public const string ScriptOutputDirectory = \"${SCRIPT_DIRECTORY}\";
	public const string ScriptDefaultName = \"${SCRIPT_DEFAULT_NAME}\";
	public const string ScriptFileExtension = \"${SCRIPT_FILE_EXTENSION}\";
}
")
endif()

message("")
find_program(Python3 NAMES python3 python REQUIRED)
if (Python3)
	message(STATUS "Python3 found: ${Python3}")
	execute_process(
			COMMAND "${Python3}" "${CMAKE_CURRENT_SOURCE_DIR}/src/generator.py"
			RESULT_VARIABLE PYTHON_GEN_RESULT
		)
		if (NOT PYTHON_GEN_RESULT EQUAL 0)
			message(FATAL_ERROR "Python generator failed (${PYTHON_GEN_RESULT})")
		endif()
endif()
message("")

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



message("Checking for dependencies...")
# Dependencies
# ASIO is Windows only. Elsewhere the SDK is not fetched and NEXUS_WITH_ASIO compiles the code out.
set(ASIOSDK_SOURCE "")
set(ASIOSDK_INCLUDE_DIRS "")
if (NMS_WITH_ASIO)
	FetchContent_Declare( 
		asio
		GIT_REPOSITORY	https://github.com/audiosdk/asio.git
		GIT_TAG			main
	)
	FetchContent_MakeAvailable(asio)
	if (EXISTS "${asio_SOURCE_DIR}/host/sample/hostsample.cpp")
		file(REMOVE_RECURSE "${asio_SOURCE_DIR}/host/sample/hostsample.cpp")
	endif()
	file(GLOB_RECURSE ASIOSDK_SOURCE
	"${asio_SOURCE_DIR}/host/*.cpp"
	"${asio_SOURCE_DIR}/host/pc/*.cpp"
	"${asio_SOURCE_DIR}/common/asio.cpp"
	)
	set(ASIOSDK_INCLUDE_DIRS
		"${asio_SOURCE_DIR}/common"
		"${asio_SOURCE_DIR}/host"
		"${asio_SOURCE_DIR}/host/pc"
	)
endif()

FetchContent_Declare( 
	rtmidi
	GIT_REPOSITORY	https://github.com/thestk/rtmidi.git
	GIT_TAG			master
)
FetchContent_MakeAvailable(rtmidi)

FetchContent_Declare( 
	vst3sdk
	GIT_REPOSITORY	https://github.com/steinbergmedia/vst3sdk.git
	GIT_TAG			master
)
if (NOT WIN32)
	# Only the VST3 hosting libraries are used; skip VSTGUI (needs X11/Wayland/Cairo) and the examples
	set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "" FORCE)
	set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(vst3sdk)
smtg_enable_vst3_sdk()


if (WIN32)
find_file(VB_AUDIO_EXE
    NAMES VBAudioMatrixCoconut_x64.exe
    PATHS "$ENV{${_pf86}}/VB/VBAudioMatrix"
          "C:/Program Files (x86)/VB/VBAudioMatrix"
    NO_DEFAULT_PATH
)

if(VB_AUDIO_EXE)
    cmake_path(GET VB_AUDIO_EXE PARENT_PATH VB_AUDIO_PATH)   # CMake 3.20+
    message(STATUS "VB Audio Path found: ${VB_AUDIO_PATH}")
else()
    message(FATAL_ERROR "VB Audio Matrix not found. Please install it ")
endif()
endif()

# The VST3 hosting module loader is platform specific
if (WIN32)
	set(VST3_MODULE_SOURCE "${vst3sdk_SOURCE_DIR}/public.sdk/source/vst/hosting/module_win32.cpp")
else()
	set(VST3_MODULE_SOURCE "${vst3sdk_SOURCE_DIR}/public.sdk/source/vst/hosting/module_linux.cpp")
	find_package(Threads REQUIRED)
endif()




# Each nms_<name> builds as a SHARED lib (dll + import lib, used by the C# app) and as a
# STATIC lib (nms_<name>_static, linked into the server) from the same sources.
function(nms_add_library name class)
	file(GLOB_RECURSE MODULE_SOURCE_FILES
	"src/${name}/private/*.cpp"
	)
	file(GLOB_RECURSE MODULE_HEADER_FILES
	"src/${name}/private/*.h"
	)
	set(sources
		src/${name}/private/${class}.cpp
		src/${name}/public/${class}.h
		${MODULE_SOURCE_FILES}
		${MODULE_HEADER_FILES}
		${ARGN}
		${ASIOSDK_SOURCE} 
		${VST3_MODULE_SOURCE}
	)
	foreach(kind SHARED STATIC)
		set(target nms_${name})
		if ("${kind}" STREQUAL "STATIC")
			set(target nms_${name}_static)
		endif()
		add_library(${target} ${kind} ${sources})
		target_include_directories(${target}
			PUBLIC  
			${CMAKE_CURRENT_SOURCE_DIR}/src/${name}/public
			${CMAKE_CURRENT_SOURCE_DIR}/src
			"${CMAKE_CURRENT_SOURCE_DIR}/src"
			${ASIOSDK_INCLUDE_DIRS}
			"${rtmidi_SOURCE_DIR}"
			"${simpleini_SOURCE_DIR}"
			"${vst3sdk_SOURCE_DIR}"
			"${rapidjson_SOURCE_DIR}/include"
			PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/${name}/private
		)
		set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
		set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
		set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
		target_link_libraries(${target} PRIVATE 
		pybind11::embed 
		sdk_hosting
		base
		pluginterfaces
		sdk_common
		)
		if (UNIX)
			target_link_libraries(${target} PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)
		endif()
		target_compile_features(${target} PUBLIC cxx_std_20)
		target_compile_definitions(${target} PRIVATE NEXUS_SCRIPT_MODULE_ID=nms_${name})
		target_compile_definitions(${target} PUBLIC "SCRIPT_OUTPUT_DIRECTORY=R\"(${SCRIPT_DIRECTORY})\"")
		target_compile_definitions(${target} PRIVATE "DEFAULT_SCRIPT_PATH=R\"(${SCRIPT_DIRECTORY}${SCRIPT_DEFAULT_NAME}${SCRIPT_FILE_EXTENSION})\"")
		find_program(PYTHON_EXECUTABLE python3)
		target_compile_definitions(${target} PRIVATE PYTHONDIR=${PYTHON_EXECUTABLE})
		set_target_properties(${target} PROPERTIES CXX_STANDARD 20)
		target_compile_definitions(${target} PRIVATE IEEE754_64FLOAT=1)
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			target_compile_definitions(${target} PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
		elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
			target_compile_definitions(${target} PRIVATE DEBUG_CONFIG=0 RELEASE_CONFIG=1)
		else()
			message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
		endif()
	endforeach()
	set_target_properties(nms_${name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endfunction()

nms_add_library(soundengine NexusSoundEngine)
nms_add_library(controlsurface NexusControlSurface)
nms_add_library(network NexusNetwork)
nms_add_library(common NexusCommon )

target_include_directories(nms_soundengine PUBLIC "${WWISE_INCLUDE_PATH}" "${WWISE_SAMPLE_PATH}" "${WWISE_SAMPLE_PATH}/SoundEngine/Common")
target_include_directories(nms_soundengine_static PUBLIC "${WWISE_INCLUDE_PATH}" "${WWISE_SAMPLE_PATH}" "${WWISE_SAMPLE_PATH}/SoundEngine/Common")
target_sources(nms_soundengine PUBLIC ${WWISE_INTEGRATION_SOURCES} ${WWISE_PLATFORM_SOURCES})
target_sources(nms_soundengine_static PUBLIC ${WWISE_INTEGRATION_SOURCES} ${WWISE_PLATFORM_SOURCES})
target_link_libraries(nms_soundengine_static PRIVATE nms_common_static)



# C# WPF APPLICATION
# The WPF project is scaffolded once (when src/app has no App.xaml); the csproj is
# rewritten from the template below on EVERY configure, so edit the csproj here.
find_program(DOTNET_EXECUTABLE dotnet)
if (NOT WIN32)
	message(STATUS "Skipping the WPF application (Windows only)")
elseif (DOTNET_EXECUTABLE)
	set(NMS_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/app")
	file(MAKE_DIRECTORY "${NMS_APP_DIR}")
	if (NOT EXISTS "${NMS_APP_DIR}/App.xaml")
		message(STATUS "Generating WPF project")
		execute_process(
			COMMAND "${DOTNET_EXECUTABLE}" new wpf --name NexusMixingSystem --output "${NMS_APP_DIR}"
			RESULT_VARIABLE NMS_APP_RESULT
		)
		if (NOT NMS_APP_RESULT EQUAL 0)
			message(WARNING "dotnet new wpf failed (${NMS_APP_RESULT})")
		endif()
		# Remove the bin directory created by project generation
		file(REMOVE_RECURSE "${NMS_APP_DIR}/bin")
		file(REMOVE_RECURSE "${NMS_APP_DIR}/out")
	endif()

	# Always overwrite the csproj. Use the CMake preset names (x64-debug / x64-release) for $(Configuration)
	file(WRITE "${NMS_APP_DIR}/NexusMixingSystem.csproj"
[==[<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>WinExe</OutputType>
    <TargetFramework>net10.0-windows</TargetFramework>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <UseWPF>true</UseWPF>
    <AppendTargetFrameworkToOutputPath>false</AppendTargetFrameworkToOutputPath>
	<ApplicationIcon>..\resources\sound-wave-color.ico</ApplicationIcon>
	<Platforms>x64</Platforms>
  </PropertyGroup>

  <ItemGroup>
    <Resource Include="..\resources\splash_name.png" Link="Resources\splash_name.png" />
    <Resource Include="..\resources\splash_blank.png" Link="Resources\splash_blank.png" />
  </ItemGroup>

  <PropertyGroup Condition="'$(Configuration)' == 'Debug'">
    <OutputPath>..\..\out\build\x64-debug\bin</OutputPath>
  </PropertyGroup>

  <PropertyGroup Condition="'$(Configuration)' == 'Release'">
    <OutputPath>..\..\out\build\x64-release\bin</OutputPath>
  </PropertyGroup>

  <!-- Pre-build step (intentionally empty) -->
  <Target Name="PreBuild" BeforeTargets="BeforeBuild">
  </Target>

  <!-- Pre-build: compile the CMake libraries into the same directory as this exe -->
  <Target Name="BuildNativeLibraries" BeforeTargets="BeforeBuild">
    <PropertyGroup>
      <NmsCMakeBuildDir>$([MSBuild]::NormalizePath('$(MSBuildProjectDirectory)', '..', '..', 'out', 'build', 'x64-$(Configuration.ToLower())'))</NmsCMakeBuildDir>
    </PropertyGroup>
    <Error Condition="!Exists('$(NmsCMakeBuildDir)/build.ninja')" Text="CMake build directory '$(NmsCMakeBuildDir)' is not configured. Run: cmake --preset x64-$(Configuration.ToLower())" />
    <!-- cl.exe needs the VS developer environment (INCLUDE/LIB) to find windows.h; load it via vcvars64.bat -->
    <Exec Command="&quot;$(MSBuildProgramFiles32)\Microsoft Visual Studio\Installer\vswhere.exe&quot; -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath" ConsoleToMSBuild="true" StandardOutputImportance="low">
      <Output TaskParameter="ConsoleOutput" PropertyName="NmsVsInstallPath" />
    </Exec>
    <Error Condition="'$(NmsVsInstallPath)' == ''" Text="Visual Studio with the C++ toolset was not found (vswhere)." />
    <Exec Command="call &quot;$(NmsVsInstallPath)\VC\Auxiliary\Build\vcvars64.bat&quot; &gt;nul &amp;&amp; cmake --build &quot;$(NmsCMakeBuildDir)&quot; --target nms_common nms_network nms_controlsurface nms_soundengine" />
  </Target>

  <!-- Output goes to out\build; remove the stray bin directory MSBuild creates -->
  <Target Name="RemoveStrayBin" AfterTargets="Restore;CoreCompile;Build">
    <RemoveDir Directories="$(MSBuildProjectDirectory)\bin" />
	<RemoveDir Directories="$(MSBuildProjectDirectory)\out" />
  </Target>

</Project>
]==])
else()
	message(WARNING "dotnet SDK not found; cannot generate src/app WPF project")
endif()

if (WIN32)
	add_executable(NexusMixingCLI WIN32 "src/command-line-interface/NexusCLI.cpp" "src/resources/nexus-mixing-desk.rc")
else()
	add_executable(NexusMixingCLI "src/command-line-interface/NexusCLI.cpp")
endif()
target_include_directories(NexusMixingCLI PUBLIC
 "${CMAKE_CURRENT_SOURCE_DIR}/src"
)


# Global Definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(NexusMixingCLI PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions(NexusMixingCLI PRIVATE DEBUG_CONFIG=0 RELEASE_CONFIG=1)
else()
	message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
endif()

target_link_libraries(NexusMixingCLI PRIVATE
	nms_soundengine_static
	nms_controlsurface_static
	nms_network_static
	nms_common_static
	pybind11::embed
)

# Pre-build step (intentionally empty)
add_custom_command(TARGET NexusMixingCLI PRE_BUILD
	COMMAND "${CMAKE_COMMAND}" -E true
)

if(NOT WIN32)
	# Wwise libs / winsock are Windows only
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8) 
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		file(GLOB WWISE_LIBS "${WWISE_LIB_PATH_WIN}/Debug/lib/*.lib")
		foreach (libfile IN LISTS WWISE_LIBS)
			message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
		endforeach()
		target_link_libraries(nms_soundengine_static PRIVATE ${WWISE_LIBS} ws2_32)
		target_link_libraries(nms_soundengine PRIVATE ${WWISE_LIBS} ws2_32)
	# Use Profile configuration because we need to be able to communicate with the Sound Engine
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")	
		file(GLOB WWISE_LIBS "${WWISE_LIB_PATH_WIN}/Profile/lib/*.lib")
		target_link_libraries(nms_soundengine_static PRIVATE ${WWISE_LIBS} ws2_32)
		target_link_libraries(nms_soundengine PRIVATE ${WWISE_LIBS} ws2_32)
	else()
		message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
	endif()
else()
	message(FATAL_ERROR "Unsupported architecture. Chaotic pointer size")
endif()

target_compile_definitions(NexusMixingCLI PRIVATE IEEE754_64FLOAT=1)
# The VST3 SDK headers require DEVELOPMENT or RELEASE when neither _DEBUG nor NDEBUG is defined (non-MSVC)
if (NOT WIN32)
	target_compile_definitions(NexusMixingCLI PRIVATE $<IF:$<CONFIG:Debug>,DEVELOPMENT=1,RELEASE=1>)
endif()

if (WIN32)
	target_include_directories(nms_soundengine_static PRIVATE "${WWISE_SAMPLE_PATH}/SoundEngine/Win32")
	target_include_directories(nms_soundengine PRIVATE "${WWISE_SAMPLE_PATH}/SoundEngine/Win32")
	target_include_directories(NexusMixingCLI PRIVATE "${WWISE_SAMPLE_PATH}/SoundEngine/Win32")
elseif(UNIX)
	target_include_directories(nms_soundengine_static PRIVATE "${WWISE_SAMPLE_PATH}/SoundEngine/POSIX")
	target_include_directories(nms_soundengine PRIVATE "${WWISE_SAMPLE_PATH}/SoundEngine/POSIX")
	target_include_directories(NexusMixingCLI PRIVATE "${WWISE_SAMPLE_PATH}/SoundEngine/POSIX")	
endif()


# Linux: provision the default user script the CLI loads when no script path is given
if (UNIX)
	set(NMS_DEFAULT_SCRIPT_DEST "${SCRIPT_DIRECTORY}${SCRIPT_DEFAULT_NAME}${SCRIPT_FILE_EXTENSION}")
	if (NOT EXISTS "${NMS_DEFAULT_SCRIPT_DEST}")
		file(MAKE_DIRECTORY "${SCRIPT_DIRECTORY}")
		file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/default_nms_script.py" "${NMS_DEFAULT_SCRIPT_DEST}")
	endif()
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		file(GLOB WWISE_LIBS "${WWISE_LIB_PATH_LNX}/Debug/lib/*.lib")
		foreach (libfile IN LISTS WWISE_LIBS)
			message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
		endforeach()
		target_link_libraries(nms_soundengine_static PRIVATE ${WWISE_LIBS} ws2_32)
		target_link_libraries(nms_soundengine PRIVATE ${WWISE_LIBS} ws2_32)
	# Use Profile configuration because we need to be able to communicate with the Sound Engine
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")	
		file(GLOB WWISE_LIBS "${WWISE_LIB_PATH_LNX}/Profile/lib/*.lib")
		target_link_libraries(nms_soundengine_static PRIVATE ${WWISE_LIBS} ws2_32)
		target_link_libraries(nms_soundengine PRIVATE ${WWISE_LIBS} ws2_32)
	else()
		message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
	endif()
endif()
