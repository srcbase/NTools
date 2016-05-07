#ifndef __BASEINC_H__
#define __BASEINC_H__
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



#include "AC/debugger.h"
#if defined(_DEBUG)
#define DEBUG_LOG	gtc::debugger::message
#define WRITE_LOG	gtc::debugger::write
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
//#define SEND_LOG	gtc::debugger::send
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

#include "basedef.h"

#endif	// __BASEINC_H__