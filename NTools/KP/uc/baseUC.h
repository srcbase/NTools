#pragma once
#include "controlUC.h"
#include "containerUC.h"
#include "AC/image.h"

namespace gtc{

#define UC_TITLE_HEIGHT				26	// 窗口标题高的默认值
#define UC_CONTAINER_SPACE_TOP		5	// 容器控件内部节点到上边框的空隙
#define UC_CONTAINER_SPACE_BOTTOM	5	// 容器控件内部节点到底边框的空隙
#define UC_CONTAINER_SPACE_LEFT		7	// 容器控件内部节点到左边框的空隙
#define UC_CONTAINER_SPACE_RIGHT	10	// 容器控件内部节点到右边框的空隙

#define UC_REFERENCE_SPACEX			8	// 两窗口左或右边框相距UC_REFERENCE_SPACEX时认为可画参考线
#define UC_REFERENCE_SPACEY			8	// 两窗口上或下边框相距UC_REFERENCE_SPACEX时认为可画参考线
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	labelUC类	2014-06-23
//	窗口的标题区，该区域始终在窗口最上面，且高度固定，宽度等于窗口的宽度
////////////////////////////////////////////////////////////////////////////////////////////////////////
class labelUC : public controlUC
{
public:
	labelUC(void);
	virtual ~labelUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void Event(BSEventUC &event);
	virtual void paintText(HDC hdc);
public:
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
protected:
	DWORD	m_dwTextColor;
	DWORD	m_dwDisabledTextColor;
	int		m_iFont;
	UINT	m_uiTextStyle;
	RECT	m_rcTextPadding;
	bool	m_bShowHtml;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	buttonUC类	2014-06-23
//	自绘按钮
////////////////////////////////////////////////////////////////////////////////////////////////////////
class buttonUC : public labelUC
{
public:
	buttonUC(void);
	virtual ~buttonUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void paintBkColor(HDC hdc);
	virtual void paintBorder(HDC hdc);
	virtual void paintStatusImage(HDC hdc);
	virtual void Event(BSEventUC &event);

	UINT getState() const {			return m_uiState;								}
	void setImage(gtc::imageResource *pimage);
protected:
	UINT				m_uiState;	// 按钮所处的状态
	gtc::imageResource	*m_pImage;// 按钮状态对应的图片
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	editUC类	2014-07-15
//	文本编辑控件
////////////////////////////////////////////////////////////////////////////////////////////////////////
class editWnd;
class editUC : public labelUC
{
	friend editWnd;
public:
	editUC();
	virtual ~editUC() {				//TRACE_CLASS_END(this)								
	}
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

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

	void setPos(RECT rc);
	void setVisible(bool bVisible = true);

	virtual void Event(BSEventUC &event);

	virtual void paintStatusImage(HDC hdc);
	virtual void paintText(HDC hdc);
protected:
	editWnd* m_pWindow;

	UINT	m_uiMaxChar;
	bool	m_bReadOnly;
	bool	m_bPasswordMode;
	wchar_t m_chPasswordChar;
	UINT	m_uiButtonState;
	//std::wstring m_sNormalImage;
	//std::wstring m_sHotImage;
	//std::wstring m_sFocusedImage;
	//std::wstring m_sDisabledImage;
	DWORD	m_dwEditbkColor;

};



////////////////////////////////////////////////////////////////////////////////////////////////////////
//	editWnd类	2014-07-15
//	文本编辑窗口
////////////////////////////////////////////////////////////////////////////////////////////////////////
class editWnd : public windowWnd
{
public:
	editWnd();
	~editWnd() {			//TRACE_CLASS_END(this)								
	}
	void init(editUC* pOwner);
	RECT calPos();

	PCWSTR getWindowClassName() const;
	PCWSTR getSuperClassName() const;
	void onFinalMessage(HWND hWnd);

	LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	editUC* m_pOwner;
	HBRUSH m_hBkBrush;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	scrollBarUC类	2014-07-17
//	滚动条栏
////////////////////////////////////////////////////////////////////////////////////////////////////////
class scrollBarUC : public controlUC
{
public:
	scrollBarUC(void);
	virtual ~scrollBarUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void paint(HDC hdc);
	//virtual void paintBorder(HDC hdc);
	//virtual void paintStatusImage(HDC hdc);
	virtual void Event(BSEventUC &event);

	virtual void setPosition(RECT &pos);
public:
	void refreshLayout();
	int getBarSize() const {			return m_iBarSize;								}
	int getScrollRange() const {		return m_iRange;								}
	void setScrollRange(int range);
	int getScrollPos() const {			return m_iScrollPos;							}
	void setScrollPos(int pos);
	int getFixedHeight() const {		return m_iBarSize;								}
	int getFixedWidth() const {			return m_iBarSize;								}
	void setContainer(containerUC *pContainer) {	m_pContainer = pContainer;			}
	containerUC* getContainer() {		return m_pContainer;							}

	void setThumbImage(gtc::imageResource *pImage);
protected:
	enum { 
		//DEFAULT_SCROLLBAR_SIZE = 16,
		DEFAULT_TIMERID = 10
	};

	BYTE		m_btAlpha;		// 透明度[0,255]越低越透明
	containerUC	*m_pContainer;	// 本窗口所属组窗口
	int			m_iBarSize;		// 垂直滚动条的宽度，水平滚动条的高度
	int			m_iRange;		// 滚动范围[0,m_nRange]
	int			m_iScrollPos;	// 当前位置
	bool		m_bHorizontal;	// 水平条还是垂直条

	gtc::imageResource	*m_pThumbImage;			// 
	RECT			m_rtThumb;				// 滑块位置
	bool			m_bMoveThumb;			// 是否正在通过滑块移动
	POINT			m_ptLastMouse;			// 开始滚动操作前鼠标最后位置
	int				m_iLastScrollPos;		// 开始滚动操作前的位置
	int				m_iLastScrollOffset;	// 

	UINT		m_uiState;		// 控件所处状态
};



}