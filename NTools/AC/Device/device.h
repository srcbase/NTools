#pragma once
#include "AC/items/items.h"
#include "libmodbus/modbus.h"

#if defined(BOOST_SERIALIZE)
#include "../../boost/serialization/map.hpp"

#endif

namespace gtc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	deviceDataImpl  类    2015-8-2
//	对设备用到的数据
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- IChannel	2014-06-22 --- >>
//	串口通讯
////////////////////////////////////////////////////////////////////////////////////////////////////////
class IChannel
{
public:
	typedef std::vector<IChannel *> PtrArrayType;
	/*****************************************************************************************
		<< --- serialPort::open	2013-12-27 --- >>
		说明：打开与设备的通讯链路，如果已经打开就增加内部计数器
		参数：
		返回值：动作是否执行成功
	*****************************************************************************************/
	virtual bool open() = 0;//{	return false;	}
	/*****************************************************************************************
		<< --- serialPort::close	2013-12-27 --- >>
		说明：关闭与设备的通讯链路，内部有一计数器，当减到为0时才真正关闭端口
		参数：
		返回值：动作是否执行成功
	*****************************************************************************************/
	virtual void close() = 0;//{}
	virtual bool isOpened() const = 0;//{			return false;					}
	virtual void* getChannelHandle() = 0;//{		return nullptr;					}
	virtual PCWSTR getName() const = 0;//{		return nullptr;					}
	virtual int32_t getChannelPort() const = 0;
protected:
	uint32_t	m_uiCounter;	// 引用计数器
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- reger	 2014-06-22 --- >>
//	寄存器单元参数
////////////////////////////////////////////////////////////////////////////////////////////////////////
class reger
{
public:
	typedef std::map<int32_t, reger*> MapType;
protected:
	reger();
public:
	reger(int32_t addr, const wchar_t *pname);
	reger(const reger &reg);
	reger& operator=(const reger &reg);
	virtual ~reger(void);
	/*****************************************************************************************
		<< --- reger::clone	2013-12-31 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual reger* clone() = 0;
	// 描述
	std::wstring description(int indent = 4) const;

	/*****************************************************************************************
		<< --- reger::read	2013-12-31 --- >>
		说明：读取寄存器的值
		参数：
		返回值：动作执行是否成功
	*****************************************************************************************/
	virtual bool read(modbus_t *phandle) = 0;
	/*****************************************************************************************
		<< --- reger::write	2013-12-31 --- >>
		说明：将值保存到寄存器
		参数：
		返回值：动作执行是否成功
	*****************************************************************************************/
	virtual bool write(modbus_t *phandle) = 0;
public:
	// 属性
	std::wstring	getName() const {					return m_wsName;					}
	void			setName(const wchar_t *pname) {		m_wsName = pname?pname:L"";			}
	void			setName(std::wstring &name) {		m_wsName = name;					}
	int32_t			getAddress() const {				return m_iAddress;					}
	void			setAddress(int32_t addr) {			m_iAddress = addr;					}
	bool			isChanged() const {					return m_isChanged != 0;			}

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_wsName;
		ar & m_iAddress;
		ar & m_isChanged;
	}
#endif

protected:
	std::wstring	m_wsName;		// 寄存器名称
	int32_t			m_iAddress;		// 寄存器地址
	//int32_t			iLenByte;	// 寄存器所占字节数
	//int32_t			m_iKeyFlag;		// 寄存器的区分标记
	int32_t			m_isChanged;	// 寄存器值是否有改变
};
//
class regerI4: public reger
{
protected:
	regerI4();
public:
	regerI4(int32_t addr, const wchar_t *pname, int32_t value);
	regerI4(const regerI4 &reg);
	regerI4& operator=(const regerI4 &reg);
	virtual ~regerI4(void);
	/*****************************************************************************************
		<< --- regerI4::clone	2013-12-31 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual reger* clone();
	// 描述
	std::wstring description(int indent = 4) const;
public:
	/*****************************************************************************************
		<< --- regerI4::read	2013-12-31 --- >>
		说明：读取寄存器的值
		参数：
		返回值：动作执行是否成功
	*****************************************************************************************/
	virtual bool read(modbus_t *phandle);
	/*****************************************************************************************
		<< --- regerI4::write	2013-12-31 --- >>
		说明：将值保存到寄存器
		参数：
		返回值：动作执行是否成功
	*****************************************************************************************/
	virtual bool write(modbus_t *phandle);

public:
	int32_t getValue() const {					return m_iValue;							}
	void setValue(int32_t val) {				m_iValue = val; m_isChanged = true;			}


#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<reger>(*this);
		ar & m_iValue;
	}
#endif

protected:
	int32_t		m_iValue;
};
//
class regerUI2: public reger
{
protected:
	regerUI2();
public:
	regerUI2(int32_t addr, const wchar_t *pname, uint16_t value);
	regerUI2(const regerUI2 &reg);
	regerUI2& operator=(const regerUI2 &reg);
	virtual ~regerUI2(void);
	/*****************************************************************************************
		<< --- regerUI2::clone	2013-12-31 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual reger* clone();
	// 描述
	std::wstring description(int indent = 4) const;
public:
	/*****************************************************************************************
		<< --- regerUI2::read	2013-12-31 --- >>
		说明：读取寄存器的值
		参数：
		返回值：动作执行是否成功
	*****************************************************************************************/
	virtual bool read(modbus_t *phandle);
	/*****************************************************************************************
		<< --- regerUI2::write	2013-12-31 --- >>
		说明：将值保存到寄存器
		参数：
		返回值：动作执行是否成功
	*****************************************************************************************/
	virtual bool write(modbus_t *phandle);

public:

	uint16_t getValue() const {					return m_usValue;							}
	void setValue(uint16_t val) {				m_usValue = val; m_isChanged = true;		}

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<reger>(*this);
		ar & m_usValue;
	}
#endif

protected:
	int32_t		m_usValue;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- device	2013-12-27 --- >>
//	所有设备单元的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class device : public itobj
{
	_DECLARE_ITOBJ_(device)
public:
	typedef std::vector<device *> PtrArrayType;
public:
	typedef enum BEDeviceProp {
		BEDevicePropName=1,BEDevicePropModelName,BEDevicePropAddress
	}BEDeviceProp;

public:
	// 设备的工作状态
	typedef enum BEDeviceStatus{
		BEDeviceStatusNone		= 0,	// 默认
		BEDeviceStatusReady		= 1,	// 就绪
		BEDeviceStatusOpened	,		// 已经打开
		BEDeviceStatusClosed	,		// 已经关闭
		BEDeviceStatusReading	,		// 正在读取数据
		BEDeviceStatusWriting			// 正在写数据
	}BEDeviceStatus;
	// 
	typedef enum BEDeviceModel {
		BEDeviceModelNone				= 0x0,		// 未知型号
		BEDeviceModelWeigherADS321		= 0x1000,	// 称重传感器 山东西泰克仪器有限公司SeTAQ AD-S321
		BEDeviceModelSCRPLC				= 0x2000,	// 调功板 大连赛伟电子PLC V2.2
		BEDeviceModelMotor				= 0x4000,	// 步进电机
		BEDeviceModelScale				= 0xF000	// 电子秤
	}BEDeviceModel;
	/*****************************************************************************************
		<< --- toStringBEDeviceModel		2014-02-12 --- >>
		说明：将枚举值转换成对应的字符描述
		参数：
		model = 被转换的枚举值
		返回值：返回枚举型的字符描述
	*****************************************************************************************/
	static std::wstring toStringBEDeviceModel(BEDeviceModel model)
	{
		switch (model)
		{
		case BEDeviceModelNone:
			return L"未定义";
			break;
		case BEDeviceModelWeigherADS321:
			return L"SeTAQ AD-S321";
			break;
		case BEDeviceModelSCRPLC:
			return L"可控硅调功板PLC";
			break;
		case BEDeviceModelMotor://				= 0x4000,	// 步进电机
			return L"步进电机";
			break;
		case BEDeviceModelScale://				= 0xF000	// 电子秤
			return L"电子秤";
			break;
		default:
			break;
		}
		return L"";
	}

	std::wstring	getModelName() const {
		switch (m_beModel)
		{
		case BEDeviceModelNone:
			return L"未定义";
			break;
		case BEDeviceModelWeigherADS321:
			return L"SeTAQ AD-S321";
			break;
		case BEDeviceModelSCRPLC:
			return L"可控硅调功板PLC";
			break;
		case BEDeviceModelMotor://				= 0x4000,	// 步进电机
			return L"步进电机";
			break;
		case BEDeviceModelScale://				= 0xF000	// 电子秤
			return L"电子秤";
			break;
		default:
			break;
		}
		return L"";
	}
public:
	//device();
	device(BEDeviceModel model = BEDeviceModelNone);
	device(const wchar_t *pname, BEDeviceModel model = BEDeviceModelNone);
	device(const device &dev);
	device& operator=(const device &dev);
	virtual ~device(void);
	// 描述
	std::wstring description(int indent = 4) const;
	/*****************************************************************************************
		<< --- device::clone	2013-11-29 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual itobj* clone() {	
		itobj *pdevice = new device(*this);
		return pdevice;
	}

public:
	// 设备控制公共方法
	///*****************************************************************************************
	//	<< --- device::initEnvironment	2013-12-27 --- >>
	//	说明：对设备进行初始化，以便设备能正常使用；如：分配存储区间，开启缓存数据处理线程等
	//	参数：
	//	返回值：动作是否执行成功
	//*****************************************************************************************/
	//virtual bool initEnvironment() {					assert(!L"未实现");return true;					}
	///*****************************************************************************************
	//	<< --  device::releaseEnvironment		2015-1-3 --- >>
	//	说明：释放initEnvironment准备的环境；如：释放分配的存储区，关闭数据处理线程等
	//	参数：
	//	返回值：动作是否执行成功
	//*****************************************************************************************/
	//virtual bool releaseEnvironment() {					assert(!L"未实现");return true;						}
	///*****************************************************************************************
	//	<< --- device::changeBaudrate	2014-02-10 --- >>
	//	说明：改变设备的通讯波特率
	//	参数：
	//	newBaudrate	= 新的波特率
	//	返回值：是否修改成功
	//*****************************************************************************************/
	//virtual bool changeBaudrate(int32_t	newBaudrate) {			assert(!L"未实现");return true;				}
	///*****************************************************************************************
	//	<< --- device::switchOff	2013-12-27 --- >>
	//	说明：切断设备，如电源，使设备不能工作
	//	参数：
	//	返回值：动作是否执行成功
	//*****************************************************************************************/
	//virtual bool switchOff() {				m_bOnline = false;	return true;				}
	///*****************************************************************************************
	//	<< --- device::switchOn	2013-12-27 --- >>
	//	说明：打开设备，如电源，使设备不能工作
	//	参数：
	//	返回值：动作是否执行成功
	//*****************************************************************************************/
	//virtual bool switchOn() {				m_bOnline = true;	return true;				}
	///*****************************************************************************************
	//	<< --- device::readParameter	2013-12-27 --- >>
	//	说明：读取设备参数
	//	参数：
	//	返回值：动作是否执行成功
	//*****************************************************************************************/
	virtual bool readParameter(UINT_PTR ptrData) {		assert(!L"未实现");return true;					}
	///*****************************************************************************************
	//	<< --- device::readParameter	2013-12-27 --- >>
	//	说明：将新的设备参数写入设备
	//	参数：
	//	返回值：动作是否执行成功
	//*****************************************************************************************/
	virtual bool writeParameter(UINT_PTR ptrData) {		assert(!L"未实现");return true;					}

	///*****************************************************************************************
	//	<< --- device::setChannels	2014-07-21 --- >>
	//	说明：设定系统中可以的通讯端口集合
	//	参数：
	//	pvecChannels	= 可以通行端口集合
	//	返回值：
	//*****************************************************************************************/
	//virtual void setChannels(const IChannel::PtrArrayType &vecChannels);
	//
	/*****************************************************************************************
		<< --- device::changeAddress	2014-01-26 --- >>
		说明：有些设备的通信地址保存在EEPROM中，调用本命令可改变地址
		参数：
		newAddress	= 新的通讯地址
		返回值：是否修改成功
	*****************************************************************************************/
	virtual bool changeAddress(uint32_t newAddress) {	assert(!L"未实现");return true;					}
	//////////////////////////////////////////////////////////////////////////
	//	2015-9-16	初始化设备运行环境
	virtual bool init(UINT_PTR ptrData) {				assert(!L"未实现");return true;					}
	//////////////////////////////////////////////////////////////////////////
	//	2015-9-16	启动设备
	virtual bool startup(UINT_PTR ptrData) {			assert(!L"未实现");return true;					}
	//////////////////////////////////////////////////////////////////////////
	//	2015-9-16	停止设备
	virtual bool stop(UINT_PTR ptrData) {				assert(!L"未实现");return true;					}
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
	std::wstring	getName() const {						return m_wsName;										}
	void			setName(const std::wstring &name) {		m_wsName = name;										}
	void			setName(const wchar_t *pname) {			m_wsName = pname?pname:L"";								}	
	uint32_t		getAddress() const {					return m_uiAddress;										}
	void			setAddress(uint32_t address) {			m_uiAddress = address;									}

	bool			online() const {						return m_bOnline;										}
	BEDeviceModel	getModel() const {						return m_beModel;										}


	void			setModel(BEDeviceModel model) {			m_beModel = model;										}
	int32_t			getPlace() const {						return m_iPlace;										}
	void			setPlace(int32_t place) {				m_iPlace = place;										}
	BEDeviceStatus	getStatus() const {						return m_beStatus;										}
	void			setStatus(BEDeviceStatus status) {		m_beStatus = status;									}
	int32_t			getOptCode() const {					return m_iOptCode;										}
	void			setOptCode(int32_t optCode) {			m_iOptCode = optCode;									}

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<itobj>(*this);
		ar & m_wsName;  
		ar & m_uiAddress;
		ar & m_bOnline;
		ar & m_beModel;
		ar & m_iPlace;
		ar & m_beStatus;
		ar & m_iOptCode;
		//ar & m_vecChannels;
	}
#endif
protected:
	std::wstring	m_wsName;			// 设备名称
	uint32_t		m_uiAddress;		// RS485设备通讯地址、IP地址
	//IChannel::PtrArrayType	m_vecChannels;	// 系统可用通讯端口集合
	bool			m_bOnline;			// 是否在线运行
	BEDeviceModel	m_beModel;			// 设备类型

	int32_t			m_iPlace;			// 设备所在位置
	BEDeviceStatus	m_beStatus;			// 当前设备的状态
	int32_t			m_iOptCode;			// 当前事务代码或操作错误码

};


}