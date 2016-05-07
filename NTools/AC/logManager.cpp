#include "logManager.h"
#include "AC/baseException.h"
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	logManager类	2015-1-15
//	采用单实例的日志管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////
template<> logManager* singleton<logManager>::m_pSingleton = NULL;

logManager& logManager::getSingleton()
{
	assert(m_pSingleton);
	return *m_pSingleton;
}
logManager* logManager::getSingletonPtr()
{
	return m_pSingleton;
}







} // end gtc