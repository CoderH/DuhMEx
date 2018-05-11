// DuhMEx.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Gdiplus;
static ULONG_PTR m_gdiplusToken;
HINSTANCE g_hModuleInstance = NULL;
static AFX_EXTENSION_MODULE DuhMExDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DUHMEX.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(DuhMExDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(DuhMExDLL);
      
      g_hModuleInstance = hInstance;
      GdiplusStartupInput m_gdiplusstartupinput;
      GdiplusStartup(&m_gdiplusToken, &m_gdiplusstartupinput, NULL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("DUHMEX.DLL Terminating!\n");
       GdiplusShutdown(m_gdiplusToken);
		// Terminate the library before destructors are called
		AfxTermExtensionModule(DuhMExDLL);
	}
	return 1;   // ok
}
