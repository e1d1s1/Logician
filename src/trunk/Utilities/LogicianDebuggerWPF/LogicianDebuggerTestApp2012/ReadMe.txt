Conditional references applied for x86 and x64 platforms:
<Reference Include="EDSEngineNET.dll" Condition="'$(Platform)'=='x86'">
  <SpecificVersion>False</SpecificVersion>
  <HintPath>..\Release\EDSEngineNET.dll</HintPath>
</Reference>
<Reference Include="EDSEngineNET.dll" Condition="'$(Platform)'=='x64'">
  <SpecificVersion>False</SpecificVersion>
  <HintPath>..\x64\Release\EDSEngineNET.dll</HintPath>
</Reference>
	