#include <process.h>
#include "sysupport.h"
//#include "AC/shellExt.h"
#pragma comment(lib, "version.lib")
//#include "AC/highLevelAPI.h"	// 加载xp下不支持的API
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	format类	2014-06-27
//	格式化类
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring format::rect(const RECT &rc, bool showSize /*= false*/)
{
	std::wstring wstr;
	wchar_t pbuf[80] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
	wstr = pbuf;
	if (showSize)
	{
		std::swprintf(pbuf, _countof(pbuf), L" %d,%d", rc.right-rc.left, rc.bottom-rc.top);
		wstr += pbuf;
	}
	return wstr;

	//std::wostringstream os;
	//os << rc.left << L"," << rc.top << L"," << rc.right << L"," << rc.bottom;
	//if (showSize)
	//{
	//	os << L" " << rc.right-rc.left << L"," << rc.bottom-rc.top;
	//}
	//os << std::ends;
	//return os.str();
}
RECT format::rect(PCWSTR prect)
{
	RECT rc = {0};
	wchar_t *pend = nullptr;
	try
	{
		rc.left = std::wcstol(prect, &pend, 10);	assert(pend);
		rc.top = std::wcstol(pend+1, &pend, 10);	assert(pend);
		rc.right = std::wcstol(pend+1, &pend, 10);	assert(pend);
		rc.bottom = std::wcstol(pend+1, nullptr, 10);
	}
	catch (...)
	{
		::ZeroMemory(&rc, sizeof(rc));
	}
	return rc;
}

std::wstring format::point(const POINT &pt)
{
	wchar_t pbuf[40] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d,%d", pt.x, pt.y);
	return pbuf;
	//std::wostringstream os;
	//os << pt.x << L"," << pt.y << std::ends;
	//return os.str();
}
POINT format::point(PCWSTR ppoint)
{
	POINT pt = {0};
	wchar_t *pend = nullptr;
	try
	{
		pt.x = std::wcstol(ppoint, &pend, 10);		assert(pend);
		pt.y = std::wcstol(pend+1, nullptr, 10);
	}
	catch (...)
	{
		pt.x = pt.y = 0;
	}
	return pt;
}
std::wstring format::size(const SIZE &sz)
{
	wchar_t pbuf[40] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d,%d", sz.cx, sz.cy);
	return pbuf;
	//std::wostringstream os;
	//os << sz.cx << L"," <<sz.cy << std::ends;
	//return os.str();
}

std::wstring format::color(DWORD clr)
{
	wchar_t pbuf[10] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"#%08X", clr);
	return pbuf;
}
DWORD format::color(PCWSTR pclr)
{
	PWSTR pstr = NULL;
	if( *pclr == L'#') pclr = ::CharNext(pclr);
	DWORD clrColor = std::wcstoul(pclr, &pstr, 16);
	return clrColor;
}
std::wstring format::floatString(float fltValue, int32_t decimals /*= 2*/)
{
	if(decimals < 0) decimals = 0;
	else if(decimals > 16) decimals = 16;
	wchar_t fmt[10] = {0};
	std::swprintf(fmt, 10, L"%%0.%df", decimals);
	wchar_t plgcValue[80] = {0};
	std::swprintf(plgcValue, _countof(plgcValue), fmt, fltValue);

	return plgcValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	convert类	2014-04-25
//	常用类型转换操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	app类
//	常用文件操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring app::mc_wsLogPath = L"";	// 日志文件路径
std::wstring app::mc_wsIniPath = L"";	// 配置文件路径
std::wstring app::mc_wsIniName = L"";	// 配置文件名称
std::wstring app::mc_wsLogFileName = L"";	// 日志文件名的开头部分
std::wstring app::mc_wsDumpFileName = L"";	// dump文件的开头部分

AdapterInfoArrayType app::getAdapterInfo(PCWSTR pmacSeparator /*= NULL*/)
{
	AdapterInfoArrayType vec;
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	PIP_ADAPTER_INFO pAdapter = NULL;
	ULONG len = sizeof(IP_ADAPTER_INFO);
	DWORD dwret = GetAdaptersInfo(pIpAdapterInfo, &len);
	if (dwret == ERROR_BUFFER_OVERFLOW)
	{
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[len];
		assert(pIpAdapterInfo);
		dwret = GetAdaptersInfo(pIpAdapterInfo, &len);
	}
	wchar_t pmac[8] = {0};
	std::wstring wsMac, wsIP;
	int addrLen;
	if (dwret == ERROR_SUCCESS)
	{
		pAdapter = pIpAdapterInfo;
		while (pAdapter)
		{
			wsMac.clear();
			wsIP.clear();
			// mac
			addrLen = (int)pAdapter->AddressLength;
			for (int idx = 0; idx < addrLen; ++idx) {
				std::swprintf(pmac, _countof(pmac), L"%02X", pAdapter->Address[idx]);
				wsMac += pmac;
				if (pmacSeparator && idx < addrLen-1)
				{
					wsMac += pmacSeparator;
				}
			}
			// ip
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			if (pIpAdapterInfo)
			{
				wsIP = gtc::stringUtil::toWChar(pIpAddrString->IpAddress.String);
			}
			//可能网卡有多IP,因此通过循环去判断
			//IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);
			//do
			//{
			//	cout<<"该网卡上的IP数量："<<++IPnumPerNetCard<<endl;
			//	cout<<"IP 地址："<<pIpAddrString->IpAddress.String<<endl;
			//	cout<<"子网地址："<<pIpAddrString->IpMask.String<<endl;
			//	cout<<"网关地址："<<pIpAdapterInfo->GatewayList.IpAddress.String<<endl;
			//	pIpAddrString=pIpAddrString->Next;
			//} while (pIpAddrString);

			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET)
			{
				vec.push_back(BSAdapterInfo(wsIP, wsMac));
			}
			//SEND_LOGA("WARNING name:<%s> desc:<%s> type:<%d> mac:<%s>", pAdapter->AdapterName, pAdapter->Description, pAdapter->Type, strmac.c_str());
			pAdapter = pAdapter->Next;
		}
		delete pIpAdapterInfo;

	}

	return vec;
}
RECT app::getDeskArea(bool containTaskbar /*= true*/)
{
	RECT rcWork = {0};
	if (containTaskbar)
	{
		SIZE sz = {GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)};
		rcWork.right = sz.cx;
		rcWork.bottom = sz.cy;
	}
	else
	{
		SystemParametersInfo(SPI_GETWORKAREA, NULL, (PVOID)&rcWork, 0);  
	}
	return rcWork;
}
bool app::addFilterWindowMessage(HWND hWnd, UINT uMsg)
{
	if(app::isWindowsXP()) return true;
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-6-9 User32.dll API 相关声明
	typedef BOOL (WINAPI* fn_ChangeWindowMessageFilter)(UINT  /*message*/, DWORD /*dwFlag*/);
	typedef BOOL (WINAPI* fn_ChangeWindowMessageFilterEx )(HWND /*hWnd*/, UINT /*message*/, DWORD /*action*/, PCHANGEFILTERSTRUCT /*pChangeFilterStruct*/);

	fn_ChangeWindowMessageFilter			_fnChangeWindowMessageFilter = NULL;
	fn_ChangeWindowMessageFilterEx			_fnChangeWindowMessageFilterEx = NULL;
	HMODULE hmdl = LoadLibrary(L"User32.dll");
	if (hmdl)
	{
		_fnChangeWindowMessageFilter = (fn_ChangeWindowMessageFilter)GetProcAddress(hmdl, "ChangeWindowMessageFilter");
		_fnChangeWindowMessageFilterEx = (fn_ChangeWindowMessageFilterEx)GetProcAddress(hmdl, "ChangeWindowMessageFilterEx");
		FreeLibrary(hmdl);
	}

	if (_fnChangeWindowMessageFilterEx)
	{
		return _fnChangeWindowMessageFilterEx(hWnd, uMsg, MSGFLT_ALLOW, NULL) == TRUE;
	}
	else if (_fnChangeWindowMessageFilter)
	{
		return _fnChangeWindowMessageFilter(uMsg, MSGFLT_ADD) == TRUE;
	}
	return false;
}

void app::captureWindow(HWND hWnd)
{
	assert(hWnd);
	HWND hWndParent = hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
	::SetForegroundWindow(hWnd);
	//::SetActiveWindow(hWndParent);
	//::SetActiveWindow(*_pdialogMainPtr);
	::ShowWindow(hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}
bool app::exec(PCWSTR pexeFile, PCWSTR pParameter /*= NULL*/, int showFlag /*= SW_SHOW*/)
{
	// 用默认浏览器打开网页	exec(L"http://www.xxx.com")
	// 打开文件夹			exec(L"c:\\")
	// 执行命令			exec(L"explorer.exe")
	// 执行命令			exec(L"c:\windows\explorer.exe")
	assert(pexeFile);
	if(!pexeFile) return false;
	if(!(stringUtil::startsWith(pexeFile, L"http://") 
		|| stringUtil::startsWith(pexeFile, L"https://")
		|| stringUtil::startsWith(pexeFile, L"www.")
		|| ::PathFileExistsW(pexeFile)
		|| stringUtil::endsWith(pexeFile, L".exe"))) {
			SEND_LOG(L"ERROR app::exec 文件不存在！<%s>", pexeFile);
			return false;
	}

	//if(!(stringUtil::startsWith(pexeFile, L"http://") 
	//	|| stringUtil::startsWith(pexeFile, L"https://")
	//	|| stringUtil::startsWith(pexeFile, L"www.")) && !::PathFileExistsW(pexeFile)) {
	//	SEND_LOG(L"ERROR app::exec 文件不存在！<%s>", pexeFile);
	//	return false;
	//}
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	SHELLEXECUTEINFO shexe = {0};
	shexe.cbSize = sizeof(SHELLEXECUTEINFO);
	shexe.fMask = SEE_MASK_DEFAULT;
	shexe.lpVerb = L"open";
	shexe.lpFile = pexeFile;
	shexe.lpParameters = pParameter;
	shexe.nShow = SW_SHOW;
	bool bl = ShellExecuteEx(&shexe) == TRUE;
	if(!bl)
		SEND_LOG(L"app::exec 启动进程失败！！！ bl:%d lasterr:%u exefile:<%s>", bl, GetLastError(), pexeFile);
	CoUninitialize();
	return bl;
}

uint64_t app::getProductVersion(PCWSTR pFileName /*= NULL*/, uint64_t *poutFileVersion /*= NULL*/)
{
	uint64_t ver = 0;
	wchar_t pBuf[MAX_PATH] = {0};
	if (!pFileName || std::wcslen(pFileName) <= 0)
	{
		GetModuleFileName(NULL, pBuf, _countof(pBuf));
		pFileName = pBuf;
	}
	DWORD dwHandle = 0;
	BYTE *pData = NULL, *pVerData = NULL;
	VS_FIXEDFILEINFO *pverInfo = NULL;
	UINT len = 0;
	DWORD dwInfoSize = GetFileVersionInfoSize(pFileName, &dwHandle);
	if (dwInfoSize > 0)
	{
		pData = new BYTE[dwInfoSize];
		//pVerData = new BYTE[sizeof(VS_FIXEDFILEINFO)];
		if (GetFileVersionInfo(pFileName, dwHandle, dwInfoSize, pData) 
			&& VerQueryValue(pData, L"\\", (LPVOID *)&pVerData, &len))
		{
			pverInfo = (VS_FIXEDFILEINFO *)pVerData;
			ver = pverInfo->dwProductVersionMS;
			ver <<= 32;
			ver |= pverInfo->dwProductVersionLS;
			if (poutFileVersion)
			{
				*poutFileVersion = pverInfo->dwFileVersionMS;
				*poutFileVersion <<= 32;
				*poutFileVersion |= pverInfo->dwFileVersionLS;
			}
		}
		
	}
	// 
	if(pData) delete[] pData;
	//if(pVerData) delete pVerData;
	return ver;
}
bool app::writeProfile(PCWSTR psection, PCWSTR pkey, PCWSTR pValue, PCWSTR pfileName)
{
	assert(psection && pkey && pValue);
	std::wstring wsini(mc_wsIniPath);
	std::wstring wsname(pfileName ? pfileName : L"");
	if (!wsname.empty() && wsname.find(L':') != std::wstring::npos)
	{	// 全路径
		wsini = wsname;
	}
	else
	{
		wsini += L"\\";
		wsini += (wsname.length() > 0 ? wsname : mc_wsIniName);

	}
	return ::WritePrivateProfileStringW(psection, pkey, pValue, wsini.c_str()) == TRUE;
}

bool app::writeProfile(PCWSTR psection, PCWSTR pkey, int32_t iValue, PCWSTR pfileName)
{
	assert(psection && pkey);
	wchar_t buf[32] = {0};
	std::swprintf(buf, _countof(buf), L"%d", iValue);
	return writeProfile(psection, pkey, buf, pfileName);
}

bool app::writeProfile(PCWSTR psection, PCWSTR pkey, const POINT &pt, PCWSTR pfileName)
{
	assert(psection && pkey);
	wchar_t buf[32] = {0};
	std::swprintf(buf, _countof(buf), L"%d,%d", pt.x, pt.y);
	return writeProfile(psection, pkey, buf, pfileName);
}

bool app::writeProfile(PCWSTR psection, PCWSTR pkey, const RECT &rc, PCWSTR pfileName /*= nullptr*/)
{
	assert(psection && pkey);
	wchar_t buf[32] = {0};
	std::swprintf(buf, _countof(buf), L"%s", gtc::format::rect(rc).c_str());
	return writeProfile(psection, pkey, buf, pfileName);
}
bool app::writeProfile(PCWSTR psection, PCWSTR pkey, datetime &dt, PCWSTR pfileName /*= nullptr*/)
{
	assert(psection && pkey);
	return writeProfile(psection, pkey, dt.format().c_str());
}
bool app::readProfile(PCWSTR psection, PCWSTR pkey, std::wstring &refoutData, PCWSTR pfileName)
{
	static std::wstring c_wsDefaultNone = L"{3ED02101-0407-41A3-A601-5C9936B36B5F}";
	assert(psection && pkey);
	std::wstring wsini(mc_wsIniPath);
	std::wstring wsname(pfileName ? pfileName : L"");
	if (!wsname.empty() && wsname.find(L':') != std::wstring::npos)
	{	// 全路径
		wsini = wsname;
	}
	else
	{
		wsini += L"\\";
		wsini += (wsname.length() > 0 ? wsname : mc_wsIniName);

	}

	wchar_t buf[MAX_PATH] = {0};
	DWORD len = GetPrivateProfileStringW(psection, pkey, c_wsDefaultNone.c_str(), buf, _countof(buf), wsini.c_str());
	buf[len] = L'\0';
	refoutData = buf;

	bool bRet = refoutData != c_wsDefaultNone;
	if(!bRet) refoutData.clear();
	return bRet;
}

bool app::readProfileLarge(PCWSTR psection, PCWSTR pkey, std::wstring &refoutData, PCWSTR pfileName /*= nullptr*/)
{
	static std::wstring c_wsDefaultNone = L"{3ED02101-0407-41A3-A601-5C9936B36B5F}";
	assert(psection && pkey);
	std::wstring wsini(mc_wsIniPath);
	std::wstring wsname(pfileName ? pfileName : L"");
	if (!wsname.empty() && wsname.find(L':') != std::wstring::npos)
	{	// 全路径
		wsini = wsname;
	}
	else
	{
		wsini += L"\\";
		wsini += (wsname.length() > 0 ? wsname : mc_wsIniName);

	}

	wchar_t buf[4000] = {0};
	DWORD len = GetPrivateProfileStringW(psection, pkey, c_wsDefaultNone.c_str(), buf, _countof(buf), wsini.c_str());
	buf[len] = L'\0';
	refoutData = buf;

	return refoutData != c_wsDefaultNone;
}

bool app::readProfileInt(PCWSTR psection, PCWSTR pkey, int32_t &refoutData, PCWSTR pfileName, int32_t dftValue /*= 0*/)
{
	assert(psection && pkey);
	std::wstring wstr;
	if (readProfile(psection, pkey, wstr, pfileName))
	{
		refoutData = std::wcstol(wstr.c_str(), NULL, 10);
		return true;
	}
	else
	{
		refoutData = dftValue;
	}
	return false;
	//std::wstring wsini(mc_wsIniPath);
	//wsini += L"\\";
	//wsini += (pfileName && std::wcslen(pfileName) > 0 ? pfileName : mc_wsIniName);

	//return GetPrivateProfileIntW(psection, pkey, dftValue, wsini.c_str());
}

bool app::readProfilePoint(PCWSTR psection, PCWSTR pkey, POINT &refoutData, PCWSTR pfileName)
{
	std::wstring wstr;
	if (readProfile(psection, pkey, wstr, pfileName) && !wstr.empty())
	{
		refoutData = format::point(wstr.c_str());
		return true;
	}
	return false;
}

bool app::readProfileRect(PCWSTR psection, PCWSTR pkey, RECT &refoutData, PCWSTR pfileName /*= nullptr*/)
{
	std::wstring wstr;
	if (readProfile(psection, pkey, wstr, pfileName) && !wstr.empty())
	{
		refoutData = format::rect(wstr.c_str());
		return true;
	}
	return false;

}

bool app::readProfileDatetime(PCWSTR psection, PCWSTR pkey, datetime &refoutData, PCWSTR pfileName /*= nullptr*/)
{
	std::wstring wstr;
	if (readProfile(psection, pkey, wstr, pfileName) && !wstr.empty())
	{
		refoutData.parse(wstr.c_str());
		return true;
	}
	return false;
}
std::wstring app::expandEnvironmentString(PCWSTR pSrc)
{
	std::wstring ws;
	DWORD len = ::ExpandEnvironmentStrings(pSrc, NULL, 0);
	if (len > 0)
	{
		wchar_t *pDst = new wchar_t[len+1];
		memset(pDst, 0, (len+1)*sizeof(wchar_t));
		::ExpandEnvironmentStrings(pSrc, pDst, len+1);
		ws = pDst;
		delete[] pDst;
	}
	return ws;
}
BEOSVersion app::getOSVersion(OSVERSIONINFOEX *posVersion /*= NULL*/)
{
	static BEOSVersion ver = BEOSVersionNone;
	static OSVERSIONINFOEX osver = {0};
	if(ver != BEOSVersionNone)
	{
		if(posVersion)
		{
			if(osver.dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX))
				memcpy_s(posVersion, sizeof(OSVERSIONINFOEX), &osver, sizeof(OSVERSIONINFOEX));
			else
				memcpy_s(posVersion, sizeof(OSVERSIONINFO), &osver, sizeof(OSVERSIONINFO));
		}
		return ver;
	}
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx((OSVERSIONINFO *)&osver))
	{
		osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO *)&osver)) return BEOSVersionNone;
	}
	SYSTEM_INFO info = {0};		//用SYSTEM_INFO结构判断64位AMD处理器  
	GetSystemInfo(&info);		//调用GetSystemInfo函数填充结构
	switch(osver.dwMajorVersion)
	{
	case 5:
		{
			switch(osver.dwMinorVersion)
			{
			case 1:
				{
					if(osver.wSuiteMask	& VER_SUITE_PERSONAL)
						ver = BEOSVersionXPHome;
					else
						ver = BEOSVersionXPProfessional;

				}
				break;
			case 2:
				{
					if(osver.wProductType == VER_NT_WORKSTATION 
						&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						ver = BEOSVersionXP64;
					else if(GetSystemMetrics(SM_SERVERR2)==0)
						ver = BEOSVersionServer2003;
					else if(GetSystemMetrics(SM_SERVERR2)!=0)
						ver = BEOSVersionServer2003R2;
				}
				break;
			}
		}
		break;
	case 6:
		{
			switch(osver.dwMinorVersion)
			{
			case 0:
				{
					if(osver.wProductType == VER_NT_WORKSTATION)
						ver = BEOSVersionVista;
					else
						ver = BEOSVersionServer2008;
				}
				break;
			case 1:
				{
					if(osver.wProductType == VER_NT_WORKSTATION)
					{
						if(osver.wSuiteMask == (VER_SUITE_PERSONAL | VER_SUITE_SINGLEUSERTS))
						ver = BEOSVersionWin7HomeBasic;
					}
					else
						ver = BEOSVersionServer2008R2;
				}
				break;
			case 2:
				{
					if(osver.wProductType == VER_NT_WORKSTATION)
						ver = BEOSVersionWin8;
					else
						ver = BEOSVersionServer2012;
				}
				break;
			case 3:
				{
					if(osver.wProductType == VER_NT_WORKSTATION)
						ver = BEOSVersionWin81;
					else
						ver = BEOSVersionServer2012R2;

				}
				break;
			}
		}
		break;
	}

	if(posVersion)
	{
		if(osver.dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX))
			memcpy_s(posVersion, sizeof(OSVERSIONINFOEX), &osver, sizeof(OSVERSIONINFOEX));
		else
			memcpy_s(posVersion, sizeof(OSVERSIONINFO), &osver, sizeof(OSVERSIONINFO));
	}
	return ver;

}

bool app::isWindowsXP()
{
	BEOSVersion osv = getOSVersion();
	return osv == BEOSVersionXPHome || osv == BEOSVersionXPProfessional || osv == BEOSVersionXP64;
}
bool app::isWindowsVistaLater()
{
	OSVERSIONINFOEX osver = {0};
	BEOSVersion osv = getOSVersion(&osver);
	return osver.dwMajorVersion >= 6 && osver.dwMinorVersion >= 0;

}
bool app::isWindows7Later()
{
	OSVERSIONINFOEX osver = {0};
	BEOSVersion osv = getOSVersion(&osver);
	return osver.dwMajorVersion >= 6 && osver.dwMinorVersion >= 1;
}
bool app::isWindows8Later()
{
	OSVERSIONINFOEX osver = {0};
	BEOSVersion osv = getOSVersion(&osver);
	return osver.dwMajorVersion >= 6 && osver.dwMinorVersion >= 2;
}

bool app::isWindowsOS64()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	return (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
		|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64);

		//bool is64 = false;
	//typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	//LPFN_ISWOW64PROCESS fnIsWow64Process;

	//fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(L"kernel32"),"IsWow64Process");
	//if (fnIsWow64Process)
	//{
	//	BOOL f64 = FALSE;
	//	if(fnIsWow64Process(GetCurrentProcess(),&f64)) is64 = f64 == TRUE;
	//}
	//else
	//{
	//	SYSTEM_INFO info = {0};		//用SYSTEM_INFO结构判断64位AMD处理器  
	//	GetSystemInfo(&info);		//调用GetSystemInfo函数填充结构
	//	is64 = info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
	//		|| info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
	//		|| info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64
	//		|| info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64;
	//}

	//return is64;
}

#if defined(_WIN32)
int app::sehFilterDefault(LPEXCEPTION_POINTERS pExcept, PCWSTR pdesc, bool createDumpFile /*= true*/)
{
	if (pExcept)
	{
		if(createDumpFile) sehUnhandleFilterDump(pExcept);

		if (pExcept->ExceptionRecord)
		{
			gtc::app::writeVSLogs(L"SEH_<%s> code:%08X flags:%d exceptionRecord:%08X exceptionAddress:%08X NumberParameters:%d", 
				pdesc?pdesc:L"", pExcept->ExceptionRecord->ExceptionCode, pExcept->ExceptionRecord->ExceptionFlags, pExcept->ExceptionRecord->ExceptionRecord,
				pExcept->ExceptionRecord->ExceptionAddress, pExcept->ExceptionRecord->NumberParameters);
			//if(pExcept->ExceptionRecord->ExceptionFlags == 0 && pExcept->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
			//	return EXCEPTION_CONTINUE_EXECUTION;
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI app::sehUnhandleFilterDump(LPEXCEPTION_POINTERS pExcept)
{
	if (pExcept)
	{
		std::wstring path(mc_wsLogPath.empty()?getAppPath():mc_wsLogPath);
		char stimer[60] = {0};
		std::time_t tm = {0};
		std::time(&tm);
		std::tm *ptminfo = std::localtime(&tm);
		assert(ptminfo);
		std::strftime(stimer, sizeof(stimer), "%Y%m%d%H%M%S.dmp", ptminfo);
		path += L"\\";
		path += mc_wsDumpFileName;
		path += convert::toWChar(stimer);
		//messageEx(L"%08X logpath:<%s>", pExcept, path.c_str());
		HANDLE hFile = ::CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION exceptInfo = {0};
			exceptInfo.ThreadId = ::GetCurrentThreadId();
			exceptInfo.ExceptionPointers = pExcept;
			exceptInfo.ClientPointers = FALSE;
			BOOL bOK = ::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &exceptInfo, NULL, NULL );
			if (!bOK)
			{
				writeVSLogs(L"app::sehUnhandleFilterDump 创建dmp文件失败#%d！", ::GetLastError());
			}
			::CloseHandle(hFile);
		}
		//if(pExcept->ExceptionRecord && pExcept->ExceptionRecord->ExceptionFlags == 0 && pExcept->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		//	return EXCEPTION_CONTINUE_EXECUTION;

	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

HGLOBAL app::createGlobalHandle(const void* ptr, int size, UINT nFlag /*= GMEM_SHARE|GMEM_MOVEABLE*/)
{
	assert(size > 0);
	if(size <= 0) return NULL;
	HGLOBAL hGlobal = GlobalAlloc(nFlag, size);  
	if (NULL != hGlobal && ptr != NULL)  
	{  
		LPVOID pdest = GlobalLock(hGlobal);  
		if (NULL != pdest)  
		{  
			memcpy_s(pdest, size, ptr, size);  
		}  
		GlobalUnlock(hGlobal);  
	}  
	return hGlobal; 
}
HGLOBAL app::copyGlobalMemory(HGLOBAL hDest, HGLOBAL hSource)
{
	assert(hSource != NULL);

	// make sure we have suitable hDest
	ULONG_PTR nSize = ::GlobalSize(hSource);
	if (hDest == NULL)
	{
		hDest = ::GlobalAlloc(GMEM_SHARE|GMEM_MOVEABLE, nSize);
		if (hDest == NULL)
			return NULL;
	}
	else if (nSize > ::GlobalSize(hDest))
	{
		// hDest is not large enough
		return NULL;
	}

	// copy the bits
	LPVOID lpSource = ::GlobalLock(hSource);
	LPVOID lpDest = ::GlobalLock(hDest);
	assert(lpDest != NULL);
	assert(lpSource != NULL);
	::memcpy_s(lpDest, (ULONG)::GlobalSize(hDest), lpSource, (ULONG)nSize);
	::GlobalUnlock(hDest);
	::GlobalUnlock(hSource);

	// success -- return hDest
	return hDest;
}

void app::setClipboard(const std::wstring &wstr)
{
	if(wstr.size() == 0) return;
	if(!OpenClipboard(NULL)) return;
	::EmptyClipboard();
	//std::string str = gtc::stringUtil::toChar(wstr.c_str());
	HGLOBAL hg = createGlobalHandle(wstr.c_str(), (wstr.length()+1)*sizeof(wchar_t), GMEM_DDESHARE);
	//HGLOBAL hg = createGlobalHandle(str.c_str(), str.length()+1, GMEM_DDESHARE);
	if(!hg)
	{
		CloseClipboard();
		return;
	}

	SetClipboardData(CF_UNICODETEXT,hg); 

	CloseClipboard();
	//GlobalFree(hg);

}

void app::setClipboard(const std::string &str)
{
	if(str.size() == 0) return;
	if(!OpenClipboard(NULL)) return;
	::EmptyClipboard();
	HGLOBAL hg = createGlobalHandle(str.c_str(), str.length()+1, GMEM_DDESHARE);
	//HGLOBAL hg = createGlobalHandle(str.c_str(), str.length()+1, GMEM_DDESHARE);
	if(!hg)
	{
		CloseClipboard();
		return;
	}

	SetClipboardData(CF_TEXT,hg); 

	CloseClipboard();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	thread  类		2015-3-18
//	线程操作的相关封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
CRITICAL_SECTION		thread::mc_csDataLock;
thread::PtrArrayType	thread::mc_threadData;

template<> thread* gtc::singleton<thread>::m_pSingleton = NULL;

static thread nc_thread;

thread::thread()
{
	::InitializeCriticalSectionAndSpinCount(&thread::mc_csDataLock, 4000);
}
thread::~thread()
{
	::DeleteCriticalSection(&thread::mc_csDataLock);
}

// gtc::singleton
thread& thread::getSingleton()
{
	return *m_pSingleton;
}
thread* thread::getSingletonPtr()
{
	return m_pSingleton;
}
HANDLE thread::start(PTHREAD_START_ADDRESS pstartAddress, void *pParam, bool onlyOne /*= false*/
	, bool isAutofree /*= true*/, volatile LONG *pLooplock /*= NULL*/, HANDLE hEvent /*= NULL*/, PCWSTR pDesc /*= NULL*/)
{
	assert(pstartAddress);
	if(!pstartAddress) return false;
	// 是否只要一个运行
	if(onlyOne && exists(pstartAddress)) return NULL;

	_BSThreadInfo *pthr = new _BSThreadInfo();
	assert(pthr);
	::memset(pthr, 0, sizeof(_BSThreadInfo));
	pthr->bAutofree = isAutofree;
	pthr->plLooplock = pLooplock;
	pthr->hEvent = hEvent;
	if(pDesc) std::wcsncpy(pthr->pDescription, pDesc, _countof(pthr->pDescription));
	pthr->pParameter = pParam;
	pthr->pthreadFunc = pstartAddress;
	pthr->hThread = (HANDLE)::_beginthreadex(NULL, 0, (unsigned int (__stdcall * ) (void *))pstartAddress, pthr, CREATE_SUSPENDED, NULL);
	if (pthr->hThread)
	{
		::EnterCriticalSection(&mc_csDataLock);
		mc_threadData.push_back(pthr);
		::LeaveCriticalSection(&mc_csDataLock);

		if(pthr->plLooplock) 
		{
			assert(*pthr->plLooplock == 0);
			InterlockedCompareExchange(pthr->plLooplock, 1, 0);
		}
		::ResumeThread(pthr->hThread);
	}
	else
		delete pthr;
	return pthr->hThread;
}

void thread::free(_BSThreadInfo *pThreadInfo)
{
	assert(pThreadInfo);
	if(!pThreadInfo) return ;
	::EnterCriticalSection(&mc_csDataLock);
	PtrArrayType::iterator iter = std::find(mc_threadData.begin(), mc_threadData.end(), pThreadInfo);
	if (iter != mc_threadData.end())
	{
		if((*iter)->bAutofree && (*iter)->hThread) CloseHandle((*iter)->hThread);
		delete *iter;
		mc_threadData.erase(iter);
	}
	::LeaveCriticalSection(&mc_csDataLock);
}
DWORD thread::waitfor(HANDLE hThread, DWORD timeout /*= INFINITE*/)
{
	assert(hThread);
	DWORD dwRet = 0;
	_BSThreadInfo *pThreadInfo = NULL;

	::EnterCriticalSection(&mc_csDataLock);
	for (PtrArrayType::iterator iter = mc_threadData.begin(); iter != mc_threadData.end(); ++iter)
	{
		if ((*iter)->hThread == hThread)
		{
			pThreadInfo = *iter;
			break;
		}
	}
	::LeaveCriticalSection(&mc_csDataLock);

	if (pThreadInfo)
	{
		if(pThreadInfo->plLooplock) InterlockedCompareExchange(pThreadInfo->plLooplock, 0, 1);
		if(pThreadInfo->hEvent) SetEvent(pThreadInfo->hEvent);
		dwRet = WaitForSingleObject(pThreadInfo->hThread, timeout);
		if (dwRet == WAIT_OBJECT_0)
		{
			if(!pThreadInfo->bAutofree) free(pThreadInfo);
		}
	}
	return dwRet;
}

bool thread::exists(PTHREAD_START_ADDRESS pstartAddress)
{
	assert(pstartAddress);
	::EnterCriticalSection(&mc_csDataLock);
	for (PtrArrayType::const_iterator iter = mc_threadData.cbegin(); iter != mc_threadData.cend(); ++iter)
	{
		if ((*iter)->pthreadFunc == pstartAddress)
		{
			::LeaveCriticalSection(&mc_csDataLock);
			return true;
		}
	}
	::LeaveCriticalSection(&mc_csDataLock);
	return false;
}

}