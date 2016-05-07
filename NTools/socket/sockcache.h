#ifndef __SOCKCACHE_H__
#define __SOCKCACHE_H__
#pragma once
#include <cstdint>
#include <deque>
#include <vector>
#include <queue>
#include <iostream>
#include <cstdio>
#include <algorithm>

#include "GT/gtcmd.h"
#include "gt/gtdcsdata.h"
#include "AC/sysupport.h"

#if defined(_WIN32)
	#include <afxwin.h>         // MFC 核心组件和标准组件
	#include <afxext.h>         // MFC 扩展
	#include <afxframewndex.h>
	#include <ATLComTime.h>
	#include <comutil.h>
	#include <afxdisp.h>
	#include <comdef.h>
#endif

namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	sockcache类	2014-04-29
//	GT socket接收、发送缓存区
//	1.采用TCP通讯，假定大数据（分包发送的）按顺序到达
//	2.缓存区分接收、发送两个独立缓存；采用临界段同步保护
//	3.发送线程
//	4.接收处理线程
////////////////////////////////////////////////////////////////////////////////////////////////////////
class sockcache
{
//public:
//	typedef struct BSGTRecvData{
//		uint32_t uiAddr;				// 发送数据的设备地址
//		std::vector<char>	vecData;	// 接收的原始数据（含括截至符'0D'）
//		bool	isEnd;					// 是否接收完成（即：是否接收到截至符'0D')
//		// 
//		BSGTRecvData()
//			: uiAddr(0), vecData(40)
//		{
//		}
//	}BSGTRecvData;
public:
	sockcache()
		//: m_wndUser(NULL), m_scMain(NULL)
		//, m_hdlSenderThread(NULL), m_uiSenderThreadID(0), m_lSenderThreadSwitch(0)
		//, m_hdlRecverThread(NULL), m_uiRecverThreadID(0), m_lRecverThreadSwitch(0)
	{

	}
public:
	///*****************************************************************************************
	//	<< --- sockcache::open		2014-04-29 --- >>
	//	说明：打开链接，准备接收、发送数据，并开启接收、发送处理线程
	//	参数：
	//	wndUser		= 调用者窗口句柄
	//	返回值：是否执行成功
	//*****************************************************************************************/
	//bool open(HWND wndUser, SOCKET scsend);
	///*****************************************************************************************
	//	<< --- sockcache::close		2014-04-29 --- >>
	//	说明：关闭链接，准备接收、发送数据，并停止接收、发送处理线程
	//	参数：
	//	返回值：是否执行成功
	//*****************************************************************************************/
	//bool close();

	/*****************************************************************************************
		<< --- sockcache::pushRecvData		2014-04-29 --- >>
		说明：将socket接收到的数据直接压入接收缓存队列，以备doThreadAnalysisRecvData线程解析
		参数：
		data	= 被接收的数据，可能有粘包
		返回值：
	*****************************************************************************************/
	void pushRecvData(const SCDataType &data)
	{
		if(data.size() <= 0)
			return;
		// 
		wchar_t buf[200] = {0};
		std::swprintf(buf, 200, L"%s(%d)", gtc::convert::toStringHex(data));
		gtc::app::writeLogs(buf,  L"pushRecvData Hex");
		// 
		m_lockRecver.Lock();
		m_queRecver.push(data);
		m_lockRecver.Unlock();
	}
	/*****************************************************************************************
		<< --- sockcache::popRecvData		2014-05-09 --- >>
		说明：从socket接收读取解析后的应答命令
		参数：
		vecRecv	= 保存应答名；应答可能有粘包，所以采用数组
		返回值：是否有应答命令
	*****************************************************************************************/
	bool popRecvData(std::vector<SCDataType> &vecRecv);

	/*****************************************************************************************
		<< --- sockcache::pushGTCmd		2014-05-07 --- >>
		说明：将用户命令直接压入发送缓存队列，以备doThreadSendGTCmd线程发送
		参数：
		pcmd	= 
		返回值：
	*****************************************************************************************/
	void pushGTCmd(gtcmd *pcmd)
	{
		assert(pcmd != NULL);
		m_lockSender.Lock();
		m_queSender.push(pcmd);
		m_lockSender.Unlock();
	}
	gtcmd* popGTCmd()
	{
		gtcmd *pcmd = nullptr;
		m_lockSender.Lock();
		if (!m_queSender.empty())
		{
			pcmd = m_queSender.front();
			m_queSender.pop();
		}
		m_lockSender.Unlock();
		return pcmd;
	}
	bool isEmptySender()  
	{
		bool bl = false;
		m_lockSender.Lock();
		bl = m_queSender.empty();
		m_lockSender.Unlock();
		return bl;
	}
	bool isEmptyRecver()  
	{
		bool bl = false;
		m_lockRecver.Lock();
		bl = m_queRecver.empty();
		m_lockRecver.Unlock();
		return bl;
	}
public:
	/*****************************************************************************************
		<< --- static sockcache::analysisRecvData		2014-04-30 --- >>
		说明：将粘包数据分割开，对收到的应答按分隔符0Dh分割（保留0Dh）
		参数：
		data	= 应答命令数据
		results	= 分割后的应答命令
		返回值：是否需要分割
	*****************************************************************************************/
	static bool analysisRecvData(const std::vector<char> &data, std::vector<std::vector<char>> &results);
	//// 线程
	///*****************************************************************************************
	//	<< --- static sockcache::doThreadSendGTCmd		2014-04-30 --- >>
	//	说明：从命令缓存区队列中不停取命令，并发送
	//	参数：
	//	pdata	= sockcache*
	//	返回值：
	//*****************************************************************************************/
	//static unsigned __stdcall doThreadSendGTCmd(void *pdata);
	///*****************************************************************************************
	//	<< --- static sockcache::doThreadSendGTCmd		2014-04-30 --- >>
	//	说明：从消息缓存区队列中不停取消息，解析，将结果以windows消息方式送给用户
	//	参数：
	//	pdata	= sockcache*
	//	返回值：
	//*****************************************************************************************/
	//static unsigned __stdcall doThreadAnalysisRecvData(void *pdata);
public:
	////int getLinkIndex() {		return m_iLinkIndex;			}
	//HWND getWndUser() {		return m_wndUser;				}
	//void setMainSocket(SOCKET sc) {				m_scMain = sc;				}
private:
	// 发送缓存区
	std::queue<gtc::gtcmd *>	m_queSender;
	CCriticalSection			m_lockSender;			// 发送缓存同步锁
	std::queue<SCDataType>		m_queRecver;
	CCriticalSection			m_lockRecver;			// 接收缓存同步锁
	// 链接索引
	//int		m_iLinkIndex;
	// 调用者窗口句柄，用来接收消息
	//HWND	m_wndUser;
	//SOCKET	m_scMain;	// 用来发送命令
	//HANDLE						m_hdlSenderThread;
	//unsigned int				m_uiSenderThreadID;
	//LONG						m_lSenderThreadSwitch;	// 线程的开关
	//// 接收缓存区
	//HANDLE						m_hdlRecverThread;
	//unsigned int				m_uiRecverThreadID;
	//LONG						m_lRecverThreadSwitch;	// 线程的开关


};




}

#endif