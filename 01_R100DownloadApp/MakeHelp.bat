@echo off
REM This batch file will not work when called by MSVC 4.x's internal build 
REM system if the path to your MSDEV\bin directory requires double quotes 
REM (i.e. \Program Files\MSDEV\Bin requires double quotes because of the 
REM space between Program and Files). If MSDEV\bin is on your system's 
REM path, you can, however, run this batch file from the command line.
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by _Setup.hpj. >"hlp\setup.hm"
echo. >>"hlp\setup.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\setup.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\setup.hm"
echo. >>"hlp\setup.hm"
echo // Prompts (IDP_*) >>"hlp\setup.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\setup.hm"
echo. >>"hlp\setup.hm"
echo // Resources (IDR_*) >>"hlp\setup.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\setup.hm"
echo. >>"hlp\setup.hm"
echo // Dialogs (IDD_*) >>"hlp\setup.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\setup.hm"
echo. >>"hlp\setup.hm"
echo // Frame Controls (IDW_*) >>"hlp\setup.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\setup.hm"
REM -- Make help for Project setup


echo Building Win32 Help files
start /wait hcrtf -x "hlp\_Setup.hpj"
echo.
if exist Debug\nul copy "hlp\setup.hlp" Debug
if exist Release\nul copy "hlp\setup.hlp" Release
echo.
