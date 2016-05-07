#include "device.h"
#if defined(_WIN32)
	#include <afxwin.h>
	#include "AC/MFC/NTool.h"
#endif
namespace gtc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- reger	 2014-06-22 --- >>
//	寄存器单元参数
////////////////////////////////////////////////////////////////////////////////////////////////////////
reger::reger()
	: m_iAddress(-1), m_wsName(L""), m_isChanged(false)
{

}

reger::reger(int32_t addr, const wchar_t *pname)
	: m_iAddress(addr), m_wsName(pname?pname:L""), m_isChanged(false)

{
}

reger::reger(const reger &reg)
	: m_iAddress(reg.m_iAddress), m_wsName(reg.m_wsName), m_isChanged(reg.m_isChanged)
{

}

reger& reger::operator=(const reger &reg) 
{
	if(this != &reg)
	{
		m_wsName = reg.m_wsName;
		m_iAddress = reg.m_iAddress;
		m_isChanged = reg.m_isChanged;
	}
	return *this;
}

reger::~reger(void) {
	DEBUG_LOG(L"free %s", description().c_str());
}

// 描述
std::wstring reger::description(int indent/* = 4*/) const 
{
	std::wostringstream os;
	gtc::outClassBegin(os, L"reger", indent);
	gtc::outClassItem(os, L"名称", m_wsName, indent << 1);
	gtc::outClassItem(os, L"地址", m_iAddress, indent << 1);
	gtc::outClassItem(os, L"是否改变", m_isChanged, indent << 1);
	gtc::outClassEnd(os, L"reger", indent);
	return os.str();

}


regerI4::regerI4()
	: reger(), m_iValue(0)
{

}

regerI4::regerI4(int32_t addr, const wchar_t *pname, int32_t value)
	: reger(addr, pname), m_iValue(value)
{
}
regerI4::regerI4(const regerI4 &reg)
	: reger(reg), m_iValue(reg.m_iValue)
{
}
regerI4& regerI4::operator=(const regerI4 &reg) {
	if(this != &reg)
	{
		reger::operator=(reg);
		m_iValue = reg.m_iValue;
	}
	return *this;
}
regerI4::~regerI4(void) {
	DEBUG_LOG(L"free %s", description().c_str());
}
/*****************************************************************************************
	<< --- regerI4::clone	2013-12-31 --- >>
	说明：克隆自己
	参数：
	返回值：克隆对象
*****************************************************************************************/
reger* regerI4::clone() 
{	
	reger* preg = new regerI4(*this);
	return preg;
}
// 描述
std::wstring regerI4::description(int indent /*= 4*/) const 
{
	std::wostringstream os;
	gtc::outClassBegin(os, L"regerI4", indent);
	gtc::outClassItem(os, L"值", m_iValue, indent << 1);
	gtc::outBaseClass(os, *(reger*)this, indent << 1);
	gtc::outClassEnd(os, L"regerI4", indent);
	return os.str();

}

bool regerI4::read(modbus_t *phandle) 
{
	if(!phandle)	return false;
	uint16_t regbuf[5] = {0};
	int rc = modbus_read_registers(phandle, m_iAddress, 2, regbuf);
	if(rc == 2)
	{
		m_iValue = convert::toInt32(regbuf);
		return true;
	}
	return false;
}
/*****************************************************************************************
	<< --- regerI4::write	2013-12-31 --- >>
	说明：将值保存到寄存器
	参数：
	返回值：动作执行是否成功
*****************************************************************************************/
bool regerI4::write(modbus_t *phandle) 
{
	if(!phandle)	return false;
	int rc = modbus_write_registers(phandle, m_iAddress, 2, (uint16_t *)&m_iValue);
	if(rc == 2)
	{
		m_isChanged = false;
		return true;
	}
	return false;
}


regerUI2::regerUI2()
	: reger(), m_usValue(0)
{
}

regerUI2::regerUI2(int32_t addr, const wchar_t *pname, uint16_t value)
	: reger(addr, pname), m_usValue(value)
{
}

regerUI2::regerUI2(const regerUI2 &reg)
	: reger(reg), m_usValue(reg.m_usValue)
{
}

regerUI2& regerUI2::operator=(const regerUI2 &reg) 
{
	if(this != &reg)
	{
		reger::operator=(reg);
		m_usValue = reg.m_usValue;
	}
	return *this;
}
regerUI2::~regerUI2(void) {
	DEBUG_LOG(L"free %s", description().c_str());

}
/*****************************************************************************************
	<< --- regerUI2::clone	2013-12-31 --- >>
	说明：克隆自己
	参数：
	返回值：克隆对象
*****************************************************************************************/
reger* regerUI2::clone() {	
	reger* preg = new regerUI2(*this);
	return preg;
}
// 描述
std::wstring regerUI2::description(int indent /*= 4*/) const {
	std::wostringstream os;
	gtc::outClassBegin(os, L"regerUI2", indent);
	gtc::outClassItem(os, L"值", m_usValue, indent << 1);
	gtc::outBaseClass(os, *(reger*)this, indent << 1);
	gtc::outClassEnd(os, L"regerUI2", indent);
	return os.str();

}

bool regerUI2::read(modbus_t *phandle) 
{
	if(!phandle)	return false;
	uint16_t regbuf[5] = {0};
	int rc = modbus_read_registers(phandle, m_iAddress, 1, regbuf);
	if(rc == 1)
	{
		m_usValue = regbuf[0];
		return true;
	}
	return false;

}
/*****************************************************************************************
	<< --- regerUI2::write	2013-12-31 --- >>
	说明：将值保存到寄存器
	参数：
	返回值：动作执行是否成功
*****************************************************************************************/
bool regerUI2::write(modbus_t *phandle) 
{
	if(!phandle)	return false;
	int rc = modbus_write_register(phandle, m_iAddress, m_usValue);
	if(rc == 2)
	{
		m_isChanged = false;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- device	2013-12-27 --- >>
//	所有设备单元的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
_IMPLEMENT_ITOBJ_(device, itobj, BEDrawShapeTypeDevice, 0x1)


device::device(BEDeviceModel model)
	: itobj()
	, m_beModel(model)
	, m_uiAddress(0)
	, m_bOnline(false)
	, m_iPlace(-1), m_iOptCode(-1), m_beStatus(BEDeviceStatusNone)
{
	setName(getModelName());
}


device::device(const wchar_t *pname, BEDeviceModel model)
	: itobj()
	, m_wsName(pname?pname:L"未定义"), m_beModel(model)
	, m_uiAddress(0)
	, m_bOnline(false)
	, m_iPlace(-1), m_iOptCode(-1), m_beStatus(BEDeviceStatusNone)
{
}

device::device(const device &dev)
	: itobj(dev)
	, m_wsName(dev.m_wsName), m_beModel(dev.m_beModel)
	, m_uiAddress(dev.m_uiAddress)/*, m_vecChannels(dev.m_vecChannels)*/
	, m_bOnline(dev.m_bOnline)
	, m_iPlace(dev.m_iPlace), m_iOptCode(dev.m_iOptCode), m_beStatus(dev.m_beStatus)
{

}

device& device::operator=(const device &dev)
{
	if(this != &dev)
	{
		itobj::operator=(dev);
		m_wsName = dev.m_wsName;
		m_uiAddress = dev.m_uiAddress;
		//m_vecChannels = dev.m_vecChannels;
		m_bOnline = dev.m_bOnline;
		m_beModel = dev.m_beModel;

		m_iPlace = dev.m_iPlace;
		m_beStatus = dev.m_beStatus;
		m_iOptCode = dev.m_iOptCode;

	}
	return *this;
}



device::~device(void)
{
	DEBUG_LOG(L"free %s", description().c_str());

}
// 描述
std::wstring device::description(int indent) const
{
	std::wostringstream os;
	gtc::outClassBegin(os, L"device", indent);
	gtc::outClassItem(os, L"内容", m_wsName, indent << 1);
	gtc::outClassItem(os, L"型号", m_beModel, indent << 1);
	gtc::outClassItem(os, L"是否在线", m_bOnline, indent << 1);
	gtc::outClassItem(os, L"状态", m_beStatus, indent << 1);
	gtc::outClassEnd(os, L"device", indent);
	return os.str();
}
#if defined(_WIN32)
// 
void device::draw(CDC *pdc)
{
}

//void device::setChannels(const IChannel::PtrArrayType &vecChannels)
//{
//	m_vecChannels = vecChannels;
//}

/*****************************************************************************************
	<< ---static device::allocPropertySectionArray	2013-12-06 --- >>
	说明：分配对象属性
	参数：
	返回值：
*****************************************************************************************/
std::size_t device::allocPropertySectionArray(itobj* pitem, bool isShare)
{
	itobjPropSection::PtrArrayType &vecSection = pitem->getPropertys();
	if(vecSection.size() > 0) pitem->freePropertys();
	//vecSection = itobj::allocPropertySectionArray(pitem, isShare);
	device *pdev = dynamic_cast<device *>(pitem);
	itobjPropSection *psec = nullptr, *psubsec = nullptr;
	entry::ArrayType vec;
	if(pdev && (psec = new itobjPropSection(L"设备信息")) != nullptr)
	{
		psec->addProperty(new itobjPropEdit(L"名称", pdev->getName().c_str(), true, L"",		BEDrawShapeTypeDevice, BEDevicePropName));
		psec->addProperty(new itobjPropEdit(L"型号", pdev->getModelName().c_str(), false, L"",	BEDrawShapeTypeDevice, BEDevicePropModelName));

		// 
		vecSection.push_back(psec);
	}

	// 
	return vecSection.size();	
}

/*****************************************************************************************
	<< --- device::updatePropertyValue	2013-12-06 --- >>
	说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
	参数：
	pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
	返回值：更改成功
*****************************************************************************************/
bool device::updatePropertyValue(itobjProp *pprop)
{
	if(pprop && itobj::updatePropertyValue(pprop))
		return true;
	if(!pprop || pprop->getTypekey() != BEDrawShapeTypeDevice)
		return false;
	bool exists = true;
	COleVariant var = pprop->getValue();
	switch (pprop->getItemkey())
	{
	case BEDevicePropName:
		var.ChangeType(VT_BSTR);
		setName(_bstr_t(var.bstrVal));
		break;
	case BEDevicePropModelName:
		break;
	case BEDevicePropAddress:
		var.ChangeType(VT_UI4);
		setAddress(var.ulVal);
		break;
	default:
		exists = false;
		break;
	}
	return exists;
}

/*****************************************************************************************
	<< --- device::messageProperty	2013-12-24 --- >>
	说明：更新指定属性标记的属性值，并将消息WM_ME_ITEM_PROPERTY_CHANGED发送给属性编辑窗口
	参数：
	typeKey		= 对象类型；如：BEDrawShapeType
	vecItemKey	= 需要更新的属性标记
	返回值：
*****************************************************************************************/
void device::messageProperty(int32_t typeKey, std::vector<int32_t> &vecItemKey)
{
	if (typeKey != BEDrawShapeTypeDevice)
	{
		__super::messageProperty(typeKey, vecItemKey);
		return;
	}

	itobjProp::PtrArrayType *pvecProp = new itobjProp::PtrArrayType();
	itobjProp *pitem = nullptr;
	COleVariant var;
	for(std::vector<int32_t>::iterator iter = vecItemKey.begin(), end = vecItemKey.end(); iter != end; ++iter)
	{
		pitem = itobjProp::findPropertyItem(typeKey, *iter);
		if(!pitem)
			break;
		var.Clear();
		bool exists = true;
		switch (BEDeviceProp(*iter))
		{
		case BEDevicePropName:
			var = getName().c_str();
			break;
		case BEDevicePropModelName:
			break;
		case BEDevicePropAddress:
			var.ChangeType(VT_UI4);
			var.ulVal = getAddress();
			break;
		default:
			exists = false;
			break;
		}
		if(exists)
		{
			pitem->setValue(var);
			pvecProp->push_back(pitem);
		}
	}

	if(pvecProp && pvecProp->size() < 1)
		delete pvecProp;
	if(pvecProp && pvecProp->size() > 0)
	{
		app::getMainWnd()->PostMessage(WM_ME_ITEM_PROPERTY_CHANGED, 0L, (LPARAM)pvecProp);
	}

}
#endif

}