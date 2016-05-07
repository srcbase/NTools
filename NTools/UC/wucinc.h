#ifndef __WUCINC_H__
#define __WUCINC_H__
#pragma once

// 很重要哦，指定了common控件的版本，否则tooltip无法正确实现
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "wucdef.h"				// baseobj.h  resourceManager.h  image.h  delegateBase.h

#include "windowWnd.h"			// 
#include "drawManager.h"			// 
#include "wucControl.h"
#include "wucContainer.h"
#include "wucBase.h"
#include "wucMenu.h"
#include "wucCombo.h"
#include "wucAnimate.h"
#include "wucWkeWebkit.h"
#include "wucWnd.h"				// windowWnd.h
#include "wucBubbleWnd.h"
#include "dialogWnd.h"



namespace wuc {
//////////////////////////////////////////////////////////////////////////////////////////
// 2015-12-11		kp007	控件布局

//////////////////////////////////////////////////////////////////////////////////////////
// 2015-12-25		kp007	某个相对于容器的位置
//	1	2	3
//	4	5	6
//	7	8	9
enum BEItemPosition
{
	BEItemPositionTopLeft			= 1,
	BEItemPositionTopCenter			,
	BEItemPositionTopRight			,
	BEItemPositionCenterLeft		,
	BEItemPositionCenterCenter		,
	BEItemPositionCenterRight		,
	BEItemPositionBottomLeft		,
	BEItemPositionBottomCenter		,
	BEItemPositionBottomRight	
};

/****************************************************************************************
	<< --- 	2015-12-25	kp007 --- >>         
	说明：在指定容器指定位置布局一个控件，并返回
	参数：
		pParent		= 父容器
		szControl	= 要生成控件大小
		oriPos		= 生成控件相对父容器的位置
	返回值：返回生成的控件
*****************************************************************************************/
extern horizontalLayoutUC* layoutControl(verticalLayoutUC *pParent, SIZE szControl, BEItemPosition oriPos);
extern horizontalLayoutUC* layoutControl(horizontalLayoutUC *pParent, SIZE szControl, BEItemPosition oriPos);


/****************************************************************************************
	<< --- layoutCenterIcon	2015-12-11	kp007 --- >>         
	说明：在指定区域中心显示图片
	参数：
		pParent		= 图标显示的区域
		pIconKey	= 图标在集合中的key名
	返回值：包含图片的容器，图片不存在返回null
*****************************************************************************************/
extern horizontalLayoutUC* layoutCenterIcon(verticalLayoutUC *pParent, gtc::image *pImage);
extern horizontalLayoutUC* layoutCenterIcon(verticalLayoutUC *pParent, const wchar_t *pIconKey);
extern horizontalLayoutUC* layoutCenterIcon(horizontalLayoutUC *pParent, const wchar_t *pIconKey);

/****************************************************************************************
	<< --- layoutGrid	2015-12-25	kp007 --- >>         
	说明：布局一个网格容器，并返回每个单元格
	参数：
		pParent		= 父容器
		vecRow		= 网格的行高
		vecCol		= 网格的列宽
	返回值：网格单元格集合，即：horizontalLayoutUC*集合
*****************************************************************************************/
extern HContainerArrayType layoutGrid(verticalLayoutUC *pParent, IntArrayType &vecRow, IntArrayType &vecCol);
extern HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, IntArrayType &vecRow, IntArrayType &vecCol);
extern HContainerArrayType layoutGrid(verticalLayoutUC *pParent, int rows, int cols, int rowCY, int colCX);
extern HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, int rows, int cols, int rowCY, int colCX);
// 奇偶不同
extern HContainerArrayType layoutGrid(verticalLayoutUC *pParent, int rows, int cols, int rowOddCY, int rowEvenCY, int colOddCX, int colEvenCX);
extern HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, int rows, int cols, int rowOddCY, int rowEvenCY, int colOddCX, int colEvenCX);
// 行同，列不同
extern HContainerArrayType layoutGrid(verticalLayoutUC *pParent, int rows, int rowCY, IntArrayType &vecCol);
extern HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, int rows, int rowCY, IntArrayType &vecCol);


}	// wuc

#endif	// __WUCINC_H__
