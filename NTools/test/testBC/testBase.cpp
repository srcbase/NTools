#include "stdafx.h"
#include "testBase.h"
std::ostream& operator << (std::ostream &os, const std::wstring &wstr)
{
	os << gtc::stringUtil::toChar(wstr);
	return os;
}

/*static*/ void testString()
{
	std::wstring ws;
	std::string ss;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 stringUtil ..." << std::endl;
	std::cout << "stringUtil::iCompareA abc,Abc:" << gtc::stringUtil::iCompare("abc", "Abc")
		<< "	abcd, abc:" << gtc::stringUtil::iCompare("abcd", "abc")
		<< "	bbc, abc:" << gtc::stringUtil::iCompare("bbc", "abc")
		<< "	abc, Abcd:" << gtc::stringUtil::iCompare("abc", "Abcd")
		<< "	abc, bbc:" << gtc::stringUtil::iCompare("abc", "bbc") << std::endl;
	std::cout << "stringUtil::iCompareW abc,Abc:" << gtc::stringUtil::iCompare(L"abc", L"Abc")
		<< "	abcd, abc:" << gtc::stringUtil::iCompare(L"abcd", L"abc")
		<< "	bbc, abc:" << gtc::stringUtil::iCompare(L"bbc", L"abc")
		<< "	abc, Abcd:" << gtc::stringUtil::iCompare(L"abc", L"Abcd")
		<< "	abc, bbc:" << gtc::stringUtil::iCompare(L"abc", L"bbc") << std::endl;


	std::wstring wstr(L"abcABC123汉字AbC");
	std::string str("abcABC123汉字AbC");
	gtc::stringUtil::toLower(wstr);
	gtc::stringUtil::toLower(str);
	std::cout << "stringUtil::toLowerA abcABC123汉字AbC: " << str.c_str() << std::endl;
	//std::wcout << L"stringUtil::toLowerW abcABC123汉字AbC:" << wstr.c_str() << std::endl;
	gtc::stringUtil::toUpper(wstr);
	gtc::stringUtil::toUpper(str);
	std::cout << "stringUtil::toUpperA abcABC123汉字AbC: " << str.c_str() << std::endl;

	std::cout << "startsWith(" << str << ",abca, true): " << gtc::stringUtil::startsWith(str, "abca") << std::endl;
	std::cout << "startsWith(" << str << ",abca, false): " << gtc::stringUtil::startsWith(str, "abca", false) << std::endl;
	std::cout << "startsWithW(" << str << ",abca, true): " << gtc::stringUtil::startsWith(wstr, L"abca") << std::endl;
	std::cout << "startsWithW(" << str << ",abca, false): " << gtc::stringUtil::startsWith(wstr, L"abca", false) << std::endl;
	std::cout << "endsWith(" << str << ",字AbC, true): " << gtc::stringUtil::endsWith(str, "字AbC") << std::endl;
	std::cout << "endsWith(" << str << ",字AbC, false): " << gtc::stringUtil::endsWith(str, "字AbC", false) << std::endl;
	std::cout << "endsWithW(" << str << ",字AbC, true): " << gtc::stringUtil::endsWith(wstr, L"字AbC") << std::endl;
	std::cout << "endsWithW(" << str << ",字AbC, false): " << gtc::stringUtil::endsWith(wstr, L"字AbC", false) << std::endl;
	std::cout << "endsWithW(" << str << ",字ABC, false): " << gtc::stringUtil::endsWith(wstr, L"字ABC", false) << std::endl;

	wstr = L" abc, 123,qwwe,汉字 ,. ,s s";
	str = " abc, 123,qwwe,汉字 ,. ,s s";
	StringArrayType vecWstr = gtc::stringUtil::split(wstr, L",");
	std::cout << "split(' abc, 123,qwwe,汉字 ,,s s', ',', 不移除首尾空格, 去除空项): size=" << vecWstr.size() << std::endl;
	for each (const std::wstring &var in vecWstr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;
	vecWstr = gtc::stringUtil::split(wstr, L",.", true, true);
	std::cout << "split('" << str << "', ',.', 移除首尾空格, 去除空项): size=" << vecWstr.size() << std::endl;
	for each (const std::wstring &var in vecWstr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;
	vecWstr = gtc::stringUtil::split(wstr, L",.", false, false);
	std::cout << "split('" << str << "', ',.', 不移除首尾空格, 不去除空项): size=" << vecWstr.size() << std::endl;
	for each (auto var in vecWstr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;
	vecWstr = gtc::stringUtil::split(wstr, L",.", true, false);
	std::cout << "split('" << str << "', ',.', 移除首尾空格, 不去除空项): size=" << vecWstr.size() << std::endl;
	for each (auto var in vecWstr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;

	SStringArrayType vecStr = gtc::stringUtil::split(str, ",");
	std::cout << "splitA(' abc, 123,qwwe,汉字 ,,s s', ',', 不移除首尾空格, 去除空项): size=" << vecStr.size() << std::endl;
	for each (const std::string &var in vecStr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;
	vecStr = gtc::stringUtil::split(str, ",.", true, true);
	std::cout << "splitA('" << str << "', ',.', 移除首尾空格, 去除空项): size=" << vecStr.size() << std::endl;
	for each (const std::string &var in vecStr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;
	vecStr = gtc::stringUtil::split(str, ",.", false, false);
	std::cout << "splitA('" << str << "', ',.', 不移除首尾空格, 不去除空项): size=" << vecStr.size() << std::endl;
	for each (auto var in vecStr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;
	vecStr = gtc::stringUtil::split(str, ",.", true, false);
	std::cout << "splitA('" << str << "', ',.', 移除首尾空格, 不去除空项): size=" << vecStr.size() << std::endl;
	for each (auto var in vecStr)
	{
		std::cout << "<" << var << "> ";
	}
	std::cout << std::endl;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 2016-2-1		kp007	替换
	wstr = L"abc123汉字￥#%789xyz";
	str = "abc123汉字￥#%789xyz";
	ws = wstr;
	ss = str;
	gtc::stringUtil::replaceAll(ws, L"789", L"0000");
	gtc::stringUtil::replaceAll(ss, "789", "0000");
	std::cout << "replaceAll('" << wstr << "', '789', '0000'): " << ws << std::endl;
	std::cout << "replaceAllA('" << str << "', '789', '0000'): " << ss << std::endl;
	ws = wstr;
	ss = str;
	gtc::stringUtil::replaceAll(ws, L"789", L"");
	gtc::stringUtil::replaceAll(ss, "789", "");
	std::cout << "replaceAll('" << wstr << "', '789', ''): " << ws << std::endl;
	std::cout << "replaceAllA('" << str << "', '789', ''): " << ss << std::endl;
	ws = wstr;
	ss = str;
	gtc::stringUtil::replaceAllAny(ws, L"178", L"**");
	gtc::stringUtil::replaceAllAny(ss, "178", "**");
	std::cout << "replaceAllAny('" << wstr << "', '178', '**'): " << ws << std::endl;
	std::cout << "replaceAllAnyA('" << str << "', '178', '**'): " << ss << std::endl;
	ws = wstr;
	ss = str;
	gtc::stringUtil::replaceAllAny(ws, L"178", L"");
	gtc::stringUtil::replaceAllAny(ss, "178", "");
	std::cout << "replaceAllAny('" << wstr << "', '178', ''): " << ws << std::endl;
	std::cout << "replaceAllAnyA('" << str << "', '178', ''): " << ss << std::endl;
	std::cout << std::endl;

	wstr = L"<name='abc' color='#12345678'>";
	str = "<name='abc' color='#12345678'>";
	ws = wstr;
	ss = str;
	gtc::stringUtil::replaceFirst(ws, L"<", L"[");
	gtc::stringUtil::replaceLast(ws, L">", L"]");
	std::cout << "replaceFirst replaceLast('" << wstr << "', '< or >', '[ or ]'): " << ws << std::endl;
	gtc::stringUtil::replaceFirst(ss, "<", "[");
	gtc::stringUtil::replaceLast(ss, ">", "]");
	std::cout << "replaceFirstA replaceLastA('" << str << "', '< or >', '[ or ]'): " << ss << std::endl;
	std::cout << std::endl;

	std::cout << "toChinese(1234567890,有单位):" << gtc::stringUtil::toChinese(1234567890, true) << std::endl;
	std::cout << "toChinese(-1234567000,有单位):" << gtc::stringUtil::toChinese(-1234567000, true) << std::endl;
	std::cout << "toChinese(-1000067000,有单位):" << gtc::stringUtil::toChinese(-1000067000, true) << std::endl;
	std::cout << "toChinese(1234567890,无单位):" << gtc::stringUtil::toChinese(1234567890, false) << std::endl;
	std::cout << "toChinese(-1234567890,无单位):" << gtc::stringUtil::toChinese(-1234567890, false) << std::endl;

	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 converter ..." << std::endl;
	gtc::real rl = 123.456;
	std::cout << "toStringA(" << rl << "): " << gtc::converter::toStringA(rl) << std::endl;
	std::cout << "toString(" << rl << "): " << gtc::converter::toString(rl) << std::endl;
	std::cout << "toStringA(" << rl << "): " << gtc::converter::toStringA(123.654) << std::endl;
	std::cout << "toString(" << rl << "): " << gtc::converter::toString(123.654) << std::endl;
	std::cout << "parseReal(123.654): " << gtc::converter::parseReal("123.654") << std::endl;
	std::cout << "parseReal(123.654): " << gtc::converter::parseReal(L"123.654") << std::endl;
	std::cout << "parseReal(A123.654): " << gtc::converter::parseReal("A123.654") << std::endl;
	std::cout << "parseReal(A123.654): " << gtc::converter::parseReal(L"A123.654") << std::endl;
	std::cout << "parseReal(123.654A): " << gtc::converter::parseReal("123.654A") << std::endl;
	std::cout << "parseReal(123.654A): " << gtc::converter::parseReal(L"123.654A") << std::endl;
	std::cout << "parseReal(123A.654): " << gtc::converter::parseReal("123A.654") << std::endl;
	std::cout << "parseReal(123A.654): " << gtc::converter::parseReal(L"123A.654") << std::endl;
	std::cout << std::endl;

	std::cout << "toStringA(true): " << gtc::converter::toStringA(true) << std::endl;
	std::cout << "toString(true): " << gtc::converter::toString(true) << std::endl;
	std::cout << "toStringA(false): " << gtc::converter::toStringA(false) << std::endl;
	std::cout << "toString(false): " << gtc::converter::toString(false) << std::endl;
	std::cout << "toStringA(true, true): " << gtc::converter::toStringA(true, true) << std::endl;
	std::cout << "toString(true, true): " << gtc::converter::toString(true, true) << std::endl;
	std::cout << "toStringA(false, true): " << gtc::converter::toStringA(false, true) << std::endl;
	std::cout << "toString(false, true): " << gtc::converter::toString(false, true) << std::endl;
	std::cout << "parseBool(true, yes, 1, on, ab)" << gtc::converter::parseBool("true") << ", "
		<< gtc::converter::parseBool("yes") << ", "
		<< gtc::converter::parseBool("1") << ", "
		<< gtc::converter::parseBool("on") << ", "
		<< gtc::converter::parseBool("ab") << std::endl;
	std::cout << "parseBool(true, yes, 1, on, ab)" << gtc::converter::parseBool(L"true") << ", "
		<< gtc::converter::parseBool(L"yes") << ", "
		<< gtc::converter::parseBool(L"1") << ", "
		<< gtc::converter::parseBool(L"on") << ", "
		<< gtc::converter::parseBool(L"ab") << std::endl;
	std::cout << "parseBool(false, no, 0, off, ab)" << gtc::converter::parseBool("false") << ", "
		<< gtc::converter::parseBool("no") << ", "
		<< gtc::converter::parseBool("0") << ", "
		<< gtc::converter::parseBool("off") << ", "
		<< gtc::converter::parseBool("ab") << std::endl;
	std::cout << "parseBool(false, no, 0, off, ab)" << gtc::converter::parseBool(L"false") << ", "
		<< gtc::converter::parseBool(L"no") << ", "
		<< gtc::converter::parseBool(L"0") << ", "
		<< gtc::converter::parseBool(L"off") << ", "
		<< gtc::converter::parseBool(L"ab") << std::endl;

	std::cout << "toStringA(-123)" << gtc::converter::toStringA(-123) << std::endl;
	std::cout << "toString(123)" << gtc::converter::toString(123) << std::endl;
	assert(gtc::converter::parseInt(L"123") == 123);
	assert(gtc::converter::parseInt(L" 123 ") == 123);
	assert(gtc::converter::parseInt(L"-123") == -123);
	assert(gtc::converter::parseInt(L" -123 ") == -123);
	assert(gtc::converter::parseInt("123") == 123);
	assert(gtc::converter::parseInt(" 123 ") == 123);
	assert(gtc::converter::parseInt("-123") == -123);
	assert(gtc::converter::parseInt(" -123 ") == -123);

	std::cout << "toStringA(" << 0x01020304 << "): " << std::hex << gtc::converter::toStringA(std::size_t(0x01020304)) << std::endl;
	std::cout << "toString(" << 0x01020304 << "): " << std::hex << gtc::converter::toString(std::size_t(0x01020304)) << std::endl;
	assert(gtc::converter::parseSizeT(L"123") == 123);
	assert(gtc::converter::parseSizeT(L" 123 ") == 123);
	std::size_t st = ~123 + 1;
	assert(gtc::converter::parseSizeT(L"-123") == (~123+1));
	assert(gtc::converter::parseSizeT(L" -123 ") == st);
	assert(gtc::converter::parseSizeT("123") == 123);
	assert(gtc::converter::parseSizeT(" 123 ") == 123);
	assert(gtc::converter::parseSizeT("-123") == st);
	assert(gtc::converter::parseSizeT(" -123 ") == st);

	gtc::point pt;
	std::cout << "point: " << gtc::converter::toStringA(pt) << "    " << gtc::converter::toString(pt) << std::endl;
	pt.x = 10;
	pt.y = 20;
	std::cout << "point: " << gtc::converter::toString(pt) << std::endl;
	assert(pt == gtc::converter::parsePoint(L"10,20"));
	std::cout << "point: parsePoint(10,20) = " << gtc::converter::toString(gtc::converter::parsePoint(L"10,20")) << std::endl;
	std::cout << "point: parsePointA(10,20) = " << gtc::converter::toString(gtc::converter::parsePoint("10,20")) << std::endl;
	std::cout << "point: parsePoint(abc) = " << gtc::converter::toString(gtc::converter::parsePoint(L"abc")) << std::endl;
	std::cout << std::endl;

	gtc::size sz;
	std::cout << "size: " << gtc::converter::toStringA(sz) << "    " << gtc::converter::toString(sz) << std::endl;
	sz.width = 50;
	sz.height = 100;
	std::cout << "size: " << gtc::converter::toString(sz) << std::endl;
	std::cout << "size: parseSize(30, 60) = " << gtc::converter::toString(gtc::converter::parseSize(L"30, 60")) << std::endl;
	std::cout << "size: parseSizeA(30, 60) = " << gtc::converter::toString(gtc::converter::parseSize("30, 60")) << std::endl;
	std::cout << "size: parseSize(abc) = " << gtc::converter::toString(gtc::converter::parseSize(L"abc")) << std::endl;
	std::cout << std::endl;

	gtc::rect rc;
	std::cout << "rect: " << gtc::converter::toStringA(rc) << "    " << gtc::converter::toString(rc) << std::endl;
	rc.x = 10;
	rc.y = 20;
	rc.width = 50;
	rc.height = 100;
	std::cout << "rect: " << gtc::converter::toString(rc) << std::endl;
	std::cout << "rect: parseRect(30, 60, 10, 20) = " << gtc::converter::toString(gtc::converter::parseRect(L"30, 60, 10, 20")) << std::endl;
	std::cout << "rect: parseRectA(30, 60, 10, 20) = " << gtc::converter::toString(gtc::converter::parseRect("30,60,10,20")) << std::endl;
	std::cout << "rect: parseRect(abc) = " << gtc::converter::toString(gtc::converter::parseRect(L"abc")) << std::endl;
	std::cout << std::endl;

	gtc::datetime dtcur;
	std::cout << "datetime cur UTC: " << gtc::converter::toStringA(dtcur) << "   " << gtc::converter::toString(dtcur) << std::endl;
	dtcur.makeLocal();
	std::cout << "datetime cur LOCAL: " << gtc::converter::toStringA(dtcur) << "   " << gtc::converter::toString(dtcur) << std::endl;
	gtc::datetime dt;
	dt.assign(2016, 2, 3);
	std::cout << "datetime UTC: " << gtc::converter::toStringA(dt) << "   " << gtc::converter::toString(dt) << std::endl;
	dtcur.makeLocal();
	std::cout << "datetime LOCAL: " << gtc::converter::toStringA(dt) << "   " << gtc::converter::toString(dt) << std::endl;

	dt = gtc::converter::parseDatetime(L"2016-2-03 12:2:23");
	std::cout << "datetime " << gtc::converter::toString(dt) << std::endl;
	dt = gtc::converter::parseDatetime(L"2016-2-03");
	std::cout << "datetime " << gtc::converter::toString(dt) << std::endl;
	dt = gtc::converter::parseDatetime(L"12:2:23");
	std::cout << "datetime " << gtc::converter::toString(dt) << std::endl;
	std::cout << "datetime: parseDatetime(12:2:23) = " << gtc::converter::toString(gtc::converter::parseDatetime(L"12:2:23")) << std::endl;
	std::cout << "datetime: parseDatetime(2000-2-3) = " << gtc::converter::toString(gtc::converter::parseDatetime(L"2000-2-3")) << std::endl;
	std::cout << "datetime: parseDatetime(2000-2-3 a:b) = " << gtc::converter::toString(gtc::converter::parseDatetime(L"2000-2-3 a:b")) << std::endl;
	std::cout << "datetime: parseDatetime(2000-22-3 0:0) = " << gtc::converter::toString(gtc::converter::parseDatetime("2000-22-3 0:0")) << std::endl;




}


void testBitwise()
{
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 bitwise ..." << std::endl;
	std::cout << "isPO2 " << std::endl;
	for (int idx = 0; idx < 34; ++idx)
	{
		std::cout << idx << ":" << gtc::bitwise::isPO2(idx) << "\t";
		if ((idx+1) % 4 == 0)  std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "byteSwap " << std::hex << std::endl;
	std::cout << "  0x1234: " << gtc::bitwise::byteSwap(uint16_t(0x1234));
	std::cout << "  0x12345678: " << gtc::bitwise::byteSwap(uint32_t(0x12345678));
	std::cout << "  0x123456789abcdef0: " << gtc::bitwise::byteSwap(uint64_t(0x123456789abcdef0));
	std::cout << std::endl;
}
#define OGRE_TOKEN_PASTE(x, y) x ## y
#define OGRE_TOKEN_PASTE_EXTRA(x, y) OGRE_TOKEN_PASTE(x, y)
#define OGRE_LOCK_MUTEX(name) std::wstring OGRE_TOKEN_PASTE_EXTRA(ogrenameLock, __LINE__) (name)


class generalData
#if GTC_MEMORY_ALLOCATOR
	: public gtc::generalAlloc
#endif
{
public:
	generalData(int id = 0);
	~generalData();

public:
	void setID(int id) { _id = id;	 }
private:
	//std::wstring _wsName;
	int			_id;
};

generalData::generalData(int id)
	: _id(id)
{
}

generalData::~generalData()
{
}

void testMemory()
{
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 alignedMemory ..." << std::endl;
	//uint8_t *p1 = new uint8_t();
	//uint8_t *p2 = new uint8_t();
	//uint8_t *p3 = new uint8_t();
	//std::cout << "allocate " << std::hex << std::setfill('0') << std::endl;
	//std::cout << "  p1: " << std::setw(8) << std::size_t(p1) << std::endl;
	//std::cout << "  p2: " << std::setw(8) << std::size_t(p2) << std::endl;
	//std::cout << "  p3: " << std::setw(8) << std::size_t(p3) << std::endl;

#if GTC_MEMORY_ALLOCATOR

	uint8_t *p4 = (uint8_t *)gtc::alignedMemory::allocate(3);
	std::cout << "  p4: " << std::setw(8) << std::size_t(p4) << std::endl;
	gtc::alignedMemory::deallocate(p4);

	//std::cout << "准备测试 stdAllocPolicy ..." << std::endl;
	//char *pstd1 = (char *)gtc::stdAllocPolicy::allocateBytes(8, __FILE__, __LINE__, __FUNCTION__);
	//char *pstd2 = (char *)gtc::stdAllocPolicy::allocateBytes(15, __FILE__, __LINE__, __FUNCTION__);
	//char *pstd3 = (char *)gtc::stdAllocPolicy::allocateBytes(55, __FILE__, __LINE__, __FUNCTION__);
	//std::cout << "	maxAllocSize: 0x" << gtc::stdAllocPolicy::getMaxAllocationSize() << std::endl;
	//std::cout << "  pstd1: " << std::setw(8) << std::size_t(pstd1) << std::endl;
	//std::cout << "  pstd2: " << std::setw(8) << std::size_t(pstd2) << std::endl;
	//std::cout << "  pstd3: " << std::setw(8) << std::size_t(pstd3) << std::endl;
	//gtc::memoryTracker::get().reportLeaks();	// 应该报告3个泄漏
	//gtc::stdAllocPolicy::deallocateBytes(pstd1);
	//gtc::stdAllocPolicy::deallocateBytes(pstd3);
	//gtc::memoryTracker::get().reportLeaks();	// 应该报告1个泄漏
	//gtc::stdAllocPolicy::deallocateBytes(pstd2);
	//gtc::memoryTracker::get().reportLeaks();	// 没有泄漏了

	std::cout << "准备测试 AllocatedObject ..." << std::endl;
	generalData *pdata = GTC_NEW generalData();
	gtc::memoryTracker::get().reportLeaks();	// 应该报告1个泄漏
	GTC_DELETE pdata;
	gtc::memoryTracker::get().reportLeaks();	// 没有泄漏了
	pdata = new generalData();			// 直接用new delete时记录分配过程，但没有详细日志
	gtc::memoryTracker::get().reportLeaks();	// 应该报告1个泄漏
	delete pdata;
	gtc::memoryTracker::get().reportLeaks();	// 没有泄漏了

	generalData *parray = GTC_NEW_ARRAY_T(generalData, 100, gtc::BEMemoryCategoryGeneral);
	for (int idx = 0; idx < 100; ++idx)
	{
		parray[idx].setID(idx);
	}
	generalData *pdata1 = GTC_NEW_T(generalData, gtc::BEMemoryCategoryGeneral);
	gtc::memoryTracker::get().reportLeaks();	// 应该报告2个泄漏
	GTC_DELETE_ARRAY_T(parray, generalData, 100, gtc::BEMemoryCategoryGeneral);
	GTC_DELETE_T(pdata1, generalData, gtc::BEMemoryCategoryGeneral);
	gtc::memoryTracker::get().reportLeaks();	// 没有泄漏了

	std::vector<generalData, gtc::STLAllocator<generalData, gtc::generalAllocPolicy>> vecData(9);
	gtc::memoryTracker::get().reportLeaks();	// 有2泄漏了

	gtc::vector<generalData>::type vecData2(18);
	gtc::memoryTracker::get().reportLeaks();	// 有2+2泄漏了
#endif

	std::cout << std::endl;
}

void testApp()
{
	std::wstring wstr;
	std::string str;
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 app ..." << std::endl;
	std::cout << "	getAppPath: " << gtc::stringUtil::toChar(gtc::app::getAppPath()) << std::endl;
	gtc::app::init(L"app.ini", L"app_data", true);
	std::cout << "	getConfigPath: " << gtc::stringUtil::toChar(gtc::app::getConfigPath()) << std::endl;
	std::cout << "	getLogPath: " << gtc::stringUtil::toChar(gtc::app::getLogPath()) << std::endl;

	std::cout << "	os: " << gtc::toString(gtc::app::getOSVersion()) << std::endl;
	std::cout << "	isWindowsXP: " << gtc::app::isWindowsXP() << std::endl;
	std::cout << "	isWindowsVistaLater: " << gtc::app::isWindowsVistaLater() << std::endl;
	std::cout << "	isWindows7Later: " << gtc::app::isWindows7Later() << std::endl;
	std::cout << "	isWindows8Later: " << gtc::app::isWindows8Later() << std::endl;
	std::cout << "	isWindowsOS64: " << gtc::app::isWindowsOS64() << std::endl;

	std::cout << "	getProductVersion: " << std::hex << gtc::app::getProductVersion() << std::endl;
	std::cout << "	getProductVersion D:\\bin_kp\\KPSglw\\KPSglwData.dll: " << std::hex << gtc::app::getProductVersion(L"D:\\bin_kp\\KPSglw\\KPSglwData.dll") << std::endl;

	gtc::app::writeProfile(L"MAIN", L"STR", L"ABC123汉字");
	assert(gtc::app::readProfile(L"MAIN", L"STR", wstr) && wstr == L"ABC123汉字");
	RECT rc = { 1,2,3,4 };
	gtc::app::writeProfile(L"MAIN", L"RC", rc);
	memset(&rc, 0, sizeof(rc));
	std::cout << "readProfileRect rect:" << gtc::app::readProfileRect(L"MAIN", L"RC", rc);
	std::cout << "<" << gtc::converter::toString(rc) << ">" << std::endl;

	POINT pt = { 10, 30 };
	gtc::app::writeProfile(L"MAIN", L"PT", pt);
	memset(&pt, 0, sizeof(pt));
	std::cout << "readProfileRect point:" << gtc::app::readProfilePoint(L"MAIN", L"PT", pt);
	std::cout << "<" << gtc::converter::toString(pt) << ">" << std::endl;

	gtc::app::writeProfile(L"MAIN", L"INT", 123456);
	int ival = 0;
	std::cout << "readProfileRect int:" << gtc::app::readProfileInt(L"MAIN", L"INT", ival);
	std::cout << "<" << std::dec << ival << ">" << std::endl;
	std::cout << std::endl;

	gtc::app::writeLogs("这是一条测试！！！", "测试标题");
	gtc::app::writeLogs("这是一条测试！！！");
	gtc::app::writeLogs(L"这是一条测试！！！", L"测试标题");
	gtc::app::writeLogs(L"这是一条测试！！！");

	//gtc::app::message(L"这是一个测试");
	//gtc::app::message(L"这是一个测试", L"abc", MB_OK, true);
	//gtc::app::messageEx(L"%s %s abc", L"这是一个测试", L"123");

	SEND_LOG(L"%s %s abc", L"这是一个测试", L"123");
	SEND_LOGA("%s %s abc", "这是一个测试", "123");

}
void testObject()
{
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 基本对象 ..." << std::endl;
	gtc::point pt(10.2, 10.8);
	std::cout << "point: " << gtc::converter::toStringA(pt) << " " << gtc::converter::toString(pt) << std::endl;
	std::cout << "point + 10,20 : " << gtc::converter::toString(pt + gtc::point(10, 20)) << std::endl;
	std::cout << "point - 10,20 : " << gtc::converter::toString(pt - gtc::point(10, 20)) << std::endl;
	assert(pt == gtc::point(10.2, 10.8));
	pt.offset(5, 5);
	std::cout << "point offset(5, 5): " << gtc::converter::toString(pt) << std::endl;
	POINT pp = pt;
	std::cout << "POINT :" << pp.x << "," << pp.x << std::endl;
	pt = pp;
	std::cout << "point <- POINT:" << gtc::converter::toString(pt) << std::endl;

	gtc::size sz(20.3, 30.9);
	std::cout << "size: " << gtc::converter::toStringA(sz) << "  " << gtc::converter::toString(sz) << std::endl;
	std::cout << "size + 10,20 : " << gtc::converter::toString(sz + gtc::size(10, 20)) << std::endl;
	std::cout << "size - 10,20 : " << gtc::converter::toString(sz - gtc::size(10, 20)) << std::endl;
	assert(sz == gtc::size(20.3, 30.9));
	SIZE ss = sz;
	std::cout << "SIZE: " << gtc::converter::toString(ss) << std::endl;
	sz = ss;
	std::cout << "size <- SIZE:" << gtc::converter::toString(sz) << std::endl;

	gtc::rect rc(10, 10, 50, 100);
	std::cout << "rect: " << gtc::converter::toStringA(rc) << "   " << gtc::converter::toString(rc) << std::endl;
	assert(rc.contains(10, 10));
	assert(!rc.contains(60, 110));
	rc.inflate(5, 5);
	assert(rc == gtc::rect(5, 5, 60, 110));
	rc.intersect(gtc::rect(5, 10, 55, 130));
	assert(rc == gtc::rect(5, 10, 55, 105));
	rc.Union(gtc::rect(3, 15, 30, 110));
	assert(rc == gtc::rect(3, 10, 57, 115));
}
std::mutex c_mutex;
std::deque <std::packaged_task<int(int)>> c_dequeTask;
void thread_do_task()
{
	static int c_idx = 0;
	while (true)
	{
		//SEND_LOG(L"thread_do_task 准备干活了");
		std::packaged_task<int(int)> task;
		{
			std::lock_guard<std::mutex> lk(c_mutex);
			if (c_dequeTask.empty())
			{
				//SEND_LOG(L"thread_do_task 没有任务");
				continue;
			}
			task = std::move(c_dequeTask.front());
			c_dequeTask.pop_front();
		}
		task(c_idx++);
	}

}
int do_task_work(int idx)
{
	SEND_LOG(L"do_task_work begin %d", idx);

	std::this_thread::sleep_for(std::chrono::seconds(2));

	SEND_LOG(L"do_task_work end %d", idx);
	return idx + 1000;
}

template<typename FUNC> 
std::future<int> post_task(FUNC f)
{
	std::packaged_task<int(int)> task(f);
	std::future<int> rt = task.get_future();
	std::lock_guard<std::mutex> lk(c_mutex);
	c_dequeTask.push_back(std::move(task));
	return rt;
}
void testThread()
{
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "准备测试 线程 ..." << std::endl;
	std::thread thr(thread_do_task);
	thr.detach();
	try
	{
		post_task(do_task_work);
		post_task(do_task_work);
		post_task(do_task_work);
		std::future<int> res = post_task(do_task_work);
		SEND_LOG(L"testThread 任务返回值：%d", res.get());

	}
	catch (const std::exception& e)
	{
		SEND_LOGA("ERROR testThread <%s>", e.what());
	}
	catch (...)
	{
		SEND_LOG(L"ERROR testThread TRY");
	}
}