#include "imageList.h"
#include <Commctrl.h>
namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	imageList 类		2015-2-3
//	参考 WTL::CImageList 对HIMAGELIST进行封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
imageList::imageList()
	: m_hImageList(NULL)
{
}
imageList::~imageList()
{
	destroy();
}

int imageList::getImageCount() const
{
	assert(m_hImageList);
	return ImageList_GetImageCount(m_hImageList);
}
bool imageList::create(int cx, int cy, UINT nFlags, int nInitial /*= 1*/, int nGrow /*= 1*/)
{
	assert(m_hImageList == NULL);
	m_hImageList = ImageList_Create(cx, cy, nFlags, nInitial, nGrow);
	return m_hImageList != NULL;
}
bool imageList::destroy()
{
	if(!m_hImageList) return false;
	bool bRet = ImageList_Destroy(m_hImageList) == TRUE;
	if(bRet) m_hImageList = NULL;
	return bRet;
}
int imageList::add(HBITMAP hBitmap, HBITMAP hBitmapMask /*= NULL*/)
{
	assert(m_hImageList);
	return ImageList_Add(m_hImageList, hBitmap, hBitmapMask);
}
int imageList::add(HBITMAP hBitmap, COLORREF clrMask)
{
	assert(m_hImageList);
	return ImageList_AddMasked(m_hImageList, hBitmap, clrMask);
}
int imageList::add(HICON hIcon)
{
	assert(m_hImageList);
	return ImageList_AddIcon(m_hImageList, hIcon);
}
bool imageList::remove(int nImage)
{
	assert(m_hImageList);
	return ImageList_Remove(m_hImageList, nImage) == TRUE;
}
bool imageList::removeAll()
{
	assert(m_hImageList);
	return ImageList_RemoveAll(m_hImageList) == TRUE;
}
bool imageList::replace(int nImage, HBITMAP hBitmap, HBITMAP hBitmapMask)
{
	assert(m_hImageList);
	return ImageList_Replace(m_hImageList, nImage, hBitmap, hBitmapMask) == TRUE;
}
bool imageList::replace(int nImage, HICON hIcon)
{
	assert(m_hImageList);
	return ImageList_ReplaceIcon(m_hImageList, nImage, hIcon) != -1;
}

bool imageList::getIconSize(SIZE &szIcon)
{
	assert(m_hImageList);
	szIcon.cx = szIcon.cy = 0;
	int cx = 0, cy = 0;
	bool bRet = ImageList_GetIconSize(m_hImageList, &cx, &cy) == TRUE;
	if (bRet)
	{
		szIcon.cx = cx;
		szIcon.cy = cy;
	}
	return bRet;
}
HICON imageList::extractIcon(int nImage)
{
	assert(m_hImageList);
	return ImageList_ExtractIcon(NULL, m_hImageList, nImage);
}

bool imageList::draw(HDC hDC, int nImage, int x, int y, UINT nStyle)
{
	assert(m_hImageList);
	assert(hDC);
	return ImageList_Draw(m_hImageList, nImage, hDC, x, y, nStyle) == TRUE;
}
bool imageList::draw(HDC hDC, int nImage, POINT pt, UINT nStyle)
{
	assert(m_hImageList);
	assert(hDC);
	return ImageList_Draw(m_hImageList, nImage, hDC, pt.x, pt.y, nStyle) == TRUE;
}
bool imageList::draw(int nImage, HDC hDC, RECT& rect, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle)
{
	assert(m_hImageList);
	assert(hDC);
	return ImageList_DrawEx(m_hImageList, nImage, hDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, rgbBk, rgbFg, fStyle) == TRUE;

}


}	// end acm