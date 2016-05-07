#ifndef __NETWRAP_H__
#define __NETWRAP_H__

#define   _WINSOCKAPI_   //   禁止windows.h包含winsock.h

#include "AC/basedef.h"
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define NET_DEF_PACKET_SIZE			32
#define NET_ECHO_REQUEST			8
#define NET_ECHO_REPLY				0

typedef struct {
	BYTE		byVerHLen;			//4位版本+4位首部长度
	BYTE		byTOS;				//服务类型
	USHORT		usTotalLen;			//总长度
	USHORT		usID;				//标识
	USHORT		usFlagFragOffset;	//3位标志+13位片偏移
	BYTE		byTTL;				//TTL
	BYTE		byProtocol;			//协议
	USHORT		usHChecksum;		//首部检验和
	ULONG		ulSrcIP;			//源IP地址
	ULONG		ulDestIP;			//目的IP地址
}BSIPHeader;

typedef struct { 
	BYTE		byType;				//类型
	BYTE		byCode;				//代码
	USHORT		usChecksum;			//检验和 
	USHORT		usID;				//标识符
	USHORT		usSeq;				//序号
	ULONG		ulTimeStamp;		//时间戳（非标准ICMP头部）
}BSICMPHeader;
typedef struct 
{
	USHORT		usSeq;
	DWORD		dwRoundTripTime;
	DWORD		dwBytes;
	DWORD		dwTTL;
}BSPingReply;
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	pingCmd  类		2015-10-22
//	封装ping命令
////////////////////////////////////////////////////////////////////////////////////////////////////////
class pingCmd 
{
public:
	pingCmd();
	~pingCmd();

	bool ping(DWORD dwDestIP, BSPingReply *pPingReply = NULL, DWORD dwTimeout = 2000);
	bool ping(char *szDestIP, BSPingReply *pPingReply = NULL, DWORD dwTimeout = 2000);
private:
	bool pingCore(DWORD dwDestIP, BSPingReply *pPingReply, DWORD dwTimeout);
	USHORT calCheckSum(USHORT *pBuffer, int nSize);
	ULONG getTickCountCalibrate();
private:
	SOCKET		m_sockRaw; 
	WSAEVENT	m_event;
	USHORT		m_usCurrentProcID;
	char		*m_szICMPData;
	bool		m_bIsInitSucc;
private:
	static USHORT mc_usPacketSeq;
};

}	// gtc
#endif	// __NETWRAP_H__