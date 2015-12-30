#include <Windows.h>
#include "resource.h"

#define HANDLEMSG(hwndDlg, uMsg, wParam, lParam, pfnHandleFunc) pfnHandleFunc(hwndDlg, uMsg, wParam, lParam)
#define TEXT_LENGTH 1024*1024

HWND g_hwndMainDlg = NULL;
HINSTANCE g_hInstance = NULL;
BOOL g_bEditMode = FALSE;
HWND g_hwndEdit = NULL;
HWND g_hwndStatic = NULL;
HWND g_hwndClose = NULL;
TCHAR g_pcText[TEXT_LENGTH];
BOOL g_bIsCtrlDown = FALSE;

HHOOK g_hKeyHook = NULL;

LRESULT CALLBACK NoteKeyboardProc(          
    int code,
    WPARAM wParam,
    LPARAM lParam
    );

INT_PTR CALLBACK NoteDialogInit(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    RECT stRect;

    g_hwndMainDlg = hwndDlg;
    g_hwndEdit = GetDlgItem(hwndDlg, IDC_EDITNOTE);
    g_hwndStatic = GetDlgItem(hwndDlg, IDC_STATICNOTE);
	g_hwndClose =GetDlgItem(hwndDlg,IDC_Close);

    SetWindowPos(hwndDlg, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
    SetWindowLong(hwndDlg, GWL_EXSTYLE, GetWindowLong(hwndDlg, GWL_EXSTYLE) & (~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW);

    GetClientRect(hwndDlg, &stRect);
    MoveWindow(g_hwndEdit, stRect.left, stRect.top, stRect.right-stRect.left, stRect.bottom-stRect.top, TRUE);
    MoveWindow(g_hwndStatic, stRect.left, stRect.top, stRect.right-stRect.left, stRect.bottom-stRect.top, TRUE);
	MoveWindow(g_hwndClose, stRect.left, stRect.top,15,15, TRUE);

    //TODO:读保存的信息来显示
    SetWindowText(g_hwndStatic, g_pcText);
    SetWindowText(g_hwndEdit, g_pcText);

    ShowWindow(g_hwndStatic, SW_NORMAL);
    EnableWindow(g_hwndEdit, FALSE);
    ShowWindow(g_hwndEdit, SW_HIDE);
    g_bEditMode = FALSE;
    g_hKeyHook = SetWindowsHookEx(WH_KEYBOARD, NoteKeyboardProc, NULL, GetCurrentThreadId());

    return TRUE;
}

INT_PTR CALLBACK NoteDialogCtlColor(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    HDC hdc = (HDC)wParam;
    HBRUSH hBrush = NULL;

    hBrush = CreateSolidBrush(RGB(255,255,0));
    SetBkColor(hdc, RGB(255,255,0));
    SetTextColor(hdc, RGB(0,0,0));

    return (INT_PTR)hBrush;
}

INT_PTR CALLBACK NoteDialogSize(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    RECT stRect;
    LONG lWidth;
    LONG lHeight;
    
    GetWindowRect(hwndDlg, &stRect);
    lWidth = stRect.right-stRect.left;
    lHeight = stRect.bottom-stRect.top;

    lWidth = (lWidth<200)?200:lWidth;
    lHeight = (lHeight<120)?120:lHeight;

    MoveWindow(g_hwndEdit, 0, 0, lWidth, lHeight, TRUE);
    MoveWindow(g_hwndStatic, 0, 0, lWidth, lHeight, TRUE);
    MoveWindow(g_hwndMainDlg, stRect.left, stRect.top, lWidth, lHeight, TRUE);

    return TRUE;
}

INT_PTR CALLBACK NoteDialogLbtnDbClick(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    SendMessage(g_hwndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    ShowWindow(g_hwndStatic, SW_HIDE);
    EnableWindow(g_hwndEdit, TRUE);
    ShowWindow(g_hwndEdit, SW_NORMAL);
    g_bEditMode = TRUE;
  
    return TRUE;
}

INT_PTR CALLBACK NoteDialogLbtnDown(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    POINTS stPoint = *((POINTS *)&lParam);
    SendMessageA(hwndDlg, WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(stPoint.x,stPoint.y));
    PostMessageA(hwndDlg, WM_LBUTTONUP,NULL,MAKELPARAM(stPoint.x,stPoint.y));
    return FALSE;
}

LRESULT CALLBACK NoteKeyboardProc(          
    int code,
    WPARAM wParam,
    LPARAM lParam
    )
{
    BOOL bIsKeyDown = (lParam & 0x80000000)?FALSE:TRUE;

    if (wParam == VK_CONTROL)
    {
        g_bIsCtrlDown = bIsKeyDown;
    }
    else if (wParam == VK_RETURN && bIsKeyDown)
    {
        if (g_bIsCtrlDown != TRUE)
        {    
            if (g_bEditMode)
            {     
                //TODO:确认信息......
                GetWindowText(g_hwndEdit, g_pcText, TEXT_LENGTH);
                //TODO:保存数据......

                SetWindowText(g_hwndStatic, g_pcText);

                ShowWindow(g_hwndStatic, SW_NORMAL);
                EnableWindow(g_hwndEdit, FALSE);
                ShowWindow(g_hwndEdit, SW_HIDE);
                g_bEditMode = FALSE;
            }
        }
        else
        {
            SendMessage(g_hwndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
            SendMessage(g_hwndEdit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)TEXT("\r\n"));
        }
    }
    else if (wParam == VK_ESCAPE)
    {
        if (g_bEditMode == FALSE)
        {
            EndDialog(g_hwndMainDlg, 0);
        }
    }

    return CallNextHookEx(g_hKeyHook, code, wParam, lParam);
}

INT_PTR CALLBACK NoteDialogProc(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        return HANDLEMSG(hwndDlg, uMsg, wParam, lParam, NoteDialogInit);

    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
        return HANDLEMSG(hwndDlg, uMsg, wParam, lParam, NoteDialogCtlColor);

    case WM_SIZE:
        return HANDLEMSG(hwndDlg, uMsg, wParam, lParam, NoteDialogSize);

    case WM_LBUTTONDOWN:
        return HANDLEMSG(hwndDlg, uMsg, wParam, lParam, NoteDialogLbtnDown);

    case WM_LBUTTONDBLCLK:
        return HANDLEMSG(hwndDlg, uMsg, wParam, lParam, NoteDialogLbtnDbClick);
    
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				 case IDC_Close: 
					 EndDialog(hwndDlg,1);
			}
		}
		 
		 return 0;
    default:
        return FALSE;
    }
}