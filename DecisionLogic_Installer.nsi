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
InstallDir $PROGRAMFILES\DecisionLogic

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
  SetOutPath $INSTDIR
  ; Put file there
  File "DecisionLogic\vc_mswu\DecisionLogic.exe"

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\DecisionLogic "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "DisplayName" "DecisionLogic"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DecisionLogic" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

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

  ; Remove files and uninstaller
  Delete $INSTDIR\DecisionLogic.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\DecisionLogic\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\DecisionLogic"
  RMDir "$INSTDIR"

SectionEnd
