#pragma once
namespace wui {

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API labelUI : public controlUI
{
public:
	labelUI();
	~labelUI() {			//TRACE_CLASS_END(this)								
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	void setTextStyle(UINT uStyle);
	UINT getTextStyle() const;
	void setTextColor(DWORD dwTextColor);
	DWORD getTextColor() const;
	void setDisabledTextColor(DWORD dwTextColor);
	DWORD getDisabledTextColor() const;
	void setFont(int index);
	int getFont() const;
	RECT getTextPadding() const;
	void setTextPadding(RECT rc);
	bool isShowHtml();
	void setShowHtml(bool bShowHtml = true);

	SIZE estimateSize(SIZE szAvailable);
	void doEvent(TEventUI& event);
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void paintText(HDC hDC);

protected:
	DWORD	m_dwTextColor;
	DWORD	m_dwDisabledTextColor;
	int		m_iFont;
	UINT	m_uTextStyle;
	RECT	m_rcTextPadding;
	bool	m_bShowHtml;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API buttonUI : public labelUI
{
public:
	buttonUI();
	~buttonUI() {		//TRACE_CLASS_END(this)			
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);
	UINT getControlFlags() const;

	bool activate();
	void setEnabled(bool bEnable = true);
	void doEvent(TEventUI& event);

	PCWSTR getNormalImage();
	void setNormalImage(PCWSTR pStrImage);
	PCWSTR getHotImage();
	void setHotImage(PCWSTR pStrImage);
	PCWSTR getPushedImage();
	void setPushedImage(PCWSTR pStrImage);
	PCWSTR getFocusedImage();
	void setFocusedImage(PCWSTR pStrImage);
	PCWSTR getDisabledImage();
	void setDisabledImage(PCWSTR pStrImage);

	void setHotTextColor(DWORD dwColor);
	DWORD getHotTextColor() const;
	void setPushedTextColor(DWORD dwColor);
	DWORD getPushedTextColor() const;
	void setFocusedTextColor(DWORD dwColor);
	DWORD getFocusedTextColor() const;
	SIZE estimateSize(SIZE szAvailable);
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void paintText(HDC hDC);
	void paintStatusImage(HDC hDC);

protected:
	UINT m_uButtonState;

	DWORD m_dwHotTextColor;
	DWORD m_dwPushedTextColor;
	DWORD m_dwFocusedTextColor;

	stringex m_sNormalImage;
	stringex m_sHotImage;
	stringex m_sPushedImage;
	stringex m_sFocusedImage;
	stringex m_sDisabledImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API optionUI : public buttonUI
{
public:
	optionUI();
	~optionUI();

	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	void setManager(paintManagerUI* pManager, controlUI* pParent, bool bInit = true);

	bool activate();
	void setEnabled(bool bEnable = true);

	PCWSTR getSelectedImage();
	void setSelectedImage(PCWSTR pStrImage);

	void setSelectedTextColor(DWORD dwTextColor);
	DWORD getSelectedTextColor();

	PCWSTR getForeImage();
	void setForeImage(PCWSTR pStrImage);

	PCWSTR getGroup() const;
	void setGroup(PCWSTR pStrGroupName = NULL);
	bool isSelected() const;
	void selected(bool bSelected);

	SIZE estimateSize(SIZE szAvailable);
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void paintStatusImage(HDC hDC);
	void paintText(HDC hDC);

protected:
	bool m_bSelected;
	stringex m_sGroupName;

	DWORD m_dwSelectedTextColor;

	stringex m_sSelectedImage;
	stringex m_sForeImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API textUI : public labelUI
{
public:
	textUI();
	~textUI();

	PCWSTR getClass() const;
	UINT getControlFlags() const;
	LPVOID getInterface(PCWSTR pstrName);

	stringex* getLinkContent(int iIndex);

	void doEvent(TEventUI& event);
	SIZE estimateSize(SIZE szAvailable);

	void paintText(HDC hDC);

protected:
	enum { MAX_LINK = 8 };
	int m_nLinks;
	RECT m_rcLinks[MAX_LINK];
	stringex m_sLinks[MAX_LINK];
	int m_nHoverLink;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API progressUI : public labelUI
{
public:
	progressUI();
	~progressUI() {			//TRACE_CLASS_END(this);								
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	bool isHorizontal();
	void setHorizontal(bool bHorizontal = true);
	int getMinValue() const;
	void setMinValue(int nMin);
	int getMaxValue() const;
	void setMaxValue(int nMax);
	int getValue() const;
	void setValue(int nValue);
	PCWSTR getFgImage() const;
	void setFgImage(PCWSTR pStrImage);

	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	void paintStatusImage(HDC hDC);

protected:
	bool m_bHorizontal;
	int m_nMax;
	int m_nMin;
	int m_nValue;

	stringex m_sFgImage;
	stringex m_sFgImageModify;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API sliderUI : public progressUI
{
public:
	sliderUI();
	~sliderUI() {			//TRACE_CLASS_END(this)								
	}
	PCWSTR getClass() const;
	UINT getControlFlags() const;
	LPVOID getInterface(PCWSTR pstrName);

	void setEnabled(bool bEnable = true);

	int getChangeStep();
	void setChangeStep(int step);
	void setThumbSize(SIZE szXY);
	RECT getThumbRect() const;
	PCWSTR getThumbImage() const;
	void setThumbImage(PCWSTR pStrImage);
	PCWSTR getThumbHotImage() const;
	void setThumbHotImage(PCWSTR pStrImage);
	PCWSTR getThumbPushedImage() const;
	void setThumbPushedImage(PCWSTR pStrImage);

	void doEvent(TEventUI& event);
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	void paintStatusImage(HDC hDC);

protected:
	SIZE m_szThumb;
	UINT m_uButtonState;
	int m_nStep;

	stringex m_sThumbImage;
	stringex m_sThumbHotImage;
	stringex m_sThumbPushedImage;

	stringex m_sImageModify;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class editWnd;

class WINUI_API editUI : public labelUI
{
	friend editWnd;
public:
	editUI();
	~editUI() {				//TRACE_CLASS_END(this)								
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);
	UINT getControlFlags() const;

	void setEnabled(bool bEnable = true);
	void setText(PCWSTR pstrText);
	void setMaxChar(UINT uMax);
	UINT getMaxChar();
	void setReadOnly(bool bReadOnly);
	bool isReadOnly() const;
	void setPasswordMode(bool bPasswordMode);
	bool isPasswordMode() const;
	void setPasswordChar(TCHAR cPasswordChar);
	wchar_t getPasswordChar() const;

	PCWSTR getNormalImage();
	void setNormalImage(PCWSTR pStrImage);
	PCWSTR getHotImage();
	void setHotImage(PCWSTR pStrImage);
	PCWSTR getFocusedImage();
	void setFocusedImage(PCWSTR pStrImage);
	PCWSTR getDisabledImage();
	void setDisabledImage(PCWSTR pStrImage);
	void setNativeEditBkColor(DWORD dwBkColor);
	DWORD getNativeEditBkColor() const;

	void setPos(RECT rc);
	void setVisible(bool bVisible = true);
	void setInternVisible(bool bVisible = true);
	SIZE estimateSize(SIZE szAvailable);
	void doEvent(TEventUI& event);
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void paintStatusImage(HDC hDC);
	void paintText(HDC hDC);

protected:
	editWnd* m_pWindow;

	UINT m_uMaxChar;
	bool m_bReadOnly;
	bool m_bPasswordMode;
	wchar_t m_cPasswordChar;
	UINT m_uButtonState;
	stringex m_sNormalImage;
	stringex m_sHotImage;
	stringex m_sFocusedImage;
	stringex m_sDisabledImage;
	DWORD m_dwEditbkColor;
};

/////////////////////////////////////////////////////////////////////////////////////
//
//

class editWnd : public windowWnd
{
public:
	editWnd();
	~editWnd() {			//TRACE_CLASS_END(this)								
	}
	void init(editUI* pOwner);
	RECT calPos();

	PCWSTR getWindowClassName() const;
	PCWSTR getSuperClassName() const;
	void onFinalMessage(HWND hWnd);

	LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	editUI* m_pOwner;
	HBRUSH m_hBkBrush;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API scrollBarUI : public controlUI
{
public:
	scrollBarUI();
	~scrollBarUI() {		//TRACE_CLASS_END(this)								
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);

	containerUI* getOwner() const;
	void setOwner(containerUI* pOwner);

	void setEnabled(bool bEnable = true);
	void setFocus();

	bool isHorizontal();
	void setHorizontal(bool bHorizontal = true);
	int getScrollRange() const;
	void setScrollRange(int nRange);
	int getScrollPos() const;
	void setScrollPos(int nPos);
	int getLineSize() const;
	void setLineSize(int nSize);

	bool getShowButton1();
	void setShowButton1(bool bShow);
	PCWSTR getButton1NormalImage();
	void setButton1NormalImage(PCWSTR pStrImage);
	PCWSTR getButton1HotImage();
	void setButton1HotImage(PCWSTR pStrImage);
	PCWSTR getButton1PushedImage();
	void setButton1PushedImage(PCWSTR pStrImage);
	PCWSTR getButton1DisabledImage();
	void setButton1DisabledImage(PCWSTR pStrImage);

	bool getShowButton2();
	void setShowButton2(bool bShow);
	PCWSTR getButton2NormalImage();
	void setButton2NormalImage(PCWSTR pStrImage);
	PCWSTR getButton2HotImage();
	void setButton2HotImage(PCWSTR pStrImage);
	PCWSTR getButton2PushedImage();
	void setButton2PushedImage(PCWSTR pStrImage);
	PCWSTR getButton2DisabledImage();
	void setButton2DisabledImage(PCWSTR pStrImage);

	PCWSTR getThumbNormalImage();
	void setThumbNormalImage(PCWSTR pStrImage);
	PCWSTR getThumbHotImage();
	void setThumbHotImage(PCWSTR pStrImage);
	PCWSTR getThumbPushedImage();
	void setThumbPushedImage(PCWSTR pStrImage);
	PCWSTR getThumbDisabledImage();
	void setThumbDisabledImage(PCWSTR pStrImage);

	PCWSTR getRailNormalImage();
	void setRailNormalImage(PCWSTR pStrImage);
	PCWSTR getRailHotImage();
	void setRailHotImage(PCWSTR pStrImage);
	PCWSTR getRailPushedImage();
	void setRailPushedImage(PCWSTR pStrImage);
	PCWSTR getRailDisabledImage();
	void setRailDisabledImage(PCWSTR pStrImage);

	PCWSTR getBkNormalImage();
	void setBkNormalImage(PCWSTR pStrImage);
	PCWSTR getBkHotImage();
	void setBkHotImage(PCWSTR pStrImage);
	PCWSTR getBkPushedImage();
	void setBkPushedImage(PCWSTR pStrImage);
	PCWSTR getBkDisabledImage();
	void setBkDisabledImage(PCWSTR pStrImage);

	void setPos(RECT rc);
	void doEvent(TEventUI& event);
	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	void doPaint(HDC hDC, const RECT& rcPaint);

	void paintBk(HDC hDC);
	void paintButton1(HDC hDC);
	void paintButton2(HDC hDC);
	void paintThumb(HDC hDC);
	void paintRail(HDC hDC);

protected:

	enum { 
		DEFAULT_SCROLLBAR_SIZE = 16,
		DEFAULT_TIMERID = 10,
	};

	bool m_bHorizontal;
	int m_nRange;
	int m_nScrollPos;
	int m_nLineSize;
	containerUI* m_pOwner;
	POINT ptLastMouse;
	int m_nLastScrollPos;
	int m_nLastScrollOffset;
	int m_nScrollRepeatDelay;

	stringex m_sBkNormalImage;
	stringex m_sBkHotImage;
	stringex m_sBkPushedImage;
	stringex m_sBkDisabledImage;

	bool m_bShowButton1;
	RECT m_rcButton1;
	UINT m_uButton1State;
	stringex m_sButton1NormalImage;
	stringex m_sButton1HotImage;
	stringex m_sButton1PushedImage;
	stringex m_sButton1DisabledImage;

	bool m_bShowButton2;
	RECT m_rcButton2;
	UINT m_uButton2State;
	stringex m_sButton2NormalImage;
	stringex m_sButton2HotImage;
	stringex m_sButton2PushedImage;
	stringex m_sButton2DisabledImage;

	RECT m_rcThumb;
	UINT m_uThumbState;
	stringex m_sThumbNormalImage;
	stringex m_sThumbHotImage;
	stringex m_sThumbPushedImage;
	stringex m_sThumbDisabledImage;

	stringex m_sRailNormalImage;
	stringex m_sRailHotImage;
	stringex m_sRailPushedImage;
	stringex m_sRailDisabledImage;

	stringex m_sImageModify;
};

}