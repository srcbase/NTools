#include "stringUtil.h"
#include "sysupport.h"
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	textEncoding  类		2015-9-14
//	编码类的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
textEncoding::~textEncoding()
{

}
int textEncoding::convert(const unsigned char* bytes) const
{
	return static_cast<int>(*bytes);
}

int textEncoding::queryConvert(const unsigned char* bytes, int length) const
{
	return static_cast<int>(*bytes);
}

int textEncoding::sequenceLength(const unsigned char* bytes, int length) const
{
	return 1;
}

int textEncoding::convert(int ch, unsigned char* bytes, int length) const
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	UTF8Encoding  类		2015-9-14
//	utf8编码
////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* UTF8Encoding::_names[] =
{
	"UTF-8",
	"UTF8",
	NULL
};


const textEncoding::CharacterMap UTF8Encoding::_charMap = 
{
	/* 00 */	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
	/* 10 */	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
	/* 20 */	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
	/* 30 */	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
	/* 40 */	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
	/* 50 */	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	/* 60 */	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 
	/* 70 */	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 
	/* 80 */	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 
	/* 90 */	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 
	/* a0 */	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 
	/* b0 */	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 
	/* c0 */	  -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2, 
	/* d0 */	  -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2, 
	/* e0 */	  -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3, 
	/* f0 */	  -4,   -4,   -4,   -4,   -4,   -4,   -4,   -4,   -5,   -5,   -5,   -5,   -6,   -6,   -1,   -1, 
};


UTF8Encoding::UTF8Encoding()
{
}


UTF8Encoding::~UTF8Encoding()
{
}


const char* UTF8Encoding::canonicalName() const
{
	return _names[0];
}


bool UTF8Encoding::isA(const std::string& encodingName) const
{
	for (const char** name = _names; *name; ++name)
	{
		if (stringUtil::iCompare(encodingName, *name) == 0)
			return true;
	}
	return false;
}


const textEncoding::CharacterMap& UTF8Encoding::characterMap() const
{
	return _charMap;
}


int UTF8Encoding::convert(const unsigned char* bytes) const
{
	int n = _charMap[*bytes];
	int uc;

	switch (n)
	{
	case -6:
	case -5:
	case -1:
		return -1;
	case -4: 
	case -3: 
	case -2:
		if (!isLegal(bytes, -n)) return -1;
		uc = *bytes & ((0x07 << (n + 4)) | 0x03);
		break;
	default:
		return n;
	}

	while (n++ < -1) 
	{	
		uc <<= 6;
		uc |= (*++bytes & 0x3F);
	}
	return uc;
}


int UTF8Encoding::convert(int ch, unsigned char* bytes, int length) const
{
	if (ch <= 0x7F)
	{
		if (bytes && length >= 1)
			*bytes = (unsigned char) ch;
		return 1;
	}
	else if (ch <= 0x7FF)
	{
		if (bytes && length >= 2)
		{
			*bytes++ = (unsigned char) (((ch >> 6) & 0x1F) | 0xC0);
			*bytes   = (unsigned char) ((ch & 0x3F) | 0x80);
		}
		return 2;
	}
	else if (ch <= 0xFFFF)
	{
		if (bytes && length >= 3)
		{
			*bytes++ = (unsigned char) (((ch >> 12) & 0x0F) | 0xE0);
			*bytes++ = (unsigned char) (((ch >> 6) & 0x3F) | 0x80);
			*bytes   = (unsigned char) ((ch & 0x3F) | 0x80);
		}
		return 3;
	}
	else if (ch <= 0x10FFFF)
	{
		if (bytes && length >= 4)
		{
			*bytes++ = (unsigned char) (((ch >> 18) & 0x07) | 0xF0);
			*bytes++ = (unsigned char) (((ch >> 12) & 0x3F) | 0x80);
			*bytes++ = (unsigned char) (((ch >> 6) & 0x3F) | 0x80);
			*bytes   = (unsigned char) ((ch & 0x3F) | 0x80);
		}
		return 4;
	}
	else return 0;
}


int UTF8Encoding::queryConvert(const unsigned char* bytes, int length) const
{
	int n = _charMap[*bytes];
	int uc;
	if (-n > length)
	{
		return n;
	}
	else
	{
		switch (n)
		{
		case -6:
		case -5:
		case -1:
			return -1;
		case -4:
		case -3:
		case -2:
			if (!isLegal(bytes, -n)) return -1;
			uc = *bytes & ((0x07 << (n + 4)) | 0x03);
			break;
		default:
			return n;
		}
		while (n++ < -1) 
		{	
			uc <<= 6;
			uc |= (*++bytes & 0x3F);
		}
		return uc;
	}
}


int UTF8Encoding::sequenceLength(const unsigned char* bytes, int length) const
{
	if (1 <= length)
	{
		int cc = _charMap[*bytes];
		if (cc >= 0)
			return 1;
		else
			return -cc;
	}
	else return -1;
}


bool UTF8Encoding::isLegal(const unsigned char *bytes, int length)
{
	// Note: The following is loosely based on the isLegalUTF8 function
	// from ftp://ftp.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.c
	// Excuse the ugliness...

	if (0 == bytes || 0 == length) return false;

	unsigned char a;
	const unsigned char* srcptr = bytes + length;
	switch (length)
	{
	default:
		return false;
		// Everything else falls through when true.
	case 4:
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 3: 
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 2:
		if ((a = (*--srcptr)) > 0xBF) return false;
		switch (*bytes) 
		{
		case 0xE0:
			if (a < 0xA0) return false; 
			break;
		case 0xED:
			if (a > 0x9F) return false; 
			break;
		case 0xF0:
			if (a < 0x90) return false; 
			break;
		case 0xF4:
			if (a > 0x8F) return false; 
			break;
		default:
			if (a < 0x80) return false;
		}
	case 1:
		if (*bytes >= 0x80 && *bytes < 0xC2) return false;
	}
	return *bytes <= 0xF4;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	UTF16Encoding  类		2015-9-14
//	utf16编码
////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* UTF16Encoding::_names[] =
{
	"UTF-16",
	"UTF16",
	NULL
};


const textEncoding::CharacterMap UTF16Encoding::_charMap = 
{
	/* 00 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 10 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 20 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 30 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 40 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 50 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 60 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 70 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 80 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* 90 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* a0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* b0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* c0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* d0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* e0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
	/* f0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
};


UTF16Encoding::UTF16Encoding(_BEByteOrderType byteOrder)
{
	setByteOrder(byteOrder);
}


UTF16Encoding::UTF16Encoding(int byteOrderMark)
{
	setByteOrder(byteOrderMark);
}


UTF16Encoding::~UTF16Encoding()
{
}


UTF16Encoding::_BEByteOrderType UTF16Encoding::getByteOrder() const
{
//#if defined(POCO_ARCH_BIG_ENDIAN)
//	return _flipBytes ? LITTLE_ENDIAN_BYTE_ORDER : BIG_ENDIAN_BYTE_ORDER;
//#else
	return _flipBytes ? BIG_ENDIAN_BYTE_ORDER : LITTLE_ENDIAN_BYTE_ORDER;
//#endif
}


void UTF16Encoding::setByteOrder(_BEByteOrderType byteOrder)
{
//#if defined(POCO_ARCH_BIG_ENDIAN)
//	_flipBytes = byteOrder == LITTLE_ENDIAN_BYTE_ORDER;
//#else
	_flipBytes = byteOrder == BIG_ENDIAN_BYTE_ORDER;;
//#endif
}


void UTF16Encoding::setByteOrder(int byteOrderMark)
{
	_flipBytes = byteOrderMark != 0xFEFF;
}


const char* UTF16Encoding::canonicalName() const
{
	return _names[0];
}


bool UTF16Encoding::isA(const std::string& encodingName) const
{
	for (const char** name = _names; *name; ++name)
	{
		if (stringUtil::iCompare(encodingName, *name) == 0)
			return true;
	}
	return false;
}


const textEncoding::CharacterMap& UTF16Encoding::characterMap() const
{
	return _charMap;
}


int UTF16Encoding::convert(const unsigned char* bytes) const
{
	uint16_t uc;
	unsigned char* p = (unsigned char*) &uc;
	*p++ = *bytes++;
	*p++ = *bytes++;

	if (_flipBytes)
	{
		gtc::convert::flipBytes(uc);
	}

	if (uc >= 0xd800 && uc < 0xdc00)
	{
		uint16_t uc2;
		p = (unsigned char*) &uc2;
		*p++ = *bytes++;
		*p++ = *bytes++;

		if (_flipBytes)
		{
			gtc::convert::flipBytes(uc2);
		}
		if (uc2 >= 0xdc00 && uc2 < 0xe000)
		{
			return ((uc & 0x3ff) << 10) + (uc2 & 0x3ff) + 0x10000;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return uc;
	}
}


int UTF16Encoding::convert(int ch, unsigned char* bytes, int length) const
{
	if (ch <= 0xFFFF)
	{
		if (bytes && length >= 2)
		{
			uint16_t ch1 = _flipBytes ? gtc::convert::flipBytes((uint16_t) ch) : (uint16_t) ch;
			unsigned char* p = (unsigned char*) &ch1;
			*bytes++ = *p++;
			*bytes++ = *p++;
		}
		return 2;
	}
	else
	{
		if (bytes && length >= 4)
		{
			int ch1 = ch - 0x10000;
			uint16_t w1 = 0xD800 + ((ch1 >> 10) & 0x3FF);
			uint16_t w2 = 0xDC00 + (ch1 & 0x3FF);
			if (_flipBytes)
			{
				w1 = gtc::convert::flipBytes(w1);
				w2 = gtc::convert::flipBytes(w2);
			}
			unsigned char* p = (unsigned char*) &w1;
			*bytes++ = *p++;
			*bytes++ = *p++;
			p = (unsigned char*) &w2;
			*bytes++ = *p++;
			*bytes++ = *p++;
		}
		return 4;
	}
}


int UTF16Encoding::queryConvert(const unsigned char* bytes, int length) const
{
	int ret = -2;

	if (length >= 2)
	{
		uint16_t uc;
		unsigned char* p = (unsigned char*) &uc;
		*p++ = *bytes++;
		*p++ = *bytes++;
		if (_flipBytes) 
			gtc::convert::flipBytes(uc);
		if (uc >= 0xd800 && uc < 0xdc00)
		{
			if (length >= 4)
			{
				uint16_t uc2;
				p = (unsigned char*) &uc2;
				*p++ = *bytes++;
				*p++ = *bytes++;
				if (_flipBytes) 
					gtc::convert::flipBytes(uc2);
				if (uc2 >= 0xdc00 && uc < 0xe000)
				{
					ret = ((uc & 0x3ff) << 10) + (uc2 & 0x3ff) + 0x10000;
				}
				else
				{
					ret = -1;	// Malformed sequence
				}
			}
			else
			{
				ret = -4;	// surrogate pair, four bytes needed
			}
		}
		else
		{
			ret = uc;
		}
	}

	return ret;
}


int UTF16Encoding::sequenceLength(const unsigned char* bytes, int length) const
{
	int ret = -2;

	if (_flipBytes)
	{
		if (length >= 1)
		{
			unsigned char c = *bytes;
			if (c >= 0xd8 && c < 0xdc)
				ret = 4;
			else
				ret = 2;
		}
	}
	else
	{
		if (length >= 2)
		{
			uint16_t uc;
			unsigned char* p = (unsigned char*) &uc;
			*p++ = *bytes++;
			*p++ = *bytes++;
			if (uc >= 0xd800 && uc < 0xdc00)
				ret = 4;
			else
				ret = 2;
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	textIterator  类		2015-9-14
//	字符转换迭代器
////////////////////////////////////////////////////////////////////////////////////////////////////////

textIterator::textIterator()
	: _pEncoding(0)
{
}


textIterator::textIterator(const std::string& str, const textEncoding& encoding)
	: _pEncoding(&encoding)
	, _it(str.begin())
	, _end(str.end())
{
}


textIterator::textIterator(const std::string::const_iterator& begin, const std::string::const_iterator& end, const textEncoding& encoding)
	: _pEncoding(&encoding)
	, _it(begin)
	, _end(end)
{
}


textIterator::textIterator(const std::string& str)
	: _pEncoding(0)
	, _it(str.end())
	, _end(str.end())
{
}


textIterator::textIterator(const std::string::const_iterator& end)
	: _pEncoding(0)
	, _it(end)
	, _end(end)
{
}


textIterator::~textIterator()
{
}


textIterator::textIterator(const textIterator& it)
	: _pEncoding(it._pEncoding)
	, _it(it._it)
	, _end(it._end)
{
}


textIterator& textIterator::operator = (const textIterator& it)
{
	if (&it != this)
	{
		_pEncoding = it._pEncoding;
		_it        = it._it;
		_end       = it._end;
	}
	return *this;
}


void textIterator::swap(textIterator& it)
{
	std::swap(_pEncoding, it._pEncoding);
	std::swap(_it, it._it);
	std::swap(_end, it._end);
}


int textIterator::operator * () const
{
	assert(_pEncoding);
	assert(_it != _end);
	std::string::const_iterator it = _it;

	unsigned char buffer[textEncoding::MAX_SEQUENCE_LENGTH];
	unsigned char* p = buffer;

	if (it != _end)
		*p++ = *it++;
	else
		*p++ = 0;

	int read = 1;
	int n = _pEncoding->queryConvert(buffer, 1);

	while (-1 > n && (_end - it) >= -n - read)
	{
		while (read < -n && it != _end)
		{ 
			*p++ = *it++; 
			read++; 
		}
		n = _pEncoding->queryConvert(buffer, read);
	}

	if (-1 > n)
	{
		return -1;
	}
	else
	{
		return n;
	}
}


textIterator& textIterator::operator ++ ()
{
	assert(_pEncoding);
	assert(_it != _end);

	unsigned char buffer[textEncoding::MAX_SEQUENCE_LENGTH];
	unsigned char* p = buffer;

	if (_it != _end)
		*p++ = *_it++;
	else
		*p++ = 0;

	int read = 1;
	int n = _pEncoding->sequenceLength(buffer, 1);

	while (-1 > n && (_end - _it) >= -n - read)
	{
		while (read < -n && _it != _end)
		{ 
			*p++ = *_it++; 
			read++; 
		}
		n = _pEncoding->sequenceLength(buffer, read);
	}
	while (read < n && _it != _end)
	{ 
		_it++; 
		read++; 
	}

	return *this;
}


textIterator textIterator::operator ++ (int)
{
	textIterator prev(*this);
	operator ++ ();
	return prev;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	textConverter  类		2015-9-14
//	字符格式转换
////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
	int nullTransform(int ch)
	{
		return ch;
	}
}

textConverter::textConverter(const textEncoding& inEncoding, const textEncoding& outEncoding, int defaultChar)
	: _inEncoding(inEncoding)
	, _outEncoding(outEncoding)
	, _defaultChar(defaultChar)
{
}


textConverter::~textConverter()
{
}


int textConverter::convert(const std::string& source, std::string& destination, Transform trans)
{
	int errors = 0;
	textIterator it(source, _inEncoding);
	textIterator end(source);
	unsigned char buffer[textEncoding::MAX_SEQUENCE_LENGTH];

	while (it != end)
	{
		int c = *it;
		if (c == -1) { ++errors; c = _defaultChar; }
		c = trans(c);
		int n = _outEncoding.convert(c, buffer, sizeof(buffer));
		if (n == 0) n = _outEncoding.convert(_defaultChar, buffer, sizeof(buffer));
		assert(n <= sizeof(buffer));
		destination.append((const char*) buffer, n);
		++it;
	}
	return errors;
}


int textConverter::convert(const void* source, int length, std::string& destination, Transform trans)
{
	assert(source);

	int errors = 0;
	const unsigned char* it  = (const unsigned char*) source;
	const unsigned char* end = (const unsigned char*) source + length;
	unsigned char buffer[textEncoding::MAX_SEQUENCE_LENGTH];

	while (it < end)
	{
		int n = _inEncoding.queryConvert(it, 1);
		int uc;
		int read = 1;

		while (-1 > n && (end - it) >= -n)
		{
			read = -n;
			n = _inEncoding.queryConvert(it, read);
		}

		if (-1 > n)
		{
			it = end;
		}
		else
		{
			it += read;
		}

		if (-1 >= n)
		{
			uc = _defaultChar;
			++errors;
		}
		else
		{
			uc = n;
		}

		uc = trans(uc);
		n = _outEncoding.convert(uc, buffer, sizeof(buffer));
		if (n == 0) n = _outEncoding.convert(_defaultChar, buffer, sizeof(buffer));
		assert(n <= sizeof(buffer));
		destination.append((const char*) buffer, n);
	}
	return errors;
}


int textConverter::convert(const std::string& source, std::string& destination)
{
	return convert(source, destination, nullTransform);
}


int textConverter::convert(const void* source, int length, std::string& destination)
{
	return convert(source, length, destination, nullTransform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	stringUtil类	2015-1-15
//	字符相关操作集合
////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::wstring stringUtil::BLANK;

int stringUtil::iCompare(const std::string &str1, const std::string &str2)
{
	std::string::const_iterator it1(str1.begin());
	std::string::const_iterator end1(str1.end());
	std::string::const_iterator it2(str2.begin());
	std::string::const_iterator end2(str2.end());
	while (it1 != end1 && it2 != end2)
	{
		char c1 = *it1;
		char c2 = *it2;
		if(std::isupper(c1)) c1 = std::tolower(c1);
		if(std::isupper(c2)) c2 = std::tolower(c2);
		
		if (c1 < c2)
			return -1;
		else if (c1 > c2)
			return 1;
		++it1; ++it2;
	}

	if (it1 == end1)
		return it2 == end2 ? 0 : -1;
	else
		return 1;
}
int stringUtil::iCompare(const std::wstring &str1, const std::wstring &str2)
{
	std::wstring::const_iterator it1(str1.begin());
	std::wstring::const_iterator end1(str1.end());
	std::wstring::const_iterator it2(str2.begin());
	std::wstring::const_iterator end2(str2.end());
	while (it1 != end1 && it2 != end2)
	{
		wchar_t c1 = *it1;
		wchar_t c2 = *it2;
		if(std::iswupper(c1)) c1 = std::towlower(c1);
		if(std::iswupper(c2)) c2 = std::towlower(c2);

		if (c1 < c2)
			return -1;
		else if (c1 > c2)
			return 1;
		++it1; ++it2;
	}

	if (it1 == end1)
		return it2 == end2 ? 0 : -1;
	else
		return 1;
}
std::wstring stringUtil::replaceFirst(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring result = source;
	replaceFirstRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceFirstRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring::size_type pos = 0;
	pos = source.find(replaceWhat,pos);
	if (pos != std::wstring::npos)
	{
		source.replace(pos,replaceWhat.size(),replaceWithWhat);
	}
}
std::string stringUtil::replaceFirst(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string result = source;
	replaceFirstRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceFirstRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string::size_type pos = 0;
	pos = source.find(replaceWhat,pos);
	if (pos != std::string::npos)
	{
		source.replace(pos,replaceWhat.size(),replaceWithWhat);
	}
}

std::wstring stringUtil::replaceLast(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring result = source;
	replaceFirstRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceLastRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring::size_type pos = 0;
	pos = source.rfind(replaceWhat,pos);
	if (pos != std::wstring::npos)
	{
		source.replace(pos,replaceWhat.size(),replaceWithWhat);
	}
}
std::string stringUtil::replaceLast(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string result = source;
	replaceFirstRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceLastRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string::size_type pos = 0;
	pos = source.rfind(replaceWhat,pos);
	if (pos != std::string::npos)
	{
		source.replace(pos,replaceWhat.size(),replaceWithWhat);
	}
}
std::wstring stringUtil::replaceAll(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring result = source;
	replaceAllRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceAllRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring::size_type pos = 0;
	while(1)
	{
		pos = source.find(replaceWhat,pos);
		if (pos == std::wstring::npos) break;
		source.replace(pos,replaceWhat.size(),replaceWithWhat);
		pos += replaceWithWhat.size();
	}
}
std::string stringUtil::replaceAll(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string result = source;
	replaceAllRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceAllRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string::size_type pos = 0;
	while(1)
	{
		pos = source.find(replaceWhat,pos);
		if (pos == std::string::npos) break;
		source.replace(pos,replaceWhat.size(),replaceWithWhat);
		pos += replaceWithWhat.size();
	}

}

void stringUtil::replaceAllAnyRef(std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring::size_type pos = source.find_first_of(replaceWhat.c_str());
	while(pos != std::wstring::npos)
	{
		source.replace(pos, 1, replaceWithWhat);
		pos += replaceWithWhat.size();

		pos = source.find_first_of(replaceWhat.c_str(), pos);
	}
}
std::wstring stringUtil::replaceAllAny(const std::wstring &source, const std::wstring &replaceWhat, const std::wstring &replaceWithWhat)
{
	std::wstring result = source;
	replaceAllAnyRef(result, replaceWhat, replaceWithWhat);
	return result;
}
void stringUtil::replaceAllAnyRef(std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string::size_type pos = source.find_first_of(replaceWhat.c_str());
	while(pos != std::string::npos)
	{
		source.replace(pos, 1, replaceWithWhat);
		pos += replaceWithWhat.size();

		pos = source.find_first_of(replaceWhat.c_str(), pos);
	}
}
std::string stringUtil::replaceAllAny(const std::string &source, const std::string &replaceWhat, const std::string &replaceWithWhat)
{
	std::string result = source;
	replaceAllAnyRef(result, replaceWhat, replaceWithWhat);
	return result;
}

void stringUtil::trim(std::wstring& wstr, std::wstring delims /*= L" \t\r"*/, bool left /*= true*/, bool right /*= true*/ )
{
	//static const std::wstring delims = L" \t\r";
	if(right) wstr.erase(wstr.find_last_not_of(delims)+1);
	if(left) wstr.erase(0, wstr.find_first_not_of(delims));
	//static const String delims = " \t\r";
	//if(right)
	//	str.erase(str.find_last_not_of(delims)+1); // trim right
	//if(left)
	//	str.erase(0, str.find_first_not_of(delims)); // trim left
}
void stringUtil::trimLeft(std::wstring& wstr, std::wstring delims /*= L" \t\r"*/)
{
	trim(wstr, delims, true, false);
}
void stringUtil::trimRight(std::wstring& wstr, std::wstring delims /*= L" \t\r"*/)
{
	trim(wstr, delims, false, true);
}
void stringUtil::trim(std::string &str, std::string delims /*= " \t\r"*/, bool left /*= true*/, bool right /*= true*/ )
{
	if(right) str.erase(str.find_last_not_of(delims)+1);
	if(left) str.erase(0, str.find_first_not_of(delims));

}
void stringUtil::trimLeft(std::string &str, std::string delims /*= " \t\r"*/)
{
	trim(str, delims, true, false);
}
void stringUtil::trimRight(std::string &str, std::string delims /*= " \t\r"*/)
{
	trim(str, delims, false, true);
}

void stringUtil::toLower(std::wstring& wstr)
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), towlower);
}
void stringUtil::toLower(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
}
void stringUtil::toUpper(std::wstring &wstr)
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), towupper);
}
void stringUtil::toUpper(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}
bool stringUtil::startsWith(const std::wstring &wstr, const std::wstring &pattern, bool lowerCase /*= true*/)
{
	std::size_t thisLen = wstr.length();
	std::size_t pattLen = pattern.length();
	if(thisLen < pattLen || pattLen == 0) return false;

	std::wstring wsbegin = wstr.substr(0, pattLen);
	if(lowerCase) stringUtil::toLower(wsbegin);

	return (wsbegin == pattern);
}

bool stringUtil::startsWith(const std::string &str, const std::string &pattern, bool lowerCase /*= true*/)
{
	std::size_t thisLen = str.length();
	std::size_t pattLen = pattern.length();
	if(thisLen < pattLen || pattLen == 0) return false;

	std::string sbegin = str.substr(0, pattLen);
	if(lowerCase) stringUtil::toLower(sbegin);

	return (sbegin == pattern);
}
bool stringUtil::endsWith(const std::wstring &wstr, const std::wstring &pattern, bool lowerCase /*= true*/)
{
	std::size_t thisLen = wstr.length();
	std::size_t pattLen = pattern.length();
	if(thisLen < pattLen || pattLen == 0) return false;

	std::wstring wsend = wstr.substr(thisLen-pattLen);
	if(lowerCase) stringUtil::toLower(wsend);

	return wsend == pattern;
}
bool stringUtil::endsWith(const std::string &str, const std::string &pattern, bool lowerCase /*= true*/)
{
	std::size_t thisLen = str.length();
	std::size_t pattLen = pattern.length();
	if(thisLen < pattLen || pattLen == 0) return false;

	std::string send = str.substr(thisLen-pattLen);
	if(lowerCase) stringUtil::toLower(send);

	return send == pattern;
}

StringArrayType stringUtil::split(const std::wstring &wstr, const std::wstring &wsPattern
	, bool trimBlank /*= false*/, bool noBlank /*= true*/)
{
	assert(wsPattern.length() > 0);
	StringArrayType vecWs;
	std::wstring ws;
	std::wstring::size_type idx = 0;
	std::wstring::size_type found = wstr.find_first_of(wsPattern);
	while(found != std::wstring::npos)
	{
		ws = wstr.substr(idx, found-idx);
		if(trimBlank) stringUtil::trim(ws);
		if (noBlank) 
		{
			if(!ws.empty()) vecWs.push_back(ws);
		}
		else vecWs.push_back(ws);
		idx = found+1;
		found = wstr.find_first_of(wsPattern, found+1);
	}
	if(idx != wstr.length()) 
	{
		ws = wstr.substr(idx);
		if(!ws.empty())
		{
			if(trimBlank) stringUtil::trim(ws);
			if (noBlank) 
			{
				if(!ws.empty()) vecWs.push_back(ws);
			}
			else vecWs.push_back(ws);
		}
	}
	return vecWs;
}

SStringArrayType stringUtil::split(const std::string &str, const std::string &strPattern
	, bool trimBlank /*= false*/, bool noBlank /*= true*/)
{
	assert(strPattern.length() > 0);
	SStringArrayType vec;
	std::string ss;
	std::string::size_type idx = 0;
	std::string::size_type found = str.find_first_of(strPattern);
	while(found != std::wstring::npos)
	{
		ss = str.substr(idx, found-idx);
		if(trimBlank) stringUtil::trim(ss);
		if (noBlank) 
		{
			if(!ss.empty()) vec.push_back(ss);
		}
		else vec.push_back(ss);
		idx = found+1;
		found = str.find_first_of(strPattern, found+1);
	}
	if(idx != str.length()) 
	{
		ss = str.substr(idx);
		if(!ss.empty())
		{
			if(trimBlank) stringUtil::trim(ss);
			if (noBlank) 
			{
				if(!ss.empty()) vec.push_back(ss);
			}
			else vec.push_back(ss);
		}
	}
	return vec;
}

std::wstring stringUtil::toChs(int num, bool unit /*= true*/)
{
	static wchar_t c_chs[] = {L'零', L'一', L'二', L'三', L'四', L'五', L'六', L'七', L'八', L'九'};
	static wchar_t c_unit[] = {L'十', L'百', L'千', L'万', L'亿'};
	std::wstring wstr, wsret;
	wchar_t pbuf[80] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d", num);
	wstr = pbuf;
	if (wstr.length() == 2)
	{
		if (wstr[0] == L'1')
		{
			wsret.push_back(L'十');
			if(wstr[1] != L'0') wsret.push_back(c_chs[wstr[1] - L'0']);
			return wsret;
		}
		else if (wstr[0] == L'2')
		{
			wsret.push_back(L'廿');
			if(wstr[1] != L'0') wsret.push_back(c_chs[wstr[1] - L'0']);
			return wsret;
		}
	}
	int unitOffset = 0;
	// ascii 表中数值顺序为 “0 1 ... 9”
	for (std::wstring::const_iterator iter = wstr.cbegin(); iter != wstr.cend(); ++iter)
	{
		if(*iter == L'-') wsret.push_back(L'负');
		//else if(*iter == L'.') wsret.push_back(L'点');
		else
		{
			wsret.push_back(c_chs[*iter - L'0']);
			if (unit)
			{
				unitOffset = wstr.cend()-iter-2;
				if(unitOffset >= 0) wsret.push_back(c_unit[unitOffset]);
			}
		}
	}
	return wsret;
}
/*static*/ std::string stringUtil::toUrlEncode(const std::string &strUrlData, bool bUpperCaseFormat /*= true*/)
{
	assert(strUrlData.length() > 0);
	std::string strDest;
	std::string strFormat("%%%02");
	//const char baseChar = bUpperCaseFormat ? 'A' : 'a';
	if(bUpperCaseFormat) strFormat += 'X';
	else strFormat += 'x';
	//const char chs = 0xA0;
	char chr;
	for (std::string::const_iterator iter = strUrlData.cbegin(); iter != strUrlData.cend(); ++iter)
	{
		chr = *iter;
		if (std::isalpha(chr) || std::isdigit(chr) || chr == '-' || chr == '_' || chr == '.' || chr == '~')
		{
			strDest += *iter;
		}
		else if (chr == ' ')
		{
			strDest += '+';
		}
		else
		{
			char pbuf[4] = {0};
			std::sprintf(pbuf, strFormat.c_str(), (unsigned char)chr);
			strDest += pbuf;
		}
	}
	return strDest;
}
/*static*/ std::string stringUtil::toUrlDecode(const std::string &strUrlData)
{
	assert(strUrlData.length() > 0);
	std::string strDest;
	char chr, chd;
	for (std::string::const_iterator iter = strUrlData.cbegin(); iter != strUrlData.cend(); ++iter)
	{
		if (*iter == '%' && iter + 2 < strUrlData.cend())
		{
			chd = 0;
			//高位
			chr = *(iter + 1);
			if(chr >= 'A' && chr <= 'F')
				chd = (chr - 'A' + 10) << 4;
			else if(chr >= 'a' && chr <= 'f')
				chd = (chr - 'a' + 10) << 4;
			else
				chd = (chr - '0') << 4;


			//低位
			chr = *(iter + 2);
			if(chr >= 'A' && chr <= 'F')
				chd |= (chr - 'A' + 10);
			else if(chr >= 'a' && chr <= 'f')
				chd |= (chr - 'a' + 10);
			else
				chd |= (chr - '0');

			strDest += chd;
			iter += 2;	// 应该+3，但for中有++iter了
		}
		else if (*iter == '+')
		{
			strDest += ' ';
		}
		else
		{
			strDest += *iter;
		}
	}
	return strDest;
}

std::wstring stringUtil::toUrlEncode(const std::wstring &wsUrlData, bool bUpperCaseFormat /*= true*/)
{
	assert(wsUrlData.length() > 0);
	std::string strUrlUtf8 = stringUtil::toChar(wsUrlData.c_str(), CP_UTF8);
	std::string strDest = toUrlEncode(strUrlUtf8, bUpperCaseFormat);

	return stringUtil::toWChar(strDest.c_str(), CP_UTF8);
}
std::wstring stringUtil::toUrlDecode(const std::wstring &wsUrlData)
{
	assert(wsUrlData.length() > 0);
	std::string strUrlUtf8 = stringUtil::toChar(wsUrlData.c_str(), CP_UTF8);
	std::string strDest = toUrlDecode(strUrlUtf8);

	return stringUtil::toWChar(strDest.c_str(), CP_UTF8);

}

std::string stringUtil::toUTF8(const std::wstring &wsData)
{
	std::string strUtf8;
	gtc::UTF16Encoding utf16;
	gtc::UTF8Encoding utf8;
	gtc::textConverter conv(utf16, utf8);
	conv.convert(gtc::stringUtil::toChar(wsData.c_str(), CP_UTF8), strUtf8);
	
	return strUtf8;

}
}	// gtc