#include "monitor.h"
namespace acm{

int monitor::mc_iMaxCount = monitor::monitorCount();	// 屏幕数量
monitor::monitor(void)
	: m_iMonitor(0)
	, m_dwStateFlag(0)
{
	//m_iMaxCount = monitorCount();// ::GetSystemMetrics(SM_CMONITORS);
	memset(&m_rcWorkArea, 0, sizeof(m_rcWorkArea));
}


monitor::~monitor(void)
{
}

int monitor::getNumber() const
{
	return m_iMonitor;
}
void monitor::setNumber(int devNum)
{
	assert(devNum >= 0 && devNum < mc_iMaxCount);
	m_iMonitor = devNum;
	// 
	DISPLAY_DEVICE devInfo = {0};
	devInfo.cb = sizeof(DISPLAY_DEVICE);
	if(EnumDisplayDevices(NULL, m_iMonitor, &devInfo, 0))
	{
		m_wsDevName = devInfo.DeviceName;
		m_wsDevDesc = devInfo.DeviceString;
		m_dwStateFlag = devInfo.StateFlags;
		// 
		DEVMODE dm = {0};
		dm.dmSize = sizeof(DEVMODE);
		if (EnumDisplaySettings(m_wsDevName.c_str(), ENUM_CURRENT_SETTINGS, &dm))
		{
			m_rcWorkArea.left = dm.dmPosition.x;
			m_rcWorkArea.top = dm.dmPosition.y;
			m_rcWorkArea.right = m_rcWorkArea.left + dm.dmPelsWidth;
			m_rcWorkArea.bottom = m_rcWorkArea.top + dm.dmPelsHeight;
		}
	}
}

bool monitor::isPrimaryScreen() const
{
	assert(m_iMonitor >= 0 && m_iMonitor < mc_iMaxCount);
	return (m_dwStateFlag & DISPLAY_DEVICE_PRIMARY_DEVICE) == DISPLAY_DEVICE_PRIMARY_DEVICE;
}
std::wstring monitor::getDeviceName() const
{
	assert(m_iMonitor >= 0 && m_iMonitor < mc_iMaxCount);
	return m_wsDevName;
}
std::wstring monitor::getDeviceDescription() const
{
	assert(m_iMonitor >= 0 && m_iMonitor < mc_iMaxCount);
	return m_wsDevDesc;
}
RECT monitor::getWorkArea() const
{
	assert(m_iMonitor >= 0 && m_iMonitor < mc_iMaxCount);
	return m_rcWorkArea;
}
SIZE monitor::getWorkAreaSize() const
{
	assert(m_iMonitor >= 0 && m_iMonitor < mc_iMaxCount);
	SIZE sz = {m_rcWorkArea.right-m_rcWorkArea.left, m_rcWorkArea.bottom-m_rcWorkArea.top};
	return sz;
}
std::wstring monitor::toString() const
{
	std::wostringstream out;
	out << L"监视器总数：" << mc_iMaxCount << L"  当前屏幕序号：" << m_iMonitor << L"  主屏：" << (isPrimaryScreen()?L"是":L"否")
		<< L"\t工作区域：" << gtc::format::rect(m_rcWorkArea, true)
		<< L"\t设备名称：" << m_wsDevName
		<< L"\t设备描述：" << m_wsDevDesc
		<< std::endl;
	return out.str();
}

int monitor::monitorCount()
{
	mc_iMaxCount = ::GetSystemMetrics(SM_CMONITORS);
	return mc_iMaxCount;
}
}