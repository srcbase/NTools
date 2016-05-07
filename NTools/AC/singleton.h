#pragma once
#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include <cassert>
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	singleton 类	2015-1-15
//	单实例模板类
////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class singleton
{
private:
	singleton(const singleton<T> &);
	singleton& operator=(const singleton<T> &);
public:

	singleton(void)
	{
		assert(!m_pSingleton);
		m_pSingleton = static_cast<T*>(this);
	}

	~singleton(void)
	{
		assert(m_pSingleton);
		m_pSingleton = nullptr;
	}

	static T& getSingleton()
	{
		assert(m_pSingleton);
		return *m_pSingleton;
	}

	static T* getSingletonPtr()
	{
		return m_pSingleton;
	}
protected:
	static T* m_pSingleton;
};


}	// end gtc
#endif
