#pragma once

namespace wui {


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API renderClip
{
public:
	~renderClip();
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;

	static void generateClip(HDC hDC, RECT rc, renderClip& clip);
	static void generateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, renderClip& clip);
	static void useOldClipBegin(HDC hDC, renderClip& clip);
	static void useOldClipEnd(HDC hDC, renderClip& clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API renderEngine
{
public:
	static DWORD adjustColor(DWORD dwColor, short H, short S, short L);
	static TImageInfo* loadImage(stringOrID bitmap, PCWSTR type = NULL, DWORD mask = 0);
	static void drawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, \
		const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, 
		bool hole = false, bool xtiled = false, bool ytiled = false);
	static bool drawImageString(HDC hDC, paintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, 
		PCWSTR pStrImage, PCWSTR pStrModify = NULL);
	static void drawColor(HDC hDC, const RECT& rc, DWORD color);
	static void drawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

	// 以下函数中的颜色参数alpha值无效
	static void drawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
	static void drawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
	static void drawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
	static void drawText(HDC hDC, paintManagerUI* pManager, RECT& rc, PCWSTR pstrText, \
		DWORD dwTextColor, int iFont, UINT uStyle);
	static void drawHtmlText(HDC hDC, paintManagerUI* pManager, RECT& rc, PCWSTR pstrText, 
		DWORD dwTextColor, RECT* pLinks, stringex* sLinks, int& nLinkRects, UINT uStyle);
	static HBITMAP generateBitmap(paintManagerUI* pManager, controlUI* pControl, RECT rc);
};



}