#ifndef __CACHE_H__
#define __CACHE_H__
#include "AC/sysupport.h"
#include <deque>
namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	queueCache类	2014-12-22
//	当短时间有大量数据产生而系统无法及时响应时，可开启一个写线程和一个读取线程
//	isUnique：队列中是否支持重复插入
////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class Type, bool isUnique = true>
class queueCache {
public:
	queueCache()
	{
		::InitializeCriticalSection(&m_csLock);
	}
	~queueCache()
	{
		::DeleteCriticalSection(&m_csLock);
	}
public:
	bool push(const Type &val)
	{
		bool bOk = false;
		::EnterCriticalSection(&m_csLock);
		if (isUnique)
		{
			if(std::find(m_queData.cbegin(), m_queData.cend(), val) == m_queData.cend())
			{
				m_queData.push_back(val);
				bOk = true;
			}
		}
		else
		{
			m_queData.push_back(val);
			bOk = true;
		}

		::LeaveCriticalSection(&m_csLock);
		return bOk;
	}
	bool pop(Type &val)
	{
		bool bl = false;;
		::EnterCriticalSection(&m_csLock);
		if (m_queData.size() > 0)
		{
			val = m_queData.front();
			m_queData.pop_front();
			bl = true;
		}

		::LeaveCriticalSection(&m_csLock);

		return bl;
	}
	std::size_t size()
	{
		std::size_t sz = 0;
		::EnterCriticalSection(&m_csLock);
		sz = m_queData.size();
		::LeaveCriticalSection(&m_csLock);

		return sz;
	}
	bool isEmpty()
	{
		bool empty = false;
		::EnterCriticalSection(&m_csLock);
		empty = m_queData.empty();
		::LeaveCriticalSection(&m_csLock);

		return empty;
	}
	bool exists(const Type &val)
	{
		bool have = false;
		::EnterCriticalSection(&m_csLock);
		have = std::find(m_queData.cbegin(), m_queData.cend(), val) != m_queData.cend();
		::LeaveCriticalSection(&m_csLock);

		return have;
	}
protected:
	CRITICAL_SECTION	m_csLock;	// 数据保护锁

	std::deque<Type>	m_queData;
};

}

#endif