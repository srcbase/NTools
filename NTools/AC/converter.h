#ifndef __CONVERTER_H__
#define __CONVERTER_H__
#pragma once
#include "basedef.h"
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	convert类	2014-04-25
//	常用类型转换操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class convert
{
private:
	convert();
	~convert();
	convert(const convert &);
	convert& operator=(const convert &);
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// 2015-12-7		kp007	按字节反转 B_0 B_1 B_2 B_3 --> B_3 B_2 B_1 B_0
	static int16_t flipBytes(int16_t value) {
		return (int16_t)flipBytes(uint16_t(value));
	}
	static uint16_t flipBytes(uint16_t value) {
#if defined(_WIN32)
		return _byteswap_ushort(value);
#else
		return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
#endif
	}
	static int32_t flipBytes(int32_t value) 
	{
		return int32_t(flipBytes(uint32_t(value)));
	}
	static uint32_t flipBytes(uint32_t value) {
#if defined(_WIN32)
		return _byteswap_ulong(value);
//#elif defined(POCO_HAVE_GCC_BYTESWAP)
//		return __builtin_bswap32(value);
#else
		return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
			| ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
#endif
	}
	static int64_t flipBytes(int64_t value) {
		return int64_t(flipBytes(uint64_t(value)));
	}
	static uint64_t flipBytes(uint64_t value) {
#if defined(_WIN32)
		return _byteswap_uint64(value);
//#elif defined(POCO_HAVE_GCC_BYTESWAP)
//		return __builtin_bswap64(value);
#else
		uint32_t hi = uint32_t(value >> 32);
		uint32_t lo = uint32_t(value & 0xFFFFFFFF);
		return uint64_t(flipBytes(hi)) | (uint64_t(flipBytes(lo)) << 32);
#endif
	}

	///*****************************************************************************************
	//	<< --- convert::toBase64		2014-09-27 --- >>
	//	说明：将二进制数据转换成base64字符串
	//	参数：
	//	sbData	= 被操作的数据
	//	返回值：base64字符串
	//*****************************************************************************************/
	//static std::wstring toBase64(const SBDataType &sbData);
	//// 将base64编码的字符串转换成字节集合
	//static SBDataType toData(PCWSTR pbase64, std::size_t lenBase64);
	/*****************************************************************************************
		<< --- convert::replace		2014-07-24 --- >>
		说明：替换字符串中指定的字符
		参数：
		wsdata	= 被操作的数据
		chScr	= 被替换的字符
		chDest	= 新的字符
		返回值：
	*****************************************************************************************/
	static void replace(std::wstring &wsdata, wchar_t chScr, wchar_t chDest);

	/*****************************************************************************************
		<< --- convert::deleteChar		2014-07-24 --- >>
		说明：删除字符串中指定的字符
		参数：
		wsdata	= 被操作的数据
		chDel	= 被删除的字符
		返回值：
	*****************************************************************************************/
	static std::wstring deleteChar(std::wstring wsdata, wchar_t chDel);

	/*****************************************************************************************
		<< --- convert::deleteNoGraph		2014-09-10 --- >>
		说明：删除字符串中的非可见字符(不包括空格字符)
		参数：
		wsdata	= 被操作的数据
		返回值：
	*****************************************************************************************/
	static std::wstring deleteNoGraph(std::wstring wsdata);
	/*****************************************************************************************
		<< --- convert::toInt32		2013-11-22 --- >>
		说明：将uint16数据转换成int32值
		参数：
		psource		= 被转化的数据源
		beginIndex	= 被转换数据开始索引
		firstHigh	= true:高字节在数组的低位
		返回值：被转换成的值
	*****************************************************************************************/
	static int32_t toInt32(uint16_t *psource, int32_t beginIndex = 0, bool firstHigh = true)
	{
		int32_t val = 0;
		if(!psource || beginIndex < 0)
			return 0;
		try
		{
			if(firstHigh)
				val = (psource[beginIndex] << 16) + psource[beginIndex+1];
			else
				val = psource[beginIndex] + (psource[beginIndex+1] << 16);
		}
		catch(...)
		{
			val = 0;
		}
		return val;
	}
	/*****************************************************************************************
		<< --- static convert::toLower	2014-06-08 --- >>
		说明：将字符串转换成大写的
		参数：
		ws	= 被转换的字符串
		返回值：
	*****************************************************************************************/
	static void toUpper(std::wstring &ws)
	{
		for (std::wstring::iterator iter = ws.begin(); iter != ws.end(); ++iter)
		{
			//if(*iter <= 0 || *iter >= 256) continue;
			if (std::iswalpha(*iter) && std::iswlower(*iter))
			{
				*iter = std::towupper(*iter);
			}
		}
	}
	static bool toUpper(PCWSTR pstr, PWSTR *ppUpper);
	static std::wstring toUpper(PCWSTR pstr)
	{
		assert(pstr);
		std::wstring wstr = pstr;
		toUpper(wstr);
		return wstr;
	}
	static void toUpper(std::string &ss)
	{
		for (std::string::iterator iter = ss.begin(); iter != ss.end(); ++iter)
		{
			//if(*iter <= 0 || *iter >= 256) continue;
			if (std::isalpha(*iter) && std::islower(*iter))
			{
				*iter = std::toupper(*iter);
			}
		}
	}
	static bool toUpper(PCSTR pstr, PSTR *ppUpper);
	static std::string toUpper(PCSTR pstr)
	{
		assert(pstr);
		std::string str = pstr;
		toUpper(str);
		return str;
	}
	/*****************************************************************************************
		<< --- static convert::toLower	2014-06-08 --- >>
		说明：将字符串转换成小写的
		参数：
		ws	= 被转换的字符串
		ppLower	= 转换后的字符串，调用者负责是否
		返回值：
	*****************************************************************************************/
	static void toLower(std::wstring &ws)
	{
		for (std::wstring::iterator iter = ws.begin(); iter != ws.end(); ++iter)
		{
			//if(*iter <= 0 || *iter >= 256) continue;
			if (std::iswalpha(*iter) && std::iswupper(*iter))
			{
				*iter = std::towlower(*iter);
			}
		}
	}
	static std::wstring toLower(PCWSTR pstr)
	{
		assert(pstr);
		std::wstring wstr = pstr;
		toLower(wstr);
		return wstr;
	}
	static bool toLower(PCWSTR pstr, PWSTR *ppLower);
	static void toLower(std::string &ss)
	{
		for (std::string::iterator iter = ss.begin(); iter != ss.end(); ++iter)
		{
			//if(*iter <= 0 || *iter >= 256) continue;
			if (std::isalpha(*iter) && std::isupper(*iter))
			{
				*iter = std::tolower(*iter);
			}
		}
	}
	static bool toLower(PCSTR pstr, PSTR *ppLower);
	static std::string toLower(PCSTR pstr)
	{
		assert(pstr);
		std::string str = pstr;
		toLower(str);
		return str;
	}
	/*****************************************************************************************
		<< --- trimLeft  2014-10-18 --- >>
		说明：去除字符串开头空格
	*****************************************************************************************/
	static void trimLeft(std::wstring &wstr);
	static std::wstring trimLeft(PCWSTR pstr);
	static void trimLeft(std::string &str);
	static std::string trimLeft(PCSTR pstr);
	/*****************************************************************************************
		<< --- trimRight  2014-10-18 --- >>
		说明：去除字符串尾端空格
	*****************************************************************************************/
	static void trimRight(std::wstring &wstr);
	static std::wstring trimRight(PCWSTR pstr);
	static void trimRight(std::string &str);
	static std::string trimRight(PCSTR pstr);
	/*****************************************************************************************
		<< --- trim  2014-10-18 --- >>
		说明：去除字符串首尾端空格
	*****************************************************************************************/
	static void trim(std::wstring &wstr);
	static std::wstring trim(PCWSTR pstr);
	static void trim(std::string &str);
	static std::string trim(PCSTR pstr);


	/*****************************************************************************************
		<< --- static convert::splitString	2014-04-30 --- >>
		说明：分割字符串
		参数：
		csSrc		= 被分割的字符
		vecDest		= 存放分割后的数据
		cSeparator	= 分割标记
		返回值：转换后的字符串
	*****************************************************************************************/
	static bool splitString(const std::wstring &csSrc, std::vector<std::wstring> &vecDest, wchar_t cSeparator)
	{
		if(csSrc.empty())
			return false;
		vecDest.clear();
		std::wstring::size_type size_pos = 0;
		std::wstring::size_type size_prev_pos = 0;

		while( (size_pos = csSrc.find_first_of( cSeparator , size_pos)) != std::wstring::npos)
		{
			std::wstring strTemp = csSrc.substr(size_prev_pos , size_pos - size_prev_pos );

			vecDest.push_back(strTemp);
			size_prev_pos = ++size_pos;
			if(size_prev_pos > csSrc.length()) size_prev_pos = csSrc.length();
		}
		if(size_prev_pos < csSrc.length())
			vecDest.push_back(&csSrc[size_prev_pos]);

		return !vecDest.empty();
	}
	static bool splitString(const std::string &csSrc, std::vector<std::string> &vecDest, char cSeparator)
	{
		if(csSrc.empty())
			return false;
		vecDest.clear();
		std::string::size_type size_pos = 0;
		std::string::size_type size_prev_pos = 0;

		while( (size_pos = csSrc.find_first_of( cSeparator , size_pos)) != std::string::npos)
		{
			std::string strTemp = csSrc.substr(size_prev_pos , size_pos - size_prev_pos );

			vecDest.push_back(strTemp);
			size_prev_pos = ++size_pos;
			if(size_prev_pos > csSrc.length()) size_prev_pos = csSrc.length();
		}
		if(size_prev_pos < csSrc.length())
			vecDest.push_back(&csSrc[size_prev_pos]);

		return !vecDest.empty();
	}
	/*****************************************************************************************
		<< --- static convert::toWChar	2014-04-25 --- >>
		说明：将单字符转换成宽字符串(UTF-8未测试)
		参数：
		pdata		= 被转换的字符
		isAnsi	= true:被转换的字符为ANSI码；false:否则为UTF-8
		返回值：转换后的字符串
	*****************************************************************************************/
	static bool toWChar(const char *pdata, wchar_t **pbufWChar, bool isAnsi = true)
	{
		assert(pdata && pbufWChar && std::strlen(pdata) > 0);
		if(!pdata || !pbufWChar || std::strlen(pdata) <= 0)
			return false;
		wchar_t *buf = nullptr;
#if defined(_WIN32)
		int len = ::MultiByteToWideChar(isAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, NULL, 0);
		if(len > 0)
		{
			buf = new wchar_t[len+1];
			memset(buf, 0, (len+1)*sizeof(wchar_t));
			len = ::MultiByteToWideChar(isAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, buf, len);
		}
#else
		if(isAnsi)
			std::setlocale(LC_CTYPE, "");
		else
			std::setlocale(LC_CTYPE, "zh_CN.utf8");
		std::size_t len = std::mbsrtowcs(nullptr, (const char**)&pdata, 0, nullptr);
		if(len > 0)
		{
			buf = new wchar_t[len+1];
			memset(buf, 0, (len+1)*sizeof(wchar_t));
			len = std::mbsrtowcs(buf, (const char**)&pdata, len, nullptr);
		}
#endif
		if(buf)
		{
			*pbufWChar = buf;
		}
		return *pbufWChar != NULL;
	}
	static std::wstring toWChar(const char *pdata, bool isAnsi = true)
	{
		std::wstring wstr;
		wchar_t *pbuf = NULL;
		if (toWChar(pdata, &pbuf, isAnsi))
		{
			if(pbuf)
				wstr = pbuf;
		}
		if(pbuf) delete[] pbuf;
		return wstr;
	}
	/*****************************************************************************************
		<< --- static convert::toWChar	2014-04-25 --- >>
		说明：将单字符转换成单字节字符串(UTF-8未测试)
		参数：
		pdata	= 被转换的字符
		toAnsi	= true:字符串转化为ANSI码；false:否则为UTF-8
		pbufChar= 转换后的字符串，调用者负责释放
		返回值：转换后的字符串
	*****************************************************************************************/
	static bool toChar(const wchar_t *pdata, char **pbufChar, bool toAnsi = true)
	{
		assert(pdata != nullptr && std::wcslen(pdata) > 0 && pbufChar != NULL);
		if(!pdata || !pbufChar || std::wcslen(pdata) <= 0) return false;
		char *buf = NULL;
#if defined(_WIN32)
		int len = ::WideCharToMultiByte(toAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, NULL,0, NULL, NULL);
		if(len > 0)
		{
			buf = new char[len+1];
			memset(buf, 0, (len+1)*sizeof(char));
			len = ::WideCharToMultiByte(toAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, buf, len, NULL, NULL);
		}
#else
		if(toAnsi)
			std::setlocale(LC_CTYPE, "");
		else
			std::setlocale(LC_CTYPE, "zh_CN.utf8");
		std::size_t len = std::wcsrtombs(nullptr, (const wchar_t**)&pdata, 0, NULL);
		if(len > 0)
		{
			buf = new char[len+1];
			memset(buf, 0, (len+1)*sizeof(char));
			len = std::wcsrtombs(buf, (const wchar_t**)&pdata, len, NULL);
		}
#endif
		if(buf)
		{
			*pbufChar = buf;
		}
		return *pbufChar != NULL;
	}
	static std::string toChar(const wchar_t *pdata, bool toAnsi = true)
	{
		assert(pdata != nullptr && std::wcslen(pdata) > 0);
		std::string cstr;
		char *buf = NULL;
		if (toChar(pdata, &buf, toAnsi))
		{
			if(buf)
				cstr = buf;
		}
		if(buf) delete[] buf;
		return cstr;
//		assert(pdata != nullptr && std::wcslen(pdata) > 0);
//		std::string cstr;
//		char *buf = nullptr;
//#if defined(_WIN32)
//		int len = ::WideCharToMultiByte(toAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, NULL,0, NULL, NULL);
//		if(len > 0)
//		{
//			buf = new char[len+1];
//			memset(buf, 0, (len+1)*sizeof(char));
//			len = ::WideCharToMultiByte(toAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, buf, len, NULL, NULL);
//		}
//#else
//		if(toAnsi)
//			std::setlocale(LC_CTYPE, "");
//		else
//			std::setlocale(LC_CTYPE, "zh_CN.utf8");
//		std::size_t len = std::wcsrtombs(nullptr, (const wchar_t**)&pdata, 0, nullptr);
//		if(len > 0)
//		{
//			buf = new char[len+1];
//			memset(buf, 0, (len+1)*sizeof(char));
//			len = std::wcsrtombs(buf, (const wchar_t**)&pdata, len, nullptr);
//		}
//#endif
//		if(buf)
//		{
//			cstr = buf;
//			delete buf;
//		}
//		return cstr;
	}

	/*****************************************************************************************
		<< --- static convert::toStringHex	2014-04-30 --- >>
		说明：将字符串以十六进制呈现
		参数：
		pdata	= 被转换的字符
		dataLen	= 被转换字符的长度
		bUpper	= 转化结果为大写还是小写
		返回值：转换后的字符串
	*****************************************************************************************/
	static std::string toStringHexA(const char *pdata, int32_t dataLen, bool bUpper = true)
	{
		assert(pdata && dataLen > 0);
		std::string str;
		char buf[3] = {0};
		for(int32_t idx = 0; idx < dataLen; ++idx)
		{
			if (bUpper)
				std::sprintf(buf, "%02X", (uint8_t)pdata[idx]);
			else
				std::sprintf(buf, "%02x", (uint8_t)pdata[idx]);
			buf[2] = '\0';
			str += buf;
		}
		return str;
	}
	static std::string toStringHexA(const SCDataType &data, bool bUpper = true)
	{
		return toStringHexA(data.data(), data.size(), bUpper);
	}
	static std::wstring toStringHex(const char *pdata, int32_t dataLen, bool bUpper = true);
	static std::wstring toStringHex(const SCDataType &data, bool bUpper = true)
	{
		return toStringHex(data.data(), data.size(), bUpper);
	}

	/*****************************************************************************************
		<< --- static convert::toIP	2014-05-07 --- >>
		说明：将字符型IP转换成数值型
		参数：
		pip	= 被转换的IP
		返回值：转换后的字符串
	*****************************************************************************************/
	static uint32_t toIP(const wchar_t *pip)
	{
		uint32_t ip = 0;
		std::vector<std::wstring> vec;
		if(splitString(pip, vec, L'.') && vec.size() == 4)
		{
			uint8_t b1, b2, b3, b4;
			b1 = (uint8_t)std::wcstol(vec[0].c_str(), nullptr, 10);
			b2 = (uint8_t)std::wcstol(vec[1].c_str(), nullptr, 10);
			b3 = (uint8_t)std::wcstol(vec[2].c_str(), nullptr, 10);
			b4 = (uint8_t)std::wcstol(vec[3].c_str(), nullptr, 10);
			ip = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
		}
		return ip;
	}
	static std::wstring toIP(const uint32_t	ip)
	{
		wchar_t sip[40] = {0};
		std::swprintf(sip, 40, L"%d.%d.%d.%d", (ip & 0x0FF000000)>>24, (ip & 0x0FF0000)>>16, (ip & 0x0FF00)>>8, (ip & 0x0FF));
		return sip;
	}

#if defined(_WIN32)
	/****************************************************************************************
		<< --- toTimet		2014-11-10 --- >>         
		说明：将文件时间转换到时间1970年1月1日00:00:00（称为UNIX系统的Epoch时间）到当前时刻的秒数
		参数：
			fileTime	= 文件时间。表示自1601年1月1日开始的100纳秒为单位的时间
		返回值：转换后的时间
	*****************************************************************************************/
	static std::time_t toTimet(const FILETIME &fileTime);
#endif
#if defined(_AFXDLL)
	/*****************************************************************************************
		<< --- static convert::getSize	2014-06-08 --- >>
		说明：得到数据所占字节数
		参数：
		vval	= 被求值的数据
		返回值：数据所占字节数
	*****************************************************************************************/
	static std::size_t getSize(const COleVariant &vval)
	{
		std::size_t sz = 0;
		switch(vval.vt)
		{
		case VT_UI1:
		case VT_I1:
		case VT_BOOL:
			sz = 1;
			break;
		case VT_I2:
		case VT_UI2:
			sz = 2;
			break;
		case VT_I4:
		case VT_UI4:
		case VT_R4:
		case VT_INT:
		case VT_UINT:
			sz = 4;
			break;
		case VT_I8:
		case VT_UI8:
		case VT_R8:
			sz = 8;
			break;
		case VT_DATE:
			sz = 8;
			break;
		case VT_BSTR:
			sz = ::SysStringByteLen(vval.bstrVal);
			break;
		case VT_ARRAY | VT_UI1:
			sz = vval.parray->rgsabound->cElements;
			break;
		default:
			sz = 0;
			break;
		}
		return sz;
	}
	/*****************************************************************************************
		<< --- static convert::toData	2014-06-08 --- >>
		说明：将COleVariant转换到字节流数据
		参数：
		varData	= 被转换的数据
		返回值：转换后的数据
	*****************************************************************************************/
	static SBDataType toData(const COleVariant &varData)
	{
		if(varData.vt != (VT_UI1 | VT_ARRAY))
			return SBDataType();
		uint8_t *pbuf = nullptr;
		std::size_t len = getSize(varData);
		SafeArrayAccessData(varData.parray,(void **)&pbuf);
		SBDataType vec(pbuf, pbuf+len);
		SafeArrayUnaccessData(varData.parray);
		return vec;
	}
	/*****************************************************************************************
		<< --- static convert::toVariant	2014-06-08 --- >>
		说明：将字节流数据转换到COleVariant
		参数：
		toVar	= 被转换后保存的数据格式
		vecData	= 被转换的数据
		返回值：
	*****************************************************************************************/
	static void toVariant(COleVariant &toVar, const SBDataType &vecData)
	{
		toVariant(toVar, vecData.data(), vecData.size());
		//toVar.Clear();
		//toVar.ChangeType(VT_NULL);
		//toVar.scode = DISP_E_PARAMNOTFOUND;
		//if(vecData.size() > 0)
		//{
		//	COleSafeArray safe;
		//	safe.CreateOneDim(VT_UI1, vecData.size(), vecData.data());
		//	toVar = safe;
		//}
	}
	/*****************************************************************************************
		<< --- static convert::toVariant		2014-03-21 --- >>
		说明：将内存缓冲区内的数据转换成COleVariant类型VT_UI1 | VT_ARRAY
		参数：
		toVar		= 转换后的数据
		pdata		= 被转换数据
		dataLen		= 缓冲区大小（字节）
		返回值：
	*****************************************************************************************/
	static void toVariant(COleVariant &toVar, const uint8_t *pdata, int32_t dataLen)
	{
		toVar.Clear();
		toVar.ChangeType(VT_NULL);
		toVar.scode = DISP_E_PARAMNOTFOUND;
		if(!pdata && dataLen > 0)
		{
			COleSafeArray safe;
			safe.CreateOneDim(VT_UI1, dataLen, pdata);
			toVar = safe;
		}
	}

	/*****************************************************************************************
		<< --- toNull  2014-10-19 --- >>
		说明：将变体类型转换到数据库空类型
		参数：
		var		= 被转换的变体类型
		返回值：
	*****************************************************************************************/
	static void toNull(COleVariant &var)
	{
		var.Clear();
		var.ChangeType(VT_NULL);
		var.scode = DISP_E_PARAMNOTFOUND;
	}
	static void toNull(_variant_t &var)
	{
		var.Clear();
		var.ChangeType(VT_NULL);
		var.scode = DISP_E_PARAMNOTFOUND;
	}
	// 将缇(twip)单位转换成像素
	static int32_t toPixelXFromTwip(int32_t vtwip) {
		CDC dc;
		dc.Attach(GetDC(NULL));
		int32_t px = dc.GetDeviceCaps(LOGPIXELSX)*vtwip/1440;

		dc.Detach();
		return px;
	}
	static int32_t toPixelYFromTwip(int32_t vtwip) {
		CDC dc;
		dc.Attach(GetDC(NULL));
		int32_t py = dc.GetDeviceCaps(LOGPIXELSY)*vtwip/1440;

		dc.Detach();
		return py;
	}
	// 像素单位转换成将缇(twip)
	static int32_t toTwipXFromPixel(int32_t vpixel) {
		CDC dc;
		dc.Attach(GetDC(NULL));
		int32_t px = 1440*vpixel/dc.GetDeviceCaps(LOGPIXELSX);

		dc.Detach();
		return px;
	}
	static int32_t toTwipYFromPixel(int32_t vpixel) {
		CDC dc;
		dc.Attach(GetDC(NULL));
		int32_t py = 1440*vpixel/dc.GetDeviceCaps(LOGPIXELSY);

		dc.Detach();
		return py;
	}
#endif
};

}	// gtc
#endif	// __CONVERTER_H__