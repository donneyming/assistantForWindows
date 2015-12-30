#include <windows.h>
#include <commctrl.h>
#include "tchar.h"
#include "resource.h"
#include "common.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "mmsystem.h"
#include "NoteDialog.h"

using namespace std;

struct decodeUINIT
{
	int nCode;
	LRESULT (* nFun) (HWND,UINT,WPARAM,LPARAM);
};
struct decodeCMD
{
	int nCode;
	LRESULT (* nFun) (HWND,WORD,HWND,WORD);
};

struct cmdName
{
	int nControl;
	LPCTSTR nName;
	LRESULT (* nFun) (HWND,WORD,HWND,WORD);
};


#define	ASSISTANT _T("个人助手")
#define dim(A)	sizeof(A)/sizeof(A[0])
#define MAX_LOADSTRING 100




//函数声明
int		FindExsit();
ATOM	InitApp(HINSTANCE);
HWND	InitWindow(HINSTANCE hInstance,int nCmdShow);
int		TerminalWindow(HINSTANCE hInstance,int nCmdShow);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT DoCreateMain (HWND,UINT,WPARAM,LPARAM);
LRESULT DoPaintMain (HWND,UINT,WPARAM,LPARAM);
LRESULT DoDestoy (HWND,UINT,WPARAM,LPARAM);
LRESULT DoCreate (HWND,UINT,WPARAM,LPARAM);
LRESULT DoActive (HWND,UINT,WPARAM,LPARAM);
LRESULT DoCommand (HWND,UINT,WPARAM,LPARAM);
LRESULT DoUpdate (HWND,UINT,WPARAM,LPARAM);
LRESULT DoTray (HWND,UINT,WPARAM,LPARAM);

LRESULT DoAdd(HWND,WORD,HWND,WORD);
LRESULT DoDel (HWND,WORD,HWND,WORD);
LRESULT DoMin (HWND,WORD,HWND,WORD);


LRESULT DoAutoStart (HWND,WORD,HWND,WORD);
LRESULT DoAutoEmail (HWND,WORD,HWND,WORD);
LRESULT DoAutoSMS (HWND,WORD,HWND,WORD);
LRESULT DoAbout (HWND,WORD,HWND,WORD);
LRESULT DoClose(HWND,WORD,HWND,WORD);
LRESULT DoPlan(HWND,WORD,HWND,WORD);

BOOL CALLBACK DialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc2(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc3(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc4(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

void  CALLBACK TimeProc(HWND,UINT,UINT,DWORD);
void  SetPrivilege();

HINSTANCE			g_hInst;			// 当前实例
HWND				g_hWndMenuBar;		// 菜单栏句柄
HWND g_hwnd;
HWND hwndList;

vector<Event> eventList;		//用于保存事件列表
#define BTN_WIDTH   100
#define BTN_HEIGHT	30


#define IDM_Add  100
#define IDM_Del	 101
#define IDM_Min	 102
#define SHUTTIMER              50
#define WM_CHANGEEVENT  WM_USER +100
#define WM_TRAY  WM_USER +102

const struct decodeUINIT MainMessage[]=
{
	WM_CREATE,DoCreateMain,
	WM_PAINT,DoPaintMain,
	WM_DESTROY,DoDestoy,
	WM_CREATE,DoCreate,
	WM_ACTIVATE,DoActive,
	WM_COMMAND,DoCommand,
	WM_CHANGEEVENT,DoUpdate,
	WM_TRAY,DoTray
};

const struct decodeCMD MenuCommand[]=
{
	ID_MENU_Start,DoAutoStart,
	ID_MENU_Email,DoAutoEmail,
	ID_MENU_SMS,DoAutoSMS,
	ID_MENU_About,DoAbout,
	ID_MENU_Exit,DoClose,
	ID_MENU_Event,DoPlan
};

const struct  cmdName BtnCommand[]=
{
	IDM_Add,_T("添加关机事件"),DoAdd,
	IDM_Del,_T("添加定时计划"),DoDel,
	IDM_Min,_T("最小化至托盘"),DoMin,
};


//入口函数

int WINAPI WinMain(	HINSTANCE hInstance,        
					HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine,            
					int nCmdShow )
{
	MSG message;

	FindExsit();
	InitApp(hInstance);
	g_hwnd = InitWindow(hInstance,nCmdShow);

	if (g_hwnd)
		{

			while (GetMessage(&message, NULL, 0, 0))
			{
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
		}

	return TerminalWindow(hInstance,message.wParam);
}

//窗口存在
int  FindExsit()
{
	
	if (::FindWindow(ASSISTANT, ASSISTANT))
	{
		::OutputDebugString(_T("Already Exist\n"));
		 return FALSE;
	}
	
	return TRUE;
}
//窗口注册
ATOM InitApp(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL,NULL);
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = (LPCWSTR)IDR_MENU;
	wc.lpszClassName = ASSISTANT;

	return RegisterClass(&wc);
}
//窗口初始化
HWND InitWindow(HINSTANCE hInstance,int nCmdShow)
{
	HWND hWnd;
	int nWidth, nHigh;
	RECT stRect;

	nWidth = 400;
	nHigh = 300;
	g_hInst = hInstance;


	hWnd = CreateWindowEx(WS_EX_TOPMOST, 
							ASSISTANT, ASSISTANT,
							WS_OVERLAPPEDWINDOW ,
							800,
							0,
							nWidth,
							nHigh,
							NULL,
							NULL,
							hInstance,
							NULL);
	if(!IsWindow(hWnd))		
		return 0;


	ShowWindow(hWnd,nCmdShow);
	
	return hWnd;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	for(i=0;i<dim(MainMessage);i++)
	{
		if(message  == MainMessage[i].nCode)
			return (*MainMessage[i].nFun)(hWnd,message,wParam,lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int		TerminalWindow(HINSTANCE hInstance,int ret)
{
	return ret;
}

LRESULT  DoCreateMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndCB;
	int j = 0;
	int hMenuHeight = 20;

	//	画按钮
	for(j = 0;j<dim(BtnCommand);j++)
	{
		HWND hwndButton;
		hwndButton = CreateWindow( 
			_T("BUTTON"),   // predefined class 
			BtnCommand[j].nName,       // button text 
			WS_EX_LEFT|WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles 
			(BTN_WIDTH+20)*j+30,        // starting x position 
			hMenuHeight,// starting y position 
			BTN_WIDTH,        // button width 
			BTN_HEIGHT,        // button height 
			hWnd,       // parent window 
			NULL,       // No menu 
			(HINSTANCE) GetWindowLong(hWnd, 0), 
			NULL);     // pointer not needed 
		SetWindowLong ( hwndButton, GWL_ID, BtnCommand[j].nControl) ;
	}

	RECT rect;
	GetWindowRect(hWnd, &rect);
	int cx,cy;
	cx = rect.right-rect.left;
    cy = rect.bottom-rect.top;

	//画ListBox
	
	hwndList = CreateWindow(
			 WC_LISTVIEW, // predefined class 
			_T("事件列表"),       // button text 
			//WS_EX_LEFT|WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles 
			WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_REPORT | LVS_SHOWSELALWAYS, 
			20,        // starting x position 
			hMenuHeight+BTN_HEIGHT+10,// starting y position 
			cx-50,        // button width 
			cy-(hMenuHeight+BTN_HEIGHT+10)-50,        // button height 
			hWnd,       // parent window 
			NULL,       // No menu 
			(HINSTANCE) GetWindowLong(hWnd, 0), 
			NULL);     // pointer not needed 

	
		LVCOLUMN lvCol = {0};
		memset(&lvCol,0,sizeof(lvCol)); // Reset Coluom
		lvCol.mask=LVCF_TEXT | LVCF_WIDTH; // Type of mask
		//lvCol.cx=0.1*(cx-20);                                // width between each coloum
		lvCol.pszText=_T("定义计划");                     // First Header
 		lvCol.cx=0.3*(cx-20);
		SendMessage(hwndList,LVM_INSERTCOLUMN,0,(LPARAM)&lvCol); // Insert/Show the coloum

		lvCol.pszText=_T("下次执行事件时间");           
		lvCol.cx=0.6*(cx-20);// Next coloum
        SendMessage(hwndList,LVM_INSERTCOLUMN,1,(LPARAM)&lvCol); // ...


		SetTimer(hWnd,SHUTTIMER,900,(TIMERPROC)TimeProc);
		return TRUE;
}
LRESULT  DoPaintMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdc;
	
	GetClientRect(hWnd,&rect);

	hdc = BeginPaint(hWnd,&ps);
	//DrawText(hdc,_T("Hello,WCE"),-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	EndPaint(hWnd,&ps);


	return TRUE;
}
LRESULT  DoDestoy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}
LRESULT DoCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	

	return 0;
}
LRESULT DoActive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return	0;
}
LRESULT DoTray (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	 if ((wParam == IDI_ICON1)&&(lParam == WM_LBUTTONDOWN))
	 {
		ShowWindow(hWnd,SW_NORMAL); 
		SetForegroundWindow(hWnd);
	 }
	return 0;
}
LRESULT DoUpdate (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Event *e =(Event*)lParam;
	LVITEM LvItem; 
	memset(&LvItem,0,sizeof(LvItem));
    LvItem.mask=LVIF_TEXT;
	LvItem.iSubItem=0;
	LvItem.pszText=e->eventtext;
	SendMessage(hwndList,LVM_INSERTITEM, 0, (LPARAM)&LvItem);
	LvItem.iSubItem=1;
	wsprintf(LvItem.pszText,_T("%d/%d/%d %d:%d:%d"),e->eventtime.wYear,
		e->eventtime.wMonth,
		e->eventtime.wDay,
		e->eventtime.wHour,
		e->eventtime.wMinute,
		e->eventtime.wSecond);
	SendMessage(hwndList,LVM_SETITEM, 0, (LPARAM)&LvItem);

	eventList.push_back(*e); //将事件添加到列表中
	return 0;
}
LRESULT DoCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
		int nCode  = LOWORD(wParam);
		for(int j = 0;j<dim(MenuCommand);j++)
		{
			if(nCode  == MenuCommand[j].nCode)
			{
				 (*MenuCommand[j].nFun)(hWnd,nCode,NULL,lParam);
			}
		}	
		for(int i = 0;i<dim(BtnCommand);i++)
		{
			if(nCode  == BtnCommand[i].nControl)
			{
				 (*BtnCommand[i].nFun)(hWnd,nCode,NULL,lParam);
			}
		}	
	return 0;
}


LRESULT DoAdd(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_CLOSE), NULL, (DLGPROC)DialogProc,0);
	return 0;
}
LRESULT DoDel(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_EVENT), NULL, (DLGPROC)DialogProc2,0);
	return 0;
}
LRESULT DoMin(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	NOTIFYICONDATA   nid; 
	memset(&nid,0,sizeof(NOTIFYICONDATA));
    nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA); 
    nid.hWnd=hwnd; 
    nid.uID=IDI_ICON1; 
    nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP; 
	nid.uCallbackMessage=WM_TRAY;//自定义的消息名称 
    nid.hIcon=LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_ICON1));
	//nid.hIcon=(HICON)LoadImage(g_hInst,_T("bitmap2.bmp"),IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
	lstrcpy(nid.szTip, _T("爱心小贴士"));//信息提示条为“计划任务提醒” 
    Shell_NotifyIcon(NIM_ADD,&nid);//在托盘区添加图标 
    ShowWindow(hwnd,SW_HIDE);//隐藏主窗口 
	return 0;


}


LRESULT DoAutoStart(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	return 0;
}
LRESULT DoAutoEmail(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	return 0;
}
LRESULT DoAutoSMS(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	return 0;
}

LRESULT DoAbout(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_ABOUT), NULL, (DLGPROC)DialogProc3,0);
	
	return 0;
}
LRESULT DoClose(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT DoPlan(HWND hwnd,WORD idItem,HWND hwndCtrl,WORD wCode)
{
	 DialogBox(g_hInst, MAKEINTRESOURCE(IDD_NOTEDLG), NULL, NoteDialogProc);
	return 0;
}
BOOL CALLBACK DialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR hdr = (LPNMHDR)lParam;
	HWND hdate = GetDlgItem(hWnd,IDC_DATETIMEPICKER1);
	HWND htime =  GetDlgItem(hWnd,IDC_DATETIMEPICKER2);
						
	 switch(Message)
	 {
	  case WM_COMMAND:
		{
			switch(LOWORD(wParam)) // what we press on?
			{
				 case IDC_OK: 
					{
						UINT r1,r2;
						r1 = IsDlgButtonChecked(hWnd,IDC_R1);
						r2 = IsDlgButtonChecked(hWnd,IDC_R2);
						if(r1 == BST_CHECKED)//被选中
						{
							//SYSTEMTIME shuttimedate;           //用以保存定时关机的时间日期
							SYSTEMTIME systime;
							SYSTEMTIME shuttimedate,shuttimetime;           //用以保存定时关机的时间日期
							DateTime_GetSystemtime(hdate,&shuttimedate);
							DateTime_GetSystemtime(htime,&shuttimetime);
							GetSystemTime(&systime);
							 if (shuttimedate.wYear < systime.wYear)
							 {
								 MessageBox(hWnd,_T("您选择的年份不合逻辑"),_T("选择关机时间错误！"),MB_OK);
								 return FALSE;
							 }
							 else if (shuttimedate.wMonth < systime.wMonth)
							 {
									 MessageBox(hWnd,_T("您选择的月份不合逻辑"),_T("选择关机时间错误！"),MB_OK);
									 return FALSE;
							 }
							 else if (shuttimedate.wDay < systime.wDay )
							 {
									MessageBox(hWnd,_T("您选择的月份不合逻辑"),_T("选择关机时间错误！"),MB_OK);
									return FALSE;
							 }
							 else
							 {
									Event  e;
									
									e.type = 1;
									e.eventtime =shuttimedate;
									e.eventtime.wHour = shuttimetime.wHour;
									e.eventtime.wMinute =shuttimetime.wMinute;
									e.eventtime.wSecond =shuttimetime.wSecond;

									wsprintf(e.eventtext,_T("%s"),_T("下次关机时间")); //将事件写入到文件
									
									//将事件发送到全局链表
									SendMessage(g_hwnd,WM_CHANGEEVENT,0,(LPARAM)&e);
									EndDialog(hWnd,1);
							 }
						
						}
					}
					break;
		
				 case IDC_Cancle:
					EndDialog(hWnd,1);
					break;

				case WM_CLOSE:
					 EndDialog(hWnd,1);
					break;
			}
		}
	 }
	return 0;
}


BOOL CALLBACK DialogProc2(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

	HWND hdate2 = GetDlgItem(hWnd,IDC_DATETIMEPICKER3);
	HWND htime2 =  GetDlgItem(hWnd,IDC_DATETIMEPICKER4);
	 switch(Message)
	 {
	  case WM_COMMAND:
		{
			switch(LOWORD(wParam)) // what we press on?
			{
				 case IDE_OK:
					 {
						UINT r3,r4;
						r3 = IsDlgButtonChecked(hWnd,IDC_R3);
						r4 = IsDlgButtonChecked(hWnd,IDC_R4);

						TCHAR eventText[50];
						memset(eventText,0,50);
						HWND hText = GetDlgItem(hWnd,IDC_EVENTTEXT);
						GetWindowText(hText,eventText,50);
						

						SYSTEMTIME shuttimedate2,shuttimetime2;           //用以保存定时关机的时间日期
						DateTime_GetSystemtime(hdate2,&shuttimedate2);
						DateTime_GetSystemtime(htime2,&shuttimetime2);

						Event  e;
						e.eventtime =shuttimedate2;
						e.eventtime.wHour = shuttimetime2.wHour;
						e.eventtime.wMinute =shuttimetime2.wMinute;
						e.eventtime.wSecond =shuttimetime2.wSecond;									
	
						lstrcpy(e.eventtext,eventText);
						if(r3 == BST_CHECKED)
						{
							e.type = 2;
						}
						
						if(r4 == BST_CHECKED)
						{
							e.type = 3;
						}

						//将事件发送到全局链表
						SendMessage(g_hwnd,WM_CHANGEEVENT,0,(LPARAM)&e);
						EndDialog(hWnd,1);
					 }
					 break;
				 case IDE_Cancle:
					EndDialog(hWnd,1);
					break;
				
				case WM_CLOSE:
					 EndDialog(hWnd,1);
					break;
			}
		}
	 }
	return 0;
}

BOOL CALLBACK DialogProc3(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	 switch(Message)  
	 {
	  case WM_COMMAND:
		  {
		  	 EndDialog(hWnd,1);
		  }
	 }
	 return 0;
}

BOOL CALLBACK DialogProc4(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
void  CALLBACK TimeProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	if (uMsg == WM_TIMER)
	{   

		for(int i= 0;i<eventList.size();i++)
		{
			Event e = 	eventList[i];
			if ((e.eventtime.wYear  == systime.wYear)  &&
				(e.eventtime.wMonth == systime.wMonth) &&
				(e.eventtime.wDay   == systime.wDay)   &&
				(e.eventtime.wHour  == systime.wHour)  &&
				(e.eventtime.wMinute== systime.wMinute)&&
				(e.eventtime.wSecond== systime.wSecond)
				)
			{

				ShowWindow(hwnd,SW_NORMAL); 
				SetForegroundWindow(hwnd);

				if(e.type == 1) //关机事件
				{
					SetPrivilege();
					ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0);	
				}

				if(e.type == 2)//震动模式
				{
					srand((unsigned)time(NULL));
					RECT rcOld, rcNew;
					GetWindowRect(hwnd, &rcOld);
					POINT pt;
					pt.x = rcOld.left;
					pt.y = rcOld.top;
					::ScreenToClient(hwnd, &pt);
					::memcpy(&rcNew, &rcOld, sizeof(RECT));

						int iWidth = rcOld.right-rcOld.left;
						int iHeight = rcOld.bottom-rcOld.top;
						int x = 0, y = 0;
						for (int i =0; i<1000; i++)
						{
							if(i%2==0)
							{
								x = rand()%80-40;
								y = rand()%80-40;
							}
							else
							{
								x = rand()%80+40;
								y = rand()%80+40;
							}
							::MoveWindow(hwnd, rcNew.left+x, rcNew.top+y, iWidth, iHeight, TRUE);
						}
						::MoveWindow(hwnd, rcOld.left, rcOld.top, iWidth, iHeight, TRUE);
				}


				//从列表框中删除该事件
				if(e.type == 3)
				{
					PlaySound(L"tada.wav",NULL,SND_FILENAME|SND_SYNC|SND_LOOP );
				}
			}
		}
	}


}

void  SetPrivilege()
{
	HANDLE                hCurrentProcess;
	HANDLE                hTokenHandle;
	LUID                  luid;
	TOKEN_PRIVILEGES      tkp;
	//TOKEN_PRIVILEGES      privilege;
	LUID_AND_ATTRIBUTES   luidAttrib; 
	//DWORD                 dSize;
	hCurrentProcess = GetCurrentProcess();
	OpenProcessToken(hCurrentProcess,TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hTokenHandle);
	
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME ,&luid);
	luidAttrib.Luid        = luid;
	luidAttrib.Attributes  = SE_PRIVILEGE_ENABLED; 
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0] = luidAttrib;
	AdjustTokenPrivileges(hTokenHandle,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL, 0); //(DWORD)(sizeof(tkp)), &privilege,&dSize);
    
}