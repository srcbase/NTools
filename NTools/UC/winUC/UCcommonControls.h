#pragma once
#include "UC/winUC/UCcontrol.h"
namespace wuc {


/////////////////////////////////////////////////////////////////////////////////////
//

class labelUC : public controlUC
{
public:
	labelUC();
	~labelUC() {			//TRACE_CLASS_END(this)								
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
	virtual void Event(BSEventUC& event);
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

class buttonUC : public labelUC
{
public:
	buttonUC();
	~buttonUC() {		//TRACE_CLASS_END(this)			
	}
	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);
	UINT getControlFlags() const;

	bool activate();
	void setEnabled(bool bEnable = true);
	virtual void Event(BSEventUC& event);

	PCWSTR getNormalImage();
	PCWSTR getHotImage();
	PCWSTR getPushedImage();
	PCWSTR getFocusedImage();
	PCWSTR getDisabledImage();
	void setNormalImage(PCWSTR pkeyName, const RECT *prcScr = nullptr);
	void setHotImage(PCWSTR pkeyName, const RECT *prcScr = nullptr);
	void setPushedImage(PCWSTR pkeyName, const RECT *prcScr = nullptr);
	void setFocusedImage(PCWSTR pkeyName, const RECT *prcScr = nullptr);
	void setDisabledImage(PCWSTR pkeyName, const RECT *prcScr = nullptr);

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

	BSImageInfo m_bsNormalImage;
	BSImageInfo m_bsHotImage;
	BSImageInfo m_bsPushedImage;
	BSImageInfo m_bsFocusedImage;
	BSImageInfo m_bsDisabledImage;
};
}

