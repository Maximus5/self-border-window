// SBWindow.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <Windowsx.h>
#include "SBWindow.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SBWINDOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SBWINDOW));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SBWINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SBWINDOW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   DWORD dwStyle = 0
	  // | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
	  // | WS_OVERLAPPED
	  // ;
   //dwStyle &= ~(WS_CAPTION|WS_THICKFRAME)
	   ;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, 300, 300, 300, 200, nullptr, nullptr, hInstance, nullptr);
   DWORD nTest = GetWindowLong(hWnd, GWL_STYLE);
   SetWindowLong(hWnd, GWL_STYLE, 0);

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
	const int border = 10;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
			RECT rc; GetClientRect(hWnd, &rc);
			HBRUSH hbr = CreateSolidBrush(RGB(0,255,0));
			RECT rcb = {rc.left, rc.top, rc.left+border, rc.bottom};
			FillRect(hdc, &rcb, hbr);
			rcb = RECT{rc.left, rc.top, rc.right, rc.top+border};
			FillRect(hdc, &rcb, hbr);
			rcb = RECT{rc.left, rc.bottom-border, rc.right, rc.bottom};
			FillRect(hdc, &rcb, hbr);
			rcb = RECT{rc.right-border, rc.top, rc.right, rc.bottom};
			FillRect(hdc, &rcb, hbr);
            DeleteObject(hbr);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_NCHITTEST:
		{
			LRESULT lrc = DefWindowProc(hWnd, message, wParam, lParam);
			if (lrc == HTCLIENT)
			{
				lrc = HTCAPTION;
				RECT rc; GetClientRect(hWnd, &rc);
				MapWindowPoints(hWnd, NULL, (LPPOINT)&rc, 2);
				POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				if (PtInRect(&rc, pt))
				{
					POINT point = {pt.x - rc.left, pt.y - rc.top};
					int nFrame = border;
					int nWidth = rc.right - rc.left;
					int nHeight = rc.bottom - rc.top;
					LRESULT& l_result = lrc;

					int nShift = GetSystemMetrics(SM_CXSMICON) * 2;
					enum Frames { left = 1, top = 2, right = 4, bottom = 8 };
					int frames = 0;
					if (point.x <= nFrame)
						frames |= Frames::left;
					if (point.y <= nFrame)
						frames |= Frames::top;
					if (point.x >= (nWidth-nFrame))
						frames |= Frames::right;
					if (point.y >= (nHeight-nFrame))
						frames |= Frames::bottom;

					if (((frames & Frames::left) && point.y <= nShift) || ((frames & Frames::top) && point.x <= nShift))
						l_result = HTTOPLEFT;
					else if (((frames & Frames::right) && point.y <= nShift) || ((frames & Frames::top) && point.x >= (nWidth-nFrame-nShift)))
						l_result = HTTOPRIGHT;
					else if (((frames & Frames::left) && point.y >= (nHeight-nFrame-nShift)) || ((frames & Frames::bottom) && point.x <= nShift))
						l_result = HTBOTTOMLEFT;
					else if (((frames & Frames::right) && point.y >= (nHeight-nFrame-nShift)) || ((frames & Frames::bottom) && point.x >= (nWidth-nFrame-nShift)))
						l_result = HTBOTTOMRIGHT;
					else if (frames & Frames::left)
						l_result = HTLEFT;
					else if (frames & Frames::right)
						l_result = HTRIGHT;
					else if (frames & Frames::top)
						l_result = HTTOP;
					else if (frames & Frames::bottom)
						l_result = HTBOTTOM;
				}
			}
			return lrc;
		}
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
