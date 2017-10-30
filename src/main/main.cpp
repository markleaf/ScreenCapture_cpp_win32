// main.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "main.h"
#include "ScreenShot.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
LPCTSTR szAppName=L"屏幕截图工具";

HDC hMemDc;										//截图的内存画板
CBitmap bmp;									//截图的位图数据
HBITMAP hBmp;									//截图的位图句柄
HBITMAP hOldBmp;								//截图的画笔句柄
ScreenShot cut;
INT xScrn,yScrn;
RECT Selectrect;								//选择框
HDC Selecthdc;
BOOL isSelected=false;							//是否选取了截图局域
BOOL isShow=false;								// 窗口是否处于显示状态

HHOOK g_hhook;									// 全局键盘钩子
HWND  g_hwnd;									// 窗口句柄
LRESULT CALLBACK	GlobleKeyProc(int, WPARAM, LPARAM);//全局键盘钩子快捷键

BOOL isMouseDown=false;							//是否按下鼠标

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

//程序入口
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	HDC hScrDc=CreateDC(L"DISPLAY",NULL,NULL,NULL);
	xScrn=GetDeviceCaps(hScrDc,HORZRES);//水平分辨率
	yScrn=GetDeviceCaps(hScrDc,VERTRES);//垂直分辨率

	HANDLE handle=::CreateMutex(NULL,FALSE,L"main");//handle为声明的HANDLE类型的全局变量   
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L"已经运行了一个截图程序。");   
		return FALSE;      
	}   

	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAIN));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
	wcex.hCursor		= LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_MAIN);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,szWindowClass,szTitle,WS_POPUP|WS_VISIBLE,
	   0,0,xScrn,yScrn,NULL,NULL,hInstance,NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   g_hwnd=hWnd;

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	LOGPEN pen;				//定义画笔(红色，1像素宽，连续)
	POINT penWidth;
	penWidth.x=1;
	penWidth.y=1;
	pen.lopnColor=0x000000FF;
	pen.lopnStyle=PS_SOLID;
	pen.lopnWidth=penWidth;
	HPEN hPen=CreatePenIndirect(&pen);

	LOGBRUSH brush;			//定义笔刷（空笔刷）
	brush.lbStyle=BS_NULL;
	HBRUSH hBrush=CreateBrushIndirect(&brush);

	POINT mousePoint;		//鼠标位置
	int width=0,height=0;	//选择局域的宽和高

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	case WM_KEYDOWN:  
		switch (wParam)  
		{  
		case VK_ESCAPE:							//ESC隐藏
			ShowWindow(g_hwnd, SW_HIDE);
			isShow=false;
			break;
		case VK_RETURN:
			if (isSelected)						//截图后回车保存截图
			{
				ShowWindow(g_hwnd, SW_HIDE);
				isShow=false;
				cut.Save(cut.hMemDc,Selectrect);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		{
			(HBRUSH)(GetClassLong(hWnd, GCL_HBRBACKGROUND) - 1);
			hMemDc=CreateCompatibleDC(hdc);									//创建客户区的兼容内存画板
			hBmp=CreateCompatibleBitmap(hdc, xScrn, yScrn);					//创建客户区的兼容的位图对象
			hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);						//选择位图对象到内存画板,获得画笔句柄

			SelectObject(hMemDc,hBrush);									//选择画笔和画刷到客户区内存画板
			SelectObject(hMemDc,hPen);
			if (isMouseDown||isSelected)//如果左键按下或已选择区域
			{
				BitBlt(hMemDc,0,0,xScrn,yScrn,cut.lhMemDc,0,0,SRCCOPY);		//拷贝LowBright截图内存画板到客户区的内存画板
				width=Selectrect.right-Selectrect.left;
				height=Selectrect.bottom-Selectrect.top;
				BitBlt(hMemDc,Selectrect.left,Selectrect.top,width,height,cut.hMemDc,Selectrect.left,Selectrect.top,SRCCOPY);		//拷贝截图内存画板到客户区的内存画板
				Rectangle(hMemDc,Selectrect.left,Selectrect.top,Selectrect.right,Selectrect.bottom);								//画选择框
			}
			else
			{
				BitBlt(hMemDc,0,0,xScrn,yScrn,cut.hMemDc,0,0,SRCCOPY);			//拷贝截图内存画板到客户区的内存画板
			}

			BitBlt(hdc,0, 0, xScrn, yScrn, hMemDc, 0, 0, SRCCOPY);				//拷贝截图内存画板到客户区设备内存画板
		//	StretchBlt(hdc,0,0,xScrn,xScrn tMemDc,0,0,xScrn,xScrn,SRCCOPY);		//拷贝截图内存画板到客户区设备内存画板，自动拉伸
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:				//禁止重绘制背景
			return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		AfxMessageBox(L"  开始截图——Ctrl+Shift+X\n  退出截图——Ctrl+Shift+Q\n  复制截图——左键双击\n  保存截图——ENTER。");
		ShowWindow(hWnd,SW_HIDE);
		if(g_hhook==NULL)
		{
			g_hhook=SetWindowsHookEx(WH_KEYBOARD_LL,GlobleKeyProc,hInst,0);	//加载全局钩子
		}
		break;
	case WM_LBUTTONDOWN:			//鼠标左键按下
		if(!isSelected)								//选择框起点
		{
			SetClassLong(g_hwnd,GCL_HCURSOR ,(LONG)LoadCursor(NULL,IDC_CROSS));
			isMouseDown=true;
			GetCursorPos(&mousePoint);
			Selectrect.left=Selectrect.right=mousePoint.x;
			Selectrect.top=Selectrect.bottom=mousePoint.y;
			InvalidateRgn(hWnd,NULL,false);
		}
		break;
	case WM_MOUSEMOVE:				//鼠标移动
		if((!isSelected)&&(isMouseDown))			//选择框终点
		{
			GetCursorPos(&mousePoint);
			Selectrect.right=mousePoint.x;
			Selectrect.bottom=mousePoint.y;
			InvalidateRgn(hWnd,NULL,false);
		}
		break;
	case WM_LBUTTONUP:				//鼠标左键释放
		if((!isSelected)&&(isMouseDown))			//选择框终结束
		{
			SetClassLong(g_hwnd,GCL_HCURSOR ,(LONG)LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR1)));
			isMouseDown=false;
			isSelected=true;
		}
		break;
	case WM_LBUTTONDBLCLK:			//鼠标左键双击
		if (isSelected)								//复制截图
		{
			ShowWindow(g_hwnd, SW_HIDE);
			isShow=false;
			cut.CopyBmpToClipboard(cut.hMemDc,Selectrect);
		}
		break;
	case WM_RBUTTONDOWN:			//鼠标右键按下
		if (!isSelected)							//未选择区域时右键隐藏窗口
		{
			ShowWindow(g_hwnd, SW_HIDE);
			isShow=false;
		}
		if (isSelected)				//选择时右键取消选择框
		{
			isSelected=false;
			Selectrect.left=Selectrect.top=Selectrect.right=Selectrect.bottom=0;
			InvalidateRgn(hWnd,NULL,false);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*****************************************
全局键盘钩子，提供功能键
******************************************/
LRESULT CALLBACK GlobleKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode>=0)
	{
		PKBDLLHOOKSTRUCT pKeyStruct=(PKBDLLHOOKSTRUCT)lParam;
		if(wParam==WM_KEYUP)
		{
			//截图，快捷键Ctrl+Shift+X
			if(((GetKeyState(VK_CONTROL)&0x80)==0x80)&&((GetKeyState(VK_SHIFT)&0x80)==0x80)&&(pKeyStruct->vkCode==0x58))
			{
				if(!isShow)			//隐藏时截图
				{
					cut.GetScreenCapture();
					InvalidateRgn(g_hwnd,NULL,false);
					isShow=true;
					ShowWindow(g_hwnd, SW_SHOWNORMAL);
				}
			}
			//退出程序，快捷键Ctrl+Alt+Q
			if(((GetKeyState(VK_CONTROL)&0x80)==0x80)&&((GetKeyState(VK_SHIFT)&0x80)==0x80)&&(pKeyStruct->vkCode==0x51))
			{
				if(isShow)			//显示时退出截图
				{
					SetWindowPos(g_hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_HIDEWINDOW);
					AfxMessageBox(L"您退出了截图程序。");
					DestroyWindow(g_hwnd);
				}
			}
		}
	}
	return CallNextHookEx(g_hhook,nCode,wParam,lParam);
}