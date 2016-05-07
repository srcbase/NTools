#ifndef __WUCMENU_H__
#define __WUCMENU_H__
#pragma once
#include "wucBase.h"
#include "wucList.h"
#include "windowWnd.h"


namespace wuc {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-8-25 	
enum BEMenuAlign {
	BEMenuAlignLeft			= 0x0000,			// 在基点左侧
	BEMenuAlignRight		= 0x0001,			// 在基点右侧
	BEMenuAlignTop			= 0x0002,			// 在基点上
	BEMenuAlignBottom		= 0x0004			// 下
};

class menuWnd;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	menuElementUC  类		2015-8-24
//	菜单项
//	菜单项标记：分割线、子菜单、命令菜单
//		分割线	= 文本为空或为L"-"为分隔条
//		子菜单	= 含有子菜单
//		命令菜单	= 无子项，且文本不为空或L"-"
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class menuElementUC : public listContainerElementUC
{
public:
	typedef std::vector<menuElementUC *>	PtrArrayType;
	friend menuWnd;
public:
	menuElementUC();
	~menuElementUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

	virtual void paint(HDC hdc, const RECT &rcPaint);
	virtual void Event(BSEventUC &event);
	virtual bool activate();
	virtual SIZE estimateSize(SIZE szAvailable);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	// IListItemUC
	virtual bool select(bool bSelect = true);
	virtual void drawItem(HDC hdc, const RECT& rcItem);
	virtual void drawItemBk(HDC hdc, const RECT& rcItem);
	// 
	bool getNormalImage(RECT &rc);
	void setNormalImage(RECT rcImgCell);
	void setNormalImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getHotImage(RECT &rc);
	void setHotImage(RECT rcImgCell);
	void setHotImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getPushedImage(RECT &rc);
	void setPushedImage(RECT rcImgCell);
	void setPushedImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getDisabledImage(RECT &rc);
	void setDisabledImage(RECT rcImgCell);
	void setDisabledImage(LONG left, LONG top, LONG right, LONG bottom);

	bool getNormalImageSelected(RECT &rc);
	void setNormalImageSelected(RECT rcImgCell);
	void setNormalImageSelected(LONG left, LONG top, LONG right, LONG bottom);
	bool getHotImageSelected(RECT &rc);
	void setHotImageSelected(RECT rcImgCell);
	void setHotImageSelected(LONG left, LONG top, LONG right, LONG bottom);
	bool getPushedImageSelected(RECT &rc);
	void setPushedImageSelected(RECT rcImgCell);
	void setPushedImageSelected(LONG left, LONG top, LONG right, LONG bottom);
	bool getDisabledImageSelected(RECT &rc);
	void setDisabledImageSelected(RECT rcImgCell);
	void setDisabledImageSelected(LONG left, LONG top, LONG right, LONG bottom);

public:
	menuWnd* getMenuWnd();
	void createMenuWnd();
public:
	buttonUC* createIconButton(PCWSTR pAttr = NULL);
	std::wstring getGroup() const;
	void setGroup(PCWSTR pstrGroupName = NULL);

	void setCmdID(UINT cmdID);
	UINT getCmdID() const;

	void setMsgWnd(HWND hWnd);
	HWND getMsgWnd() const;
//public:
//	gtc::eventSource	OnMenuCommand;
protected:
	UINT			m_uiCmdID;			// 菜单命令ID
	std::wstring	m_wsGroupName;		// 菜单组标记（同组只能右一个被选中）
	menuWnd			*m_pWindow;

	buttonUC		*m_buttonIcon;

	HWND			m_hwndMsg;			// 接收菜单消息

};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	menuUC  类		2015-8-24
//	菜单封装类
//	m_rcInset：菜单对话框四周留边
//	m_rcPadding：菜单项左右图标区域宽
//	m_bsInfo.iFont：菜单项的默认字体
//	菜单项具有4态：DISABLED PUSHED HOT NORMAL SELECTED（开关 check菜单 option菜单）
////////////////////////////////////////////////////////////////////////////////////////////////////////
class menuUC : public listUC
{
public:
public:
	menuUC();
	~menuUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

	virtual int getItemIndex(controlUC* pControl) const ;
	virtual bool setItemIndex(controlUC* pControl, int iIndex);
	virtual bool add(controlUC* pControl);
	virtual bool addAt(controlUC* pControl, int iIndex);
	virtual bool remove(controlUC* pControl);

	virtual SIZE estimateSize(SIZE szAvailable);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void setDefaultItemHeight(int height);
	int getDefaultItemHeight() const;
	void setSeparatorColor(DWORD dwClr);
	DWORD getSeparatorColor() const;
public:
	// 创建子菜单
	void createMenuWnd();
	// 弹出菜单
	bool trackPopupMenu(HWND hParent, LONG x, LONG y, DWORD dwFlag = BEMenuAlignLeft|BEMenuAlignTop);
	// 
	menuElementUC* addSeparator();
	// 普通菜单，其可能带有三态图标的普通按钮
	menuElementUC* addMenu(UINT cmdID, PCWSTR pText, PCWSTR pbtnAttr = NULL);
	menuElementUC* addMenu(PCWSTR pText, menuElementUC::PtrArrayType &vecSubMenu);

public:
	// 只创建不添加
	menuElementUC* createSeparator();
	// 普通菜单，其可能带有三态图标的普通按钮
	menuElementUC* createMenu(UINT cmdID, PCWSTR pText, PCWSTR pAttrList = NULL);
	// 创建菜单，含有子菜单
	menuElementUC* createMenu(PCWSTR pText, menuElementUC::PtrArrayType &vecSubMenu);
public:
	POINT getBasePoint() const;
	DWORD getPopAlign() const;

public:
	// 得到普通带图标菜单项属性
	static std::wstring getNormalAttributeList(PCWSTR pImage, RECT *pNormal, RECT *pHot = NULL, RECT *pPushed = NULL, RECT *pDisabled = NULL);

public:
	////////////////////////////////////////////////////////////////////////////////////////////
	//// 2015-12-14		kp007	这种方式不好，当弹出模式对话框时，菜单窗口接收到kill_focus消息，其内部关闭菜单窗口，也就是否了，可是OnMenuCommand还处于调用中（模式窗口）
	//gtc::eventSource	OnMenuCommand;

	
	void registerMessageWnd(HWND hWnd);
	HWND getMessageWnd() const;

	static UINT getSelfRegMsg();
protected:
	POINT	m_ptBase;					// 菜单弹出位置
	DWORD	m_dwAlign;					// 菜单弹出方向
	int		m_iDefaultItemHeight;		// 菜单项的默认高度
	DWORD	m_clrSeparator;				// 分割线颜色
	//int		m_iIconAreaWidth;			// 左侧图标区域宽度
	//int		m_iSubiconAreaWidth;		// 有子菜单时右侧图标绘制区域宽度

	HWND	m_hwndMsg;					// 接收菜单消息

	static UINT	mc_ucMsgMenu;			// 注册消息，wuc菜单发送(post)的命令消息
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	menuWnd  类		2015-8-24
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class menuWnd : public windowWnd
{
public:
	typedef std::vector<menuWnd *> PtrArrayType;
public:
	menuWnd();
	~menuWnd();
	virtual PCWSTR getWindowClassName() const {		return L"menuWnd";								}
	virtual UINT getWindowStyle() const {			return WS_POPUP|WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	}
	virtual UINT getWindowExStyle() const {			return WS_EX_TOOLWINDOW | WS_EX_TOPMOST;		}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

	void init(HWND hParent, menuUC *pLayout);
	void init(menuElementUC *pMenuElem);
public:
	//static void AddWnd(menuWnd *pWnd);
	static void closeAll();
	static void closeWnd(HWND hWnd);
public:
	menuUC* getLayout() const;
protected:
	drawManager		m_drawManager;

	menuUC			*m_pLayout;
	menuElementUC	*m_pOwner;

	static PtrArrayType	mc_vecWnd;
};

}	// wuc
#endif	// __WUCMENU_H__