# Microsoft Developer Studio Project File - Name="DuhMEx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DuhMEx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DuhMEx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DuhMEx.mak" CFG="DuhMEx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DuhMEx - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DuhMEx - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DuhMEx - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release/DuhMEx_vc60.dll"
# Begin Custom Build
OutDir=.\Release
TargetName=DuhMEx_vc60
InputPath=.\Release\DuhMEx_vc60.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo 拷贝二进制文件 
	xcopy $(OutDir)\$(TargetName).dll ..\lib\release\ /y 
	xcopy $(OutDir)\$(TargetName).lib ..\lib\release\ /y 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DuhMEx - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/DuhMEx_vc60D.dll" /pdbtype:sept
# Begin Custom Build
OutDir=.\Debug
ProjDir=.
TargetName=DuhMEx_vc60D
InputPath=.\Debug\DuhMEx_vc60D.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo 拷贝二进制文件 
	xcopy $(OutDir)\$(TargetName).dll ..\lib\debug\ /y 
	xcopy $(OutDir)\$(TargetName).lib ..\lib\debug\ /y 
	echo 拷贝头文件 
	xcopy $(ProjDir)\DetailWnd.h ..\include\ /y 
	xcopy $(ProjDir)\DuhMenu.h ..\include\ /y 
	xcopy $(ProjDir)\DuhStatic.h ..\include\ /y 
	xcopy $(ProjDir)\SearchEdit.h ..\include\ /y 
	xcopy $(ProjDir)\AES.h ..\include\ /y 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DuhMEx - Win32 Release"
# Name "DuhMEx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AES.cpp
# End Source File
# Begin Source File

SOURCE=.\Base64.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\DuhMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\DuhMEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DuhMEx.def
# End Source File
# Begin Source File

SOURCE=.\DuhMEx.rc
# End Source File
# Begin Source File

SOURCE=.\DuhStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\GdipEx.cpp
# End Source File
# Begin Source File

SOURCE=.\HookMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinMenuMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinMenuWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Subclass.cpp
# End Source File
# Begin Source File

SOURCE=.\WinClasses.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AES.h
# End Source File
# Begin Source File

SOURCE=.\Base64.h
# End Source File
# Begin Source File

SOURCE=.\DetailWnd.h
# End Source File
# Begin Source File

SOURCE=.\DuhMenu.h
# End Source File
# Begin Source File

SOURCE=.\DuhStatic.h
# End Source File
# Begin Source File

SOURCE=.\GdipEx.h
# End Source File
# Begin Source File

SOURCE=.\HookMgr.h
# End Source File
# Begin Source File

SOURCE=.\PictureEx.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Rijndael.h
# End Source File
# Begin Source File

SOURCE=.\SearchEdit.h
# End Source File
# Begin Source File

SOURCE=.\SkinMenuMgr.h
# End Source File
# Begin Source File

SOURCE=.\SkinMenuWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\wclassdefines.h
# End Source File
# Begin Source File

SOURCE=.\WinClasses.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DuhMEx.rc2
# End Source File
# Begin Source File

SOURCE=.\res\loading_small.gif
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
