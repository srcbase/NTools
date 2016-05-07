#ifndef __BASEDEF_H__
#define __BASEDEF_H__
#pragma once
#pragma warning(disable:4005)

#include <cstdint>
#include <cstddef>
#include <assert.h>
#include <cctype>
#include <cwctype>
#include <cwchar>
#include <cstring>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <memory>
#if defined(_ATL)
	#include <atltypes.h>
	#include <ATLComTime.h>
#endif
#if defined(_AFXDLL)
	#include <afxwin.h>
	#include <afxdisp.h>
	#include <afxext.h>         // MFC 扩展
	#include <afxframewndex.h>
#else
#endif
#if defined(_WIN32) && !defined(_AFXDLL)
	#include <windows.h>
	#include <windowsx.h>
#endif
#if defined(_WIN32)
	#include <process.h>
	#include <comutil.h>
	#include <comdef.h>
	#include <commctrl.h>
	#include <stddef.h>
	#include <tchar.h>
	#include <assert.h>
	#include <crtdbg.h>
	#include <malloc.h>
	#include <olectl.h>
	#include <ShlObj.h>

#endif
#include "debugger.h"
#include "baseEnum.h"
#if defined(_DEBUG)
#define DEBUG_LOG	gtc::debugger::message
#define WRITE_LOG	gtc::debugger::write
//#define SEND_LOG	gtc::debugger::write/*_tick*/
#define SEND_LOG	gtc::debugger::send/*_tick*/
#define SEND_LOGA	gtc::debugger::sendA
#define SEND_LASTERROR()	SEND_LOG(L"ERROR %s 异常捕获，错误码：%u", __FUNCTIONW__, ::GetLastError())

#else
#define DEBUG_LOG	
#define WRITE_LOG	
#define SEND_LOG	
#define SEND_LOGA
#define SEND_LASTERROR()
//
//#define DEBUG_LOG	gtc::debugger::message
//#define WRITE_LOG	gtc::debugger::write
//#define SEND_LOG	gtc::debugger::write
//#define SEND_LOGA	gtc::debugger::sendA
//#define SEND_LASTERROR()	SEND_LOG(L"ERROR %s 异常捕获，错误码：%u", __FUNCTIONW__, ::GetLastError())

//#define DEBUG_LOG	gtc::debugger::write
//#define WRITE_LOG	gtc::debugger::write
//#define SEND_LOG	gtc::debugger::write
//#define SEND_LOGA
//#define SEND_LASTERROR()

#endif
#ifndef UINT_PTR
	typedef unsigned int UINT_PTR;
#endif
#ifndef INT_PTR
	typedef int INT_PTR;
#endif

#ifndef MAX_PATH
	#define MAX_PATH 260
#endif

//////////////////////////////////////////////////////////////////////////
// 基本数据类型定义
// 基于用户使用的SOCKET发送接收数据类型
typedef std::vector<char>		SCDataType;
typedef std::vector<wchar_t>	SWDataType;
// 字节流数据类型，用来存储二进制数据
typedef std::vector<uint8_t>	SBDataType;
// 
typedef std::map<std::wstring, std::wstring>	StringMapType;
typedef std::map<std::string, std::string>		SStringMapType;
typedef std::map<INT_PTR, std::wstring>			IStringMapType;
typedef std::vector<std::wstring>				StringArrayType;
typedef std::vector<std::string>				SStringArrayType;
typedef std::vector<int>						IntArrayType;
typedef std::set<int>							IntSetType;
#if defined(_WIN32)
	typedef std::vector<LPITEMIDLIST>			IIDLArrayType;
#endif
//////////////////////////////////////////////////////////////////////////
//	2015-9-20	
#define RGB_FLIP(color)		(((color) & 0xFF00FF00) | (((color) >> 16) & 0x0FF) | (((color) << 16) & 0x0FF0000))
// 数据库配置文件中的key
#define DBKEY_LOGIN					L"LOGIN"
#define DBKEY_PASSWORD				L"PW"
#define DBKEY_IP					L"IP"
#define DBKEY_NAMEORPATH			L"DB"
#define DBKEY_TYPE					L"TYPE"
// 消息对话框的标题
#define _NDEF_MESSAGEBOX_TITLE		L"信息提示"
// 日志文件的路径
#define _NDEF_DIR_GTDATA			L"kpzm"
#define _NDEF_DIR_GTLOG				L"LOG"

// 查找文件对话框用
static wchar_t gc_wsImageFilter[] = L"图标文件(*.ico)|*.ico|图像文件(*.jpg)|*.jpg|bmp文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||";
// 时间转换用
#define EPOCH_OFFSET 116444736000000000LL	// 1601年1月1日到1970年1月1日的时间差，单位100纳秒
// 定义float型数据的最大最小值
#define FLOAT_MAX	(3000000000.0f)
#define FLOAT_MIN	(-3000000000.0f)
//namespace gtc{
//#if defined(_DEBUG)
//	#define DEBUG_LOG __debug
//#else
//	#define DEBUG_LOG
//#endif
//
//static void __debug(const wchar_t *vpformat, ...) {
//	/*if(vhaveLine)
//		OutputDebugString(_T("------------------------------------------------------\n"));*/
//	wchar_t pinfo[512] = {0};
//	va_list argList;
//	va_start(argList, vpformat);
//	std::vswprintf(pinfo, _countof(pinfo)-sizeof(wchar_t), vpformat, argList);
//	va_end(argList);
//#if defined(_WIN32)
//	std::wcscat(pinfo, L"\n");
//	OutputDebugStringW(pinfo);
//#endif
//
//}
#define TIME_TYPE_YEAR		1900		// 当数据类型为时间类型时对应的默认年，这样方便比较等操作
#define TIME_TYPE_MONTH		1			// 当数据类型为时间类型时对应的默认月，这样方便比较等操作
#define TIME_TYPE_DAY		1			// 当数据类型为时间类型时对应的默认日，这样方便比较等操作


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



static std::wstring toString(BEOrientationFlag orientate)
{
	switch(orientate)
	{
	case BEOrientationFlagNone:				return L"BEOrientationFlagNone";		break;
	case BEOrientationFlagCenter:			return L"BEOrientationFlagCenter";		break;
	case BEOrientationFlagTop:				return L"BEOrientationFlagTop";			break;
	case BEOrientationFlagBottom:			return L"BEOrientationFlagBottom";		break;
	case BEOrientationFlagLeft:				return L"BEOrientationFlagLeft";		break;
	case BEOrientationFlagRight:			return L"BEOrientationFlagRight";		break;
	case BEOrientationFlagLeftTop:			return L"BEOrientationFlagLeftTop";		break;
	case BEOrientationFlagLeftBottom:		return L"BEOrientationFlagLeftBottom";	break;
	case BEOrientationFlagRightTop:			return L"BEOrientationFlagRightTop";	break;
	case BEOrientationFlagRightBottom:		return L"BEOrientationFlagRightBottom";	break;
	default:								return L"未知方向";		break;
	}
	return L"未知方向";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 绘图对象动作发起方标记
//	int32_t			iSender;		// 动作触发者标记
//	BEItemAction	beAction;		// 动作类别
typedef struct BSActionSender {
	BESenderFlag	beSender;		// 动作触发者标记
	BEItemAction	beAction;		// 动作类别
	INT_PTR			itemObject;		// 正在改变的项
	BSActionSender(BESenderFlag sender, BEItemAction action, INT_PTR itObj = 0)
		: beSender(sender), beAction(action), itemObject(itObj)
	{}
}BSActionSender;

///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-5 字体定义	
struct BSFontInfo
{
	std::wstring wsFontName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;

	BSFontInfo()
		: iSize(12), bBold(false), bUnderline(false), bItalic(false)
	{}
	BSFontInfo(PCWSTR pfontName, int fontSize = 12) 
		: wsFontName(pfontName?pfontName:L""), iSize(fontSize)
		, bBold(false), bUnderline(false), bItalic(false)
	{}

};
struct BSFontInfoUC : public BSFontInfo
{
	HFONT hFont;
	//std::wstring wsFontName;
	//int iSize;
	//bool bBold;
	//bool bUnderline;
	//bool bItalic;
	TEXTMETRIC tm;

	BSFontInfoUC()
		: BSFontInfo(), hFont(NULL)
	{
		memset(&tm, 0, sizeof(tm));
	}
	~BSFontInfoUC()
	{
		SEND_LOG(L"BSFontInfoUC free ...");
	}
};
//////////////////////////////////////////////////////////////////////////
// 文件属性信息
typedef struct BSFileAttribute
{
	uint32_t		uiAttribute;	// 属性
	std::time_t		dtCreate;		// 创建日期
	std::time_t		dtLastAccess;	// 
	std::time_t		dtLastWrite;	// 修改日期
	int64_t			llSize;			// 大小
}BSFileAttribute;

#define FILE_ATTRIBUTE_CREATE_TIME			0x0001		// 文件属性开关标记，创建时间
#define FILE_ATTRIBUTE_LASTACCESS_TIME		0x0002		// 文件属性开关标记，最后访问时间
#define FILE_ATTRIBUTE_LASTWRITE_TIME		0x0004		// 文件属性开关标记，最后写时间
#define FILE_ATTRIBUTE_ALL_TIME				(FILE_ATTRIBUTE_CREATE_TIME|FILE_ATTRIBUTE_LASTACCESS_TIME|FILE_ATTRIBUTE_LASTWRITE_TIME)

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

// 绘制文本的格式
// 单行、垂直水平居中，超出区域部分用3点省略号代替
#define DRAWTEXT_FLAG_TITLE		(DT_CENTER|DT_END_ELLIPSIS|DT_NOPREFIX|DT_EDITCONTROL|DT_VCENTER|DT_SINGLELINE)
// 文本顶端居中绘制，单词超过绘制区域的宽度时自动换行(单词分割开换行)，当超出底边区域时用3点省略号代替超出部分
#define DRAWTEXT_FLAG_EDIT		(DT_CENTER|DT_MODIFYSTRING|DT_END_ELLIPSIS|DT_NOPREFIX|DT_EDITCONTROL|DT_WORDBREAK)
// 类似桌面节点文本绘制样式；文本顶端居中绘制，单词超过绘制区域的宽度时用3点省略号代替超出部分(单词不分割)，当超出底边区域时用3点省略号代替超出部分
#define DRAWTEXT_FLAG_NODE		(DT_CENTER|DT_MODIFYSTRING|DT_END_ELLIPSIS|DT_NOPREFIX|DT_EDITCONTROL|DT_WORDBREAK|DT_WORD_ELLIPSIS|DT_NOFULLWIDTHCHARBREAK)
#define DRAWTEXT_FLAG_LIST		(DT_LEFT  |DT_MODIFYSTRING|DT_END_ELLIPSIS|DT_NOPREFIX|DT_EDITCONTROL|DT_VCENTER|DT_SINGLELINE)
// 文本全先生，自动换行
#define DRAWTEXT_FLAG_SHOWALL	(DT_CENTER|DT_NOPREFIX|DT_EDITCONTROL|DT_WORDBREAK)
//#define DRAWTEXT_FLAG_SHOWALL	(DT_CENTER|DT_NOPREFIX|DT_EDITCONTROL|DT_WORDBREAK)
//DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX | DT_EDITCONTROL

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

/*****************************************************************************************
	<< --- BERelationalOptor		2013-11-22 --- >>
	说明：将枚举值转换成对应的字符描述
	参数：
	relational = 被转换的枚举值
	返回值：返回枚举型的字符描述
*****************************************************************************************/
static std::wstring toString(BERelationalOptor relational)
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

static std::wostream& operator<<(std::wostream &wos, const std::wstring &wstr)
{
	return wos << wstr.c_str();
}

static UINT keyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
	return uState;
}

/*****************************************************************************************
	<< ---  global::operator==RECT		2014-06-25 --- >>
	说明：判断两个区域坐标是否一致
	参数：
	rt1		= 被比较的区域1
	rt2		= 被比较的区域2
	返回值：是否一致
*****************************************************************************************/
static bool operator==(const RECT &rt1, const RECT &rt2)
{
	return rt1.left == rt2.left && rt1.top == rt2.top && rt1.right == rt2.right && rt1.bottom == rt2.bottom;
}

static bool operator!=(const RECT &rt1, const RECT &rt2)
{
	return !(rt1 == rt2);
}

static bool isEmpty(const RECT &rt)
{
	return rt.left == rt.right && rt.top == rt.bottom;
}

static bool rcInRect(const RECT &rtIn, const RECT &rt)
{
	POINT pt1 = {rt.left,rt.top};
	POINT pt2 = {rt.right,rt.bottom};
	return ::PtInRect(&rtIn, pt1) && ::PtInRect(&rtIn, pt2);
}

static int rectWidth(const RECT &rt)
{
	return rt.right-rt.left;
}

static int rectHeight(const RECT &rt)
{
	return rt.bottom - rt.top;
}


static SIZE rectSize(const RECT &rt)
{
	SIZE sz = {rt.right-rt.left, rt.bottom-rt.top};
	return sz;
}

static RECT rectJoin(const RECT &rt1, const RECT &rt2)
{
	RECT rt = {0};
	rt.left = min(rt1.left, rt2.left);
	rt.top = min(rt1.top, rt2.top);
	rt.right = max(rt1.right, rt2.right);
	rt.bottom = max(rt1.bottom, rt2.bottom);
	return rt;
}
static SIZE operator+(const SIZE &sz1, const SIZE &sz2)
{
	SIZE sz = {sz1.cx + sz2.cx, sz1.cy + sz2.cy};
	return sz;
}

/*****************************************************************************************
	<< ---  global::operator==POINT		2014-06-25 --- >>
	说明：判断两个点坐标是否一致
	参数：
	pt1		= 被比较的点1
	pt2		= 被比较的点2
	返回值：是否一致
*****************************************************************************************/
static bool operator==(const POINT &pt1, const POINT &pt2)
{
	return pt1.x == pt2.x && pt1.y == pt2.y;
}
static bool operator!=(const POINT &pt1, const POINT &pt2)
{
	return !(pt1==pt2);
}

static bool operator==(const SIZE &sz1, const SIZE &sz2)
{
	return sz1.cx == sz2.cx && sz1.cy == sz2.cy;
}
static bool operator!=(const SIZE &sz1, const SIZE &sz2)
{
	return !(sz1 == sz2);
}

/*****************************************************************************************
	<< ---  global::calculateAreaCell		2014-08-01 --- >>
	说明：将区域块按指定单元格大小进行分割，单元格间的空隙为spaceXY，且区域四边没有空隙；解析的顺序为从左到右，从上到下
	参数：
	szArea		= 被分割的区域1
	szCell		= 单元格大小
	spaceXY		= 单元格间的空隙
	返回值：单元格坐标集合
*****************************************************************************************/
static std::vector<RECT> calculateAreaCell(SIZE szArea, SIZE szCell, int spaceXY = 0)
{
	std::vector<RECT> vecCell;
	int rows = (szArea.cy + spaceXY)/(szCell.cy + spaceXY);
	int cols = (szArea.cx + spaceXY)/(szCell.cx + spaceXY);
	RECT rc = {0};
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			rc.left = col * (szCell.cx + spaceXY);
			rc.top = row * (szCell.cy + spaceXY);
			rc.right = rc.left + szCell.cx;
			rc.bottom = rc.top + szCell.cy;
			vecCell.push_back(rc);
		}
	}
	return vecCell;
}
/****************************************************************************************
	<< --- sliceAreaCell		2015-7-27 --- >>         
	说明：根据位置索引得到其所在区域。位置索引，isHorizontal=true时：水平从左到右，从上到下，如：	0 1 2
																					3 4 5
																					6 7 8
										isHorizontal=false时：从上到下，水平从左到右，如：0 3 6
																					1 4 7
																					2 5 8
	参数：
		posIndex		= 要定位的位置索引
		szCell			= 定位单元格大小
		isHorizontal	= 索引计数方向
		fixCount		= isHorizontal=true表示固定的列数，isHorizontal=false表示固定的行数
	返回值：
*****************************************************************************************/
static RECT sliceAreaCell(int posIndex, SIZE szCell, bool isHorizontal = true, int fixCount = 3)
{
	assert(posIndex >= 0);
	int col, row;
	if (isHorizontal)
	{
		col = posIndex % fixCount;
		row = posIndex / fixCount;
	}
	else
	{
		col = posIndex / fixCount;
		row = posIndex % fixCount;
	}
	RECT rc = {0};
	rc.left = col * szCell.cx;
	rc.right = rc.left + szCell.cx;
	rc.top = row * szCell.cy;
	rc.bottom = rc.top + szCell.cy;
	return rc;
}
static RECT sliceAreaCell(int posIndex, int cxcy, bool isHorizontal = true, int fixCount = 3)
{
	SIZE sz = {cxcy, cxcy};
	return sliceAreaCell(posIndex, sz, isHorizontal, fixCount);
}
static std::wstring sliceAreaCellString(int posIndex, SIZE szCell, bool isHorizontal = true, int fixCount = 3)
{
	RECT rc = sliceAreaCell(posIndex, szCell, isHorizontal, fixCount);
	wchar_t pbuf[80] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
	return pbuf;
}
static std::wstring sliceAreaCellString(int posIndex, int cxcy, bool isHorizontal = true, int fixCount = 3)
{
	SIZE sz = {cxcy, cxcy};
	return sliceAreaCellString(posIndex, sz, isHorizontal, fixCount);
}
namespace gtc{

	// 公共函数静态库
	static std::wostream& outClassBegin(std::wostream &os, const std::wstring &cls, int indent)
	{
		os << std::setw(indent) << std::left << L"" << cls.c_str() << L"\n";
		os << std::setw(indent) << L"" << std::setw(2) << L"[" << L"\n";
		return os;
	}
	static std::wostream& outClassEnd(std::wostream &os, const std::wstring &cls, int indent)
	{
		os << std::setw(indent) << std::left << L"" << std::setw(2) << L"]" << cls.c_str() << L"\n";
		return os;
	}

	template<class Value>
	static std::wostream& outClassItem(std::wostream &os, const std::wstring &title, Value &val, int indent)
	{
		os << std::right << std::setw(indent+10) /*<< " *" << std::setw(16)*/ << title.c_str() /* << std::setw(16)*/ << "  >  " << val << std::endl;// "\n";
		return os;
	}
	template<class Type>
	static std::wostream& outBaseClass(std::wostream &os, Type &ty, int indent)
	{
		os << std::setw(indent) << std::left << L"" << L"base class:" << "\n" << ty.description(indent);
		return os;
	}

}
//}
#endif