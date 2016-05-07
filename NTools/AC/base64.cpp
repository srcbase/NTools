#include "base64.h"
#include "stringUtil.h"
namespace gtc 
{
//编码表
static const char c_encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char c_map[256] =  
{  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
	  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
	 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
	 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
	 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255  
}; 
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	base64	类 2015-12-15	kp007
//	base64编码封装类
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string base64::encode(const uint8_t *pinData, std::size_t inLen)
{
	assert(pinData);
	assert(inLen > 0);
	if(!pinData || inLen == 0) return "";

	std::string strEncode;
	std::size_t idx = 0;
	std::size_t leven = inLen/3*3;
	for(idx = 0; idx < leven; idx += 3)  
	{  
		strEncode += c_encode[pinData[0] >> 2];  
		strEncode += c_encode[((pinData[0] & 3) << 4) + (pinData[1] >> 4)];  
		strEncode += c_encode[((pinData[1] & 0xF) << 2) + (pinData[2] >> 6)];  
		strEncode += c_encode[pinData[2] & 0x3F];
		pinData += 3;
	}
	if (idx < inLen)  
	{  
		unsigned char a = pinData[0];  
		unsigned char b = ((idx + 1) < inLen) ? pinData[1] : 0;  
		unsigned char c = 0;  

		strEncode += c_encode[a >> 2];  
		strEncode += c_encode[((a & 3) << 4) + (b >> 4)];  
		strEncode += ((idx + 1) < inLen) ? c_encode[((b & 0xF) << 2) + (c >> 6)] : '=';  
		strEncode += '=';  
	} 
	return strEncode;
}
std::string base64::encode(const std::string &inData)
{
	assert(!inData.empty());
	return encode((const uint8_t *)inData.c_str(), inData.length());
}

std::wstring base64::encodeW(const std::string &inData)
{
	assert(!inData.empty());
	std::string str = encode(inData);

	return gtc::stringUtil::toWChar(str.c_str());
}
std::wstring base64::encode(const std::wstring &inData)
{
	assert(!inData.empty());
	std::string str = gtc::stringUtil::toChar(inData.c_str());
	str = encode(str);

	return gtc::stringUtil::toWChar(str.c_str());
}
std::string base64::encode(const SBDataType &sbData)
{
	assert(sbData.size() > 0);
	if(sbData.size() == 0) return "";

	return encode(sbData.data(), sbData.size());
}

std::wstring base64::encodeW(const SBDataType &sbData)
{
	std::string str = encode(sbData);
	return gtc::stringUtil::toWChar(str.c_str());
}




std::string base64::decode(const char *pinData, std::size_t inLen)
{
	unsigned long t, x, y, z;  
	unsigned char c;  
	unsigned long g = 3;
	std::string strDecode;
	for(x = y = z = t = 0; x < inLen; x++)  
	{  
		c = c_map[pinData[x]];  
		if(c == 255) continue;  
		if(c == 254) { c = 0; g--; }  

		t = (t << 6) | c;  

		if(++y == 4)  
		{  
			strDecode += (char)((t >> 16) & 0xFF);
			if(g > 1) strDecode += (char)((t >> 8) & 0xFF);
			if(g > 2) strDecode += (char)(t & 0xFF);
			y = t = 0;
		}  
	}

	return strDecode;
}


std::string base64::decode(const std::string &inData)
{
	assert(!inData.empty());
	if(inData.empty()) return "";

	return decode(inData.data(), inData.length());
}
std::wstring base64::decode(const std::wstring &inData)
{
	std::string str = gtc::stringUtil::toChar(inData.c_str());
	str = decode(str);

	return gtc::stringUtil::toWChar(str.c_str());
}
SBDataType base64::decodeToData(const char *pinData, std::size_t inLen)
{
	assert(pinData);
	assert(inLen > 0);
	std::string str = decode(pinData, inLen);

	return SBDataType(str.cbegin(), str.cend());
}
SBDataType base64::decodeToData(const std::string &inData)
{
	assert(!inData.empty());
	return decodeToData(inData.c_str(), inData.length());
}
SBDataType base64::decodeToData(const std::wstring &inData)
{
	assert(!inData.empty());
	std::string str = decode(gtc::stringUtil::toChar(inData.c_str()));
	return decodeToData(str);
}
}	// gtc