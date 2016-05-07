#ifndef __SYNCOBJECT_H__
#define __SYNCOBJECT_H__
#pragma once
#include "sysupport.h"

#if defined(GTC_BOOST_THREAD)
#include "boost/thread.hpp"

//D:/NTools/boost/lib/libboost_thread-vc100-mt-gd-1_59.lib
//D:/NTools/boost/lib/libboost_date_time-vc100-mt-gd-1_59.lib
//D:/NTools/boost/lib/libboost_system-vc100-mt-gd-1_59.lib
//D:/NTools/boost/lib/libboost_chrono-vc100-mt-gd-1_59.lib
//
//#endif
#endif
//#define SYNC_OBJECT_LOG_ON		// 是否显示日志开关

namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	criticalSection  类		2015-7-14
//	对CRITICAL_SECTION封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class criticalSection
{
public:
	criticalSection();
	~criticalSection();

	bool tryEnter(PCWSTR pdesc = NULL);
	void enter(PCWSTR pdesc = NULL);
	void leave(PCWSTR pdesc = NULL);
private:
	CRITICAL_SECTION		m_cs;
	std::wstring			m_strPrevEntry;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	optex  类		2015-5-26
//	互斥同步锁类，可跨进程(带名称)
////////////////////////////////////////////////////////////////////////////////////////////////////////
class optex
{
public:
	optex(DWORD dwSpinCount = 4000) 
	{
		commonConstructor(dwSpinCount, NULL);
	}
	optex(PCWSTR pName, DWORD dwSpinCount = 4000)
	{
		commonConstructor(dwSpinCount, pName);
	}
	~optex();
	void setSpinCount(DWORD dwSpinCount);
	void enter(PCWSTR pDesc = NULL);
	bool tryEnter(PCWSTR pDesc = NULL);
	void leave(PCWSTR pDesc = NULL);
	bool isSingleProcessOptex() const {
		return m_hFileMap == NULL;
	}

	void setDescription(PCWSTR pDesc);
	std::wstring getDescription() const;

	std::wstring toString() const;
private:
	void commonConstructor(DWORD dwSpinCount, PCWSTR pName);
private:
	// 进程间共享的数据
	typedef struct {
		DWORD	dwSpinCount;	// 指明试图进入optex的线程在等待事件内核对象之前应该尝试进入的次数。在单
								// 处理器计算机上，这个值总是0
		long	lLockCount;		// 指明线程试图进入optex的次数。如果没有线程进入optex，那么这个值是0
		DWORD	dwProcessID;	// 进程ID
		DWORD	dwThreadId;		// 指明拥有optex的线程的唯一ID。如果没有线程拥有optex，那么这个值是0
		long	lRecurseCount;	// 指明线程拥有optex的次数。如果optex没有被线程所拥有，则这个值是0
	} SHAREDINFO, *PSHAREDINFO;
private:
	HANDLE			m_hEvent;	// 这是个事件内核对象的句柄，只有当一个线程试图在另一个线程拥有optex时进
								// 入该optex，才使用这个句柄。内核对象句柄是与进程相关的句柄，这就是该成员
								// 为什么不使用SHAREDINFO结构的原因
	HANDLE			m_hFileMap;	// 这是文件映象内核对象的句柄，当多进程共享一个optex时，便使用这个句柄。
								// 内核对象句柄属于与进程相关的句柄，这就是为什么该成员不是SHAREDINFO结
								// 构的原因。对于单进程optex来说，这个值总是NULL
	PSHAREDINFO		m_psi;		// 这是指向潜在的共享optex数据成员的指针。内存地址是与进程相关的地址，这
								// 就是为什么该成员不使用SHAREDINFO结构的原因。对于单进程optex来说，它
								// 指向一个堆栈分配的内存块。对于多进程optex来说，它指向一个内存映射文件
	std::wstring	m_wsDesc;	// 

	std::wstring	m_wsPrevEntry;
	static int mc_bUniprocessorHost;	// 是否为单处理器 1：单处理器 0：多处理器 -1：该值未处理
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	swmrg  类		2015-5-26
//	单个写入程序/多个阅读程序的保护
//	4个原则：
//	1) 当一个线程正在写入数据时，其他任何线程不能写入数据。
//	2) 当一个线程正在写入数据时，其他任何线程不能读取数据。
//	3) 当一个线程正在读取数据时，其他任何线程不能写入数据。
//	4) 当一个线程正在读取数据时，其他线程也能够读取数据。
////////////////////////////////////////////////////////////////////////////////////////////////////////
class swmrg
{
public:
	swmrg(PCWSTR pDesc = NULL);
	~swmrg();

	void waitToRead(PCWSTR pDesc = NULL);        // 执行读取共享资源
	void waitToWrite(PCWSTR pDesc = NULL);       // 执行读取或写入共享资源的
	void done(PCWSTR pDesc = NULL);              // 不再访问共享资源时

	void setDescription(PCWSTR pDesc);
	std::wstring getDescription() const;

	std::wstring toString() const;
private:
	CRITICAL_SECTION	m_cs;				// 用于保护所有的其他成员变量，这样，对它们的操作就能够以原子操作方式来完成
	HANDLE				m_hsemReaders;		// 当许多线程调用waitToRead，但是由于m_nActivee是-1而被拒绝访问时，
											// 所有阅读线程均等待该信标。当最后一个正在等待的阅读线程调用done时，
											// 该信标被释放，其数量是m_nWaitingReaders，从而唤醒所有正在等待的阅读线程
	HANDLE				m_hsemWriters;		// 当线程调用waitToWrite，但是由于m_nActive大于0而被拒绝访问时，所
											// 有写入线程均等待该信标。当一个线程正在等待时，新阅读线程将被拒绝
											// 访问该资源。这可以防止阅读线程垄断该资源。当最后一个拥有资源访问
											// 权的阅读线程调用done时，该信标就被释放，其数量是1，从而唤醒一个正
											// 在等待的写入线程
	int					m_nWaitingReaders;	// 表示想要访问资源的阅读线程的数量。该值被初始化为0，当m_nActive
											// 是-1时，每当线程调用一次waitToRead，该值就递增1
	int					m_nWaitingWriters;	// 表示想要访问资源的写入线程的数量。该值被初始化为0，当m_nActive大
											// 于0时，每当线程调用一次waitToWrite，该值就递增1
	int					m_nActive;			// 用于反映共享资源的当前状态。如果该值是0，那么没有线程在访问资源。
											// 如果该值大于0，这个值用于表示当前读取该资源的线程的数量。如果这个
											// 数量是负值，那么写入程序正在将数据写入该资源。唯一有效的负值是-1
	std::wstring		m_wsDesc;			// 
	std::wstring		m_wsWriter;			// 当前写者描述
	std::wstring		m_wsReader;			// 当前读者描述

};

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-10-8 以下是基于boost的同步
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	readLock  类		2015-10-8
//	读取锁保护，boost::shared_lock<boost::shared_mutex>锁不支持嵌套，所以封装了下
////////////////////////////////////////////////////////////////////////////////////////////////////////
class readLock 
{
private:
	readLock();
	readLock(const readLock &rlk);
	readLock& operator=(const readLock &rlk);
public:
#if defined(GTC_BOOST_THREAD)
	readLock(boost::shared_mutex &rdMutex, bool &bLocked, PCWSTR pfile = NULL, PCWSTR pfunc = NULL)
		: m_readLock(rdMutex, boost::defer_lock), m_brefLocked(bLocked)
	{
		if (!m_brefLocked)
		{
			m_readLock.lock();
			m_brefLocked = m_readLock.owns_lock();
#if defined(_DEBUG)
			wchar_t pbuf[MAX_PATH] = {0};
			std::swprintf(pbuf, _countof(pbuf), L"mutex:<%08X> threadID:<%d> file:<%s> func:<%s>", &rdMutex, GetCurrentThreadId(), pfile ?pfile :L"", pfunc ?pfunc :L"");
			mc_mapPreInfo[(INT_PTR)&rdMutex] = pbuf;

#endif
		}
	}
#endif
	~readLock() 
	{
#if defined(GTC_BOOST_THREAD)
		if(m_readLock.owns_lock())
			m_brefLocked = false;
#else
		assert(!L"此处还未定义！！！");
#endif
		//m_brefLocked = m_readLock.owns_lock();
	}

private:
#if defined(GTC_BOOST_THREAD)

	boost::shared_lock<boost::shared_mutex>	m_readLock;
#endif
	bool	&m_brefLocked;
#if defined(_DEBUG)
	static IStringMapType	mc_mapPreInfo;		// key=以同步锁的互斥量地址 value=前次读写信息

#endif

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	writeLock  类		2015-10-8
//	为了统一
////////////////////////////////////////////////////////////////////////////////////////////////////////
class writeLock
{
public:
#if defined(GTC_BOOST_THREAD)
	writeLock(boost::shared_mutex &wtMutex, bool &bLocked, PCWSTR pfile = NULL, PCWSTR pfunc = NULL)
		: m_writeLock(wtMutex, boost::defer_lock), m_brefLocked(bLocked)
	{
		//if (!m_brefLocked)
		{
			if(m_brefLocked) 
				Sleep(0);
			m_writeLock.lock();
			m_brefLocked = m_writeLock.owns_lock();
#if defined(_DEBUG)
			wchar_t pbuf[MAX_PATH] = {0};
			std::swprintf(pbuf, _countof(pbuf), L"mutex:<%08X> threadID:<%d> file:<%s> func:<%s>", &wtMutex, GetCurrentThreadId(), pfile ?pfile :L"", pfunc ?pfunc :L"");
			mc_mapPreInfo[(INT_PTR)&wtMutex] = pbuf;

#endif
		}
//		else
//		{
//#if defined(_DEBUG)
//			IStringMapType::const_iterator iter = mc_mapPreInfo.find((INT_PTR)&wtMutex);
//			if (iter != mc_mapPreInfo.cend())
//			{
//				SEND_LOG(L"NOTE writeLock 已经被锁定！！！！！！%s", iter->second.c_str());
//			}
//
//#endif
//			//assert(!L"已经被锁定！！！");
//		}
	}
#endif

	~writeLock()
	{
#if defined(GTC_BOOST_THREAD)
		if(m_writeLock.owns_lock())
			m_brefLocked = false;
#else
		assert(!L"此处还未定义！！！");
#endif
	}
private:
#if defined(GTC_BOOST_THREAD)

	boost::unique_lock<boost::shared_mutex>	m_writeLock;
#endif
	bool					&m_brefLocked;
#if defined(_DEBUG)
	static IStringMapType	mc_mapPreInfo;		// key=以同步锁的互斥量地址 value=前次读写信息

#endif
};

}	// gtc
#endif	// end __SYNCOBJECT_H__