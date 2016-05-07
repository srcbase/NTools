#include "drawer.h"

namespace gtc{

static COLORREF pixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
		GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
		GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

}
static BOOL WINAPI alphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
                        int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
    HDC hTempDC = ::CreateCompatibleDC(hDC);
    if (NULL == hTempDC)
        return FALSE;

    //Creates Source DIB
    LPBITMAPINFO lpbiSrc = NULL;
    // Fill in the BITMAPINFOHEADER
    lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
	{
		::DeleteDC(hTempDC);
		return FALSE;
	}
    lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiSrc->bmiHeader.biWidth = dwWidth;
    lpbiSrc->bmiHeader.biHeight = dwHeight;
    lpbiSrc->bmiHeader.biPlanes = 1;
    lpbiSrc->bmiHeader.biBitCount = 32;
    lpbiSrc->bmiHeader.biCompression = BI_RGB;
    lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biClrUsed = 0;
    lpbiSrc->bmiHeader.biClrImportant = 0;

    COLORREF* pSrcBits = NULL;
    HBITMAP hSrcDib = CreateDIBSection (
        hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
        NULL, NULL);

    if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
    {
		delete [] lpbiSrc;
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
    ::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
    ::SelectObject (hTempDC, hOldTempBmp);

    //Creates Destination DIB
    LPBITMAPINFO lpbiDest = NULL;
    // Fill in the BITMAPINFOHEADER
    lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiDest == NULL)
	{
        delete [] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
	}

    lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiDest->bmiHeader.biWidth = dwWidth;
    lpbiDest->bmiHeader.biHeight = dwHeight;
    lpbiDest->bmiHeader.biPlanes = 1;
    lpbiDest->bmiHeader.biBitCount = 32;
    lpbiDest->bmiHeader.biCompression = BI_RGB;
    lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiDest->bmiHeader.biXPelsPerMeter = 0;
    lpbiDest->bmiHeader.biYPelsPerMeter = 0;
    lpbiDest->bmiHeader.biClrUsed = 0;
    lpbiDest->bmiHeader.biClrImportant = 0;

    COLORREF* pDestBits = NULL;
    HBITMAP hDestDib = CreateDIBSection (
        hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
        NULL, NULL);

    if ((NULL == hDestDib) || (NULL == pDestBits))
    {
        delete [] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    ::SelectObject (hTempDC, hDestDib);
    ::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
    ::SelectObject (hTempDC, hOldTempBmp);

    double src_darken;
    BYTE nAlpha;

    for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
    {
        nAlpha = LOBYTE(*pSrcBits >> 24);
        src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
        if( src_darken < 0.0 ) src_darken = 0.0;
        *pDestBits = pixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
    } //for

    ::SelectObject (hTempDC, hDestDib);
    ::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
    ::SelectObject (hTempDC, hOldTempBmp);

    delete [] lpbiDest;
    ::DeleteObject(hDestDib);

    delete [] lpbiSrc;
    ::DeleteObject(hSrcDib);

    ::DeleteDC(hTempDC);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	drawer类		2014-06-21
//	绘图类的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////

//drawer::drawer(HDC hdc)
//	: m_hDC(hdc), m_grap(hdc)
//{
//	m_grap.SetPageUnit(Gdiplus::UnitPixel);
//}
//drawer::~drawer()
//{
//	
//}
void drawer::repositionBitmap(HBITMAP *phBitmap, int minCXCY)
{
	assert(phBitmap);
	if(!phBitmap) return;
	BITMAP bm = {0};
	GetObject(*phBitmap, sizeof(bm), &bm);
	int cxcy = max(max(bm.bmWidth, bm.bmHeight), minCXCY);
	int offX = 0, offY = 0;
	if (cxcy == minCXCY)
	{	// 移动第9格
		offX = (cxcy - bm.bmWidth)/2;
		if(minCXCY <= 32)
			offY = (cxcy-bm.bmHeight)/2;
		else
			offY = cxcy - bm.bmHeight - 6;// (cxcy - bm.bmHeight)/2;
		if(offY < 0) offY = 0;
	}
	else
	{
		if(bm.bmWidth == bm.bmHeight) return;
		if (bm.bmWidth > bm.bmHeight)
		{
			offY = (bm.bmWidth - bm.bmHeight) / 2;
		}
		else
		{
			offX = (bm.bmHeight - bm.bmWidth) / 2;
		}

	}

	HDC hdc = GetDC(NULL);
	HDC hdcDest = CreateCompatibleDC(hdc);
	HDC hdcSrc = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, cxcy, cxcy);
	HBITMAP oldDestBit = (HBITMAP)SelectObject(hdcDest, hBitmap);
	HBITMAP oldSrcBit = (HBITMAP)SelectObject(hdcSrc, *phBitmap);

	::BitBlt(hdcDest, offX, offY, cxcy, cxcy, hdcSrc, 0, 0, SRCCOPY);

	::DeleteObject(*phBitmap);
	*phBitmap = hBitmap;
	SelectObject(hdcSrc, oldSrcBit);
	SelectObject(hdcDest, oldDestBit);
	::DeleteDC(hdcDest);
	::DeleteDC(hdcSrc);
	::ReleaseDC(NULL, hdc);
}
Gdiplus::GraphicsPath* drawer::makeRoundRect(const rect &rcArea, int roundWidth, int roundHeight)
{
	Gdiplus::GraphicsPath path;
	Gdiplus::GraphicsPath *ppath = path.Clone();
	int left = rcArea.x;
	int top = rcArea.y;
	int right = left + rcArea.width;
	int bottom = top + rcArea.height;
	//Gdiplus::GraphicsPath *pgp = gp.Clone();
	ppath->AddArc(right-roundWidth, top, roundWidth, roundHeight, 270, 90);

	ppath->AddArc(right-roundWidth, bottom-roundHeight, roundWidth, roundHeight, 0, 90);

	ppath->AddArc(left, bottom - roundHeight, roundWidth, roundHeight, 90, 90);

	ppath->AddArc(left, top, roundWidth, roundHeight, 180, 90);

	ppath->AddLine(left + roundWidth, top, right - roundWidth/2, top);
	return ppath;
}

void drawer::buildStringFormat(Gdiplus::StringFormat &strformat, BEAlignMode algin)
{
	switch (algin)
	{
	case BEAlignModeLeftTop:
		strformat.SetAlignment(Gdiplus::StringAlignmentNear);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		break;
	case BEAlignModeAllLeft:
	case BEAlignModeLeftCenter:
		strformat.SetAlignment(Gdiplus::StringAlignmentNear);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		break;
	case BEAlignModeLeftBottom:
		strformat.SetAlignment(Gdiplus::StringAlignmentNear);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		break;
	case BEAlignModeCenterTop:
		strformat.SetAlignment(Gdiplus::StringAlignmentCenter);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		break;
	case BEAlignModeAllCenter:
	case BEAlignModeCenterCenter:
		strformat.SetAlignment(Gdiplus::StringAlignmentCenter);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		break;
	case BEAlignModeCenterBottom:
		strformat.SetAlignment(Gdiplus::StringAlignmentCenter);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		break;
	case BEAlignModeRightTop:
		strformat.SetAlignment(Gdiplus::StringAlignmentFar);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		break;
	case BEAlignModeAllRight:
	case BEAlignModeRightCenter:
		strformat.SetAlignment(Gdiplus::StringAlignmentFar);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		break;
	case BEAlignModeRightBottom:
		strformat.SetAlignment(Gdiplus::StringAlignmentFar);
		strformat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		break;
	default:
		break;
	}
}
void drawer::convertToStringFormat(Gdiplus::StringFormat &strformat, UINT uiFormat, bool isVertical)
{
	INT flag = 0;
	//enum StringFormatFlags{  
	//	StringFormatFlagsDirectionRightToLeft = 0x00000001,//从右至左输出文本  
	//	StringFormatFlagsDirectionVertical = 0x00000002,//垂直方向输出文本  
	//	StringFormatFlagsNoFitBlackBox = 0x00000004,  // 允许部分字符延伸该字符串的布局矩形。 默认情况下，将重新定位字符以避免任何延伸。
	//	StringFormatFlagsDisplayFormatControl = 0x00000020,  // 控制字符（如从左到右标记）随具有代表性的标志符号一起显示在输出中
	//	StringFormatFlagsNoFontFallback = 0x00000400,  // 对于请求的字体中不支持的字符，禁用回退到可选字体。 缺失的任何字符都用缺失标志符号的字体显示，通常是一个空的方块。
	//	StringFormatFlagsMeasureTrailingSpaces = 0x00000800,  // 包括每一行结尾处的尾随空格
	//	StringFormatFlagsNoWrap = 0x00001000,//输出时不使用自动换行功能  
	//	StringFormatFlagsLineLimit = 0x00002000,  // 在格式化的矩形中只布置整行
	//	StringFormatFlagsNoClip = 0x00004000//不使用裁剪  
	//};
	if(uiFormat & DT_SINGLELINE)
		flag |= Gdiplus::StringFormatFlagsNoWrap;
	if(uiFormat & DT_NOCLIP) 
		flag |= Gdiplus::StringFormatFlagsNoClip;
	if(isVertical)
		flag |= Gdiplus::StringFormatFlagsDirectionVertical;
	//if(uiFormat & DT_TOP) 
		strformat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	if(uiFormat & DT_VCENTER)
		strformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);	// + DT_SINGLELINE
	if(uiFormat & DT_BOTTOM)
		strformat.SetLineAlignment(Gdiplus::StringAlignmentFar);		// + DT_BOTTOM
	//if(uiFormat & DT_LEFT) 
		strformat.SetAlignment(Gdiplus::StringAlignmentNear);
	if(uiFormat & DT_CENTER) 
		strformat.SetAlignment(Gdiplus::StringAlignmentCenter);
	if(uiFormat & DT_RIGHT) 
		strformat.SetAlignment(Gdiplus::StringAlignmentFar);
	// 
	if(uiFormat & DT_NOPREFIX) 
		strformat.SetHotkeyPrefix(Gdiplus::HotkeyPrefixHide);
	if(uiFormat & DT_PREFIXONLY) 
		strformat.SetHotkeyPrefix(Gdiplus::HotkeyPrefixShow);
	//enum StringTrimming{  
	//	StringTrimmingNone      = 0,			//不使用去尾  
	//	StringTrimmingCharacter = 1,			//以字符为单位去尾  
	//	StringTrimmingWord      = 2,			//以单词为单位去尾  
	//	StringTrimmingEllipsisCharacter = 3,	//以字符为单位去尾，略去部分使用省略号表示  
	//	StringTrimmingEllipsisWord      = 4,	//以单词为单位去尾，略去部分使用省略号表示  
	//	StringTrimmingEllipsisPath      = 5		//略去字符串中间部分，保证字符的首尾都能够显示  
	//};  
	if(uiFormat & DT_END_ELLIPSIS) 
		strformat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
	else if(uiFormat & DT_PATH_ELLIPSIS) 
		strformat.SetTrimming(Gdiplus::StringTrimmingEllipsisPath);
	else if(uiFormat & DT_WORD_ELLIPSIS) 
		strformat.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);
	if (uiFormat & DT_WORDBREAK)
	{
		strformat.SetTrimming(Gdiplus::StringTrimmingWord);
	}

	strformat.SetFormatFlags(flag);
}
void drawer::drawLine(Gdiplus::Graphics &grap, const BSPen &pen, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	Gdiplus::Pen *ppen = pen.toGdiplus();

	grap.DrawLine(ppen, x1, y1, x2, y2);
	delete ppen;
}
void drawer::fillRectangle(Gdiplus::Graphics &grap, const BSBrush &brush, const rect &rt)
{
	Gdiplus::Brush *pbrush = brush.toGdiplus();
	if (!pbrush)
	{
		pbrush = nullptr;
	}
	grap.FillRectangle(pbrush, rt.toGdiplus());

	delete pbrush;
}
void drawer::fillRectangle(Gdiplus::Graphics &grap, DWORD clrbk, const rect &rt)
{
	//Gdiplus::SolidBrush brush(Gdiplus::Color(clrbk));
	//grap.FillRectangle(&brush, rt.toGdiplus());
	fillRectangle(grap, BSBrush(clrbk), rt);
}

void drawer::fillRoundRectangle(Gdiplus::Graphics &grap, const BSBrush &brush, const rect &rcArea, int roundWidth, int roundHeight)
{
	Gdiplus::Brush *pbrush = brush.toGdiplus();
	Gdiplus::GraphicsPath *ppath = makeRoundRect(rcArea, roundWidth, roundHeight);

	grap.FillPath(pbrush, ppath);

	delete ppath;
	delete pbrush;
}
void drawer::drawBorder(Gdiplus::Graphics &grap, const BSPen &pen, const rect &rt, int style /*= BEBorderStyleRectangle*/)
{
	Gdiplus::Pen *ppen = pen.toGdiplus();
	if(style & BEBorderStyleTop)
	{
		grap.DrawLine(ppen, rt.getLeft(), rt.getTop(), rt.getRight(), rt.getTop());
	}
	if(style & BEBorderStyleBottom)
	{
		grap.DrawLine(ppen, rt.getLeft(), rt.getBottom(), rt.getRight(), rt.getBottom());
	}
	if(style & BEBorderStyleLeft)
	{
		grap.DrawLine(ppen, rt.getLeft(), rt.getTop(), rt.getLeft(), rt.getBottom());
	}
	if(style & BEBorderStyleRight)
	{
		grap.DrawLine(ppen, rt.getRight(), rt.getTop(), rt.getRight(), rt.getBottom());
	}
	delete ppen;
}

void drawer::drawRoundBorder(Gdiplus::Graphics &grap, const BSPen &pen, const rect &rcArea, int roundWidth, int roundHeight)
{
	Gdiplus::Pen *ppen = pen.toGdiplus();
	Gdiplus::GraphicsPath *ppath = makeRoundRect(rcArea, roundWidth, roundHeight);
	
	grap.DrawPath(ppen, ppath);
	delete ppath;
	delete ppen;
}

rect drawer::measureString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont
	, UINT uiFormat, INT *pcodepointsFitted /*= NULL*/, INT *plinesFilled /*= NULL*/)
{
	rect rcOut = rtArea;
	if(!ptext || std::wcslen(ptext) <= 0) return rcOut;
	// 绘制区域
	Gdiplus::RectF rc = rtArea;
	// 字体
	HDC hdc = grap.GetHDC();
	Gdiplus::Font gfont(hdc, hFont);
	grap.ReleaseHDC(hdc);
	// 格式
	Gdiplus::StringFormat format;
	convertToStringFormat(format, uiFormat, false);
	Gdiplus::RectF rco;
	if (Gdiplus::Ok == grap.MeasureString(ptext, -1, &gfont, rc, &format, &rco, pcodepointsFitted, plinesFilled))
	{
		rcOut.setRect(rco.X, rco.Y, Gdiplus::REAL(rco.Width+0.99999f), Gdiplus::REAL(rco.Height+0.99999f));
	}
	return rcOut;
}
rect drawer::measureString(PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat
	, INT *pcodepointsFitted /*= NULL*/, INT *plinesFilled /*= NULL*/)
{
	HDC hdc = GetDC(NULL);
	Gdiplus::Graphics grap(hdc);
	rect rc = measureString(grap, ptext, rtArea, hFont, uiFormat, pcodepointsFitted, plinesFilled);
	::ReleaseDC(NULL, hdc);

	return rc;
}
std::wstring drawer::stringMeasure(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat)
{
	assert(hFont);
	if(rtArea.isEmptyArea() || !ptext || std::wcslen(ptext) == 0 || !hFont) return L"";
	std::wstring wstr = ptext;
	std::size_t idx = 0;
	for (idx = 1; idx < wstr.length(); ++idx)
	{
		gtc::rect rc;//(rtArea.x, rtArea.y, 0, 0);
		rc = measureString(grap, wstr.substr(0, idx).c_str(), rc, hFont, uiFormat);
		if (rc.width > rtArea.width || rc.height > rtArea.height)
		{
			return wstr.substr(0, idx-1);
			break;
		}
	}
	return wstr;
}

std::wstring drawer::stringMeasure(PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat)
{
	HDC hdc = GetDC(NULL);
	Gdiplus::Graphics grap(hdc);
	std::wstring wstr = stringMeasure(grap, ptext, rtArea, hFont, uiFormat);
	::ReleaseDC(NULL, hdc);

	return wstr;

}
void drawer::drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, const BSFont &font, 
	BEAlignMode algin /*= BEAlignModeCenterCenter*/, uint32_t clrText /*= 0xff000000*/, bool isVertical /*= false*/,
	bool bTrimming /*= true*/)
{
	HFONT hFont = font.createFont();
	//LOGFONT lgfont = {0};
	//font.toMSFont(&lgfont);
	//HDC hdc = grap.GetHDC();
	//Gdiplus::Font gfont(hdc, &lgfont);
	//grap.ReleaseHDC(hdc);
	UINT flag = algin;
	if(bTrimming) flag |= DT_WORD_ELLIPSIS;
	drawString(grap, ptext, rtArea, hFont, flag, clrText, isVertical);
	if(hFont) DeleteObject(hFont);
}

void drawer::drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, 
	BEAlignMode algin /*= BEAlignModeCenterCenter*/, uint32_t clrText /*= 0xff000000*/, bool isVertical /*= false*/,
	bool bTrimming /*= true*/)
{
	//LOGFONT lgfont = {0};
	//font.toMSFont(&lgfont);
	//HDC hdc = grap.GetHDC();
	//Gdiplus::Font gfont(hdc, hFont);
	//grap.ReleaseHDC(hdc);
	UINT flag = algin;
	if(bTrimming) flag |= DT_WORD_ELLIPSIS;
	drawString(grap, ptext, rtArea, hFont, flag, clrText, isVertical);
}

//void drawer::drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, 
//	const Gdiplus::Font &font, BEAlignMode algin, uint32_t clrText, bool isVertical,
//	bool bTrimming)
//{
//	if(!ptext || std::wcslen(ptext) <= 0)
//		return;
//
//
//	// 格式
//	Gdiplus::StringFormat format;
//	int32_t flag = format.GetFormatFlags();
//	flag |= Gdiplus::StringFormatFlagsNoWrap|Gdiplus::StringFormatFlagsDisplayFormatControl;	// 当行输出
//	if(isVertical)
//		flag |= Gdiplus::StringFormatFlagsDirectionVertical;	// 垂直输出
//	format.SetFormatFlags(flag);
//	if(bTrimming)
//		format.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);
//	format.SetHotkeyPrefix(Gdiplus::HotkeyPrefixShow);
//	buildStringFormat(format, algin);
//	// 字体
//	// 区域
//	// 文本色
//	Gdiplus::SolidBrush brush(clrText);
//	// 
//
//	grap.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
//	Gdiplus::RectF rc = rtArea;
//
//#if defined(WORK_AXIS_CAD)
//	grap.ScaleTransform(1, -1);
//	rc.Y = -rc.Y-rc.Height;
//	grap.DrawString(ptext, -1, &font, rc, &format, &brush);
//	grap.ResetTransform();
//#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
//	grap.DrawString(ptext, -1, &font, rc, &format, &brush);
//#endif
//}
void drawer::drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont
	, UINT uiFormat /*= DT_SINGLELINE|DT_VCENTER|DT_CENTER*/, uint32_t clrText /*= 0xFF000000*/, bool isVertical /*= false*/)
{
	if(!ptext || std::wcslen(ptext) <= 0) return;
	// 绘制区域
	Gdiplus::RectF rc = rtArea;
	// 字体
	HDC hdc = grap.GetHDC();
	Gdiplus::Font gfont(hdc, hFont);
	grap.ReleaseHDC(hdc);
	// 文本色
	Gdiplus::SolidBrush brush(clrText);
	// 格式
	Gdiplus::StringFormat format;
	convertToStringFormat(format, uiFormat, isVertical);
	// 
	//enum TextRenderingHint{  
	//	TextRenderingHintSystemDefault = 0,			//使用与系统相同的处理方式  
	//	TextRenderingHintSingleBitPerPixelGridFit,	//不消除锯齿，使用网格匹配  
	//	TextRenderingHintSingleBitPerPixel,			//不消除锯齿，不使用网格匹配  
	//	TextRenderingHintAntiAliasGridFit,			//消除锯齿，使用网格匹配  
	//	TextRenderingHintAntiAlias,					//消除锯齿，不使用网格匹配  
	//	TextRenderingHintClearTypeGridFit			//在液晶显示器上使用ClearType技术增强字体清晰度  
	//}; 
	grap.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
#if defined(WORK_AXIS_CAD)
	grap.ScaleTransform(1, -1);
	rc.Y = -rc.Y-rc.Height;
	grap.DrawString(ptext, -1, &gfont, rc, &format, &brush);
	grap.ResetTransform();
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	grap.DrawString(ptext, -1, &gfont, rc, &format, &brush);
#endif
}
void drawer::drawStringEffectBk(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, BEAlignMode algin, 
	uint32_t clrText /*= 0xffffffff*/, uint32_t clrBk /*= 0xff212121*/)
{
	if(!ptext || std::wcslen(ptext) <= 0)
		return;
	HDC hdc = grap.GetHDC();
	LOGFONT logfont = {0};
	::GetObject(hFont, sizeof(LOGFONT), &logfont);
	Gdiplus::FontFamily font(logfont.lfFaceName);
	Gdiplus::Font gfont(hdc, hFont);
	HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);
	TEXTMETRIC tm = {0};
	::GetTextMetrics(hdc, &tm);
	::SelectObject(hdc, hOldFont);

	grap.ReleaseHDC(hdc);
	
	//Gdiplus::Bitmap bmp(rtArea.width, rtArea.height, &grap);
	//Gdiplus::GraphicsPath path;
	//Gdiplus::StringFormat strformat;
	//Gdiplus::Rect rc = rtArea;
	//buildStringFormat(strformat, algin);
	//path.AddString(ptext, std::wcslen(ptext), &font, Gdiplus::FontStyleRegular, tm.tmHeight, rc, &strformat);
	//Gdiplus::Graphics *pbmp = Gdiplus::Graphics::FromImage(&bmp);
	//Gdiplus::Matrix mx(1.0f/5,0,0,1.0f/5,-(1.0f/5),-(1.0f/5));
	//pbmp->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);	// 消除锯齿的呈现
	//pbmp->SetTransform(&mx);
	//Gdiplus::Pen pen(Gdiplus::Color(clrBk), 18);
	////pen.SetLineJoin(Gdiplus::LineJoinRound);
	//pbmp->DrawPath(&pen, &path);
	//Gdiplus::SolidBrush brush(clrText);
	//pbmp->FillPath(&brush, &path);

	//grap.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);	// 消除锯齿的呈现
	//grap.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);	// 指定高质量双三次插值法
	//grap.SetTransform(&Gdiplus::Matrix(1,0,0,1,50,50));
	//grap.DrawImage(&bmp,rc,0,0,bmp.GetWidth(),bmp.GetHeight(),Gdiplus::UnitPixel);
	//grap.FillPath(&brush, &path);
	//grap.ResetTransform();
	grap.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);	// 消除锯齿的呈现
	grap.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);	// 指定高质量双三次插值法

	Gdiplus::GraphicsPath path;
	Gdiplus::StringFormat strformat;
	Gdiplus::RectF rc = rtArea;
	//path.AddString(ptext, std::wcslen(ptext), &font, Gdiplus::FontStyleRegular, 20, Gdiplus::Point(100,100), &strformat);
	buildStringFormat(strformat, algin);
	path.AddString(ptext, std::wcslen(ptext), &font, Gdiplus::FontStyleBold, (Gdiplus::REAL)tm.tmHeight, rc, &strformat);

	for(int j=1; j<6; ++j)
	{
		Gdiplus::Pen pen(Gdiplus::Color(clrBk), (Gdiplus::REAL)j);
		pen.SetLineJoin(Gdiplus::LineJoinRound);
		grap.DrawPath(&pen, &path);
	}

	Gdiplus::SolidBrush brush(clrText);
	//grap.FillPath(&brush, &path);
	grap.DrawString(ptext, -1, &gfont, rc, &strformat, &brush);
	
}

bool drawer::drawImage(Gdiplus::Graphics &grap, Gdiplus::Image *pImage,  const rect &rcDest, 
	 const rect &rcScr, Gdiplus::Unit unit /*= Gdiplus::UnitPixel*/)
{
	assert(pImage);
	if(!pImage) return false;
	// 这里有个奇怪的现象，当目标宽度或高度与图像源不一致时，GDI+会从源多绘制一个像素？？？？？？为什么
	INT doupCX = 0, doupCY = 0;
	if(rcDest.width != rcScr.width) doupCX = 1;
	if(rcDest.height != rcScr.height) doupCY = 1;
	return grap.DrawImage(pImage, rcDest, rcScr.x, rcScr.y, rcScr.width-doupCX, rcScr.height-doupCY, unit) == Gdiplus::Ok;
}

bool drawer::drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest, 
	const rect &rcScr, Gdiplus::Unit unit /*= Gdiplus::UnitPixel*/)
{
	assert(pImage);
	if(!pImage) return false;
	if (!pImage->isPatch())
	{
		return drawImage(grap, *pImage, rcDest, rcScr, unit);
	}
	return drawImage9Patch(grap, pImage, rcDest, rcScr, unit);
}

bool drawer::drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest)
{
	assert(pImage);
	if(!pImage) return false;
	RECT rcSrc = {0, 0, pImage->getWidth(), pImage->getHeight()};
	return drawImage(grap, pImage, rcDest, rcSrc);
	//return grap.DrawImage(pImage, rcDest, 0, 0, pImage->GetWidth(), pImage->GetHeight(), Gdiplus::UnitPixel ) == Gdiplus::Ok;
}

bool drawer::drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest, uint32_t clrMask)
{
	assert(pImage);
	if(!pImage) return false;
	Gdiplus::ImageAttributes imgMask;
	imgMask.SetColorKey(clrMask, clrMask);
	return grap.DrawImage(*pImage, rcDest, 0, 0, pImage->getWidth(), pImage->getHeight(), Gdiplus::UnitPixel, &imgMask ) == Gdiplus::Ok;

}
//bool drawer::drawImage9Patch(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, 
//	size szPatch, Gdiplus::Unit unit /*= Gdiplus::UnitPixel*/)
//{
//	/*	绘制步骤
//		1	5	2
//		7	9	8
//		4	6	3
//	*/
//	size szImage = rcScr.getSize();
//	size szDest = rcDest.getSize();
//	if(szImage == szDest)
//		return drawImage(grap, pImage, rcDest, rcScr);
//	if(szPatch.width > szImage.width) szPatch.width = szImage.width;
//	if(szPatch.height > szImage.height) szPatch.height = szImage.height;
//	rect dest, scr;
//	// 1
//	dest = rcDest;
//	scr = rcScr;
//	dest.setSize(szPatch);
//	scr.setSize(szPatch);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 2
//	dest.offset(szDest.width-szPatch.width, 0);
//	scr.offset(szImage.width-szPatch.width, 0);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 3
//	dest.offset(0, szDest.height-szPatch.height);
//	scr.offset(0, szImage.height-szPatch.height);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 4
//	dest.offset(szPatch.width-szDest.width, 0);
//	scr.offset(szPatch.width-szImage.width, 0);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 5
//	dest = rcDest;
//	scr = rcScr;
//	dest.height = szPatch.height;
//	dest.deflate(szPatch.width, 0);
//	scr.height = szPatch.height;
//	scr.deflate(szPatch.width, 0);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 6
//	dest.offset(0, szDest.height-szPatch.height);
//	scr.offset(0, szImage.height-szPatch.height);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 7
//	dest = rcDest;
//	scr = rcScr;
//	dest.width = szPatch.width;
//	dest.deflate(0, szPatch.height);
//	scr.width = szPatch.width;
//	scr.deflate(0, szPatch.height);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 8
//	dest.offset(szDest.width-szPatch.width, 0);
//	scr.offset(szImage.width-szPatch.width, 0);
//	drawImage(grap, pImage, dest, scr, unit);
//	// 9
//	dest = rcDest;
//	scr = rcScr;
//	dest.deflate(szPatch.width, szPatch.height);
//	scr.deflate(szPatch.width, szPatch.height);
//	drawImage(grap, pImage, dest, scr, unit);
//
//	return true;
//}

bool drawer::drawImage9Patch(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, RECT rcCorner, Gdiplus::Unit unit /*= Gdiplus::UnitPixel*/)
{
	/*	九宫个布局为
		1	2	3
		4	5	6
		7	8	9
	*/

	const size szImage = rcScr.getSize();
	const size szDest = rcDest.getSize();
	if(szImage == szDest)
		return drawImage(grap, pImage, rcDest, rcScr);
	if(rcCorner.left > szImage.width) rcCorner.left = szImage.width;
	if(rcCorner.right > szImage.width) rcCorner.right = szImage.width;
	if(rcCorner.top > szImage.height) rcCorner.top = szImage.height;
	if(rcCorner.bottom > szImage.height) rcCorner.bottom = szImage.height;
	rect dest, scr;
	// 1
	dest = rcDest;
	scr = rcScr;
	dest.setSize(rcCorner.left, rcCorner.top);
	scr.setSize(rcCorner.left, rcCorner.top);
	if(rcCorner.left > 0 && rcCorner.top > 0)
		drawImage(grap, pImage, dest, scr, unit);
	// 2
	dest.offset(dest.width, 0);
	dest.width = szDest.width - rcCorner.left - rcCorner.right;
	scr.offset(scr.width, 0);
	scr.width = szImage.width - rcCorner.left - rcCorner.right;
	if(rcCorner.top > 0)
		drawImage(grap, pImage, dest, scr, unit);

	// 3
	dest.offset(dest.width, 0);
	dest.width = rcCorner.right;
	scr.offset(scr.width, 0);
	scr.width = rcCorner.right;
	if(rcCorner.top > 0 && rcCorner.right > 0)
		drawImage(grap, pImage, dest, scr, unit);
	
	// 4
	dest.offset(-(szDest.width-rcCorner.right), dest.height);
	dest.setSize(rcCorner.left, szDest.height - rcCorner.top - rcCorner.bottom);
	scr.offset(-(szImage.width-rcCorner.right), scr.height);
	scr.setSize(rcCorner.left, szImage.height - rcCorner.top - rcCorner.bottom);
	if(rcCorner.left > 0)
		drawImage(grap, pImage, dest, scr, unit);
	// 5
	dest.offset(dest.width, 0);
	scr.offset(scr.width, 0);
	dest.width = szDest.width - rcCorner.left - rcCorner.right;
	scr.width = szImage.width - rcCorner.left - rcCorner.right;
	drawImage(grap, pImage, dest, scr, unit);
	// 6
	dest.offset(dest.width, 0);
	scr.offset(scr.width, 0);
	dest.width = rcCorner.right;
	scr.width = rcCorner.right;
	if(rcCorner.right > 0)
		drawImage(grap, pImage, dest, scr, unit);
	// 7
	dest.offset(-(szDest.width-rcCorner.right), dest.height);
	dest.width = rcCorner.left;
	dest.height = rcCorner.bottom;
	scr.offset(-(szImage.width-rcCorner.right), scr.height);
	scr.width = rcCorner.left;
	scr.height = rcCorner.bottom;
	if(rcCorner.left > 0 && rcCorner.bottom > 0)
		drawImage(grap, pImage, dest, scr, unit);
	// 8
	dest.offset(dest.width, 0);
	dest.width = szDest.width - rcCorner.left - rcCorner.right;
	scr.offset(scr.width, 0);
	scr.width = szImage.width - rcCorner.left - rcCorner.right;
	if(rcCorner.bottom > 0)
		drawImage(grap, pImage, dest, scr, unit);
	// 9
	dest.offset(dest.width, 0);
	dest.width = rcCorner.right;
	scr.offset(scr.width, 0);
	scr.width = rcCorner.right;
	if(rcCorner.right > 0 && rcCorner.bottom > 0)
		drawImage(grap, pImage, dest, scr, unit);

	return true;
}
bool drawer::drawImage9Patch(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest, const rect &rcScr, Gdiplus::Unit unit /*= Gdiplus::UnitPixel*/)
{
	assert(pImage);
	return drawImage9Patch(grap, *pImage, rcDest, rcScr, pImage->getPatch(), unit);
}

void drawer::drawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint,
	const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel,
	BYTE uFade, bool hole, bool xtiled, bool ytiled)
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

	if( lpAlphaBlend == NULL ) lpAlphaBlend = alphaBitBlt;
	if( hBitmap == NULL ) return;

	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
	::SetStretchBltMode(hDC, HALFTONE);

	RECT rcTemp = {0};
	RECT rcDest = {0};
	if( lpAlphaBlend && (alphaChannel || uFade < 255) ) {
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
		// middle
		if( !hole ) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				if( !xtiled && !ytiled ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
				else if( xtiled && ytiled ) {
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int j = 0; j < iTimesY; ++j ) {
						LONG lDestTop = rcDest.top + lHeight * j;
						LONG lDestBottom = rcDest.top + lHeight * (j + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom ) {
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						for( int i = 0; i < iTimesX; ++i ) {
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right ) {
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
								lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
								rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
						}
					}
				}
				else if( xtiled ) {
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					for( int i = 0; i < iTimes; ++i ) {
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if( lDestRight > rcDest.right ) {
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}
						lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
							hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
					}
				}
				else { // ytiled
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int i = 0; i < iTimes; ++i ) {
						LONG lDestTop = rcDest.top + lHeight * i;
						LONG lDestBottom = rcDest.top + lHeight * (i + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom ) {
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
							hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
					}
				}
			}
		}

		// left-top
		if( rcCorners.left > 0 && rcCorners.top > 0 ) {
			rcDest.left = rc.left;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
			}
		}
		// top
		if( rcCorners.top > 0 ) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
					rcCorners.left - rcCorners.right, rcCorners.top, bf);
			}
		}
		// right-top
		if( rcCorners.right > 0 && rcCorners.top > 0 ) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
			}
		}
		// left
		if( rcCorners.left > 0 ) {
			rcDest.left = rc.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
					rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// right
		if( rcCorners.right > 0 ) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
					rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// left-bottom
		if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
			rcDest.left = rc.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
			}
		}
		// bottom
		if( rcCorners.bottom > 0 ) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
					rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
			}
		}
		// right-bottom
		if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
					rcCorners.bottom, bf);
			}
		}
	}
	else 
	{
		if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
			&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
			&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
		{
			if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
					hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
			}
		}
		else
		{
			// middle
			if( !hole ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					if( !xtiled && !ytiled ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
					else if( xtiled && ytiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int j = 0; j < iTimesY; ++j ) {
							LONG lDestTop = rcDest.top + lHeight * j;
							LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							for( int i = 0; i < iTimesX; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
									lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
									rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
							}
						}
					}
					else if( xtiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right ) {
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
						}
					}
					else { // ytiled
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestTop = rcDest.top + lHeight * i;
							LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
						}
					}
				}
			}

			// left-top
			if( rcCorners.left > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
				}
			}
			// top
			if( rcCorners.top > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
						rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
				}
			}
			// right-top
			if( rcCorners.right > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
				}
			}
			// left
			if( rcCorners.left > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
						rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
				}
			}
			// right
			if( rcCorners.right > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
				}
			}
			// left-bottom
			if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
				}
			}
			// bottom
			if( rcCorners.bottom > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
				}
			}
			// right-bottom
			if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
						rcCorners.bottom, SRCCOPY);
				}
			}
		}
	}

	::SelectObject(hCloneDC, hOldBitmap);
	::DeleteDC(hCloneDC);
}
}