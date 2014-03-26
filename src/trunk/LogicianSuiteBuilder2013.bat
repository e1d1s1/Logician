@echo off
SET MSBUILD_PATH="%ProgramFiles(x86)%\MSBuild\12.0\Bin\MSBuild.exe"
echo "MSBUILD.exe Path: %MSBUILD_PATH%"
echo "Creating VS2013 bin folder..."
mkdir VC12
mkdir VC12\EDSEngineWinRT
mkdir VC12\ROMWinRT
mkdir VC12\x64
mkdir VC12\x64\EDSEngineWinRT
mkdir VC12\x64\ROMWinRT

echo "Building EDSEngine..."
%MSBUILD_PATH% "EDSEngine\EDSEngine2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngine2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngine2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngine\EDSEngine2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "EDSEngine\Release MSXML\*.lib" VC12 /y
copy "EDSEngine\x64\Release MSXML\*.lib" VC12\x64 /y
echo "EDSEngine build complete"

echo "Building EDSEngineTestApp..."
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "EDSEngine\EDSEngineTestApp\Release MSXML\EDSEngineTestApp2013.exe" VC12 /y
copy "EDSEngine\EDSEngineTestApp\x64\Release MSXML\EDSEngineTestApp2013.exe" VC12\x64 /y
copy "EDSEngine\EDSEngineTestApp\test_project.gz" VC12 /y
copy "EDSEngine\EDSEngineTestApp\test_project.gz" VC12\x64 /y
echo "EDSEngineTestApp build complete"

echo "Building EDSEngineNET..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "EDSEngineNET\Release\*.dll" VC12 /y
copy "EDSEngineNET\x64\Release\*.dll" VC12\x64 /y
echo "EDSEngineNET build complete"

echo "Building EDSEngineNETTestApp..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2013\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2013\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2013\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2013\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "EDSEngineNET\EDSEngineNETTestApp2013\bin\x86\Release\EDSEngineNETTestApp.exe" VC12 /y
copy "EDSEngineNET\EDSEngineNETTestApp2013\bin\x64\Release\EDSEngineNETTestApp.exe" VC12\x64 /y
echo "EDSEngineNETTestApp build complete"

echo "Building Relational Object Model..."
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "RelationalObjectModel\Release MSXML\*.lib" VC12 /y
copy "RelationalObjectModel\x64\Release MSXML\*.lib" VC12\x64 /y
echo "Relational Object Model build complete"

echo "Building ROMAppConsoleTest..."
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2013.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "RelationalObjectModel\ROMAppConsoleTest\Release MSXML\ROMAppConsoleTest2013.exe" VC12 /y
copy "RelationalObjectModel\ROMAppConsoleTest\x64\Release MSXML\ROMAppConsoleTest2013.exe" VC12\x64 /y
echo "ROMAppConsoleTest build complete"

echo "Building ROMNET..."
%MSBUILD_PATH% "ROMNET\ROMNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "ROMNET\ROMNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=Win32
%MSBUILD_PATH% "ROMNET\ROMNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "ROMNET\ROMNET2013.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "ROMNET\Release\*.dll" VC12 /y
copy "ROMNET\x64\Release\*.dll" VC12\x64 /y
echo "ROMNET build complete"

echo "Building ROMNETTestApplication..."
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2013\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2013\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2013\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2013\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "ROMNET\ROMNETTestApplication2013\bin\x86\Release\ROMNETTestApplication.exe" VC12 /y
copy "ROMNET\ROMNETTestApplication2013\bin\x64\Release\ROMNETTestApplication.exe" VC12\x64 /y
echo "ROMNETTestApplication build complete"

echo "Building DecisionLogic..."
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:clean,rebuild /p:Configuration="Unicode Debug" /p:VisualStudioVersion=12.0
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:clean,rebuild /p:Configuration="Unicode Release" /p:VisualStudioVersion=12.0
copy "DecisionLogic\vc_mswu\*.exe" VC12 /y
echo "DecisionLogic build complete"

echo "Building Utilities"
echo "Building LogicianDebuggerWPF"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2013.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2013.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2013.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2013.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "Utilities\LogicianDebuggerWPF\bin\x86\Release\LogicianDebuggerWPF.dll" VC12 /y
copy "Utilities\LogicianDebuggerWPF\bin\x86\Release\WPFToolkit.dll" VC12 /y
copy "Utilities\LogicianDebuggerWPF\bin\x64\Release\LogicianDebuggerWPF.dll" VC12\x64 /y
copy "Utilities\LogicianDebuggerWPF\bin\x64\Release\WPFToolkit.dll" VC12\x64 /y
echo "Building LogicianDebuggerWPF complete"

echo "Building LogicianDebuggerTestApp2013..."
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\LogicianDebuggerTestApp2013.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\LogicianDebuggerTestApp2013.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\LogicianDebuggerTestApp2013.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\LogicianDebuggerTestApp2013.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\bin\x86\Release\LogicianDebuggerTestApp2013.exe" VC12 /y
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\config.xml" VC12 /y
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\bin\x64\Release\LogicianDebuggerTestApp2013.exe" VC12\x64 /y
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2013\config.xml" VC12\x64 /y
echo "Building LogicianDebuggerTestApp2012 complete"

echo "Building EDSEngineWinRTNativeWrapper..."
%MSBUILD_PATH% "EDSEngineWinRTNativeWrapper\EDSEngineWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "EDSEngineWinRTNativeWrapper\EDSEngineWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "EDSEngineWinRTNativeWrapper\EDSEngineWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngineWinRTNativeWrapper\EDSEngineWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "EDSEngineWinRTNativeWrapper\Release\EDSEngineWinRT\*.*" VC12\EDSEngineWinRT /y
copy "EDSEngineWinRTNativeWrapper\x64\Release\EDSEngineWinRT\*.*" VC12\x64\EDSEngineWinRT /y
echo "EDSEngineWinRTNativeWrapper build complete"

echo "Building ROMWinRTNativeWrapper..."
%MSBUILD_PATH% "ROMWinRTNativeWrapper\ROMWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "ROMWinRTNativeWrapper\ROMWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x86
%MSBUILD_PATH% "ROMWinRTNativeWrapper\ROMWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=12.0 /p:Platform=x64
%MSBUILD_PATH% "ROMWinRTNativeWrapper\ROMWinRT.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=12.0 /p:Platform=x64
copy "ROMWinRTNativeWrapper\Release\ROMWinRT\*.*" VC12\ROMWinRT /y
copy "ROMWinRTNativeWrapper\x64\Release\ROMWinRT\*.*" VC12\x64\ROMWinRT /y
echo "ROMWinRTNativeWrapper build complete"

echo "All builds complete"
pause
