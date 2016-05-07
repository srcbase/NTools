#ifndef __SOCKETCLIENT_H__
#define __SOCKETCLIENT_H__
#pragma once
#include <cstring>
#include <cstdint>
#include <map>

#include "ac/sysupport.h"
#include "socket/sockme.h"
#include "socket/sockcache.h"
#include "gt/gtdcsdata.h"

// GT指令的应答状态
typedef enum BEGTCMDRespondResult {
	BEGTCMDRespondResultOK			= 0,		// 指令执行成功
	BEGTCMDRespondResultFailure		= 1000,		// 指令执行失败
	BEGTCMDRespondResultTimeout		= 1001		// 超时
};

namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- sockclient	2014-05-06 --- >>
//	客户端套接字的操作
////////////////////////////////////////////////////////////////////////////////////////////////////////
class sockclient : public gtc::socket
{
private:
	sockclient()
		: m_bHeartbeat(false), m_unPort(0)
		, m_hdlWaitforThread(NULL), m_uiWaitforThreadID(0), m_lWaitforThreadSwitch(0)
		, m_hdlHeartbeatThread(NULL), m_uiHeartbeatThreadID(0), m_lHeartbeatThreadSwitch(0)
		, m_lCacheThreadSwitch(0)
		, m_uiRecverThreadID(0), m_hdlRecverThread(NULL)
		, m_uiSenderThreadID(0), m_hdlSenderThread(NULL)
		, m_heventWaitforSender(NULL), m_heventWaitforRecver(NULL)
	{
	}
public:
	sockclient(const char *phost, uint16_t port, bool supportheartbeat = true)
		: m_csHost(phost?phost:""), m_unPort(port), m_bHeartbeat(supportheartbeat)
		, m_hdlWaitforThread(NULL), m_uiWaitforThreadID(0), m_lWaitforThreadSwitch(0)
		, m_hdlHeartbeatThread(NULL), m_uiHeartbeatThreadID(0), m_lHeartbeatThreadSwitch(0)
		, m_lCacheThreadSwitch(0)
		, m_uiRecverThreadID(0), m_hdlRecverThread(NULL)
		, m_uiSenderThreadID(0), m_hdlSenderThread(NULL)
		, m_heventWaitforSender(NULL), m_heventWaitforRecver(NULL)
	{
		m_heventWaitforSender = CreateEvent(NULL, true, false, NULL);
		m_heventWaitforRecver = CreateEvent(NULL, true, false, NULL);
	}
public:
	/*****************************************************************************************
		<< --- sockclient::startup	2014-05-06 --- >>
		说明：启动对远端的TCP通讯服务
			1、启动等待接受消息线程
			2、启动消息解析线程，m_sckCache控制
			3、启动命令发送线程，m_sckCache控制
			4、根据m_bHeartbeat决定是否开启心跳线程，gtc::socket控制
		参数：
		hwnd	= 接收消息的窗口句柄
		返回值：通讯句柄
	*****************************************************************************************/
	uint32_t startup(HWND hwnd);

	/*****************************************************************************************
		<< --- sockclient::startup	2014-05-06 --- >>
		说明：关闭远程链接，并停止startup()创建的线程
		参数：
		返回值：
	*****************************************************************************************/
	void shutdown();
	/*****************************************************************************************
	<< --- sockclient::pushGTCmd		2014-05-07 --- >>
	说明：将用户命令直接压入发送缓存队列
		参数：
		pcmd	= 
		返回值：
	*****************************************************************************************/
	void pushGTCmd(gtcmd *pcmd)
	{
		m_sckCache.pushGTCmd(pcmd);
		SetEvent(m_heventWaitforSender);
	}
	void pushGTCmdResponse(const SCDataType &data)
	{
		m_sckCache.pushRecvData(data);
		SetEvent(m_heventWaitforRecver);
	}
public:
	/*****************************************************************************************
		<< --- static sockclient::doThreadWaitforData		2014-05-06 --- >>
		说明：等待来至控制器的消息，并将消息压入消息缓存队列
		参数：
		pdata	= sockclient*
		返回值：
	*****************************************************************************************/
	static unsigned __stdcall doThreadWaitforData(void *pdata);
	/*****************************************************************************************
		<< --- static sockclient::doThreadHeartbeat		2014-05-06 --- >>
		说明：从消息缓存区队列中不停取消息，解析，将结果以windows消息方式送给用户
		参数：
		pdata	= sockclient*
		返回值：
	*****************************************************************************************/
	static unsigned __stdcall doThreadHeartbeat(void *pdata);
	/*****************************************************************************************
		<< --- static sockclient::doThreadSendGTCmd		2014-05-09 --- >>
		说明：从命令缓存区队列中不停取命令，并发送
		参数：
		pdata	= sockclient*
		返回值：
	*****************************************************************************************/
	static unsigned __stdcall doThreadSendGTCmd(void *pdata);
	/*****************************************************************************************
		<< --- static sockclient::doThreadSendGTCmd		2014-05-09 --- >>
		说明：从消息缓存区队列中不停取消息，解析，将结果以windows消息方式送给用户
		参数：
		pdata	= sockclient*
		返回值：
	*****************************************************************************************/
	static unsigned __stdcall doThreadAnalysisRecvData(void *pdata);
public:
	// 属性
	std::string	getHost() const {			return m_csHost;				}
	uint16_t	getPort() const {			return m_unPort;				}
public:
	// 指令应答结果
	//BEGTCMDRespondResult	m_beRespondResult;
	std::map<HANDLE, BEGTCMDRespondResult> m_mapRespondResult;	// 满足多线程
private:
	HWND			m_hWnd;
	// 是否支持心跳检测
	bool			m_bHeartbeat;
	// 服务主机的名称或IP
	std::string		m_csHost;
	// 通讯端口号
	uint16_t		m_unPort;
	// 等待接收消息线程开关
	LONG volatile	m_lWaitforThreadSwitch;
	unsigned int	m_uiWaitforThreadID;
	HANDLE			m_hdlWaitforThread;
	// 心跳线程开关
	LONG volatile	m_lHeartbeatThreadSwitch;
	unsigned int	m_uiHeartbeatThreadID;
	HANDLE			m_hdlHeartbeatThread;
	// 采用接收、发送缓存区
	sockcache		m_sckCache;
	// 缓存区命令发送、解析线程开关
	LONG volatile	m_lCacheThreadSwitch;
	unsigned int	m_uiSenderThreadID;
	HANDLE			m_hdlSenderThread;
	unsigned int	m_uiRecverThreadID;
	HANDLE			m_hdlRecverThread;
	// 发送接受线程的等待事件
	// 发送缓存区没有命令数据时就处于睡眠等待中
	HANDLE			m_heventWaitforSender;
	// 接收缓存区如果没有数据时就处于睡眠等待中
	HANDLE			m_heventWaitforRecver;
};



}
#endif