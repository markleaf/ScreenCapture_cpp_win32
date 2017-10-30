// main.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "main.h"
#include "ScreenShot.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
LPCTSTR szAppName=L"��Ļ��ͼ����";

HDC hMemDc;										//��ͼ���ڴ滭��
CBitmap bmp;									//��ͼ��λͼ����
HBITMAP hBmp;									//��ͼ��λͼ���
HBITMAP hOldBmp;								//��ͼ�Ļ��ʾ��
ScreenShot cut;
INT xScrn,yScrn;
RECT Selectrect;								//ѡ���
HDC Selecthdc;
BOOL isSelected=false;							//�Ƿ�ѡȡ�˽�ͼ����
BOOL isShow=false;								// �����Ƿ�����ʾ״̬

HHOOK g_hhook;									// ȫ�ּ��̹���
HWND  g_hwnd;									// ���ھ��
LRESULT CALLBACK	GlobleKeyProc(int, WPARAM, LPARAM);//ȫ�ּ��̹��ӿ�ݼ�

BOOL isMouseDown=false;							//�Ƿ������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

//�������
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˷��ô��롣
	HDC hScrDc=CreateDC(L"DISPLAY",NULL,NULL,NULL);
	xScrn=GetDeviceCaps(hScrDc,HORZRES);//ˮƽ�ֱ���
	yScrn=GetDeviceCaps(hScrDc,VERTRES);//��ֱ�ֱ���

	HANDLE handle=::CreateMutex(NULL,FALSE,L"main");//handleΪ������HANDLE���͵�ȫ�ֱ���   
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L"�Ѿ�������һ����ͼ����");   
		return FALSE;      
	}   

	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAIN));

	// ����Ϣѭ��:
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
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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

	LOGPEN pen;				//���廭��(��ɫ��1���ؿ�����)
	POINT penWidth;
	penWidth.x=1;
	penWidth.y=1;
	pen.lopnColor=0x000000FF;
	pen.lopnStyle=PS_SOLID;
	pen.lopnWidth=penWidth;
	HPEN hPen=CreatePenIndirect(&pen);

	LOGBRUSH brush;			//�����ˢ���ձ�ˢ��
	brush.lbStyle=BS_NULL;
	HBRUSH hBrush=CreateBrushIndirect(&brush);

	POINT mousePoint;		//���λ��
	int width=0,height=0;	//ѡ�����Ŀ�͸�

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
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
		case VK_ESCAPE:							//ESC����
			ShowWindow(g_hwnd, SW_HIDE);
			isShow=false;
			break;
		case VK_RETURN:
			if (isSelected)						//��ͼ��س������ͼ
			{
				ShowWindow(g_hwnd, SW_HIDE);
				isShow=false;
				cut.Save(cut.hMemDc,Selectrect);
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		{
			(HBRUSH)(GetClassLong(hWnd, GCL_HBRBACKGROUND) - 1);
			hMemDc=CreateCompatibleDC(hdc);									//�����ͻ����ļ����ڴ滭��
			hBmp=CreateCompatibleBitmap(hdc, xScrn, yScrn);					//�����ͻ����ļ��ݵ�λͼ����
			hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);						//ѡ��λͼ�����ڴ滭��,��û��ʾ��

			SelectObject(hMemDc,hBrush);									//ѡ�񻭱ʺͻ�ˢ���ͻ����ڴ滭��
			SelectObject(hMemDc,hPen);
			if (isMouseDown||isSelected)//���������»���ѡ������
			{
				BitBlt(hMemDc,0,0,xScrn,yScrn,cut.lhMemDc,0,0,SRCCOPY);		//����LowBright��ͼ�ڴ滭�嵽�ͻ������ڴ滭��
				width=Selectrect.right-Selectrect.left;
				height=Selectrect.bottom-Selectrect.top;
				BitBlt(hMemDc,Selectrect.left,Selectrect.top,width,height,cut.hMemDc,Selectrect.left,Selectrect.top,SRCCOPY);		//������ͼ�ڴ滭�嵽�ͻ������ڴ滭��
				Rectangle(hMemDc,Selectrect.left,Selectrect.top,Selectrect.right,Selectrect.bottom);								//��ѡ���
			}
			else
			{
				BitBlt(hMemDc,0,0,xScrn,yScrn,cut.hMemDc,0,0,SRCCOPY);			//������ͼ�ڴ滭�嵽�ͻ������ڴ滭��
			}

			BitBlt(hdc,0, 0, xScrn, yScrn, hMemDc, 0, 0, SRCCOPY);				//������ͼ�ڴ滭�嵽�ͻ����豸�ڴ滭��
		//	StretchBlt(hdc,0,0,xScrn,xScrn tMemDc,0,0,xScrn,xScrn,SRCCOPY);		//������ͼ�ڴ滭�嵽�ͻ����豸�ڴ滭�壬�Զ�����
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:				//��ֹ�ػ��Ʊ���
			return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		AfxMessageBox(L"  ��ʼ��ͼ����Ctrl+Shift+X\n  �˳���ͼ����Ctrl+Shift+Q\n  ���ƽ�ͼ�������˫��\n  �����ͼ����ENTER��");
		ShowWindow(hWnd,SW_HIDE);
		if(g_hhook==NULL)
		{
			g_hhook=SetWindowsHookEx(WH_KEYBOARD_LL,GlobleKeyProc,hInst,0);	//����ȫ�ֹ���
		}
		break;
	case WM_LBUTTONDOWN:			//����������
		if(!isSelected)								//ѡ������
		{
			SetClassLong(g_hwnd,GCL_HCURSOR ,(LONG)LoadCursor(NULL,IDC_CROSS));
			isMouseDown=true;
			GetCursorPos(&mousePoint);
			Selectrect.left=Selectrect.right=mousePoint.x;
			Selectrect.top=Selectrect.bottom=mousePoint.y;
			InvalidateRgn(hWnd,NULL,false);
		}
		break;
	case WM_MOUSEMOVE:				//����ƶ�
		if((!isSelected)&&(isMouseDown))			//ѡ����յ�
		{
			GetCursorPos(&mousePoint);
			Selectrect.right=mousePoint.x;
			Selectrect.bottom=mousePoint.y;
			InvalidateRgn(hWnd,NULL,false);
		}
		break;
	case WM_LBUTTONUP:				//�������ͷ�
		if((!isSelected)&&(isMouseDown))			//ѡ����ս���
		{
			SetClassLong(g_hwnd,GCL_HCURSOR ,(LONG)LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR1)));
			isMouseDown=false;
			isSelected=true;
		}
		break;
	case WM_LBUTTONDBLCLK:			//������˫��
		if (isSelected)								//���ƽ�ͼ
		{
			ShowWindow(g_hwnd, SW_HIDE);
			isShow=false;
			cut.CopyBmpToClipboard(cut.hMemDc,Selectrect);
		}
		break;
	case WM_RBUTTONDOWN:			//����Ҽ�����
		if (!isSelected)							//δѡ������ʱ�Ҽ����ش���
		{
			ShowWindow(g_hwnd, SW_HIDE);
			isShow=false;
		}
		if (isSelected)				//ѡ��ʱ�Ҽ�ȡ��ѡ���
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
ȫ�ּ��̹��ӣ��ṩ���ܼ�
******************************************/
LRESULT CALLBACK GlobleKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode>=0)
	{
		PKBDLLHOOKSTRUCT pKeyStruct=(PKBDLLHOOKSTRUCT)lParam;
		if(wParam==WM_KEYUP)
		{
			//��ͼ����ݼ�Ctrl+Shift+X
			if(((GetKeyState(VK_CONTROL)&0x80)==0x80)&&((GetKeyState(VK_SHIFT)&0x80)==0x80)&&(pKeyStruct->vkCode==0x58))
			{
				if(!isShow)			//����ʱ��ͼ
				{
					cut.GetScreenCapture();
					InvalidateRgn(g_hwnd,NULL,false);
					isShow=true;
					ShowWindow(g_hwnd, SW_SHOWNORMAL);
				}
			}
			//�˳����򣬿�ݼ�Ctrl+Alt+Q
			if(((GetKeyState(VK_CONTROL)&0x80)==0x80)&&((GetKeyState(VK_SHIFT)&0x80)==0x80)&&(pKeyStruct->vkCode==0x51))
			{
				if(isShow)			//��ʾʱ�˳���ͼ
				{
					SetWindowPos(g_hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_HIDEWINDOW);
					AfxMessageBox(L"���˳��˽�ͼ����");
					DestroyWindow(g_hwnd);
				}
			}
		}
	}
	return CallNextHookEx(g_hhook,nCode,wParam,lParam);
}