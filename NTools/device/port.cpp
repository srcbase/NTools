#include "port.h"
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")
namespace dev {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	serialPort  类    2016-1-12
//	通讯端口基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
serialPort::serialPort()
	: m_nDatabit(8), m_cStopbit(2), m_cParity('N'), m_iBaud(9600), m_iPort(0)
{

}
serialPort::~serialPort()
{

}

bool serialPort::open()
{
	return true;
}
void serialPort::close()
{

}
bool serialPort::isOpened() const
{
	return false;
}
//void* serialPort::getChannelHandle()
//{
//	return NULL;
//}
//std::wstring serialPort::getPortName() const
//{
//	return L"";
//}
//int32_t serialPort::getChannelPort() const
//{
//	return m_iPort;
//}


// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
	0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif
/*static*/ IntSetType serialPort::enumPort(bool bIgnoreBusyPorts /*= true*/)
{
	IntSetType setPort;
	GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;
	wchar_t pbuf[MAX_PATH];
	try
	{
		hDevInfo = SetupDiGetClassDevs( guidDev,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
			);

		if(hDevInfo == INVALID_HANDLE_VALUE) GTC_EXCEPT_DEFAULT(L"SetupDiGetClassDevs 失败", 0);

		// Enumerate the serial ports
		BOOL bOk = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		for (DWORD ii=0; bOk; ii++) {
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				NULL, guidDev, ii, &ifcData);
			if (bOk) {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk) {
					// Got a path to the device. Try to get some more info.
					wchar_t fname[256];
					wchar_t desc[256];
					BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(PBYTE)fname, sizeof(fname), NULL);
					bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
						(PBYTE)desc, sizeof(desc), NULL);

					//DWORD addr = 0;
					//wchar_t pbuf[256];
					//bSuccess = /*bSuccess &&*/ SetupDiGetDeviceRegistryProperty(
					//	hDevInfo, &devdata, SPDRP_BUSNUMBER, NULL,
					//	(PBYTE)&addr, sizeof(addr), NULL);
					//bSuccess = /*bSuccess &&*/ SetupDiGetDeviceRegistryProperty(
					//	hDevInfo, &devdata, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL,
					//	(PBYTE)pbuf, sizeof(pbuf), NULL);

					BOOL bUsbDevice = FALSE;
					wchar_t locinfo[256];
					if (SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
						(PBYTE)locinfo, sizeof(locinfo), NULL))
					{
						// Just check the first three characters to determine
						// if the port is connected to the USB bus. This isn't
						// an infallible method; it would be better to use the
						// BUS GUID. Currently, Windows doesn't let you query
						// that though (SPDRP_BUSTYPEGUID seems to exist in
						// documentation only).

						SEND_LOG(L"serialPort::enumPort locinfo:<%s>", locinfo);
						bUsbDevice = gtc::stringUtil::startsWith(locinfo, L"usb");// (strncmp(locinfo, "USB", 3)==0);
					}
					if (bSuccess) {
						// Add an entry to the array
						//std::wstring wstr = pDetData->DevicePath;
						//wstr = fname;
						//wstr = desc;
						//si.strFriendlyName = fname;
						//si.strPortDesc = desc;
						//si.bUsbDevice = bUsbDevice;
						//asi.Add(si);
						SEND_LOG(L"serialPort::enumPort friendly:<%s> port:<%s>", fname, desc);

						// 解析串口号
						std::wstring wstr = fname;
						std::wstring::size_type pos = wstr.rfind(L"(COM");
						if (pos != std::wstring::npos)
						{
							pos += 4;
							std::wstring::size_type posEnd = wstr.find(L')', pos);
							if (posEnd != std::wstring::npos)
							{
								wstr = wstr.substr(pos, posEnd - pos);
								setPort.insert((int)std::wcstol(wstr.c_str(), nullptr, 10) - 1);	// 串口基于0
							}
						}

					}
				}
				else
				{
					GTC_EXCEPT_DEFAULT(L"SetupDiGetDeviceInterfaceDetail failed", GetLastError());
				}
			}
			else {
				GTC_EXCEPT_DEFAULT(L"SetupDiEnumDeviceInterfaces failed", GetLastError());
			}
		}

	}
	catch(gtc::baseException &e)
	{
		SEND_LOG(L"ERROR serialPort::enumPort error:<%s>", e.description().c_str());
	}
	// 
	if (pDetData != NULL)
		delete [] (char*)pDetData;
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	return setPort;
}



}	// dev