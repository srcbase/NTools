#ifndef __ANORMAL_H__
#define __ANORMAL_H__
#pragma once
#include <string>
#include <vector>
#include <cstdint>



//#include <boost/serialization/string.hpp>
//#include <boost/serialization/vector.hpp>
////

#if defined(_WIN32)

	#include <windows.h>
	#include <windowsx.h>
	#include <commctrl.h>
	#include <stddef.h>
	#include <richedit.h>
	#include <tchar.h>
	#include <assert.h>
	#include <crtdbg.h>
	#include <malloc.h>
	#include <olectl.h>
	#include <gdiplus.h>
#endif
#if defined(_AFXDLL)
	#include <afxwin.h>
	#include <atltypes.h>
	#include <gdiplus.h>
#endif


namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	本文档主要定义acg的枚举型、结构型、常量
////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MAX_PATH
	#define MAX_PATH 260
#endif
// 数据库配置文件中的key
#define DBKEY_LOGIN				L"LOGIN"
#define DBKEY_PASSWORD			L"PW"
#define DBKEY_IP				L"IP"
#define DBKEY_NAMEORPATH		L"DB"
#define DBKEY_TYPE				L"TYPE"
// 消息对话框的标题
#define _GTDEF_MESSAGEBOX_TITLE		L"信息提示"
// 日志文件的路径
#define _GTDEF_DIR_GTDATA			L"GTDATA"
#define _GTDEF_DIR_GTLOG			L"LOG"

// 基于用户使用的SOCKET发送接收数据类型
typedef std::vector<char> SCDataType;


// 映射模式
//#define WORK_AXIS_NORMAL	// 常规坐标系，向下、右为递增，逻辑单位为像素
//#define WORK_AXIS_DOC		// 文档坐标系，向下、右为递增，逻辑单位为0.1毫米
//#define WORK_AXIS_CAD		// 绘图坐标系，向上、右为递增，逻辑单位为0.1毫米
typedef enum BEWorkAxis {
	BEWorkAxisNormal	= 1,	// 常规坐标系，向下、右为递增，逻辑单位为像素
	BEWorkAxisDOC		,		// 文档坐标系，向下、右为递增，逻辑单位为0.1毫米
	BEWorkAxisCAD				// 绘图坐标系，向上、右为递增，逻辑单位为0.1毫米
}BEWorkAxis;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 自定义通用数据类型
typedef enum BEDataType{
							// VT_TYPE			STL						ADO.DataType			sqlite3
	BEDataTypeNone = 0,		// VT_EMPTY			无						adEmpty					SQLITE_NULL
	//BEDataTypeNULL = 1,		// VT_NULL
	BEDataTypeByte,			// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
	BEDataTypeBoolean,		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
	BEDataTypeString,		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
	BEDataTypeInt16,		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
	BEDataTypeInt32,		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
	BEDataTypeInt64,		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
	BEDataTypeFloat,		// VT_R4			float					adSingle				SQLITE_FLOAT
	BEDataTypeDouble,		// VT_R8			double					adDouble				SQLITE_FLOAT
	BEDataTypeDate,			// VT_DATE			tm						adDate					SQLITE_FLOAT
	BEDataTypeTime,			// VT_DATE			tm						adDate					SQLITE_FLOAT
	BEDataTypeDatetime,		// VT_DATE			tm						adDate					SQLITE_FLOAT
	BEDataTypeImage,		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
	BEDataTypeText			// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
}BEDataType;

/*****************************************************************************************
	<< --- toStringBEDataType		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	optor = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BEDataType optor)
{
	switch (optor)
	{
	case BEDataTypeNone: //= 0,		// VT_EMPTY
		return L"BEDataTypeNone";
		break;
	//case BEDataTypeNULL: // = 1,		// VT_NULL
	//	return L"BEDataTypeNULL";
	//	break;
	case BEDataTypeByte: //,			// VT_UI1
		return L"BEDataTypeByte";
		break;
	case BEDataTypeBoolean: //,		// VT_BOOL
		return L"BEDataTypeBoolean";
		break;
	case BEDataTypeString: //,		// VT_BSTR
		return L"BEDataTypeString";
		break;
	case BEDataTypeInt16:
		return L"BEDataTypeInt16";
		break;
	case BEDataTypeInt32: //,		// VT_I4
		return L"BEDataTypeInt32";
		break;
	case BEDataTypeInt64: //,		// VT_I8
		return L"BEDataTypeInt64";
		break;
	case BEDataTypeFloat: //,		// VT_R4
		return L"BEDataTypeFloat";
		break;
	case BEDataTypeDouble: //,		// VT_R8
		return L"BEDataTypeDouble";
		break;
	case BEDataTypeDate: //,			// VT_DATE
		return L"BEDataTypeDate";
		break;
	case BEDataTypeTime: //,			// VT_DATE
		return L"BEDataTypeTime";
		break;
	case BEDataTypeDatetime: //,		// VT_DATE
		return L"BEDataTypeDatetime";
		break;
	case BEDataTypeImage: //,		// 字节流 VT_ARRAY | VT_UI1。2011-07-12
		return L"BEDataTypeImage";
		break;
	case BEDataTypeText: //			// 大型字符串VT_BSTR
		return L"BEDataTypeText";
		break;
	default:
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 绘图对象动作类别
typedef enum BEItemAction {
	BEItemActionAdd		= 1,	// 新增
	BEItemActionModify	,		// 修改
	BEItemActionDelete	,		// 删除
	BEItemActionSelect			// 选中
}BEItemAction;
///////////////////////////////////////////////////////////////////////////////////////////////////
// 动作的发送方
typedef enum BESenderFlag {
	BESenderFlagApp			= 1,	// 系统App
	BESenderFlagDoc			,		// 文档对象CDocument
	BESenderFlagMainFrame	,		// 主框架CMainFrame
	BESenderFlagView		,		// 视图CView
	BESenderFlagTreeDock	,		// 树状浮动窗口
	BESenderFlagPropDock			// 属性浮动窗口
}BESenderFlag;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 绘图对象动作发起方标记
//	int32_t			iSender;		// 动作触发者标记
//	BEItemAction	beAction;		// 动作类别
typedef struct BSActionSender {
	BESenderFlag	beSender;		// 动作触发者标记
	BEItemAction	beAction;		// 动作类别
	BSActionSender(BESenderFlag sender, BEItemAction action)
		: beSender(sender), beAction(action)
	{}
}BSActionSender;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 坐标轴单位类别
typedef enum BEUnitType {
	BEUnitTypeWeight	= 1,		// 重量
	BEUnitTypeTime					// 时间
}BEUnitType;


///////////////////////////////////////////////////////////////////////////////////////////////////
// 重量单位
typedef enum BEUnitWeight{
	 BEUnitWeightMicrogram	= 1,		// 微克
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
	case BEUnitWeightMicrogram:
		return L"微克";
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
		break;
	}
	return L"";
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
		break;
	}
	return L"";
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
// 数据库参数的输入方向
enum BEParameterDirection{
    BEParameterDirectionUnknown			= 0,//adParamUnknown,// = 0,
    BEParameterDirectionInput			= 1,//adParamInput,// = 1,
    BEParameterDirectionOutput			= 2,//adParamOutput,// = 2,
    BEParameterDirectionInputOUtput		= 3,//adParamInputOutput,// = 3,
    BEParameterDirectionReturnValue		= 4//adParamReturnValue// = 4
};
/*****************************************************************************************
	<< --- toStringBEParameterDirection		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	dir = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BEParameterDirection dir)
{
	switch (dir)
	{
	case BEParameterDirectionUnknown:			//= 0,		// 无 系统中有CM_NONE = 0
		return L"BEParameterDirectionUnknown";
		break;
	case BEParameterDirectionInput:			//= 1,		// 记次
		return L"BEParameterDirectionInput";
		break;
	case BEParameterDirectionOutput:			//= 2,		// 求和
		return L"BEParameterDirectionOutput";
		break;
	case BEParameterDirectionInputOUtput:			//= 3,		// 求平均
		return L"BEParameterDirectionInputOUtput";
		break;
	case BEParameterDirectionReturnValue:			//= 4,		// 最大值
		return L"BEParameterDirectionReturnValue";
		break;
	default:
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 我的水平居中对齐方式
#ifndef DT_TOP
	#define DT_TOP                      0x00000000
#endif
#ifndef DT_LEFT
	#define DT_LEFT                     0x00000000
#endif
#ifndef DT_CENTER
	#define DT_CENTER                   0x00000001
#endif
#ifndef DT_RIGHT
	#define DT_RIGHT                    0x00000002
#endif
#ifndef DT_VCENTER
	#define DT_VCENTER                  0x00000004
#endif
#ifndef DT_BOTTOM
	#define DT_BOTTOM                   0x00000008
#endif
#ifndef DT_SINGLELINE
	#define DT_SINGLELINE               0x00000020
#endif
typedef enum BEAlignMode{
	BEAlignModeAllLeft			= DT_LEFT|DT_SINGLELINE|DT_VCENTER,
	BEAlignModeAllRight			= DT_RIGHT|DT_SINGLELINE|DT_VCENTER,
	BEAlignModeAllCenter		= DT_CENTER|DT_SINGLELINE|DT_VCENTER,
	BEAlignModeLeftTop			= DT_LEFT|DT_TOP,			// 图片顶部左对齐
	BEAlignModeLeftCenter		= DT_LEFT|DT_VCENTER,		// 图片中间左对齐
	BEAlignModeLeftBottom		= DT_LEFT|DT_BOTTOM,		// 图片底部左对齐
	BEAlignModeCenterTop		= DT_CENTER|DT_TOP,			// 图片顶部居中
	BEAlignModeCenterCenter		= DT_CENTER|DT_VCENTER,		// 图片中间居中
	BEAlignModeCenterBottom		= DT_CENTER|DT_BOTTOM,		// 图片底部居中
	BEAlignModeRightTop			= DT_RIGHT|DT_TOP,			// 图片顶部右对齐
	BEAlignModeRightCenter		= DT_RIGHT|DT_VCENTER,		// 图片中间右对齐
	BEAlignModeRightBottom		= DT_RIGHT|DT_BOTTOM		// 图片底部右对齐
}BEAlignMode;
/*****************************************************************************************
	<< --- toStringBEAlignMode		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	optor = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BEAlignMode optor)
{
	switch (optor)
	{
	case BEAlignModeAllLeft:			//= DT_LEFT|DT_SINGLELINE|DT_VCENTER,
		return L"BEAlignModeAllLeft";
		break;
	case BEAlignModeAllRight:			//= DT_RIGHT|DT_SINGLELINE|DT_VCENTER,
		return L"BEAlignModeAllRight";
		break;
	case BEAlignModeAllCenter:			//= DT_CENTER|DT_SINGLELINE|DT_VCENTER,
		return L"BEAlignModeAllCenter";
		break;
	case BEAlignModeLeftTop:			//= DT_LEFT|DT_TOP,			// 图片顶部左对齐
		return L"BEAlignModeLeftTop";
		break;
	case BEAlignModeLeftCenter:			//= DT_LEFT|DT_VCENTER,		// 图片中间左对齐
		return L"BEAlignModeLeftCenter";
		break;
	case BEAlignModeLeftBottom:			//= DT_LEFT|DT_BOTTOM,		// 图片底部左对齐
		return L"BEAlignModeLeftBottom";
		break;
	case BEAlignModeCenterTop:			//= DT_CENTER|DT_TOP,		// 图片顶部居中
		return L"BEAlignModeCenterTop";
		break;
	case BEAlignModeCenterCenter:			//= DT_CENTER,				// 图片中间居中
		return L"BEAlignModeCenterCenter";
		break;
	case BEAlignModeCenterBottom:			//= DT_CENTER|DT_BOTTOM,	// 图片底部居中
		return L"BEAlignModeCenterBottom";
		break;
	case BEAlignModeRightTop:			//= DT_RIGHT|DT_TOP,			// 图片顶部右对齐
		return L"BEAlignModeRightTop";
		break;
	case BEAlignModeRightCenter:			//= DT_RIGHT|DT_VCENTER,	// 图片中间右对齐
		return L"BEAlignModeRightCenter";
		break;
	case BEAlignModeRightBottom:			//= DT_RIGHT					// 图片底部右对齐
		return L"BEAlignModeRightBottom";
		break;
	default:
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 单元格的边框样式
typedef enum BEBorderStyle {
	BEBorderStyleNone			= 0x0,			// 无边界框
	BEBorderStyleTop			= 0x1,			// 仅上边框
	BEBorderStyleBottom			= 0x2,			// 仅下边框
	BEBorderStyleLeft			= 0x4,			// 仅左边框
	BEBorderStyleRight			= 0x8,			// 仅右边框
	BEBorderStyleTopBottom		= 0x3,			// 上下边框
	BEBorderStyleLeftRight		= 0xC,			// 左右边框
	BEBorderStyleRectangle		= 0xF			// 矩形边框

}BEBorderStyle;

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
// 我的计算方式
enum BEComputeMethod{
	BEComputeMethodNone			= 0,		// 无 系统中有CM_NONE = 0
	BEComputeMethodCount		= 1,		// 记次
	BEComputeMethodSum			= 2,		// 求和
	BEComputeMethodAvg			= 3,		// 求平均
	BEComputeMethodMax			= 4,		// 最大值
	BEComputeMethodMin			= 5			// 最小值
};
/*****************************************************************************************
	<< --- toStringBELogicOptor		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	cmethod = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BEComputeMethod cmethod)
{
	switch (cmethod)
	{
	case BEComputeMethodNone:			//= 0,		// 无 系统中有CM_NONE = 0
		return L"BEComputeMethodNone";
		break;
	case BEComputeMethodCount:			//= 1,		// 记次
		return L"BEComputeMethodCount";
		break;
	case BEComputeMethodSum:			//= 2,		// 求和
		return L"BEComputeMethodSum";
		break;
	case BEComputeMethodAvg:			//= 3,		// 求平均
		return L"BEComputeMethodAvg";
		break;
	case BEComputeMethodMax:			//= 4,		// 最大值
		return L"BEComputeMethodMax";
		break;
	case BEComputeMethodMin:			//= 5			// 最小值
		return L"BEComputeMethodMin";
		break;
	default:
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 关系运算符
enum BERelationalOptor{
	BERelationalOptorNone				= 0,		// 未定义的
	BERelationalOptorEquals				= 1,		// 等于
	BERelationalOptorNotEqual			= 2,		// 不等于
	BERelationalOptorLessthan			= 3,		// 小于
	BERelationalOptorLessthanOrEqual	= 4,		// 小于等于
	BERelationalOptorGreaterthan		= 5,		// 大于
	BERelationalOptorGreaterthanOrEqual = 6			// 大于等于
};
/*****************************************************************************************
	<< --- BERelationalOptor		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	relational = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toStirng(BERelationalOptor relational)
{
	switch (relational)
	{
	case BERelationalOptorNone:					//= 0,		// 未定义的
		return L"BERelationalOptorNone";
		break;
	case BERelationalOptorEquals:				//= 1,		// 等于
		return L"BERelationalOptorEquals";
		break;
	case BERelationalOptorNotEqual:				//= 2,		// 不等于
		return L"BERelationalOptorNotEqual";
		break;
	case BERelationalOptorLessthan:				//= 3,		// 小于
		return L"BERelationalOptorLessthan";
		break;
	case BERelationalOptorLessthanOrEqual:		//= 4,		// 小于等于
		return L"BERelationalOptorLessthanOrEqual";
		break;
	case BERelationalOptorGreaterthan:			//= 5,		// 大于
		return L"BERelationalOptorGreaterthan";
		break;
	case BERelationalOptorGreaterthanOrEqual:	//= 6			// 大于等于
		return L"BERelationalOptorGreaterthanOrEqual";
		break;
	default:
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 逻辑运算符
enum BELogicOptor{
	BELogicOptorNone			= 0,		// 未定义的
	BELogicOptorAnd				= 1,		// 逻辑与
	BELogicOptorOr				= 2,		// 或
	BELogicOptorNot				= 3			// 非
};

/*****************************************************************************************
	<< --- toStringBELogicOptor		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	optor = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BELogicOptor optor)
{
	switch (optor)
	{
	case BELogicOptorNone:			//= 0,		// 未定义的
		return L"BELogicOptorNone";
		break;
	case BELogicOptorAnd:			//= 1,		// 逻辑与
		return L"BELogicOptorAnd";
		break;
	case BELogicOptorOr:			//= 2,		// 或
		return L"BELogicOptorOr";
		break;
	case BELogicOptorNot:			//= 3		// 非
		return L"BELogicOptorNot";
		break;
	default:
		break;
	}
	return L"";
}
////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// 系统支持的数据库类型
enum BEDBType{
	BEDBTypeMSSql = 1,		// MSSQL
	BEDBTypeOracle,			// 
	BEDBTypeAccess,			// 
	BEDBTypeSqlite3			// 
};
/*****************************************************************************************
	<< --- toStringBEDBType		2013-11-18 --- >>
	说明：返回BEDBType类型字符描述形式
	参数：
	dbtype	= 数据库类型
	返回值： BEDBType类型字符描述形式
*****************************************************************************************/
static std::wstring toString(BEDBType dbtype)
{
	switch(dbtype)
	{
	case BEDBTypeMSSql: //= 1,		// MSSQL
		return L"BEDBTypeMSSql";
		break;
	case BEDBTypeOracle: //,			// 
		return L"BEDBTypeOracle";
		break;
	case BEDBTypeAccess: //,			// 
		return L"BEDBTypeAccess";
		break;
	case BEDBTypeSqlite3: //			// 
		return L"BEDBTypeSqlite3";
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 命令模式
enum BEDBCmdType{
	BEDBCmdTypeText = 1,//adCmdText,	// 
	BEDBCmdTypeProc = 4	//adCmdStoredProc
};
/*****************************************************************************************
	<< --- toStringBEDBCmdType		2013-11-18 --- >>
	说明：返回BEDBType类型字符描述形式
	参数：
	cmdtype	= 数据库类型
	返回值： BEDBCmdType类型字符描述形式
*****************************************************************************************/
static std::wstring toString(BEDBCmdType cmdtype)
{
	switch(cmdtype)
	{
	case BEDBCmdTypeText: //= 1,		// MSSQL
		return L"BEDBCmdTypeText";
		break;
	case BEDBCmdTypeProc: //,			// 
		return L"BEDBCmdTypeProc";
		break;
	}
	return L"";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 一般用来给CComboBox CListBox控件添加辅助项
typedef enum BEAuxiliaryItem
{
	BEAuxiliaryItemNone		= 0x0,	// 无辅助项
	BEAuxiliaryItemAll		= 0x1,	// 辅助项“全部”，对应的ItemData=-1
	BEAuxiliaryItemWu		= 0x2,	// 辅助项“无”，对应的ItemData=-2
	BEAuxiliaryItemNull		= 0x4	// 辅助项“为空”，对应的ItemData=-3
}BEAuxiliaryItem;

// 点
typedef struct BSPoint
{
	int32_t x;	// 指定点的 X 坐标
	int32_t y;	// 指定点的 Y 坐标
public:
	BSPoint()
		: x(0), y(0)
	{}
	BSPoint(int32_t vx, int32_t vy)
		: x(vx), y(vy)
	{
	}
	BSPoint(const BSPoint &point)
		: x(point.x), y(point.y)
	{
	}
#if defined(_AFXDLL)
	BSPoint(const CPoint &point)
		: x(point.x), y(point.y)
	{
	}
#endif
	BSPoint& operator=(const BSPoint &point)
	{
		if(this != &point)
		{
			x = point.x;
			y = point.y;
		}
		return *this;
	}
	~BSPoint() 
	{
	}
public:
#if defined(_AFXDLL)
	CPoint toMSPoint() const {
		CPoint pt;
		pt.x = x;
		pt.y = y;
		return pt;
	}
	Gdiplus::Point toGdiplus() const {
		return Gdiplus::Point(x, y);
	}
	Gdiplus::PointF toGdiplusF() const {
		return Gdiplus::PointF(x, y);
	}
	// 偏移对象，vx>0向右，vy>0向下
	void offset(int32_t offx, int32_t offy)
	{
#if defined(WORK_AXIS_CAD)
		x += offx;
		y -= offy;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		x += offx;
		y += offy;
#endif
	}
#endif
//private:
//	friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        // 注意，保存时总是保存最新版本
//		ar & x;
//		ar & y;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//		ar & x;
//		ar & y;
//    }
//    BOOST_SERIALIZATION_SPLIT_MEMBER()	

}BSPoint;
// 区域大小
typedef struct BSSize
{
	int32_t cx;		// 区域宽
	int32_t cy;		// 区域高
public:
	BSSize() : cx(0), cy(0)
	{
	}
	BSSize(int32_t width, int32_t height)
		: cx(width), cy(height)
	{
	}
	BSSize(const BSSize &size)
		: cx(size.cx), cy(size.cy)
	{
	}
	BSSize& operator=(const BSSize &size)
	{
		if(this != &size)
		{
			cx = size.cx;
			cy = size.cy;
		}
		return *this;
	}
	~BSSize()
	{
	}
public:
#if defined(_AFXDLL)
	CSize toMSSize() const {
		CSize sz;
		sz.cx = cx;
		sz.cy = cy;
		return sz;
	}
	Gdiplus::Size toGdiplus() const {
		return Gdiplus::Size(cx, cy);
	}
	Gdiplus::SizeF toGdiplusF() const {
		return Gdiplus::SizeF(cx, cy);
	}

	BSSize(const CSize &size)
		: cx(size.cx), cy(size.cy)
	{
	}
	BSSize& operator=(const CSize &size)
	{
		cx = size.cx;
		cy = size.cy;
		return *this;
	}
#endif
//private:
//	friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        // 注意，保存时总是保存最新版本
//		ar & cx;
//		ar & cy;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//		ar & cx;
//		ar & cy;
//    }
//    BOOST_SERIALIZATION_SPLIT_MEMBER()	


}BSSize;

// 定义矩形的左上角和右下角的坐标
typedef struct BSRect
{
	int32_t left;		// 指定矩形的左上角的 x 坐标
	int32_t top;		// 指定矩形的左上角的 y 坐标
	int32_t right;		// 指定矩形的右下角的 x 坐标
	int32_t bottom;	// 指定矩形的右下角的 y 坐标
public:
	BSRect()
		: left(0), top(0), right(0), bottom(0)
	{
	}
	BSRect(BSPoint point, BSSize size)
		: left(point.x), top(point.y), right(left+size.cx)
	{
#if defined(WORK_AXIS_CAD)
		bottom = top-size.cy;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		bottom = top+size.cy;
#endif
	}
	BSRect(int32_t vleft, int32_t vtop, int32_t vright, int32_t vbottom)
		: left(vleft), top(vtop), right(vright), bottom(vbottom)
	{
	}
	BSRect(const BSRect &rect)
		: left(rect.left), top(rect.top), right(rect.right), bottom(rect.bottom)
	{
	}
	BSRect& operator=(const BSRect &rect)
	{
		if(this != &rect)
		{
			left = rect.left;
			top = rect.top;
			right = rect.right;
			bottom = rect.bottom;
		}
		return *this;
	}
	~BSRect() {}
public:
	void setRect(int32_t vleft, int32_t vtop, int32_t vright, int32_t vbottom)
	{
		left = vleft;
		top = vtop;
		right = vright;
		bottom = vbottom;
	}
	void setRect(BSPoint point, BSSize size)
	{
		left = point.x;
		top = point.y;
		right = left + size.cx;
		bottom = top + size.cy;
		//#if defined(WORK_AXIS_CAD)
//		right = left + size.cx;
//		bottom = top - size.cy;
//#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
//		right = left + size.cx;
//		bottom = top + size.cy;
//#else
//		right = left + size.cx;
//		bottom = top + size.cy;
//#endif
	}
	BSPoint topLeft() const
	{
		return BSPoint(left, top);
	}
	BSPoint bottomRight() const
	{
		return BSPoint(right, bottom);
	}
	BSPoint center() const
	{
#if defined(WORK_AXIS_CAD)
		return BSPoint(left+width()/2, top-height()/2);
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		return BSPoint(left+width()/2, top+height()/2);
#else
		return BSPoint(left+width()/2, top+height()/2);
#endif
	}
	int32_t width() const
	{
		return std::abs(right-left);
	}
	int32_t height() const
	{
		return std::abs(bottom-top);
	}
	// 是否为空
	bool isEmpty() const 
	{
		return left==right && top==bottom;
	}
	// 旋转90度
	void rotate90() {
		BSPoint pt = center();
		BSRect rt;
		rt.left = rt.right = pt.x;
		rt.top = rt.bottom = pt.y;
		rt.inflateRect(height()/2, width()/2);
		operator=(rt);
	}

	// 规范化 
	void normalizeRect()
	{
#if defined(WORK_AXIS_CAD)
		if(left > right)
			std::swap(left, right);
		if(bottom > top)
			std::swap(bottom, top);
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		if(left > right)
			std::swap(left, right);
		if(top > bottom)
			std::swap(top, bottom);
#else
		if(left > right)
			std::swap(left, right);
		if(top > bottom)
			std::swap(top, bottom);
#endif
	}
	// 查看点是否在对象内
	bool ptInRect(const BSPoint &pt)
	{
		normalizeRect();
#if defined(WORK_AXIS_CAD)
		return (pt.x >= left && pt.x <= right) && (pt.y >= bottom && pt.y <= top);
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		return (pt.x >= left && pt.x <= right) && (pt.y >= top && pt.y <= bottom);
#else
		return (pt.x >= left && pt.x <= right) && (pt.y >= top && pt.y <= bottom);
#endif
	}
	bool rtInRect(BSRect &rt)
	{
		rt.normalizeRect();
		return ptInRect(rt.topLeft()) && ptInRect(rt.bottomRight());
	}
	// 偏移对象，vx>0向右，vy>0向下
	void offsetRect(int32_t vx, int32_t vy)
	{
#if defined(WORK_AXIS_CAD)
		left += vx;
		right += vx;
		top -= vy;
		bottom -= vy;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left += vx;
		right += vx;
		top += vy;
		bottom += vy;
#else
		left += vx;
		right += vx;
		top += vy;
		bottom += vy;
#endif
	}
	// 扩展对象 >0扩大 <0缩小
	void inflateRect(int32_t vleft, int32_t vtop, int32_t vright, int32_t vbottom)
	{
#if defined(WORK_AXIS_CAD)
		left -= vleft;
		right += vright;
		top += vtop;
		bottom -= vbottom;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left -= vleft;
		right += vright;
		top -= vtop;
		bottom += vbottom;
#else
		left -= vleft;
		right += vright;
		top -= vtop;
		bottom += vbottom;
#endif
	}
	void inflateRect(int32_t vx, int32_t vy){
		inflateRect(vx, vy, vx, vy);
	}
	// 扩展对象 >0缩小 <0扩大
	void deflateRect(int32_t vleft, int32_t vtop, int32_t vright, int32_t vbottom)
	{
#if defined(WORK_AXIS_CAD)
		left += vleft;
		right -= vright;
		top -= vtop;
		bottom += vbottom;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left += vleft;
		right -= vright;
		top += vtop;
		bottom -= vbottom;
#else
		left += vleft;
		right -= vright;
		top += vtop;
		bottom -= vbottom;
#endif
	}
	void deflateRect(int32_t vx, int32_t vy){
		deflateRect(vx, vy, vx, vy);
	}
public:
	// 交集
	BSRect operator&(const BSRect &rect) 
	{
		BSRect rt, rt1(rect);
		rt1.normalizeRect();
		BSRect rt2(*this);
		rt2.normalizeRect();
#if defined(WORK_AXIS_CAD)
		rt.setRect(max(rt1.left,rt2.left), min(rt1.top,rt2.top),
			min(rt1.right,rt2.right), max(rt1.bottom,rt2.bottom));
		if(rt.left > rt.right || rt.top < rt.bottom)
			rt.setRect(0,0,0,0);
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		rt.setRect(max(rt1.left,rt2.left), max(rt1.top,rt2.top),
			min(rt1.right,rt2.right), min(rt1.bottom,rt2.bottom));
		if(rt.left > rt.right || rt.top > rt.bottom)
			rt.setRect(0,0,0,0);
#else
		rt.setRect(max(rt1.left,rt2.left), max(rt1.top,rt2.top),
			min(rt1.right,rt2.right), min(rt1.bottom,rt2.bottom));
		if(rt.left > rt.right || rt.top > rt.bottom)
			rt.setRect(0,0,0,0);
#endif
		return rt;
	}
	void operator&=(const BSRect &rect) 
	{
		*this = *this & rect;
	}
public:
#if defined(_AFXDLL)
	CRect toMSRect() const {
		CRect rt;
		rt.SetRect(left, top, right, bottom);
		return rt;
	}
	Gdiplus::Rect toGdiplus() const {
#if defined(WORK_AXIS_CAD)
		return Gdiplus::Rect(left, bottom, width(), height());
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		return Gdiplus::Rect(left, top, width(), height());
#endif
	}
	Gdiplus::RectF toGdiplusF() const {
#if defined(WORK_AXIS_CAD)
		return Gdiplus::RectF(left, bottom, width(), height());
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		return Gdiplus::RectF(left, top, width(), height());
#endif
	}
	void setRect(RECT rt) {
		left = rt.left;
		right = rt.right;
		top = rt.top;
		bottom = rt.bottom;
		//normalizeRect();
	}

#endif
//private:
//	friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        // 注意，保存时总是保存最新版本
//		ar & left;
//		ar & top;
//		ar & right;
//		ar & bottom;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//		ar & left;
//		ar & top;
//		ar & right;
//		ar & bottom;
//    }
//    BOOST_SERIALIZATION_SPLIT_MEMBER()	

}BSRect;

// 画笔类型
typedef struct BSPen
{

	BEPenStyle	style;	// 画笔样式
	uint32_t	width;	// 逻辑单位,宽度
	uint32_t	color;	// 画笔颜色
public:
	BSPen() : style(BEPenStyleSolid), width(3), color(0xFF000000) {}
	BSPen(BEPenStyle vstyle, uint32_t vwidth, uint32_t vcolor)
		: style(vstyle), width(vwidth), color(vcolor)
	{
	}
	BSPen(const BSPen &pen)
		: style(pen.style), width(pen.width), color(pen.color)
	{
	}
	BSPen& operator=(const BSPen &pen)
	{
		if(this != &pen)
		{
			style = pen.style;
			width = pen.width;
			color = pen.color;
		}
		return *this;
	}
	~BSPen()
	{
	}
public:
#if defined(_WIN32)
	LOGPEN toMSPen() const {
		LOGPEN lgpen;
		lgpen.lopnColor = color;
		lgpen.lopnStyle = style;
		lgpen.lopnWidth.x = lgpen.lopnWidth.y = width;
		return lgpen;
	}
	/*****************************************************************************************
		<< --- BSPen::toGdiplus	2014-01-09 --- >>
		说明：根据自定义画笔结构生成绘制Gdiplus::Pen*
		参数：
		返回值：根据画笔样式生成Gdiplus::Pen*，调用者负责释放
	*****************************************************************************************/
	Gdiplus::Pen* toGdiplus() const {
		Gdiplus::Pen *ppen = new Gdiplus::Pen(color, width);
		if(style == BEPenStyleNull)
			ppen->SetColor(color & 0x00FFFFFF);	// 透明
		ppen->SetDashStyle(Gdiplus::DashStyle(style));
		return ppen;
	}

#endif
//private:
//	friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        // 注意，保存时总是保存最新版本
//		ar & style;
//		ar & width;
//		ar & color;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//		ar & style;
//		ar & width;
//		ar & color;
//    }
    //BOOST_SERIALIZATION_SPLIT_MEMBER()	
}BSPen;

// 定义了一个实际画刷的样式、颜色和模式
typedef struct BSBrush
{
	BEBrushStyle	style;		// 画刷样式
	uint32_t		backColor;	// 画刷背景填充颜色
	uint32_t		foreColor;	// 画刷前景色
	BEHatchStyle	hatch;		// 阴影样式
public:
	BSBrush() : style(BEBrushStyleNull), backColor(0xFFFFFF), foreColor(0xFF000000), hatch(BEHatchStyleHorizontal)
	{
	}
	BSBrush(BEBrushStyle vstyle, uint32_t vforeColor, uint32_t vbackColor, BEHatchStyle vhatch)
		: style(vstyle), foreColor(vforeColor), backColor(vbackColor), hatch(vhatch)
	{
	}
	BSBrush(uint32_t vbackColor)
		: style(BEBrushStyleSolid), foreColor(vbackColor), backColor(vbackColor), hatch(BEHatchStyleHorizontal)
	{
	}
	BSBrush(const BSBrush &brush)
		: style(brush.style), foreColor(brush.foreColor), backColor(brush.backColor), hatch(brush.hatch)
	{
	}
	BSBrush& operator=(const BSBrush &brush)
	{
		if(this != &brush)
		{
			style = brush.style;
			foreColor = brush.foreColor;
			backColor = brush.backColor;
			hatch = brush.hatch;
		}
		return *this;
	}
	~BSBrush()
	{
	}
public:
#if defined(_WIN32)
	LOGBRUSH toMSBrush() const {
		LOGBRUSH lgbrush;
		lgbrush.lbColor = foreColor;
		lgbrush.lbHatch = hatch;
		lgbrush.lbStyle = style;
		return lgbrush;
	}
	/*****************************************************************************************
		<< --- BSBrush::toGdiplus	2014-01-09 --- >>
		说明：根据自定义画刷结构生成绘制Gdiplus::Brush
		参数：
		返回值：根据画刷样式生成SolidBrush或HatchBrush形画刷，调用者负责释放
	*****************************************************************************************/
	Gdiplus::Brush* toGdiplus() const {
		Gdiplus::Brush *pbrush = nullptr;
		switch (style)
		{
		case BEBrushStyleSolid:
			pbrush = new Gdiplus::SolidBrush(backColor);
			break;
		case BEBrushStyleNull:
			pbrush = new Gdiplus::SolidBrush(Gdiplus::Color::Transparent);// backColor & 0x00FFFFFF);
			break;
		case BEBrushStyleHatch:
			pbrush = new Gdiplus::HatchBrush(Gdiplus::HatchStyle(hatch), foreColor, backColor);
			break;
		default:
			break;
		}
		return pbrush;
	}

#endif
//private:
//	friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        // 注意，保存时总是保存最新版本
//		ar & style;
//		ar & foreColor;
//		ar & backColor;
//		ar & hatch;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//		ar & style;
//		ar & foreColor;
//		ar & backColor;
//		ar & hatch;
//    }
//    BOOST_SERIALIZATION_SPLIT_MEMBER()	
}BSBrush;
// 字体类别
typedef enum BEFontSize {
	BEFontSizeZero=0,BEFontSizeZeroSmall,BEFontSizeOne,BEFontSizeOneSmall,BEFontSizeTwo,BEFontSizeTwoSmall,
	BEFontSizeThree,BEFontSizeThreeSmall,BEFontSizeFour,BEFontSizeFourSmall,BEFontSizeFive,BEFontSizeFiveSmall,
	BEFontSizeSix,BEFontSizeSixSmall,BEFontSizeSeven,BEFontSizeEight
}BEFontSize;

// 字体结构
typedef struct BSFont
{
#define FACENAME_MAX 32
	int32_t	iHeight;		// 以逻辑单位指定字体字符元(character cell)或字符的高度
	int32_t	iWidth;			// 以逻辑单位指定字体字符的平均宽度
	int32_t	iEscapement;	// 以十分之一度为单位指定每一行文本输出时相对于页面底端的角度
	int32_t	iOrientation;	// 以十分之一度为单位指定字符基线相对于页面底端的角度
	int32_t	iWeight;		// 指定字体重量。在Windows中，字体重量这个术语用来指代字体的粗细程度。
							//     lfWeight的范围为0到1000，正常情况下的字体重量为400，粗体为700。如果lfWeight为0，则使用默认的字体重量
	bool	bItalic;		// 为TRUE时使用斜体
	bool	bUnderline;	// 为TRUE时给字体添加下划线
	bool	bStrikeOut;	// 为TRUE时给字体添加删除线
	uint8_t nCharSet;		// 指定字符集
	uint8_t nOutPrecision;	// 指定输出精度
	uint8_t nClipPrecision;	// 指定剪辑精度
	uint8_t nQuality;		// 定义输出质量
	uint8_t nPitchAndFamily;// 指定字体的字符间距和族
	wchar_t wcFaceName[FACENAME_MAX];// 字体名称	
public:
	BSFont()
		: iHeight(-20), iWidth(0), iEscapement(0), iOrientation(0), iWeight(400)
		, bItalic(false), bUnderline(false), bStrikeOut(false)
		, nCharSet(0/*ANSI_CHARSET*/), nOutPrecision(0/*OUT_DEFAULT_PRECIS*/), nClipPrecision(0/*CLIP_DEFAULT_PRECIS*/)
		, nQuality(0/*DEFAULT_QUALITY*/), nPitchAndFamily(0/*DEFAULT_PITCH*/)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", L"宋体");
	}
	BSFont(BEFontSize fontSize)
		: iWidth(0), iEscapement(0), iOrientation(0), iWeight(400)
		, bItalic(false), bUnderline(false), bStrikeOut(false)
		, nCharSet(0/*ANSI_CHARSET*/), nOutPrecision(3/*OUT_STROKE_PRECIS*/), nClipPrecision(2/*CLIP_STROKE_PRECIS*/)
		, nQuality(1/*DRAFT_QUALITY*/), nPitchAndFamily(2/*VARIABLE_PITCH*/)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", L"宋体");
		setFontSize(fontSize);
	}

	BSFont(const std::wstring &fontname, BEFontSize fontSize = BEFontSizeFourSmall)
		: iWidth(0), iEscapement(0), iOrientation(0), iWeight(400)
		, bItalic(false), bUnderline(false), bStrikeOut(false)
		, nCharSet(0/*ANSI_CHARSET*/), nOutPrecision(0/*OUT_DEFAULT_PRECIS*/), nClipPrecision(0/*CLIP_DEFAULT_PRECIS*/)
		, nQuality(0/*DEFAULT_QUALITY*/), nPitchAndFamily(0/*DEFAULT_PITCH*/)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", fontname.c_str());
		setFontSize(fontSize);
	}
	BSFont(const BSFont &font)
		: iHeight(font.iHeight), iWidth(font.iWidth), iEscapement(font.iEscapement), iOrientation(font.iOrientation), iWeight(font.iWeight)
		, bItalic(font.bItalic), bUnderline(font.bUnderline), bStrikeOut(font.bStrikeOut)
		, nCharSet(font.nCharSet), nOutPrecision(font.nOutPrecision), nClipPrecision(font.nClipPrecision)
		, nQuality(font.nQuality), nPitchAndFamily(font.nPitchAndFamily)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", font.wcFaceName);
	}
	BSFont& operator=(const BSFont &font)
	{
		if(this != &font)
		{
			iHeight = font.iHeight;
			iWidth = font.iWidth;
			iEscapement = font.iEscapement;
			iOrientation = font.iOrientation;
			iWeight = font.iWeight;
			bItalic = font.bItalic;
			bUnderline = font.bUnderline;
			bStrikeOut = font.bStrikeOut;
			nCharSet = font.nCharSet;
			nOutPrecision = font.nOutPrecision;
			nClipPrecision = font.nClipPrecision;
			nQuality = font.nQuality;
			nPitchAndFamily = font.nPitchAndFamily;
			std::swprintf(wcFaceName, FACENAME_MAX, L"%s", font.wcFaceName);
		}
		return *this;
	}
	~BSFont()
	{
	}
public:
	std::vector<uint8_t> toData() {
		uint8_t *pbuf = (uint8_t*)this;
		std::vector<uint8_t> vec(pbuf, pbuf+sizeof(BSFont));
		return vec;
	}
	bool fromData(std::vector<uint8_t> vecData) {
		if(vecData.size() != sizeof(BSFont))
			return false;
		BSFont font;
		std::memcpy((void*)&font, vecData.data(), sizeof(BSFont));
		*this = font;
		return true;
	}
	void setFontSize(BEFontSize fontSize) {
		switch (fontSize)
		{
		case BEFontSizeZero:
			iHeight = -70;
			break;
		case BEFontSizeZeroSmall:
			iHeight = -60;
			break;
		case BEFontSizeOne:
			iHeight = -43;
			break;
		case BEFontSizeOneSmall:
			iHeight = -40;
			break;
		case BEFontSizeTwo:
			iHeight = -37;
			break;
		case BEFontSizeTwoSmall:
			iHeight = -30;
			break;
		case BEFontSizeThree:
			iHeight = -27;
			break;
		case BEFontSizeThreeSmall:
			iHeight = -25;
			break;
		case BEFontSizeFour:
			iHeight = -23;
			break;
		case BEFontSizeFourSmall:
			iHeight = -20;
			break;
		case BEFontSizeFive:
			iHeight = -18;
			break;
		case BEFontSizeFiveSmall:
			iHeight = -15;
			break;
		case BEFontSizeSix:
			iHeight = -13;
			break;
		case BEFontSizeSixSmall:
			iHeight = -11;
			break;
		case BEFontSizeSeven:
			iHeight = -9;
			break;
		case BEFontSizeEight:
			iHeight = -8;
			break;
		default:
			iHeight = -20;
			break;
		}
	}

#if defined(_AFXDLL)
	void toMSFont(LOGFONT *plgfont, CDC *pdc = nullptr) const {
		if(plgfont){
			plgfont->lfHeight = -iHeight;
			plgfont->lfWidth = iWidth;
			plgfont->lfEscapement = iEscapement;
			plgfont->lfWeight = iWeight;
			plgfont->lfItalic = bItalic;
			plgfont->lfUnderline = bUnderline;
			plgfont->lfStrikeOut = bStrikeOut;
			plgfont->lfCharSet	= nCharSet;
			plgfont->lfOutPrecision = nOutPrecision;
			plgfont->lfClipPrecision = nClipPrecision;
			plgfont->lfQuality = nQuality;
			plgfont->lfPitchAndFamily = nPitchAndFamily;
			std::swprintf(plgfont->lfFaceName, LF_FACESIZE, L"%s", wcFaceName);
#if defined(WORK_AXIS_CAD) || defined(WORK_AXIS_DOC)	// 往右、上递增；1逻辑单位等于0.1毫米
			plgfont->lfHeight *=254/72; 
#else
			if(pdc)
				plgfont->lfHeight = -MulDiv(plgfont->lfHeight, pdc->GetDeviceCaps(LOGPIXELSY), 72);
#endif

		}

	}

#endif
//private:
//	friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        // 注意，保存时总是保存最新版本
//		ar & iHeight;
//		ar & iWidth;
//		ar & iEscapement;
//		ar & iOrientation;
//		ar & iWeight;
//		ar & bItalic;
//		ar & bUnderline;
//		ar & bStrikeOut;
//		ar & nCharSet;
//		ar & nOutPrecision;
//		ar & nClipPrecision;
//		ar & nQuality;
//		ar & nPitchAndFamily;
//		ar & wcFaceName;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//		ar & iHeight;
//		ar & iWidth;
//		ar & iEscapement;
//		ar & iOrientation;
//		ar & iWeight;
//		ar & bItalic;
//		ar & bUnderline;
//		ar & bStrikeOut;
//		ar & nCharSet;
//		ar & nOutPrecision;
//		ar & nClipPrecision;
//		ar & nQuality;
//		ar & nPitchAndFamily;
//		ar & wcFaceName;
//    }
//    BOOST_SERIALIZATION_SPLIT_MEMBER()	


}BSFont;
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- BSBTClassNode	2013-12-12 --- >>
//	定义一个类继承关系的B树。如下：
//		depth:0				classA
//							/    \
//		depth:1			classB	 classC
//						/		  /	  |	\	\
//		depth:2		classD	   classE F  G   H
//						|		|
//		depth:3		classI	   classJ
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct BSBTClassNode {
	std::string						className;
	int32_t							classVersion;
	int32_t							classType;		// 类类型，在整个树中唯一key
	int32_t							classDepth;		// 类继承深度
	BSBTClassNode					*pparentClass;	// 父类
	std::vector<BSBTClassNode *>	vecSubClass;	// 子类
	BSBTClassNode(const char *pname, int32_t type, int32_t depth, BSBTClassNode *pnode, BSBTClassNode *pparentNode)
		: className(pname?pname:""), classType(type), classDepth(depth), pparentClass(pparentNode)
	{
		if(pnode && pparentClass)
			pparentClass->vecSubClass.push_back(pnode);
	}
	//// 公共方法
	//BSBTClassNode *getNodeByDepth(int32_t depth) {
	//	if(depth > classDepth)
	//		return nullptr;
	//	BSBTClassNode *pnode = this;
	//	while(pnode)
	//	{
	//		if(pnode->classDepth == depth)
	//			return pnode;
	//		pnode = pnode->pparentClass;
	//	}
	//	return pnode;
	//}
	///*****************************************************************************************
	//	<< --- static findCommunityBasecalss	2013-12-12 --- >>
	//	说明：查找指定对象组的公共基类类型
	//	参数：
	//	pnode1	= 被比较节点1
	//	pnode2	= 被比较节点2
	//	返回值：公共基对象类型
	//*****************************************************************************************/
	//static BSBTClassNode* findCommunityBasecalss(BSBTClassNode* pnode1, BSBTClassNode *pnode2)
	//{
	//	// 节点是类静态生产的，所以同一个类的节点地址只有一个
	//	if(pnode1 == pnode2)
	//		return pnode1;
	//	if(pnode1 && pnode2)
	//	{
	//		// 定位到同一深度
	//		if(pnode1->classDepth < pnode2->classDepth)
	//			pnode2 = pnode2->getNodeByDepth(pnode1->classDepth);
	//		else if(pnode1->classDepth > pnode2->classDepth)
	//			pnode1 = pnode1->getNodeByDepth(pnode2->classDepth);
	//		while(pnode1 && pnode2)
	//		{
	//			if(pnode1 == pnode2)
	//				return pnode1;
	//			pnode1 = pnode1->pparentClass;
	//			pnode2 = pnode2->pparentClass;
	//		}
	//	}
	//	return nullptr;
	//}
}BSBTClassNode;
// 类定义文件中使用
#define _DECLARE_BTNODE_(class_name) \
public: \
	static BSBTClassNode class##class_name; \
public: \
	virtual BSBTClassNode* getBTClassNode() const { \
		return &class##class_name; \
	} \
	virtual int32_t getType(int32_t depth) const { /*根据深度得到基类的类型*/\
		BSBTClassNode *pnode = &class##class_name; \
		while(pnode) \
		{ \
			if(pnode->classDepth == depth) \
				return pnode->classType; \
			pnode = pnode->pparentClass; \
		} \
		return -1; \
	} \
	virtual int32_t getType() const { \
		return class##class_name.classType; \
	}
// 类实现文件中使用，根类中使用
#define _IMPLEMENT_ROOT_BTNODE_(class_root, class_type) \
	BSBTClassNode class_root::class##class_root = BSBTClassNode(#class_root, class_type, 0, nullptr, nullptr, class_root::allocSharePropertySectionArray); 
// 类实现文件中使用，派生类中使用
#define _IMPLEMENT_BTNODE_(class_name, base_class_name, class_type) \
	BSBTClassNode class_name::class##class_name = BSBTClassNode( \
	#class_name, class_type, base_class_name::class##base_class_name.classDepth+1, \
	&class_name::class##class_name, &base_class_name::class##base_class_name, class_name::allocSharePropertySectionArray); 



} // end gtc


#endif