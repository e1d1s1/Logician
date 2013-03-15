@echo off
SET MSBUILD_PATH=%systemroot%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe
echo "MSBUILD.exe Path: %MSBUILD_PATH%"
echo "Creating VS2010 bin folder..."
mkdir VC10


echo "Building EDSEngine..."
%MSBUILD_PATH% "EDSEngine\EDSEngine.vcxproj" /t:rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "EDSEngine\EDSEngine.vcxproj" /t:rebuild /p:Configuration="Release MSXML"
copy "EDSEngine\Release MSXML\*.lib" VC10 /y
echo "EDSEngine build complete"

echo "Building EDSEngineTestApp..."
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp.vcxproj" /t:rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp.vcxproj" /t:rebuild /p:Configuration="Release MSXML"
echo "EDSEngineTestApp build complete"

echo "Building EDSEngineNET..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET.vcxproj" /t:rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET.vcxproj" /t:rebuild /p:Configuration="Release"
copy "EDSEngineNET\Release\*.dll" VC10 /y
echo "EDSEngineNET build complete"

echo "Building EDSEngineNETTestApp..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp\EDSEngineNETTestApp.csproj" /t:rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp\EDSEngineNETTestApp.csproj" /t:rebuild /p:Configuration="Release"
echo "EDSEngineNETTestApp build complete"

echo "Building LogicianJS..."
%MSBUILD_PATH% "LogicianJS\LogicianJS.csproj" /t:rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "LogicianJS\LogicianJS.csproj" /t:rebuild /p:Configuration="Release"
echo "LogicianJS build complete"


echo "Building Relational Object Model..."
%MSBUILD_PATH% "Relational Object Model\Relational Object Model.vcxproj" /t:rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "Relational Object Model\Relational Object Model.vcxproj" /t:rebuild /p:Configuration="Release MSXML"
copy "Relational Object Model\Release MSXML\*.lib" VC10 /y
echo "Relational Object Model build complete"

echo "Building ROMAppConsoleTest..."
%MSBUILD_PATH% "Relational Object Model\ROMAppConsoleTest\ROMAppConsoleTest.vcxproj" /t:rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "Relational Object Model\ROMAppConsoleTest\ROMAppConsoleTest.vcxproj" /t:rebuild /p:Configuration="Release MSXML"
echo "ROMAppConsoleTest build complete"

echo "Building ROMNET..."
%MSBUILD_PATH% "ROMNET\ROMNET.vcxproj" /t:rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "ROMNET\ROMNET.vcxproj" /t:rebuild /p:Configuration="Release"
copy "ROMNET\Release\*.dll" VC10 /y
echo "ROMNET build complete"

echo "Building ROMNETTestApplication..."
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication\ROMNETTestApplication.csproj" /t:rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication\ROMNETTestApplication.csproj" /t:rebuild /p:Configuration="Release"
echo "ROMNETTestApplication build complete"

echo "Building DecisionLogic..."
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:rebuild /p:Configuration="Unicode Debug"
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:rebuild /p:Configuration="Unicode Release"
echo "DecisionLogic build complete"

echo "Building Utilities"
echo "Building LogicianDebuggerWPF"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger.csproj" /t:rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger.csproj" /t:rebuild /p:Configuration="Release"
copy "Utilities\LogicianDebuggerWPF\bin\Release\*.dll" VC10 /y
echo "Building LogicianDebuggerWPF complete"

echo "Building LogicianDebugger"
%MSBUILD_PATH% "Utilities\LogicianDebugger\ROMDebugApp.vcxproj" /t:rebuild /p:Configuration="Unicode Debug"
%MSBUILD_PATH% "Utilities\LogicianDebugger\ROMDebugApp.vcxproj" /t:rebuild /p:Configuration="Unicode Release"
echo "LogicianDebugger build complete"

echo "All builds complete"
pause
