#include "theme.h"
//#include <vsstyle.h>
namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	theme类	2014-12-29
//	windows主题相关操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////

HMODULE	theme::mc_hmdlTheme = NULL;	// UxTheme.dll
static fn_OpenThemeData openThemeData = NULL;
static fn_CloseThemeData closeThemeData = NULL;
static fn_DrawThemeTextEx drawThemeTextEx = NULL;
static fn_GetThemeSysFont getThemeSysFont = NULL;
static fn_DrawThemeIcon drawThemeIcon = NULL;
static fn_DrawThemeBackground drawThemeBackground = NULL;
static fn_DrawThemeParentBackground drawThemeParentBackground = NULL;
static fn_GetThemeColor getThemeColor = NULL;

bool theme::initEnvironment(PCWSTR pdllPath /*= L"UxTheme.dll"*/)
{
	if(mc_hmdlTheme) return true;
	assert(pdllPath && std::wcslen(pdllPath) > 0);
	if(!pdllPath || std::wcslen(pdllPath) <= 0) return false;
	mc_hmdlTheme = ::LoadLibraryW(L"UxTheme.dll");
	if (!mc_hmdlTheme) return false;

	openThemeData = (fn_OpenThemeData)GetProcAddress(mc_hmdlTheme, "OpenThemeData");
	assert(openThemeData);
	closeThemeData = (fn_CloseThemeData)GetProcAddress(mc_hmdlTheme, "CloseThemeData");
	assert(closeThemeData);
	drawThemeTextEx = (fn_DrawThemeTextEx)GetProcAddress(mc_hmdlTheme, "DrawThemeTextEx");
	assert(drawThemeTextEx);
	getThemeSysFont = (fn_GetThemeSysFont)GetProcAddress(mc_hmdlTheme, "GetThemeSysFont");
	assert(getThemeSysFont);
	drawThemeIcon = (fn_DrawThemeIcon)GetProcAddress(mc_hmdlTheme, "DrawThemeIcon");
	assert(drawThemeIcon);
	drawThemeBackground = (fn_DrawThemeBackground)GetProcAddress(mc_hmdlTheme, "DrawThemeBackground");
	assert(drawThemeBackground);
	drawThemeParentBackground = (fn_DrawThemeParentBackground)GetProcAddress(mc_hmdlTheme, "DrawThemeParentBackground");
	assert(drawThemeParentBackground);
	getThemeColor = (fn_GetThemeColor)GetProcAddress(mc_hmdlTheme, "GetThemeColor");
	assert(getThemeColor);

	return mc_hmdlTheme != NULL;
}

void theme::releaseEnvironment()
{
	if(mc_hmdlTheme) ::FreeLibrary(mc_hmdlTheme);
	mc_hmdlTheme = NULL;
}
theme::theme()
	: m_hTheme(NULL)
	, m_hwndTheme(NULL)
{
}
theme::~theme()
{
	close();
}

bool theme::open(HWND hWnd, PCWSTR pClassList)
{
	//assert(openThemeData);
	if(!openThemeData) return false;
	m_hTheme = openThemeData(hWnd, pClassList);

	return m_hTheme != NULL;
}

bool theme::close()
{
	//assert(closeThemeData);
	if(!closeThemeData) return false;
	HRESULT hr = S_FALSE;
	if(m_hTheme) hr = closeThemeData(m_hTheme);
	
	m_hTheme = NULL;
	m_hwndTheme = NULL;

	return SUCCEEDED(hr);
}
bool theme::isOpen() const
{
	return m_hTheme != NULL;
}
bool theme::drawShadowText(HDC hDC, PCWSTR pstrText, LPRECT prcArea, 
	COLORREF clrText /*= 0x0FFFFFF*/, COLORREF clrShadow /*= 0x00*/, 
	DWORD dwFlags /*= DRAWTEXT_FLAG_TITLE*/, int iFontID /*= TMT_ICONTITLEFONT*/)
{
	assert(pstrText && prcArea && hDC);
	assert(m_hTheme && drawThemeTextEx);
	if (!hDC || !pstrText || !prcArea) return false;
	bool bOk = false;
	// 
	// Setup the theme drawing options.
	DTTOPTS dttOpts = {sizeof(DTTOPTS)};
	dttOpts.dwFlags = DTT_COMPOSITED | DTT_SHADOWCOLOR | DTT_TEXTCOLOR | DTT_SHADOWTYPE | DTT_SHADOWOFFSET;
	dttOpts.crShadow = clrShadow & 0x00FFFFFF;
	dttOpts.crText = clrText & 0x00FFFFFF;
	dttOpts.iTextShadowType = TST_CONTINUOUS;
	dttOpts.ptShadowOffset.x = dttOpts.ptShadowOffset.y = 2;
	//// 字体
	//LOGFONTW lgFont = {0};
	//HFONT holdFont = NULL;
	//HFONT hFont = NULL;
	//if (SUCCEEDED(getThemeSysFont(m_hTheme, iFontID, &lgFont)))
	//{
	//	hFont = CreateFontIndirectW(&lgFont);
	//	if(hFont)
	//	{
	//		holdFont = (HFONT)SelectObject(hDC, hFont);
	//	}
	//}

	// 
	bOk = SUCCEEDED(drawThemeTextEx(m_hTheme, hDC, 0, 0, pstrText, -1, dwFlags, prcArea, &dttOpts));
	

	//// release
	//if (hFont)
	//{
	//	SelectObject(hDC, holdFont);
	//	DeleteObject(hFont);
	//}

	return bOk;
}

bool theme::drawIcon(HDC hDC, int iImageIndex, HIMAGELIST hImageList, LPRECT prcArea, int iPartID, int iStateID)
{
	assert(m_hTheme && drawThemeIcon);
	assert(hDC && iImageIndex >= 0 && hImageList && prcArea);
	if(!hDC || iImageIndex < 0 || !hImageList || !prcArea) return false;
	bool bOk = false;

	bOk = SUCCEEDED(drawThemeIcon(m_hTheme, hDC, iPartID, iStateID, prcArea, hImageList, iImageIndex));

	return bOk;
}

bool theme::drawBackground(HDC hDC, LPRECT prcArea, int iPartID, int iStateID)
{
	assert(m_hTheme && drawThemeBackground);
	assert(hDC && prcArea);
	if(!hDC || !prcArea) return false;
	bool bOk = false;

	COLORREF clr = GetSysColor(COLOR_HOTLIGHT);
	//getThemeColor(m_hTheme, iPartID, iStateID, TMT_HOTTRACKING, &clr);
	HBRUSH hBrush = CreateSolidBrush(clr);
	//HBRUSH holdBrush = (HBRUSH)SelectObject(hDC, hBrush);
	FillRect(hDC, prcArea, hBrush);
	DeleteObject(hBrush);

	//bOk = SUCCEEDED(drawThemeBackground(m_hTheme, hDC, iPartID, iStateID, prcArea, NULL));

	return bOk;
}
bool theme::drawParentBackground(HDC hDC, LPRECT prcArea, HWND hwndChild /*= NULL*/)
{
	assert(m_hTheme && drawThemeParentBackground);
	assert(hDC && prcArea);
	if(!hDC || !prcArea) return false;
	if(hwndChild == NULL && m_hwndTheme == NULL) return false;
	bool bOk = false;

	bOk = SUCCEEDED(drawThemeParentBackground(hwndChild ? hwndChild : m_hwndTheme, hDC, prcArea));

	return bOk;
}

}