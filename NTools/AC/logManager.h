#pragma once
#ifndef __LOGMANAGER_H__
#define __LOGMANAGER_H__
#include "AC/basedef.h"
#include "AC/singleton.h"

namespace gtc
{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	logManager类	2015-1-15
//	采用单实例的日志管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class logManager : public singleton<logManager>
{
public:
	// 特例化singleton的定义
	static logManager& getSingleton();
	static logManager* getSingletonPtr();

};

}

#endif