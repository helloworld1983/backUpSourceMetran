# Microsoft Developer Studio Project File - Name="setup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=setup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "setup.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "setup.mak" CFG="setup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "setup - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "setup - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "setup - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "setup - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\current\Util" /I "..\current\System" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "setup - Win32 Release"
# Name "setup - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_Setup.cpp
# End Source File
# Begin Source File

SOURCE=.\_Setup.rc
# End Source File
# Begin Source File

SOURCE=.\BOOTFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\COMMAND.CPP
# End Source File
# Begin Source File

SOURCE=.\Commport.cpp
# End Source File
# Begin Source File

SOURCE=.\CRC.CPP
# End Source File
# Begin Source File

SOURCE=.\dlgGetSerialNumber.cpp
# End Source File
# Begin Source File

SOURCE=.\ExitSuccess.cpp
# End Source File
# Begin Source File

SOURCE=.\Installer.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\SetupHardware.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupLang.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupWelcome.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_Setup.h
# End Source File
# Begin Source File

SOURCE=.\BOOTFILE.H
# End Source File
# Begin Source File

SOURCE=.\COMMAND.H
# End Source File
# Begin Source File

SOURCE=.\COMMPORT.H
# End Source File
# Begin Source File

SOURCE=.\CRC.H
# End Source File
# Begin Source File

SOURCE=.\dlgGetSerialNumber.h
# End Source File
# Begin Source File

SOURCE=.\DownloadProtocol.h
# End Source File
# Begin Source File

SOURCE=.\ExitSuccess.h
# End Source File
# Begin Source File

SOURCE=.\Get_Serial_Number.h
# End Source File
# Begin Source File

SOURCE=.\GetSerialNumber.h
# End Source File
# Begin Source File

SOURCE=.\Installer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SetupHardware.h
# End Source File
# Begin Source File

SOURCE=.\SetupLang.h
# End Source File
# Begin Source File

SOURCE=.\SetupProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupSheet.h
# End Source File
# Begin Source File

SOURCE=.\SetupType.h
# End Source File
# Begin Source File

SOURCE=.\SetupUtilities.h
# End Source File
# Begin Source File

SOURCE=.\SetupWelcome.h
# End Source File
# Begin Source File

SOURCE=.\SetupWindow.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\_Setup.ico
# End Source File
# Begin Source File

SOURCE=.\res\_Setup.rc2
# End Source File
# Begin Source File

SOURCE=.\hlp\_Setup.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\res\computer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\esprit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\r100.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Setup16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Setup256.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\setup\Source\.properties
# End Source File
# Begin Source File

SOURCE=.\res\_Setup.pif
# End Source File
# Begin Source File

SOURCE=.\infotest.htm
# End Source File
# Begin Source File

SOURCE=.\MakeSetup.mak
# End Source File
# Begin Source File

SOURCE=.\setup\Source\Setup.exe
# End Source File
# Begin Source File

SOURCE=.\res\sleep.exe
# End Source File
# End Target
# End Project
