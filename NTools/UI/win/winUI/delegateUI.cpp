#include "StdAfx.h"
#include "delegateUI.h"


namespace wui{


delegateBase::delegateBase(void* pObject, void* pFn) 
{
	//TRACE_CLASS_BEGIN(this)
	m_pObject = pObject;
	m_pFn = pFn; 
}

delegateBase::delegateBase(const delegateBase& rhs) 
{
	//TRACE_CLASS_BEGIN(this)
	m_pObject = rhs.m_pObject;
	m_pFn = rhs.m_pFn; 
}

delegateBase::~delegateBase()
{
	//TRACE_CLASS_END(this)
}

bool delegateBase::equals(const delegateBase& rhs) const 
{
	return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool delegateBase::operator() (void* param) 
{
	return invoke(param); 
}

void* delegateBase::getFn() 
{
	return m_pFn; 
}

void* delegateBase::getObject() 
{
	return m_pObject; 
}

eventSource::~eventSource()
{
	//TRACE_CLASS_END(this)
	for (std::vector<delegateBase *>::iterator iter = m_aDelegates.begin(); iter != m_aDelegates.end(); ++iter)
	{
		delete *iter;
	}
	m_aDelegates.clear();
}

eventSource::operator bool()
{
	return m_aDelegates.size() > 0;
}

void eventSource::operator+= (delegateBase& d)
{ 
	//for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
	//	CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
	//	if( pObject && pObject->Equals(d) ) return;
	//}
	for (std::vector<delegateBase *>::iterator iter = m_aDelegates.begin(); iter != m_aDelegates.end(); ++iter)
	{
		if((*iter)->equals(d)) return;
	}
	//m_aDelegates.Add(d.Copy());
	m_aDelegates.push_back(d.copy());
}

void eventSource::operator+= (FnType pFn)
{ 
	(*this) += makeDelegate(pFn);
}

void eventSource::operator-= (delegateBase& d) 
{
	//for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
	//	CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
	//	if( pObject && pObject->Equals(d) ) {
	//		delete pObject;
	//		m_aDelegates.Remove(i);
	//		return;
	//	}
	//}

	for (std::vector<delegateBase *>::iterator iter = m_aDelegates.begin(); iter != m_aDelegates.end(); ++iter)
	{
		if ((*iter)->equals(d))
		{
			delete *iter;
			m_aDelegates.erase(iter);
			return;
		}
	}
}
void eventSource::operator-= (FnType pFn)
{ 
	(*this) -= makeDelegate(pFn);
}

bool eventSource::operator() (void* param) 
{
	//for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
	//	CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
	//	if( pObject && !(*pObject)(param) ) return false;
	//}
	for (std::vector<delegateBase *>::iterator iter = m_aDelegates.begin(); iter != m_aDelegates.end(); ++iter)
	{
		if (!((*(*iter))(param)))
		{
			return false;
		}
	}
	return true;
}

}