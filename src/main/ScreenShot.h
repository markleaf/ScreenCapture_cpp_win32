
#pragma once  
#include "stdafx.h"

#include <GdiPlus.h>  
using namespace Gdiplus;  
#pragma comment(lib,"GdiPlus.lib")   

class ScreenShot
{
public:
	ScreenShot();
	~ScreenShot();
	HDC pdeskdc;		//桌面窗口的内存画板
	CRect re;			//桌面窗口尺寸

	HDC hMemDc;			//截图的内存画板
	CBitmap bmp;		//截图的位图对象
	HBITMAP hBmp;		//截图的位图句柄
	HBITMAP hOldBmp;	//截图的画笔句柄

	HDC lhMemDc;		//LowBright截图的内存画板
	CBitmap lbmp;		//LowBright截图的位图对象
	HBITMAP lhBmp;		//LowBright截图的位图句柄
	HBITMAP lhOldBmp;	//LowBright截图的画笔句柄

	void GetScreenCapture();
	void Save(HDC sourceDc,CRect rect);
	void CopyBmpToClipboard(HDC sourceDc,CRect rect);
	void CovertToLowBright(HBITMAP hSourceBmp,HDC sourceDc);

private:
};
