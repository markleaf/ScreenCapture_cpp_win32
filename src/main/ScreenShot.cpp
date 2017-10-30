
#include "stdafx.h"
#include "ScreenShot.h"
#include <iostream>
using namespace std;

ScreenShot::ScreenShot()
{

}

ScreenShot::~ScreenShot()
{
	DeleteObject(re);
	DeleteObject(hBmp);
	DeleteObject(hOldBmp);
	DeleteDC(hMemDc);
	DeleteDC(pdeskdc);
}

void ScreenShot::GetScreenCapture(void)
{
/*	HDC hScrDc;										//屏幕设备内存画板
	hScrDc=CreateDC(L"DISPLAY",NULL,NULL,NULL);		//获取屏幕设备内存画板
	g_hMemDc=CreateCompatibleDC(hScrDc);			//创建兼容屏幕设备的内存画板
	xScrn=GetDeviceCaps(hScrDc,HORZRES);			//水平分辨率
	yScrn=GetDeviceCaps(hScrDc,VERTRES);			//垂直分辨率
*/
	HWND pDesktop=GetDesktopWindow();
	pdeskdc = GetDC(pDesktop);													//获取桌面窗口内存画板
	GetClientRect(pDesktop,&re);												//获取桌面窗口的尺寸

	hMemDc=CreateCompatibleDC(pdeskdc);											//创建桌面窗口的兼容内存画板
	hBmp=CreateCompatibleBitmap(pdeskdc, re.Width(), re.Height());				//创建桌面窗口的兼容位图对象
	hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);									//选择截图的位图对象到内存画板,获得截图的画笔句柄
	BitBlt(hMemDc,0, 0, re.Width(), re.Height(), pdeskdc, 0, 0, SRCCOPY);		//拷贝桌面窗口的内存画板到截图的内存画板

	CPoint po;
	GetCursorPos(&po);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	HICON hinco = (HICON)GetCursor();
	DrawIcon(hMemDc,po.x, po.y, hinco);											//获取鼠标位置,添加鼠标图像到截图内存画板
//	SelectObject(hMemDc,hOldBmp);												//重新选中截图的画笔句柄,这会清空内存画板数据

	lhMemDc=CreateCompatibleDC(pdeskdc);										//创建桌面窗口的LowBright兼容内存画板
	lhBmp=CreateCompatibleBitmap(pdeskdc, re.Width(), re.Height());				//创建桌面窗口的LowBright兼容位图对象
	lhOldBmp=(HBITMAP)SelectObject(lhMemDc,lhBmp);								//选择LowBright位图对象到LowBright内存画板,获得LowBright画笔句柄
	BitBlt(lhMemDc,0, 0, re.Width(), re.Height(), hMemDc, 0, 0, SRCCOPY);		//拷贝截图位图的内存画板到LowBright截图的内存画板
	CovertToLowBright(lhBmp,lhMemDc);
}

void ScreenShot::CopyBmpToClipboard(HDC sourceDc,CRect rect)
{
	HDC hMemDc;										//区域截图的内存画板
	CBitmap bmp;									//区域截图的位图数据
	HBITMAP hBmp;									//区域截图的位图句柄
	HBITMAP hOldBmp;								//区域截图的画笔句柄
	int width=rect.right-rect.left;
	int hight=rect.bottom-rect.top;

	hMemDc=CreateCompatibleDC(sourceDc);										//创建区域截图的兼容内存画板
	hBmp=CreateCompatibleBitmap(sourceDc, width, hight);						//创建区域截图的兼容位图对象
	hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);									//选择区域截图位图对象到内存画板,获得画笔句柄
	BitBlt(hMemDc,0,0,width,hight, sourceDc,rect.left,rect.top,SRCCOPY);		//拷贝截图的的内存画板到区域截图的内存画板

	HWND hd=GetDesktopWindow();
	if(OpenClipboard(hd))
	{
		EmptyClipboard();
		SetClipboardData(CF_BITMAP,hBmp);
		CloseClipboard();
	}
}

void ScreenShot::Save(HDC sourceDc,CRect rect)
{
	HDC hMemDc;										//区域截图的内存画板
	CBitmap bmp;									//区域截图的位图数据
	HBITMAP hBmp;									//区域截图的位图句柄
	HBITMAP hOldBmp;								//区域截图的画笔句柄
	int width=rect.right-rect.left;
	int hight=rect.bottom-rect.top;

	hMemDc=CreateCompatibleDC(sourceDc);										//创建区域截图的兼容内存画板
	hBmp=CreateCompatibleBitmap(sourceDc, width, hight);						//创建区域截图的兼容位图对象
	hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);									//选择区域截图位图对象到内存画板,获得画笔句柄
	BitBlt(hMemDc,0,0,width,hight, sourceDc,rect.left,rect.top,SRCCOPY);		//拷贝截图的的内存画板到区域截图的内存画板

	CImage image;  
	image.Attach(hBmp);								//将位图加入图片表中

	TCHAR strPath[MAX_PATH];
	TCHAR* lpStrPath = strPath;
	SHGetSpecialFolderPath(0,strPath,CSIDL_DESKTOPDIRECTORY,0);

	CString strFileName="";										//构造文件名
	CTime t = CTime::GetCurrentTime();
	CString tt = t.Format("%Y%m%d%H%M%S");	
	strFileName = _T("截图_");
	strFileName += tt;

	CString strFilter=L"PNG 图像文件|*.png|JPG 图像文件|*.jpg|位图文件(*.bmp)|*.bmp||";
	CFileDialog dlg(false,L"png",strFileName,NULL,strFilter);
	dlg.m_ofn.lpstrInitialDir=lpStrPath;
	if ( IDOK == dlg.DoModal())
	{
		image.Save(dlg.GetPathName());
	}
}

void ScreenShot::CovertToLowBright(HBITMAP hSourceBmp,HDC sourceDc)
{
	HBITMAP retBmp=hSourceBmp;
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	GetDIBits(sourceDc,retBmp,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);
	BYTE* bits=new BYTE[bmpInfo.bmiHeader.biSizeImage];
	GetBitmapBits(retBmp,bmpInfo.bmiHeader.biSizeImage,bits);
	int bytePerPixel=4;//默认32位
	if(bmpInfo.bmiHeader.biBitCount==24)
	{
		bytePerPixel=3;
	}
	for(DWORD i=0;i<bmpInfo.bmiHeader.biSizeImage;i+=bytePerPixel)
	{
		BYTE r=*(bits+i);
		BYTE g=*(bits+i+1);
		BYTE b=*(bits+i+2);
		*(bits+i)=r/1.5;
		*(bits+i+1)=g/1.5;
		*(bits+i+2)=b/1.5;
	}
	SetBitmapBits(retBmp,bmpInfo.bmiHeader.biSizeImage,bits);
	delete[] bits;
}
