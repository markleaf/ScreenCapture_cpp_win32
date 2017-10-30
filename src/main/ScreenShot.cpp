
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
/*	HDC hScrDc;										//��Ļ�豸�ڴ滭��
	hScrDc=CreateDC(L"DISPLAY",NULL,NULL,NULL);		//��ȡ��Ļ�豸�ڴ滭��
	g_hMemDc=CreateCompatibleDC(hScrDc);			//����������Ļ�豸���ڴ滭��
	xScrn=GetDeviceCaps(hScrDc,HORZRES);			//ˮƽ�ֱ���
	yScrn=GetDeviceCaps(hScrDc,VERTRES);			//��ֱ�ֱ���
*/
	HWND pDesktop=GetDesktopWindow();
	pdeskdc = GetDC(pDesktop);													//��ȡ���洰���ڴ滭��
	GetClientRect(pDesktop,&re);												//��ȡ���洰�ڵĳߴ�

	hMemDc=CreateCompatibleDC(pdeskdc);											//�������洰�ڵļ����ڴ滭��
	hBmp=CreateCompatibleBitmap(pdeskdc, re.Width(), re.Height());				//�������洰�ڵļ���λͼ����
	hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);									//ѡ���ͼ��λͼ�����ڴ滭��,��ý�ͼ�Ļ��ʾ��
	BitBlt(hMemDc,0, 0, re.Width(), re.Height(), pdeskdc, 0, 0, SRCCOPY);		//�������洰�ڵ��ڴ滭�嵽��ͼ���ڴ滭��

	CPoint po;
	GetCursorPos(&po);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	HICON hinco = (HICON)GetCursor();
	DrawIcon(hMemDc,po.x, po.y, hinco);											//��ȡ���λ��,������ͼ�񵽽�ͼ�ڴ滭��
//	SelectObject(hMemDc,hOldBmp);												//����ѡ�н�ͼ�Ļ��ʾ��,�������ڴ滭������

	lhMemDc=CreateCompatibleDC(pdeskdc);										//�������洰�ڵ�LowBright�����ڴ滭��
	lhBmp=CreateCompatibleBitmap(pdeskdc, re.Width(), re.Height());				//�������洰�ڵ�LowBright����λͼ����
	lhOldBmp=(HBITMAP)SelectObject(lhMemDc,lhBmp);								//ѡ��LowBrightλͼ����LowBright�ڴ滭��,���LowBright���ʾ��
	BitBlt(lhMemDc,0, 0, re.Width(), re.Height(), hMemDc, 0, 0, SRCCOPY);		//������ͼλͼ���ڴ滭�嵽LowBright��ͼ���ڴ滭��
	CovertToLowBright(lhBmp,lhMemDc);
}

void ScreenShot::CopyBmpToClipboard(HDC sourceDc,CRect rect)
{
	HDC hMemDc;										//�����ͼ���ڴ滭��
	CBitmap bmp;									//�����ͼ��λͼ����
	HBITMAP hBmp;									//�����ͼ��λͼ���
	HBITMAP hOldBmp;								//�����ͼ�Ļ��ʾ��
	int width=rect.right-rect.left;
	int hight=rect.bottom-rect.top;

	hMemDc=CreateCompatibleDC(sourceDc);										//���������ͼ�ļ����ڴ滭��
	hBmp=CreateCompatibleBitmap(sourceDc, width, hight);						//���������ͼ�ļ���λͼ����
	hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);									//ѡ�������ͼλͼ�����ڴ滭��,��û��ʾ��
	BitBlt(hMemDc,0,0,width,hight, sourceDc,rect.left,rect.top,SRCCOPY);		//������ͼ�ĵ��ڴ滭�嵽�����ͼ���ڴ滭��

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
	HDC hMemDc;										//�����ͼ���ڴ滭��
	CBitmap bmp;									//�����ͼ��λͼ����
	HBITMAP hBmp;									//�����ͼ��λͼ���
	HBITMAP hOldBmp;								//�����ͼ�Ļ��ʾ��
	int width=rect.right-rect.left;
	int hight=rect.bottom-rect.top;

	hMemDc=CreateCompatibleDC(sourceDc);										//���������ͼ�ļ����ڴ滭��
	hBmp=CreateCompatibleBitmap(sourceDc, width, hight);						//���������ͼ�ļ���λͼ����
	hOldBmp=(HBITMAP)SelectObject(hMemDc,hBmp);									//ѡ�������ͼλͼ�����ڴ滭��,��û��ʾ��
	BitBlt(hMemDc,0,0,width,hight, sourceDc,rect.left,rect.top,SRCCOPY);		//������ͼ�ĵ��ڴ滭�嵽�����ͼ���ڴ滭��

	CImage image;  
	image.Attach(hBmp);								//��λͼ����ͼƬ����

	TCHAR strPath[MAX_PATH];
	TCHAR* lpStrPath = strPath;
	SHGetSpecialFolderPath(0,strPath,CSIDL_DESKTOPDIRECTORY,0);

	CString strFileName="";										//�����ļ���
	CTime t = CTime::GetCurrentTime();
	CString tt = t.Format("%Y%m%d%H%M%S");	
	strFileName = _T("��ͼ_");
	strFileName += tt;

	CString strFilter=L"PNG ͼ���ļ�|*.png|JPG ͼ���ļ�|*.jpg|λͼ�ļ�(*.bmp)|*.bmp||";
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
	int bytePerPixel=4;//Ĭ��32λ
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
