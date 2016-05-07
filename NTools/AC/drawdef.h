#ifndef __ANORMAL_H__
#define __ANORMAL_H__
#pragma once

//#include "AC/basedef.h"
//#include <boost/serialization/string.hpp>
//#include <boost/serialization/vector.hpp>
////
#if defined(_AFXDLL)

#elif defined(_WIN32)


#endif

namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	本文档主要定义acg的枚举型、结构型、常量
////////////////////////////////////////////////////////////////////////////////////////////////////////


//// 数据库配置文件中的key
//#define DBKEY_LOGIN				L"LOGIN"
//#define DBKEY_PASSWORD			L"PW"
//#define DBKEY_IP				L"IP"
//#define DBKEY_NAMEORPATH		L"DB"
//#define DBKEY_TYPE				L"TYPE"
//// 消息对话框的标题
//#define _GTDEF_MESSAGEBOX_TITLE		L"信息提示"
//// 日志文件的路径
//#define _GTDEF_DIR_GTDATA			L"GTDATA"
//#define _GTDEF_DIR_GTLOG			L"LOG"
//
//// 基于用户使用的SOCKET发送接收数据类型
//typedef std::vector<char> SCDataType;
//// 字节流数据类型，用来存储二进制数据
//typedef std::vector<uint8_t> SBDataType;

// 映射模式
// 1、采用WORK_AXIS_CAD坐标系时，绘图区在第一象限，向右，向上为递增
//		 ^
//		 |
// height|
//		 |
//		 ----------->
//	   (0,0)   width
//		
// 2、采用WORK_AXIS_DOC || WORK_AXIS_NORMAL坐标系时
// 
//	   (0,0)   width
//		 ----------->
//		 |
// height|
//		 |
//       V
//	
typedef enum BEWorkAxis {
	BEWorkAxisNormal	= 1,	// 常规坐标系，向下、右为递增，逻辑单位为像素
	BEWorkAxisDOC		,		// 文档坐标系，向下、右为递增，逻辑单位为0.1毫米
	BEWorkAxisCAD				// 绘图坐标系，向上、右为递增，逻辑单位为0.1毫米
}BEWorkAxis;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 坐标轴单位类别
typedef enum BEUnitType {
	BEUnitTypeWeight	= 1,		// 重量
	BEUnitTypeTime					// 时间
}BEUnitType;


///////////////////////////////////////////////////////////////////////////////////////////////////
// 重量单位
typedef enum BEUnitWeight{
	 BEUnitWeightMilligram	= 1,		// 毫克
	 BEUnitWeightGram,					// 克
	 BEUnitWeightLiang,					// 两
	 BEUnitWeightJing,					// 斤
	 BEUnitWeightKilogram,				// 公斤
	 BEUnitWeightTon					// 吨
}BEUnitWeight;
static std::wstring toString(BEUnitWeight unit)
{
	switch (unit)
	{
	case BEUnitWeightMilligram:
		return L"毫克";
		break;
	case BEUnitWeightGram:
		return L"克";
		break;
	case BEUnitWeightLiang:
		return L"两";
		break;
	case BEUnitWeightJing:
		return L"斤";
		break;
	case BEUnitWeightKilogram:
		return L"公斤";
		break;
	case BEUnitWeightTon:
		return L"吨";
		break;
	default:
		assert(false && L"记重单位");
		break;
	}
	return L"";
}
// 对应的毫克数
static int toMilligram(BEUnitWeight unit)
{
	switch (unit)
	{
	case BEUnitWeightMilligram:
		return 1;
		break;
	case BEUnitWeightGram:
		return 1000;
		break;
	case BEUnitWeightLiang:
		return 50000;
		break;
	case BEUnitWeightJing:
		return 500000;
		break;
	case BEUnitWeightKilogram:
		return 1000000;
		break;
	case BEUnitWeightTon:
		return 1000000000;
		break;
	default:
		assert(false && L"记重单位");
		break;
	}
	return 0;
}
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
// 时间单位
typedef enum BEUnitTime {
	BEUnitTimeMillisecond = 1,			// 毫秒
	BEUnitTimeSecond,					// 秒
	BEUnitTimeMinute					// 分
}BEUnitTime;
static std::wstring toString(BEUnitTime unit) {
	switch (unit)
	{
	case BEUnitTimeMillisecond:
		return L"毫秒";
		break;
	case BEUnitTimeSecond:
		return L"秒";
		break;
	case BEUnitTimeMinute:
		return L"分";
		break;
	default:
		assert(false && L"记时单位");
		break;
	}
	return L"";
}
// 对应的毫秒数
static int toMillisecond(BEUnitTime unit)
{
	switch (unit)
	{
	case BEUnitTimeMillisecond:
		return 1;
		break;
	case BEUnitTimeSecond:
		return 1000;
		break;
	case BEUnitTimeMinute:
		return 60000;
		break;
	default:
		assert(false && L"记时单位");
		break;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 数据表格列标题样式
typedef enum BEGridColCaptionStyle {
	BEGridColCaptionStyleNone		= 0,	// 不支持列标题
	BEGridColCaptionStyleSingle		,		// 单行标题
	BEGridColCaptionStyleDouble				// 双行标题，第一行标题实行行合并
}BEGridColCaptionStyle;
/*****************************************************************************************
	<< --- toStringBEGridColCaptionStyle		2013-12-16 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	optor = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BEGridColCaptionStyle colCaptionStyle)
{
	switch (colCaptionStyle)
	{
	case BEGridColCaptionStyleNone:
		return L"BEGridColCaptionStyleNone";
		break;
	case BEGridColCaptionStyleSingle:
		return L"BEGridColCaptionStyleSingle";
		break;
	case BEGridColCaptionStyleDouble:
		return L"BEGridColCaptionStyleDouble";
		break;
	default:
		break;
	}
	return L"";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// 画笔的样式
typedef enum BEPenStyle {
	BEPenStyleSolid		= 0,	// 0
	BEPenStyleDash,				// 1/* -------  */
	BEPenStyleDot,				// 2/* .......  */
	BEPenStyleDashDot,			// 3/* _._._._  */
	BEPenStyleDashDotDot,		// 4/* _.._.._  */
	BEPenStyleCustom,			// 5
	BEPenStyleNull				// 6
}BEPenStyle;
///////////////////////////////////////////////////////////////////////////////////////////////////
// 画刷的样式
typedef enum BEBrushStyle {
	BEBrushStyleSolid	= 0,	// 实心
	BEBrushStyleNull	,		// 空
	BEBrushStyleHatch			// 条纹刷
}BEBrushStyle;
///////////////////////////////////////////////////////////////////////////////////////////////////
// 画刷的条纹样式
typedef enum BEHatchStyle {
	BEHatchStyleHorizontal		= 0,	/* ----- */
	BEHatchStyleVertical		,		/* ||||| */
	BEHatchStyleForwardDiagonal	,		/* \\\\\ */
	BEHatchStyleBackwardDiagonal,		/* ///// */
	BEHatchStyleCross			,		/* +++++ */
	BEHatchStyleDiagonalCross			/* xxxxx */

}BEHatchStyle;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 一般用来给CComboBox CListBox控件添加辅助项
typedef enum BEAuxiliaryItem
{
	BEAuxiliaryItemNone		= 0x0,	// 无辅助项
	BEAuxiliaryItemAll		= 0x1,	// 辅助项“全部”，对应的ItemData=-1
	BEAuxiliaryItemWu		= 0x2,	// 辅助项“无”，对应的ItemData=-2
	BEAuxiliaryItemNull		= 0x4	// 辅助项“为空”，对应的ItemData=-3
}BEAuxiliaryItem;


// 


} // end gtc


#endif