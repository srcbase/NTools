#pragma once
#ifndef __IMAGELIST_H__
#define __IMAGELIST_H__
#include "basedef.h"

namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	imageList 类		2015-2-3
//	参考 WTL::CImageList 对HIMAGELIST进行封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class imageList
{
public:
	imageList();
	~imageList();

	operator HIMAGELIST() const {				return m_hImageList;							}
	bool isNull() const {						return (m_hImageList == NULL);					}
	int getImageCount() const;
	bool create(int cx, int cy, UINT nFlags, int nInitial = 1, int nGrow = 1);
	bool destroy();
	int add(HBITMAP hBitmap, HBITMAP hBitmapMask = NULL);
	int add(HBITMAP hBitmap, COLORREF clrMask);
	int add(HICON hIcon);
	bool remove(int nImage);
	bool removeAll();
	bool replace(int nImage, HBITMAP hBitmap, HBITMAP hBitmapMask);
	bool replace(int nImage, HICON hIcon);

	bool getIconSize(SIZE &szIcon);

	HICON extractIcon(int nImage);

	bool draw(HDC hDC, int nImage, int x, int y, UINT nStyle);
	bool draw(HDC hDC, int nImage, POINT pt, UINT nStyle);
	bool draw(int nImage, HDC hDC, RECT& rect, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle);
protected:
	HIMAGELIST	m_hImageList;
};

}	// end acm
#endif