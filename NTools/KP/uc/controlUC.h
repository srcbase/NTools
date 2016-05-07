#pragma once
#include "ucdef.h"
#include "windowUC.h"
namespace gtc{
class controlUC
{
public:
	controlUC(void);
	virtual ~controlUC(void);
public:
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void Event(BSEventUC &event);
	virtual const std::wstring& getName() const;
	virtual void setName(PCWSTR pstrName);

	virtual void paint(HDC hdc);
	virtual void paintBkColor(HDC hdc);
	virtual void paintBkImage(HDC hdc);
	virtual void paintStatusImage(HDC hdc);
	virtual void paintText(HDC hdc);
	virtual void paintBorder(HDC hdc);

	virtual bool activate();
	virtual windowUC* getManager();
	virtual void setManager(windowUC *pManager, controlUC *pParent);
	virtual controlUC* getParent() const;

	// 文本相关
	virtual PCWSTR getText() const;
	virtual void setText(PCWSTR pstrText);
	// 
	DWORD getBkColor() const;
	void setBkColor(DWORD dwBackColor);
	DWORD getBorderColor() const;
	void setBorderColor(DWORD dwBorderColor);
	DWORD getFocusBorderColor() const;
	void setFocusBorderColor(DWORD dwBorderColor);
	int getBorderSize() const;
	void setBorderSize(int nSize);
	SIZE getBorderRound() const;
	void setBorderRound(SIZE cxyRound);
	// 
	virtual int getMinWidth() const;
	virtual void setMinWidth(int cx);
	virtual int getMaxWidth() const;
	virtual void setMaxWidth(int cx);
	virtual int getMinHeight() const;
	virtual void setMinHeight(int cy);
	virtual int getMaxHeight() const;
	virtual void setMaxHeight(int cy);
	// 一些重要的属性
	virtual bool isVisible() const;
	virtual void setVisible(bool bVisible = true);
	virtual bool isEnabled() const;
	virtual void setEnabled(bool bEnable = true);
	virtual bool isMouseEnabled() const;
	virtual void setMouseEnabled(bool bEnable = true);
	virtual bool isFocused() const;
	virtual void setFocus(bool bFocus = true);
	virtual bool isFloat() const;
	virtual void setFloat(bool bFloat = true);

	void invalidate();
	bool isUpdateNeeded() const;
	void needUpdate();
	void needParentUpdate();
	// 客户坐标
	RECT getPosition() const;
	virtual void setPosition(RECT &pos);
	/*****************************************************************************************
		<< ---  groupUC::ptIn		2014-06-24 --- >>
		说明：查看点是否在控件上
		参数：
		pt = 参考点（客户坐标）
		返回值：是否在
	*****************************************************************************************/
	bool ptIn(const POINT &pt);

	void clientToScreen(RECT *prect);
	void clientToScreen(POINT *ppt);

	// 菜单
	virtual bool isContextMenuUsed() const;
	virtual void setContextMenuUsed(bool bMenuUsed);
	// 鼠标提示
	virtual std::wstring getToolTip() const;
	virtual void setToolTip(PCWSTR pstrText);
	// 快捷键
	virtual TCHAR getShortcut() const;
	virtual void setShortcut(TCHAR ch);

protected:
	windowUC		*m_pwndManager;		// 控件所属窗口
	std::wstring	m_wsName;			// 控件名称，同一窗体内的控件名称必须唯一
	controlUC		*m_pParent;			// 控件的父控件

	RECT			m_rcItem;			// 控件在m_pWnd内的逻辑坐标
	SIZE			m_cxyMin;
	SIZE			m_cxyMax;

	std::wstring	m_wsText;			// 控件内容
	DWORD			m_dwBackColor;
	DWORD			m_dwBorderColor;
	DWORD			m_dwFocusBorderColor;
	bool			m_bVisible;			// 
	bool			m_bFocused;			// 
	bool			m_bEnabled;			// 
	bool			m_bFloat;
	bool			m_bMouseEnabled;
	bool			m_bUpdateNeeded;	// 是否需要重新绘制
	bool			m_bMenuUsed;		// 支持弹出式菜单		
	
	std::wstring	m_wsToolTip;
	wchar_t			m_chShortcut;

	int				m_nBorderSize;
	SIZE			m_cxyBorderRound;
};



}