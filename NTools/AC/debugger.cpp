#include "debugger.h"
#include "AC/sysupport.h"


namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	debugger类	2015-1-8
//	输出调试信息，设置调试断点，输入调试日志
////////////////////////////////////////////////////////////////////////////////////////////////////////

BELogType debugger::preprocessFormat(const wchar_t *pformat, int &offset)
{
	gtc::BELogType logType = gtc::BELogTypeNormal;
	offset = 0;
	if (pformat)
	{
		if(std::wcsncmp(pformat, L"WARNING ", 8) == 0)
		{
			offset = 8;
			logType = gtc::BELogTypeWarning;
		}
		else if(std::wcsncmp(pformat, L"NOTE ", 5) == 0)
		{
			offset = 5;
			logType = gtc::BELogTypeNote;
		}
		else if(std::wcsncmp(pformat, L"ERROR ", 6) == 0)
		{
			offset = 6;
			logType = gtc::BELogTypeError;
		}
	}

	return logType;
}
BELogType debugger::preprocessFormat(const char *pformat, int &offset)
{
	gtc::BELogType logType = gtc::BELogTypeNormal;
	offset = 0;
	if (pformat)
	{
		if(std::strncmp(pformat, "WARNING ", 8) == 0)
		{
			offset = 8;
			logType = gtc::BELogTypeWarning;
		}
		else if(std::strncmp(pformat, "NOTE ", 5) == 0)
		{
			offset = 5;
			logType = gtc::BELogTypeNote;
		}
		else if(std::strncmp(pformat, "ERROR ", 6) == 0)
		{
			offset = 6;
			logType = gtc::BELogTypeError;
		}
	}

	return logType;
}
bool debugger::isAvailable() 
{
	return IsDebuggerPresent() == TRUE;
}
//void debugger::message(const std::wstring &msg)
//{
//#if defined(_DEBUG)
//	std::fputws(L"\n==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==\n", stderr);
//	std::fputws(msg.c_str(), stderr);
//	std::fputws(L"\n==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==\n", stderr);
//	OutputDebugStringW(msg.c_str());
//	OutputDebugStringW(L"\n");
//#endif
//}
//void debugger::message(const std::string& msg)
//{
//#if defined(_DEBUG)
//	message(gtc::convert::toWChar(msg.c_str()));
//#endif
//}

void debugger::message(const wchar_t *pformat, ...)
{
#if defined(_DEBUG)
	wchar_t pinfo[DBGER_MAX_LOG_LEN] = {0};
	va_list argList;
	va_start(argList, pformat);
	std::vswprintf(pinfo, _countof(pinfo)-sizeof(wchar_t), pformat, argList);
	va_end(argList);
	std::fputws(L"\n==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==\n", stderr);
	std::fputws(pinfo, stderr);
	std::fputws(L"\n==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==*==\n", stderr);
	std::wcscat(pinfo, L"\n");
	OutputDebugStringW(pinfo);
#endif

}

void debugger::write(const wchar_t *pformat, ...)
{
	wchar_t pinfo[DBGER_MAX_LOG_LEN] = {0};
	va_list argList;
	va_start(argList, pformat);
	std::vswprintf(pinfo, _countof(pinfo)-sizeof(wchar_t), pformat, argList);
	va_end(argList);
	gtc::app::writeLogs(pinfo);
}

void debugger::send(const wchar_t *pformat, ...)
{
	int offset = 0;
	gtc::BELogType logType = preprocessFormat(pformat, offset);
	//if (pformat)
	//{
	//	if(std::wcsncmp(pformat, L"WARNING ", 8) == 0)
	//	{
	//		offset = 8;
	//		logType = gtc::BELogTypeWarning;
	//	}
	//	else if(std::wcsncmp(pformat, L"NOTE ", 5) == 0)
	//	{
	//		offset = 5;
	//		logType = gtc::BELogTypeNote;
	//	}
	//	else if(std::wcsncmp(pformat, L"ERROR ", 6) == 0)
	//	{
	//		offset = 6;
	//		logType = gtc::BELogTypeError;
	//	}
	//}

	wchar_t szBuffer[DBGER_MAX_LOG_LEN] = { 0 };
	va_list args;
	pformat += offset;
	va_start(args, pformat);
	std::vswprintf(szBuffer, _countof(szBuffer) - 1 - offset, pformat, args);
	va_end(args);

	gtc::app::sendtoUser(logType, szBuffer/*+offset*/, DBGER_WND_MAIN_CAPTION, DBGER_WND_MAIN_CLASS);

}
//void debugger::send_tick(const wchar_t *pformat, ...)
//{
//	static DWORD c_oldTick = GetTickCount();
//
//	gtc::BELogType logType = gtc::BELogTypeNormal;
//	int offset = 0;
//	if (pformat)
//	{
//		if(std::wcsncmp(pformat, L"WARNING ", 8) == 0)
//		{
//			offset = 8;
//			logType = gtc::BELogTypeWarning;
//		}
//		else if(std::wcsncmp(pformat, L"NOTE ", 5) == 0)
//		{
//			offset = 5;
//			logType = gtc::BELogTypeNote;
//		}
//		else if(std::wcsncmp(pformat, L"ERROR ", 6) == 0)
//		{
//			offset = 6;
//			logType = gtc::BELogTypeError;
//		}
//	}
//	wchar_t szBuffer[DBGER_MAX_LOG_LEN] = { 0 };
//	va_list args;
//	pformat += offset;
//	va_start(args, pformat);
//	std::vswprintf(szBuffer, _countof(szBuffer) - 1 - offset, pformat, args);
//	va_end(args);
//
//	std::wstring wstr = szBuffer;
//	std::swprintf(szBuffer, _countof(szBuffer), L" tick:%06d ", GetTickCount() - c_oldTick);
//	c_oldTick = GetTickCount();
//	std::wstring::size_type pos = wstr.find(L' ');
//	if (pos != std::wstring::npos)
//	{
//		wstr.insert(pos, szBuffer);
//	}
//	else
//	{
//		wstr.append(szBuffer);
//	}
//	gtc::app::sendtoUser(logType, wstr.c_str(), DBGER_WND_MAIN_CAPTION, DBGER_WND_MAIN_CLASS);
//}
void debugger::sendA(const char *pformat, ...)
{
	int offset = 0;
	gtc::BELogType logType = preprocessFormat(pformat, offset);
	//if (pformat)
	//{
	//	if(std::strncmp(pformat, "WARNING ", 8) == 0)
	//	{
	//		offset = 8;
	//		logType = gtc::BELogTypeWarning;
	//	}
	//	else if(std::strncmp(pformat, "NOTE ", 5) == 0)
	//	{
	//		offset = 5;
	//		logType = gtc::BELogTypeNote;
	//	}
	//	else if(std::strncmp(pformat, "ERROR ", 6) == 0)
	//	{
	//		offset = 6;
	//		logType = gtc::BELogTypeError;
	//	}
	//}

	char szBuffer[DBGER_MAX_LOG_LEN] = { 0 };
	va_list args;
	pformat += offset;
	va_start(args, pformat);
	std::vsprintf(szBuffer, pformat, args);
	va_end(args);

	gtc::app::sendtoUser(logType, gtc::stringUtil::toWChar(szBuffer).c_str()/*+offset*/, DBGER_WND_MAIN_CAPTION, DBGER_WND_MAIN_CLASS);
}

//void debugger::send(BELogType logType, const wchar_t *pformat, ...)
//{
//	wchar_t szBuffer[DBGER_MAX_LOG_LEN] = { 0 };
//	va_list args;
//	va_start(args, pformat);
//	std::vswprintf(szBuffer, _countof(szBuffer) - sizeof(wchar_t), pformat, args);
//	va_end(args);
//	//gtc::app::sendtoUser(szBuffer, L"Untitled - testDeskHost");
//	gtc::app::sendtoUser(logType, szBuffer, DBGER_WND_MAIN_CAPTION, DBGER_WND_MAIN_CLASS);
//
//}
//void debugger::sendA(BELogType logType, const char *pformat, ...)
//{
//	char szBuffer[DBGER_MAX_LOG_LEN] = {0};
//	va_list args;
//	va_start(args, pformat);
//	std::vsprintf(szBuffer, pformat, args);
//	va_end(args);
//	send(logType, L"%s", gtc::stringUtil::toWChar(szBuffer).c_str());
//
//}

//void debugger::message(const std::string& msg, const char* file, int line)
//{
//#if defined(_DEBUG)
//	std::ostringstream str;
//	str << msg << " [in file \"" << file << "\", line " << line << "]";
//	message(str.str());
//#endif
//}


void debugger::enter()
{
#if defined(_DEBUG)
	if (isAvailable())
	{
		DebugBreak();
	}
#endif
}

void debugger::enter(const std::wstring& msg)
{
#if defined(_DEBUG)
	message(msg.c_str());
	enter();
#endif
}


void debugger::enter(const std::wstring& msg, const char* file, int line)
{
#if defined(_DEBUG)	
	message(L"%s [in file \"%s\", line %d]", msg.c_str(), gtc::convert::toWChar(file).c_str(), line);
	enter();
#endif
}


void debugger::enter(const char* file, int line)
{
#if defined(_DEBUG)
	message(L"BREAK [in file \"%s\", line %d]", gtc::convert::toWChar(file).c_str(), line);
	enter();
#endif
}
}