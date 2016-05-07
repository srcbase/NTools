#pragma once
#include "kpdeskGroupWnd.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	kpdeskWnd类	2014-06-04
//	桌面信息类
//	前期用来作为与用户交互用，后期可能用来做为墙纸
////////////////////////////////////////////////////////////////////////////////////////////////////////
class kpdeskWnd  : public wui::windowWnd, public wui::INotifyUI
{
public:

public:
	kpdeskWnd(void);
	~kpdeskWnd(void);
public:
	// wui
	virtual PCWSTR getWindowClassName() const { return L"WUIDeskWnd";							}
	virtual UINT getClassStyle() const {		return CS_DBLCLKS;								}
	UINT getWindowStyle() const {				return WS_THICKFRAME;							}
	UINT getWindowExStyle() const {				return WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT|WS_EX_TOOLWINDOW;							}
	virtual void onFinalMessage(HWND hWnd);
	virtual void notify(wui::TNotifyUI& msg);
	virtual LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:

	/*****************************************************************************************
		<< ---  kpdeskWnd::createGroupWindow		2014-06-04 --- >>
		说明：创建桌面图标管理组窗口；如：系统工具，应用程序，图片，文件夹，文档文件，其它文件
		参数：
		返回值：
	*****************************************************************************************/
	void createGroupWindow();

	/*****************************************************************************************
		<< ---  kpdeskWnd::loadGroupWindow	2014-06-06 --- >>
		说明：从配置文件中加载分组窗口
		参数：
		返回值：
	*****************************************************************************************/
	void loadGroupWindow();

	/*****************************************************************************************
		<< ---  kpdeskWnd::closeGroupWindow	2014-06-04 --- >>
		说明：关闭由createGroupWindow创建的窗体
		参数：
		返回值：
	*****************************************************************************************/
	void closeGroupWindow();

	/*****************************************************************************************
		<< ---  kpdeskWnd::classifyNodes		2014-06-09 --- >>
		说明：对桌面图标进行分类
		参数：
		mapGroup	= 图标所属组窗口字典
		返回值：
	*****************************************************************************************/
	bool classifyNodes(std::map<BEDeskGroupFlag, kpdeskGroupWnd *> &mapGroup);

	/*****************************************************************************************
		<< ---  kpdeskWnd::classifyNodes		2014-06-14 --- >>
		说明：根据桌面图标名称找到图标所属的窗口
		参数：
		nodeName	= 图标名称
		返回值：
			！= null	所属窗口
			= null	不存在
	*****************************************************************************************/
	kpdeskGroupWnd* findGroupWnd(const std::wstring &nodeName);
	
	/*****************************************************************************************
		<< ---  kpdeskWnd::getNodeContainScreenPOS		2014-06-14 --- >>
		说明：根据桌面图标名称得到其所属窗体容器的屏幕坐标
		参数：
		nodeName	= 图标名称
		返回值：
			！= empty()	所属窗口
			= empty()	不存在
	*****************************************************************************************/
	RECT getNodeContainScreenPOS(const std::wstring &nodeName);
public:	// staic
	
	
	/*****************************************************************************************
		<< --- static kpdeskWnd::initHost		2014-06-04 --- >>
		说明：启动环境设置
			1、创建一个不可见窗体（以后可能是墙纸窗体）用来与用户交互。如接收用户发送的退出命令
			2、得到桌面信息
			3、创建分组窗口
		参数：
		pparam	= 本dll的HMODULE
		返回值：
		= 0 成功
		> 0 失败
	*****************************************************************************************/
	static unsigned int WINAPI initHost(void *pparam);

	/*****************************************************************************************
		<< --- static kpdeskWnd::stopHost		2014-06-05 --- >>
		说明：恢复默认环境，并关闭所有窗口
		参数：
		pparam	= 本dll的HMODULE
		返回值：
		= 0 成功
		> 0 失败
	*****************************************************************************************/
	static unsigned int WINAPI stopHost(void *pparam);

public:
	std::vector<kpdeskGroupWnd *>& getGroupWnd() {				return m_vecGroupWnd;		}
protected:
	wui::paintManagerUI m_paintManager;
public:
	//static HMODULE		mc_hModule;			// 本dll的HMODULE
	//static HWND			mc_hProgman;		// Program Manager窗口句柄
	//static HWND			mc_hShelldll;		// ShellDLL_DefView窗口句柄
	//static HWND			mc_hDeskListView;	// 桌面控件SysListVew32句柄
protected:
	bool	m_bFirstExec;				// 软件安装后第一次运行
	//gtc::kpdeskAdmin	m_deskAdmin;	// 
	std::vector<kpdeskGroupWnd *>	m_vecGroupWnd;	// 桌面图标分组窗体
	gtc::idlNode::IDINodeMap		m_mapDeskNodes;	// 桌面上的图标
};

