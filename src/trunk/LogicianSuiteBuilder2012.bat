@echo off
SET MSBUILD_PATH=%systemroot%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe
echo "MSBUILD.exe Path: %MSBUILD_PATH%"
echo "Creating VS2012 bin folder..."
mkdir VC11

echo "Building EDSEngine..."
%MSBUILD_PATH% "EDSEngine\EDSEngine2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "EDSEngine\EDSEngine2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0
copy "EDSEngine\Release MSXML\*.lib" VC11 /y
echo "EDSEngine build complete"

echo "Building EDSEngineTestApp..."
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "EDSEngine\EDSEngineTestApp\EDSEngineTestApp2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0
echo "EDSEngineTestApp build complete"

echo "Building EDSEngineNET..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0
copy "EDSEngineNET\Release\*.dll" VC11 /y
echo "EDSEngineNET build complete"

echo "Building EDSEngineNETTestApp..."
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2012\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "EDSEngineNET\EDSEngineNETTestApp2012\EDSEngineNETTestApp.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0
echo "EDSEngineNETTestApp build complete"

echo "Building Relational Object Model..."
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "RelationalObjectModel\Relational Object Model 2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0
copy "RelationalObjectModel\Release MSXML\*.lib" VC11 /y
echo "Relational Object Model build complete"

echo "Building ROMAppConsoleTest..."
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug MSXML" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "RelationalObjectModel\ROMAppConsoleTest\ROMAppConsoleTest2012.vcxproj" /t:clean,rebuild /p:Configuration="Release MSXML" /p:VisualStudioVersion=11.0
echo "ROMAppConsoleTest build complete"

echo "Building ROMNET..."
%MSBUILD_PATH% "ROMNET\ROMNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "ROMNET\ROMNET2012.vcxproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0
copy "ROMNET\Release\*.dll" VC11 /y
echo "ROMNET build complete"

echo "Building ROMNETTestApplication..."
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2012\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "ROMNET\ROMNETTestApplication2012\ROMNETTestApplication.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0
echo "ROMNETTestApplication build complete"

echo "Building DecisionLogic..."
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:clean,rebuild /p:Configuration="Unicode Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "DecisionLogic\DecisionLogic.vcxproj" /t:clean,rebuild /p:Configuration="Unicode Release" /p:VisualStudioVersion=11.0
echo "DecisionLogic build complete"

echo "Building Utilities"
echo "Building LogicianDebuggerWPF"
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger.csproj" /t:clean,rebuild /p:Configuration="Debug" /p:VisualStudioVersion=11.0
%MSBUILD_PATH% "Utilities\LogicianDebuggerWPF\LogicianDebugger.csproj" /t:clean,rebuild /p:Configuration="Release" /p:VisualStudioVersion=11.0
copy "Utilities\LogicianDebuggerWPF\bin\Release\LogicianDebuggerWPF.dll" VC11 /y
copy "Utilities\LogicianDebuggerWPF\bin\Release\WPFToolkit.dll" VC11 /y
echo "Building LogicianDebuggerWPF complete"

echo "All builds complete"
pause
