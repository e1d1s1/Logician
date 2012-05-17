; DecisionLogic_Installer.nsi
;
; 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install hte Logician Suite into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Logician Suite (Dev)"

; The file to write
OutFile "Logician_Setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Logician

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Logician" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------



; The stuff to install
Section "Visual C++ 2008 Runtime"
	; VS2008 Redist
	SetOutPath $TEMP
	File "vcredist_x86.exe"
	Exec '/q:a /c:"vcredist_x86.exe /q"'
	Delete /REBOOTOK "vcredist_x86.exe"
SectionEnd

Section "Logician Suite"
	; Set output path to the installation directory.
	SetOutPath $INSTDIR

	;3rd party libs
	File "iconv-1.9.2.win32\bin\iconv.dll"
	File "iconv-1.9.2.win32\lib\iconv.lib"
	File "iconv-1.9.2.win32\lib\iconv_a.lib"
	File "libxml2-2.7.3.win32\bin\libxml2.dll"
	File "libxml2-2.7.3.win32\lib\libxml2.lib"
	File "libxml2-2.7.3.win32\lib\libxml2_a.lib"
	File "libxml2-2.7.3.win32\lib\libxml2_a_dll.lib"
	File "libxslt-1.1.24.win32\bin\libexslt.dll"
	File "libxslt-1.1.24.win32\bin\libxslt.dll"
	File "libxslt-1.1.24.win32\lib\libexslt.lib"
	File "libxslt-1.1.24.win32\lib\libexslt_a.lib"
	File "libxslt-1.1.24.win32\lib\libxslt.lib"
	File "libxslt-1.1.24.win32\lib\libxslt_a.lib"
	File "zlib-1.2.3.win32\bin\zlib1.dll"
	File "zlib-1.2.3.win32\lib\zdll.lib"
	File "zlib-1.2.3.win32\lib\zlib.lib"
	
	File "..\trunk\EDSEngine\Release MSXML\EDSEngine.lib"
	File "..\trunk\EDSEngineNET\Release\EDSEngineNET.dll"
	File "..\trunk\Relational Object Model\Release MSXML\Relational Object Model.lib"
	File "..\trunk\ROMNET\Release\ROMNET.dll"
	File "..\trunk\LogicianJS\KnowledgeBase.js"
	File "..\trunk\LogicianJS\ROMNode.js"
	File "..\trunk\LogicianJS\LogicianSilverlight\LogicianSilverlight\Bin\Release\LogicianSilverlight.dll"
	;File "..\trunk\LogicianJS\Flash\LogicianFlash\bin\LogicianFlash.swc"
	File "..\trunk\Utilities\LogicianDebuggerWPF\bin\Release\LogicianDebuggerWPF.dll"

	; Set output path to the installation directory.
	SetOutPath $INSTDIR\DecisionLogic
	; Put file there
	File "..\trunk\DecisionLogic\vc_mswu\DecisionLogic.exe"
	File "..\trunk\DecisionLogic\vc_mswu\DecisionLogicHelp.htm"
	File "..\trunk\DecisionLogic\vc_mswu\Figure1.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure2.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure3.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure4.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure5.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure6.png"

	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\DecisionLogic "Install_Dir" "$INSTDIR"
	
	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Logician" "DisplayName" "Logician Libraries"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Logician" "UninstallString" '"$INSTDIR\uninstall_logician_libraries.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Logician" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Logician" "NoRepair" 1
	WriteUninstaller "uninstall_logician_libraries.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
	CreateDirectory "$SMPROGRAMS\DecisionLogic"
	CreateShortCut "$SMPROGRAMS\DecisionLogic\Uninstall.lnk" "$INSTDIR\uninstall_logician_libraries.exe" "" "$INSTDIR\DecisionLogic\uninstall_logician_libraries.exe" 0
	CreateShortCut "$SMPROGRAMS\DecisionLogic\DecisionLogic.lnk" "$INSTDIR\DecisionLogic\DecisionLogic.exe" "" "$INSTDIR\DecisionLogic\DecisionLogic.exe" 0
SectionEnd

;--------------------------------

; Uninstaller

Section "un.LogicianSuite"

	; Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Logician"
	DeleteRegKey HKLM SOFTWARE\Logician
	DeleteRegKey HKCU SOFTWARE\Logician

	; Remove files and uninstaller
	Delete $INSTDIR\DecisionLogic\*.*
	Delete $INSTDIR\*.*
	Delete $INSTDIR\uninstall_logician_libraries.exe
	
	; Remove shortcuts, if any
	Delete "$SMPROGRAMS\DecisionLogic\*.*"

	; Remove directories used
	RMDir "$SMPROGRAMS\DecisionLogic"
	RMDir "$INSTDIR\DecisionLogic"
	RMDir "$INSTDIR"

SectionEnd