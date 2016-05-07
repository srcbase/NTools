#ifndef __SYSUPPORT_H__
#define __SYSUPPORT_H__
#pragma once
#include <fstream>
#include <cstdarg>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cctype>
#include <cwctype>
#include <algorithm>
#include "AC/basedef.h"		// 包含 debugger.h
#include "AC/singleton.h"
#include "AC/baseException.h"
#include "AC/stringUtil.h"
#include "AC/converter.h"
#include "AC/datetime.h"
//#include "AC/debugger.h"
#if defined(BOOST_SERIALIZE)
#include "../boost/serialization/export.hpp"
#include "../boost/serialization/base_object.hpp"
#include "../boost/serialization/string.hpp"
#include "../boost/serialization/vector.hpp"
#include "../boost/serialization/map.hpp"
#endif


#if defined(_WIN32)
	#include <comutil.h>
	#include <Shellapi.h>     // Included for shell constants such as FO_* values
	#include <ShlObj.h>
	#include <stgprop.h>
	#include <Shlwapi.h>
	#pragma comment( lib, "shlwapi.lib") 
	#include <dbghelp.h>
	#pragma comment(lib, "dbghelp.lib")
#endif
#if defined(_AFXDLL )
	#include <afxMDIFrameWndEx.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// 2015-12-18		kp007	网卡信息
struct BSAdapterInfo
{
	std::wstring	wsIP;
	std::wstring	wsMAC;

	BSAdapterInfo(std::wstring ip, std::wstring mac)
		: wsIP(ip), wsMAC(mac)
	{}
};
typedef std::vector<BSAdapterInfo> AdapterInfoArrayType;

namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	format类	2014-06-27
//	格式化类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class format
{
public:
	/*****************************************************************************************
		<< --- format::rect		2014-06-27 --- >>
		说明：将RECT转化成“left,top,right,bottom width,height”形式
	*****************************************************************************************/
	static std::wstring rect(const RECT &rc, bool showSize = false);
	static RECT rect(PCWSTR prect);
	/*****************************************************************************************
		<< --- format::point		2014-06-27 --- >>
		说明：将POINT转化成“x,y”形式
	*****************************************************************************************/
	static std::wstring point(const POINT &pt);
	static POINT point(PCWSTR ppoint);
	/*****************************************************************************************
		<< --- format::size		2014-06-27 --- >>
		说明：将SIZE转化成“cx,cy”形式
	*****************************************************************************************/
	static std::wstring size(const SIZE &sz);

	/****************************************************************************************
		<< --- 	color	2015-8-19 --- >>         
		说明：颜色转换，格式:#HHHHHHHH
		参数：
		返回值：
	*****************************************************************************************/
	static std::wstring color(DWORD clr);
	static DWORD color(PCWSTR pclr);
	/*****************************************************************************************
		<< --  floatString		2015-1-4 --- >>
		说明：格式化输出浮点数
		参数：
			fltValue	= 被格式化的浮点值
			decimals	= 保留的小数点数
		返回值：格式化后的字符串
	*****************************************************************************************/
	static std::wstring floatString(float fltValue, int32_t decimals = 2);

};





////////////////////////////////////////////////////////////////////////////////////////////////////////
//	app类	2014-04-25
//	常用应用操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class app
{
public:
	static std::wstring mc_wsLogPath;	// 日志文件路径
	static std::wstring mc_wsIniPath;	// 配置文件路径
	static std::wstring mc_wsIniName;	// 配置文件名称
	static std::wstring mc_wsLogFileName;	// 日志文件名的开头部分
	static std::wstring mc_wsDumpFileName;	// dump文件的开头部分

	static std::wstring getConfigPath() {
		return mc_wsIniPath;
	}
	static std::wstring getLogPath() {
		return mc_wsLogPath;
	}
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// 2015-12-18		kp007	的到网卡信息
	static AdapterInfoArrayType getAdapterInfo(PCWSTR pmacSeparator = NULL);
	// 返回桌面大小，bDeskTaskbar：是否包含桌面任务栏
	static RECT getDeskArea(bool containTaskbar = true);
	/****************************************************************************************
		<< --- addFilterWindowMessage		2015-6-9 --- >>         
		说明：添加需要取消拦截的消息。拦截通常发生在“低权限程序往高权限进程发送自定义消息”
		参数：
			hWnd	= 接收消息的窗口
			uMsg	= 自定义消息
		返回值：是否执行成功
	*****************************************************************************************/
	static bool addFilterWindowMessage(HWND hWnd, UINT uMsg);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-11-4 捕获窗口，并使其得到鼠标焦点	
	static void captureWindow(HWND hWnd);
	/****************************************************************************************
		<< --- exec		2015-6-9 --- >>         
		说明：启动可执行文件
		参数：
			pexeFile	= 可执行文件全路径
			pParameter	= 运行参数
			showFlag	= 显示标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool exec(PCWSTR pexeFile, PCWSTR pParameter = NULL, int showFlag = SW_SHOW);
	/****************************************************************************************
		<< --- 	getProductVersion	2015-5-14 --- >>         
		说明：得到程序的版本号
		参数：
			pFileName	= 要得到版本信息的文件
			poutFileVersion	= 不为空时返回文件版本号
		返回值：= 0 失败，否则返回产品版本号
	*****************************************************************************************/
	static uint64_t getProductVersion(PCWSTR pFileName = NULL, uint64_t *poutFileVersion = NULL);
	/****************************************************************************************
		<< --- 	expandEnvironmentString	2015-2-15 --- >>         
		说明：扩展带有环境变量的字符创，如："%ProgramFiles%"-->"C:\Program Files"
		参数：
			pSrc = 数据源
		返回值：扩展后的字符串
	*****************************************************************************************/
	static std::wstring expandEnvironmentString(PCWSTR pSrc);
	/*****************************************************************************************
		<< --- static app::getOSVersion		2014-04-25 --- >>
		说明：得到操作系统的版本号
		参数：
		posVersion		= 操作系统版本结构
		返回值：操作系统的版本号
	*****************************************************************************************/
	static BEOSVersion getOSVersion(OSVERSIONINFOEX *posVersion = NULL);
	static bool isWindowsXP();
	static bool isWindowsVistaLater();
	static bool isWindows7Later();
	static bool isWindows8Later();
	static bool isWindowsOS64();
	/****************************************************************************************
		<< --- standardisePath		2015-1-15 --- >>         
		说明：将路径转换成标准格式，如：c:/folder1/folder2/
		参数：
			initPath	= 原路径
		返回值：转换后的标准路径
	*****************************************************************************************/
	static std::wstring standardisePath(const std::wstring &initPath)
	{
		std::wstring path = initPath;

		std::replace(path.begin(), path.end(), L'\\', L'/');
		if(path.back() != L'/')
			path += L'/';

		return path;
	}
	/*****************************************************************************************
		<< --- static app::message		2014-04-25 --- >>
		说明：浏览选择文件
		参数：
		vpinfo		= 要显示的消息
		vptitle		= 消息框的标题
		vtype		= 消息框中命令按钮类型
		viswriter	= 是否将消息写入日志文件
		返回值：选中的命令按钮标记
	*****************************************************************************************/
	static std::wstring getAppPath(void)
	{
		wchar_t str[MAX_PATH];
		std::wstring path;
//#if defined(_UNICODE)
//		path = _wpgmptr;
//#else
//		path = convert::toWChar(_pgmptr);
//#endif
		uint32_t len = 0;
		// 得到系统参数
	#if defined(_WIN32)
		len = ::GetModuleFileNameW(nullptr, str, MAX_PATH);
		str[len] = '\0';
	#endif
		path = str;
		path = path.substr(0, path.rfind('\\'));//.Left(path.ReverseFind('\\'));
		return path;
	}
#if defined(_AFXDLL)
	/*****************************************************************************************
		<< --- CAApp::getMainWnd	2014-06-14 --- >>
		说明：得到应用程序主框架
		参数：
		返回值：主框架CWnd*
	*****************************************************************************************/
	static CWnd* getMainWnd()
	{
		CWinApp* app = ::AfxGetApp();
		if(app)
			return app->GetMainWnd();
		return nullptr;
	}
	static CView* getActiveView()
	{
		CWnd *pWnd = getMainWnd();
		
		if(CMDIFrameWnd *pMDIWnd = dynamic_cast<CMDIFrameWnd *>(pWnd))
		{
			if(pMDIWnd->MDIGetActive()) return pMDIWnd->MDIGetActive()->GetActiveView();
			return NULL;
		}
		else if(CFrameWnd *pframe = dynamic_cast<CFrameWnd *>(pWnd))
			return pframe->GetActiveView();
		assert(!"没有活动的视图！");
		return NULL;
	}
#endif
	/*****************************************************************************************
		<< --- static app::init		2014-04-26 --- >>
		说明：初始化系统环境
		1. 创建目录\\GTDATA\\LOG
		参数：
		isAppPath	=	true  应用程序路径
						false 系统登录用户Application Data下
		返回值：
	*****************************************************************************************/
	static void init(PCWSTR pinifile = L"kpzm.ini", PCWSTR plogfileName = L"app", PCWSTR pdumpfileName = L"du", PCWSTR pfolder = _NDEF_DIR_GTDATA, bool isAppPath = false)
	{
		std::setlocale(LC_ALL, "");

		mc_wsIniName = pinifile ? pinifile : L"";
		mc_wsLogFileName = plogfileName ? plogfileName : L"";
		mc_wsDumpFileName = pdumpfileName ? pdumpfileName : L"";
		if(isAppPath)
			mc_wsLogPath = getAppPath();
		else
		{
#if defined(_WIN32)
			wchar_t path[MAX_PATH] = {0};
			if(SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path)))
			{
				mc_wsLogPath = path;
			}
#else
#endif
		}
#if defined(_WIN32)
		mc_wsLogPath += L"\\";
		mc_wsLogPath += pfolder;
		if(!::PathIsDirectoryW(mc_wsLogPath.c_str()) /*file::existsDirectory(mc_wsLogPath.c_str())*/)
			::CreateDirectoryW(mc_wsLogPath.c_str(), nullptr);
		mc_wsIniPath = mc_wsLogPath;	// 设定配置文件路径
		mc_wsLogPath += L"\\";
		mc_wsLogPath += _NDEF_DIR_GTLOG;
		//MessageBoxW(NULL, mc_wsLogPath.c_str(), L"path", 0);
		if(!::PathIsDirectoryW(mc_wsLogPath.c_str())/*file::existsDirectory(mc_wsLogPath.c_str())*/)
			::CreateDirectoryW(mc_wsLogPath.c_str(), nullptr);
#else
#endif
	}

	static void sendtoUser(const std::wstring &wsLog, PCWSTR pWindowName, PCWSTR pClassName = NULL)
	{
		static HWND hwnd = NULL;
		if(!hwnd) hwnd = FindWindowW(pClassName, pWindowName);
		if (hwnd != NULL && IsWindow(hwnd))
		{
			COPYDATASTRUCT stCopy = {0};
			// stCopy.lpData = (void *)strLog.c_str();
			stCopy.cbData = wsLog.length() * sizeof(wchar_t);
			stCopy.lpData = (void *)wsLog.c_str();

			SendMessage(hwnd, WM_COPYDATA,(WPARAM)NULL, (LPARAM)&stCopy);
			//OutputDebugString(wsLog.c_str());
		}
	}
	static void sendtoUser(BELogType logType, const std::wstring &wsLog, PCWSTR pWindowName, PCWSTR pClassName = NULL)
	{
		static HWND hwnd = NULL;
		if(!hwnd) hwnd = FindWindowW(pClassName, pWindowName);
		if (hwnd != NULL && IsWindow(hwnd))
		{
			//BSLogData logdata = {logType};
			//std::wcsncpy(logdata.pLogDesc, wsLog.c_str(), _countof(logdata.pLogDesc));
			COPYDATASTRUCT stCopy = {0};
			stCopy.dwData = (ULONG_PTR(DBGER_LOG_TYPE) << 16) | ULONG_PTR(logType);
			stCopy.cbData = (wsLog.length()+1)*sizeof(wchar_t);
			stCopy.lpData = (void *)wsLog.c_str();

			SendMessage(hwnd, WM_COPYDATA,(WPARAM)NULL, (LPARAM)&stCopy);
			//OutputDebugString(wsLog.c_str());
		}
		//else
		//{
		//	writeLogs(wsLog.c_str());
		//}
	}

	/*****************************************************************************************
		<< --- static app::message		2014-04-25 --- >>
		说明：浏览选择文件
		参数：
		vpinfo		= 要显示的消息
		vptitle		= 消息框的标题
		vtype		= 消息框中命令按钮类型
		viswriter	= 是否将消息写入日志文件
		返回值：选中的命令按钮标记
	*****************************************************************************************/
	static int message(const wchar_t *vpinfo, const wchar_t *vptitle = nullptr, uint32_t vtype = 0/*MB_OK*/, bool viswriter = false) {
		int ret = -1;
	#if defined(_WIN32)
		if(vptitle != NULL)
			ret = ::MessageBoxW(NULL, vpinfo, vptitle, MB_ICONINFORMATION|vtype);
		else
			ret = ::MessageBoxW(NULL, vpinfo, _NDEF_MESSAGEBOX_TITLE, MB_ICONINFORMATION|vtype);
	#else
	#endif
		if(viswriter)
		{
			writeLogs(vpinfo, vptitle);
		}

		return ret;
	}
	static void messageEx(const wchar_t *vpformat, ...) {
		static const int len = 4096;
		static wchar_t pinfo[len] = {0};
		va_list argList;
		va_start(argList, vpformat);
		std::vswprintf(pinfo, len, vpformat, argList);
		va_end(argList);
	#if defined(_WIN32)
		message(pinfo);
	#else
	#endif
	}

	/*****************************************************************************************
		<< --- static app::writeLogs		2014-04-25 --- >>
		说明：浏览选择文件
		参数：
		pinfo	= 被写的信息
		ptitle	= 被写信息的标题分类
		showmsg	= 是否显示消息
		返回值：
	*****************************************************************************************/
	static void writeLogs(const wchar_t *pinfo, const wchar_t *ptitle = NULL, bool showmsg = false)
	{
		if(pinfo == NULL || std::wcslen(pinfo) <= 0)
			return;
		const int len = 4096;
		wchar_t str[len] = {0};
		std::wstring path(mc_wsLogPath.empty()?getAppPath():mc_wsLogPath);
		char stimer[40] = {0};
		char logname[40] = {0};
		std::time_t tm = {0};
		std::time(&tm);
		std::tm *ptminfo = std::localtime(&tm);
		assert(ptminfo);
		std::strftime(stimer, sizeof(stimer), "%Y-%m-%d %H:%M:%S", ptminfo);
		std::strftime(logname, sizeof(logname), "%Y%m%d.ino", ptminfo);
		//path += L"\\";
		//path += _NDEF_DIR_GTDATA;
		//path += L"\\";
		//path += _NDEF_DIR_GTLOG;
		//path += L"\\";
		//path = mc_wsLogPath;
		path += L"\\";
		path += mc_wsLogFileName;
		path += convert::toWChar(logname);
		// 写文件
		if(ptitle != NULL)
			std::swprintf(str, len, L"%s\t %s\t%s  ", convert::toWChar(stimer).c_str(), ptitle, pinfo);
		else
			std::swprintf(str, len, L"%s\t %s  ", convert::toWChar(stimer).c_str(), pinfo);
		
		//file::writeLine(path.c_str(), str);
		std::wofstream file(path.c_str(), std::wofstream::app | std::wofstream::out);
		if(file)
		{
			// 支持中文输出
			file.imbue(std::locale("", std::locale::all ^ std::locale::numeric));
			file << str << std::endl;
			file.close();
		}
		// 
		if(showmsg)
			message(str, ptitle);

	}
	static void writeLogs(const char *pinfo, const char *ptitle = NULL, bool showmsg = false)
	{
		std::wstring wsinfo = convert::toWChar(pinfo);
		std::wstring title = convert::toWChar(ptitle);
		//std::wstring nameflag = convert::toWChar(pnameflag);
		writeLogs(wsinfo.c_str(), title.c_str(), showmsg);
	}
	static void writeVSLogs(const wchar_t *vpformat, ...) {
		const int len = 4096;
		wchar_t pinfo[len] = {0};
		va_list argList;
		va_start(argList, vpformat);
		std::vswprintf(pinfo, len, vpformat, argList);
		va_end(argList);
		// 
		writeLogs(pinfo);
	}
	/*****************************************************************************************
		<< --- static app::writeProfile 	2014-06-05 --- >>
		说明：写配置文件信息
		参数：
		psection	= 段名称
		pkey		= 键名称
		pValue		= 键值
		pfileName	= 配置文件名称
		返回值：是否执行成功
	*****************************************************************************************/
	static bool writeProfile(PCWSTR psection, PCWSTR pkey, PCWSTR pValue, PCWSTR pfileName = nullptr);
	static bool writeProfile(PCWSTR psection, PCWSTR pkey, int32_t iValue, PCWSTR pfileName = nullptr);
	static bool writeProfile(PCWSTR psection, PCWSTR pkey, const POINT &pt, PCWSTR pfileName = nullptr);
	static bool writeProfile(PCWSTR psection, PCWSTR pkey, const RECT &rc, PCWSTR pfileName = nullptr);
	static bool writeProfile(PCWSTR psection, PCWSTR pkey, datetime &dt, PCWSTR pfileName = nullptr);
	/*****************************************************************************************
		<< --- static app::readProfile 	2014-06-05 --- >>
		说明：读取配置文件信息
		参数：
		psection	= 段名称
		pkey		= 键名称
		pValue		= 键值
		pfileName	= 配置文件名称
		返回值：是否执行成功
	*****************************************************************************************/
	static bool readProfile(PCWSTR psection, PCWSTR pkey, std::wstring &refoutData, PCWSTR pfileName = nullptr);
	static bool readProfileLarge(PCWSTR psection, PCWSTR pkey, std::wstring &refoutData, PCWSTR pfileName = nullptr);
	static bool readProfileInt(PCWSTR psection, PCWSTR pkey, int32_t &refoutData, PCWSTR pfileName = nullptr, int32_t dftValue = 0);
	static bool readProfilePoint(PCWSTR psection, PCWSTR pkey, POINT &refoutData, PCWSTR pfileName = nullptr);
	static bool readProfileRect(PCWSTR psection, PCWSTR pkey, RECT &refoutData, PCWSTR pfileName = nullptr);
	static bool readProfileDatetime(PCWSTR psection, PCWSTR pkey, datetime &refoutData, PCWSTR pfileName = nullptr);
	/*****************************************************************************************
		<< --- static app::lastError 	2014-05-30 --- >>
		说明：得到最后错误信息描述
		参数：
		isMsgbox	= 是否已弹出消息框方式显示错误信息
		isWrite		= 是否将错误信息写入日志文件
		返回值：
	*****************************************************************************************/
	static void lastError(bool isMsgbox = false, bool isWrite = true)
	{
		// 必须有一项为真
		assert(isWrite || isMsgbox);
#if defined(_WIN32)
		DWORD err = ::GetLastError();

		HLOCAL hlocal = NULL;   // Buffer that gets the error message string

		// Use the default system locale since we look for Windows messages.
		// Note: this MAKELANGID combination has 0 as value
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

		// Get the error code's textual description
		BOOL fOk = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER, 
			NULL, err, systemLocale, 
			(PWSTR) &hlocal, 0, NULL);

		if (!fOk) {
			// Is it a network-related error?
			HMODULE hDll = LoadLibraryExW(L"netmsg.dll", NULL, 
				DONT_RESOLVE_DLL_REFERENCES);

			if (hDll != NULL) {
				fOk = FormatMessageW(
					FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_ALLOCATE_BUFFER,
					hDll, err, systemLocale,
					(PWSTR) &hlocal, 0, NULL);
				FreeLibrary(hDll);
			}
		}
		if (fOk && hlocal)
		{
			if(isWrite)
				writeLogs((PCWSTR)LocalLock(hlocal), L"LAST_ERROR");
			if(isMsgbox)
			{
				wchar_t buf[40];
				std::swprintf(buf, _countof(buf), L"信息提示(%u)", err);
				message((PCWSTR)LocalLock(hlocal), buf);
			}
			LocalFree(hlocal);
		}
#endif
	}
#if defined(_WIN32)
	/****************************************************************************************
		<< --- sehFilterDefault		2014-12-3 --- >>         
		说明：
		参数：
		返回值：
	*****************************************************************************************/
	static int sehFilterDefault(LPEXCEPTION_POINTERS pExcept, PCWSTR pdesc, bool createDumpFile = true);
	/****************************************************************************************
		<< --- sehUnhandleFilterDump		2014-12-9 --- >>         
		说明：
		参数：
		返回值：
	*****************************************************************************************/
	static LONG WINAPI sehUnhandleFilterDump(LPEXCEPTION_POINTERS pExcept);
#endif
	/*****************************************************************************************
		<< --- static app::createGUID 	2014-06-09 --- >>
		说明：创建字符型GUID
		参数：
		返回值：新的guid
	*****************************************************************************************/
	static std::wstring createGUID() {
		wchar_t buf[64] = {0};
#if defined(_WIN32)
		GUID guid = {0};
		if (SUCCEEDED(::CoCreateGuid(&guid)))
		{
			std::swprintf(buf, _countof(buf), L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
				guid.Data1,
				guid.Data2,
				guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
		}
#endif
		return buf;
	}
	/****************************************************************************************
		<< --- 	createGlobalHandle	2015-3-6 --- >>         
		说明：创建HGLOBAL
		参数：
			ptr		= 数据源；ptr = NULL时只分配空间
			size	= 数据源大小，单位：字节
			nFlag	= 内存分配标记
		返回值：
			!= NULL 成功
			= NLL 失败
	*****************************************************************************************/
	static HGLOBAL createGlobalHandle(const void* ptr, int size, UINT nFlag = GMEM_SHARE|GMEM_MOVEABLE);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-22 复制HGLOBAL，成功时返回hDest，否则返回NULL	
	static HGLOBAL copyGlobalMemory(HGLOBAL hDest, HGLOBAL hSource);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-10-20 设定剪切板数据
	static void setClipboard(const std::wstring &wstr);
	static void setClipboard(const std::string &str);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	thread  类		2015-3-18
//	线程操作的相关封装
////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define THREAD_INFO_MASK_AUTOFREE		0x0001		// _BSThreadInfo::bAutofree 是否有效
//#define THREAD_INFO_MASK_LOOPLOCK		0x0002		// _BSThreadInfo::plLooplock 是否有效
//#define THREAD_INFO_MASK_EVENT			0x0004		// _BSThreadInfo::hEvent 是否有效
class thread : public gtc::singleton<thread>
{
private:
	thread(const thread &);
	thread& operator=(const thread &);
public:
	thread();
	~thread();
public:
	// gtc::singleton
	static thread& getSingleton();
	static thread* getSingletonPtr();
public:
	struct _BSThreadInfo {
		HANDLE			hThread;				// 线程句柄
		void			*pParameter;			// 线程参数
		bool			bAutofree;				// 是否自动释放，=true时，退出线程时释放本节点，通过free()函数
												// 一般内部只执行简单的操作时该变量设定为“true”
		volatile LONG	*plLooplock;			// 线程内部循环条件开关 =1继续循环 =0退出循环
		HANDLE			hEvent;					// 满足条件后触发事件
		wchar_t			pDescription[MAX_PATH];	// 描述信息
		unsigned int (WINAPI *pthreadFunc)(_BSThreadInfo *);	// 线程函数地址，可以用来作同一时刻只有一个线程的区分条件
	};
	typedef std::vector<_BSThreadInfo *> PtrArrayType;
	typedef unsigned int (WINAPI *PTHREAD_START_ADDRESS)(gtc::thread::_BSThreadInfo *);
public:
	/****************************************************************************************
		<< --- 	start	2015-3-18 --- >>         
		说明：启动一个线程
		参数：
			pstartAddress	= 线程地址
			pParam			= 线程参数
			onlyOne			= 同一时刻同一线程地址是否只能启动一个线程
			isAutofree		= 是否在线程退出时自动是否参数，并关闭线程句柄
			pLooplock		= 线程内部循环条件
			pDesc			= 描述信息
		返回值：
			!= NULL 启动的线程句柄
			= NULL	启动失败
	*****************************************************************************************/
	static HANDLE start(PTHREAD_START_ADDRESS pstartAddress, void *pParam, bool onlyOne = false, bool isAutofree = true, volatile LONG *pLooplock = NULL, HANDLE hEvent = NULL, PCWSTR pDesc = NULL);

	/****************************************************************************************
		<< --- 	free	2015-3-18 --- >>         
		说明：释放线程参数信息，一般在线程退出后最后一步执行本函数。
		参数：
			pThreadInfo	= 线程参数信息
		返回值：
	*****************************************************************************************/
	static void free(_BSThreadInfo *pThreadInfo);

	/****************************************************************************************
		<< --- 	waitfor	2015-3-25 --- >>         
		说明：等待线程退出
		参数：
			hThread		= 要退出的线程句柄
			timeout		= 等待耗时
		返回值：
	*****************************************************************************************/
	static DWORD waitfor(HANDLE hThread, DWORD timeout = INFINITE);
private:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-25 查验指定线程函数的线程是否已经存在
	static bool exists(PTHREAD_START_ADDRESS pstartAddress);
private:
	static CRITICAL_SECTION mc_csDataLock;	// 对mc_threadData保护
	static PtrArrayType mc_threadData;
};
}
#endif