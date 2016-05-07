#pragma once
namespace wui {


class txtWinHost;

class WINUI_API richEditUI : public containerUI, public IMessageFilterUI
{
public:
	richEditUI();
	~richEditUI();

	PCWSTR getClass() const;
	LPVOID getInterface(PCWSTR pstrName);
	UINT getControlFlags() const;

	bool isWantTab();
	void setWantTab(bool bWantTab = true);
	bool isWantReturn();
	void setWantReturn(bool bWantReturn = true);
	bool isWantCtrlReturn();
	void setWantCtrlReturn(bool bWantCtrlReturn = true);
	bool isRich();
	void setRich(bool bRich = true);
	bool isReadOnly();
	void setReadOnly(bool bReadOnly = true);
	bool getWordWrap();
	void setWordWrap(bool bWordWrap = true);
	int getFont();
	void setFont(int index);
	void setFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	LONG getWinStyle();
	void setWinStyle(LONG lStyle);
	DWORD getTextColor();
	void setTextColor(DWORD dwTextColor);
	int getLimitText();
	void setLimitText(int iChars);
	long getTextLength(DWORD dwFlags = GTL_DEFAULT) const;
	std::wstring getText() const;
	void setText(PCWSTR pstrText);
	bool getModify() const;
	void setModify(bool bModified = true) const;
	void getSel(CHARRANGE &cr) const;
	void getSel(long& nStartChar, long& nEndChar) const;
	int setSel(CHARRANGE &cr);
	int setSel(long nStartChar, long nEndChar);
	void replaceSel(PCWSTR lpszNewText, bool bCanUndo);
	void replaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
	std::wstring getSelText() const;
	int setSelAll();
	int setSelNone();
	WORD getSelectionType() const;
	bool getZoom(int& nNum, int& nDen) const;
	bool setZoom(int nNum, int nDen);
	bool setZoomOff();
	bool getAutoURLDetect() const;
	bool setAutoURLDetect(bool bAutoDetect = true);
	DWORD getEventMask() const;
	DWORD setEventMask(DWORD dwEventMask);
	std::wstring getTextRange(long nStartChar, long nEndChar) const;
	void hideSelection(bool bHide = true, bool bChangeStyle = false);
	void scrollCaret();
	int insertText(long nInsertAfterChar, PCWSTR lpstrText, bool bCanUndo = false);
	int appendText(PCWSTR lpstrText, bool bCanUndo = false);
	DWORD getDefaultCharFormat(CHARFORMAT2 &cf) const;
	bool setDefaultCharFormat(CHARFORMAT2 &cf);
	DWORD getSelectionCharFormat(CHARFORMAT2 &cf) const;
	bool setSelectionCharFormat(CHARFORMAT2 &cf);
	bool setWordCharFormat(CHARFORMAT2 &cf);
	DWORD getParaFormat(PARAFORMAT2 &pf) const;
	bool setParaFormat(PARAFORMAT2 &pf);
	bool redo();
	bool undo();
	void clear();
	void copy();
	void cut();
	void paste();
	int getLineCount() const;
	std::wstring getLine(int nIndex, int nMaxLength) const;
	int lineIndex(int nLine = -1) const;
	int lineLength(int nLine = -1) const;
	bool lineScroll(int nLines, int nChars = 0);
	point getCharPos(long lChar) const;
	long lineFromChar(long nIndex) const;
	point posFromChar(UINT nChar) const;
	int charFromPos(point pt) const;
	void emptyUndoBuffer();
	UINT setUndoLimit(UINT nLimit);
	long streamIn(int nFormat, EDITSTREAM &es);
	long streamOut(int nFormat, EDITSTREAM &es);

	void doInit();
	// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
	// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
	virtual HRESULT txSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
	IDropTarget* getTxDropTarget();
	virtual bool onTxViewChanged();
	virtual void onTxNotify(DWORD iNotify, void *pv);

	void setScrollPos(SIZE szPos);
	void lineUp();
	void lineDown();
	void pageUp();
	void pageDown();
	void homeUp();
	void endDown();
	void lineLeft();
	void lineRight();
	void pageLeft();
	void pageRight();
	void homeLeft();
	void endRight();

	SIZE estimateSize(SIZE szAvailable);
	void setPos(RECT rc);
	void doEvent(TEventUI& event);
	void doPaint(HDC hDC, const RECT& rcPaint);

	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	LRESULT messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
	txtWinHost* m_pTwh;
	bool m_bVScrollBarFixing;
	bool m_bWantTab;
	bool m_bWantReturn;
	bool m_bWantCtrlReturn;
	bool m_bRich;
	bool m_bReadOnly;
	bool m_bWordWrap;
	DWORD m_dwTextColor;
	int m_iFont;
	int m_iLimitText;
	LONG m_lTwhStyle;
};

}