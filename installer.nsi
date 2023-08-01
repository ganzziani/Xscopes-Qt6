############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
############################################################################################

!define APP_NAME "XScopes Interface"
!define COMP_NAME "Gabotronics"
!define WEB_SITE "https://gabotronics.com/"
!define VERSION "00.00.01.01"
!define COPYRIGHT "Gabotronics © 2023"
!define DESCRIPTION "Application"
!define LICENSE_TXT "exe\gpl-3.0.txt"
!define INSTALLER_NAME "Output\XScopes-Installer.exe"
!define MAIN_APP_EXE "Xscopes.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define REG_START_MENU "Start Menu Folder"

var SM_Folder

######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
Icon "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\Gabotronics\XScopes"

######################################################################

!include "MUI.nsh"
!include "FileFunc.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!insertmacro MUI_PAGE_DIRECTORY

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Gabotronics"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${MAIN_APP_EXE}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetOutPath "$INSTDIR"

File "exe\gpl-3.0.txt"
File "exe\Qt6Core.dll"
File "exe\Qt6Gui.dll"
File "exe\Qt6Widgets.dll"
File "exe\Qt6Network.dll"
File "exe\ReadMe.TXT"
File "exe\Xscopes.exe"
File "exe\dfu-programmer.exe"
SetOutPath "$INSTDIR\docs"
File "exe\docs\xscopes-manual.pdf"
SetOutPath "$INSTDIR\iconengines"
File "exe\iconengines\qsvgicon.dll"
SetOutPath "$INSTDIR\imageformats"
File "exe\imageformats\qgif.dll"
File "exe\imageformats\qico.dll"
File "exe\imageformats\qjpeg.dll"
File "exe\imageformats\qsvg.dll"
SetOutPath "$INSTDIR\platforms"
File "exe\platforms\qwindows.dll"
SetOutPath "$INSTDIR\tls"
File "exe\tls\qcertonlybackend.dll"
File "exe\tls\qopensslbackend.dll"
File "exe\tls\qschannelbackend.dll"
SetOutPath "$INSTDIR\styles"
File "exe\styles\qwindowsvistastyle.dll"
${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
IntFmt $0 "0x%08X" $0
WriteRegDWORD ${REG_ROOT} "${UNINSTALL_PATH}" "EstimatedSize" "$0"

SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\$SM_Folder"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\XScopes_Manual.lnk" "$INSTDIR\docs\xscopes-manual.pdf"
CreateShortCut "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\Gabotronics\XScopes"
CreateShortCut "$SMPROGRAMS\Gabotronics\XScopes\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\Gabotronics\XScopes\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}

Delete "$INSTDIR\gpl-3.0.txt"
Delete "$INSTDIR\Qt6Core.dll"
Delete "$INSTDIR\Qt6Gui.dll"
Delete "$INSTDIR\Qt6Widgets.dll"
Delete "$INSTDIR\Qt6Network.dll"
Delete "$INSTDIR\ReadMe.TXT"
Delete "$INSTDIR\XScopes.exe"
Delete "$INSTDIR\dfu-programmer.exe"
Delete "$INSTDIR\platforms\qwindows.dll"
Delete "$INSTDIR\imageformats\qgif.dll"
Delete "$INSTDIR\imageformats\qico.dll"
Delete "$INSTDIR\imageformats\qjpeg.dll"
Delete "$INSTDIR\imageformats\qsvg.dll"
Delete "$INSTDIR\iconengines\qsvgicon.dll"
Delete "$INSTDIR\styles\qwindowsvistastyle.dll"
Delete "$INSTDIR\tls\qcertonlybackend.dll"
Delete "$INSTDIR\tls\qopensslbackend.dll"
Delete "$INSTDIR\tls\qschannelbackend.dll"
Delete "$INSTDIR\firmware\*.*"
 
RmDir "$INSTDIR\platforms"
RmDir "$INSTDIR\imageformats"
RmDir "$INSTDIR\iconengines"
RmDir "$INSTDIR\styles"
RmDir "$INSTDIR\tls"
RmDir "$INSTDIR\firmware"
Delete "$INSTDIR\uninstall.exe"

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\XScopes_Manual.lnk"
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\Gabotronics\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\Gabotronics\XScopes\Uninstall ${APP_NAME}.lnk"
Delete "$SMPROGRAMS\Gabotronics\XSCopes\XScopes_Manual.lnk"
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\Gabotronics\XScopes"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

