#ifndef __DRAWDATA_H__
#define __DRAWDATA_H__
#pragma once
#include "generalobj.h"
namespace gtc {


///////////////////////////////////////////////////////////////////////////////////////////////////
// 重量单位
static entry* gc_tbUnitWeight[] = {
	new entry(BEUnitWeightMilligram,	L"微克"),
	new entry(BEUnitWeightGram,			L"克"),
	new entry(BEUnitWeightLiang,		L"两"),
	new entry(BEUnitWeightJing,			L"斤"),
	new entry(BEUnitWeightKilogram,		L"公斤"),
	new entry(BEUnitWeightTon,			L"吨")
};
///////////////////////////////////////////////////////////////////////////////////////////////////
// 时间单位
//typedef enum BEUnitTime {
//	BEUnitTimeMillisecond = 1,			// 毫秒
//	BEUnitTimeSecond,					// 秒
//	BEUnitTimeMinute					// 分
//}BEUnitTime;
static entry* gc_tbUnitTime[] = {
	new entry(BEUnitTimeMillisecond,	L"毫秒"),
	new entry(BEUnitTimeSecond,			L"秒"),
	new entry(BEUnitTimeMinute,			L"分")
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- BEAlignMode类型的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbAlignMode[] = {
	new entry(BEAlignModeAllLeft,		L"单行居左"),
	new entry(BEAlignModeAllRight,		L"单行居右"),
	new entry(BEAlignModeAllCenter,		L"单行居中"),
	new entry(BEAlignModeLeftTop,		L"左侧靠上"),
	new entry(BEAlignModeLeftCenter,	L"左侧靠中"),
	new entry(BEAlignModeLeftBottom,	L"左侧靠下"),
	new entry(BEAlignModeCenterTop,		L"中间靠上"),
	new entry(BEAlignModeCenterCenter,	L"中间靠中"),
	new entry(BEAlignModeCenterBottom,	L"中间靠下"),
	new entry(BEAlignModeRightTop,		L"右侧靠上"),
	new entry(BEAlignModeRightCenter,	L"右侧靠中"),
	new entry(BEAlignModeRightBottom,	L"右侧靠下")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 画笔样式的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbPenStyle[] = {
	new entry(BEPenStyleSolid,		L"Solid"),
	new entry(BEPenStyleDash,		L"Dash"),		/* -------  */
	new entry(BEPenStyleDot,		L"Dot"),		/* .......  */
	new entry(BEPenStyleDashDot,	L"DashDot"),	/* _._._._  */
	new entry(BEPenStyleDashDotDot,	L"DashDotDot"),	/* _.._.._  */
	new entry(BEPenStyleCustom,		L"Custom"),
	new entry(BEPenStyleNull,		L"Null")

};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 画刷样式的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbBrushStyle[] = {
	new entry(BEBrushStyleSolid,	L"SOLID"),
	new entry(BEBrushStyleNull,		L"NULL"),
	new entry(BEBrushStyleHatch,	L"HATCHED")//,	
	//entry(3,	L"PATTERN"),
	//entry(5,	L"DIBPATTERN"),
	//entry(6,	L"DIBPATTERNPT"),
	//entry(7,	L"PATTERN8X8"),
	//entry(8,	L"DIBPATTERN8X8")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 画刷样式的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbBrushHatch[] = {
	new entry(BEHatchStyleHorizontal,		L"Horizontal"),			/* ----- */
	new entry(BEHatchStyleVertical,			L"Vertical"),			/* ||||| */
	new entry(BEHatchStyleForwardDiagonal,	L"ForwardDiagonal"),	/* \\\\\ */
	new entry(BEHatchStyleBackwardDiagonal,	L"BackwardDiagonal"),	/* ///// */
	new entry(BEHatchStyleCross,			L"Cross"),				/* +++++ */
	new entry(BEHatchStyleDiagonalCross,	L"DiagonalCross")		/* xxxxx */
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 纸张方向的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbPaperOrientation[] = {
	new entry(1,	L"纵向"), 
	new entry(0,	L"横向")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 页码样式的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbPageNoStyle[] = {
	new entry(0,	L"空白"), 
	new entry(1,	L"第？页 共x页"),
	new entry(2,	L"日期")
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- CBItemGrid列标题的样式的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbGridColCaptionStyle[] = {
	new entry(BEGridColCaptionStyleNone,	L"无标题"), 
	new entry(BEGridColCaptionStyleSingle,	L"单行标题"),
	new entry(BEGridColCaptionStyleDouble,	L"双行标题")
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 单元格边框样式的字典数组entry[]	2013-12-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
static entry* gc_tbBoardStyle[] = {
	new entry(BEBorderStyleNone,		L"无边框"), 
	new entry(BEBorderStyleTop,			L"仅画上边框"),
	new entry(BEBorderStyleBottom,		L"仅画下边框"),
	new entry(BEBorderStyleLeft,		L"仅画左边框"),
	new entry(BEBorderStyleRight,		L"仅画右边框"),
	new entry(BEBorderStyleTopBottom,	L"画上下边框"),
	new entry(BEBorderStyleLeftRight,	L"画左右边框"),
	new entry(BEBorderStyleRectangle,	L"矩形边框")
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 单元格边框样式的字典数组entry[]	2015-08-06 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////

}
#endif