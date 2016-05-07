#ifndef __CURLWRAP_H__
#define __CURLWRAP_H__

/*****************************************************************************************
 *	项目	针对libcurl库进行的封装
 *
 *	2016-1-19		kp007		
 *	1、要申明预处理器定义BUILDING_LIBCURL宏 !!!!!!!!!!!!!!!!!!!!!!!!
 *	2、需要的依赖库ws2_32.lib wldap32.lib libeay32.lib ssleay32.lib
 *	
 *	
 *	
 *****************************************************************************************/
#pragma once
#include <string>
#include "AC/baseException.h"
#include "AC/basedef.h"
#include "AC/stringUtil.h"
#include "curl/curl.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wldap32.lib")
//#pragma comment(lib,"D:\\NTools\\openssl\\libeay32.lib")
//#pragma comment(lib,"D:\\NTools\\openssl\\ssleay32.lib")
#if defined(_DEBUG)
//#	pragma comment(lib,"D:\\NTools\\curl\\libcurl_ssl_d.lib")
#	pragma comment(lib,"D:\\NTools\\curl\\libcurl_d.lib")

#else
//#	pragma comment(lib,"D:\\NTools\\curl\\libcurl_ssl.lib")
#	pragma comment(lib,"D:\\NTools\\curl\\libcurl.lib")
#endif
// 需要在“项目属性\C/C++\预处理”加入BUILDING_LIBCURL
//#ifndef BUILDING_LIBCURL
//#define BUILDING_LIBCURL
//#endif
namespace gtc {

#define  CURL_ERRUSER_NORMAL		CURL_LAST+1000	// 自定义错误码
static std::wstring _errorInfoCurl(int errCode);

#define CURL_EXCEPT(code) GTC_EXCEPT_EX(gtc::baseException::BSExceptionCodeCurlAssertionFailed, code, _errorInfoCurl(code))

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-9-14 要post的数据格式
enum BEPostDataFlag {
	BEPostDataFlagNormal		= 0x0001,		// 常规格式：CURLFORM_PTRNAME, xxx, CURLFORM_PTRCONTENTS, yyy
	BEPostDataFlagData			= 0x0101,		// 二进制数据：CURLFORM_PTRNAME，，CURLFORM_BUFFER，，CURLFORM_BUFFERPTR，，CURLFORM_BUFFERLENGTH
	BEPostDataFlagFile			= 0x0201		// 文件：CURLFORM_PTRNAME，，CURLFORM_FILE
};

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-9-14 要post的数据
struct BSPostDataItem {
	BEPostDataFlag	dataFlag;		// 格式
	std::string		strName;		// 名称
	std::string		strData;		// 数据：普通文本 或文件路径 或二进制数据

	BSPostDataItem(const std::string &sname, const std::string &sdata, BEPostDataFlag flag = BEPostDataFlagNormal)
		: strName(sname), strData(sdata), dataFlag(flag)
	{}
	BSPostDataItem(const std::string &sname, const std::wstring &wsdata, BEPostDataFlag flag = BEPostDataFlagNormal)
		: strName(sname), strData(gtc::stringUtil::toChar(wsdata.c_str())), dataFlag(flag)
	{}
	BSPostDataItem(const std::string &sname, int idata, BEPostDataFlag flag = BEPostDataFlagNormal)
		: strName(sname), dataFlag(flag)
	{
		char pbuf[80] = {0};
		std::sprintf(pbuf, "%d", idata);
		strData = pbuf;
	}
	BSPostDataItem(const std::string &sname, const SBDataType &sdata)
		: strName(sname), dataFlag(BEPostDataFlagData)
	{
		strData.assign(sdata.cbegin(), sdata.cend());
	}
};
typedef std::vector<BSPostDataItem> PostDataArrayType;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	curl  类		2015-7-7
//	基于libcurl的封装类
//	1、属性中加预定义宏BUILDING_LIBCURL
//	2、属性中加预定义库文件ws2_32.lib、winmm.lib、wldap32.lib
////////////////////////////////////////////////////////////////////////////////////////////////////////
class curl
{
protected:
	class curl_ptr {
	public:
		curl_ptr() : _pcurl(NULL)
		{
			_pcurl = curl_easy_init();
			assert(_pcurl);
		}
		~curl_ptr()
		{
			if (_pcurl)
			{
				curl_easy_cleanup(_pcurl);
			}
		}
		operator CURL*() const 
		{
			return _pcurl;
		}
		operator bool() const 
		{
			return _pcurl != NULL;
		}
		CURL* get()
		{
			assert(_pcurl);
			return _pcurl;
		}
	private:
		curl_ptr(const curl_ptr &);
		curl_ptr& operator=(const curl_ptr &);
	private:
		CURL *_pcurl;
	};
public:
	curl();
	~curl();

public:
	// 初始化libcurl环境
	static void initEnvironment(long flags = CURL_GLOBAL_ALL);
	// 清空libcurl环境
	static void clearupEnvironment();

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-7 下载网页
	static std::string downloadWeb(const std::string &webUrl, int timeout = 0);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-9 下载文件
	static SBDataType downloadFile(const std::string &fileUrl, int timeout = 0);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-9-7 post
	//static std::string post(const std::string &webUrl, const std::string &fields);
	static std::string post(const std::string &webUrl, const std::string &fields, int timeout = 0, PCSTR pCAinfo = NULL);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-9-11 post 采用multipart/formdata请求	
	//////////////////////////////////////////////////////////////////////////////////////////
	static std::string post(const std::string &webUrl, const PostDataArrayType &postData, int timeout = 0, PCSTR pCAinfo = NULL);
	//	2015-9-8 get
	static std::string get(const std::string &webUrl);
protected:
	//static std::string mc_wrWebBuffer;		// downloadWeb中WRITEDATA对应缓存
	//static std::string mc_wrFileBuffer;		// downloadFile中WRITEDATA对应缓存
	//static std::string mc_rddataBuffer;		// READDATA对应缓存
};
}
#endif	// __CURLWRAP_H__