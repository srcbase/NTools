#pragma once

#include "AC/sysupport.h"

namespace gtc {

//////////////////////////////////////////////////////////////////////////
// archive类 2014-08-24
// 序列化基类
//////////////////////////////////////////////////////////////////////////
class archive
{
public:
	// 操作模式：写入或读取
	typedef enum BEMode { BEModeStore = 0, BEModeLoad = 1 } BEMode;
	// 数据格式
	typedef enum BEFormat { BEFormatBinary = 0, BEFormatXML } BEFormat;
protected:
	archive(const archive &ar);
	archive& operator =(const archive &ar);
public:
	
public:
	// 属性
	bool isStoring() const;
	bool isLoading() const;

	
protected:
	BEMode		m_beMode;		// 操作模式
	BEFormat	m_beFormat;		// 数据格式
};

}