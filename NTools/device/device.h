#ifndef __DEV_DEVICE_H__
#define __DEV_DEVICE_H__
#pragma once

#include "devdef.h"
namespace dev {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	device  类    2016-1-11
//	所有设备基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class device
{
public:
	device();
	virtual ~device();

public:
	void setName(std::wstring wsname) {				m_wsName = wsname;						}
	const std::wstring& getName() const {			return m_wsName;						}
	void setAddress(int32_t addr) {					m_iAddress = addr;						}
	int32_t getAddress() const {					return m_iAddress;						}
	void setModel(BEDeviceModel model) {			m_beModel = model;						}
	BEDeviceModel getModel() const {				return m_beModel;						}
protected:
	std::wstring	m_wsName;		// 设备名称
	int32_t			m_iAddress;		// 设备地址
	BEDeviceModel	m_beModel;		// 设备型号
};


}	// dev

#endif	// __DEV_DEVICE_H__