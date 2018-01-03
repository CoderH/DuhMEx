// dllmain.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;
static ULONG_PTR m_gdiplusToken;
HINSTANCE g_hModuleInstance = NULL;
static AFX_EXTENSION_MODULE DuhMExDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DuhMEx.DLL ���ڳ�ʼ��!\n");
		
		// ��չ DLL һ���Գ�ʼ��
		if (!AfxInitExtensionModule(DuhMExDLL, hInstance))
			return 0;
		// ���� DLL ���뵽��Դ����
		// ע��:  �������չ DLL ��
		//  MFC ���� DLL (�� ActiveX �ؼ�)��ʽ���ӵ���
		//  �������� MFC Ӧ�ó������ӵ�������Ҫ
		//  �����д� DllMain ���Ƴ������������һ��
		//  �Ӵ���չ DLL �����ĵ����ĺ����С�  ʹ�ô���չ DLL ��
		//  ���� DLL Ȼ��Ӧ��ʽ
		//  ���øú����Գ�ʼ������չ DLL��  ����
		//  CDynLinkLibrary ���󲻻ḽ�ӵ�
		//  ���� DLL ����Դ���������������ص�
		//  ���⡣

		new CDynLinkLibrary(DuhMExDLL);


        g_hModuleInstance = hInstance;
        GdiplusStartupInput m_gdiplusstartupinput;
        GdiplusStartup(&m_gdiplusToken, &m_gdiplusstartupinput, NULL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("DuhMEx.DLL ������ֹ!\n");
        GdiplusShutdown(m_gdiplusToken);
		// �ڵ�����������֮ǰ��ֹ�ÿ�
		AfxTermExtensionModule(DuhMExDLL);
	}
	return 1;   // ȷ��
}
