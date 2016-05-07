#pragma once
#include "wucControl.h"
#include "AC\SyncObject.h"

namespace wuc {

class scrollBarUC;

class IContainerUC
{
public:
	virtual controlUC* getItemAt(int iIndex) const = 0;
	virtual int getItemIndex(controlUC* pControl) const  = 0;
	virtual bool setItemIndex(controlUC* pControl, int iIndex)  = 0;
	virtual int getCount() const = 0;
	virtual bool add(controlUC* pControl) = 0;
	virtual bool addAt(controlUC* pControl, int iIndex)  = 0;
	virtual bool remove(controlUC* pControl) = 0;
	virtual bool removeAt(int iIndex)  = 0;
	virtual void removeAll() = 0;
	virtual controlUC* front() const = 0;
	virtual controlUC* back() const = 0;
};

class containerUC : public controlUC, public IContainerUC
{
public:
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-10-8 定义对m_items的保护锁	
#if defined(GTC_BOOST_THREAD)
	protected:
		mutable boost::shared_mutex m_rwMutex;
		mutable bool				m_bReadLocked;
		mutable bool				m_bWriteLocked;
	public:
#define READ_LOCK	gtc::readLock	rdLock(m_rwMutex, m_bReadLocked, __FILEW__, __FUNCTIONW__);
#define WRITE_LOCK	gtc::writeLock	wtLock(m_rwMutex, m_bWriteLocked, __FILEW__, __FUNCTIONW__);
#else
	#define READ_LOCK
	#define WRITE_LOCK
#endif
public:
	containerUC(void);
	virtual ~containerUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void Event(BSEventUC &event);
	virtual void paint(HDC hdc, const RECT &rcPaint);

	virtual RECT getInset() const;
	virtual void setInset(RECT rcInset); // 设置内边距，相当于设置客户区
	virtual void setInset(LONG left, LONG top, LONG right, LONG bottom);
	virtual int getChildPadding() const;
	virtual void setChildPadding(int iPadding);
	virtual bool isMouseChildEnabled() const;
	virtual void setMouseChildEnabled(bool bEnable = true);
	virtual bool isAutoDestroy() const;
	virtual void setAutoDestroy(bool bAuto);
	virtual bool isDelayedFree() const;
	virtual void setDelayedFree(bool bDelayed);
	virtual bool isDelayedDestroy() const;
	virtual void setDelayedDestroy(bool bDelayed);

	virtual void setVisible(bool bVisible = true);
	virtual void setInternVisible(bool bVisible = true);
	virtual void setMouseEnabled(bool bEnabled = true);

	virtual void setManager(drawManager *pManager, controlUC *pParent, bool bInit = true);
	virtual controlUC* findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual bool existsControl(controlUC *pControl);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	virtual controlUC* getItemAt(int iIndex) const;
	virtual int getItemIndex(controlUC* pControl) const ;
	virtual bool setItemIndex(controlUC* pControl, int iIndex);
	virtual int getCount() const;
	virtual bool add(controlUC* pControl);
	virtual bool addAt(controlUC* pControl, int iIndex);
	virtual bool remove(controlUC* pControl);
	virtual bool removeAt(int iIndex) ;
	virtual void removeAll();
	virtual controlUC* front() const;
	virtual controlUC* back() const;

	virtual int findSelectable(int iIndex, bool bForward = true) const;

	virtual void setPosition(RECT pos);


	virtual SIZE getScrollPos() const;
	virtual SIZE getScrollRange() const;
	virtual void setScrollPos(SIZE szPos);
	virtual void lineUp(int step = 0);
	virtual void lineDown(int step = 0);
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
	//virtual void buildScrollBar(scrollBarUC *pScrollbarVertical, scrollBarUC *pScrollbarHorizontal = nullptr);
	virtual void enableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
	virtual scrollBarUC* getVerticalScrollBar() const;
	virtual scrollBarUC* getHorizontalScrollBar() const;
protected:
	virtual void setFloatPos(int iIndex);
	virtual void processScrollBar(RECT rc, int cxRequired, int cyRequired);
public:
	// 容器最小尺寸
	virtual SIZE getMinSize() const;
	bool isAutoHideScroll() const;
	void setAutoHideScroll(bool bAutoHide);
	bool isDriftScroll() const;
	void setDriftScroll(bool bDrift = true);
protected:
	ControlArrayType	m_items;
	//mutable gtc::swmrg			m_swItems;	// 对m_items数据保护
	RECT	m_rcInset;				// 容器控件的内边距
	int		m_iChildPadding;		// 子控件间的额外间距
	bool	m_bMouseChildEnabled;	// 鼠标是否能操纵子控件
	bool	m_bAutoDestroy;			// 是否允许子控件释放。如comboWnd中条目项来自comboUC，所以当释放comboWnd时无需释放其内的条目项
	bool	m_bDelayedFree;			// 删除子控件时要延迟释放。当自控件在click消息中释放时，就需要置m_bAutoDestroy=false，这时被释放控件放入threadDelayedFreeControl线程
	bool	m_bDelayedDestroy;		// 一般都需要延迟释放，因为其子控件可能被mousemove等消息捕获
	bool	m_bAutoHideScroll;		// 是否自动隐藏滚动条（即：鼠标进入时显示、离开时隐藏）
	bool	m_bDriftScroll;			// 滚动条是否在容器内容上方（即：true=滚动条背后有容器的内容，false=滚动条在容器内容的外侧）
	bool	m_bScrollProcess;		// 防止SetPos循环调用
	//SIZE	m_szSpace;				// 节点间的行cy、列cx空隙

	//int m_iChildPadding;
	//bool m_bAutoDestroy;
	//bool m_bMouseChildEnabled;
	//bool m_bScrollProcess; // 防止SetPos循环调用

	scrollBarUC* m_pVerticalScrollBar;
	scrollBarUC* m_pHorizontalScrollBar;

};



/////////////////////////////////////////////////////////////////////////////////////
//

class verticalLayoutUC : public containerUC
{
public:
	verticalLayoutUC();
	~verticalLayoutUC() {		//TRACE_CLASS_END(this)
		//gtc::app::writeVSLogs(L"free verticalLayoutUC <%s>", m_wsName.c_str());
	}
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void Event(BSEventUC& event);

	void setSepHeight(int iHeight);
	int getSepHeight() const;
	void setSepImmMode(bool bImmediately);
	bool isSepImmMode() const;

	virtual void setPosition(RECT pos);

	RECT getThumbRect(bool bUseNew = false) const;

protected:
	int		m_iSepHeight;		// 分隔符高度,正负表示分隔符在顶部还是底部,如(4)
	UINT	m_uButtonState;		
	POINT	ptLastMouse;
	RECT	m_rcNewPos;
	bool	m_bImmMode;			// 拖动分隔符是否立即改变大小,如(false)
};


/////////////////////////////////////////////////////////////////////////////////////
//

class horizontalLayoutUC : public containerUC
{
public:
	horizontalLayoutUC();
	~horizontalLayoutUC() {				//TRACE_CLASS_END(this)					
	}
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void Event(BSEventUC& event);

	void setSepWidth(int iWidth);
	int getSepWidth() const;
	void setSepImmMode(bool bImmediately);
	bool isSepImmMode() const;

	virtual void setPosition(RECT pos);

	RECT getThumbRect(bool bUseNew = false) const;

protected:
	int		m_iSepWidth;		// 分隔符宽,正负表示分隔符在左边还是右边,如(-4)
	UINT	m_uButtonState;
	POINT	ptLastMouse;
	RECT	m_rcNewPos;
	bool	m_bImmMode;			// 拖动分隔符是否立即改变大小,如(false)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	tabLayoutUC  类		2015-7-27
//	只有一个可见元素的容器
////////////////////////////////////////////////////////////////////////////////////////////////////////
class tabLayoutUC : public containerUC
{
public:
	tabLayoutUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

	virtual bool add(controlUC* pControl);
	virtual bool addAt(controlUC* pControl, int iIndex);
	virtual bool remove(controlUC* pControl);
	virtual void removeAll();
	int getCurSel() const;
	bool selectItem(int iIndex);

	virtual void setPosition(RECT rc);

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
protected:
	int		m_iCurSel;
};


}