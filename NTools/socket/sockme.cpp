#include "sockme.h"
#include <cstdio>
#include <assert.h>
#include <process.h>
namespace gtc {
// 当前sockets信息
WSADATA socket::mc_wsData = {0};
// 基础端口号
uint16_t socket::mc_unBaseport = 0;

socket::socket(void)
	: m_scMain(INVALID_SOCKET)
	, m_beType(BESocketTypeStream)
{
}


socket::~socket(void)
{
}

/*****************************************************************************************
	<< --- static sockme::init	2014-04-08 --- >>
	说明：初始化socket调用的环境；进程中只需执行一次。
	参数：
	major	= 套接字的主板本号
	minor	= 套接字的次板本号
	返回值：是否成功
*****************************************************************************************/
bool socket::init(uint8_t major, uint8_t minor)
{
	return WSAStartup(MAKEWORD(major, minor), &mc_wsData)==0;
}

/*****************************************************************************************
	<< --- static sockme::clean	2014-04-08 --- >>
	说明：释放socket调用的环境；进程中只需执行一次。
	参数：
	返回值：
*****************************************************************************************/
void socket::clean()
{
	WSACleanup();
}
/*****************************************************************************************
	<< --- static socket::threadECHO	2014-04-10 --- >>
	说明：用来与客户端进行沟通的线程。
	参数：
	parg	= 与客户端连接的套接字
	返回值：0=成功
*****************************************************************************************/
unsigned __stdcall socket::threadECHO(void *parg)
{
	const int len = 4096;
	char buf[len];
	SOCKET scSub = *(SOCKET*)parg;
	int rc = recv(scSub, buf, len, 0);
	while (rc != SOCKET_ERROR && rc > 0)
	{
		buf[rc] = '\0';
		if(::send(scSub, buf, rc, 0) == SOCKET_ERROR)
		{
			//errorInfo("echo服务发送数据失败！");
#ifdef _DEBUG
			std::cout << "echo服务发送数据失败！" << std::endl;
#endif
			return -1;
		}
		std::cout << int(scSub) << ":" << buf << std::endl;
		rc = recv(scSub, buf, len, 0);
	}
	if(rc == SOCKET_ERROR)
		return -2;
#ifdef _DEBUG
	std::cout << scSub << "：结束服务！" << std::endl;
#endif
	closesocket(scSub);
	return 0;
}

/*****************************************************************************************
	<< --- static socket::threadHeartBeat	2014-05-04 --- >>
	说明：用来与服务端进行沟通的线程。
		以固定时间SOCKET_HEARTBEAT_INTERVAL往服务端发送心跳包。
	参数：
	parg	= 与服务端连接的套接字
	返回值：0=成功
*****************************************************************************************/
unsigned __stdcall socket::threadHeartBeat(void *parg)
{
	char buf[] = "heart beat";
	SCDataType data(std::begin(buf), std::end(buf));
	SOCKET scSub = *(SOCKET*)parg;
	while(true)
	{
#if defined(_DEBUG)
		std::cout << "start send heart beat..." << std::endl;
#endif
		if(!send(scSub, data))
		{
#if defined(_DEBUG)
			char buf[40] = {0};
			std::sprintf(buf, "发送心跳包失败！%d", ::WSAGetLastError());
			std::cout << buf << std::endl;
#endif
			closesocket(scSub);
			break;
		}
		Sleep(SOCKET_HEARTBEAT_INTERVAL);
	}
	return 0;
}
/*****************************************************************************************
	<< --- socket::connect	2014-04-08 --- >>
	说明：打开链接套接字，一般被客户端调用
	参数：
	phost		= 被链接的设备主机名称或IP地址，如：www.163.com 或192.168.1.55
	pservice	= 链接采用的服务操作或服务端口号，如：daytime 或3328
	transport	= 传输协议，如：tcp或udp
	返回值：是否链接成功
*****************************************************************************************/
bool socket::connect(const char *phost, const char *pservice, const char* ptransport)
{
	m_csHost = phost?phost:"";
	m_csService = pservice? pservice: "";
	m_csTransport = ptransport ? ptransport : "";
	return connect();
}
bool socket::connect()
{
	sockaddr_in scaddr = {0};
	scaddr.sin_family = AF_INET;
	// 服务端口号
	if(servent *pse = getservbyname(m_csService.c_str(), m_csTransport.c_str()))
		scaddr.sin_port = pse->s_port;
	else if((scaddr.sin_port = htons((u_short)std::atoi(m_csService.c_str()))) == 0)
	{
		errorInfo("无法得到服务“%s”的入口点！", m_csService.c_str());
		return false;
	}
	// 主机IP地址
	if(hostent *phe = gethostbyname(m_csHost.c_str()))
		memcpy_s(&scaddr.sin_addr, sizeof(scaddr.sin_addr), phe->h_addr, phe->h_length);
	else if((scaddr.sin_addr.s_addr = inet_addr(m_csHost.c_str())) == INADDR_ANY)
	{
		errorInfo("无法识别的主机地址“%s”！", m_csHost.c_str());
		return false;
	}
	// 传输协议
	protoent *ppe = getprotobyname(m_csTransport.c_str());
	if(!ppe)
	{
		errorInfo("无法得到与协议“%s”相关的信息！", m_csTransport.c_str());
		return false;
	}
	if(_stricmp(m_csTransport.c_str(), "udp") == 0)
		m_beType = BESocketTypeDgram;
	else
		m_beType = BESocketTypeStream;
	// create socket
	m_scMain = ::socket(PF_INET, int(m_beType), ppe->p_proto);
	if(m_scMain == INVALID_SOCKET)
	{
		errorInfo("无法创建套接字！");
		return false;
	}
	// connect socket
	if(::connect(m_scMain, (sockaddr *)&scaddr, sizeof(scaddr)) == SOCKET_ERROR)
	{
		closesocket(m_scMain);
		m_scMain = INVALID_SOCKET;
		errorInfo("无法打开到%s.%s的链接！", m_csHost.c_str(), m_csService.c_str());
		return false;
	}
	//#ifdef _DEBUG
	std::cout << "链接到服务:" << m_csHost.c_str() << ":" << m_csService.c_str() << "(" << scaddr.sin_port << ") " << m_csTransport.c_str() << std::endl;
	//#endif

	return true;
}
/*****************************************************************************************
	<< --- socket::daytime	2014-04-10 --- >>
	说明：从daytime服务取得时间
	参数：
	now		= 保存取得的服务器时间
	返回值：是否执行成功
*****************************************************************************************/
 bool socket::daytime(std::time_t &now)
{
	memset(&now, 0, sizeof(now));
	std::string msg("what time is it?");
	::send(m_scMain, msg.data(), msg.length(), 0);
	// 
	SCDataType data;
	if(receive(data) && data.size() >= sizeof(now))
	{
		memcpy_s(&now, sizeof(now), data.data(), sizeof(now));
		now = ntohl((u_long)now);
		now -= SOCKET_WINEPOCH;
	}
	else
	{
		errorInfo("接收daytime服务数据失败！");
		return false;
	}
	return true;

}


/*****************************************************************************************
	<< --- socket::passive	2014-04-09 --- >>
	说明：创建被动套接字，一般被服务端调用
	参数：
	pservice	= 链接采用的服务操作或服务端口号，如：daytime 或3328
	ptransport	= 传输协议，如：tcp或udp
	queueLen	= 所希望的连接请求队列的长度，采用UDP时该参数无效
	返回值：是否链接成功
*****************************************************************************************/
bool socket::passive(const char *pservice, const char* ptransport, int32_t queueLen)
{
	sockaddr_in scaddr = {0};
	scaddr.sin_family = AF_INET;
	scaddr.sin_addr.s_addr = INADDR_ANY;
	// 服务名或端口号
	if(servent *pse = getservbyname(pservice, ptransport))
		scaddr.sin_port = htons(ntohs(pse->s_port)+mc_unBaseport);
	else if((scaddr.sin_port = htons((u_short)atoi(pservice))) == 0)
	{
		errorInfo("无法得到服务“%s”的入口点！", pservice);
		return false;
	}
	// 传输协议
	protoent *ppe = getprotobyname(ptransport);
	if(!ppe)
	{
		errorInfo("无法得到与协议“%s”相关的信息！", ptransport);
		return false;
	}
	if(_stricmp(ptransport, "udp") == 0)
		m_beType = BESocketTypeDgram;
	else
		m_beType = BESocketTypeStream;
	// create socket
	m_scMain = ::socket(PF_INET, int(m_beType), ppe->p_proto);
	if(m_scMain == INVALID_SOCKET)
	{
		errorInfo("无法创建套接字！");
		return false;
	}
	// bind socket
	if(bind(m_scMain, (sockaddr *)&scaddr, sizeof(scaddr)) == SOCKET_ERROR)
	{
		errorInfo("无法邦定到“%s”端口上！", pservice);
		return false;
	}
	if(m_beType == BESocketTypeStream && listen(m_scMain, queueLen) == SOCKET_ERROR)
	{
		errorInfo("无法启动对“%s”端口的侦听！", pservice);
		return false;
	}
#ifdef _DEBUG
	std::cout << "已启动服务:" << pservice << "(" << scaddr.sin_port << ") " << ptransport << std::endl;
#endif
	return true;
}

/*****************************************************************************************
	<< --- socket::serviceDaytimeUDP	2014-04-10 --- >>
	说明：创建基于daytime服务的被动套接字
	参数：
	返回值：是否执行成功
*****************************************************************************************/
bool socket::serviceDaytimeUDP()
{
	const int len = 2048;
	char buf[len] = {0};
	sockaddr_in clientAddr = {0};
	int clientAddrLen = sizeof(clientAddr);
	std::time_t now = {0};
	if(passive("daytime", "udp", 0))
	{
		while(true)
		{
			if(recvfrom(m_scMain, buf, len, 0, (sockaddr *)&clientAddr, &clientAddrLen) == SOCKET_ERROR)
			{
				errorInfo("执行daytime服务的接收命令失败！");
				return false;
			}
#ifdef _DEBUG
			std::cout << "接收到" << inet_ntoa(clientAddr.sin_addr) << " 的请求 ..." << std::endl;
#endif
			std::time(&now);
			now = htonl((u_long)(now + SOCKET_WINEPOCH));
			sendto(m_scMain, (char *)&now, sizeof(now), 0, (sockaddr *)&clientAddr, clientAddrLen);
		}

	}
	else
	{
		errorInfo("构建daytime服务失败！");
		return false;
	}
	return true;
}

/*****************************************************************************************
	<< --- socket::serviceECHOTCP	2014-04-10 --- >>
	说明：创建基于echo服务的被动套接字；采用并发方式。
	参数：
	返回值：是否执行成功
*****************************************************************************************/
bool socket::serviceECHOTCP()
{
	const int len = 2048;
	char buf[len] = {0};
	sockaddr_in clientAddr = {0};
	int clientAddrLen = sizeof(clientAddr);
	SOCKET scClient;
	if(passive("echo", "tcp", 10))
	{
		while(true)
		{
			if((scClient = accept(m_scMain, (sockaddr *)&clientAddr, &clientAddrLen)) == INVALID_SOCKET)
			{
				errorInfo("执行accept命令失败！");
				return false;
			}
#ifdef _DEBUG
			std::cout << "接收到" << inet_ntoa(clientAddr.sin_addr) << " 的请求 ..." << std::endl;
#endif
			if(_beginthread((void (*)(void*))threadECHO, SOCKET_STKSIZE, (void*)scClient) < 0)
			{
				errorInfo("启动echo服务子线程失败！%d", strerror(errno));
			}
		}

	}
	else
	{
		errorInfo("构建daytime服务失败！");
		return false;
	}
	return true;
}

/*****************************************************************************************
	说明：创建基于port端口的心跳包检测服务；采用并发方式。
		对接收的数据加上一个头标记进行回发。
	参数：
	port = 端口号
	返回值：是否执行成功
*****************************************************************************************/
bool socket::serviceHeartBeat(uint16_t port)
{
	std::string hbHead = "HB";

	const int len = 2048;
	char buf[len] = {0};
	sockaddr_in clientAddr = {0};
	int clientAddrLen = sizeof(clientAddr);
	SOCKET scaccept;
	char pport[10] = {0};
	std::sprintf(pport, "%u", port);
	u_long nonblock = 1;
	FD_SET fdread;
	timeval tv = {2, 0};	// 2秒
	std::list<BSSocketInfo *>::iterator iter;
	if(passive(pport, "tcp", 10))
	{
		if(ioctlsocket(m_scMain, FIONBIO, &nonblock) == SOCKET_ERROR)
		{
			errorInfo("执行ioctlsocket命令失败！");
			return false;
		}
		while(true)
		{
			FD_ZERO(&fdread);
			FD_SET(m_scMain, &fdread);
			// 将以建立的链接加入
			for (iter = m_lstSocket.begin(); iter != m_lstSocket.end(); ++iter)
			{
				FD_SET((*iter)->scObject, &fdread);
			}
			int rt = select(0, &fdread, nullptr, nullptr, &tv);
			if (rt == 0)
			{
				continue;	// 超时继续监听
			}
			else if(rt == SOCKET_ERROR)
			{
				errorInfo("监听失败！");
				break;
			}
			// 是否是主链接有请求，
			if (FD_ISSET(m_scMain, &fdread))
			{
				if (m_lstSocket.size() >= FD_SETSIZE)
				{
					errorInfo("超出服务的最大链接数%d", FD_SETSIZE);
					continue;
				}
				if((scaccept = accept(m_scMain, (sockaddr *)&clientAddr, &clientAddrLen)) != INVALID_SOCKET)
				{
					if(ioctlsocket(scaccept, FIONBIO, &nonblock) == SOCKET_ERROR)
					{
						errorInfo("执行ioctlsocket命令失败2！");
						return false;
					}
					m_lstSocket.push_front(new BSSocketInfo(scaccept));
				}			
			}
			// 处理请求
			for(iter = m_lstSocket.begin(); iter != m_lstSocket.end(); ++iter)
			{
				if (FD_ISSET((*iter)->scObject, &fdread))
				{
					if (receive(*iter))
					{
#if defined(_DEBUG)
						char buf[SOCKET_RECVBUFF_LEN] = {0};
						//std::strncpy (buf, (*iter)->dataRecv.data(), SOCKET_RECVBUFF_LEN);
						//std::copy((*iter)->dataRecv.begin(), (*iter)->dataRecv.end(), buf);
						std::sprintf(buf, "%d %d %s", (*iter)->scObject, (long)(*iter)->tmLastRecv, (*iter)->dataRecv.data());
						std::cout << buf << std::endl;
#endif
						SCDataType data(std::begin(SOCKET_HEARTBEAT_HEADER), std::end(SOCKET_HEARTBEAT_HEADER));
						data.insert(data.end(), (*iter)->dataRecv.begin(), (*iter)->dataRecv.end());
						this->send((*iter)->scObject, data);
					}
					else
					{	// 释放关闭的链接
						std::cout << "接收失败！" << WSAGetLastError() << std::endl;
						//errorInfo("接收失败！"); 
						closesocket((*iter)->scObject);
						FD_CLR((*iter)->scObject, &fdread);
						delete (*iter);
						m_lstSocket.erase(iter);
						break;
					}
				}
			}
		}
		// 清空已建立的链接

	}
	else
	{
		errorInfo("构建HeartBeat服务失败！");
		return false;
	}
	return true;
}

/*****************************************************************************************
	<< --- socket::receive	2014-04-09 --- >>
	说明：接收数据
	参数：
	sc		= 套接字
	vecData	= 存放被接收的数据
	返回值：是否执行成功
*****************************************************************************************/
bool socket::receive(SOCKET sc, SCDataType &vecData)
{
	uint8_t buf[SOCKET_RECVBUFF_LEN] = {0};
	int rc = 0;
	vecData.clear();
	// 接收数据
	//switch (m_beType)
	//{
	//case socket::BESocketTypeStream:
	//	{
			rc = recv(sc, (char *)buf, SOCKET_RECVBUFF_LEN, 0);
			if (rc == SOCKET_ERROR )
			{
				return false;
			}
			while (rc > 0)
			{
				if(rc == SOCKET_ERROR)
				{
					vecData.clear();
					return false;
				}
				vecData.insert(vecData.cend(), std::begin(buf), std::begin(buf)+rc);
				// 
				if(rc < SOCKET_RECVBUFF_LEN)
					break;
				rc = recv(sc, (char *)buf, SOCKET_RECVBUFF_LEN, 0);
			}
	//	}

	//	break;
	//case socket::BESocketTypeDgram:
	//	{
	//		rc = recv(sc, (char *)buf, SOCKET_RECVBUFF_LEN, 0);
	//		if(rc == SOCKET_ERROR)
	//		{
	//			vecData.clear();
	//			return false;
	//		}
	//		vecData.insert(vecData.cend(), std::begin(buf), std::begin(buf)+rc);
	//	}

	//	break;
	//default:
	//	break;
	//}	
	return true;
}
bool socket::receive(BSSocketInfo *pscinfo)
{
	assert(pscinfo && pscinfo->scObject != INVALID_SOCKET);
	bool bl = false;
	if(bl = receive(pscinfo->scObject, pscinfo->dataRecv))
	{
		std::time(&pscinfo->tmLastRecv);
	}
	return bl;
}
bool socket::receive(SCDataType &vecData)
{
	return receive(m_scMain, vecData);
}

/*****************************************************************************************
	<< --- socket::send	2014-04-10 --- >>
	说明：发送数据
	参数：
	sc		= 套接字
	vecData	= 被发送的数据
	返回值：是否执行成功
*****************************************************************************************/
bool socket::send(SOCKET sc, const SCDataType &vecData)
{
	//char buf[SOCKET_RECVBUFF_LEN] = {0};
	//std::copy(vecData.begin(), vecData.end(), buf);
	//return ::send(sc, buf, vecData.size(), 0) != SOCKET_ERROR;
	return ::send(sc, vecData.data(), vecData.size(), 0) != SOCKET_ERROR;
}
bool socket::send(const SCDataType &vecData)
{
	//assert(!vecData.empty());
	return this->send(m_scMain, vecData);// ::send(m_scMain, vecData.data(), vecData.size(), 0) != SOCKET_ERROR;
}


/*****************************************************************************************
	<< --- socket::clientEcho	2014-04-09 --- >>
	说明：执行echo服务的客户端
	参数：
	sendData	= 被发送的数据
	recvData	= 接收的数据
	返回值：是否执行成功
*****************************************************************************************/
bool socket::clientEcho(const std::string &sendData, std::string &recvData)
{
	// 发送数据
	::send(m_scMain, sendData.c_str(), sendData.length(), 0);

	return true;
}

/*****************************************************************************************
	<< --- socket::clientHeartBeat	2014-04-09 --- >>
	说明：执行心跳包服务的客户端
	内部启动一个新线程threadHeartBeat，以固定间隔发送心跳包到服务端
	参数：
		phostip	= 目标主机名或IP
		port	= 通讯端口号
	返回值：是否执行成功
*****************************************************************************************/
bool socket::clientHeartBeat(const char *phostip, const uint16_t port)
{
	char sport[10] = {0};
	std::sprintf(sport, "%u", port);
	if (connect(phostip, sport, "tcp"))
	{
		_beginthreadex(nullptr, 0, socket::threadHeartBeat, &m_scMain, 0, nullptr);
	}
	else
	{
		errorInfo("打开到%s:%u的心跳测试失败！", phostip, port);
	}
	return true;
}

}