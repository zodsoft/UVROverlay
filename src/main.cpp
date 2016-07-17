#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "hid.lib")


#include "StaticOverlay.h"
#include "WindowOverlay.h"
#include "OverlayManager.h"
#include "OverlayTexture.h"
#include "RenderEnvironment.h"
#include "VertexObject.h"
#include <openvr.h>
#include "resources.h"
#include "../UniversalVROverlay/resource.h"
#include <Psapi.h>
#include <PathCch.h>
#include <shellapi.h>

#include "../UniversalVROverlay/mainwindow.h"
#include <QApplication>

extern "C"
{
#include <hidsdi.h>
}

#include <CommCtrl.h>



//Will Need to move to WindowManager class
#include <vector>
#include "../UniversalVROverlay/WindowDescriptor.h"


#pragma comment(lib, "Pathcch.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND  hWnd;										// Main Window HWND
HFONT hFont;


HWND AddButton, DelButton, OverlayList;			//Controler HWNDs

HWND SettingsStatic;							
//Static Area Controllers
HWND TitleText, TitleBox;
HWND OverlayTypeText, OverlayTypeBox;
HWND KeybindText, KeybindBox;
HWND xRotate, yRotate, zRotate;
HWND xRotateText, yRotateText, zRotateText;


HWND xTranslate, yTranslate, zTranslate;
HWND xTranslateLeft, xTranslateRight, xTranslateLeftBig, xTranslateRightBig;
HWND yTranslateUp, yTranslateDown, yTranslateUpBig, yTranslateDownBig;
HWND zTranslateForward, zTranslateBack, zTranslateForwardBig, zTranslateBackBig;

HWND xTranslateText, yTranslateText, zTranslateText;
HWND scaleSlide;

HWND controllerCombo;

WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND targethWnd;
HWND targethWnd2;

//D3D env
RenderEnvironment* env;

//OverlayMgr
OverlayManager* mgr;

//Selected Overlay Index
int selectedIndex;

//VR System
vr::IVRSystem *pVRSystem;


std::vector<WindowDescriptor> wndVec;

												// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK    EnumProc(HWND hWnd, LPARAM lParam);
INT_PTR	CALLBACK	DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void				updateListBox();
void				setOptionsIndex();



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	LPWSTR * szArgList;
	int argCount;

	//Test Version check to make sure it only runs on systems I authorize at compile time
#if defined(TEST_VER) && defined(HDD_SERIAL)
	unsigned long hd_serial = 0;
	GetVolumeInformation(L"C:\\", NULL, 0, &hd_serial, NULL, NULL, NULL, 0);
	if (hd_serial != HDD_SERIAL)
	{
		return -1;
	}
#endif






	//Init values
	selectedIndex = -1;
	//AllocConsole();
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT3, szWindowClass, MAX_LOADSTRING);


	//szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	

	
	//MyRegisterClass(hInstance);
	/*hFont = CreateFont(
		14, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Arial");*/

	//Init d3d and manager
	//TODO: add VR hooking to it's own class
	


	// Perform application initialization:
	/*if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}*/
	
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT3));

	MSG msg;

	
	//----VR Init-------------------------------------------------------
	vr::HmdError m_eLastHmdError;
	pVRSystem = vr::VR_Init(&m_eLastHmdError, vr::VRApplication_Overlay);

	if (m_eLastHmdError != vr::VRInitError_None)
	{

		return 1;
	}

	//----Overlay D3dInit-----------------------------------------------

	
	//env = new RenderEnvironment(adapterIndex);
	mgr = new OverlayManager();
	QApplication a(__argc, __argv);
	MainWindow w(0,mgr, pVRSystem);
	w.show();
	hWnd = (HWND)w.effectiveWinId();

	
	

	
		
	//Setup viewport
	/*D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(1280);
	viewport.Height = static_cast<float>(720);
	viewport.MinDepth = D3D11_MIN_DEPTH;
	viewport.MaxDepth = D3D11_MAX_DEPTH;*/

	////D3D11
	//env->getContext()->RSSetViewports(1, &viewport);

	RAWINPUTDEVICE Rid[3];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x04;
	Rid[0].dwFlags = RIDEV_INPUTSINK;                 // adds joystick
	Rid[0].hwndTarget = hWnd;

	//Rid[1].usUsagePage = 0x01;
	//Rid[1].usUsage = 0x05;
	//Rid[1].dwFlags = RIDEV_INPUTSINK;                 // adds game pad
	//Rid[1].hwndTarget = hWnd;

	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;                 // adds hid mouse
	Rid[1].hwndTarget = hWnd;

	//Rid[3].usUsagePage = 0x01;
	//Rid[3].usUsage = 0x06;
	//Rid[3].dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;                 // adds hid keyboard
	//Rid[3].hwndTarget = hWnd;

	/*if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
	{
		
		MessageBox(NULL, L"Failed Raw", boost::lexical_cast<std::wstring>(GetLastError()).c_str(), MB_OK);
		
	}
		*/

	//------------------------------------------------------------------
	// Main message loop:
	return a.exec();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_INPUT)
		{
			UINT dwSize;

			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,
				sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL)
			{
				return 0;
			}

			if (GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,
				sizeof(RAWINPUTHEADER)) != dwSize)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				MessageBox(NULL, L"Test", L"Test", MB_OK);
			}
			if (raw->header.dwType == RIM_TYPEHID)
			{
				UINT bufferSize;
				GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize);
				LPBYTE deviceInfo = new BYTE[bufferSize];
				
				//Add check for allocation
				

				//Get the HID preparsed data
				GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, deviceInfo, &bufferSize);
				PHIDP_PREPARSED_DATA deviceInfoData = (PHIDP_PREPARSED_DATA)deviceInfo;
				HIDP_CAPS caps;
				HidP_GetCaps(deviceInfoData, &caps);

				LPBYTE ButtonCapsBuf = new BYTE[sizeof(HIDP_BUTTON_CAPS) * caps.NumberInputButtonCaps];
				//Add check for allocation

				//Get the button capabilities from HID
				PHIDP_BUTTON_CAPS ButtonCaps = (PHIDP_BUTTON_CAPS)ButtonCapsBuf;
				USHORT capsLength = caps.NumberInputButtonCaps;
				HidP_GetButtonCaps(HidP_Input, ButtonCaps, &capsLength, deviceInfoData);

				
				LPBYTE ValCapsBuf = new BYTE[sizeof(HIDP_BUTTON_CAPS) * caps.NumberInputValueCaps];

				//Find the current button usage data from HID device
				USAGE usage[128];
				ULONG usageLength = ButtonCaps->Range.UsageMax - ButtonCaps->Range.UsageMin + 1;
				HidP_GetUsages(HidP_Input, ButtonCaps->UsagePage, 0, usage, &usageLength, deviceInfoData, (PCHAR)raw->data.hid.bRawData, raw->data.hid.dwSizeHid);


				GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, NULL, &bufferSize);
				LPBYTE deviceNameInfoBuf = new BYTE[sizeof(wchar_t) * bufferSize];

				//Add check for allocation


				//Get the HID Name
				GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, deviceNameInfoBuf, &bufferSize);

				wchar_t DeviceName[127];
				HANDLE HIDHandle = CreateFile((LPCWSTR)deviceNameInfoBuf, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
				if (HIDHandle)
				{
					HidD_GetProductString(HIDHandle, DeviceName, sizeof(wchar_t) * 126);
					CloseHandle(HIDHandle);
					for (int i = 0; i < usageLength; i++)
					{
						MessageBox(NULL, boost::lexical_cast<std::wstring>(usage[i] - ButtonCaps->Range.UsageMin).c_str(), std::wstring(DeviceName).c_str(), MB_OK);
					}
				}
				

				//Clean up byte arrays;
				delete[] deviceNameInfoBuf;
				delete[] ValCapsBuf;
				delete[] ButtonCapsBuf;
				delete[] deviceInfo;
			}

			//Clean up byte arrays;
			delete[] lpb;
				
		}
		if (msg.message == WM_QUIT)
			break;
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
				
		if (selectedIndex >= 0)
		{

			mgr->getOverlays()[selectedIndex]->setTrans(X_AXIS, SendMessage(xTranslate, TBM_GETPOS, 0, 0));
			mgr->getOverlays()[selectedIndex]->setTrans(Y_AXIS, SendMessage(yTranslate, TBM_GETPOS, 0, 0));
			mgr->getOverlays()[selectedIndex]->setTrans(Z_AXIS, SendMessage(zTranslate, TBM_GETPOS, 0, 0));

			mgr->getOverlays()[selectedIndex]->setRotate(X_AXIS, SendMessage(xRotate, TBM_GETPOS, 0, 0));
			mgr->getOverlays()[selectedIndex]->setRotate(Y_AXIS, SendMessage(yRotate, TBM_GETPOS, 0, 0));
			mgr->getOverlays()[selectedIndex]->setRotate(Z_AXIS, SendMessage(zRotate, TBM_GETPOS, 0, 0));

			mgr->getOverlays()[selectedIndex]->setScale(SendMessage(scaleSlide, TBM_GETPOS, 0, 0));

			mgr->getOverlays()[selectedIndex]->setTracking(SendMessage(controllerCombo, CB_GETCURSEL, 0, 0));
		}
	}
	return (int)msg.wParam;
}

BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
{
	TCHAR title[500] = L"";
	TCHAR wndTitle[500] = L"";
	ZeroMemory(title, sizeof(title));

	if (IsWindowVisible(hWnd))
	{
		//GetWindowText(hWnd, title, sizeof(title) / sizeof(title[0]));
		DWORD PID = NULL;
		GetWindowThreadProcessId(hWnd, &PID);
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
		GetModuleFileNameEx(hProcess, NULL, title, sizeof(title) / sizeof(title[0]));
		//GetModuleFileName()
		//GetWindowModuleFileName(hWnd, title, sizeof(title) / sizeof(title[0]));
		
		TCHAR *name = wcsrchr(title, L'\\');
		
		if (GetWindowText(hWnd, wndTitle, 500) > 0 && name != NULL)
		{
			//--------------------------------------------------
			wndVec.push_back(WindowDescriptor(hWnd, std::wstring(wndTitle), std::wstring(name)));
		}
		//--------------------------------------------------
		CloseHandle(hProcess);
		
	}

	
	return TRUE;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT3));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32PROJECT3);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	/*hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640 , 480, nullptr, nullptr, hInstance, nullptr);*/
	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 785,565, nullptr, nullptr, hInstance, nullptr);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case WM_CREATE:
	{
		//---Define Window Controls-----------------------------------------
		AddButton = CreateWindowEx(
			NULL,
			L"Button",
			L"+",
			WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			225, 0,
			30, 30,
			hWnd, NULL,
			hInst,
			NULL);
		SendMessage(AddButton, WM_SETFONT, (WPARAM)hFont, TRUE);
		OverlayList = CreateWindowEx(
			NULL,
			L"ListBox",
			L"test test test",
			WS_BORDER | WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
			5, 30,
			250, 500,
			hWnd, NULL,
			hInst,
			NULL);
		SendMessage(OverlayList, WM_SETFONT, (WPARAM)hFont, TRUE);
		SettingsStatic = CreateWindowEx(
			NULL,
			L"Button",
			L"Overlay Settings",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			265, 23,
			500, 500,
			hWnd, NULL,
			hInst,
			NULL);
		SendMessage(SettingsStatic, WM_SETFONT, (WPARAM)hFont, TRUE);

		//-----------------------------------------------

		/*
		HWND TitleText, TitleBox;
		HWND OverlayTypeText, OverlayTypeBox;
		HWND KeybindText, KeybindBox;
		
		*/

		TitleText = CreateWindowEx(
			NULL,
			L"Static",
			L"Title: ",
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 22,
			100, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(TitleText, WM_SETFONT, (WPARAM)hFont, TRUE);

		TitleBox = CreateWindowEx(
			NULL,
			L"edit",
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			85, 20,
			100, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(TitleBox, WM_SETFONT, (WPARAM)hFont, TRUE);


		OverlayTypeText = CreateWindowEx(
			NULL,
			L"Static",
			L"Overlay Type: ",
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 47,
			100, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(OverlayTypeText, WM_SETFONT, (WPARAM)hFont, TRUE);

		OverlayTypeBox = CreateWindowEx(
			NULL,
			L"edit",
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			85, 45,
			100, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(OverlayTypeBox, WM_SETFONT, (WPARAM)hFont, TRUE);

		KeybindText = CreateWindowEx(
			NULL,
			L"Static",
			L"Keybind: ",
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 72,
			100, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(KeybindText, WM_SETFONT, (WPARAM)hFont, TRUE);

		KeybindBox = CreateWindowEx(
			NULL,
			L"edit",
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			85, 70,
			100, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(KeybindBox, WM_SETFONT, (WPARAM)hFont, TRUE);

		/*
		HWND xRotate, yRotate, zRotate;
		HWND xRotateText, yRotateText, zRotateText;
		HWND xTranslate, yTranslate, zTranslate;
		HWND xTranslateText, yTranslateText, zTranslateText;
		*/
		xRotate = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 100,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(xRotate, TBM_SETRANGE, TRUE, MAKELONG(-180, 180));

		yRotate = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 130,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(yRotate, TBM_SETRANGE, TRUE, MAKELONG(-180, 180));
		
		zRotate = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 160,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(zRotate, TBM_SETRANGE, TRUE, MAKELONG(-180, 180));



		//-----------------------------------------------

		xTranslate = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			//WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			WS_CHILD | WS_TABSTOP,
			10, 200,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(xTranslate, TBM_SETRANGE, TRUE, MAKELONG(-100, 100));

		//X-Translate Buttons
		xTranslateLeft = CreateWindowEx(
			NULL,
			L"Button",
			L"<",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			35, 220,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);
		xTranslateLeftBig = CreateWindowEx(
			NULL,
			L"Button",
			L"<<",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			10, 220,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);

		xTranslateRight = CreateWindowEx(
			NULL,
			L"Button",
			L">",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			135, 220,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);
		xTranslateRightBig = CreateWindowEx(
			NULL,
			L"Button",
			L">>",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			160, 220,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);

		//Y-Translate Buttons
		yTranslateUp = CreateWindowEx(
			NULL,
			L"Button",
			L"^",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			85, 200,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);
		yTranslateUpBig = CreateWindowEx(
			NULL,
			L"Button",
			L"^\n^",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			85, 175,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);

		yTranslateDown = CreateWindowEx(
			NULL,
			L"Button",
			L"V",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			85, 250,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);
		yTranslateDownBig = CreateWindowEx(
			NULL,
			L"Button",
			L"V\nV",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			85, 275,
			25, 25,
			SettingsStatic, NULL,
			hInst,
			NULL);

		
		

		yTranslate = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			//WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			WS_CHILD | WS_TABSTOP,
			10, 230,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(yTranslate, TBM_SETRANGE, TRUE, MAKELONG(-100, 100));

		zTranslate = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			//WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			WS_CHILD | WS_TABSTOP,
			10, 260,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(zTranslate, TBM_SETRANGE, TRUE, MAKELONG(-100, 100));

		//-----------------------------------------------
		controllerCombo = CreateWindowEx(
			NULL,
			L"ComboBox",
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST ,
			210, 260,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(controllerCombo, WM_SETFONT, (WPARAM)hFont, TRUE);		
		SendMessage(controllerCombo, CB_ADDSTRING, 0, (LPARAM)std::wstring(L"Spacial").c_str());
		SendMessage(controllerCombo, CB_ADDSTRING, 0, (LPARAM)L"HMD");
		SendMessage(controllerCombo, CB_ADDSTRING, 0, (LPARAM)L"Controller 1");
		SendMessage(controllerCombo, CB_ADDSTRING, 0, (LPARAM)L"Controller 2");

		//-----------------------------------------------

		scaleSlide = CreateWindowEx(
			NULL,
			TRACKBAR_CLASSW,
			L"Test",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 360,
			175, 18,
			SettingsStatic, NULL,
			hInst,
			NULL);
		SendMessage(scaleSlide, TBM_SETRANGE, TRUE, MAKELONG(0, 100));



		//TODO: Populate list from saved data;


		break;
	}
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 785;
		mmi->ptMinTrackSize.y = 565;
		return 0;
		break;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		int hiWd = HIWORD(wParam);
		switch (hiWd)
		{
		case BN_CLICKED:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_WNDSELECT), hWnd, DlgProc);
			break;
		case LBN_SELCHANGE:
			setOptionsIndex();
	

		}
		
		switch (wmId)
		{

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// TODO: Add any drawing code that uses hdc here...
		
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND cBoxHwnd;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		//TODO: Move this to a windowmanager.getList function
		cBoxHwnd = GetDlgItem(hwndDlg, IDC_COMBO1);
		wndVec.clear();
		EnumWindows(EnumProc, 0);
		for (std::vector<WindowDescriptor>::iterator it = wndVec.begin(); it != wndVec.end(); ++it)
		{
			SendMessage(cBoxHwnd, CB_ADDSTRING, 0, (LPARAM)(it->getTitle()).c_str());
		}
		
		
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_WNDREFRESH)
		{
			cBoxHwnd = GetDlgItem(hwndDlg, IDC_COMBO1);
			SendMessage(cBoxHwnd, CB_RESETCONTENT, 0, 0);
			wndVec.clear();
			EnumWindows(EnumProc, 0);
			for (std::vector<WindowDescriptor>::iterator it = wndVec.begin(); it != wndVec.end(); ++it)
			{
				SendMessage(cBoxHwnd, CB_ADDSTRING, 0, (LPARAM)(it->getTitle()).c_str());
			}
		}
		if (LOWORD(wParam) == IDYES )
		{
			cBoxHwnd = GetDlgItem(hwndDlg, IDC_COMBO1);
			int index = SendMessage(cBoxHwnd, CB_GETCURSEL, 0, 0);
			if (index != CB_ERR)
			{
				
				WindowOverlay* overlay = new WindowOverlay(new OverlayTexture(env), pVRSystem);
				mgr->addOverlay(overlay);
				
				vr::HmdMatrix34_t overlayDistanceMtx2;
				memset(&overlayDistanceMtx2, 0, sizeof(overlayDistanceMtx2));
				overlay->setHwnd(wndVec[index].getHwnd());
				overlay->setName(wndVec[index].getTitle());
				overlay->setExeName(wndVec[index].getExe());
				overlay->setOverlayMatrix(overlayDistanceMtx2);
				overlay->ShowOverlay();

				updateListBox();


			}
			
			EndDialog(hwndDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDNO)
		{
			EndDialog(hwndDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;

}

void updateListBox()
{
	SendMessage(OverlayList, LB_RESETCONTENT, 0, 0);
	const std::vector<std::shared_ptr<Overlay>> vec = mgr->getOverlays();
	for (std::vector<std::shared_ptr<Overlay>>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		SendMessage(OverlayList, LB_ADDSTRING, 0, (LPARAM)(*it)->getName().c_str());
	}

}

void setOptionsIndex()
{
	 int index = SendMessage(OverlayList, LB_GETCURSEL, 0, 0);
	 
	 if (index == LB_ERR)
	 {
		 selectedIndex = -1;
		 return;
	 }
	 selectedIndex = index;
	 
	 SendMessage(xTranslate, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getTrans(X_AXIS));
	 SendMessage(yTranslate, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getTrans(Y_AXIS));
	 SendMessage(zTranslate, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getTrans(Z_AXIS));

	 SendMessage(xRotate, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getRotate(X_AXIS));
	 SendMessage(yRotate, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getRotate(Y_AXIS));
	 SendMessage(zRotate, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getRotate(Z_AXIS));

	 SendMessage(scaleSlide, TBM_SETPOS, TRUE, mgr->getOverlays()[selectedIndex]->getScale());

	 SendMessage(controllerCombo, CB_SETCURSEL, mgr->getOverlays()[selectedIndex]->getTracking(), 0);
}


