#ifndef __THEME_H__
#define __THEME_H__
#pragma once
#include "AC/basedef.h"
#include <Uxtheme.h>
#include <tmschema.h>
namespace acm {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	theme类	2014-12-29
//	windows主题相关操作类
//  参考：
//		阴影字：http://msdn.microsoft.com/en-us/library/bb688195(VS.85).aspx
//		扩展的任务栏工具条：http://www.codeproject.com/Articles/14141/Band-Objects-NET-Redux
//		windows常用控件：http://www.codeproject.com/Articles/1688/Add-XP-Visual-Style-Support-to-OWNERDRAW-Controls
//		Theme Explorer：http://www.codeproject.com/Articles/620045/Custom-Controls-in-Win-API-Visual-Styles
////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define DRAWTEXT_FLAG_NODE		(DT_CENTER|DT_MODIFYSTRING|DT_END_ELLIPSIS|DT_WORDBREAK|DT_WORD_ELLIPSIS)
// Uxtheme API声明
typedef HTHEME  (WINAPI* fn_OpenThemeData)(HWND, PCWSTR);
typedef HRESULT (WINAPI* fn_CloseThemeData)(HTHEME);
typedef HRESULT (WINAPI* fn_DrawThemeTextEx)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwFlags, LPRECT pRect, const DTTOPTS *pOptions);
typedef HRESULT (WINAPI* fn_GetThemeSysFont)(HTHEME hTheme, int iFontID, LOGFONTW *plf);
typedef HRESULT (WINAPI* fn_DrawThemeIcon)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex);
typedef HRESULT (WINAPI* fn_DrawThemeBackground)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect);
typedef HRESULT (WINAPI* fn_DrawThemeParentBackground)(HWND hwnd, HDC hdc, const RECT *prc);
typedef HRESULT (WINAPI* fn_GetThemeColor)(HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor);
class theme {
public:
	theme();
	~theme();

	/****************************************************************************************
		<< --- open		2014-12-29 --- >>         
		说明：打开主题
		参数：
			pClassList	= 主题类描述
			hWnd		= 窗口句柄
		返回值：是否执行成功
	*****************************************************************************************/
	bool open(HWND hWnd, PCWSTR pClassList);
	/****************************************************************************************
		<< --- close		2014-12-29 --- >>         
		说明：
		参数：
		返回值：是否执行成功
	*****************************************************************************************/
	bool close();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-5 是否已打开
	bool isOpen() const;
	/****************************************************************************************
		<< --- drawShadowText		2014-12-29 --- >>         
		说明：绘制类似桌面节点标签那种阴影
		参数：
			hdc			= 绘图环境
			pstrText	= 被绘制的文本
			prcArea		= 绘制区域
			dwFlags		= 绘制方式
			iFontID		= 字体索引
			clrText		= 文本色
			clrShadow	= 阴影色
		返回值：是否执行成功
	*****************************************************************************************/
	bool drawShadowText(HDC hDC, PCWSTR pstrText, LPRECT prcArea, COLORREF clrText = 0x0FFFFFF, COLORREF clrShadow = 0x00, DWORD dwFlags = DRAWTEXT_FLAG_TITLE, int iFontID = TMT_ICONTITLEFONT);
	
	/****************************************************************************************
		<< --- drawBackground		2014-12-30 --- >>         
		说明：绘制背景
		参数：
			hdc			= 绘图环境
			prcArea		= 绘制区域
			iPartID		= 绘制控件哪部分
			iStateID	= 所绘制区域的状态
		返回值：是否执行成功
	*****************************************************************************************/
	bool drawBackground(HDC hDC, LPRECT prcArea, int iPartID, int iStateID);

	/****************************************************************************************
		<< --- drawParentBackground		2014-12-30 --- >>         
		说明：绘制父窗口背景
		参数：
		hdc			= 绘图环境
		prcArea		= 绘制区域
		hwndChild	= 子窗口
		返回值：是否执行成功
	*****************************************************************************************/
	bool drawParentBackground(HDC hDC, LPRECT prcArea, HWND hwndChild = NULL);

	/****************************************************************************************
		<< --- drawIcon		2014-12-30 --- >>         
		说明：绘制图标
		参数：
			hdc			= 绘图环境
			iImageIndex	= 在图标列表控件中的索引
			hImageList	= 存放图标集合，图标列表控件
			prcArea		= 绘制区域
			iPartID		= 绘制控件哪部分
			iStateID	= 所绘制区域的状态
		返回值：是否执行成功
	*****************************************************************************************/
	bool drawIcon(HDC hDC, int iImageIndex, HIMAGELIST hImageList, LPRECT prcArea, int iPartID, int iStateID);
public:
	/****************************************************************************************
		<< --- initEnvironment		2014-12-29 --- >>         
		说明：加载主题API，整个系统只需执行一次
		参数：
			pdllPath	= 主题库路径
		返回值：是否执行成功
	*****************************************************************************************/
	static bool initEnvironment(PCWSTR pdllPath = L"UxTheme.dll");
	/****************************************************************************************
		<< --- releaseEnvironment		2014-12-29 --- >>         
		说明：释放initEnvironment加载的资源
		参数：
		返回值：
	*****************************************************************************************/
	static void releaseEnvironment();
protected:
	static HMODULE	mc_hmdlTheme;	// UxTheme.dll
	HWND	m_hwndTheme;	// 主题窗口
	HTHEME	m_hTheme;		// 
};

}
#endif