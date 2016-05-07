#ifndef __DATETIME_H__
#define __DATETIME_H__
#pragma once
#include "basedef.h"
namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	timezone  类		2015-8-5
//	时区封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class timestamp;
class timezone
{
public:
	static int utcOffset();
	/// Returns the offset of local time to UTC, in seconds.
	///     local time = UTC + utcOffset() + dst().

	static int dst();
	/// Returns the daylight saving time offset in seconds if
	/// daylight saving time is in use.
	///     local time = UTC + utcOffset() + dst().

	static bool isDst(const timestamp& timeStamp);
	/// Returns true if daylight saving time is in effect
	/// for the given time. Depending on the operating system
	/// platform this might only work reliably for certain
	/// date ranges, as the C library's localtime() function
	/// is used.

	static int tzd();
	/// Returns the time zone differential for the current timezone.
	/// The timezone differential is computed as utcOffset() + dst()
	/// and is expressed in seconds.

	static std::string name();
	/// Returns the timezone name currently in effect.

	static std::string standardName();
	/// Returns the timezone name if not daylight saving time is in effect.

	static std::string dstName();
	/// Returns the timezone name if daylight saving time is in effect.
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	timespan  类		2015-8-4
//	时间差封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class timespan
{
public:
	timespan();
	timespan(int64_t microseconds);
	timespan(long seconds, long microseconds);
	timespan(int days, int hours, int minutes, int seconds = 0, int microSeconds = 0);
	timespan(const timespan& timespan);
	~timespan();

	timespan& operator = (const timespan& timespan);
	timespan& operator = (int64_t microseconds);
	timespan& assign(int days, int hours, int minutes, int seconds = 0, int microSeconds = 0);
	timespan& assign(long seconds, long microseconds);

	bool operator == (const timespan& ts) const {			return m_span == ts.m_span;					}
	bool operator != (const timespan& ts) const {			return m_span != ts.m_span;					}
	bool operator >  (const timespan& ts) const {			return m_span > ts.m_span;					}
	bool operator >= (const timespan& ts) const {			return m_span >= ts.m_span;					}
	bool operator <  (const timespan& ts) const {			return m_span < ts.m_span;					}
	bool operator <= (const timespan& ts) const {			return m_span <= ts.m_span;					}

	bool operator == (int64_t microSeconds) const {			return m_span == microSeconds;				}
	bool operator != (int64_t microSeconds) const {			return m_span != microSeconds;				}
	bool operator >  (int64_t microSeconds) const {			return m_span > microSeconds;				}
	bool operator >= (int64_t microSeconds) const {			return m_span >= microSeconds;				}
	bool operator <  (int64_t microSeconds) const {			return m_span < microSeconds;				}
	bool operator <= (int64_t microSeconds) const {			return m_span <= microSeconds;				}

	timespan operator + (const timespan& d) const;
	timespan operator - (const timespan& d) const;
	timespan& operator += (const timespan& d);
	timespan& operator -= (const timespan& d);

	timespan operator + (int64_t microSeconds) const;
	timespan operator - (int64_t microSeconds) const;
	timespan& operator += (int64_t microSeconds);
	timespan& operator -= (int64_t microSeconds);

	int days() const;

	int hour() const;
	int hours() const;
	/// Returns the total number of hours.

	int minute() const;
	/// Returns the number of minutes (0 to 59).

	int minutes() const;
	/// Returns the total number of minutes.

	int second() const;
	/// Returns the number of seconds (0 to 59).

	int seconds() const;
	/// Returns the total number of seconds.

	int millisecond() const;
	/// Returns the number of milliseconds (0 to 999).

	int64_t milliseconds() const;
	/// Returns the total number of milliseconds.

	int microsecond() const;
	/// Returns the fractions of a millisecond
	/// in microseconds (0 to 999).

	int useconds() const;
	/// Returns the fractions of a second
	/// in microseconds (0 to 999999).

	int64_t microseconds() const;
public:
	static const int64_t MILLISECONDS; /// The number of microseconds in a millisecond.
	static const int64_t SECONDS;      /// The number of microseconds in a second.
	static const int64_t MINUTES;      /// The number of microseconds in a minute.
	static const int64_t HOURS;        /// The number of microseconds in a hour.
	static const int64_t DAYS;         /// The number of microseconds in a day.

protected:
	int64_t		m_span;		// microseconds 微妙，百万分之一秒
};
inline int timespan::days() const
{
	return int(m_span/DAYS);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	timestamp  类		2015-8-5
//	时间戳
////////////////////////////////////////////////////////////////////////////////////////////////////////
class timestamp
{
public:
	static const int64_t TIMEVAL_MIN; /// Minimum timestamp value.
	static const int64_t TIMEVAL_MAX; /// Maximum timestamp value.

public:
	timestamp();
	/// Creates a timestamp with the current time.

	timestamp(int64_t tv);
	/// Creates a timestamp from the given time value
	/// (microseconds since midnight, January 1, 1970).

	timestamp(const timestamp& other);
	/// Copy constructor.
	timestamp(const std::tm &ttm);
	timestamp(const FILETIME &fileTime);
	timestamp(const SYSTEMTIME &sysTime);
	~timestamp();
	/// Destroys the timestamp

	timestamp& operator = (const timestamp& other);
	timestamp& operator = (int64_t tv);

	void update();
	/// Updates the timestamp with the current time.
	timestamp& operator = (const std::tm &ttm);
	timestamp& operator = (const FILETIME &fileTime);
	timestamp& operator = (const SYSTEMTIME &sysTime);

	bool operator == (const timestamp& ts) const {			return _ts == ts._ts;						}
	bool operator != (const timestamp& ts) const {			return _ts != ts._ts;						}
	bool operator >  (const timestamp& ts) const {			return _ts > ts._ts;						}
	bool operator >= (const timestamp& ts) const {			return _ts >= ts._ts;						}
	bool operator <  (const timestamp& ts) const {			return _ts < ts._ts;						}
	bool operator <= (const timestamp& ts) const {			return _ts <= ts._ts;						}

	timestamp operator +  (int64_t d) const {				return timestamp(_ts + d);					}
	timestamp operator +  (const timespan& span) const {	return *this + span.microseconds();			}
	timestamp operator -  (int64_t d) const {				return timestamp(_ts - d);					}
	timestamp operator -  (const timespan& span) const {		return *this - span.microseconds();			}
	int64_t   operator -  (const timestamp& ts) const {		return _ts - ts._ts;							}
	timestamp& operator += (int64_t d) {					_ts += d;	return *this;					}
	timestamp& operator += (const timespan& span) {			return *this += span.microseconds();		}
	timestamp& operator -= (int64_t d) {						_ts -= d;	return *this;					}
	timestamp& operator -= (const timespan& span) {			return *this -= span.microseconds();			}

	std::time_t epochTime() const;
	/// Returns the timestamp expressed in time_t.
	/// time_t base time is midnight, January 1, 1970.
	/// Resolution is one second.

	int64_t utcTime() const;
	/// Returns the timestamp expressed in UTC-based
	/// time. UTC base time is midnight, October 15, 1582.
	/// Resolution is 100 nanoseconds.

	int64_t epochMicroseconds() const;
	/// Returns the timestamp expressed in microseconds
	/// since the Unix epoch, midnight, January 1, 1970.

	int64_t elapsed() const;
	/// Returns the time elapsed since the time denoted by
	/// the timestamp. Equivalent to Timestamp() - *this.

	bool isElapsed(int64_t interval) const;
	/// Returns true iff the given interval has passed
	/// since the time denoted by the timestamp.

	int64_t raw() const;
	/// Returns the raw time value.
	///
	/// Same as epochMicroseconds().
public:
	static timestamp fromEpochTime(std::time_t t);
	/// Creates a timestamp from a std::time_t.

	static timestamp fromUtcTime(int64_t val);
	/// Creates a timestamp from a UTC time value
	/// (100 nanosecond intervals since midnight,
	/// October 15, 1582).

	static int64_t resolution();
	/// Returns the resolution in units per second.
	/// Since the timestamp has microsecond resolution,
	/// the returned value is always 1000000.

#if defined(_WIN32)
	static timestamp fromFileTimeNP(uint32_t fileTimeLow, uint32_t fileTimeHigh);
	void toFileTimeNP(uint32_t& fileTimeLow, uint32_t& fileTimeHigh) const;
#endif
private:
	int64_t _ts;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	datetime  类		2015-5-12
//	日期时间类封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class datetime
{
public:
	enum Months
		/// Symbolic names for month numbers (1 to 12).
	{
		January = 1,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December
	};

	enum DaysOfWeek
		/// Symbolic names for week day numbers (0 to 6).
	{
		Sunday = 0,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday
	};
	static const std::string MONTH_NAMES[12];
public:
	datetime();
	datetime(const timestamp& stamp);
	/// Creates a DateTime for the date and time given in
	/// a Timestamp.

	datetime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
	/// Creates a DateTime for the given Gregorian date and time.
	///   * year is from 0 to 9999.
	///   * month is from 1 to 12.
	///   * day is from 1 to 31.
	///   * hour is from 0 to 23.
	///   * minute is from 0 to 59.
	///   * second is from 0 to 59.
	///   * millisecond is from 0 to 999.
	///   * microsecond is from 0 to 999.

	datetime(double julianDay, bool isJulian = true);
	/// Creates a DateTime for the given Julian day.

	datetime(int64_t utcTime, int64_t diff);
	/// Creates a DateTime from an UtcTimeVal and a TimeDiff.
	///
	/// Mainly used internally by DateTime and friends.

	//datetime(int year, int month, int day, int hour, int mintue, int second);
	datetime(const std::tm &ttm);
	datetime(const FILETIME &fileTime);
	datetime(const SYSTEMTIME &sysTime);
	datetime(const datetime &st);
	~datetime();

	datetime& operator = (const datetime& dateTime);
	/// Assigns another DateTime.
	datetime& operator = (const timestamp& stamp);
	/// Assigns a Timestamp.
	datetime& operator = (double julianDay);
	/// Assigns a Julian day.
	datetime& operator = (const std::tm &ttm);
	datetime& operator = (const FILETIME &fileTime);
	datetime& operator = (const SYSTEMTIME &sysTime);
	operator DATE() const;

	bool operator == (const datetime& dateTime) const {		return _utcTime == dateTime._utcTime;		}
	bool operator != (const datetime& dateTime) const {		return _utcTime != dateTime._utcTime;		}
	bool operator <  (const datetime& dateTime) const {		return _utcTime < dateTime._utcTime;		}
	bool operator <= (const datetime& dateTime) const {		return _utcTime <= dateTime._utcTime;		}
	bool operator >  (const datetime& dateTime) const {		return _utcTime > dateTime._utcTime;		}
	bool operator >= (const datetime& dateTime) const {		return _utcTime >= dateTime._utcTime;		}

	datetime  operator +  (const timespan& span) const;
	datetime  operator -  (const timespan& span) const;
	timespan  operator -  (const datetime& dateTime) const;
	datetime& operator += (const timespan& span);
	datetime& operator -= (const timespan& span);

	void setMSDate(DATE date);
	DATE getMSDate() const;

	double julianDay() const;
	/// Returns the julian day for the date and time.

	timestamp timeStamp() const;
	/// Returns the date and time expressed as a Timestamp.

	int64_t utcTime() const {			return _utcTime;											}
	int year() const {					return _year;												}
	int month() const {					return _month;												}
	int day() const {					return _day;												}
	int hour() const {					return _hour;												}
	int minute() const {				return _minute;												}
	int second() const {				return _second;												}
	int millisecond() const {			return _millisecond;										}
	int microsecond() const {			return _microsecond;										}
	int week(int firstDayOfWeek = Monday) const;
	int dayOfWeek() const;
	int dayOfYear() const;
	datetime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
	
	void makeUTC(int tzd);
	/// Converts a local time into UTC, by applying the given time zone differential.
	// 以当前系统所属时区转换到UTC
	void makeUTC();
	void makeLocal(int tzd);
	/// Converts a UTC time into a local time, by applying the given time zone differential.
	// 转换到系统当前时区时间
	void makeLocal();	
	//// 转换
	//std::time_t toTime();
	//void asSystemTime(SYSTEMTIME &st) const;
	//bool changeToUTC();
	//bool changeToLocal();
	// 格式化输出
	std::wstring format(PCWSTR pformat = L"%Y-%m-%d %H:%M:%S" /*YYYY-MM-DD HH:MM:SS*/);
	bool parse(std::wstring wsdt, PCWSTR pformat = L"%Y-%m-%d %H:%M:%S" /*YYYY-MM-DD HH:MM:SS*/);
public:
	// 是否为闰年
	static bool isLeapYear(int year);
	// 某月所保有的天数
	static int daysOfMonth(int year, int month);
	// 是否为有效时间格式
	static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
	// 得到当前本地时间
	static int currentTimeZone();
	// 解析时间
	static bool parse(const std::string& fmt, const std::string& strDatetime, datetime& dateTime, int& timeZoneDifferential);
	static bool format(const datetime &dateTime, const std::string& fmt, std::string& strDatetime);
protected:
	static int parseMonth(std::string::const_iterator& it, const std::string::const_iterator& end);
	static int parseTZD(std::string::const_iterator& it, const std::string::const_iterator& end);
	static int parseAMPM(std::string::const_iterator& it, const std::string::const_iterator& end, int hour);
protected:	
	static double toJulianDay(int64_t utcTime);
	/// Computes the Julian day for an UTC time.

	static double toJulianDay(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
	/// Computes the Julian day for a gregorian calendar date and time.
	/// See <http://vsg.cape.com/~pbaum/date/jdimp.htm>, section 2.3.1 for the algorithm.

	static int64_t toUtcTime(double julianDay);
	/// Computes the UTC time for a Julian day.

	void computeGregorian(double julianDay);
	/// Computes the Gregorian date for the given Julian day.
	/// See <http://vsg.cape.com/~pbaum/date/injdimp.htm>, section 3.3.1 for the algorithm.

	void computeDaytime();
	/// Extracts the daytime (hours, minutes, seconds, etc.) from the stored utcTime.

private:
	//std::tm m_tm;

	void checkLimit(short& lower, short& higher, short limit);
	void normalize();
	///utility functions used to correct the overflow in computeGregorian

	int64_t _utcTime;
	short  _year;
	short  _month;
	short  _day;
	short  _hour;
	short  _minute;
	short  _second;
	short  _millisecond;
	short  _microsecond;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	lunarCalendar  类		2015-8-5
//	阴历日历
////////////////////////////////////////////////////////////////////////////////////////////////////////
class lunarCalendar
{
protected:
	static const WORD START_YEAR; 
	static const WORD END_YEAR;

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5 阳历到阴历的转换
	static datetime toLunar(const gtc::datetime &dtSolar);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5 阴历到阳历的转换
	static datetime toSolar(const gtc::datetime &dtLunar);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5		返回阴历iLunarYear年的闰月月份，如没有返回0。1901年1月---2050年12月
	static WORD getLeapMonth(WORD iLunarYear);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5		返回阴历iLunarYer年阴历iLunarMonth月的天数，如果iLunarMonth为闰月，
	//				高字为第二个iLunarMonth月的天数，否则高字为0 
	//				1901年1月---2050年12月
	static LONG lunarMonthDays(WORD iLunarYear, WORD iLunarMonth);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5 返回阴历iLunarYear年的总天数。1901年1月---2050年12月
	static WORD lunarYearDays(WORD iLunarYear);

	// 格式化输出
	static std::wstring formatMonth(int month, bool bUnit = true /* +月 */);
	static std::wstring formatDay(int day, bool bUnit = true /* +日 */);
	static std::wstring formatWeek(int week, bool bUnit = true /* 星期+ */);
protected:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5 	计算从1901年1月1日过iSpanDays天后的阴历日期
	static void calcLunarDate(WORD &iYear, WORD &iMonth ,WORD &iDay, LONG iSpanDays);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-5 计算公历两个日期间相差的天数  1年1月1日 --- 65535年12月31日
	static LONG calcDateDiff(WORD iEndYear, WORD iEndMonth, WORD iEndDay,
		WORD iStartYear = START_YEAR, 
		WORD iStartMonth =1, WORD iStartDay =1);	
};
}	// gtc

#endif	// __DATETIME_H__