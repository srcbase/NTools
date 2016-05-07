#pragma once
class dialogMain : public wuc::windowWnd, public wuc::INotifyUC, public gtc::singleton<dialogMain>
{
public:
	dialogMain(void);
	~dialogMain(void);

	// gtc::singleton
	static dialogMain& getSingleton();
	static dialogMain* getSingletonPtr();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	窗口样式	2015-1-19
	virtual PCWSTR getWindowClassName() const {		return DBGER_WND_MAIN_CLASS;					}
	virtual PCWSTR getWindowCaption() const {		return DBGER_WND_MAIN_CAPTION;					}
	virtual UINT getClassStyle() const {			return CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS;		}
	virtual UINT getWindowStyle() const {			return WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;}
	virtual UINT getWindowExStyle() const {			return WS_EX_TOPMOST|WS_EX_TOOLWINDOW;			}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-20	相对WUC::handleMessage多了bHandled参数，其用来判断用户是否接收了对消息处理
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	virtual void onFinalMessage(HWND hWnd);
	virtual void notify(wuc::BSNotifyUC& msg);
public:
	LRESULT onCreate();
	LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void onInit();
	LRESULT onMessageTray(WPARAM wParam, LPARAM lParam);
	// 菜单命令
	void onMenuCommand(WPARAM wParam, LPARAM lParam);
	void onClickMinbox();
	void onClickClose();
	void onClickSave();
	void onClickClear();

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-7-24 创建本窗体
	bool create();
	// 任务栏图标
	bool createTray();
	void destroyTray();
	void modifyTray();
	// 添加一条日志
	void addLog(gtc::BELogType logType, PCWSTR pInfo);
public:
	void setCaption(PCWSTR pTitle);
	void setStateText(PCWSTR pState);
protected:
	wuc::drawManager		m_drawManager;

	wuc::horizontalLayoutUC	*m_pTitleBar;		// 标题栏
	wuc::horizontalLayoutUC *m_pToolBar;		// 工具栏
	wuc::horizontalLayoutUC	*m_pClientUC;		// 客户区
	wuc::horizontalLayoutUC	*m_pTaskBar;		// 任务栏或状态栏

	wuc::labelUC			*m_labelCaption;	// 窗口标题文本
	wuc::labelUC			*m_labelState;		// 状态文本
	wuc::buttonUC			*m_buttonMinBox;	// 最小化
	wuc::buttonUC			*m_buttonClose;		// 关闭
	wuc::buttonUC			*m_buttonClear;		// 清空
	wuc::buttonUC			*m_buttonSave;		// 保存
	wuc::wkeWebkitUC		*m_web;				// 
	// 任务栏图标结构数据
	NOTIFYICONDATA			m_nid;

};

