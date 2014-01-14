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
	ExecWait '"vcredist_x86.exe" /q:a /c:"VCREDI~1.EXE /q:a /c:""msiexec /i vcredist.msi /qb!"" "'
	Delete /REBOOTOK "vcredist_x86.exe"
SectionEnd

Section "Visual C++ 2013 Runtime"
	; VS2013 Redist
	SetOutPath $TEMP
	File "vcredist_x86.exe"
	ExecWait '"vcredist_x86.exe" /q:a /c:"VCREDI~1.EXE /q:a /c:""msiexec /i vcredist.msi /qb!"" "'
	Delete /REBOOTOK "vcredist_x86.exe"
SectionEnd

Section "Logician Suite"
	; Set output path to the installation directory.
	SetOutPath $INSTDIR\VC11
	File "..\trunk\VC12\EDSEngine.lib"
	File "..\trunk\VC12\EDSEngineNET.dll"
	File "..\trunk\VC12\RelationalObjectModel.lib"
	File "..\trunk\VC12\ROMNET.dll"
	File "..\trunk\VC12\LogicianDebuggerWPF.dll"
	File "..\trunk\VC12\WPFToolkit.dll"
	File "..\trunk\LogicianJS\LogicianSilverlight\LogicianSilverlight\Bin\Release\LogicianSilverlight.dll"
	SetOutPath $INSTDIR\VC12\EDSEngineWinRT
	File "..\trunk\VC12\EDSEngineWinRT\EDSEngineWinRT.winmd"
	File "..\trunk\VC12\EDSEngineWinRT\EDSEngineWinRT.pri"
	File "..\trunk\VC12\EDSEngineWinRT\EDSEngineWinRT.lib"
	File "..\trunk\VC12\EDSEngineWinRT\EDSEngineWinRT.exp"
	File "..\trunk\VC12\EDSEngineWinRT\EDSEngineWinRT.dll"
	SetOutPath $INSTDIR\VC12\ROMWinRT
	File "..\trunk\VC12\ROMWinRT\ROMWinRT.winmd"
	File "..\trunk\VC12\ROMWinRT\ROMWinRT.pri"
	File "..\trunk\VC12\ROMWinRT\ROMWinRT.lib"
	File "..\trunk\VC12\ROMWinRT\ROMWinRT.exp"
	File "..\trunk\VC12\ROMWinRT\ROMWinRT.dll"
	SetOutPath $INSTDIR\VC12\x64
	File "..\trunk\VC12\x64\EDSEngine.lib"
	File "..\trunk\VC12\x64\EDSEngineNET.dll"
	File "..\trunk\VC12\x64\RelationalObjectModel.lib"
	File "..\trunk\VC12\x64\ROMNET.dll"
	File "..\trunk\VC12\x64\LogicianDebuggerWPF.dll"
	File "..\trunk\VC12\x64\WPFToolkit.dll"
	SetOutPath $INSTDIR\VC12\x64\EDSEngineWinRT
	File "..\trunk\VC12\x64\EDSEngineWinRT\EDSEngineWinRT.winmd"
	File "..\trunk\VC12\x64\EDSEngineWinRT\EDSEngineWinRT.pri"
	File "..\trunk\VC12\x64\EDSEngineWinRT\EDSEngineWinRT.lib"
	File "..\trunk\VC12\x64\EDSEngineWinRT\EDSEngineWinRT.exp"
	File "..\trunk\VC12\x64\EDSEngineWinRT\EDSEngineWinRT.dll"	
	SetOutPath $INSTDIR\VC12\x64\ROMWinRT
	File "..\trunk\VC12\x64\ROMWinRT\ROMWinRT.winmd"
	File "..\trunk\VC12\x64\ROMWinRT\ROMWinRT.pri"
	File "..\trunk\VC12\x64\ROMWinRT\ROMWinRT.lib"
	File "..\trunk\VC12\x64\ROMWinRT\ROMWinRT.exp"
	File "..\trunk\VC12\x64\ROMWinRT\ROMWinRT.dll"

	; Set output path to the installation directory.
	SetOutPath $INSTDIR\DecisionLogic
	; Put file there
	File "..\trunk\VC9\DecisionLogic.exe"
	File "..\trunk\DecisionLogic\vc_mswu\DecisionLogicHelp.htm"
	File "..\trunk\DecisionLogic\vc_mswu\Figure1.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure2.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure3.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure4.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure5.png"
	File "..\trunk\DecisionLogic\vc_mswu\Figure6.png"
	
	SetOutPath $INSTDIR
	File "..\trunk\LogicianJS\KnowledgeBase.js"
	File "..\trunk\LogicianJS\ROMNode.js"
	;File "..\trunk\LogicianJS\Flash\LogicianFlash\bin\LogicianFlash.swc"
	
	SetOutPath $INSTDIR\ajaxslt
	File "..\trunk\LogicianJS\ajaxslt\*.*"	
	
	SetOutPath $INSTDIR
	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\DecisionLogic "Install_Dir" "$INSTDIR"
	
	; Write the file association for DecisionLogic to registry
	WriteRegStr HKCR .dlp "" "DecisionLogicProject"
	WriteRegStr HKCR DecisionLogicProject\shell\open\command "" '$INSTDIR\DecisionLogic\DecisionLogic.exe "%1"'
	WriteRegStr HKCR DecisionLogicProject\DefaultIcon "" '$INSTDIR\DecisionLogic\DecisionLogic.exe'
	
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
	DeleteRegKey HKCR .dlp
	DeleteRegKey HKCR DecisionLogicProject

	; Remove files and uninstaller
	Delete $INSTDIR\DecisionLogic\*.*
	Delete $INSTDIR\VC12\x64\EDSEngineWinRT\*.*
	Delete $INSTDIR\VC12\x64\ROMWinRT\*.*
	Delete $INSTDIR\VC12\x64\*.*
	Delete $INSTDIR\VC12\EDSEngineWinRT\*.*
	Delete $INSTDIR\VC12\ROMWinRT\*.*
	Delete $INSTDIR\VC11\x64\EDSEngineWinRT\*.*
	Delete $INSTDIR\VC11\x64\ROMWinRT\*.*
	Delete $INSTDIR\VC11\x64\*.*
	Delete $INSTDIR\VC11\EDSEngineWinRT\*.*
	Delete $INSTDIR\VC11\ROMWinRT\*.*
	Delete $INSTDIR\VC12\*.*
	Delete $INSTDIR\VC11\*.*
	Delete $INSTDIR\VC10\*.*
	Delete $INSTDIR\VC9\*.*
	Delete $INSTDIR\ajaxslt\*.*
	Delete $INSTDIR\*.*
	Delete $INSTDIR\uninstall_logician_libraries.exe
	
	; Remove shortcuts, if any
	Delete "$SMPROGRAMS\DecisionLogic\*.*"

	; Remove directories used
	RMDir "$SMPROGRAMS\DecisionLogic"
	RMDir "$INSTDIR\DecisionLogic"
	RMDir "$INSTDIR\VC9"
	RMDir "$INSTDIR\VC10"
	RMDir "$INSTDIR\VC12\x64\EDSEngineWinRT"
	RMDir "$INSTDIR\VC12\x64\ROMWinRT"
	RMDir "$INSTDIR\VC12\x64"
	RMDir "$INSTDIR\VC12\EDSEngineWinRT"
	RMDir "$INSTDIR\VC12\ROMWinRT"
	RMDir "$INSTDIR\VC11"
	RMDir "$INSTDIR\ajaxslt"
	RMDir "$INSTDIR"

SectionEnd