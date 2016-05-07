#pragma once
#include "AC/basedef.h"
#include "AC/drawdef.h"
#include "AC/baseobj.h"
#include "AC/image.h"
#if defined(_WIN32)
// GDI+
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#endif
namespace gtc{
/////////////////////////////////////////////////////////////////////////////////////////////////////
////	2015-1-22	
//static Gdiplus::Rect toGdiplus(const RECT &rt)
//{
//	Gdiplus::Rect rc(rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top);
//	return rc;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	drawer类		2014-06-21
//	绘图类的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////

class drawer
{
public:
	//drawer(HDC hdc);
	//~drawer();
	/*****************************************************************************************
		<< --- drawer::drawLine	2014-08-04 --- >>
		说明：绘制线
		参数：
		grap	= 绘图环境
		pen		= 画笔
		x1,y1	= 起点坐标
		x2,y2	= 终点坐标
		返回值：
	*****************************************************************************************/
	static void drawLine(Gdiplus::Graphics &grap, const BSPen &pen, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
	/*****************************************************************************************
		<< --- drawer::fillRectangle	2014-06-28 --- >>
		说明：绘制矩形的边框
		参数：
		grap	= 绘图环境
		brush	= 填充样式
		rt		= 边框区域
		返回值：
	*****************************************************************************************/
	static void fillRectangle(Gdiplus::Graphics &grap, const BSBrush &brush, const rect &rt);
	static void fillRectangle(Gdiplus::Graphics &grap, DWORD clrbk, const rect &rt);

	/*****************************************************************************************
		<< --- drawer::fillRoundRectangle	2014-08-04 --- >>
		说明：填充圆角矩形区域
		参数：
		grap		= 绘图环境
		brush		= 填充样式
		rcArea		= 边框区域
		roundWidth	= 圆角区域的宽度
		roundHeight	= 圆角区域的高度
		返回值：
	*****************************************************************************************/
	static void fillRoundRectangle(Gdiplus::Graphics &grap, const BSBrush &brush, const rect &rcArea, int roundWidth, int roundHeight);

	/*****************************************************************************************
		<< --- drawer::rectBorder	2014-06-21 --- >>
		说明：绘制矩形的边框
		参数：
		grap	= 绘图环境
		pen		= 画笔
		rt		= 边框区域
		style	= 边框样式
		返回值：
	*****************************************************************************************/
	static void drawBorder(Gdiplus::Graphics &grap, const BSPen &pen, const rect &rt, int style = BEBorderStyleRectangle);
	/*****************************************************************************************
		<< --- drawer::drawRoundBorder	2014-06-21 --- >>
		说明：绘制矩形的圆角边框
		参数：
		grap		= 绘图环境
		pen			= 画笔
		rcArea		= 边框区域
		roundWidth	= 圆角区域的宽度
		roundHeight	= 圆角区域的高度
		返回值：
	*****************************************************************************************/
	static void drawRoundBorder(Gdiplus::Graphics &grap, const BSPen &pen, const rect &rcArea, int roundWidth, int roundHeight);

	/****************************************************************************************
		<< ---  measureString		2015-7-31 --- >>         
		说明：测量字符串的显示区域
		参数：
			grap		= GDI+对象引用
			ptext		= 被测量的文本
			rtArea		= width=0时计算文本显示需要的高度，height=0时计算文本显示需要的高度
			hFont		= 字体
			uiFormat	= 文本显示格式
		返回值：
	*****************************************************************************************/
	static rect measureString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat, INT *pcodepointsFitted = NULL, INT *plinesFilled = NULL);
	static rect measureString(PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat, INT *pcodepointsFitted = NULL, INT *plinesFilled = NULL);
	
	/****************************************************************************************
		<< --- 	stringMeasure	2015-10-10 --- >>         
		说明：测试指定区域内能显示那些字符，并返回
		参数：
			grap		= GDI+对象引用
			ptext		= 被测量的文本
			rtArea		= 字符显示区域
			hFont		= 字体
			uiFormat	= 文本显示格式
		返回值：可显示的字符
	*****************************************************************************************/
	static std::wstring stringMeasure(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat);
	static std::wstring stringMeasure(PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat);
	/*****************************************************************************************
		<< --- drawer::drawString	2014-01-13 --- >>
		说明：在指定区域输出文本
		参数：
		grap		= GDI+对象引用
		ptext		= 被输出的文本
		font		= 字体引用
		algin		= 文本输出的对齐方式
		rtArea		= 文本输出区域
		clrText		= 文本颜色
		isVertical	= 文本输出方式，true=垂直，false=水平
		bTrimming	= 超出显示区域的文字用"..."代替
		返回值：
	*****************************************************************************************/ 
	static void drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, const BSFont &font, BEAlignMode algin = BEAlignModeCenterCenter, uint32_t clrText = 0xFE000000, bool isVertical = false, bool bTrimming = false);
	static void drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, BEAlignMode algin = BEAlignModeCenterCenter, uint32_t clrText = 0xFE000000, bool isVertical = false, bool bTrimming = false);
	//static void drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, const Gdiplus::Font &font, BEAlignMode algin, uint32_t clrText, bool isVertical, bool bTrimming);
	static void drawString(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, UINT uiFormat = DT_SINGLELINE|DT_VCENTER|DT_CENTER, uint32_t clrText = 0xFE000000, bool isVertical = false);
	
	/****************************************************************************************
		<< --- drawStringEffectBk		2014-11-6 --- >>         
		说明：绘制类似win7桌面节点文字样式的特效
		参数：
			grap		= GDI+对象引用
			ptext		= 被输出的文本
			rtArea		= 文本输出区域
			font		= 字体引用
			clrText		= 文本颜色
			clrBk		= 文字背景色
		返回值：
	*****************************************************************************************/
	static void drawStringEffectBk(Gdiplus::Graphics &grap, PCWSTR ptext, const rect &rtArea, HFONT hFont, BEAlignMode algin, uint32_t clrText = 0xFEFFFFFF, uint32_t clrBk = 0xFE212121);
	/*****************************************************************************************
		<< --- drawer::drawImage	2014-07-16 --- >>
		说明：将图片某区域输出到指定位置
		参数：
		grap		= GDI+对象引用
		pImage		= 被输出的图像
		rcDest		= 图像输出的目标区域
		rcScr		= 图像中的某一部分
		unit		= 单位
		返回值：执行是否成功
	*****************************************************************************************/ 
	static bool drawImage(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	static bool drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest, const rect &rcScr, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	/*****************************************************************************************
		<< ---  controlUC::drawImage		2014-07-31 --- >>
		说明：将图片某一区域绘制到目标位置
		参数：
		grap	= 绘图环境
		pImage	= 图像
		rcDest	= 目标位置
		返回值：执行是否成功
	*****************************************************************************************/
	static bool drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest);
	static bool drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest, uint32_t clrMask);
	/****************************************************************************************
		<< --- drawImage9Part		2015-1-30 --- >>         
		说明：采用九宫格方式绘制图片，如：绘制带有圆角的背景图
				四个角源与目标尺寸一致；上下边的高度与目标一致，长度拉伸；左右边的宽度与目标一致，高度拉伸；中间平铺
		参数：
		grap		= GDI+对象引用
		pImage		= 被输出的图像
		rcDest		= 图像输出的目标区域
		rcScr		= 图像中的某一部分
		szPatch		= 4个角的尺寸，上下边的高=cy，左右边的宽=cx
		unit		= 单位
		返回值：
	*****************************************************************************************/
	//static bool drawImage9Patch(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, size szPatch, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	static bool drawImage9Patch(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, RECT rcCorner, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	static bool drawImage9Patch(Gdiplus::Graphics &grap, gtc::image *pImage, const rect &rcDest, const rect &rcScr, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	/****************************************************************************************
		<< --- drawImage9Part		2015-1-30 --- >>         
		说明：采用取两端和中间的方式绘制图片，一般绘制区与图片高度一致，如：绘制窄的水平滚动条
				两端与目标尺寸一致；中间高度与目标一致，长度拉伸
		参数：
		grap		= GDI+对象引用
		pImage		= 被输出的图像
		rcDest		= 图像输出的目标区域
		rcScr		= 图像中的某一部分
		iPatch		= 左右两端的宽度，=0时表示其值为图片的高度
		unit		= 单位
		返回值：
	*****************************************************************************************/
	//static bool drawImage3PatchH(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, int iPatch = 0, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	/****************************************************************************************
		<< --- drawImage9Part		2015-1-30 --- >>         
		说明：采用取两端和中间的方式绘制图片，一般绘制区与图片高度一致，如：绘制窄的水平滚动条
				两端与目标尺寸一致；中间宽度与目标一致，高度拉伸
		参数：
		grap		= GDI+对象引用
		pImage		= 被输出的图像
		rcDest		= 图像输出的目标区域
		rcScr		= 图像中的某一部分
		iPatch		= 上下两端的高度，=0时表示其值为图片的宽度
		unit		= 单位
		返回值：
	*****************************************************************************************/
	//static bool drawImage3PatchV(Gdiplus::Graphics &grap, Gdiplus::Image *pImage, const rect &rcDest, const rect &rcScr, int iPatch = 0, Gdiplus::Unit unit = Gdiplus::UnitPixel);

	/****************************************************************************************
		<< --- drawImage 	2016-3-4	kp007 --- >>         
		说明：
		参数：
		返回值：
	*****************************************************************************************/
	static void drawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint,
		const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255,
		bool hole = false, bool xtiled = false, bool ytiled = false);
public:
	/****************************************************************************************
		<< --- 		2015-2-6 --- >>         
		说明：重新绘制位图，新位图宽高相等，且等于原图大边，并将原图的图案至于新图当中
		参数：
			phBitmap	= 被修饰的位图，并设置成新位图
			destCXCY	= 位图最小尺寸，小于此大小则图标居中显示
		返回值：
	*****************************************************************************************/
	static void repositionBitmap(HBITMAP *phBitmap, int minCXCY);
protected:
	/****************************************************************************************
		<< --- makeRoundRect		2014-11-6 --- >>         
		说明：
		参数：
		返回值：
	*****************************************************************************************/
	static Gdiplus::GraphicsPath* makeRoundRect(const rect &rcArea, int roundWidth, int roundHeight);
	/****************************************************************************************
		<< --- 	buildStringFormat	2014-11-6 --- >>         
		说明：构建合适的格式化字符串
		参数：
			strformat	= 被构建字符串格式
			algin		= 对齐方式
		返回值：
	*****************************************************************************************/
	static void buildStringFormat(Gdiplus::StringFormat &strformat, BEAlignMode algin);
	static void convertToStringFormat(Gdiplus::StringFormat &strformat, UINT uiFormat, bool isVertical);
	//protected:
//	HDC m_hDC;
//	Gdiplus::Graphics m_grap;
};

}