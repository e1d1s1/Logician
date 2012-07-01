@echo off
SET MSBUILD_PATH=%systemroot%\Microsoft.NET\Framework\v3.5\msbuild.exe
echo "MSBUILD.exe Path: %MSBUILD_PATH%"
echo "Creating VS2008 bin folder..."
mkdir VC9

echo "Building EDSEngine..."
%MSBUILD_PATH% "EDSEngine\EDSEngine.vcproj" /t:clean,rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "EDSEngine\EDSEngine.vcproj" /t:clean,rebuild /p:Configuration="Release MSXML"
copy "EDSEngine\Release MSXML\*.lib" VC9 /y
echo "EDSEngine build complete"

echo "Building EDSEngineTestApp..."
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp.vcproj" /t:clean,rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp.vcproj" /t:clean,rebuild /p:Configuration="Release MSXML"
echo "EDSEngineTestApp build complete"

echo "Building EDSEngineNET..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET.vcproj" /t:clean,rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET.vcproj" /t:clean,rebuild /p:Configuration="Release"
copy "EDSEngineNET\Release\*.dll" VC9 /y
echo "EDSEngineNET build complete"

echo "Building EDSEngineNETTestApp..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2008\EDSEngineNETTestApp2008.csproj" /t:clean,rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2008\EDSEngineNETTestApp2008.csproj" /t:clean,rebuild /p:Configuration="Release"
echo "EDSEngineNETTestApp build complete"

echo "Building Relational Object Model..."
%MSBUILD_PATH% "Relational Object Model\Relational Object Model.vcproj" /t:clean,rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "Relational Object Model\Relational Object Model.vcproj" /t:clean,rebuild /p:Configuration="Release MSXML"
copy "Relational Object Model\Release MSXML\*.lib" VC9 /y
echo "Relational Object Model build complete"

echo "Building ROMAppConsoleTest..."
%MSBUILD_PATH% "Relational Object Model\ROMAppConsoleTest\ROMAppConsoleTest.vcproj" /t:clean,rebuild /p:Configuration="Debug MSXML"
%MSBUILD_PATH% "Relational Object Model\ROMAppConsoleTest\ROMAppConsoleTest.vcproj" /t:clean,rebuild /p:Configuration="Release MSXML"
echo "ROMAppConsoleTest build complete"

echo "Building ROMNET..."
%MSBUILD_PATH% "ROMNET\ROMNET.vcproj" /t:clean,rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "ROMNET\ROMNET.vcproj" /t:clean,rebuild /p:Configuration="Release"
copy "ROMNET\Release\*.dll" VC9 /y
echo "ROMNET build complete"

echo "Building ROMNETTestApplication..."
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2008\ROMNETTestApplication2008.csproj" /t:clean,rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2008\ROMNETTestApplication2008.csproj" /t:clean,rebuild /p:Configuration="Release"
echo "ROMNETTestApplication build complete"

echo "Building DecisionLogic..."
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcproj" /t:clean,rebuild /p:Configuration="Unicode Debug"
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcproj" /t:clean,rebuild /p:Configuration="Unicode Release"
copy "DecisionLogic\vc_mswu\*.exe" VC9 /y
echo "DecisionLogic build complete"

echo "Building Utilities"
echo "Building LogicianDebuggerWPF"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2008.csproj" /t:clean,rebuild /p:Configuration="Debug"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger2008.csproj" /t:clean,rebuild /p:Configuration="Release"
copy "Utilities\LogicianDebuggerWPF\bin\Release\LogicianDebuggerWPF.dll" VC9 /y
copy "Utilities\LogicianDebuggerWPF\bin\Release\WPFToolkit.dll" VC9 /y
echo "Building LogicianDebuggerWPF complete"

echo "All builds complete"
pause
