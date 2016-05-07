#include "delegateBase.h"

namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	eventSource  类		2015-8-27
//	事件源
////////////////////////////////////////////////////////////////////////////////////////////////////////
eventSource::eventSource()
#if defined(GTC_BOOST_THREAD)
	: m_bReadLocked(false), m_bWriteLocked(false)
#endif

{

}

eventSource::eventSource(const eventSource &es)
{
	operator=(es);
}
eventSource::~eventSource()
{
	for (delegateBase::PtrArrayType::iterator iter = m_vecDelegate.begin(); iter != m_vecDelegate.end(); ++iter)
	{
		delete *iter;
	}
	m_vecDelegate.clear();
}
eventSource& eventSource::operator= (const eventSource &es)
{
	if(this != &es)
	{
		WRITE_LOCK
		for (delegateBase::PtrArrayType::const_iterator iter = es.m_vecDelegate.cbegin(); iter != es.m_vecDelegate.cend(); ++iter)
		{
			m_vecDelegate.push_back((*iter)->copy());
		}
	}
	return *this;
}
eventSource& eventSource::operator= (const delegateBase& dbase)
{
	WRITE_LOCK
	for (delegateBase::PtrArrayType::iterator iter = m_vecDelegate.begin(); iter != m_vecDelegate.end(); ++iter)
	{
		delete *iter;
	}
	m_vecDelegate.clear();
	m_vecDelegate.push_back(dbase.copy());
	return *this;
}
eventSource::operator bool()
{
	return m_vecDelegate.size() > 0;
}
void eventSource::operator+= (const delegateBase& dbase)
{
	WRITE_LOCK
	delegateBase::PtrArrayType::iterator iter = std::find(m_vecDelegate.begin(), m_vecDelegate.end(), dbase);
	if (iter == m_vecDelegate.end())
	{
		m_vecDelegate.push_back(dbase.copy());
	}
}
void eventSource::operator+= (FUNC_TYPE pfunc)
{
	operator+=(makeDelegate(pfunc));
}
void eventSource::operator-= (const delegateBase& dbase)
{
	WRITE_LOCK
	delegateBase::PtrArrayType::iterator iter = std::find(m_vecDelegate.begin(), m_vecDelegate.end(), dbase);
	if (iter != m_vecDelegate.end())
	{
		delete *iter;
		m_vecDelegate.erase(iter);
	}
}
void eventSource::operator-= (FUNC_TYPE pfunc)
{
	operator-=(makeDelegate(pfunc));
}
void eventSource::operator()(WPARAM wParam, LPARAM lParam)
{
	WRITE_LOCK
	if(m_vecDelegate.size() == 0) return;
	for (delegateBase::PtrArrayType::iterator iter = m_vecDelegate.begin(); iter != m_vecDelegate.end(); ++iter)
	{
		(*(*iter))(wParam, lParam);
	}
}


}	// gtc