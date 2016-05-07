#ifndef __STRINGUTIL_H__
#define __STRINGUTIL_H__
#pragma once
#include "basedef.h"

namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	textEncoding  类		2015-9-14
//	编码类的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class textEncoding
{
public:
	enum
	{
		MAX_SEQUENCE_LENGTH = 6 /// The maximum character byte sequence length supported.
	};

	typedef int CharacterMap[256];
	/// The map[b] member gives information about byte sequences
	/// whose first byte is b.
	/// If map[b] is c where c is >= 0, then b by itself encodes the Unicode scalar value c.
	/// If map[b] is -1, then the byte sequence is malformed.
	/// If map[b] is -n, where n >= 2, then b is the first byte of an n-byte
	/// sequence that encodes a single Unicode scalar value. Byte sequences up
	/// to 6 bytes in length are supported.

	virtual ~textEncoding();
	/// Destroys the encoding.

	virtual const char* canonicalName() const = 0;
	/// Returns the canonical name of this encoding,
	/// e.g. "ISO-8859-1". Encoding name comparisons are case
	/// insensitive.

	virtual bool isA(const std::string& encodingName) const = 0;
	/// Returns true if the given name is one of the names of this encoding.
	/// For example, the "ISO-8859-1" encoding is also known as "Latin-1".
	///
	/// Encoding name comparision are be case insensitive.

	virtual const CharacterMap& characterMap() const = 0;
	/// Returns the CharacterMap for the encoding.
	/// The CharacterMap should be kept in a static member. As
	/// characterMap() can be called frequently, it should be
	/// implemented in such a way that it just returns a static
	/// map. If the map is built at runtime, this should be
	/// done in the constructor.

	virtual int convert(const unsigned char* bytes) const;
	/// The convert function is used to convert multibyte sequences;
	/// bytes will point to a byte sequence of n bytes where 
	/// sequenceLength(bytes, length) == -n, with length >= n.
	///
	/// The convert function must return the Unicode scalar value
	/// represented by this byte sequence or -1 if the byte sequence is malformed.
	/// The default implementation returns (int) bytes[0].

	virtual	int queryConvert(const unsigned char* bytes, int length) const;
	/// The queryConvert function is used to convert single byte characters 
	/// or multibyte sequences;
	/// bytes will point to a byte sequence of length bytes.
	///
	/// The queryConvert function must return the Unicode scalar value
	/// represented by this byte sequence or -1 if the byte sequence is malformed
	/// or -n where n is number of bytes requested for the sequence, if lenght is 
	/// shorter than the sequence.
	/// The length of the sequence might not be determined by the first byte, 
	/// in which case the conversion becomes an iterative process:
	/// First call with length == 1 might return -2,
	/// Then a second call with lenght == 2 might return -4
	/// Eventually, the third call with length == 4 should return either a 
	/// Unicode scalar value, or -1 if the byte sequence is malformed.
	/// The default implementation returns (int) bytes[0].

	virtual int sequenceLength(const unsigned char* bytes, int length) const;
	/// The sequenceLength function is used to get the lenth of the sequence pointed
	/// by bytes. The length paramater should be greater or equal to the length of 
	/// the sequence.
	///
	/// The sequenceLength function must return the lenght of the sequence
	/// represented by this byte sequence or a negative value -n if length is 
	/// shorter than the sequence, where n is the number of byte requested 
	/// to determine the length of the sequence.
	/// The length of the sequence might not be determined by the first byte, 
	/// in which case the conversion becomes an iterative process as long as the 
	/// result is negative:
	/// First call with length == 1 might return -2,
	/// Then a second call with lenght == 2 might return -4
	/// Eventually, the third call with length == 4 should return 4.
	/// The default implementation returns 1.

	virtual int convert(int ch, unsigned char* bytes, int length) const;
	/// Transform the Unicode character ch into the encoding's 
	/// byte sequence. The method returns the number of bytes
	/// used. The method must not use more than length characters.
	/// Bytes and length can also be null - in this case only the number
	/// of bytes required to represent ch is returned.
	/// If the character cannot be converted, 0 is returned and
	/// the byte sequence remains unchanged.
	/// The default implementation simply returns 0.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	UTF8Encoding  类		2015-9-14
//	utf8编码
////////////////////////////////////////////////////////////////////////////////////////////////////////
class UTF8Encoding: public textEncoding
	/// UTF-8 text encoding, as defined in RFC 2279.
{
public:
	UTF8Encoding();
	~UTF8Encoding();
	const char* canonicalName() const;
	bool isA(const std::string& encodingName) const;
	const CharacterMap& characterMap() const;
	int convert(const unsigned char* bytes) const;
	int convert(int ch, unsigned char* bytes, int length) const;
	int queryConvert(const unsigned char* bytes, int length) const;
	int sequenceLength(const unsigned char* bytes, int length) const;

	static bool isLegal(const unsigned char *bytes, int length);
	/// Utility routine to tell whether a sequence of bytes is legal UTF-8.
	/// This must be called with the length pre-determined by the first byte.
	/// The sequence is illegal right away if there aren't enough bytes 
	/// available. If presented with a length > 4, this function returns false.
	/// The Unicode definition of UTF-8 goes up to 4-byte sequences.
	/// 
	/// Adapted from ftp://ftp.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.c
	/// Copyright 2001-2004 Unicode, Inc.

private:
	static const char* _names[];
	static const CharacterMap _charMap;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	UTF16Encoding  类		2015-9-14
//	utf16编码
////////////////////////////////////////////////////////////////////////////////////////////////////////

class UTF16Encoding: public textEncoding
	/// UTF-16 text encoding, as defined in RFC 2781.
	///
	/// When converting from UTF-16 to Unicode, surrogates are
	/// reported as they are - in other words, surrogate pairs
	/// are not combined into one Unicode character. 
	/// When converting from Unicode to UTF-16, however, characters
	/// outside the 16-bit range are converted into a low and
	/// high surrogate.
{
public:
	enum _BEByteOrderType
	{
		BIG_ENDIAN_BYTE_ORDER,
		LITTLE_ENDIAN_BYTE_ORDER,
		NATIVE_BYTE_ORDER
	};

	UTF16Encoding(_BEByteOrderType byteOrder = NATIVE_BYTE_ORDER);
	/// Creates and initializes the encoding for the given byte order.

	UTF16Encoding(int byteOrderMark);
	/// Creates and initializes the encoding for the byte-order
	/// indicated by the given byte-order mark, which is the Unicode
	/// character 0xFEFF.

	~UTF16Encoding();

	_BEByteOrderType getByteOrder() const;
	/// Returns the byte-order currently in use.

	void setByteOrder(_BEByteOrderType byteOrder);
	/// Sets the byte order.

	void setByteOrder(int byteOrderMark);
	/// Sets the byte order according to the given
	/// byte order mark, which is the Unicode
	/// character 0xFEFF.

	const char* canonicalName() const;
	bool isA(const std::string& encodingName) const;
	const CharacterMap& characterMap() const;
	int convert(const unsigned char* bytes) const;
	int convert(int ch, unsigned char* bytes, int length) const;
	int queryConvert(const unsigned char* bytes, int length) const;
	int sequenceLength(const unsigned char* bytes, int length) const;

private:
	bool _flipBytes;
	static const char* _names[];
	static const CharacterMap _charMap;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	textIterator  类		2015-9-14
//	字符转换迭代器
////////////////////////////////////////////////////////////////////////////////////////////////////////

class textIterator
	/// An unidirectional iterator for iterating over characters in a string.
	/// The textIterator uses a TextEncoding object to
	/// work with multi-byte character encodings like UTF-8.
	/// Characters are reported in Unicode.
	///
	/// Example: Count the number of UTF-8 characters in a string.
	///
	///     UTF8Encoding utf8Encoding;
	///     std::string utf8String("....");
	///     textIterator it(utf8String, utf8Encoding);
	///     textIterator end(utf8String);
	///     int n = 0;
	///     while (it != end) { ++n; ++it; }
	///
	/// NOTE: When an UTF-16 encoding is used, surrogate pairs will be
	/// reported as two separate characters, due to restrictions of
	/// the TextEncoding class.
	///
	/// For iterating over char buffers, see the TextBufferIterator class.
{
public:
	textIterator();
	/// Creates an uninitialized textIterator.

	textIterator(const std::string& str, const textEncoding& encoding);
	/// Creates a textIterator for the given string.
	/// The encoding object must not be deleted as long as the iterator
	/// is in use.

	textIterator(const std::string::const_iterator& begin, const std::string::const_iterator& end, const textEncoding& encoding);
	/// Creates a textIterator for the given range.
	/// The encoding object must not be deleted as long as the iterator
	/// is in use.

	textIterator(const std::string& str);
	/// Creates an end textIterator for the given string.

	textIterator(const std::string::const_iterator& end);
	/// Creates an end textIterator.

	~textIterator();
	/// Destroys the textIterator.

	textIterator(const textIterator& it);
	/// Copy constructor.

	textIterator& operator = (const textIterator& it);
	/// Assignment operator.

	void swap(textIterator& it);
	/// Swaps the iterator with another one.

	int operator * () const;
	/// Returns the Unicode value of the current character.
	/// If there is no valid character at the current position,
	/// -1 is returned.

	textIterator& operator ++ (); 
	/// Prefix increment operator.

	textIterator operator ++ (int);		
	/// Postfix increment operator.

	bool operator == (const textIterator& it) const;
	/// Compares two iterators for equality.

	bool operator != (const textIterator& it) const;
	/// Compares two iterators for inequality.

	textIterator end() const;
	/// Returns the end iterator for the range handled
	/// by the iterator.

private:
	const textEncoding*         _pEncoding;
	std::string::const_iterator _it;
	std::string::const_iterator _end;
};


//
// inlines
//
inline bool textIterator::operator == (const textIterator& it) const
{
	return _it == it._it;
}


inline bool textIterator::operator != (const textIterator& it) const
{
	return _it != it._it;
}


inline void swap(textIterator& it1, textIterator& it2)
{
	it1.swap(it2);
}


inline textIterator textIterator::end() const
{
	return textIterator(_end);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	textConverter  类		2015-9-14
//	字符格式转换
////////////////////////////////////////////////////////////////////////////////////////////////////////
class textConverter
	/// A textConverter converts strings from one encoding
	/// into another.
{
public:
	typedef int (*Transform)(int);
	/// Transform function for convert.

	textConverter(const textEncoding& inEncoding, const textEncoding& outEncoding, int defaultChar = '?');
	/// Creates the textConverter. The encoding objects must not be deleted while the
	/// textConverter is in use.

	~textConverter();
	/// Destroys the textConverter.

	int convert(const std::string& source, std::string& destination, Transform trans);
	/// Converts the source string from inEncoding to outEncoding
	/// and appends the result to destination. Every character is
	/// passed to the transform function.
	/// If a character cannot be represented in outEncoding, defaultChar
	/// is used instead.
	/// Returns the number of encoding errors (invalid byte sequences
	/// in source).

	int convert(const void* source, int length, std::string& destination, Transform trans);
	/// Converts the source buffer from inEncoding to outEncoding
	/// and appends the result to destination. Every character is
	/// passed to the transform function.
	/// If a character cannot be represented in outEncoding, defaultChar
	/// is used instead.
	/// Returns the number of encoding errors (invalid byte sequences
	/// in source).

	int convert(const std::string& source, std::string& destination);
	/// Converts the source string from inEncoding to outEncoding
	/// and appends the result to destination.
	/// If a character cannot be represented in outEncoding, defaultChar
	/// is used instead.
	/// Returns the number of encoding errors (invalid byte sequences
	/// in source).

	int convert(const void* source, int length, std::string& destination);
	/// Converts the source buffer from inEncoding to outEncoding
	/// and appends the result to destination.
	/// If a character cannot be represented in outEncoding, defaultChar
	/// is used instead.
	/// Returns the number of encoding errors (invalid byte sequences
	/// in source).

private:
	textConverter();
	textConverter(const textConverter&);
	textConverter& operator = (const textConverter&);

	const textEncoding& _inEncoding;
	const textEncoding& _outEncoding;
	int                 _defaultChar;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	stringUtil类	2015-1-15
//	字符相关操作集合
////////////////////////////////////////////////////////////////////////////////////////////////////////
class stringUtil
{
private:
	stringUtil(){}
	
public:
	// Constant blank string, useful for returning by ref where local does not exist
	static const std::wstring BLANK;
	/****************************************************************************************
		<< --- 		2015-9-14 --- >>         
		说明：字符串比较，忽略大小写
		参数：
		返回值：
	*****************************************************************************************/
	static int iCompare(const std::string &str1, const std::string &str2);
	static int iCompare(const std::wstring &str1, const std::wstring &str2);

	/****************************************************************************************
		<< --- replaceFirst		2015-7-13 --- >>         
		说明：替换第一个满足条件的字符串
		参数：
			source			= 数据源
			replaceWhat		= 需要替换的字符串
			replaceWithWhat	= 替换为字符串
		返回值：替换后的字符串
	*****************************************************************************************/
	 static std::wstring replaceFirst(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	 static void replaceFirstRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	 static std::string replaceFirst(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);
	 static void replaceFirstRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);
	 static std::wstring replaceLast(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	 static void replaceLastRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	 static std::string replaceLast(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);
	 static void replaceLastRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);

	/****************************************************************************************
		<< --- replaceAll 2015-7-6 --- >>         
		说明：字符串替换
		参数：
			source			= 数据源
			replaceWhat		= 需要替换的字符串
			replaceWithWhat	= 替换为字符串
		返回值：替换后的字符串
	*****************************************************************************************/
	static std::wstring replaceAll(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	static void replaceAllRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	static std::string replaceAll(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);
	static void replaceAllRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);
	/****************************************************************************************
		<< --- replaceAllAny		2016-1-15	kp007 --- >>         
		说明：将满足任意字符的替换为指定字符串
		参数：
			source			= 数据源				“abcdef”
			replaceWhat		= 需要替换的“字符集合”	"be"
			replaceWithWhat	= 替换为字符串			"***"
		返回值：									"a***cd***f"
	*****************************************************************************************/
	static void replaceAllAnyRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	static std::wstring replaceAllAny(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat);
	static void replaceAllAnyRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);
	static std::string replaceAllAny(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat);

	/****************************************************************************************
		<< --- trim		2015-1-15 --- >>         
		说明：去除字符串前端或尾部的空格字符（如：' ' '\r' '\t'）
		参数：
		返回值：
	*****************************************************************************************/
	static void trim(std::wstring &wstr, std::wstring delims = L" \t\r", bool left = true, bool right = true );
	static void trimLeft(std::wstring &wstr, std::wstring delims = L" \t\r");
	static void trimRight(std::wstring &wstr, std::wstring delims = L" \t\r");
	static void trim(std::string &str, std::string delims = " \t\r", bool left = true, bool right = true );
	static void trimLeft(std::string &str, std::string delims = " \t\r");
	static void trimRight(std::string &str, std::string delims = " \t\r");
	/****************************************************************************************
		<< --- toLower		2015-1-15 --- >>         
		说明：将字符串转换到小写
		参数：
		返回值：
	*****************************************************************************************/
	static void toLower(std::wstring &wstr);
	static void toLower(std::string &str);
	/****************************************************************************************
		<< --- toUpper		2015-1-15 --- >>         
		说明：将字符串转换到大写
		参数：
		返回值：
	*****************************************************************************************/
	static void toUpper(std::wstring &wstr);
	static void toUpper(std::string &str);

	/****************************************************************************************
		<< --- startsWith		2015-1-15 --- >>         
		说明：验证字符串是否以指定字符串开头
		参数：
			wstr		= 被验证的字符串
			pattern		= 被比较的字符串
			lowerCase	= 将被查验的字符串转换到小写后再比较
		返回值：是否有
	*****************************************************************************************/
	static bool startsWith(const std::wstring &wstr, const std::wstring &pattern, bool lowerCase = true);
	static bool startsWith(const std::string &str, const std::string &pattern, bool lowerCase = true);
	/****************************************************************************************
		<< --- endsWith		2015-1-15 --- >>         
		说明：验证字符串是否以指定字符串结尾
		参数：
		wstr		= 被验证的字符串
		pattern		= 被比较的字符串
		lowerCase	= 将被查验的字符串转换到小写后再比较
		返回值：是否有
	*****************************************************************************************/
	static bool endsWith(const std::wstring &wstr, const std::wstring &pattern, bool lowerCase = true);
	static bool endsWith(const std::string &str, const std::string &pattern, bool lowerCase = true);

	/****************************************************************************************
		<< --- split		2015-5-11 --- >>         
		说明：将字符串以指定字符分隔成字符串数组
		参数：
			wstr		= 被分割的字符串
			wsPattern	= 分隔符
			trimBlank	= 是否移除分割后字符段两端的空格字符
			noBlank		= 释放保留空白字符串，默认：不保留
		返回值：分割后的字符串数组
	*****************************************************************************************/
	static StringArrayType split(const std::wstring &wstr, const std::wstring &wsPattern, bool trimBlank = false, bool noBlank = true);
	static SStringArrayType split(const std::string &str, const std::string &strPattern, bool trimBlank = false, bool noBlank = true);

	/*****************************************************************************************
		<< --- static convert::toWChar	2014-04-25 --- >>
		说明：将单字符转换成宽字符串(UTF-8未测试)
		参数：
		pdata		= 被转换的字符
		isAnsi	= true:被转换的字符为ANSI码；false:否则为UTF-8
		返回值：转换后的字符串
	*****************************************************************************************/
	static bool toWChar(const char *pdata, wchar_t **pbufWChar, UINT codePage = CP_ACP /* = CP_ACP*/)
	{
		assert(pdata && pbufWChar);
		if(!pdata || !pbufWChar || std::strlen(pdata) <= 0)
			return false;
		wchar_t *buf = nullptr;
#if defined(_WIN32)
		int len = ::MultiByteToWideChar(codePage, 0, pdata, -1, NULL, 0);
		if(len > 0)
		{
			buf = new wchar_t[len+1];
			memset(buf, 0, (len+1)*sizeof(wchar_t));
			len = ::MultiByteToWideChar(codePage, 0, pdata, -1, buf, len);
		}
#else
		if(codePage == CP_ACP)
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
	static std::wstring toWChar(const char *pdata, UINT codePage = CP_ACP /* = CP_ACP*/)
	{
		assert(pdata);
		if(!pdata || std::strlen(pdata) == 0) return L"";
		std::wstring wstr;
		wchar_t *pbuf = NULL;
		if (toWChar(pdata, &pbuf, codePage))
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
	static bool toChar(const wchar_t *pdata, char **pbufChar, UINT codePage = CP_ACP /* = CP_ACP*/)
	{
		assert(pdata != NULL && pbufChar != NULL);
		if(!pdata || !pbufChar || std::wcslen(pdata) <= 0) return false;
		char *buf = NULL;
#if defined(_WIN32)
		int len = ::WideCharToMultiByte(codePage, 0, pdata, -1, NULL,0, NULL, NULL);
		if(len > 0)
		{
			buf = new char[len+1];
			memset(buf, 0, (len+1)*sizeof(char));
			len = ::WideCharToMultiByte(codePage, 0, pdata, -1, buf, len, NULL, NULL);
		}
#else
		if(codePage == CP_ACP)
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
	static std::string toChar(const wchar_t *pdata, UINT codePage = CP_ACP /* = CP_ACP*/)
	{
		assert(pdata);
		if(!pdata || std::wcslen(pdata) == 0) return "";
		std::string cstr;
		char *buf = NULL;
		if (toChar(pdata, &buf, codePage))
		{
			if(buf)
				cstr = buf;
		}
		if(buf) delete[] buf;
		return cstr;
	}

	/****************************************************************************************
		<< --- 	toChs	2015-9-1 --- >>         
		说明：将数字转换到汉字形式
		参数：
			num		= 要转换的数
			unit	= 是否加入单位，=true 123 --> 一百二十三，否则转换成 --> 一二三
		返回值：转换后的字符
	*****************************************************************************************/
	static std::wstring toChs(int num, bool unit = true);
	/****************************************************************************************
		<< --- 	toUrlEncode toUrlDecode	2015-9-10 --- >>         
		说明：url编解码
		参数：
			wsUrlData	= 被编解码的数据
			bUpperCase	= 是否以大写十六进制字母表示
		返回值：
	*****************************************************************************************/
	static std::wstring toUrlEncode(const std::wstring &wsUrlData, bool bUpperCaseFormat = true);
	static std::wstring toUrlDecode(const std::wstring &wsUrlData);
	static std::string toUrlEncode(const std::string &strUrlData, bool bUpperCaseFormat = true);
	static std::string toUrlDecode(const std::string &strUrlData);

	/****************************************************************************************
		<< --- 		2015-9-15 --- >>         
		说明：字符串转换
		参数：
			wsData = 被转化的数据
		返回值：转换结果
	*****************************************************************************************/
	static std::string toUTF8(const std::wstring &wsData);

	
	static std::wstring toHex(const uint8_t *pData, int32_t dataLen, bool isUpper = true);
};



}	// gtc
#endif	// __STRINGUTIL_H__