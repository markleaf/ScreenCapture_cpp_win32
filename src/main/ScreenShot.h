
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
	HDC pdeskdc;		//���洰�ڵ��ڴ滭��
	CRect re;			//���洰�ڳߴ�

	HDC hMemDc;			//��ͼ���ڴ滭��
	CBitmap bmp;		//��ͼ��λͼ����
	HBITMAP hBmp;		//��ͼ��λͼ���
	HBITMAP hOldBmp;	//��ͼ�Ļ��ʾ��

	HDC lhMemDc;		//LowBright��ͼ���ڴ滭��
	CBitmap lbmp;		//LowBright��ͼ��λͼ����
	HBITMAP lhBmp;		//LowBright��ͼ��λͼ���
	HBITMAP lhOldBmp;	//LowBright��ͼ�Ļ��ʾ��

	void GetScreenCapture();
	void Save(HDC sourceDc,CRect rect);
	void CopyBmpToClipboard(HDC sourceDc,CRect rect);
	void CovertToLowBright(HBITMAP hSourceBmp,HDC sourceDc);

private:
};
