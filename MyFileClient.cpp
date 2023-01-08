// MyFileClient.cpp : main source file for MyFileClient.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlsplit.h>

#include "resource.h"

#include "View.h"
#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

void _printf(const char* mess, int val)
{

}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

HINSTANCE hInstRich;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

//	hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
//	const char *ptr = CRichEditCtrl::GetLibraryName();
	hInstRich = ::LoadLibrary("RichEd32.dll");


	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	::FreeLibrary(hInstRich);

	return nRet;
}
