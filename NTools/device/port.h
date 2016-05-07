#ifndef __DEV_PORT_H__
#define __DEV_PORT_H__
#pragma once
#include "device.h"

namespace dev {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IChannel  类    2016-1-12
//	通讯设备接口
////////////////////////////////////////////////////////////////////////////////////////////////////////
class IChannel
{
public:
	virtual ~IChannel() {}
	virtual bool open() = 0;
	virtual void close() = 0;
	virtual bool isOpened() const = 0;
	//virtual void* getChannelHandle() = 0;
	//virtual std::wstring getPortName() const = 0;
	//virtual int32_t getChannelPort() const = 0;

};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	serialPort  类    2016-1-12
//	通讯端口基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class serialPort : public device, public IChannel
{
public:
	serialPort();
	virtual ~serialPort();

public:
	//////////////////////////////////////////////////////////////////////////
	//	2016-1-12	IChannel
	virtual bool open();
	virtual void close();
	virtual bool isOpened() const;
	//virtual void* getChannelHandle();
	//virtual std::wstring getPortName() const;
	//virtual int32_t getChannelPort() const;

public:
	//////////////////////////////////////////////////////////////////////////
	//	2016-1-24	枚举系统可用的串口
	static IntSetType enumPort(bool bIgnoreBusyPorts = true);
public:
	// 属性
	uint8_t	getDatabit() const {				return m_nDatabit;							}
	void	setDatabit(uint8_t databit) {
		if(databit >= 5 && databit <= 8)
			m_nDatabit = databit;
		else
			m_nDatabit = 8;
	}
	int8_t	getStopbit() const {				return m_cStopbit;							}
	void	setStopbit(int8_t stopbit) {
		m_cStopbit = stopbit==1?1:2;
	}
	int8_t	getParity() const {					return m_cParity;							}
	void	setParity(int8_t parity) {
		if(parity == 'O' || parity == 'o')
			parity = 'O';
		else if(parity == 'E' || parity == 'e')
			parity = 'E';
		else
			parity = 'N';
		m_cParity = parity;
	}
	int32_t	getBaudrate() const {					return m_iBaud;							}
	void	setBaudrate(int32_t baud) {
		switch(baud)
		{
		case 110:
		case 300:
		case 600:
		case 1200:
		case 2400:
		case 4800:
		case 9600:
		case 19200:
		case 38400:
		case 57600:
		case 115200:
			m_iBaud = baud;
			break;
		default:
			m_iBaud = 9600;
			break;
		}
	}
	int32_t	getPort() const {					return m_iPort;								}
	void	setPort(int32_t port) {				m_iPort = port < 0?0:port;					}
private:
	uint8_t		m_nDatabit;		// 数据位的个数
	int8_t		m_cStopbit;		// 停止位的个数；如：	1=ONESTOPBIT，2=TWOSTOPBITS
	int8_t		m_cParity;		// 奇偶校验位；如：'N', 'O', 'E'
	int32_t		m_iBaud;		// 波特率；如：9600, 19200, 57600, 115200, etc
	int32_t		m_iPort;		// 端口号；如：0=com1,1=com2...

	// 端口句柄
	//modbus_t*	m_pctxPort;
};


}	// dev

#endif	// __DEV_PORT_H__