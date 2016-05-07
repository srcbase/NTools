#pragma once
#include "AC/basedef.h"
#include "AC/sysupport.h"
#include <GdiPlus.h>
//#include <GdiPlusImaging.h>
//#pragma comment(lib, "gdiplus.lib")
//
#if defined(_ATL) || defined(_AFXDLL)
	#include <atlbase.h>
	#include <atlimage.h>
#endif
namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	image类	2014-07-10
//	GDI+图像类封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class image
{
public:
	typedef std::vector<image *> PtrArrayType;
public:
	image();
	image(PCWSTR pFile);
	image(HBITMAP hBitmap);
	image(HICON hIcon);
	image(Gdiplus::Image *pImage);
	image(const image &img);
	virtual ~image();
	operator Gdiplus::Image*() const {		return m_pImage;							}

	image& operator = (Gdiplus::Image *pImage);
	virtual void clear() {
		//gtc::app::writeVSLogs(L"image::clear %08X", m_pImage);
		if (m_hBuffer)
		{
			GlobalFree(m_hBuffer);
			m_hBuffer = NULL;
		}
		if (m_pImage)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
		memset(&m_rcPatch, 0, sizeof(m_rcPatch));
		memset(&m_szImage, 0, sizeof(m_szImage));
	}

	bool load(PCWSTR pFile, bool bCopy = false);
	bool load(HBITMAP hBitmap);
	bool load(HICON hIcon);
	bool load(const SBDataType &imgData);
	bool save(PCWSTR pfilePath, PCWSTR pfileExt = L"jpg");
	image* clone();
public:
	void setPatch(RECT rc);
	void setPatch(LONG left, LONG top, LONG right, LONG bottom);
	RECT getPatch() const;
	bool isPatch() const;
	SIZE getImageSize() const;
	int getWidth() const;
	int getHeight() const;
public:
#if defined(_ATL) || defined(_AFXDLL)
	bool toImage(ATL::CImage &image);
#endif
	HBITMAP toHBitmap(COLORREF clrBk = 0x0FFFFFFFF, SIZE *pszNew = NULL);
public:
	static bool getImageCLSID(PCWSTR pimgExtName, CLSID *pclsidImage);
protected:
	HGLOBAL			m_hBuffer;		// 用来保存来之HBITMAP等创建的图像
	Gdiplus::Image *m_pImage;
	RECT			m_rcPatch;		// 采用九宫格绘制时图片分块定位点

	mutable SIZE	m_szImage;		// 图像大小
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	imageResource类	2014-07-10
//	资源图像类封装
////////////////////////////////////////////////////////////////////////////////////////////////////////

class imageResource : public image
{
protected:
	//HGLOBAL m_hBuffer;
	UINT	m_uiID;
public:
	imageResource()	: /*m_hBuffer(NULL), */m_uiID(0) {  }
	imageResource(PCWSTR pName, PCWSTR pType = MAKEINTRESOURCEW(10) /*RT_RCDATA*/, HMODULE hInst = NULL)
		: /*m_hBuffer(NULL)
		, */m_uiID(0)
	{
		load(pName, pType, hInst);
	}
	imageResource(UINT id, PCWSTR pType = MAKEINTRESOURCEW(10) /*RT_RCDATA*/, HMODULE hInst = NULL)
		: /*m_hBuffer(NULL)
		, */m_uiID(id)
	{ 
		load(id, pType, hInst);
	}
	imageResource(UINT id, UINT type, HMODULE hInst = NULL)
		: /*m_hBuffer(NULL)
		  , */m_uiID(id)
	{ 
		load(id, type, hInst); 
	}
	virtual ~imageResource() {
		//gtc::app::writeVSLogs(L"imageResource free %08X  resid:%d", m_hBuffer, m_uiID);
		clear();
	}

	virtual void clear();

	bool load(PCWSTR pName, PCWSTR pType = MAKEINTRESOURCEW(10)/*RT_RCDATA*/, HMODULE hInst = NULL);
	bool load(UINT id, PCWSTR pType = MAKEINTRESOURCEW(10)/*RT_RCDATA*/, HMODULE hInst = NULL) { 
		m_uiID = id;
		return load(MAKEINTRESOURCEW(id), pType, hInst); 
	}
	bool load(UINT id, UINT type, HMODULE hInst = NULL) { 
		m_uiID = id;
		return load(MAKEINTRESOURCEW(id), MAKEINTRESOURCEW(type), hInst); 
	}

	UINT getResourceID() const {		return m_uiID;				}

};


}