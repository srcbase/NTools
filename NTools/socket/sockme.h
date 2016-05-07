#ifndef __SOCKME_H__
#define __SOCKME_H__

#pragma once
#include <iostream>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <ctime>
#include <iostream>
#include <cstdint>
#include <list>

#include <afxsock.h> 
//#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#include "ac/sysupport.h"
#include "gt/gtdcsdata.h"
namespace gtc {
#define SOCKET_RECVBUFF_LEN			1024		// 接收缓存区大小
#define SOCKET_SENDBUFF_LEN			1024		// 发送缓存区大小
#define SOCKET_WINEPOCH				2208988800	// 是因特网纪元与windows纪元之间的差值
#define SOCKET_STKSIZE				16536
#define SOCKET_HEARTBEAT_HEADER		"HB:"		// 心跳包的回复头
#define SOCKET_HEARTBEAT_INTERVAL	3000		// 心跳包发送间隔（毫秒）
#define SOCKET_CONNECTION_RETRYS	10			// 重链次数
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- sockme	2014-04-08 --- >>
//	基于套接字的操作
////////////////////////////////////////////////////////////////////////////////////////////////////////
class socket
{
public:
	// 传输协议
	typedef enum BESocketType {
		BESocketTypeStream	= SOCK_STREAM,		// TCP
		BESocketTypeDgram	= SOCK_DGRAM		// UDP
	};
	/* 记录socket信息 */
	typedef struct BSSocketInfo{
		SOCKET		scObject;		// 
		SCDataType	dataRecv;		// 用来存放接收的数据
		SCDataType	dataSend;		// 用来存放发送的数据
		std::time_t	tmLastSend;		// 最近一次发送数据的时间
		std::time_t tmLastRecv;		// 最近一次接收数据的时间
		// 
		BSSocketInfo()
			: scObject(INVALID_SOCKET)
			//, dataRecv(512,0), dataSend(512, 0)
			, tmLastSend(0), tmLastRecv(0)
		{

		}
		BSSocketInfo(SOCKET sct)
			: scObject(sct)
			//, dataRecv(512,0), dataSend(512, 0)
			, tmLastSend(0), tmLastRecv(0)
		{
		}
	}BSSocketInfo;
public:
	socket(void);
	virtual ~socket(void);
public:
	// static 
	/*****************************************************************************************
		<< --- static socket::init	2014-04-08 --- >>
		说明：初始化socket调用的环境；进程中只需执行一次。
		参数：
		major	= 套接字的主板本号
		minor	= 套接字的次板本号
		返回值：是否成功
	*****************************************************************************************/
	static bool init(uint8_t major = 2, uint8_t minor = 2);
	/*****************************************************************************************
		<< --- static socket::clean	2014-04-08 --- >>
		说明：释放socket调用的环境；进程中只需执行一次。
		参数：
		返回值：
	*****************************************************************************************/
	static void clean();
	/*****************************************************************************************
		<< --- socket::receive	2014-04-09 --- >>
		说明：接收数据
		参数：
		sc		= 套接字
		vecData	= 存放被接收的数据
		返回值：是否执行成功
	*****************************************************************************************/
	static bool receive(SOCKET sc, SCDataType &vecData);
	static bool receive(BSSocketInfo *pscinfo);
	/*****************************************************************************************
		<< --- socket::send	2014-04-10 --- >>
		说明：发送数据
		参数：
		sc		= 套接字
		vecData	= 被发送的数据
		返回值：是否执行成功
	*****************************************************************************************/
	static bool send(SOCKET sc, const SCDataType &vecData);

	/*****************************************************************************************
		<< --- static socket::threadECHO	2014-04-10 --- >>
		说明：用来与客户端进行沟通的线程。
		参数：
		parg	= 与客户端连接的套接字
		返回值：0=成功
	*****************************************************************************************/
	static unsigned __stdcall threadECHO(void *parg);

	/*****************************************************************************************
		<< --- static socket::threadHeartBeat	2014-05-04 --- >>
		说明：用来与服务端进行沟通的线程。
			以固定时间SOCKET_HEARTBEAT_INTERVAL往服务端发送心跳包。
		参数：
		parg	= 与服务端连接的套接字
		返回值：0=成功
	*****************************************************************************************/
	static unsigned __stdcall threadHeartBeat(void *parg);
public:
	/*****************************************************************************************
		<< --- socket::connect	2014-04-08 --- >>
		说明：打开链接套接字，一般被客户端调用
		参数：
		phost		= 被链接的设备主机名称或IP地址，如：www.163.com 或192.168.1.55
		pservice	= 链接采用的服务操作或服务端口号，如：daytime 或3328
		ptransport	= 传输协议，如：tcp或udp
		返回值：是否链接成功
	*****************************************************************************************/
	bool connect(const char *phost, const char *pservice, const char* transport);
	bool connect();
	/*****************************************************************************************
		<< --- socket::daytime	2014-04-10 --- >>
		说明：尝试关闭connect打开的套接字
		参数：
		返回值：是否执行成功
	*****************************************************************************************/
	bool close() {
		bool bret = closesocket(m_scMain);
		if(!bret)
			errorInfo("closesocket失败！");
		return bret;
	}

	/*****************************************************************************************
		<< --- socket::daytime	2014-04-10 --- >>
		说明：从daytime服务取得时间
		参数：
		now		= 保存取得的服务器时间
		返回值：是否执行成功
	*****************************************************************************************/
	bool daytime(std::time_t &now);
	/*****************************************************************************************
		<< --- socket::passive	2014-04-09 --- >>
		说明：创建被动套接字，一般被服务端调用
		参数：
		pservice	= 链接采用的服务操作或服务端口号，如：daytime 或3328
		ptransport	= 传输协议，如：tcp或udp
		queueLen	= 所希望的连接请求队列的长度，采用UDP时该参数无效
		返回值：是否执行成功
	*****************************************************************************************/
	bool passive(const char *pservice, const char* ptransport, int32_t queueLen);
	/*****************************************************************************************
		<< --- socket::serviceDaytimeUDP	2014-04-10 --- >>
		说明：创建基于daytime服务的被动套接字；采用循环方式。
		参数：
		返回值：是否执行成功
	*****************************************************************************************/
	bool serviceDaytimeUDP();
	/*****************************************************************************************
		<< --- socket::serviceECHOTCP	2014-04-10 --- >>
		说明：创建基于echo服务的被动套接字；采用并发方式。
		参数：
		返回值：是否执行成功
	*****************************************************************************************/
	bool serviceECHOTCP();

	/*****************************************************************************************
		<< --- socket::serviceHeartBeat	2014-05-04 --- >>
		说明：创建基于port端口的心跳包检测服务；采用并发方式。
			对接收的数据加上一个头标记进行回发。
		参数：
		port = 端口号
		返回值：是否执行成功
	*****************************************************************************************/
	bool serviceHeartBeat(uint16_t port);

	/*****************************************************************************************
		<< --- socket::receive	2014-04-09 --- >>
		说明：接收数据
		参数：
		vecData	= 存放被接收的数据
		返回值：是否执行成功
	*****************************************************************************************/
	bool receive(SCDataType &vecData);
	/*****************************************************************************************
		<< --- socket::send	2014-04-10 --- >>
		说明：发送数据
		参数：
		vecData	= 被发送的数据
		返回值：是否执行成功
	*****************************************************************************************/
	bool send(const SCDataType &vecData);
	/*****************************************************************************************
		<< --- socket::clientEcho	2014-04-09 --- >>
		说明：执行echo服务的客户端
		参数：
		sendData	= 被发送的数据
		recvData	= 接收的数据
		返回值：是否执行成功
	*****************************************************************************************/
	bool clientEcho(const std::string &sendData, std::string &recvData);

	/*****************************************************************************************
		<< --- socket::clientHeartBeat	2014-04-09 --- >>
		说明：执行心跳包服务的客户端
		内部启动一个新线程threadHeartBeat，以固定间隔发送心跳包到服务端
		参数：
		phostip	= 目标主机名或IP
		port	= 通讯端口号
		返回值：是否执行成功
		*****************************************************************************************/
	bool clientHeartBeat(const char *phostip, const uint16_t port);
private:
	// 支持函数
	void errorInfo(const char *pformat, ...)
	{
		const int32_t len = 2048;
		char pinfo[len] = {0};
		va_list argList;
		va_start(argList, pformat);
		std::vsprintf(pinfo, pformat, argList);
		va_end(argList);
		m_csErrinfo = pinfo;
		// 
		int err = WSAGetLastError();
		if(err != 0)
		{
			std::sprintf(pinfo, "SEC：%d", err);
			m_csErrinfo += pinfo;
		}
#if defined(_DEBUG)
		std::cout << m_csErrinfo.c_str() << std::endl;
#endif
	}

public:
	// 属性
	std::string	getErrorInfo() const {				return m_csErrinfo;						}
protected:
	// 当前sockets信息
	static WSADATA		mc_wsData;
	// 基础端口号
	static uint16_t		mc_unBaseport;
protected:
	std::string		m_csHost;		// 主机名称或IP
	std::string		m_csService;	// 服务名称或端口号 echo 或6780
	std::string		m_csTransport;	// 传输协议tcp/udp
	// 主套接字（客户端的链接socket或服务端的等待socket）
	SOCKET			m_scMain;
	// 传输协议类型（数据流或数据报）
	BESocketType	m_beType;
	// 错误信息
	std::string		m_csErrinfo;
	// 异步IO select;大小不能超过FD_SETSIZE
	std::list<BSSocketInfo *>	m_lstSocket;
};





}
#endif