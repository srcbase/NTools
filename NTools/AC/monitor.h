#pragma once
#include "AC/basedef.h"
#include "AC/sysupport.h"

namespace acm{
class monitor
{
public:
	monitor(void);
	virtual ~monitor(void);



public:
	int getNumber() const;
	void setNumber(int devNum);
	bool isPrimaryScreen() const;
	std::wstring getDeviceName() const;
	std::wstring getDeviceDescription() const;
	RECT getWorkArea() const;
	SIZE getWorkAreaSize() const;
	std::wstring toString() const;
public:
	static int monitorCount();
protected:
	static int mc_iMaxCount;	// ÆÁÄ»ÊýÁ¿
	int m_iMonitor;		// ¼àÊÓÆ÷ÐòºÅ
	DWORD m_dwStateFlag;// 
	std::wstring m_wsDevName;
	std::wstring m_wsDevDesc;
	RECT m_rcWorkArea;
};

}