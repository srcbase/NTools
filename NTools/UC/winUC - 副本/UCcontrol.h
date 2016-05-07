#pragma once
namespace wuc{


/////////////////////////////////////////////////////////////////////////////////////
//

typedef controlUC* (CALLBACK* FINDCONTROLPROC)(controlUC*, LPVOID);

class WINUC_API controlUC
{
public:
	controlUC();
	virtual ~controlUC();

public:
	virtual std::wstring getName() const;
	virtual void setName(PCWSTR pstrName);
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual bool activate();
	virtual paintManagerUC* getManager() const;
	virtual void setManager(paintManagerUC* pManager, controlUC* pParent, bool bInit = true);
	virtual controlUC* getParent() const;

	// 文本相关
	virtual std::wstring getText() const;
	virtual void setText(PCWSTR pstrText);

	// 图形相关
	DWORD getBkColor() const;
	void setBkColor(DWORD dwBackColor);
	//DWORD getBkColor2() const;
	//void setBkColor2(DWORD dwBackColor);
	//DWORD getBkColor3() const;
	//void setBkColor3(DWORD dwBackColor);
	//PCWSTR getBkImage();
	void setBkImage(PCWSTR pkeyImage, const RECT &rcScr);
	void setBkImage(PCWSTR pkeyImage);
	DWORD getBorderColor() const;
	void setBorderColor(DWORD dwBorderColor);
	DWORD getFocusBorderColor() const;
	void setFocusBorderColor(DWORD dwBorderColor);
	//bool isColorHSL() const;
	//void setColorHSL(bool bColorHSL);
	int getBorderSize() const;
	void setBorderSize(int nSize);
	SIZE getBorderRound() const;
	void setBorderRound(SIZE cxyRound);
	bool drawImage(HDC hDC, PCWSTR pStrImage, PCWSTR pStrModify = NULL);

	// 位置相关
	virtual const RECT& getPos() const;
	virtual void setPos(RECT rc);
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual int getX() const;
	virtual int getY() const;
	virtual RECT getPadding() const;
	virtual void setPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
	virtual SIZE getFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void setFixedXY(SIZE szXY);      // 仅float为true时有效
	virtual int getFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void setFixedWidth(int cx);      // 预设的参考值
	virtual int getFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void setFixedHeight(int cy);     // 预设的参考值
	virtual int getMinWidth() const;
	virtual void setMinWidth(int cx);
	virtual int getMaxWidth() const;
	virtual void setMaxWidth(int cx);
	virtual int getMinHeight() const;
	virtual void setMinHeight(int cy);
	virtual int getMaxHeight() const;
	virtual void setMaxHeight(int cy);
	virtual void setRelativePos(SIZE szMove,SIZE szZoom);
	virtual void setRelativeParentSize(SIZE sz);
	virtual BSRelativePosUC getRelativePos() const;
	virtual bool isRelativePos() const;

	// 鼠标提示
	virtual std::wstring getToolTip() const;
	virtual void setToolTip(PCWSTR pstrText);

	// 快捷键
	virtual TCHAR getShortcut() const;
	virtual void setShortcut(TCHAR ch);

	// 菜单
	virtual bool isContextMenuUsed() const;
	virtual void setContextMenuUsed(bool bMenuUsed);

	// 用户属性
	virtual const std::wstring& getUserData(); // 辅助函数，供用户使用
	virtual void setUserData(PCWSTR pstrText); // 辅助函数，供用户使用
	virtual UINT_PTR getTag() const; // 辅助函数，供用户使用
	virtual void setTag(UINT_PTR pTag); // 辅助函数，供用户使用

	// 一些重要的属性
	virtual bool isVisible() const;
	virtual void setVisible(bool bVisible = true);
	virtual void setInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
	virtual bool isEnabled() const;
	virtual void setEnabled(bool bEnable = true);
	virtual bool isMouseEnabled() const;
	virtual void setMouseEnabled(bool bEnable = true);
	virtual bool isFocused() const;
	virtual void setFocus();
	virtual bool isFloat() const;
	virtual void setFloat(bool bFloat = true);

	virtual controlUC* findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	void invalidate();
	bool isUpdateNeeded() const;
	void needUpdate();
	void needParentUpdate();
	//DWORD getAdjustColor(DWORD dwColor);

	virtual void init();
	//virtual void doInit();

	virtual void Event(BSEventUC& event);
	//virtual void doEvent(BSEventUC& event);

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	controlUC* applyAttributeList(PCWSTR pstrList);

	virtual SIZE estimateSize(SIZE szAvailable);

	virtual void paint(HDC hDC, const RECT& rcPaint);
	virtual void paintBkColor(HDC hDC);
	virtual void paintBkImage(HDC hDC);
	virtual void paintStatusImage(HDC hDC);
	virtual void paintText(HDC hDC);
	virtual void paintBorder(HDC hDC);

	virtual void doPostPaint(HDC hDC, const RECT& rcPaint);
//
//public:
//	eventSource onInit;
//	eventSource onDestroy;
//	eventSource onSize;
//	eventSource onEvent;
//	eventSource onNotify;

protected:
	paintManagerUC* m_pManager;
	controlUC*		m_pParent;
	std::wstring	m_wsName;

	bool	m_bUpdateNeeded;
	bool	m_bMenuUsed;
	RECT	m_rcItem;
	RECT	m_rcPadding;
	SIZE	m_cXY;
	SIZE	m_cxyFixed;
	SIZE	m_cxyMin;
	SIZE	m_cxyMax;
	bool	m_bVisible;
	bool	m_bInternVisible;
	bool	m_bEnabled;
	bool	m_bMouseEnabled;
	bool	m_bFocused;
	bool	m_bFloat;
	bool			m_bSetPos; // 防止SetPos循环调用
	BSRelativePosUC m_tRelativePos;

	std::wstring	m_wsText;
	std::wstring	m_wsToolTip;
	wchar_t			m_chShortcut;
	std::wstring	m_wsUserData;
	UINT_PTR		m_pTag;

	DWORD			m_dwBackColor;
	BSImageInfo		m_bsBackImage;
	//DWORD			m_dwBackColor2;
	//DWORD			m_dwBackColor3;
	//std::wstring	m_sBkImage;
	DWORD			m_dwBorderColor;
	DWORD			m_dwFocusBorderColor;
	//bool			m_bColorHSL;
	int				m_nBorderSize;
	SIZE			m_cxyBorderRound;
	RECT			m_rcPaint;
};

}
