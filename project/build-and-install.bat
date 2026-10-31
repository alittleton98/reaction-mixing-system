@echo off
rem Configures and builds the x64-release preset (native libraries, CLI and WPF app), then copies
rem the release binaries to "%ProgramFiles%\Nexus Mixing System".
rem The build runs unelevated; only the copy step asks for administrator rights.
rem Usage: build-and-install.bat [--install-only]
setlocal

set "NMS_SOURCE_DIR=%~dp0"
set "NMS_SOURCE_DIR=%NMS_SOURCE_DIR:~0,-1%"
set "NMS_PRESET=x64-release"
set "NMS_BUILD_DIR=%NMS_SOURCE_DIR%\out\build\%NMS_PRESET%"
set "NMS_INSTALL_DIR=%ProgramFiles%\Nexus Mixing System"

if /i "%~1"=="--install-only" goto install

rem cl.exe, ninja and the preset's VSINSTALLDIR come from the VS developer environment
where cl.exe >nul 2>&1
if errorlevel 1 (
	set "NMS_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
	call :load_vcvars || goto fail
)

echo.
echo === Configuring %NMS_PRESET% ===
pushd "%NMS_SOURCE_DIR%"
cmake --preset %NMS_PRESET%
set "NMS_RESULT=%ERRORLEVEL%"
popd
if not "%NMS_RESULT%"=="0" goto fail

echo.
echo === Building native targets ===
cmake --build "%NMS_BUILD_DIR%" || goto fail

echo.
echo === Building WPF app ===
dotnet build "%NMS_SOURCE_DIR%\src\app\NexusMixingSystem.csproj" -c Release -p:Platform=x64 || goto fail

echo.
echo === Installing to %NMS_INSTALL_DIR% ===
net session >nul 2>&1
if not errorlevel 1 goto install
rem Not elevated: rerun just the install step as administrator and wait for it
powershell -NoProfile -Command "exit (Start-Process -FilePath '%~f0' -ArgumentList '--install-only' -Verb RunAs -Wait -PassThru).ExitCode"
if errorlevel 1 goto fail
echo Installed to %NMS_INSTALL_DIR%
pause
exit /b 0

:install
if not exist "%NMS_BUILD_DIR%\NexusMixingSystem.exe" (
	echo Release build not found in "%NMS_BUILD_DIR%". Run without --install-only first.
	goto install_fail
)
rem Runtime files only; the build dir also holds objects, import libs and CMake/Ninja state
robocopy "%NMS_BUILD_DIR%" "%NMS_INSTALL_DIR%" *.exe *.dll *.runtimeconfig.json *.deps.json /R:2 /W:1 /NJH /NP
rem robocopy: 0-7 are success codes, 8+ are failures
if errorlevel 8 goto install_fail
pause
exit /b 0

:install_fail
echo Install failed. Is the app still running?
pause
exit /b 1

:load_vcvars
if not exist "%NMS_VSWHERE%" (
	echo vswhere.exe not found; install Visual Studio with the C++ workload.
	exit /b 1
)
set "NMS_VS_PATH="
for /f "usebackq delims=" %%i in (`"%NMS_VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "NMS_VS_PATH=%%i"
if not defined NMS_VS_PATH (
	echo Visual Studio with the C++ toolset was not found.
	exit /b 1
)
call "%NMS_VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
exit /b 0

:fail
echo.
echo Build failed.
pause
exit /b 1
