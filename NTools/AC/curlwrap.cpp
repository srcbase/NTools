#include "curlwrap.h"
#include "AC\stringUtil.h"
#include <fstream>
namespace gtc {

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-7-9 libcurl错误码
static std::wstring _errorInfoCurl(int errCode)
{
	switch (errCode)
	{
	case CURLE_UNSUPPORTED_PROTOCOL:    return L"1 CURLE_UNSUPPORTED_PROTOCOL";
	case CURLE_FAILED_INIT:             return L"2 CURLE_FAILED_INIT";
	case CURLE_URL_MALFORMAT:           return L"3 CURLE_URL_MALFORMAT";
	case CURLE_NOT_BUILT_IN:            return L"4 - [was obsoleted in August 2007 for "
                                    L"7.17.0, reused in April 2011 for 7.21.5] ";
	case CURLE_COULDNT_RESOLVE_PROXY:   return L"5 CURLE_COULDNT_RESOLVE_PROXY";
	case CURLE_COULDNT_RESOLVE_HOST:    return L"6 CURLE_COULDNT_RESOLVE_HOST";
	case CURLE_COULDNT_CONNECT:         return L"7 CURLE_COULDNT_CONNECT";
	case CURLE_FTP_WEIRD_SERVER_REPLY:  return L"8 CURLE_FTP_WEIRD_SERVER_REPLY";
	case CURLE_REMOTE_ACCESS_DENIED:    return L"9 a service was denied by the server "
                                    L"due to lack of access - when login fails "
                                    L"this is not returned. ";
	case CURLE_FTP_ACCEPT_FAILED:       return L"10 - [was obsoleted in April 2006 for "
                                    L"7.15.4, reused in Dec 2011 for 7.24.0]";
	case CURLE_FTP_WEIRD_PASS_REPLY:    return L"11 CURLE_FTP_WEIRD_PASS_REPLY";
	case CURLE_FTP_ACCEPT_TIMEOUT:      return L"12 - timeout occurred accepting server "
                                    L"[was obsoleted in August 2007 for 7.17.0, "
                                    L"reused in Dec 2011 for 7.24.0]";
	case CURLE_FTP_WEIRD_PASV_REPLY:    return L"13 CURLE_FTP_WEIRD_PASV_REPLY";
	case CURLE_FTP_WEIRD_227_FORMAT:    return L"14 CURLE_FTP_WEIRD_227_FORMAT";
	case CURLE_FTP_CANT_GET_HOST:       return L"15 CURLE_FTP_CANT_GET_HOST";
	case CURLE_HTTP2:                   return L"16 - A problem in the http2 framing layer. "
                                    L"[was obsoleted in August 2007 for 7.17.0, "
                                    L"reused in July 2014 for 7.38.0] ";
	case CURLE_FTP_COULDNT_SET_TYPE:    return L"17 CURLE_FTP_COULDNT_SET_TYPE";
	case CURLE_PARTIAL_FILE:            return L"18 CURLE_PARTIAL_FILE";
	case CURLE_FTP_COULDNT_RETR_FILE:   return L"19 CURLE_FTP_COULDNT_RETR_FILE";
	case CURLE_OBSOLETE20:              return L"20 - NOT USED ";
	case CURLE_QUOTE_ERROR:             return L"21 - quote command failure ";
	case CURLE_HTTP_RETURNED_ERROR:     return L"22 CURLE_HTTP_RETURNED_ERROR";
	case CURLE_WRITE_ERROR:             return L"23 CURLE_WRITE_ERROR";
	case CURLE_OBSOLETE24:              return L"24 - NOT USED ";
	case CURLE_UPLOAD_FAILED:           return L"25 - failed upload \"command\" ";
	case CURLE_READ_ERROR:              return L"26 - couldn't open/read from file ";
	case CURLE_OUT_OF_MEMORY:           return L"27 CURLE_OUT_OF_MEMORY";
  /* Note: CURLE_OUT_OF_MEMORY may sometimes indicate a conversion error
           instead of a memory allocation error if CURL_DOES_CONVERSIONS
           is defined
  */
	case CURLE_OPERATION_TIMEDOUT:      return L"28 - the timeout time was reached ";
	case CURLE_OBSOLETE29:              return L"29 - NOT USED ";
	case CURLE_FTP_PORT_FAILED:         return L"30 - FTP PORT operation failed ";
	case CURLE_FTP_COULDNT_USE_REST:    return L"31 - the REST command failed ";
	case CURLE_OBSOLETE32:              return L"32 - NOT USED ";
	case CURLE_RANGE_ERROR:             return L"33 - RANGE \"command\" didn't work ";
	case CURLE_HTTP_POST_ERROR:         return L"34 CURLE_HTTP_POST_ERROR";
	case CURLE_SSL_CONNECT_ERROR:       return L"35 - wrong when connecting with SSL ";
	case CURLE_BAD_DOWNLOAD_RESUME:     return L"36 - couldn't resume download ";
	case CURLE_FILE_COULDNT_READ_FILE:  return L"37 CURLE_FILE_COULDNT_READ_FILE";
	case CURLE_LDAP_CANNOT_BIND:        return L"38 CURLE_LDAP_CANNOT_BIND";
	case CURLE_LDAP_SEARCH_FAILED:      return L"39 CURLE_LDAP_SEARCH_FAILED";
	case CURLE_OBSOLETE40:              return L"40 - NOT USED ";
	case CURLE_FUNCTION_NOT_FOUND:      return L"41 CURLE_FUNCTION_NOT_FOUND";
	case CURLE_ABORTED_BY_CALLBACK:     return L"42 CURLE_ABORTED_BY_CALLBACK";
	case CURLE_BAD_FUNCTION_ARGUMENT:   return L"43 CURLE_BAD_FUNCTION_ARGUMENT";
	case CURLE_OBSOLETE44:              return L"44 - NOT USED ";
	case CURLE_INTERFACE_FAILED:        return L"45 - CURLOPT_INTERFACE failed ";
	case CURLE_OBSOLETE46:              return L"46 - NOT USED ";
	case CURLE_TOO_MANY_REDIRECTS :     return L"47 - catch endless re-direct loops ";
	case CURLE_UNKNOWN_OPTION:          return L"48 - User specified an unknown option ";
	case CURLE_TELNET_OPTION_SYNTAX :   return L"49 - Malformed telnet option ";
	case CURLE_OBSOLETE50:              return L"50 - NOT USED ";
	case CURLE_PEER_FAILED_VERIFICATION: return L"51 - peer's certificate or fingerprint "
                                    L"wasn't verified fine ";
	case CURLE_GOT_NOTHING:             return L"52 - when this is a specific error ";
	case CURLE_SSL_ENGINE_NOTFOUND:     return L"53 - SSL crypto engine not found ";
	case CURLE_SSL_ENGINE_SETFAILED:    return L"54 - can not set SSL crypto engine as "
                                    L"default ";
	case CURLE_SEND_ERROR:              return L"55 - failed sending network data ";
	case CURLE_RECV_ERROR:              return L"56 - failure in receiving network data ";
	case CURLE_OBSOLETE57:              return L"57 - NOT IN USE ";
	case CURLE_SSL_CERTPROBLEM:         return L"58 - problem with the local certificate ";
	case CURLE_SSL_CIPHER:              return L"59 - couldn't use specified cipher ";
	case CURLE_SSL_CACERT:              return L"60 - problem with the CA cert (path?) ";
	case CURLE_BAD_CONTENT_ENCODING:    return L"61 - Unrecognized/bad encoding ";
	case CURLE_LDAP_INVALID_URL:        return L"62 - Invalid LDAP URL ";
	case CURLE_FILESIZE_EXCEEDED:       return L"63 - Maximum file size exceeded ";
	case CURLE_USE_SSL_FAILED:          return L"64 - Requested FTP SSL level failed ";
	case CURLE_SEND_FAIL_REWIND:        return L"65 - Sending the data requires a rewind "
                                    L"that failed ";
	case CURLE_SSL_ENGINE_INITFAILED:   return L"66 - failed to initialise ENGINE ";
	case CURLE_LOGIN_DENIED:            return L"67 - user, password or similar was not "
                                    L"accepted and we failed to login ";
	case CURLE_TFTP_NOTFOUND:           return L"68 - file not found on server ";
	case CURLE_TFTP_PERM:               return L"69 - permission problem on server ";
	case CURLE_REMOTE_DISK_FULL:        return L"70 - out of disk space on server ";
	case CURLE_TFTP_ILLEGAL:            return L"71 - Illegal TFTP operation ";
	case CURLE_TFTP_UNKNOWNID:          return L"72 - Unknown transfer ID ";
	case CURLE_REMOTE_FILE_EXISTS:      return L"73 - File already exists ";
	case CURLE_TFTP_NOSUCHUSER:         return L"74 - No such user ";
	case CURLE_CONV_FAILED:             return L"75 - conversion failed ";
	case CURLE_CONV_REQD:               return L"76 - caller must register conversion "
                                    L"callbacks using curl_easy_setopt options "
                                    L"CURLOPT_CONV_FROM_NETWORK_FUNCTION, "
                                    L"CURLOPT_CONV_TO_NETWORK_FUNCTION, and "
                                    L"CURLOPT_CONV_FROM_UTF8_FUNCTION ";
	case CURLE_SSL_CACERT_BADFILE:      return L"77 - could not load CACERT file, missing "
                                    L"or wrong format ";
	case CURLE_REMOTE_FILE_NOT_FOUND:   return L"78 - remote file not found ";
	case CURLE_SSH:                     return L"79 - error from the SSH layer, somewhat "
                                    L"generic so the error message will be of "
                                    L"interest when this has happened ";

	case CURLE_SSL_SHUTDOWN_FAILED:     return L"80 - Failed to shut down the SSL "
                                    L"connection ";
	case CURLE_AGAIN:                   return L"81 - socket is not ready for send/recv, "
                                    L"wait till it's ready and try again (Added "
                                    L"in 7.18.2) ";
	case CURLE_SSL_CRL_BADFILE:         return L"82 - could not load CRL file, missing or "
                                    L"wrong format (Added in 7.19.0) ";
	case CURLE_SSL_ISSUER_ERROR:        return L"83 - Issuer check failed.  (Added in "
                                    L"7.19.0) ";
	case CURLE_FTP_PRET_FAILED:         return L"84 - a PRET command failed ";
	case CURLE_RTSP_CSEQ_ERROR:         return L"85 - mismatch of RTSP CSeq numbers ";
	case CURLE_RTSP_SESSION_ERROR:      return L"86 - mismatch of RTSP Session Ids ";
	case CURLE_FTP_BAD_FILE_LIST:       return L"87 - unable to parse FTP file list ";
	case CURLE_CHUNK_FAILED:            return L"88 - chunk callback reported error ";
	case CURLE_NO_CONNECTION_AVAILABLE: return L"89 - No connection available, the "
                                    L"session will be queued ";
	case CURLE_SSL_PINNEDPUBKEYNOTMATCH: return L"90 - specified pinned public key did not "
                                    L"match ";
	case CURLE_SSL_INVALIDCERTSTATUS:   return L"91 - invalid certificate status ";
	case CURL_ERRUSER_NORMAL:			return L"自定义";
	}
	return L"CURL_UNKNOWN_ERROR";
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	curl  类		2015-7-7
//	基于libcurl的封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
//std::string curl::mc_wrWebBuffer;		// downloadWeb中WRITEDATA对应缓存
//std::string curl::mc_wrFileBuffer;		// downloadFile中WRITEDATA对应缓存
//std::string curl::mc_rddataBuffer;		// READDATA对应缓存
static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	//static int idx = 0;
	if (writerData == NULL)
		return 0;

	std::size_t len = size * nmemb;
	writerData->append(data, len);

	return len;
}
static int writerData(uint8_t *data, size_t size, size_t nmemb, SBDataType *writerData)
{
	//static int idx = 0;
	if (writerData == NULL)
		return 0;

	std::size_t len = size * nmemb;
	writerData->insert(writerData->end(), data, data + len);
	
	return len;
}
curl::curl()
{

}
curl::~curl()
{

}

void curl::initEnvironment(long flags /*= CURL_GLOBAL_ALL*/)
{
	curl_global_init(flags);
}
void curl::clearupEnvironment()
{
	curl_global_cleanup();
	//mc_wrWebBuffer.clear();
	//mc_wrFileBuffer.clear();
	//mc_rddataBuffer.clear();
}


std::string curl::downloadWeb(const std::string &webUrl, int timeout /*= 0*/)
{
	assert(!webUrl.empty());
	if(webUrl.empty()) return "";
	//CURL *pcurl = NULL;
	curl_ptr pcurl;
	CURLcode code;
	std::string strBuffer;
	//mc_wrWebBuffer.clear();
	//pcurl = curl_easy_init();
	if (pcurl)
	{
		code = curl_easy_setopt(pcurl, CURLOPT_URL, webUrl.c_str());
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, writer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, &strBuffer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		if (timeout > 0)
		{
			code = curl_easy_setopt(pcurl, CURLOPT_TIMEOUT, timeout);        //设置超时
			if(code != CURLE_OK) CURL_EXCEPT(code);
		}
		code = curl_easy_perform(pcurl);
		if(code != CURLE_OK)
		{
			strBuffer.clear();
			CURL_EXCEPT(code);
		}
		//if(pcurl) curl_easy_cleanup(pcurl);
	}
	return strBuffer;
	//if(strBuffer.empty()) return L"";
	//return gtc::stringUtil::toWChar(strBuffer.c_str(), CP_UTF8);
}
//static bool writeFile(const wchar_t *pfullfile, const char *pData, std::size_t dataLen, bool isAppend = false)
//{
//	assert(pfullfile);
//	assert(pData);
//	std::ofstream::openmode mode = isAppend ?std::ofstream::app :std::ofstream::trunc;
//	std::ofstream file(pfullfile, mode | std::ifstream::binary | std::ifstream::in);
//	if(file)
//	{
//		file.write(pData, dataLen);
//		file.close();
//		return true;
//	}
//	return false;
//
//}
SBDataType curl::downloadFile(const std::string &fileUrl, int timeout /*= 0*/)
{
	SBDataType vecData;
	assert(!fileUrl.empty());
	if(fileUrl.empty()) return vecData;
	curl_ptr pcurl;
	CURLcode code;
	std::string strBuffer;
	if (pcurl)
	{
		code = curl_easy_setopt(pcurl, CURLOPT_URL, fileUrl.c_str());
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, writerData);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, &vecData);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		if (timeout > 0)
		{
			code = curl_easy_setopt(pcurl, CURLOPT_TIMEOUT, timeout);        //设置超时
			if(code != CURLE_OK) CURL_EXCEPT(code);
		}
//#if defined(_DEBUG)
//		//SEND_LOGA("curl::downloadFile begin threadID:<%d> url:<%s>", GetCurrentThreadId(), fileUrl.c_str());
//		DWORD tick = GetTickCount();
//#endif
		code = curl_easy_perform(pcurl);
//#if defined(_DEBUG)
//		SEND_LOGA("NOTE curl::downloadFile end   threadID:<%d> 耗时：<%d> url:<%s>",  GetCurrentThreadId(), GetTickCount()-tick, fileUrl.c_str());
//		tick = GetTickCount();
//#endif
		if(code != CURLE_OK) 
		{
			vecData.clear();
			CURL_EXCEPT(code);
		}
	}

	return vecData;
}

//std::string curl::post(const std::string &webUrl, const std::string &fields)
//{
//	std::string sret;
//	assert(!webUrl.empty());
//	if(webUrl.empty()) return sret;
//	CURL *curl = NULL;
//	CURLcode code;
//	mc_wrFileBuffer.clear();
//
//	curl = curl_easy_init();
//	if (curl)
//	{
//		code = curl_easy_setopt(curl, CURLOPT_POST, 1);
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		code = curl_easy_setopt(curl, CURLOPT_URL, webUrl.c_str());
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.c_str());    // 指定post内容
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, fields.size());
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
//		//curl_easy_setopt(curl, CURLOPT_HEADER, 0);
//		//curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
//		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
//		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
//
//		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
//		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
//		//curl_easy_setopt(curl,CURLOPT_CAINFO, "ca-cert.pem" );
//
//		code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mc_wrFileBuffer);
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
//		//curl_easy_setopt(curl, CURLOPT_HEADER, 1);  
//		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
//		//curl_easy_setopt(curl,CURLOPT_COOKIESESSION,1L);
//		//curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/Users/kp/curlposttest.cookie");
//		code = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/Users/kp/curlposttest.cookie"); // 指定cookie文件
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
//		code = curl_easy_perform(curl);
//		//std::string str = curl_easy_strerror(code);
//		if(code != CURLE_OK) CURL_EXCEPT(code);
//		
//		sret.assign(mc_wrFileBuffer.cbegin(), mc_wrFileBuffer.cend());
//		//writeFile(L"d:\\111\\a.jpg", (const char *)vecData.data(), vecData.size());
//		curl_easy_cleanup(curl);
//	}
//	//curl_global_cleanup();
//	return sret;
//}
/*static*/ std::string curl::post(const std::string &webUrl, const std::string &fields, int timeout /*= 0*/, PCSTR pCAinfo /*= NULL*/)
{
	std::string sret;
	assert(!webUrl.empty());
	if(webUrl.empty()) return sret;
	//CURL *pcurl = NULL;
	curl_ptr pcurl;
	CURLcode code;

	std::string strBuffer;
	//mc_wrFileBuffer.clear();
	//curl_global_init(CURL_GLOBAL_DEFAULT);
	//pcurl = curl_easy_init();
	if (pcurl)
	{
		//curl_slist *headers = NULL;
		//headers = curl_slist_append(headers, "Accept:text/javascript,application/json;*/*;q=0.01");  
		//headers = curl_slist_append(headers, "Accept-Encoding:gzip,deflate");  
		//headers = curl_slist_append(headers, "Accept-Language:zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");  
		//headers = curl_slist_append(headers, "Connection:keep-alive");  
		//headers = curl_slist_append(headers, "Cache-Control:no-cache");  
		//headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded;charset=UTF-8");  
		//code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);	// 改协议头
		//if(code != CURLE_OK) CURL_EXCEPT(code);

		code = curl_easy_setopt(pcurl, CURLOPT_URL, webUrl.c_str());
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_POST, 1);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_POSTFIELDS, fields.c_str());    // 指定post内容
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_POSTFIELDSIZE, fields.size());
		if(code != CURLE_OK) CURL_EXCEPT(code);
		//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		//curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		//curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
		if (stringUtil::startsWith(webUrl, "https://"))
		{
			if (pCAinfo == NULL)
			{
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYHOST, 0);
			}
			else
			{
				assert(!L"还未实现！！！");
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYPEER, 1);
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYHOST, 1);
				//curl_easy_setopt(curl,CURLOPT_CAINFO,"ca-cert.pem");
			}
		}

		code = curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, writer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, &strBuffer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		//code = curl_easy_setopt(curl,CURLOPT_HEADER,1); //将响应头信息和相应体一起传给write_data  
		//code = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);		//打印调试信息 
		code = curl_easy_setopt(pcurl, CURLOPT_FOLLOWLOCATION, 1);	//设置为非0,响应头信息location
		if(code != CURLE_OK) CURL_EXCEPT(code);
		//curl_easy_setopt(curl,CURLOPT_COOKIESESSION, 1);
		curl_easy_setopt(pcurl, CURLOPT_COOKIEJAR, "/Users/kp/curlposttest.cookie");
		code = curl_easy_setopt(pcurl, CURLOPT_COOKIEFILE, "/Users/kp/curlposttest.cookie"); // 指定cookie文件
		if(code != CURLE_OK) CURL_EXCEPT(code);
		if (timeout > 0)
		{
			code = curl_easy_setopt(pcurl, CURLOPT_TIMEOUT, timeout);        //设置超时
			if(code != CURLE_OK) CURL_EXCEPT(code);
		}
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		code = curl_easy_perform(pcurl);
		//std::string str = curl_easy_strerror(code);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		
		sret.assign(strBuffer.cbegin(), strBuffer.cend());
		//writeFile(L"d:\\111\\a.jpg", (const char *)vecData.data(), vecData.size());
		//curl_easy_cleanup(pcurl);
	}
	//curl_global_cleanup();
	return sret;
}

/*static*/ std::string curl::post(const std::string &webUrl, const PostDataArrayType &postData, int timeout /*= 0*/, PCSTR pCAinfo /*= NULL*/)
{
	std::string sret;
	assert(!webUrl.empty());
	if(webUrl.empty()) return sret;
	//CURL *pcurl = NULL;
	curl_ptr pcurl;
	curl_httppost *formpost = NULL;
	curl_httppost *lastptr  = NULL;
	CURLcode code;

	std::string strBuffer;
	//mc_wrFileBuffer.clear();

	//pcurl = curl_easy_init();
	if (pcurl)
	{
		code = curl_easy_setopt(pcurl, CURLOPT_URL, webUrl.c_str());
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_POST, 1);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_NOSIGNAL, 1);  
		if(code != CURLE_OK) CURL_EXCEPT(code);
		// 
		for (PostDataArrayType::const_iterator iter = postData.cbegin(); iter != postData.cend(); ++iter)
		{
			switch(iter->dataFlag)
			{
			case BEPostDataFlagNormal:
				{
					curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, iter->strName.c_str()
						, CURLFORM_PTRCONTENTS, iter->strData.c_str()
						//, CURLFORM_NAMELENGTH, iter->strName.length()
						, CURLFORM_END);
				}
				break;
			case BEPostDataFlagFile:
				{
					curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, iter->strName.c_str()
						, CURLFORM_FILE, iter->strData.c_str()/*,   CURLFORM_CONTENTTYPE, "image/jpeg"*/
						, CURLFORM_END);
				}
				break;
			case BEPostDataFlagData:
				{
					curl_formadd(&formpost, &lastptr
						, CURLFORM_PTRNAME, iter->strName.c_str()
						, CURLFORM_BUFFER, "{65F6F681-863A-4038-BEB3-33CE2BCB5EBC}.tmp"
						, CURLFORM_BUFFERPTR, iter->strData.data()
						, CURLFORM_BUFFERLENGTH, iter->strData.size()
						, CURLFORM_END);

				}
				break;
			}
		}
		//for (SStringMapType::const_iterator iter = mapFields.cbegin(); iter != mapFields.cend(); ++iter)
		//{
		//	if (iter->first == "pic")
		//	{
		//		//curl_formadd(&formpost, &lastptr,
		//		//	CURLFORM_PTRNAME, "pic",
		//		//	CURLFORM_BUFFER, "cswuyg_test.txt",
		//		//	CURLFORM_BUFFERPTR, iter->second.data(),
		//		//	CURLFORM_BUFFERLENGTH, iter->second.size(),
		//		//	CURLFORM_END);
		//		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "pic",   CURLFORM_FILE, "d:\\abc.jpg"/*,   CURLFORM_CONTENTTYPE, "image/jpeg"*/, CURLFORM_END);
		//	}
		//	else
		//	{
		//		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, iter->first.c_str(), CURLFORM_PTRCONTENTS, iter->second.c_str(), CURLFORM_END);
		//	}
		//}
		code = curl_easy_setopt(pcurl, CURLOPT_HTTPPOST, formpost);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		// 
		if (stringUtil::startsWith(webUrl, "https://"))
		{
			if (pCAinfo == NULL)
			{
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYHOST, 0);
			}
			else
			{
				assert(!L"还未实现！！！");
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYPEER, 1);
				curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYHOST, 1);
				//curl_easy_setopt(curl,CURLOPT_CAINFO,"ca-cert.pem");
			}
		}
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, writer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, &strBuffer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		/* 与服务器通信交互cookie，默认在内存中，可以是不存在磁盘中的文件或留空 */  
		code = curl_easy_setopt(pcurl, CURLOPT_COOKIEFILE, "/Users/kp/curlposttest2.cookie");
		if(code != CURLE_OK) CURL_EXCEPT(code);
		/* 与多个CURL或浏览器交互cookie，会在释放内存后写入磁盘文件 */  
		code = curl_easy_setopt(pcurl, CURLOPT_COOKIEJAR, "/Users/kp/curlposttest2.cookie");
		if(code != CURLE_OK) CURL_EXCEPT(code);
		if (timeout > 0)
		{
			code = curl_easy_setopt(pcurl, CURLOPT_TIMEOUT, timeout);        //设置超时
			if(code != CURLE_OK) CURL_EXCEPT(code);
		}
		code = curl_easy_perform(pcurl);
		if(code != CURLE_OK) CURL_EXCEPT(code);

		sret.assign(strBuffer.cbegin(), strBuffer.cend());
		//curl_easy_cleanup(pcurl);
	}
	if(formpost) curl_formfree(formpost);
	//curl_global_cleanup();
	return sret;
}
/*static*/ std::string curl::get(const std::string &webUrl)
{
	std::string sret;
	assert(!webUrl.empty());
	if(webUrl.empty()) return sret;
	//CURL *pcurl = NULL;
	curl_ptr pcurl;
	CURLcode code;

	std::string strBuffer;
	//mc_wrFileBuffer.clear();
	curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: Agent-007");
	//pcurl = curl_easy_init();
	if (pcurl)
	{
		code = curl_easy_setopt(pcurl, CURLOPT_HTTPHEADER, headers);	// 改协议头
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_URL, webUrl.c_str());
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, writer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, &strBuffer);
		if(code != CURLE_OK) CURL_EXCEPT(code);
		//code = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &mc_wrFileBuffer);
		//if(code != CURLE_OK) CURL_EXCEPT(code);
		code = curl_easy_perform(pcurl);
		if(code != CURLE_OK) CURL_EXCEPT(code);

		sret.assign(strBuffer.cbegin(), strBuffer.cend());
		//writeFile(L"d:\\111\\a.jpg", (const char *)vecData.data(), vecData.size());

		curl_slist_free_all(headers);
		//curl_easy_cleanup(pcurl);
	}

	return sret;
}
}