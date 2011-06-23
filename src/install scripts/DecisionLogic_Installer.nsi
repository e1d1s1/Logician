; DecisionLogic_Installer.nsi
;
; 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install DecisionLogic.exe into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "DecisionLogic"

; The file to write
OutFile "DecisionLogic_Setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Logician

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\DecisionLogic" "Install_Dir"

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
Section "DecisionLogic (required)"
	SectionIn RO

	; VS2008 Redist
	SetOutPath $TEMP
	File "vcredist_x86.exe"
	Exec '/q:a /c:"vcredist_x86.exe /q"'
	Delete /REBOOTOK "vcredist_x86.exe"

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
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "DisplayName" "DecisionLogic"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "UninstallString" '"$INSTDIR\uninstall_decisionlogic.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "NoRepair" 1
	WriteUninstaller "uninstall_decisionlogic.exe"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
	CreateDirectory "$SMPROGRAMS\DecisionLogic"
	CreateShortCut "$SMPROGRAMS\DecisionLogic\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\DecisionLogic\DecisionLogic.lnk" "$INSTDIR\DecisionLogic.exe" "" "$INSTDIR\DecisionLogic.exe" 0
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

	; Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic"
	DeleteRegKey HKLM SOFTWARE\DecisionLogic
	DeleteRegKey HKCU SOFTWARE\DecisionLogic

	; Remove files and uninstaller
	Delete $INSTDIR\DecisionLogic\*.*
	Delete $INSTDIR\uninstall_decisionlogic.exe

	; Remove shortcuts, if any
	Delete "$SMPROGRAMS\DecisionLogic\*.*"

	; Remove directories used
	RMDir "$SMPROGRAMS\DecisionLogic"
	RMDir "$INSTDIR\DecisionLogic"
  
	; Remove directories used
	RMDir "$INSTDIR"

SectionEnd