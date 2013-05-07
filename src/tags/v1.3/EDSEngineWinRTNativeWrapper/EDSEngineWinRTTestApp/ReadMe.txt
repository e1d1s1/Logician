Conditional references applied for x86 and x64 platforms:
<Reference Include="EDSEngineNET" Condition="'$(Platform)'=='x86'">
  <SpecificVersion>False</SpecificVersion>
  <HintPath>..\Release\EDSEngineWinRT\EDSEngineWinRT.winmd</HintPath>
  <IsWinMDFile>true</IsWinMDFile>
</Reference>
<Reference Include="EDSEngineNET" Condition="'$(Platform)'=='x64'">
  <SpecificVersion>False</SpecificVersion>
  <HintPath>..\Release\EDSEngineWinRT\EDSEngineWinRT.winmd</HintPath>
  <IsWinMDFile>true</IsWinMDFile>
</Reference>
	