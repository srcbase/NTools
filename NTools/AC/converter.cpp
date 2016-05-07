#include "converter.h"
#include "stringUtil.h"
namespace gtc 
{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	convert类	2014-04-25
//	常用类型转换操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////

//std::wstring convert::toBase64(const SBDataType &sbData)
//{
//	std::wstring wsstr;
//
//	return wsstr;
//}
//SBDataType convert::toData(PCWSTR pbase64, std::size_t lenBase64)
//{
//	SBDataType data;
//
//	return data;
//}
void convert::replace(std::wstring &wsdata, wchar_t chScr, wchar_t chDest)
{
	std::wstring::size_type pos = 0;
	while((pos = wsdata.find(chScr, pos)) != std::wstring::npos)
	{
		wsdata.replace(pos++, 1, 1, chDest);
	}
}
std::wstring convert::deleteChar(std::wstring wsdata, wchar_t chDel)
{
	std::wstring wsstr;
	std::wstring::size_type pos = 0;
	while((pos = wsdata.find(chDel, pos)) != std::wstring::npos)
	{
		wsstr = wsdata.substr(0, pos);
		wsstr += wsdata.substr(pos+1);
		wsdata = wsstr;
	}

	return wsdata;
}
std::wstring convert::deleteNoGraph(std::wstring wsdata)
{
	//std::wstring wsstr;
	//std::wstring::size_type pos = 0;
	for (std::wstring::iterator iter = wsdata.begin(); iter != wsdata.end();)
	{
		if (!std::iswspace(*iter) && !std::iswgraph(*iter))
		{
			iter = wsdata.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	return wsdata;
}

bool convert::toUpper(PCWSTR pstr, PWSTR *ppUpper)
{
	if(!pstr || !ppUpper) return false;
	std::wstring ws = toUpper(pstr);
	std::wstring::size_type len = ws.length();
	if (len > 0)
	{
		++len;
		*ppUpper = new wchar_t[len];
		std::wcsncpy(*ppUpper, ws.data(), len);
		(*ppUpper)[len-1] = L'\0';
	}
	return *ppUpper != NULL;
}

bool convert::toUpper(PCSTR pstr, PSTR *ppUpper)
{
	if(!pstr || !ppUpper) return false;
	std::string str = toUpper(pstr);
	std::string::size_type len = str.length();
	if (len > 0)
	{
		++len;
		*ppUpper = new char[len];
		std::strncpy(*ppUpper, str.data(), len);
		(*ppUpper)[len-1] = '\0';
	}
	return *ppUpper != NULL;
}

bool convert::toLower(PCWSTR pstr, PWSTR *ppLower)
{
	if(!pstr || !ppLower) return false;
	std::wstring ws = toLower(pstr);
	std::wstring::size_type len = ws.length();
	if (len > 0)
	{
		++len;
		*ppLower = new wchar_t[len];
		std::wcsncpy(*ppLower, ws.data(), len);
		(*ppLower)[len-1] = L'\0';
	}

	return *ppLower != NULL;
}

bool convert::toLower(PCSTR pstr, PSTR *ppLower)
{
	if(!pstr || !ppLower) return false;
	std::string str = toLower(pstr);
	std::string::size_type len = str.length();
	if (len > 0)
	{
		++len;
		*ppLower = new char[len];
		std::strncpy(*ppLower, str.data(), len);
		(*ppLower)[len-1] = '\0';
	}

	return *ppLower != NULL;
}

void convert::trimLeft(std::wstring &wstr)
{
	for (std::wstring::iterator iter = wstr.begin(); iter != wstr.end(); )
	{
		if(*iter == L' ')
			iter = wstr.erase(iter);
		else
			break;
	}
}
std::wstring convert::trimLeft(PCWSTR pstr)
{
	std::wstring wstr = pstr ?pstr :L"";
	trimLeft(wstr);
	return wstr;
}
void convert::trimLeft(std::string &str)
{
	for (std::string::iterator iter = str.begin(); iter != str.end(); )
	{
		if(*iter == ' ')
			iter = str.erase(iter);
		else
			break;
	}
}
std::string convert::trimLeft(PCSTR pstr)
{
	std::string wstr = pstr ?pstr :"";
	trimLeft(wstr);
	return wstr;
}

void convert::trimRight(std::wstring &wstr)
{
	int sp = 0;
	for (std::wstring::reverse_iterator iter = wstr.rbegin(); iter != wstr.rend(); ++iter)
	{
		if(*iter == L' ')
			++sp;
		else
			break;
	}
	if (sp > 0)
	{
		wstr = wstr.substr(0, wstr.length()-sp);
	}
}
std::wstring convert::trimRight(PCWSTR pstr)
{
	std::wstring wstr = pstr? pstr :L"";
	trimRight(wstr);
	return wstr;
}
void convert::trimRight(std::string &str)
{
	int sp = 0;
	for (std::string::reverse_iterator iter = str.rbegin(); iter != str.rend(); ++iter)
	{
		if(*iter == ' ')
			++sp;
		else
			break;
	}
	if (sp > 0)
	{
		str = str.substr(0, str.length()-sp);
	}
}
std::string convert::trimRight(PCSTR pstr)
{
	std::string str = pstr? pstr :"";
	trimRight(str);
	return str;
}

void convert::trim(std::wstring &wstr)
{
	trimLeft(wstr);
	trimRight(wstr);
}
std::wstring convert::trim(PCWSTR pstr)
{
	std::wstring wstr = pstr? pstr :L"";
	trim(wstr);
	return wstr;
}
void convert::trim(std::string &str)
{
	trimLeft(str);
	trimRight(str);
}
std::string convert::trim(PCSTR pstr)
{
	std::string str = pstr? pstr :"";
	trim(str);
	return str;
}

std::wstring convert::toStringHex(const char *pdata, int32_t dataLen, bool bUpper /*= true*/)
{
	std::string str = toStringHexA(pdata, dataLen, bUpper);
	return gtc::stringUtil::toWChar(str.c_str());
}
#if defined(_WIN32)

std::time_t convert::toTimet(const FILETIME &fileTime)
{
	//std::time_t tt = 0;
	//int64_t ll = int64_t(fileTime.dwHighDateTime << 32) | fileTime.dwLowDateTime;
	LONGLONG  ll;     
	ULARGE_INTEGER ui;    
	ui.LowPart=fileTime.dwLowDateTime;    
	ui.HighPart=fileTime.dwHighDateTime;
	ll = fileTime.dwHighDateTime;
	ll <<= 32;
	ll |= fileTime.dwLowDateTime;
	//ll=(fileTime.dwHighDateTime<<32)+fileTime.dwLowDateTime; 

	return (LONGLONG)(ui.QuadPart- EPOCH_OFFSET) /10000000;
	//return (ll-EPOCH_OFFSET)/10000000;
}

#endif
}	// gtc