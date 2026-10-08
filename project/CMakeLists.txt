# Reaction Mixing System CMake configuration file
#


cmake_minimum_required (VERSION 3.26)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(FetchContent)
# Don't hit the network on every reconfigure once dependencies are downloaded
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

project ("reaction-mixing-system")
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
else()
    set(GIT_COMMIT_COUNT "0")
    set(GIT_COMMIT_HASH "unknown")
endif()

set(RMD_MAJOR_NUMBER "0")
set(RMD_MINOR_NUMBER "1")
set(RMD_BUILD_NUMBER ${GIT_COMMIT_COUNT})

add_compile_definitions(RMD_MAJOR_NUMBER=0)
add_compile_definitions(RMD_MINOR_NUMBER=1)
add_compile_definitions(RMD_BUILD_NUMBER=${GIT_COMMIT_COUNT})
add_compile_definitions(RMD_VERSION_STRING="${RMD_MAJOR_NUMBER}.${RMD_MINOR_NUMBER}.${RMD_BUILD_NUMBER}")
if (DEFINED ENV{RMSASIODEVICENAME})
  add_compile_definitions(ASIO_DEVICE_NAME="$ENV{RMSASIODEVICENAME}")
else()
  message(FATAL_ERROR "Environment variable RMSASIODEVICENAME is required")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


if (NOT DEFINED ENV{VST_SSL_NATIVE_CHANNELSTRIP})
	message(FATAL_ERROR "Environment variable VST_SSL_NATIVE_CHANNELSTRIP not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_4000E_CHANNELSTRIP})
	message(FATAL_ERROR "Environment variable VST_SSL_4000E_CHANNELSTRIP not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_4000G_CHANNELSTRIP})
	message(FATAL_ERROR "Environment variable VST_SSL_4000G_CHANNELSTRIP not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_NATIVE_BUSCOMPRESSOR})
	message(FATAL_ERROR "Environment variable VST_SSL_NATIVE_BUSCOMPRESSOR not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_METER})
	message(FATAL_ERROR "Environment variable VST_SSL_METER not defined")
endif()



# Dependencies
FetchContent_Declare( 
	asio
	GIT_REPOSITORY	https://github.com/audiosdk/asio.git
	GIT_TAG			main
)
FetchContent_MakeAvailable(asio)

if (EXISTS "${asio_SOURCE_DIR}/host/sample/hostsample.cpp")
	file(REMOVE_RECURSE "${asio_SOURCE_DIR}/host/sample/hostsample.cpp")
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
FetchContent_MakeAvailable(vst3sdk)
smtg_enable_vst3_sdk()


# Build the libraries into the same directory as the C# exe (out/build/<preset>)
# Set after the FetchContent dependencies so they keep their default locations.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_library(rms_soundengine SHARED
	src/soundengine/private/ReactionSoundEngine.cpp
	src/soundengine/public/ReactionSoundEngine.h
)
target_include_directories(rms_soundengine
	PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/soundengine/public
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/soundengine/private
)
target_compile_features(rms_soundengine PUBLIC cxx_std_20)

add_library(rms_controlsurface SHARED
	src/controlsurface/private/ReactionControlSurface.cpp
	src/controlsurface/public/ReactionControlSurface.h
)
target_include_directories(rms_controlsurface
	PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/controlsurface/public
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/controlsurface/private
)
target_compile_features(rms_controlsurface PUBLIC cxx_std_20)

add_library(rms_network SHARED
	src/network/private/ReactionNetwork.cpp
	src/network/public/ReactionNetwork.h
)
target_include_directories(rms_network
	PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/network/public
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/network/private
)
target_compile_features(rms_network PUBLIC cxx_std_20)

add_library(rms_common SHARED
	src/common/private/ReactionCommon.cpp
	src/common/public/ReactionCommon.h
)
target_include_directories(rms_common
	PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/public
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/common/private
)
target_compile_features(rms_common PUBLIC cxx_std_20)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rms_soundengine PROPERTY CXX_STANDARD 20)
endif()
foreach(lib rms_soundengine rms_controlsurface rms_network rms_common)
	set_target_properties(${lib} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endforeach()

# C# WPF APPLICATION
# The WPF project is scaffolded once (when src/app has no App.xaml); the csproj is
# rewritten from the template below on EVERY configure, so edit the csproj here.
find_program(DOTNET_EXECUTABLE dotnet)
if (DOTNET_EXECUTABLE)
	set(RMD_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/app")
	file(MAKE_DIRECTORY "${RMD_APP_DIR}")
	if (NOT EXISTS "${RMD_APP_DIR}/App.xaml")
		message(STATUS "Generating WPF project")
		execute_process(
			COMMAND "${DOTNET_EXECUTABLE}" new wpf --name ReactionMixingSystem --output "${RMD_APP_DIR}"
			RESULT_VARIABLE RMD_APP_RESULT
		)
		if (NOT RMD_APP_RESULT EQUAL 0)
			message(WARNING "dotnet new wpf failed (${RMD_APP_RESULT})")
		endif()
		# Remove the bin directory created by project generation
		file(REMOVE_RECURSE "${RMD_APP_DIR}/bin")
		file(REMOVE_RECURSE "${RMD_APP_DIR}/out")
	endif()

	# Always overwrite the csproj. Use the CMake preset names (x64-debug / x64-release) for $(Configuration)
	file(WRITE "${RMD_APP_DIR}/ReactionMixingSystem.csproj"
[==[<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>WinExe</OutputType>
    <TargetFramework>net10.0-windows</TargetFramework>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <UseWPF>true</UseWPF>
    <AppendTargetFrameworkToOutputPath>false</AppendTargetFrameworkToOutputPath>
	<ApplicationIcon>..\resources\sound-wave-color.ico</ApplicationIcon>
  </PropertyGroup>

  <ItemGroup>
    <Resource Include="..\resources\splash_name.png" Link="Resources\splash_name.png" />
    <Resource Include="..\resources\splash_blank.png" Link="Resources\splash_blank.png" />
  </ItemGroup>

  <PropertyGroup Condition="'$(Configuration)' == 'Debug'">
    <OutputPath>..\..\out\build\x64-debug\</OutputPath>
  </PropertyGroup>

  <PropertyGroup Condition="'$(Configuration)' == 'Release'">
    <OutputPath>..\..\out\build\x64-release\</OutputPath>
  </PropertyGroup>

  <!-- Pre-build: compile the CMake libraries into the same directory as this exe -->
  <Target Name="BuildNativeLibraries" BeforeTargets="BeforeBuild">
    <PropertyGroup>
      <RmdCMakeBuildDir>$([MSBuild]::NormalizePath('$(MSBuildProjectDirectory)', '..', '..', 'out', 'build', 'x64-$(Configuration.ToLower())'))</RmdCMakeBuildDir>
    </PropertyGroup>
    <Error Condition="!Exists('$(RmdCMakeBuildDir)/build.ninja')" Text="CMake build directory '$(RmdCMakeBuildDir)' is not configured. Run: cmake --preset x64-$(Configuration.ToLower())" />
    <!-- cl.exe needs the VS developer environment (INCLUDE/LIB) to find windows.h; load it via vcvars64.bat -->
    <Exec Command="&quot;$(MSBuildProgramFiles32)\Microsoft Visual Studio\Installer\vswhere.exe&quot; -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath" ConsoleToMSBuild="true" StandardOutputImportance="low">
      <Output TaskParameter="ConsoleOutput" PropertyName="RmdVsInstallPath" />
    </Exec>
    <Error Condition="'$(RmdVsInstallPath)' == ''" Text="Visual Studio with the C++ toolset was not found (vswhere)." />
    <Exec Command="call &quot;$(RmdVsInstallPath)\VC\Auxiliary\Build\vcvars64.bat&quot; &gt;nul &amp;&amp; cmake --build &quot;$(RmdCMakeBuildDir)&quot; --target rms_common rms_network rms_controlsurface rms_soundengine" />
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

add_executable(ReactionMixingServer WIN32 "src/server/ReactionAudioServer.cpp" "src/resources/reaction-mixing-desk.rc")
target_include_directories(ReactionMixingServer PUBLIC 
 "${CMAKE_CURRENT_SOURCE_DIR}/src"
)