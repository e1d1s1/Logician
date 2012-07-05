; DecisionLogic_Installer.nsi
;
; 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install hte Logician Suite into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Logician Suite 1.0.7"

; The file to write
OutFile "Logician_v107_Setup.exe"

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

Section "Logician Suite"
	; Set output path to the installation directory.
	SetOutPath $INSTDIR\VC9
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

	File "..\tags\v1.0.7\VC9\EDSEngine.lib"
	File "..\tags\v1.0.7\VC9\EDSEngineNET.dll"
	File "..\tags\v1.0.7\VC9\RelationalObjectModel.lib"
	File "..\tags\v1.0.7\VC9\ROMNET.dll"
	File "..\tags\v1.0.7\VC9\LogicianDebuggerWPF.dll"
	File "..\tags\v1.0.7\VC9\WPFToolkit.dll"
	
	
	SetOutPath $INSTDIR\VC10
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
	
	File "..\tags\v1.0.7\VC10\EDSEngine.lib"
	File "..\tags\v1.0.7\VC10\EDSEngineNET.dll"
	File "..\tags\v1.0.7\VC10\RelationalObjectModel.lib"
	File "..\tags\v1.0.7\VC10\ROMNET.dll"
	File "..\tags\v1.0.7\VC10\LogicianDebuggerWPF.dll"
	File "..\tags\v1.0.7\VC10\WPFToolkit.dll"
	File "..\tags\v1.0.7\LogicianJS\LogicianSilverlight\LogicianSilverlight\Bin\Release\LogicianSilverlight.dll"

	; Set output path to the installation directory.
	SetOutPath $INSTDIR\DecisionLogic
	; Put file there
	File "..\tags\v1.0.7\VC9\DecisionLogic.exe"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\DecisionLogicHelp.htm"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\Figure1.png"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\Figure2.png"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\Figure3.png"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\Figure4.png"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\Figure5.png"
	File "..\tags\v1.0.7\DecisionLogic\vc_mswu\Figure6.png"
	
	SetOutPath $INSTDIR
	File "..\tags\v1.0.7\LogicianJS\KnowledgeBase.js"
	File "..\tags\v1.0.7\LogicianJS\ROMNode.js"
	;File "..\tags\v1.0.7\LogicianJS\Flash\LogicianFlash\bin\LogicianFlash.swc"
	
	SetOutPath $INSTDIR\ajaxslt
	File "..\tags\v1.0.7\LogicianJS\ajaxslt\*.*"	
	
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
	SetOutPath $INSTDIR\DecisionLogic
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
	RMDir "$INSTDIR\ajaxslt"
	RMDir "$INSTDIR"

SectionEnd