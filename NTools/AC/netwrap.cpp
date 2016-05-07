#include "netwrap.h"


namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	pingCmd  类		2015-10-22
//	封装ping命令
////////////////////////////////////////////////////////////////////////////////////////////////////////
USHORT pingCmd::mc_usPacketSeq = 0;

pingCmd::pingCmd()
	: m_sockRaw(NULL), m_event(NULL)
	, m_usCurrentProcID(0), m_szICMPData(NULL)
	, m_bIsInitSucc(false)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	
	m_event = WSACreateEvent();
	m_usCurrentProcID = (USHORT)GetCurrentProcessId();
	if ((m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0)) != SOCKET_ERROR)
	{
		WSAEventSelect(m_sockRaw, m_event, FD_READ);
		m_bIsInitSucc = true;
		
		m_szICMPData = (char*)malloc(NET_DEF_PACKET_SIZE + sizeof(BSICMPHeader));
		
		if (m_szICMPData == NULL)
		{
			m_bIsInitSucc = false;
		}
	}

}
pingCmd::~pingCmd()
{
	WSACleanup();
	if (NULL != m_szICMPData)
	{
		free(m_szICMPData);
		m_szICMPData = NULL;
	}
}

bool pingCmd::ping(DWORD dwDestIP, BSPingReply *pPingReply /*= NULL*/, DWORD dwTimeout /*= 2000*/)
{
	return pingCore(dwDestIP, pPingReply, dwTimeout);
}
bool pingCmd::ping(char *szDestIP, BSPingReply *pPingReply /*= NULL*/, DWORD dwTimeout /*= 2000*/)
{
	assert(szDestIP);
	if (NULL != szDestIP)
	{
		return pingCore(inet_addr(szDestIP), pPingReply, dwTimeout);
	}
	return false;   
}
bool pingCmd::pingCore(DWORD dwDestIP, BSPingReply *pPingReply, DWORD dwTimeout)
{
	//判断初始化是否成功
	assert(m_bIsInitSucc);
	if (!m_bIsInitSucc)
	{
		return false;
	}
	//配置SOCKET
	sockaddr_in sockaddrDest;
	sockaddrDest.sin_family = AF_INET;
	sockaddrDest.sin_addr.s_addr = dwDestIP;
	int nSockaddrDestSize = sizeof(sockaddrDest);
	
	//构建ICMP包
	int nICMPDataSize = NET_DEF_PACKET_SIZE + sizeof(BSICMPHeader);
	ULONG ulSendTimestamp = getTickCountCalibrate();
	USHORT usSeq = ++mc_usPacketSeq;
	memset(m_szICMPData, 0, nICMPDataSize);
	BSICMPHeader *pICMPHeader = (BSICMPHeader*)m_szICMPData;
	pICMPHeader->byType = NET_ECHO_REQUEST;
	pICMPHeader->byCode = 0;
	pICMPHeader->usID = m_usCurrentProcID;
	pICMPHeader->usSeq = usSeq;
	pICMPHeader->ulTimeStamp = ulSendTimestamp;
	pICMPHeader->usChecksum = calCheckSum((USHORT*)m_szICMPData, nICMPDataSize);
	
	//发送ICMP报文
	if (sendto(m_sockRaw, m_szICMPData, nICMPDataSize, 0, (struct sockaddr*)&sockaddrDest, nSockaddrDestSize) == SOCKET_ERROR)
	{
		return false;
	}
	//判断是否需要接收相应报文
	if (pPingReply == NULL)
	{
		return true;
	}
	
	char recvbuf[256] = {"\0"};
	while (TRUE)
	{
		//接收响应报文
		if (WSAWaitForMultipleEvents(1, &m_event, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
		{
			WSANETWORKEVENTS netEvent;
			WSAEnumNetworkEvents(m_sockRaw, m_event, &netEvent);
			
			if (netEvent.lNetworkEvents & FD_READ)
			{
				ULONG nRecvTimestamp = getTickCountCalibrate();
				int nPacketSize = recvfrom(m_sockRaw, recvbuf, 256, 0, (struct sockaddr*)&sockaddrDest, &nSockaddrDestSize);
				if (nPacketSize != SOCKET_ERROR)
				{
					BSIPHeader *pIPHeader = (BSIPHeader*)recvbuf;
					USHORT usIPHeaderLen = (USHORT)((pIPHeader->byVerHLen & 0x0f) * 4);
					BSICMPHeader *pICMPHeader = (BSICMPHeader*)(recvbuf + usIPHeaderLen);
					
					if (pICMPHeader->usID == m_usCurrentProcID	//是当前进程发出的报文
						&& pICMPHeader->byType == NET_ECHO_REPLY		//是ICMP响应报文
						&& pICMPHeader->usSeq == usSeq			//是本次请求报文的响应报文
						)
					{
						pPingReply->usSeq = usSeq;
						pPingReply->dwRoundTripTime = nRecvTimestamp - pICMPHeader->ulTimeStamp;
						pPingReply->dwBytes = nPacketSize - usIPHeaderLen - sizeof(BSICMPHeader);
						pPingReply->dwTTL = pIPHeader->byTTL;
						return true;
					}
				}
			}
		}
		//超时
		if (getTickCountCalibrate() - ulSendTimestamp >= dwTimeout)
		{
			return false;
		}
	}

	return true;
}
USHORT pingCmd::calCheckSum(USHORT *pBuffer, int nSize)
{
	unsigned long ulCheckSum=0;
	while(nSize > 1)
	{
		ulCheckSum += *pBuffer++;
		nSize -= sizeof(USHORT);
	}
	if(nSize )
	{
		ulCheckSum += *(UCHAR*)pBuffer;
	}
	
	ulCheckSum = (ulCheckSum >> 16) + (ulCheckSum & 0xffff);
	ulCheckSum += (ulCheckSum >>16);
	
	return (USHORT)(~ulCheckSum);
}
ULONG pingCmd::getTickCountCalibrate()
{
	static ULONG s_ulFirstCallTick = 0;
	static LONGLONG s_ullFirstCallTickMS = 0;
	
	SYSTEMTIME systemtime;
	FILETIME filetime;
	GetLocalTime(&systemtime);
	SystemTimeToFileTime(&systemtime, &filetime);
	LARGE_INTEGER liCurrentTime;
	liCurrentTime.HighPart = filetime.dwHighDateTime;
	liCurrentTime.LowPart = filetime.dwLowDateTime;
	LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000;
	
	if (s_ulFirstCallTick == 0)
	{
		s_ulFirstCallTick = GetTickCount();
	}
	if (s_ullFirstCallTickMS == 0)
	{
		s_ullFirstCallTickMS = llCurrentTimeMS;
	}
	
	return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}

}	// gtc