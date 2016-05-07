#include "datetime.h"
#include <limits>
#include "stringUtil.h"
#include "baseException.h"
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	timezone  类		2015-8-5
//	时区封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
int timezone::utcOffset()
{
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	return -tzInfo.Bias*60;
}


int timezone::dst()
{
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	return dstFlag == TIME_ZONE_ID_DAYLIGHT ? -tzInfo.DaylightBias*60 : 0;
}


bool timezone::isDst(const timestamp& timeStamp)
{
	std::time_t time = timeStamp.epochTime();
	struct std::tm* tms = std::localtime(&time);
	if (!tms) GTC_EXCEPT(baseException::BSExceptionCodeParseFailed, L"cannot get local time DST flag"); //throw Poco::SystemException("cannot get local time DST flag");
	return tms->tm_isdst > 0;
}


std::string timezone::name()
{
	std::string result;
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	WCHAR* ptr = dstFlag == TIME_ZONE_ID_DAYLIGHT ? tzInfo.DaylightName : tzInfo.StandardName;
#if defined(POCO_WIN32_UTF8)
	UnicodeConverter::toUTF8(ptr, result);
#else
	char buffer[256];
	DWORD rc = WideCharToMultiByte(CP_ACP, 0, ptr, -1, buffer, sizeof(buffer), NULL, NULL);
	if (rc) result = buffer;
#endif
	return result;
}


std::string timezone::standardName()
{
	std::string result;
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	WCHAR* ptr = tzInfo.StandardName;
#if defined(POCO_WIN32_UTF8)
	UnicodeConverter::toUTF8(ptr, result);
#else
	char buffer[256];
	DWORD rc = WideCharToMultiByte(CP_ACP, 0, ptr, -1, buffer, sizeof(buffer), NULL, NULL);
	if (rc) result = buffer;
#endif
	return result;
}


std::string timezone::dstName()
{
	std::string result;
	TIME_ZONE_INFORMATION tzInfo;
	DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
	WCHAR* ptr = tzInfo.DaylightName;
#if defined(POCO_WIN32_UTF8)
	UnicodeConverter::toUTF8(ptr, result);
#else
	char buffer[256];
	DWORD rc = WideCharToMultiByte(CP_ACP, 0, ptr, -1, buffer, sizeof(buffer), NULL, NULL);
	if (rc) result = buffer;
#endif
	return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	timespan  类		2015-8-4
//	时间差封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
const int64_t timespan::MILLISECONDS = 1000;
const int64_t timespan::SECONDS      = 1000*timespan::MILLISECONDS;
const int64_t timespan::MINUTES      =   60*timespan::SECONDS;
const int64_t timespan::HOURS        =   60*timespan::MINUTES;
const int64_t timespan::DAYS         =   24*timespan::HOURS;
timespan::timespan()
	: m_span(0)
{

}
timespan::timespan(int64_t microseconds)
	: m_span(microseconds)
{}
timespan::timespan(long seconds, long microseconds)
	: m_span(int64_t(seconds)*SECONDS + microseconds)
{

}
timespan::timespan(int days, int hours, int minutes, int seconds, int microSeconds)
	: m_span(int64_t(days)*DAYS + int64_t(hours)*HOURS + int64_t(minutes)*MINUTES + int64_t(seconds)*seconds + microSeconds)
{

}
timespan::timespan(const timespan& timespan)
	: m_span(timespan.m_span)
{

}
timespan::~timespan()
{

}



inline int timespan::hour() const
{
	return int((m_span/HOURS)%24);
}
inline int timespan::hours() const
{
	return int(m_span/HOURS);
}
/// Returns the total number of hours.

inline int timespan::minute() const
{
	return int((m_span/MINUTES)%60);
}
/// Returns the number of minutes (0 to 59).

inline int timespan::minutes() const
{
	return int(m_span/MINUTES);
}
/// Returns the total number of minutes.

inline int timespan::second() const
{
	// 不能用“return seconds()%60;”这种形式，会影响精度
	return int((m_span/SECONDS)%60);
}
/// Returns the number of seconds (0 to 59).

inline int timespan::seconds() const
{
	return int(m_span/SECONDS);
}
/// Returns the total number of seconds.

inline int timespan::millisecond() const
{
	return int((m_span/MILLISECONDS)%1000);
}
/// Returns the number of milliseconds (0 to 999).

inline int64_t timespan::milliseconds() const
{
	return m_span/MILLISECONDS;
}
/// Returns the total number of milliseconds.

inline int timespan::microsecond() const
{
	return m_span % 1000;
}
/// Returns the fractions of a millisecond
/// in microseconds (0 to 999).

inline int timespan::useconds() const
{
	return m_span % 1000000;
}
/// Returns the fractions of a second
/// in microseconds (0 to 999999).

inline int64_t timespan::microseconds() const
{
	return m_span;
}
timespan& timespan::operator = (const timespan& timespan)
{
	m_span = timespan.m_span;
	return *this;
}
timespan& timespan::operator = (int64_t microseconds)
{
	m_span = microseconds;
	return *this;
}
timespan& timespan::assign(int days, int hours, int minutes, int seconds, int microSeconds)
{
	m_span = int64_t(days)*DAYS + int64_t(hours)*HOURS + int64_t(minutes)*MINUTES + int64_t(seconds)*seconds + microSeconds;
	return *this;
}
timespan& timespan::assign(long seconds, long microseconds)
{
	m_span = int64_t(seconds)*SECONDS + microseconds;
	return *this;
}

timespan timespan::operator + (const timespan& d) const
{
	return timespan(m_span + d.m_span);
}
timespan timespan::operator - (const timespan& d) const
{
	return timespan(m_span - d.m_span);
}
timespan& timespan::operator += (const timespan& d)
{
	m_span += d.m_span;
	return *this;
}
timespan& timespan::operator -= (const timespan& d)
{
	m_span -= d.m_span;
	return *this;

}

timespan timespan::operator + (int64_t microSeconds) const
{
	return timespan(m_span + microSeconds);
}
timespan timespan::operator - (int64_t microSeconds) const
{
	return timespan(m_span - microSeconds);
}
timespan& timespan::operator += (int64_t microSeconds)
{
	m_span += microSeconds;
	return *this;
}
timespan& timespan::operator -= (int64_t microSeconds)
{
	m_span -= microSeconds;
	return *this;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	timestamp  类		2015-8-5
//	时间戳
////////////////////////////////////////////////////////////////////////////////////////////////////////
const int64_t timestamp::TIMEVAL_MIN = -0x7fffffffffffffff-1;
const int64_t timestamp::TIMEVAL_MAX = 0x7fffffffffffffff;
//const int64_t timestamp::TIMEVAL_MIN = std::numeric_limits<int64_t>::min();
//const int64_t timestamp::TIMEVAL_MAX = std::numeric_limits<int64_t>::max();

timestamp::timestamp()
{
	update();
}

timestamp::timestamp(int64_t tv)
	: _ts(tv)
{

}

timestamp::timestamp(const timestamp& other)
	: _ts(other._ts)
{

}
timestamp::timestamp(const std::tm &ttm)
{
	operator = (ttm);
}
timestamp::timestamp(const FILETIME &fileTime)
{
	operator = (fileTime);
}
timestamp::timestamp(const SYSTEMTIME &sysTime)
{
	operator = (sysTime);
}
timestamp::~timestamp()
{

}

timestamp& timestamp::operator = (const timestamp& other)
{
	if(this != &other)
		_ts = other._ts;
	return *this;
}
timestamp& timestamp::operator = (int64_t tv)
{
	_ts = tv;
	return *this;
}

inline std::time_t timestamp::epochTime() const
{
	return std::time_t(_ts/resolution());
}
inline int64_t timestamp::utcTime() const
{
	return _ts*10 + (int64_t(0x01b21dd2) << 32) + 0x13814000;
}

inline int64_t timestamp::epochMicroseconds() const
{
	return _ts;
}

inline int64_t timestamp::elapsed() const
{
	timestamp now;
	return now - *this;
}

inline bool timestamp::isElapsed(int64_t interval) const
{
	timestamp now;
	int64_t diff = now - *this;
	return diff >= interval;
}

inline int64_t timestamp::raw() const
{
	return _ts;
}
void timestamp::update()
{
#if defined(_WIN32) || defined(_WIN64)

	FILETIME ft;
#if defined(_WIN32_WCE)
	GetSystemTimeAsFileTimeWithMillisecondResolution(&ft);
#else
	GetSystemTimeAsFileTime(&ft);
#endif

	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
	epoch.LowPart  = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;

	ULARGE_INTEGER ts;
	ts.LowPart  = ft.dwLowDateTime;
	ts.HighPart = ft.dwHighDateTime;
	ts.QuadPart -= epoch.QuadPart;
	_ts = ts.QuadPart/10;

#elif defined(POCO_VXWORKS)

	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts))
		throw SystemException("cannot get time of day");
	_ts = TimeVal(ts.tv_sec)*resolution() + ts.tv_nsec/1000;

#else

	struct timeval tv;
	if (gettimeofday(&tv, NULL))
		throw SystemException("cannot get time of day");
	_ts = TimeVal(tv.tv_sec)*resolution() + tv.tv_usec;

#endif
}

timestamp& timestamp::operator = (const std::tm &ttm)
{
	SYSTEMTIME sysTime = {0};
	sysTime.wYear		= ttm.tm_year + 1900;
	sysTime.wMonth		= ttm.tm_mon + 1;
	sysTime.wDay		= ttm.tm_mday;
	sysTime.wHour		= ttm.tm_hour;
	sysTime.wMinute		= ttm.tm_min;
	sysTime.wSecond		= ttm.tm_sec;
	sysTime.wDayOfWeek	= ttm.tm_wday;
	*this = sysTime;

	return *this;
}
timestamp& timestamp::operator = (const FILETIME &fileTime)
{
	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
	epoch.LowPart  = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;

	ULARGE_INTEGER ts;
	ts.LowPart  = fileTime.dwLowDateTime;
	ts.HighPart = fileTime.dwHighDateTime;
	ts.QuadPart -= epoch.QuadPart;
	_ts = ts.QuadPart/10;
	return *this;
}
timestamp& timestamp::operator = (const SYSTEMTIME &sysTime)
{
	FILETIME fileTime = {0};
	::SystemTimeToFileTime(&sysTime, &fileTime);
	*this = fileTime;
	return *this;
}
/*static*/ timestamp timestamp::fromEpochTime(std::time_t t)
{
	return timestamp(int64_t(t)*resolution());
}

/*static*/ timestamp timestamp::fromUtcTime(int64_t val)
{
	val -= (int64_t(0x01b21dd2) << 32) + 0x13814000;
	val /= 10;
	return timestamp(val);
}

/*static*/ int64_t timestamp::resolution()
{
	return 1000000;
}

#if defined(_WIN32)
/*static*/ timestamp timestamp::fromFileTimeNP(uint32_t fileTimeLow, uint32_t fileTimeHigh)
{
	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
	epoch.LowPart  = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;

	ULARGE_INTEGER ts;
	ts.LowPart  = fileTimeLow;
	ts.HighPart = fileTimeHigh;
	ts.QuadPart -= epoch.QuadPart;

	return timestamp(ts.QuadPart/10);
}
void timestamp::toFileTimeNP(uint32_t& fileTimeLow, uint32_t& fileTimeHigh) const
{
	ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
	epoch.LowPart  = 0xD53E8000;
	epoch.HighPart = 0x019DB1DE;

	ULARGE_INTEGER ts;
	ts.QuadPart  = _ts*10;
	ts.QuadPart += epoch.QuadPart;
	fileTimeLow  = ts.LowPart;
	fileTimeHigh = ts.HighPart;
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	datetime  类		2015-5-12
//	日期时间类封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SKIP_JUNK() \
while (it != end && !std::isdigit(*it)) ++it


#define SKIP_DIGITS() \
while (it != end && std::isdigit(*it)) ++it


#define PARSE_NUMBER(var) \
while (it != end && std::isdigit(*it)) var = var*10 + ((*it++) - '0')


#define PARSE_NUMBER_N(var, n) \
{ int i = 0; while (i++ < n && it != end && std::isdigit(*it)) var = var*10 + ((*it++) - '0'); }


#define PARSE_FRACTIONAL_N(var, n) \
{ int i = 0; while (i < n && it != end && std::isdigit(*it)) { var = var*10 + ((*it++) - '0'); i++; } while (i++ < n) var *= 10; }
const std::string datetime::MONTH_NAMES[] =
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

datetime::datetime()
{
	timestamp now;
	_utcTime = now.utcTime();
	computeGregorian(julianDay());
	computeDaytime();
}
datetime::datetime(const timestamp& stamp)
	: _utcTime(stamp.utcTime())
{
	computeGregorian(julianDay());
	computeDaytime();
}
datetime::datetime(const datetime &st)
	: _utcTime(st._utcTime)
	, _year(st._year), _month(st._month), _day(st._day)
	, _hour(st._hour), _minute(st._minute), _second(st._second)
	, _millisecond(st._millisecond), _microsecond(st._microsecond)
{
}
datetime::datetime(int year, int month, int day, int hour /*= 0*/, int minute /*= 0*/, int second /*= 0*/
	, int millisecond /*= 0*/, int microsecond /*= 0*/)
	: _year(year), _month(month), _day(day)
	, _hour(hour), _minute(minute), _second(second)
	, _millisecond(millisecond), _microsecond(microsecond)
{
	assert(year >= 0 && year <= 9999);
	assert(month >= 1 && month <= 12);
	assert(day >= 1 && day <= daysOfMonth(year, month));
	assert(hour >= 0 && hour <= 23);
	assert(minute >= 0 && minute <= 59);
	assert(second >= 0 && second <= 59);
	assert(millisecond >= 0 && millisecond <= 999);
	assert(microsecond >= 0 && microsecond <= 999);

	_utcTime = toUtcTime(toJulianDay(year, month, day)) + 10*(hour*timespan::HOURS + minute*timespan::MINUTES + second*timespan::SECONDS + millisecond*timespan::MILLISECONDS + microsecond);

}

datetime::datetime(double julianDay, bool isJulian /*= true*/)
{
	if (isJulian)
	{
		_utcTime = toUtcTime(julianDay);
		computeGregorian(julianDay);
	}
	else
	{
		setMSDate(julianDay);
	}
}
datetime::datetime(int64_t utcTime, int64_t diff)
	: _utcTime(utcTime + diff*10)
{
	computeGregorian(julianDay());
	computeDaytime();
}
datetime::datetime(const std::tm &ttm)
{
	operator = (ttm);
}
datetime::datetime(const FILETIME &fileTime)
{
	operator = (fileTime);
}
datetime::datetime(const SYSTEMTIME &sysTime)
{
	operator = (sysTime);
}

datetime::~datetime()
{

}

inline double datetime::toJulianDay(int64_t utcTime)
{
	double utcDays = double(utcTime)/864000000000.0;
	return utcDays + 2299160.5; // first day of Gregorian reform (Oct 15 1582)
}

double datetime::toJulianDay(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
	// lookup table for (153*month - 457)/5 - note that 3 <= month <= 14.
	static int lookup[] = {-91, -60, -30, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337};

	// day to double
	double dday = double(day) + ((double((hour*60 + minute)*60 + second)*1000 + millisecond)*1000 + microsecond)/86400000000.0;
	if (month < 3)
	{
		month += 12;
		--year;
	}
	double dyear = double(year);
	return dday + lookup[month] + 365*year + std::floor(dyear/4) - std::floor(dyear/100) + std::floor(dyear/400) + 1721118.5;
}
inline int64_t datetime::toUtcTime(double julianDay)
{
	return int64_t((julianDay - 2299160.5)*864000000000.0);
}

void datetime::checkLimit(short& lower, short& higher, short limit)
{
	if (lower >= limit)
	{
		higher += short(lower / limit);
		lower   = short(lower % limit);
	}
}


void datetime::normalize()
{
	checkLimit(_microsecond, _millisecond, 1000);
	checkLimit(_millisecond, _second, 1000);
	checkLimit(_second, _minute, 60);
	checkLimit(_minute, _hour, 60);
	checkLimit(_hour, _day, 24);

	if (_day > daysOfMonth(_year, _month))
	{
		_day -= daysOfMonth(_year, _month);
		if (++_month > 12)
		{
			++_year;
			_month -= 12;
		}
	}
}

void datetime::computeGregorian(double julianDay)
{
	double z    = std::floor(julianDay - 1721118.5);
	double r    = julianDay - 1721118.5 - z;
	double g    = z - 0.25;
	double a    = std::floor(g / 36524.25);
	double b    = a - std::floor(a/4);
	_year       = short(std::floor((b + g)/365.25));
	double c    = b + z - std::floor(365.25*_year);
	_month      = short(std::floor((5*c + 456)/153));
	double dday = c - std::floor((153.0*_month - 457)/5) + r;
	_day        = short(dday);
	if (_month > 12)
	{
		++_year;
		_month -= 12;
	}
	r      *= 24;
	_hour   = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 60;
	_minute = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 60;
	_second = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 1000;
	_millisecond = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 1000;
	_microsecond = short(r + 0.5);

	normalize();

	assert(_month >= 1 && _month <= 12);
	assert(_day >= 1 && _day <= daysOfMonth(_year, _month));
	assert(_hour >= 0 && _hour <= 23);
	assert(_minute >= 0 && _minute <= 59);
	assert(_second >= 0 && _second <= 59);
	assert(_millisecond >= 0 && _millisecond <= 999);
	assert(_microsecond >= 0 && _microsecond <= 999);
}


void datetime::computeDaytime()
{
	timespan span(_utcTime/10);
	int hour = span.hour();
	// Due to double rounding issues, the previous call to computeGregorian()
	// may have crossed into the next or previous day. We need to correct that.
	if (hour == 23 && _hour == 0)
	{
		_day--;
		if (_day == 0)
		{
			_month--;
			if (_month == 0)
			{
				_month = 12;
				_year--;
			}
			_day = daysOfMonth(_year, _month);
		}
	}
	else if (hour == 0 && _hour == 23)
	{
		_day++;
		if (_day > daysOfMonth(_year, _month))
		{
			_month++;
			if (_month > 12)
			{
				_month = 1;
				_year++;
			}
			_day = 1;
		}
	}
	_hour        = hour;
	_minute      = span.minute();
	_second      = span.second();
	_millisecond = span.millisecond();
	_microsecond = span.microsecond();
}

/*static*/ bool datetime::isLeapYear(int year)
{
	return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}
// 某月所保有的天数
/*static*/ int datetime::daysOfMonth(int year, int month)
{
	assert(month >= 1 && month <= 12);
	static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month == 2 && isLeapYear(year))
		return 29;
	else
		return daysOfMonthTable[month];
}
// 是否为有效时间格式
bool datetime::isValid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
	return
		(year >= 0 && year <= 9999) &&
		(month >= 1 && month <= 12) &&
		(day >= 1 && day <= daysOfMonth(year, month)) &&
		(hour >= 0 && hour <= 23) &&
		(minute >= 0 && minute <= 59) &&
		(second >= 0 && second <= 59) &&
		(millisecond >= 0 && millisecond <= 999) &&
		(microsecond >= 0 && microsecond <= 999);
}

int datetime::currentTimeZone()
{
	datetime now;
	std::time_t epochTime = now.timeStamp().epochTime();
	std::tm* broken = std::localtime(&epochTime);
	if (!broken) GTC_EXCEPT(gtc::baseException::BSExceptionCodeParseFailed, L"cannot get local time");
	return (timezone::utcOffset() + ((broken->tm_isdst == 1) ? 3600 : 0));
}

/*static*/ int datetime::parseMonth(std::string::const_iterator& it, const std::string::const_iterator& end)
{
	std::string month;
	while (it != end && (std::isspace(*it) || std::ispunct(*it))) ++it;
	bool isFirst = true;
	while (it != end && std::isalpha(*it)) 
	{
		char ch = (*it++);
		if (isFirst) { month += std::toupper(ch); isFirst = false; }
		else month += std::tolower(ch);
	}
	if (month.length() < 3) return 0;// throw SyntaxException("Month name must be at least three characters long", month);
	for (int i = 0; i < 12; ++i) 
	{
		if (MONTH_NAMES[i].find(month) == 0)
			return i + 1;
	}
	//throw SyntaxException("Not a valid month name", month);
	return 0;
}
/*static*/ int datetime::parseTZD(std::string::const_iterator& it, const std::string::const_iterator& end)
{
	struct Zone
	{
		const char* designator;
		int         timeZoneDifferential;
	};

	static Zone zones[] =
	{
		{"Z",           0},
		{"UT",          0},
		{"GMT",         0},
		{"BST",    1*3600},
		{"IST",    1*3600},
		{"WET",         0},
		{"WEST",   1*3600},
		{"CET",    1*3600},
		{"CEST",   2*3600},
		{"EET",    2*3600},
		{"EEST",   3*3600},
		{"MSK",    3*3600},
		{"MSD",    4*3600},
		{"NST",   -3*3600-1800},
		{"NDT",   -2*3600-1800},
		{"AST",   -4*3600},
		{"ADT",   -3*3600},
		{"EST",   -5*3600},
		{"EDT",   -4*3600},
		{"CST",   -6*3600},
		{"CDT",   -5*3600},
		{"MST",   -7*3600},
		{"MDT",   -6*3600},
		{"PST",   -8*3600},
		{"PDT",   -7*3600},
		{"AKST",  -9*3600},
		{"AKDT",  -8*3600},
		{"HST",  -10*3600},
		{"AEST",  10*3600},
		{"AEDT",  11*3600},
		{"ACST",   9*3600+1800},
		{"ACDT",  10*3600+1800},
		{"AWST",   8*3600},
		{"AWDT",   9*3600}
	};

	int tzd = 0;
	while (it != end && std::isspace(*it)) ++it;
	if (it != end)
	{
		if (std::isalpha(*it))
		{
			std::string designator;
			designator += *it++;
			if (it != end && std::isalpha(*it)) designator += *it++;
			if (it != end && std::isalpha(*it)) designator += *it++;
			if (it != end && std::isalpha(*it)) designator += *it++;
			for (unsigned i = 0; i < sizeof(zones)/sizeof(Zone); ++i)
			{
				if (designator == zones[i].designator)
				{
					tzd = zones[i].timeZoneDifferential;
					break;
				}
			}
		}
		if (it != end && (*it == '+' || *it == '-'))
		{
			int sign = *it == '+' ? 1 : -1;
			++it;
			int hours = 0;
			PARSE_NUMBER_N(hours, 2);
			if (it != end && *it == ':') ++it;
			int minutes = 0;
			PARSE_NUMBER_N(minutes, 2);
			tzd += sign*(hours*3600 + minutes*60);
		}
	}
	return tzd;
}
/*static*/ int datetime::parseAMPM(std::string::const_iterator& it, const std::string::const_iterator& end, int hour)
{
	std::string ampm;
	while (it != end && (std::isspace(*it) || std::ispunct(*it))) ++it;
	while (it != end && std::isalpha(*it)) 
	{
		char ch = (*it++);
		ampm += std::toupper(ch);
	}
	if (ampm == "AM")
	{
		if (hour == 12)
			return 0;
		else
			return hour;
	}
	else if (ampm == "PM")
	{
		if (hour < 12)
			return hour + 12;
		else
			return hour;
	}
	else return -1;// throw SyntaxException("Not a valid AM/PM designator", ampm);
}
bool datetime::parse(const std::string& fmt, const std::string& strDatetime, datetime& dateTime, int& timeZoneDifferential)
{
	if (fmt.empty() || strDatetime.empty()) return false;

	int year   = 0;
	int month  = 0;
	int day    = 0;
	int hour   = 0;
	int minute = 0;
	int second = 0;
	int millis = 0;
	int micros = 0;
	int tzd    = 0;

	std::string::const_iterator it   = strDatetime.begin();
	std::string::const_iterator end  = strDatetime.end();
	std::string::const_iterator itf  = fmt.begin();
	std::string::const_iterator endf = fmt.end();

	while (itf != endf && it != end)
	{
		if (*itf == '%')
		{
			if (++itf != endf)
			{
				switch (*itf)
				{
				case 'w':
				case 'W':
					while (it != end && std::isspace(*it)) ++it;
					while (it != end && std::isalpha(*it)) ++it;
					break;
				case 'b':
				case 'B':
					month = parseMonth(it, end);
					break;
				case 'd':
				case 'e':
				case 'f':
					SKIP_JUNK();
					PARSE_NUMBER_N(day, 2);
					break;
				case 'm':
				case 'n':
				case 'o':
					SKIP_JUNK();
					PARSE_NUMBER_N(month, 2);
					break;					 
				case 'y':
					SKIP_JUNK();
					PARSE_NUMBER_N(year, 2);
					if (year >= 69) 
						year += 1900;
					else
						year += 2000;
					break;
				case 'Y':
					SKIP_JUNK();
					PARSE_NUMBER_N(year, 4);
					break;
				case 'r':
					SKIP_JUNK();
					PARSE_NUMBER(year);
					if (year < 1000)
					{
						if (year >= 69) 
							year += 1900;
						else
							year += 2000;
					}
					break;
				case 'H':
				case 'h':
					SKIP_JUNK();
					PARSE_NUMBER_N(hour, 2);
					break;
				case 'a':
				case 'A':
					hour = parseAMPM(it, end, hour);
					break;
				case 'M':
					SKIP_JUNK();
					PARSE_NUMBER_N(minute, 2);
					break;
				case 'S':
					SKIP_JUNK();
					PARSE_NUMBER_N(second, 2);
					break;
				case 's':
					SKIP_JUNK();
					PARSE_NUMBER_N(second, 2);
					if (it != end && (*it == '.' || *it == ','))
					{
						++it;
						PARSE_FRACTIONAL_N(millis, 3);
						PARSE_FRACTIONAL_N(micros, 3);
						SKIP_DIGITS();
					}
					break;
				case 'i':
					SKIP_JUNK();
					PARSE_NUMBER_N(millis, 3);
					break;
				case 'c':
					SKIP_JUNK();
					PARSE_NUMBER_N(millis, 1);
					millis *= 100;
					break;
				case 'F':
					SKIP_JUNK();
					PARSE_FRACTIONAL_N(millis, 3);
					PARSE_FRACTIONAL_N(micros, 3);
					SKIP_DIGITS();
					break;
				case 'z':
				case 'Z':
					tzd = parseTZD(it, end);
					break;
				}
				++itf;
			}
		}
		else ++itf;
	}
	if (month == 0) month = 1;
	if (day == 0) day = 1;
	if (datetime::isValid(year, month, day, hour, minute, second, millis, micros))
		dateTime.assign(year, month, day, hour, minute, second, millis, micros);
	else 
		return false;//throw SyntaxException("date/time component out of range");
	timeZoneDifferential = tzd;
	return true;
}

/*static*/ bool datetime::format(const datetime &dateTime, const std::string& fmt, std::string& strDatetime)
{
	strDatetime.clear();
	std::time_t epochTime = dateTime.timeStamp().epochTime();
	std::tm* broken = std::localtime(&epochTime);
	if(!broken) return false;
	char pbuf[120] = {0};
	std::strftime(pbuf, _countof(pbuf), fmt.c_str(), broken);
	strDatetime = pbuf;
	return true;
}
std::wstring datetime::format(PCWSTR pformat /*= L"%F %T"*/ /*YYYY-MM-DD HH:MM:SS*/)
{
	assert(pformat);
	std::string fmt = stringUtil::toChar(pformat);
	std::string strdt;
	format(*this, fmt, strdt);
	return stringUtil::toWChar(strdt.c_str());
}
bool datetime::parse(std::wstring wsdt, PCWSTR pformat /*= L"%Y-%m-%d %H:%M:%S" YYYY-MM-DD HH:MM:SS*/)
{
	std::string strfmt = stringUtil::toChar(pformat);
	std::string strdt = stringUtil::toChar(wsdt.c_str());
	int timeZone = 0;
	return parse(strfmt, strdt, *this, timeZone);
}
//std::time_t datetime::toTime()
//{
//	return std::mktime(&m_tm);
//}
//void datetime::asSystemTime(SYSTEMTIME &st) const
//{
//	//::(&st, sizeof(st));
//	st.wYear = m_tm.tm_year + 1900;
//	st.wMonth = m_tm.tm_mon + 1;
//	st.wDay = m_tm.tm_mday;
//	st.wHour = m_tm.tm_hour;
//	st.wMinute = m_tm.tm_min;
//	st.wSecond = m_tm.tm_sec;
//	st.wDayOfWeek = m_tm.tm_wday;
//	st.wMilliseconds = 0;
//}
//bool datetime::changeToUTC()
//{
//	std::time_t tt = toTime();
//	std::tm *ptm = std::gmtime(&tt);
//	if(ptm)
//	{
//		m_tm = *ptm;
//		return true;
//	}
//	return false;
//}
//bool datetime::changeToLocal()
//{
//	TIME_ZONE_INFORMATION tzi = {0};
//	if(GetTimeZoneInformation(&tzi) != TIME_ZONE_ID_INVALID)
//	{
//		SYSTEMTIME st = {0}, stLocal = {0};
//		asSystemTime(st);
//		if (SystemTimeToTzSpecificLocalTime(&tzi, &st, &stLocal))
//		{
//			*this = stLocal;
//			return true;
//		}
//	}
//	//std::time_t tt = toTime();
//	//std::tm *ptm = std::localtime(&tt);
//	//if(ptm)
//	//{
//	//	m_tm = *ptm;
//	//	return true;
//	//}
//	return false;
//}
datetime::operator DATE() const
{
	return getMSDate();
}
datetime& datetime::operator = (const timestamp& stamp)
{
	_utcTime = stamp.utcTime();
	computeGregorian(julianDay());
	computeDaytime();
	return *this;
}
/// Assigns a Timestamp.
datetime& datetime::operator = (double julianDay)
{
	_utcTime = toUtcTime(julianDay);
	computeGregorian(julianDay);
	return *this;
}
datetime& datetime::operator = (const datetime &dt)
{
	if (&dt != this)
	{
		_utcTime     = dt._utcTime;
		_year        = dt._year;
		_month       = dt._month;
		_day         = dt._day;
		_hour        = dt._hour;
		_minute      = dt._minute;
		_second      = dt._second;
		_millisecond = dt._millisecond;
		_microsecond = dt._microsecond;
	}
	return *this;
}
datetime& datetime::operator = (const std::tm &ttm)
{
	timestamp ts = ttm;
	*this = ts;

	return *this;
}
datetime& datetime::operator = (const FILETIME &fileTime)
{
	timestamp ts = fileTime;
	*this = ts;
	return *this;
}
datetime& datetime::operator = (const SYSTEMTIME &sysTime)
{
	timestamp ts = sysTime;
	*this = ts;

	return *this;
}

datetime  datetime::operator +  (const timespan& span) const
{
	return datetime(_utcTime, span.microseconds());
}
datetime  datetime::operator -  (const timespan& span) const
{
	return datetime(_utcTime, -span.microseconds());
}
timespan  datetime::operator -  (const datetime& dateTime) const
{
	return timespan((_utcTime - dateTime._utcTime)/10);
}
datetime& datetime::operator += (const timespan& span)
{
	_utcTime += span.microseconds()*10;
	computeGregorian(julianDay());
	computeDaytime();
	return *this;
}
datetime& datetime::operator -= (const timespan& span)
{
	_utcTime -= span.microseconds()*10;
	computeGregorian(julianDay());
	computeDaytime();
	return *this;
}

void datetime::setMSDate(DATE date)
{
	SYSTEMTIME st = {0};
	::VariantTimeToSystemTime(date, &st);
	operator = (st);
}

DATE datetime::getMSDate() const
{
	DATE dt = 0;
	SYSTEMTIME systime = {0};
	systime.wYear = _year;
	systime.wMonth = _month;
	systime.wDay = _day;
	systime.wHour = _hour;
	systime.wMinute = _minute;
	systime.wSecond = _second;
	systime.wDayOfWeek = dayOfWeek();
	systime.wMilliseconds = _millisecond;
	::SystemTimeToVariantTime(&systime, &dt);
	return dt;
}
double datetime::julianDay() const
{
	return toJulianDay(_utcTime);
}

timestamp datetime::timeStamp() const
{
	return timestamp::fromUtcTime(_utcTime);
}

int datetime::week(int firstDayOfWeek /*= Monday*/) const
{
	assert(firstDayOfWeek >= 0 && firstDayOfWeek <= 6);

	/// find the first firstDayOfWeek.
	int baseDay = 1;
	while (datetime(_year, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

	int doy  = dayOfYear();
	int offs = baseDay <= 4 ? 0 : 1; 
	if (doy < baseDay)
		return offs;
	else
		return (doy - baseDay)/7 + 1 + offs;
}
int datetime::dayOfWeek() const
{
	return int((std::floor(julianDay() + 1.5))) % 7;
}
int datetime::dayOfYear() const
{
	int doy = 0;
	for (int month = 1; month < _month; ++month)
		doy += daysOfMonth(_year, month);
	doy += _day;
	return doy;
}

datetime& datetime::assign(int year, int month, int day, int hour /*= 0*/, int minute /*= 0*/, int second /*= 0*/
	, int millisecond /*= 0*/, int microsecond /*= 0*/)
{
	assert(year >= 0 && year <= 9999);
	assert(month >= 1 && month <= 12);
	assert(day >= 1 && day <= daysOfMonth(year, month));
	assert(hour >= 0 && hour <= 23);
	assert(minute >= 0 && minute <= 59);
	assert(second >= 0 && second <= 59);
	assert(millisecond >= 0 && millisecond <= 999);
	assert(microsecond >= 0 && microsecond <= 999);

	_utcTime     = toUtcTime(toJulianDay(year, month, day)) + 10*(hour*gtc::timespan::HOURS + minute*gtc::timespan::MINUTES + second*gtc::timespan::SECONDS + millisecond*gtc::timespan::MILLISECONDS + microsecond);
	_year        = year;
	_month       = month;
	_day         = day;
	_hour        = hour;
	_minute      = minute;
	_second      = second;
	_millisecond = millisecond;
	_microsecond = microsecond;

	return *this;
}

void datetime::makeUTC(int tzd)
{
	operator -= (timespan(((int64_t) tzd)*timespan::SECONDS));
}
void datetime::makeUTC()
{
	makeUTC(currentTimeZone());
}
void datetime::makeLocal(int tzd)
{
	operator += (timespan(((int64_t) tzd)*timespan::SECONDS));
}
void datetime::makeLocal()
{
	//std::time_t epochTime = timeStamp().epochTime();
	//std::tm* broken = std::localtime(&epochTime);
	//if (!broken) GTC_EXCEPT(gtc::baseException::BSExceptionCodeParseFailed, L"cannot get local time");
	//int tzd = (timezone::utcOffset() + ((broken->tm_isdst == 1) ? 3600 : 0));
	makeLocal(currentTimeZone());
	//*this += timespan(((int64_t) _tzd)*timespan::SECONDS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	lunarCalendar  类		2015-8-5
//	阴历日历
////////////////////////////////////////////////////////////////////////////////////////////////////////
const WORD lunarCalendar::START_YEAR = 1901; 
const WORD lunarCalendar::END_YEAR   = 2050;
//数组gLunarDay存入阴历1901年到2100年每年中的月天数信息， 
//阴历每月只能是29或30天，一年用12（或13）个二进制位表示，对应位为1表30天，否则为29天 
static WORD gLunarMonthDay[] = 
{ 
	//测试数据只有1901.1.1 --2050.12.31 
	0X4ae0, 0Xa570, 0X5268, 0Xd260, 0Xd950, 0X6aa8, 0X56a0, 0X9ad0, 0X4ae8, 0X4ae0,   //1910 
	0Xa4d8, 0Xa4d0, 0Xd250, 0Xd548, 0Xb550, 0X56a0, 0X96d0, 0X95b0, 0X49b8, 0X49b0,   //1920 
	0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada8, 0X2b60, 0X9570, 0X4978, 0X4970, 0X64b0,   //1930 
	0Xd4a0, 0Xea50, 0X6d48, 0X5ad0, 0X2b60, 0X9370, 0X92e0, 0Xc968, 0Xc950, 0Xd4a0,   //1940 
	0Xda50, 0Xb550, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950, 0Xb4a8, 0X6ca0,   //1950 
	0Xb550, 0X55a8, 0X4da0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa950, 0Xe950, 0X6aa0, 0Xad50,   //1960 
	0Xab50, 0X4b60, 0Xa570, 0Xa570, 0X5260, 0Xe930, 0Xd950, 0X5aa8, 0X56a0, 0X96d0,   //1970 
	0X4ae8, 0X4ad0, 0Xa4d0, 0Xd268, 0Xd250, 0Xd528, 0Xb540, 0Xb6a0, 0X96d0, 0X95b0,   //1980 
	0X49b0, 0Xa4b8, 0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada0, 0Xab60, 0X9370, 0X4978,   //1990 
	0X4970, 0X64b0, 0X6a50, 0Xea50, 0X6b28, 0X5ac0, 0Xab60, 0X9368, 0X92e0, 0Xc960,   //2000 
	0Xd4a8, 0Xd4a0, 0Xda50, 0X5aa8, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950,   //2010 
	0Xb4a0, 0Xb550, 0Xb550, 0X55a8, 0X4ba0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa930, 0X74a8,   //2020 
	0X6aa0, 0Xad50, 0X4da8, 0X4b60, 0X9570, 0Xa4e0, 0Xd260, 0Xe930, 0Xd530, 0X5aa0,   //2030 
	0X6b50, 0X96d0, 0X4ae8, 0X4ad0, 0Xa4d0, 0Xd258, 0Xd250, 0Xd520, 0Xdaa0, 0Xb5a0,   //2040 
	0X56d0, 0X4ad8, 0X49b0, 0Xa4b8, 0Xa4b0, 0Xaa50, 0Xb528, 0X6d20, 0Xada0, 0X55b0,   //2050 

}; 

//数组gLanarMonth存放阴历1901年到2050年闰月的月份，如没有则为0，每字节存两年 
static BYTE  gLunarMonth[] = 
{ 
	0X00, 0X50, 0X04, 0X00, 0X20,   //1910 
	0X60, 0X05, 0X00, 0X20, 0X70,   //1920 
	0X05, 0X00, 0X40, 0X02, 0X06,   //1930 
	0X00, 0X50, 0X03, 0X07, 0X00,   //1940 
	0X60, 0X04, 0X00, 0X20, 0X70,   //1950 
	0X05, 0X00, 0X30, 0X80, 0X06,   //1960 
	0X00, 0X40, 0X03, 0X07, 0X00,   //1970 
	0X50, 0X04, 0X08, 0X00, 0X60,   //1980 
	0X04, 0X0a, 0X00, 0X60, 0X05,   //1990 
	0X00, 0X30, 0X80, 0X05, 0X00,   //2000 
	0X40, 0X02, 0X07, 0X00, 0X50,   //2010 
	0X04, 0X09, 0X00, 0X60, 0X04,   //2020 
	0X00, 0X20, 0X60, 0X05, 0X00,   //2030 
	0X30, 0Xb0, 0X06, 0X00, 0X50,   //2040 
	0X02, 0X07, 0X00, 0X50, 0X03    //2050 
}; 

WORD lunarCalendar::getLeapMonth(WORD iLunarYear) 
{
	BYTE &flag = gLunarMonth[(iLunarYear - START_YEAR)/2]; 
    return  (iLunarYear - START_YEAR)%2 ? flag&0x0f : flag>>4; 
} 

LONG lunarCalendar::lunarMonthDays(WORD iLunarYear, WORD iLunarMonth) 
{
	if(iLunarYear < START_YEAR)
		return 30L; 
	WORD height =0 ,low =29;
	int iBit = 16 - iLunarMonth; 

    if(iLunarMonth > getLeapMonth(iLunarYear) && getLeapMonth(iLunarYear))
		iBit--;
	if(gLunarMonthDay[iLunarYear - START_YEAR] & (1<<iBit))
		low++; 

	if(iLunarMonth == getLeapMonth(iLunarYear))
		if(gLunarMonthDay[iLunarYear - START_YEAR] & (1<< (iBit -1)))
			height = 30;
		else
			height = 29; 

	return MAKELONG(low, height); 
} 

WORD lunarCalendar::lunarYearDays(WORD iLunarYear) 
{
	WORD days =0;
	for(WORD  i=1; i<=12; i++)
	{
		LONG  tmp = lunarMonthDays(iLunarYear ,i);
		days += HIWORD(tmp);
		days += LOWORD(tmp);
	} 
    return days; 
} 


//// add by handong: 显示 生肖  及 干支 纪年 ：   pShX(兔(又名得到之兔))  pGZhi 甲子 
//void FormatShengXiao(WORD  iYear, char *pShX, char *pGZhi) 
//{        
//        char szText1[]="甲乙丙丁戊己庚辛壬癸"; 
//        char szText2[]="子丑寅卯辰巳午未申酉戌亥"; 
//        char szText3[]="鼠牛虎免龙蛇马羊猴鸡狗猪"; 
//        char szText4[]="屋上之鼠海内之牛山林之虎"; 
//        memcpy(pGZhi,  szText1+((iYear-4)%10)*2,2); 
//        memcpy(pGZhi+2,szText2+((iYear-4)%12)*2,2); 
//        pGZhi[4]=0; 
//         
//        memcpy(pShX,szText3+((iYear-4)%12)*2,2); 
//        strcpy(pShX+2,"(又名:)"); 
//         
//}         

void lunarCalendar::calcLunarDate(WORD &iYear, WORD &iMonth ,WORD &iDay, LONG iSpanDays) 
{
	//阳历1901年2月19日为阴历1901年正月初一 
	//阳历1901年1月1日到2月19日共有49天
	if(iSpanDays <49)
	{
		iYear  = START_YEAR-1;
		if(iSpanDays <19)
		{
			iMonth = 11;
			iDay = 11 + WORD(iSpanDays);
		}
		else
		{
			iMonth = 12;
			iDay = WORD(iSpanDays) - 18;
		}
		return ;
	}
	//下面从阴历1901年正月初一算起
	iSpanDays -= 49; 
    iYear = START_YEAR;
	iMonth = 1;
	iDay = 1;
	//计算年
	LONG tmp = lunarYearDays(iYear);
	while(iSpanDays >= tmp)
	{
		iSpanDays -= tmp;
		tmp = lunarYearDays(++iYear);
	} 
    //计算月
	tmp = LOWORD(lunarMonthDays(iYear, iMonth));
	while(iSpanDays >= tmp)
	{
		iSpanDays -= tmp;
		if(iMonth == getLeapMonth(iYear))
		{
			tmp  = HIWORD(lunarMonthDays(iYear, iMonth));
			if(iSpanDays < tmp)
				break;
			iSpanDays -= tmp;
		}
		tmp = LOWORD(lunarMonthDays(iYear, ++iMonth));
	}
	//计算日
	iDay += WORD(iSpanDays); 
} 
LONG lunarCalendar::calcDateDiff(WORD iEndYear, WORD iEndMonth, WORD iEndDay, 
                                    WORD  iStartYear, WORD iStartMonth, WORD iStartDay) 
{ 
        WORD monthday[]={0, 31, 59 ,90, 120, 151, 181, 212, 243, 273, 304, 334};  

        //计算两个年份1月1日之间相差的天数 
        LONG iDiffDays =(iEndYear - iStartYear)*365; 
        iDiffDays += (iEndYear-1)/4 - (iStartYear-1)/4; 
        iDiffDays -= ((iEndYear-1)/100 - (iStartYear-1)/100); 
        iDiffDays += (iEndYear-1)/400 - (iStartYear-1)/400; 

    //加上iEndYear年1月1日到iEndMonth月iEndDay日之间的天数 
    iDiffDays += monthday[iEndMonth-1] + 
                                           (datetime::isLeapYear(iEndYear)&&iEndMonth>2? 1: 0); 
    iDiffDays += iEndDay; 

        //减去iStartYear年1月1日到iStartMonth月iStartDay日之间的天数 
        iDiffDays -= (monthday[iStartMonth-1] +  
                                  (datetime::isLeapYear(iStartYear)&&iStartMonth>2 ? 1: 0)); 
    iDiffDays -= iStartDay;      
        return iDiffDays; 
} 
datetime lunarCalendar::toLunar(const gtc::datetime &dtSolar)
{
	static datetime c_dtStart(1901, 1, 1);
	datetime dt(dtSolar.year(), dtSolar.month(), dtSolar.day());
	timespan ts = dt - c_dtStart;
	WORD year, month, day;
	calcLunarDate(year, month, day, ts.days());
	dt.assign(year, month, day);

	return dt;
}
datetime lunarCalendar::toSolar(const gtc::datetime &dtLunar)
{
	timespan oneDay(1, 0, 0, 0);
	datetime dtTmp;
	datetime dtSolar = dtLunar;
	for (int idx = 0; idx < 100; ++idx)
	{
		dtSolar += oneDay;
		dtTmp = toLunar(dtSolar);
		if(dtTmp == dtLunar) return dtSolar;
	}
	GTC_EXCEPT(baseException::BSExceptionCodeParseFailed, L"阴历到阳历转换失败");
	return dtLunar;
}

////获取农历LunarY年LunarM月LunarD日公历日期（Y年M月D日）
//void CovertLunarToSolar(WORD LunarY,WORD LunarM,WORD LunarD,WORD &Y,WORD &M,WORD &D) 
//{ 
//        CTime TmpDate(LunarY,LunarM,LunarD,12,12,12); 
//        CTime Up,Down; 
//        CTimeSpan       InterveDate(60,0,0,0); //前后2个月天搜索 
//        CTimeSpan       OneDay(1,0,0,0); 
//        WORD Year, Mon, Day; 
//        WORD LunarYear,LunarMon,LunarDay; 
//         
//        Up = TmpDate + InterveDate; 
//        Down = TmpDate - InterveDate; 
//         
//        CTimeSpan Days=Up-Down; 
//         
//        int Len = Days.GetDays(); 
//        TmpDate=Down; 
//        for(int i=0; i<Len; i++) 
//        { 
//                TmpDate = TmpDate + OneDay; 
//                Year = TmpDate.GetYear(); Mon = TmpDate.GetMonth();  Day = TmpDate.GetDay(); 
//                GetLunarDate(Year, Mon, Day, LunarYear, LunarMon, LunarDay); 
//                if( (LunarYear==LunarY) && (LunarMon==LunarM) && (LunarDay==LunarD) )//found 
//                { 
//                        Y=Year; M=Mon; D=Day; 
//                        return;                                          
//                } 
//        } 
//  
//} 
////格式化农历中的year
//void FormatLunarYear(WORD  iYear, char *pBuffer) 
//{        
//        char szText1[]="甲乙丙丁戊己庚辛壬癸"; 
//        char szText2[]="子丑寅卯辰巳午未申酉戌亥"; 
//        char szText3[]="鼠牛虎免龙蛇马羊猴鸡狗猪"; 
//
//        memcpy(pBuffer,  szText1+((iYear-4)%10)*2,2); 
//        memcpy(pBuffer+2,szText2+((iYear-4)%12)*2,2); 
//        pBuffer[4]=' '; 
//        memcpy(pBuffer+5,szText3+((iYear-4)%12)*2,2); 
//        strcpy(pBuffer+7,"年"); 
//}

/*static*/ std::wstring lunarCalendar::formatMonth(int month, bool bUnit /*= true*/)
{
	static const wchar_t c_pmonths[] = L"正二三四五六七八九十冬腊";
	assert(month >= 1 && month <= 12);
	if(month < 1 || month > 12) return L"";
	std::wstring wsmonth;
	if(month == 11) wsmonth = L"十一";
	else if(month == 12) wsmonth = L"十二";
	else wsmonth = c_pmonths[month-1];
	if(bUnit)
		wsmonth += L"月";
	return wsmonth;
}
std::wstring lunarCalendar::formatDay(int day, bool bUnit /*= true*/)
{
	static const wchar_t c_pPrefix[] = L"初十廿三";
	static const wchar_t c_pShi[] = L"一二三四五六七八九";
	assert(day >= 1 && day <= 30);
	if(day < 1 || day > 30) return L"";
	std::wstring wsday;
	if(day == 10) wsday = L"初十";
	else if(day == 20) wsday = L"二十";
	else if(day == 30) wsday = L"三十";
	else
	{
		int pre = day / 10;
		int sub = day % 10;

		wsday = c_pPrefix[pre];
		wsday += c_pShi[sub-1];

	}
	if(bUnit)
		wsday += L'日';
	return wsday;
}

std::wstring lunarCalendar::formatWeek(int week, bool bUnit)
{
	static const wchar_t c_pPrefix[] = L"日一二三四五六";

	assert(week >= 0 && week <= 6);
	if(week < 0 || week > 6) return L"";
	std::wstring wsweek(bUnit ? L"星期" : L"");
	wsweek += c_pPrefix[week];
	return wsweek;
}
////格式化农历中的day
//void FormatLunarDay(WORD  iDay, char *pBuffer) 
//{ 
//    char szText1[]="初十廿三"; 
//        char szText2[]="一二三四五六七八九十"; 
//        if(iDay != 20 && iDay !=30) 
//        { 
//                memcpy(pBuffer, szText1 + (iDay-1)/10*2 ,2); 
//                memcpy(pBuffer+2, szText2 + ((iDay-1)%10)*2 ,2); 
//                pBuffer[4]='\0'; 
//        } 
//        else 
//        { 
//        memcpy(pBuffer, szText1 + iDay/10*2, 2); 
//                strcpy(pBuffer+2, szText2 +18); 
//        } 
//} 

}	// gtc