Conditional references applied for x86 and x64 platforms:
<Reference Include="ROMWinRT" Condition="'$(Platform)'=='x86'">
  <SpecificVersion>False</SpecificVersion>
  <HintPath>..\Release\ROMWinRT\ROMWinRT.winmd</HintPath>
  <IsWinMDFile>true</IsWinMDFile>
</Reference>
<Reference Include="ROMWinRT" Condition="'$(Platform)'=='x64'">
  <SpecificVersion>False</SpecificVersion>
  <HintPath>..\x64\Release\ROMWinRT\ROMWinRT.winmd</HintPath>
  <IsWinMDFile>true</IsWinMDFile>
</Reference>
	