@echo off
SET MSBUILD_PATH=%systemroot%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe
echo "MSBUILD.exe Path: %MSBUILD_PATH%"
echo "Creating VS2012 bin folder..."
mkdir VC11
mkdir VC11\x64
mkdir Silverlight

echo "Building EDSEngine..."
%MSBUILD_PATH% "EDSEngine\EDSEngine2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngine2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngine2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngine\EDSEngine2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "EDSEngine\Release MSXML\*.lib" VC11 /y
copy "EDSEngine\x64\Release MSXML\*.lib" VC11\x64 /y
echo "EDSEngine build complete"

echo "Building EDSEngineTestApp..."
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "EDSEngine\EDSEngineTestApp\Release MSXML\EDSEngineTestApp2012.exe" VC11 /y
copy "EDSEngine\EDSEngineTestApp\x64\Release MSXML\EDSEngineTestApp2012.exe" VC11\x64 /y
copy "EDSEngine\EDSEngineTestApp\test_project.gz" VC11 /y
copy "EDSEngine\EDSEngineTestApp\test_project.gz" VC11\x64 /y
echo "EDSEngineTestApp build complete"

echo "Building EDSEngineNET..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "EDSEngineNET\Release\*.dll" VC11 /y
copy "EDSEngineNET\x64\Release\*.dll" VC11\x64 /y
echo "EDSEngineNET build complete"

echo "Building EDSEngineNETTestApp..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2012\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2012\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2012\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2012\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "EDSEngineNET\EDSEngineNETTestApp2012\bin\x86\Release\EDSEngineNETTestApp.exe" VC11 /y
copy "EDSEngineNET\EDSEngineNETTestApp2012\bin\x64\Release\EDSEngineNETTestApp.exe" VC11\x64 /y
echo "EDSEngineNETTestApp build complete"

echo "Building Relational Object Model..."
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "RelationalObjectModel\Release MSXML\*.lib" VC11 /y
copy "RelationalObjectModel\x64\Release MSXML\*.lib" VC11\x64 /y
echo "Relational Object Model build complete"

echo "Building ROMAppConsoleTest..."
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "RelationalObjectModel\ROMAppConsoleTest\Release MSXML\ROMAppConsoleTest2012.exe" VC11 /y
copy "RelationalObjectModel\ROMAppConsoleTest\x64\Release MSXML\ROMAppConsoleTest2012.exe" VC11\x64 /y
echo "ROMAppConsoleTest build complete"

echo "Building ROMNET..."
%MSBUILD_PATH% "ROMNET\ROMNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "ROMNET\ROMNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=Win32
%MSBUILD_PATH% "ROMNET\ROMNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "ROMNET\ROMNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "ROMNET\Release\*.dll" VC11 /y
copy "ROMNET\x64\Release\*.dll" VC11\x64 /y
echo "ROMNET build complete"

echo "Building ROMNETTestApplication..."
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2012\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2012\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2012\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2012\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "ROMNET\ROMNETTestApplication2012\bin\x86\Release\ROMNETTestApplication.exe" VC11 /y
copy "ROMNET\ROMNETTestApplication2012\bin\x64\Release\ROMNETTestApplication.exe" VC11\x64 /y
echo "ROMNETTestApplication build complete"

echo "Building DecisionLogic..."
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:clean,rebuild /p:Configuration="Unicode Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:clean,rebuild /p:Configuration="Unicode Release" /p:VisualStudioVersion=11.0
echo "DecisionLogic build complete"

echo "Building Utilities"
echo "Building LogicianDebuggerWPF"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2012.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2012.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2012.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2012.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "Utilities\LogicianDebuggerWPF\bin\x86\Release\LogicianDebuggerWPF.dll" VC11 /y
copy "Utilities\LogicianDebuggerWPF\bin\x86\Release\WPFToolkit.dll" VC11 /y
copy "Utilities\LogicianDebuggerWPF\bin\x64\Release\LogicianDebuggerWPF.dll" VC11\x64 /y
copy "Utilities\LogicianDebuggerWPF\bin\x64\Release\WPFToolkit.dll" VC11\x64 /y
echo "Building LogicianDebuggerWPF complete"

echo "Building LogicianDebuggerTestApp2012..."
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\LogicianDebuggerTestApp2012.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\LogicianDebuggerTestApp2012.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x86
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\LogicianDebuggerTestApp2012.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0 /p:Platform=x64
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\LogicianDebuggerTestApp2012.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0 /p:Platform=x64
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\bin\x86\Release\LogicianDebuggerTestApp2012.exe" VC11 /y
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\config.xml" VC11 /y
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\bin\x64\Release\LogicianDebuggerTestApp2012.exe" VC11\x64 /y
copy "Utilities\LogicianDebuggerWPF\LogicianDebuggerTestApp2012\config.xml" VC11\x64 /y
echo "Building LogicianDebuggerTestApp2012 complete"

echo "All builds complete"
pause
