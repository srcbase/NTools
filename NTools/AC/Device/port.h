#pragma once

#include "device.h"

namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- 与串口通讯相关定义 entry[]	2014-02-12 --- >>
////////////////////////////////////////////////////////////////////////////////////////////////////////
// RS485地址
static entry* gc_tbRS485Address[] = {
	new entry(0,	L"广播地址"), 
	new entry(1,	L"1"), 
	new entry(2,	L"2"), 
	new entry(3,	L"3"), 
	new entry(4,	L"4"), 
	new entry(5,	L"5"), 
	new entry(6,	L"6"), 
	new entry(7,	L"7"), 
	new entry(8,	L"8"), 
	new entry(9,	L"9"), 
	new entry(10,	L"10"), 
	new entry(11,	L"11"), 
	new entry(12,	L"12"), 
	new entry(13,	L"13"), 
	new entry(14,	L"14"), 
	new entry(15,	L"15"), 
	new entry(16,	L"16"), 
	new entry(17,	L"17"), 
	new entry(18,	L"18"), 
	new entry(19,	L"19"), 
	new entry(20,	L"20"), 
	new entry(21,	L"21"), 
	new entry(22,	L"22"), 
	new entry(23,	L"23"), 
	new entry(24,	L"24"), 
	new entry(25,	L"25"), 
	new entry(26,	L"26"), 
	new entry(27,	L"27"), 
	new entry(28,	L"28"), 
	new entry(29,	L"29"), 
	new entry(30,	L"30"), 
	new entry(31,	L"31")
};

// 通讯端口
static entry* gc_tbSerialPort[] = {
	new entry(0,	L"COM1"), 
	new entry(1,	L"COM2"), 
	new entry(2,	L"COM3"), 
	new entry(3,	L"COM4"), 
	new entry(4,	L"COM5"), 
	new entry(5,	L"COM6"), 
	new entry(6,	L"COM7"), 
	new entry(7,	L"COM8"), 
	new entry(8,	L"COM9"), 
	new entry(9,	L"COM10"), 
	new entry(10,	L"COM11"), 
	new entry(11,	L"COM12"), 
	new entry(12,	L"COM13"), 
	new entry(13,	L"COM14"), 
	new entry(14,	L"COM15"), 
	new entry(15,	L"COM16"), 
	new entry(16,	L"COM17"), 
	new entry(17,	L"COM18"), 
	new entry(18,	L"COM19"), 
	new entry(19,	L"COM20"), 
	new entry(20,	L"COM21"), 
	new entry(21,	L"COM22"), 
	new entry(22,	L"COM23"), 
	new entry(23,	L"COM24"), 
	new entry(24,	L"COM25"), 
	new entry(25,	L"COM26"), 
	new entry(26,	L"COM27"), 
	new entry(27,	L"COM28"), 
	new entry(28,	L"COM29"), 
	new entry(29,	L"COM30"), 
	new entry(30,	L"COM31"), 
	new entry(31,	L"COM32")
};
// 波特率
static entry* gc_tbSerialBaudrate[] = {
	new entry(110,	L"110"),
	new entry(300,	L"300"),
	new entry(600,	L"600"),
	new entry(1200,	L"1200"),
	new entry(2400,	L"2400"),
	new entry(4800,	L"4800"),
	new entry(9600,	L"9600"),
	new entry(19200,	L"19200"),
	new entry(38400,	L"38400"),
	new entry(57600,	L"57600"),
	new entry(115200,	L"115200")
};
// 数据位
static entry* gc_tbSerialDatabit[] = {
	new entry(5, L"5"),
	new entry(6, L"6"),
	new entry(7, L"7"),
	new entry(8, L"8")
};
// 停止位
static entry* gc_tbSerialStopbit[] = {
	new entry(1, L"1"),
	new entry(2, L"2")
};
// 奇偶校验
static entry* gc_tbSerialParity[] = {
	new entry('N', L"无校验 N"),
	new entry('E', L"偶校验 E"),
	new entry('O', L"奇校验 O")
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- serialPort	2013-12-27 --- >>
//	串口通讯
////////////////////////////////////////////////////////////////////////////////////////////////////////
class serialPort : public device , public IChannel
{
	_DECLARE_ITOBJ_(serialPort)
public:
	typedef std::vector<serialPort*> PtrArrayType;
public:
	typedef enum BESPortProp {
		BESPortPropPort=1,BESPortPropBaud,BESPortPropDatabit,BESPortPropStopbit,BESPortPropParity
	}BESPortProp;

public:
	serialPort();
	serialPort(const wchar_t *pname, int32_t port, int32_t baud, BEDeviceModel model = BEDeviceModelNone);
	serialPort(const serialPort &sport);
	serialPort& operator=(const serialPort &sport);
	virtual ~serialPort(void);
	// 描述
	std::wstring description(int indent = 4) const;
	/*****************************************************************************************
		<< --- device::clone	2013-11-29 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual itobj* clone() {	
		itobj* pdevice = new serialPort(*this);
		return pdevice;
	}
public:
	// IChannel
	/*****************************************************************************************
		<< --- serialPort::open	2013-12-27 --- >>
		说明：打开与设备的通讯链路
		参数：
		返回值：动作是否执行成功
	*****************************************************************************************/
	virtual bool open();
	/*****************************************************************************************
		<< --- serialPort::close	2013-12-27 --- >>
		说明：关闭与设备的通讯链路
		参数：
		返回值：动作是否执行成功
	*****************************************************************************************/
	virtual void close();
	virtual bool isOpened() const;
	virtual void* getChannelHandle();
	virtual PCWSTR getName() const;
	virtual int32_t getChannelPort() const;
public:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// WIN32 的虚继承
#if defined(_WIN32)
	// 
	virtual void draw(CDC *pdc);

	/*****************************************************************************************
		<< --- static device::allocPropertySectionArray	2013-12-06 --- >>
		说明：分配对象属性
		参数：
		返回值：
	*****************************************************************************************/
	static std::size_t allocPropertySectionArray(itobj* pitem, bool isShare);
	virtual std::size_t allocPropertySectionArray(bool isShare) {
		return allocPropertySectionArray(this, isShare);
	}
	/*****************************************************************************************
		<< --- device::updatePropertyValue	2013-12-06 --- >>
		说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
		参数：
		pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
		返回值：更改成功
	*****************************************************************************************/
	virtual bool updatePropertyValue(itobjProp *pprop);
	/*****************************************************************************************
		<< --- device::messageProperty	2013-12-24 --- >>
		说明：更新指定属性标记的属性值，并将消息WM_ME_ITEM_PROPERTY_CHANGED发送给属性编辑窗口
		参数：
		typeKey		= 对象类型；如：BEDrawShapeType
		vecItemKey	= 需要更新的属性标记
		返回值：
	*****************************************************************************************/
	virtual void messageProperty(int32_t typeKey, std::vector<int32_t> &vecItemKey);
#endif

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

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<device>(*this);
		ar & m_nDatabit;  
		ar & m_cStopbit;
		ar & m_cParity;
		ar & m_iBaud;
		ar & m_iPort;
		//ar & m_vecChannels;
	}
#endif
protected:
	uint8_t		m_nDatabit;		// 数据位的个数
	int8_t		m_cStopbit;		// 停止位的个数；如：	1=ONESTOPBIT，2=TWOSTOPBITS
	int8_t		m_cParity;		// 奇偶校验位；如：'N', 'O', 'E'
	int32_t		m_iBaud;		// 波特率；如：9600, 19200, 57600, 115200, etc
	int32_t		m_iPort;		// 端口号；如：0=com1,1=com2...

	// 端口句柄
	modbus_t*	m_pctxPort;

};
}