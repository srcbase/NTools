#pragma once

namespace wui {

/////////////////////////////////////////////////////////////////////////////////////
//

class IContainerUI
{
public:
	virtual controlUI* getItemAt(int iIndex) const = 0;
	virtual int getItemIndex(controlUI* pControl) const  = 0;
	virtual bool setItemIndex(controlUI* pControl, int iIndex)  = 0;
	virtual int getCount() const = 0;
	virtual bool add(controlUI* pControl) = 0;
	virtual bool addAt(controlUI* pControl, int iIndex)  = 0;
	virtual bool remove(controlUI* pControl) = 0;
	virtual bool removeAt(int iIndex)  = 0;
	virtual void removeAll() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
class scrollBarUI;

class WINUI_API containerUI : public controlUI, public IContainerUI
{
public:
	containerUI();
	virtual ~containerUI();

public:
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	controlUI* getItemAt(int iIndex) const;
	int getItemIndex(controlUI* pControl) const;
	bool setItemIndex(controlUI* pControl, int iIndex);
	int getCount() const;
	bool add(controlUI* pControl);
	bool addAt(controlUI* pControl, int iIndex);
	bool remove(controlUI* pControl);
	bool removeAt(int iIndex);
	void removeAll();

	void doEvent(TEventUI& event);
	void setVisible(bool bVisible = true);
	void setInternVisible(bool bVisible = true);
	void setMouseEnabled(bool bEnable = true);

	virtual RECT getInset() const;
	virtual void setInset(RECT rcInset); // 设置内边距，相当于设置客户区
	virtual int getChildPadding() const;
	virtual void setChildPadding(int iPadding);
	virtual bool isAutoDestroy() const;
	virtual void setAutoDestroy(bool bAuto);
	virtual bool isDelayedDestroy() const;
	virtual void setDelayedDestroy(bool bDelayed);
	virtual bool isMouseChildEnabled() const;
	virtual void setMouseChildEnabled(bool bEnable = true);

	virtual int findSelectable(int iIndex, bool bForward = true) const;

	void setPos(RECT rc);
	void doPaint(HDC hDC, const RECT& rcPaint);

	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void setManager(paintManagerUI* pManager, controlUI* pParent, bool bInit = true);
	controlUI* findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	virtual SIZE getScrollPos() const;
	virtual SIZE getScrollRange() const;
	virtual void setScrollPos(SIZE szPos);
	virtual void lineUp();
	virtual void lineDown();
	virtual void pageUp();
	virtual void pageDown();
	virtual void homeUp();
	virtual void endDown();
	virtual void lineLeft();
	virtual void lineRight();
	virtual void pageLeft();
	virtual void pageRight();
	virtual void homeLeft();
	virtual void endRight();
	virtual void enableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
	virtual scrollBarUI* getVerticalScrollBar() const;
	virtual scrollBarUI* getHorizontalScrollBar() const;

protected:
	virtual void setFloatPos(int iIndex);
	virtual void processScrollBar(RECT rc, int cxRequired, int cyRequired);

protected:
	std::vector<controlUI *> m_items;
	RECT m_rcInset;
	int m_iChildPadding;
	bool m_bAutoDestroy;
	bool m_bDelayedDestroy;
	bool m_bMouseChildEnabled;
	bool m_bScrollProcess; // 防止SetPos循环调用

	scrollBarUI* m_pVerticalScrollBar;
	scrollBarUI* m_pHorizontalScrollBar;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API verticalLayoutUI : public containerUI
{
public:
	verticalLayoutUI();
	~verticalLayoutUI() {		//TRACE_CLASS_END(this)							
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);
	UINT getControlFlags() const;

	void setSepHeight(int iHeight);
	int getSepHeight() const;
	void setSepImmMode(bool bImmediately);
	bool isSepImmMode() const;
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	void doEvent(TEventUI& event);

	void setPos(RECT rc);
	void doPostPaint(HDC hDC, const RECT& rcPaint);

	RECT getThumbRect(bool bUseNew = false) const;

protected:
	int m_iSepHeight;
	UINT m_uButtonState;
	POINT ptLastMouse;
	RECT m_rcNewPos;
	bool m_bImmMode;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API horizontalLayoutUI : public containerUI
{
public:
	horizontalLayoutUI();
	~horizontalLayoutUI() {				//TRACE_CLASS_END(this)					
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);
	UINT getControlFlags() const;

	void setSepWidth(int iWidth);
	int getSepWidth() const;
	void setSepImmMode(bool bImmediately);
	bool isSepImmMode() const;
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	void doEvent(TEventUI& event);

	void setPos(RECT rc);
	void doPostPaint(HDC hDC, const RECT& rcPaint);

	RECT getThumbRect(bool bUseNew = false) const;

protected:
	int m_iSepWidth;
	UINT m_uButtonState;
	POINT ptLastMouse;
	RECT m_rcNewPos;
	bool m_bImmMode;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API tileLayoutUI : public containerUI
{
public:
	tileLayoutUI();
	~tileLayoutUI() {		//TRACE_CLASS_END(this)								
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	void setPos(RECT rc);

	int getColumns() const;
	void setColumns(int nCols);

	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

protected:
	int m_nColumns;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API dialogLayoutUI : public containerUI
{
public:
	dialogLayoutUI();
	~dialogLayoutUI() {				//TRACE_CLASS_END(this);						
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	void setStretchMode(controlUI* pControl, UINT uMode);

	void setPos(RECT rc);
	SIZE estimateSize(SIZE szAvailable);
	RECT recalcArea();   

protected:  
	typedef struct 
	{
		controlUI* pControl;
		UINT uMode;
		RECT rcItem;
	} STRETCHMODE;

	std::vector<STRETCHMODE> m_aModes;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API tabLayoutUI : public containerUI
{
public:
	tabLayoutUI();
	~tabLayoutUI() {		//TRACE_CLASS_END(this)								
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	bool add(controlUI* pControl);
	bool addAt(controlUI* pControl, int iIndex);
	bool remove(controlUI* pControl);
	void removeAll();
	int getCurSel() const;
	bool selectItem(int iIndex);

	void setPos(RECT rc);

	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

protected:
	int m_iCurSel;
};

}