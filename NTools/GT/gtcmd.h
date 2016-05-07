#ifndef __GTCMD_H__
#define __GTCMD_H__
#pragma once

#include <cstdint>
#include <vector>
#include <ctime>
#include "socket/sockme.h"
#include "gt/gtdcsdata.h"

namespace gtc {
// dll与控制器间的通信指令类型
enum BEGTSocketCmd{
	BEGTSocketCmdNone	= 1,
	BEGTSocketCmdZ		,	// Z指令
	BEGTSocketCmdZ1		,		// Z1指令：初始化单个表示器
	BEGTSocketCmdAc		,
	BEGTSocketCmdG		,
	BEGTSocketCmdH		,
	BEGTSocketCmdPn		,
	BEGTSocketCmdT
};

static std::wstring toString(BEGTSocketCmd cmd)
{
	switch(cmd)
	{
	case BEGTSocketCmdNone:	//= 1,
		return L"未定义指令";
		break;
	case BEGTSocketCmdZ:		//,	// Z指令
		return L"Z指令";
		break;
	case BEGTSocketCmdZ1:		//,		// Z1指令：初始化单个表示器
		return L"Z1指令";
		break;
	case BEGTSocketCmdAc:		//,
		return L"Ac指令";
		break;
	case BEGTSocketCmdG:		//,
		return L"G指令";
		break;
	case BEGTSocketCmdH:		//,
		return L"H指令";
		break;
	case BEGTSocketCmdPn:		//,
		return L"Pn指令";
		break;
	case BEGTSocketCmdT:
		return L"T指令";
		break;
	}
	return L"";
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmd类	2014-04-26
//	GT常用指令基类
//	为什么不把所有命令封装在一个类或结构中？因为：指令间差异太大
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmd
{
protected:
	gtcmd() : m_csCmdType(""), m_beCmdType(BEGTSocketCmdNone) {}
public:
	gtcmd(const char *pcmd, BEGTSocketCmd cmd) : m_csCmdType(pcmd ? pcmd : ""), m_beCmdType(cmd) {}
	virtual ~gtcmd() {}
public:
	/*****************************************************************************************
		<< --- gtcmd::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		//if(m_csCmdType.length() <= 0)
		//	return false;
		cmdData.assign(m_csCmdType.begin(), m_csCmdType.end());
		return cmdData.size() > 0;
	}
public:
	/*****************************************************************************************
		<< --- static gtcmd::appendAddr		2014-04-26 --- >>
		说明：将地址码以ascii格式追加到cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：
	*****************************************************************************************/
	static void appendAddr(std::vector<char> &cmdData, uint32_t addr)
	{
		cmdData.push_back(addr/1000%10+'0');
		cmdData.push_back(addr/100%10+'0');
		cmdData.push_back(addr/10%10+'0');
		cmdData.push_back(addr%10+'0');
	}
	/*****************************************************************************************
		<< --- static gtcmd::appendCRC		2014-04-28 --- >>
		说明：对现有数据cmdData进行CRC校验，并附加校验码
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：
	*****************************************************************************************/
	static void appendCRC(std::vector<char> &cmdData)
	{
	}
	/*****************************************************************************************
		<< --- static gtcmd::appendEOF		2014-04-26 --- >>
		说明：将指令结束符以ascii格式追加到cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：
	*****************************************************************************************/
	static void appendEOF(std::vector<char> &cmdData)
	{
		cmdData.push_back(0);
		cmdData.push_back(0xD);
	}
public:
	BEGTSocketCmd	getCmdType() const {			return m_beCmdType;			}
private:
	// 命令分类标记
	std::string		m_csCmdType;	
	BEGTSocketCmd	m_beCmdType;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdZ类	2014-04-26
//	GT Z指令，系统开始运行时发送，对所有部件单元的初始化
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdZ : public gtcmd
{
public:
	gtcmdZ()
		: gtcmd("Z", BEGTSocketCmdZ)
	{}
public:	
	/*****************************************************************************************
		<< --- gtcmdZ::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdZ1类	2014-04-26
//	GT Z1指令，初始化指定表示器
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdZ1 : public gtcmd
{
protected:
	gtcmdZ1() : gtcmd() {}
public:
	gtcmdZ1(uint32_t addr)
		: gtcmd("Z1", BEGTSocketCmdZ1) , m_uiAddr(addr)
	{}
public:	
	/*****************************************************************************************
		<< --- gtcmdZ1::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// 邦定地址
		gtcmd::appendAddr(cmdData, m_uiAddr);
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
private:
	// 被初始化表示器的地址
	uint32_t	m_uiAddr;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdAc类	2014-04-26
//	GT Ac指令，设置表示器地址
//	1.地址的设置需要一个一个地执行
//	2.首先传送Z指令
//	3.Ac指令不可在发送A指令之后执行
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdAc : public gtcmd
{
protected:
	gtcmdAc() : gtcmd() {}
public:
	gtcmdAc(uint32_t oldAddr, uint32_t newAddr)
		: gtcmd("Ac", BEGTSocketCmdAc) , m_uiOldAddr(oldAddr), m_uiNewAddr(newAddr)
	{}
public:	
	/*****************************************************************************************
		<< --- gtcmdAc::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// 邦定地址
		gtcmd::appendAddr(cmdData, m_uiOldAddr);
		gtcmd::appendAddr(cmdData, m_uiNewAddr);
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
private:
	// 表示器原地址
	uint32_t	m_uiOldAddr;
	// 表示器新地址
	uint32_t	m_uiNewAddr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	BSGTCMDDataPn结构	2014-04-28
//	GT Pn指令的数据部分
////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct BSGTCMDDataPn{
	uint16_t	unAddr;		// 标签地址
	uint8_t		btModel;	// 显示模板号
	std::vector<char>	vecData;	// 模板内容
	// 
	BSGTCMDDataPn(uint16_t addr, uint8_t model, const char *pdata, const int dataLen)
		: unAddr(addr), btModel(model), vecData(pdata, pdata+dataLen)
	{
	}
	void buildCmd(std::vector<char> &cmdData)
	{
		// 邦定地址
		gtcmd::appendAddr(cmdData, unAddr);
		cmdData.push_back(btModel+'0');
		cmdData.insert(cmdData.end(), vecData.begin(), vecData.end());
		cmdData.push_back(_GTCMD_MODEL_SPLIT);
	}
}BSGTCMDDataPn;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdPn类	2014-04-28
//	GT Pn指令，刷新表示器内容
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdPn : public gtcmd
{
protected:
	gtcmdPn() : gtcmd() {}
public:
	gtcmdPn(uint8_t blkNo)
		: gtcmd("Pn", BEGTSocketCmdPn) , m_btBlkNo(blkNo)
	{}
public:	
	/*****************************************************************************************
		<< --- gtcmdPn::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// 区域
		cmdData.push_back(m_btBlkNo/10%10+'0');
		cmdData.push_back(m_btBlkNo%10+'0');
		//cmdData.push_back(m_btBlkNo);
		// 表示器数据
		std::vector<BSGTCMDDataPn>::iterator it;
		for(it = m_vecData.begin(); it != m_vecData.end(); ++it)
			it->buildCmd(cmdData);
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
public:
	/*****************************************************************************************
		<< --- gtcmdPn::pushSELData		2014-05-12 --- >>
		说明：构建标签内容
		参数：
		addr	= 标签地址
		model	= 显示模板号
		pdata	= 显示内容
		dataLen	= 显示内容长度
		返回值：
	*****************************************************************************************/
	void pushSELData(uint16_t addr, uint8_t model, const char *pdata, const int dataLen)
	{
		m_vecData.push_back(BSGTCMDDataPn(addr, model, pdata, dataLen));
	}
private:
	// 区域号
	uint8_t		m_btBlkNo;
	// 表示器数据
	std::vector<BSGTCMDDataPn>	m_vecData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdG类	2014-04-28
//	GT G指令，设置条码读取机的工作状态（开始接收，停止接收）
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdG : public gtcmd
{
protected:
	gtcmdG() : gtcmd() {}
public:
	gtcmdG(uint32_t addr, bool execOpen)
		: gtcmd("G", BEGTSocketCmdG) , m_uiAddr(addr), m_execOpenOrClose(execOpen)
	{}
public:	
	/*****************************************************************************************
		<< --- gtcmdG::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// 邦定地址
		gtcmd::appendAddr(cmdData, m_uiAddr);
		// 开关指令
		cmdData.push_back(m_execOpenOrClose?'O':'C');
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
private:
	// 条码读取机地址
	uint32_t	m_uiAddr;
	// 条码读取机的执行动作（true：开始接收'O''o'；false：停止接收'C''c'）
	bool		m_execOpenOrClose;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdT类	2014-04-28
//	GT T指令，货架指示灯和单点I/O指令
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdT : public gtcmd
{
protected:
	gtcmdT() : gtcmd() {}
public:
	gtcmdT(uint32_t addr, uint8_t ioData = 0)
		: gtcmd("T", BEGTSocketCmdT) , m_uiAddr(addr), m_btIOData(ioData)
	{}
public:	
	/*****************************************************************************************
		<< --- gtcmdT::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// 邦定地址
		gtcmd::appendAddr(cmdData, m_uiAddr);
		// 开关指令
		cmdData.push_back(m_btIOData);
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
public:
	void openRedLight() {				m_btIOData |= 0x4;					}
	void closeRedLight() {				m_btIOData &= ~0x4;					}
	void openGreenLight() {				m_btIOData |= 0x2;					}
	void closeGreenLight() {			m_btIOData &= ~0x2;					}
private:
	// 条码读取机地址
	uint32_t	m_uiAddr;
	// 单点I/O的标记数据
	uint8_t		m_btIOData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gtcmdH类	2014-04-28
//	GT H指令，货架指示灯和单点I/O指令
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gtcmdH : public gtcmd
{
public:
	gtcmdH()
		: gtcmd("H", BEGTSocketCmdH)
	{
		std::time(&m_tmLast);
	}
public:	
	/*****************************************************************************************
		<< --- gtcmdT::getCmd		2014-04-26 --- >>
		说明：将命令按指定格式组织好后存入cmdData
		参数：
		cmdData		= 用来存放格式化后的命令
		返回值：是否执行成功
	*****************************************************************************************/
	virtual bool getCmd(std::vector<char> &cmdData)
	{
		if(!gtcmd::getCmd(cmdData))
			return false;
		// CRC
		gtcmd::appendCRC(cmdData);
		// 指令结束符
		gtcmd::appendEOF(cmdData);
		return cmdData.size() > 0;
	}
public:
	/*****************************************************************************************
		<< --- gtcmdT::refreshTime		2014-05-08 --- >>
		说明：更新“最后一次成功发送时间”为当前时间
		参数：
		返回值：
	*****************************************************************************************/
	void	refreshTime() {			std::time(&m_tmLast);			}
	/*****************************************************************************************
		<< --- gtcmdT::differenceTime		2014-05-08 --- >>
		说明：返回当前时间距“最后一次成功发送时间”的时间差（单位秒）
		参数：
		返回值：时间差（单位秒）
	*****************************************************************************************/
	double	differenceTime() const {
		std::time_t now;
		std::time(&now);
		return std::difftime(now, m_tmLast);
	}
private:
	std::time_t  m_tmLast;	// 最后一次成功发送时间
};

}
#endif