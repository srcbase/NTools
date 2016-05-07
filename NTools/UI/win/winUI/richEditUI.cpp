#include "StdAfx.h"
#include "richEditUI.h"

#pragma comment(lib, "Riched20.lib")

namespace wui {


const LONG cInitTextMax = (32 * 1024) - 1;

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

#include <textserv.h>

class txtWinHost : public ITextHost
{
public:
	txtWinHost();
	BOOL init(richEditUI *re , const CREATESTRUCT *pcs);
	virtual ~txtWinHost();

	ITextServices* getTextServices(void) { return pserv; }
	void setClientRect(RECT *prc);
	RECT* getClientRect() { return &rcClient; }
	BOOL getWordWrap(void) { return fWordWrap; }
	void setWordWrap(BOOL fWordWrap);
	BOOL getReadOnly();
	void setReadOnly(BOOL fReadOnly);
	void setFont(HFONT hFont);
	void setColor(DWORD dwColor);
	SIZEL* getExtent();
	void setExtent(SIZEL *psizelExtent);
	void limitText(LONG nChars);
	BOOL isCaptured();

	BOOL getAllowBeep();
	void setAllowBeep(BOOL fAllowBeep);
	WORD getDefaultAlign();
	void setDefaultAlign(WORD wNewAlign);
	BOOL getRichTextFlag();
	void setRichTextFlag(BOOL fNew);
	LONG getDefaultLeftIndent();
	void setDefaultLeftIndent(LONG lNewIndent);
	BOOL setSaveSelection(BOOL fSaveSelection);
	BOOL getActiveState(void) { return fInplaceActive; }
	BOOL doSetCursor(RECT *prc, POINT *pt);
	void setTransparent(BOOL fTransparent);
	void getControlRect(LPRECT prc);
	LONG setAccelPos(LONG laccelpos);
	wchar_t setPasswordChar(wchar_t chPasswordChar);
	void setDisabled(BOOL fOn);
	LONG setSelBarWidth(LONG lSelBarWidth);
	BOOL getTimerState();

	void setCharFormat(CHARFORMAT2W &c);
	void setParaFormat(PARAFORMAT2 &p);
	// -----------------------------
	//	ITextServices interface
	// -----------------------------
	virtual HRESULT OnTxInPlaceDeactivate();
	virtual HRESULT OnTxInPlaceActivate(LPCRECT prcClient);

	// -----------------------------
	//	IUnknown interface
	// -----------------------------
	virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG _stdcall AddRef(void);
	virtual ULONG _stdcall Release(void);

	// -----------------------------
	//	ITextHost interface
	// -----------------------------
	virtual HDC TxGetDC();
	virtual INT TxReleaseDC(HDC hdc);
	virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
	virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void TxViewChange(BOOL fUpdate);
	virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
	virtual BOOL TxShowCaret(BOOL fShow);
	virtual BOOL TxSetCaretPos(INT x, INT y);
	virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
	virtual void TxKillTimer(UINT idTimer);
	virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	virtual void TxSetCapture(BOOL fCapture);
	virtual void TxSetFocus();
	virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
	virtual BOOL TxScreenToClient (LPPOINT lppt);
	virtual BOOL TxClientToScreen (LPPOINT lppt);
	virtual HRESULT TxActivate( LONG * plOldState );
	virtual HRESULT TxDeactivate( LONG lNewState );
	virtual HRESULT TxGetClientRect(LPRECT prc);
	virtual HRESULT TxGetViewInset(LPRECT prc);
	virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
	virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF TxGetSysColor(int nIndex);
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT TxGetMaxLength(DWORD *plength);
	virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
	virtual HRESULT TxGetPasswordChar(TCHAR *pch);
	virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
	virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
	virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
	virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
	virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	virtual HRESULT TxNotify(DWORD iNotify, void *pv);
	virtual HIMC TxImmGetContext(void);
	virtual void TxImmReleaseContext(HIMC himc);
	virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

private:
	richEditUI *m_re;
	ULONG	cRefs;					// Reference Count
	ITextServices	*pserv;		    // pointer to Text Services object
	// Properties

	DWORD		dwStyle;				// style bits

	unsigned	fEnableAutoWordSel	:1;	// enable Word style auto word selection?
	unsigned	fWordWrap			:1;	// Whether control should word wrap
	unsigned	fAllowBeep			:1;	// Whether beep is allowed
	unsigned	fRich				:1;	// Whether control is rich text
	unsigned	fSaveSelection		:1;	// Whether to save the selection when inactive
	unsigned	fInplaceActive		:1; // Whether control is inplace active
	unsigned	fTransparent		:1; // Whether control is transparent
	unsigned	fTimer				:1;	// A timer is set
	unsigned    fCaptured           :1;

	LONG		lSelBarWidth;			// Width of the selection bar
	LONG  		cchTextMost;			// maximum text size
	DWORD		dwEventMask;			// DoEvent mask to pass on to parent window
	LONG		icf;
	LONG		ipf;
	RECT		rcClient;				// Client Rect for this control
	SIZEL		sizelExtent;			// Extent array
	CHARFORMAT2W cf;					// Default character format
	PARAFORMAT2	pf;					    // Default paragraph format
	LONG		laccelpos;				// Accelerator position
	wchar_t		chPasswordChar;		    // Password character
};

// Convert Pixels on the X axis to Himetric
LONG dXtoHimetricX(LONG dx, LONG xPerInch)
{
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG dYtoHimetricY(LONG dy, LONG yPerInch)
{
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

HRESULT initDefaultCharFormat(richEditUI* re, CHARFORMAT2W* pcf, HFONT hfont) 
{
	memset(pcf, 0, sizeof(CHARFORMAT2W));
	LOGFONT lf;
	if( !hfont )
		hfont = re->getManager()->getFont(re->getFont());
	::GetObject(hfont, sizeof(LOGFONT), &lf);

	DWORD dwColor = re->getTextColor();
	pcf->cbSize = sizeof(CHARFORMAT2W);
	pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
	LONG yPixPerInch = GetDeviceCaps(re->getManager()->getPaintDC(), LOGPIXELSY);
	pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
	pcf->yOffset = 0;
	pcf->dwEffects = 0;
	pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
	if(lf.lfWeight >= FW_BOLD)
		pcf->dwEffects |= CFE_BOLD;
	if(lf.lfItalic)
		pcf->dwEffects |= CFE_ITALIC;
	if(lf.lfUnderline)
		pcf->dwEffects |= CFE_UNDERLINE;
	pcf->bCharSet = lf.lfCharSet;
	pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
	_tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
	//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
	MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

	return S_OK;
}

HRESULT initDefaultParaFormat(richEditUI* re, PARAFORMAT2* ppf) 
{	
	memset(ppf, 0, sizeof(PARAFORMAT2));
	ppf->cbSize = sizeof(PARAFORMAT2);
	ppf->dwMask = PFM_ALL;
	ppf->wAlignment = PFA_LEFT;
	ppf->cTabCount = 1;
	ppf->rgxTabs[0] = lDefaultTab;

	return S_OK;
}

HRESULT createHost(richEditUI *re, const CREATESTRUCT *pcs, txtWinHost **pptec)
{
	HRESULT hr = E_FAIL;
	//GdiSetBatchLimit(1);

	txtWinHost *phost = new txtWinHost();
	if(phost)
	{
		if (phost->init(re, pcs))
		{
			*pptec = phost;
			hr = S_OK;
		}
	}

	if (FAILED(hr))
	{
		delete phost;
	}

	return TRUE;
}

txtWinHost::txtWinHost() : m_re(NULL)
{
	::ZeroMemory(&cRefs, sizeof(txtWinHost) - offsetof(txtWinHost, cRefs));
	cchTextMost = cInitTextMax;
	laccelpos = -1;
}

txtWinHost::~txtWinHost()
{
	pserv->OnTxInPlaceDeactivate();
	pserv->Release();
}

////////////////////// Create/Init/Destruct Commands ///////////////////////

BOOL txtWinHost::init(richEditUI *re, const CREATESTRUCT *pcs)
{
	IUnknown *pUnk;
	HRESULT hr;

	m_re = re;
	// Initialize Reference count
	cRefs = 1;

	// Create and cache CHARFORMAT for this control
	if(FAILED(initDefaultCharFormat(re, &cf, NULL)))
		goto err;

	// Create and cache PARAFORMAT for this control
	if(FAILED(initDefaultParaFormat(re, &pf)))
		goto err;

	// edit controls created without a window are multiline by default
	// so that paragraph formats can be
	dwStyle = ES_MULTILINE;

	// edit controls are rich by default
	fRich = re->isRich();

	cchTextMost = re->getLimitText();

	if (pcs )
	{
		dwStyle = pcs->style;

		if ( !(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) )
		{
			fWordWrap = TRUE;
		}
	}

	if( !(dwStyle & ES_LEFT) )
	{
		if(dwStyle & ES_CENTER)
			pf.wAlignment = PFA_CENTER;
		else if(dwStyle & ES_RIGHT)
			pf.wAlignment = PFA_RIGHT;
	}

	fInplaceActive = TRUE;

	// Create Text Services component
	if(FAILED(CreateTextServices(NULL, this, &pUnk)))
		goto err;

	hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

	// Whether the previous call succeeded or failed we are done
	// with the private interface.
	pUnk->Release();

	if(FAILED(hr))
	{
		goto err;
	}

	// Set window text
	if(pcs && pcs->lpszName)
	{
#ifdef _UNICODE		
		if(FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName)))
			goto err;
#else
		size_t iLen = _tcslen(pcs->lpszName);
		LPWSTR lpText = new wchar_t[iLen + 1];
		::ZeroMemory(lpText, (iLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
		if(FAILED(pserv->TxSetText((LPWSTR)lpText))) {
			delete[] lpText;
			goto err;
		}
		delete[] lpText;
#endif
	}

	return TRUE;

err:
	return FALSE;
}

/////////////////////////////////  IUnknown ////////////////////////////////


HRESULT txtWinHost::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) 
		|| IsEqualIID(riid, IID_ITextHost)) 
	{
		AddRef();
		*ppvObject = (ITextHost *) this;
		hr = S_OK;
	}

	return hr;
}

ULONG txtWinHost::AddRef(void)
{
	return ++cRefs;
}

ULONG txtWinHost::Release(void)
{
	ULONG c_Refs = --cRefs;

	if (c_Refs == 0)
	{
		delete this;
	}

	return c_Refs;
}

/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC txtWinHost::TxImmGetContext(void)
{
	return NULL;
}

void txtWinHost::TxImmReleaseContext(HIMC himc)
{
	//::ImmReleaseContext( hwnd, himc );
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC txtWinHost::TxGetDC()
{
	return m_re->getManager()->getPaintDC();
}

int txtWinHost::TxReleaseDC(HDC hdc)
{
	return 1;
}

BOOL txtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
	scrollBarUI* pVerticalScrollBar = m_re->getVerticalScrollBar();
	scrollBarUI* pHorizontalScrollBar = m_re->getHorizontalScrollBar();
	if( fnBar == SB_VERT && pVerticalScrollBar ) {
		pVerticalScrollBar->setVisible(fShow == TRUE);
	}
	else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
		pHorizontalScrollBar->setVisible(fShow == TRUE);
	}
	else if( fnBar == SB_BOTH ) {
		if( pVerticalScrollBar ) pVerticalScrollBar->setVisible(fShow == TRUE);
		if( pHorizontalScrollBar ) pHorizontalScrollBar->setVisible(fShow == TRUE);
	}
	return TRUE;
}

BOOL txtWinHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
	if( fuSBFlags == SB_VERT ) {
		m_re->enableScrollBar(true, m_re->getHorizontalScrollBar() != NULL);
		m_re->getVerticalScrollBar()->setVisible(fuArrowflags != ESB_DISABLE_BOTH);
	}
	else if( fuSBFlags == SB_HORZ ) {
		m_re->enableScrollBar(m_re->getVerticalScrollBar() != NULL, true);
		m_re->getHorizontalScrollBar()->setVisible(fuArrowflags != ESB_DISABLE_BOTH);
	}
	else if( fuSBFlags == SB_BOTH ) {
		m_re->enableScrollBar(true, true);
		m_re->getVerticalScrollBar()->setVisible(fuArrowflags != ESB_DISABLE_BOTH);
		m_re->getHorizontalScrollBar()->setVisible(fuArrowflags != ESB_DISABLE_BOTH);
	}
	return TRUE;
}

BOOL txtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	scrollBarUI* pVerticalScrollBar = m_re->getVerticalScrollBar();
	scrollBarUI* pHorizontalScrollBar = m_re->getHorizontalScrollBar();
	if( fnBar == SB_VERT && pVerticalScrollBar ) {
		if( nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0 ) {
			pVerticalScrollBar->setVisible(false);
		}
		else {
			pVerticalScrollBar->setVisible(true);
			pVerticalScrollBar->setScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
		}
	}
	else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
		if( nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0 ) {
			pHorizontalScrollBar->setVisible(false);
		}
		else {
			pHorizontalScrollBar->setVisible(true);
			pHorizontalScrollBar->setScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
		}   
	}
	return TRUE;
}

BOOL txtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
	scrollBarUI* pVerticalScrollBar = m_re->getVerticalScrollBar();
	scrollBarUI* pHorizontalScrollBar = m_re->getHorizontalScrollBar();
	if( fnBar == SB_VERT && pVerticalScrollBar ) {
		pVerticalScrollBar->setScrollPos(nPos);
	}
	else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
		pHorizontalScrollBar->setScrollPos(nPos);
	}
	return TRUE;
}

void txtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	if( prc == NULL ) {
		m_re->getManager()->invalidate(rcClient);
		return;
	}
	RECT rc = *prc;
	m_re->getManager()->invalidate(rc);
}

void txtWinHost::TxViewChange(BOOL fUpdate) 
{
	if( m_re->onTxViewChanged() ) m_re->invalidate();
}

BOOL txtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	return ::CreateCaret(m_re->getManager()->getPaintWindow(), hbmp, xWidth, yHeight);
}

BOOL txtWinHost::TxShowCaret(BOOL fShow)
{
	if(fShow)
		return ::ShowCaret(m_re->getManager()->getPaintWindow());
	else
		return ::HideCaret(m_re->getManager()->getPaintWindow());
}

BOOL txtWinHost::TxSetCaretPos(INT x, INT y)
{
	return ::SetCaretPos(x, y);
}

BOOL txtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	fTimer = TRUE;
	return m_re->getManager()->setTimer(m_re, idTimer, uTimeout) == TRUE;
}

void txtWinHost::TxKillTimer(UINT idTimer)
{
	m_re->getManager()->killTimer(m_re, idTimer);
	fTimer = FALSE;
}

void txtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
	return;
}

void txtWinHost::TxSetCapture(BOOL fCapture)
{
	if (fCapture) m_re->getManager()->setCapture();
	else m_re->getManager()->releaseCapture();
	fCaptured = fCapture;
}

void txtWinHost::TxSetFocus()
{
	m_re->setFocus();
}

void txtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
	::SetCursor(hcur);
}

BOOL txtWinHost::TxScreenToClient(LPPOINT lppt)
{
	return ::ScreenToClient(m_re->getManager()->getPaintWindow(), lppt);	
}

BOOL txtWinHost::TxClientToScreen(LPPOINT lppt)
{
	return ::ClientToScreen(m_re->getManager()->getPaintWindow(), lppt);
}

HRESULT txtWinHost::TxActivate(LONG *plOldState)
{
	return S_OK;
}

HRESULT txtWinHost::TxDeactivate(LONG lNewState)
{
	return S_OK;
}

HRESULT txtWinHost::TxGetClientRect(LPRECT prc)
{
	*prc = rcClient;
	getControlRect(prc);
	return NOERROR;
}

HRESULT txtWinHost::TxGetViewInset(LPRECT prc) 
{
	prc->left = prc->right = prc->top = prc->bottom = 0;
	return NOERROR;	
}

HRESULT txtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	*ppCF = &cf;
	return NOERROR;
}

HRESULT txtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = &pf;
	return NOERROR;
}

COLORREF txtWinHost::TxGetSysColor(int nIndex) 
{
	return ::GetSysColor(nIndex);
}

HRESULT txtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	*pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
	return NOERROR;
}

HRESULT txtWinHost::TxGetMaxLength(DWORD *pLength)
{
	*pLength = cchTextMost;
	return NOERROR;
}

HRESULT txtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
	*pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
		ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

	return NOERROR;
}

HRESULT txtWinHost::TxGetPasswordChar(TCHAR *pch)
{
#ifdef _UNICODE
	*pch = chPasswordChar;
#else
	::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
	return NOERROR;
}

HRESULT txtWinHost::TxGetAcceleratorPos(LONG *pcp)
{
	*pcp = laccelpos;
	return S_OK;
} 										   

HRESULT txtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
	return S_OK;
}

HRESULT txtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
	return S_OK;
}

HRESULT txtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
	DWORD dwProperties = 0;

	if (fRich)
	{
		dwProperties = TXTBIT_RICHTEXT;
	}

	if (dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}

	if (dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

	if (fEnableAutoWordSel)
	{
		dwProperties |= TXTBIT_AUTOWORDSEL;
	}

	if (fWordWrap)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}

	if (fAllowBeep)
	{
		dwProperties |= TXTBIT_ALLOWBEEP;
	}

	if (fSaveSelection)
	{
		dwProperties |= TXTBIT_SAVESELECTION;
	}

	*pdwBits = dwProperties & dwMask; 
	return NOERROR;
}


HRESULT txtWinHost::TxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_REQUESTRESIZE ) {
		RECT rc;
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		getControlRect(&rc);
		rc.bottom = rc.top + preqsz->rc.bottom;
		rc.right  = rc.left + preqsz->rc.right;
		setClientRect(&rc);
		return S_OK;
	}
	m_re->onTxNotify(iNotify, pv);
	return S_OK;
}

HRESULT txtWinHost::TxGetExtent(LPSIZEL lpExtent)
{
	*lpExtent = sizelExtent;
	return S_OK;
}

HRESULT	txtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
	*plSelBarWidth = lSelBarWidth;
	return S_OK;
}

void txtWinHost::setWordWrap(BOOL fWordWrap)
{
	fWordWrap = fWordWrap;
	pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL txtWinHost::getReadOnly()
{
	return (dwStyle & ES_READONLY) != 0;
}

void txtWinHost::setReadOnly(BOOL fReadOnly)
{
	if (fReadOnly)
	{
		dwStyle |= ES_READONLY;
	}
	else
	{
		dwStyle &= ~ES_READONLY;
	}

	pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, 
		fReadOnly ? TXTBIT_READONLY : 0);
}

void txtWinHost::setFont(HFONT hFont) 
{
	if( hFont == NULL ) return;
	LOGFONT lf;
	::GetObject(hFont, sizeof(LOGFONT), &lf);
	LONG yPixPerInch = ::GetDeviceCaps(m_re->getManager()->getPaintDC(), LOGPIXELSY);
	cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
	if(lf.lfWeight >= FW_BOLD)
		cf.dwEffects |= CFE_BOLD;
	if(lf.lfItalic)
		cf.dwEffects |= CFE_ITALIC;
	if(lf.lfUnderline)
		cf.dwEffects |= CFE_UNDERLINE;
	cf.bCharSet = lf.lfCharSet;
	cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
	_tcscpy(cf.szFaceName, lf.lfFaceName);
#else
	//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
	MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
#endif

	pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

void txtWinHost::setColor(DWORD dwColor)
{
	cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
	pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

SIZEL* txtWinHost::getExtent() 
{
	return &sizelExtent;
}

void txtWinHost::setExtent(SIZEL *psizelExtent) 
{ 
	sizelExtent = *psizelExtent; 
	pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void txtWinHost::limitText(LONG nChars)
{
	cchTextMost = nChars;
	if( cchTextMost <= 0 ) cchTextMost = cInitTextMax;
	pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

BOOL txtWinHost::isCaptured()
{
	return fCaptured;
}

BOOL txtWinHost::getAllowBeep()
{
	return fAllowBeep;
}

void txtWinHost::setAllowBeep(BOOL fAllowBeep)
{
	fAllowBeep = fAllowBeep;

	pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
		fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD txtWinHost::getDefaultAlign()
{
	return pf.wAlignment;
}

void txtWinHost::setDefaultAlign(WORD wNewAlign)
{
	pf.wAlignment = wNewAlign;

	// Notify control of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL txtWinHost::getRichTextFlag()
{
	return fRich;
}

void txtWinHost::setRichTextFlag(BOOL fNew)
{
	fRich = fNew;

	pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
		fNew ? TXTBIT_RICHTEXT : 0);
}

LONG txtWinHost::getDefaultLeftIndent()
{
	return pf.dxOffset;
}

void txtWinHost::setDefaultLeftIndent(LONG lNewIndent)
{
	pf.dxOffset = lNewIndent;

	pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void txtWinHost::setClientRect(RECT *prc) 
{
	rcClient = *prc;

	LONG xPerInch = ::GetDeviceCaps(m_re->getManager()->getPaintDC(), LOGPIXELSX); 
	LONG yPerInch =	::GetDeviceCaps(m_re->getManager()->getPaintDC(), LOGPIXELSY); 
	sizelExtent.cx = dXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
	sizelExtent.cy = dYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);

	pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL txtWinHost::setSaveSelection(BOOL f_SaveSelection)
{
	BOOL fResult = f_SaveSelection;

	fSaveSelection = f_SaveSelection;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
		fSaveSelection ? TXTBIT_SAVESELECTION : 0);

	return fResult;		
}

HRESULT	txtWinHost::OnTxInPlaceDeactivate()
{
	HRESULT hr = pserv->OnTxInPlaceDeactivate();

	if (SUCCEEDED(hr))
	{
		fInplaceActive = FALSE;
	}

	return hr;
}

HRESULT	txtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
	fInplaceActive = TRUE;

	HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

	if (FAILED(hr))
	{
		fInplaceActive = FALSE;
	}

	return hr;
}

BOOL txtWinHost::doSetCursor(RECT *prc, POINT *pt)
{
	RECT rc = prc ? *prc : rcClient;

	// Is this in our rectangle?
	if (PtInRect(&rc, *pt))
	{
		RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;
		pserv->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  m_re->getManager()->getPaintDC(),
			NULL, prcClient, pt->x, pt->y);

		return TRUE;
	}

	return FALSE;
}

void txtWinHost::getControlRect(LPRECT prc)
{
	prc->top = rcClient.top;
	prc->bottom = rcClient.bottom;
	prc->left = rcClient.left;
	prc->right = rcClient.right;
}

void txtWinHost::setTransparent(BOOL f_Transparent)
{
	fTransparent = f_Transparent;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG txtWinHost::setAccelPos(LONG l_accelpos)
{
	LONG laccelposOld = l_accelpos;

	laccelpos = l_accelpos;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

	return laccelposOld;
}

wchar_t txtWinHost::setPasswordChar(wchar_t ch_PasswordChar)
{
	wchar_t chOldPasswordChar = chPasswordChar;

	chPasswordChar = ch_PasswordChar;

	// notify text services of property change
	pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
		(chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

	return chOldPasswordChar;
}

void txtWinHost::setDisabled(BOOL fOn)
{
	cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
	cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

	if( !fOn )
	{
		cf.dwEffects &= ~CFE_DISABLED;
	}

	pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
		TXTBIT_CHARFORMATCHANGE);
}

LONG txtWinHost::setSelBarWidth(LONG l_SelBarWidth)
{
	LONG lOldSelBarWidth = lSelBarWidth;

	lSelBarWidth = l_SelBarWidth;

	if (lSelBarWidth)
	{
		dwStyle |= ES_SELECTIONBAR;
	}
	else
	{
		dwStyle &= (~ES_SELECTIONBAR);
	}

	pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

	return lOldSelBarWidth;
}

BOOL txtWinHost::getTimerState()
{
	return fTimer;
}

void txtWinHost::setCharFormat(CHARFORMAT2W &c)
{
	cf = c;
}

void txtWinHost::setParaFormat(PARAFORMAT2 &p)
{
	pf = p;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

richEditUI::richEditUI() : m_pTwh(NULL), m_bVScrollBarFixing(false), m_bWantTab(true), m_bWantReturn(true), 
	m_bWantCtrlReturn(true), m_bRich(true), m_bReadOnly(false), m_bWordWrap(false), m_dwTextColor(0), m_iFont(-1), 
	m_iLimitText(cInitTextMax), m_lTwhStyle(ES_MULTILINE)
{
}

richEditUI::~richEditUI()
{
	if( m_pTwh ) {
		m_pTwh->Release();
		m_pManager->removeMessageFilter(this);
	}
}

PCWSTR richEditUI::getClass() const
{
	return L"RichEditUI";
}

LPVOID richEditUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"RichEdit") == 0 ) return static_cast<richEditUI*>(this);
	return containerUI::getInterface(pstrName);
}

UINT richEditUI::getControlFlags() const
{
	if( !isEnabled() ) return controlUI::getControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

bool richEditUI::isWantTab()
{
	return m_bWantTab;
}

void richEditUI::setWantTab(bool bWantTab)
{
	m_bWantTab = bWantTab;
}

bool richEditUI::isWantReturn()
{
	return m_bWantReturn;
}

void richEditUI::setWantReturn(bool bWantReturn)
{
	m_bWantReturn = bWantReturn;
}

bool richEditUI::isWantCtrlReturn()
{
	return m_bWantCtrlReturn;
}

void richEditUI::setWantCtrlReturn(bool bWantCtrlReturn)
{
	m_bWantCtrlReturn = bWantCtrlReturn;
}

bool richEditUI::isRich()
{
	return m_bRich;
}

void richEditUI::setRich(bool bRich)
{
	m_bRich = bRich;
	if( m_pTwh ) m_pTwh->setRichTextFlag(bRich);
}

bool richEditUI::isReadOnly()
{
	return m_bReadOnly;
}

void richEditUI::setReadOnly(bool bReadOnly)
{
	m_bReadOnly = bReadOnly;
	if( m_pTwh ) m_pTwh->setReadOnly(bReadOnly);
}

bool richEditUI::getWordWrap()
{
	return m_bWordWrap;
}

void richEditUI::setWordWrap(bool bWordWrap)
{
	m_bWordWrap = bWordWrap;
	if( m_pTwh ) m_pTwh->setWordWrap(bWordWrap);
}

int richEditUI::getFont()
{
	return m_iFont;
}

void richEditUI::setFont(int index)
{
	m_iFont = index;
	if( m_pTwh ) {
		m_pTwh->setFont(getManager()->getFont(m_iFont));
	}
}

void richEditUI::setFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	if( m_pTwh ) {
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcscpy(lf.lfFaceName, pStrFontName);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -nSize;
		if( bBold ) lf.lfWeight += FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return;
		m_pTwh->setFont(hFont);
		::DeleteObject(hFont);
	}
}

LONG richEditUI::getWinStyle()
{
	return m_lTwhStyle;
}

void richEditUI::setWinStyle(LONG lStyle)
{
	m_lTwhStyle = lStyle;
}

DWORD richEditUI::getTextColor()
{
	return m_dwTextColor;
}

void richEditUI::setTextColor(DWORD dwTextColor)
{
	m_dwTextColor = dwTextColor;
	if( m_pTwh ) {
		m_pTwh->setColor(dwTextColor);
	}
}

int richEditUI::getLimitText()
{
	return m_iLimitText;
}

void richEditUI::setLimitText(int iChars)
{
	m_iLimitText = iChars;
	if( m_pTwh ) {
		m_pTwh->limitText(m_iLimitText);
	}
}

long richEditUI::getTextLength(DWORD dwFlags) const
{
	GETTEXTLENGTHEX textLenEx;
	textLenEx.flags = dwFlags;
#ifdef _UNICODE
	textLenEx.codepage = 1200;
#else
	textLenEx.codepage = CP_ACP;
#endif
	LRESULT lResult;
	txSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
	return (long)lResult;
}

std::wstring richEditUI::getText() const
{
	long lLen = getTextLength(GTL_DEFAULT);
	PWSTR lpText = NULL;
	GETTEXTEX gt;
	gt.flags = GT_DEFAULT;
#ifdef _UNICODE
	gt.cb = sizeof(wchar_t) * (lLen + 1) ;
	gt.codepage = 1200;
	lpText = new wchar_t[lLen + 1];
	::ZeroMemory(lpText, (lLen + 1) * sizeof(wchar_t));
#else
	gt.cb = sizeof(TCHAR) * lLen * 2 + 1;
	gt.codepage = CP_ACP;
	lpText = new TCHAR[lLen * 2 + 1];
	::ZeroMemory(lpText, (lLen * 2 + 1) * sizeof(TCHAR));
#endif
	gt.lpDefaultChar = NULL;
	gt.lpUsedDefChar = NULL;
	txSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
	std::wstring sText(lpText);
	delete[] lpText;
	return sText;
}

void richEditUI::setText(PCWSTR pstrText)
{
	m_sText = pstrText;
	if( !m_pTwh ) return;
	setSel(0, -1);
	replaceSel(pstrText, FALSE);
}

bool richEditUI::getModify() const
{ 
	if( !m_pTwh ) return false;
	LRESULT lResult;
	txSendMessage(EM_GETMODIFY, 0, 0, &lResult);
	return (BOOL)lResult == TRUE;
}

void richEditUI::setModify(bool bModified) const
{ 
	txSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

void richEditUI::getSel(CHARRANGE &cr) const
{ 
	txSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
}

void richEditUI::getSel(long& nStartChar, long& nEndChar) const
{
	CHARRANGE cr;
	txSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
	nStartChar = cr.cpMin;
	nEndChar = cr.cpMax;
}

int richEditUI::setSel(CHARRANGE &cr)
{ 
	LRESULT lResult;
	txSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
	return (int)lResult;
}

int richEditUI::setSel(long nStartChar, long nEndChar)
{
	CHARRANGE cr;
	cr.cpMin = nStartChar;
	cr.cpMax = nEndChar;
	LRESULT lResult;
	txSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
	return (int)lResult;
}

void richEditUI::replaceSel(PCWSTR lpszNewText, bool bCanUndo)
{
#ifdef _UNICODE		
	txSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
#else
	int iLen = _tcslen(lpszNewText);
	LPWSTR lpText = new wchar_t[iLen + 1];
	::ZeroMemory(lpText, (iLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpszNewText, -1, (LPWSTR)lpText, iLen) ;
	txSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpText, 0); 
	delete[] lpText;
#endif
}

void richEditUI::replaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
	txSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

std::wstring richEditUI::getSelText() const
{
	if( !m_pTwh ) return std::wstring();
	CHARRANGE cr;
	cr.cpMin = cr.cpMax = 0;
	txSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
	LPWSTR lpText = NULL;
	lpText = new wchar_t[cr.cpMax - cr.cpMin + 1];
	::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(wchar_t));
	txSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
	std::wstring sText;
	sText = (LPCWSTR)lpText;
	delete[] lpText;
	return sText;
}

int richEditUI::setSelAll()
{
	return setSel(0, -1);
}

int richEditUI::setSelNone()
{
	return setSel(-1, 0);
}

bool richEditUI::getZoom(int& nNum, int& nDen) const
{
	LRESULT lResult;
	txSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
	return (BOOL)lResult == TRUE;
}

bool richEditUI::setZoom(int nNum, int nDen)
{
	if (nNum < 0 || nNum > 64) return false;
	if (nDen < 0 || nDen > 64) return false;
	LRESULT lResult;
	txSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
	return (BOOL)lResult == TRUE;
}

bool richEditUI::setZoomOff()
{
	LRESULT lResult;
	txSendMessage(EM_SETZOOM, 0, 0, &lResult);
	return (BOOL)lResult == TRUE;
}

WORD richEditUI::getSelectionType() const
{
	LRESULT lResult;
	txSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
	return (WORD)lResult;
}

bool richEditUI::getAutoURLDetect() const
{
	LRESULT lResult;
	txSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
	return (BOOL)lResult == TRUE;
}

bool richEditUI::setAutoURLDetect(bool bAutoDetect)
{
	LRESULT lResult;
	txSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
	return (BOOL)lResult == FALSE;
}

DWORD richEditUI::getEventMask() const
{
	LRESULT lResult;
	txSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
	return (DWORD)lResult;
}

DWORD richEditUI::setEventMask(DWORD dwEventMask)
{
	LRESULT lResult;
	txSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
	return (DWORD)lResult;
}

std::wstring richEditUI::getTextRange(long nStartChar, long nEndChar) const
{
	TEXTRANGEW tr = { 0 };
	tr.chrg.cpMin = nStartChar;
	tr.chrg.cpMax = nEndChar;
	LPWSTR lpText = NULL;
	lpText = new wchar_t[nEndChar - nStartChar + 1];
	::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(wchar_t));
	tr.lpstrText = lpText;
	txSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
	std::wstring sText;
	sText = (LPCWSTR)lpText;
	delete[] lpText;
	return sText;
}

void richEditUI::hideSelection(bool bHide, bool bChangeStyle)
{
	txSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
}

void richEditUI::scrollCaret()
{
	txSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

int richEditUI::insertText(long nInsertAfterChar, PCWSTR lpstrText, bool bCanUndo)
{
	int nRet = setSel(nInsertAfterChar, nInsertAfterChar);
	replaceSel(lpstrText, bCanUndo);
	return nRet;
}

int richEditUI::appendText(PCWSTR lpstrText, bool bCanUndo)
{
	int nRet = setSel(-1, -1);
	replaceSel(lpstrText, bCanUndo);
	return nRet;
}

DWORD richEditUI::getDefaultCharFormat(CHARFORMAT2 &cf) const
{
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
	return (DWORD)lResult;
}

bool richEditUI::setDefaultCharFormat(CHARFORMAT2 &cf)
{
	if( !m_pTwh ) return false;
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
	if( (BOOL)lResult == TRUE ) {
		CHARFORMAT2W cfw;
		cfw.cbSize = sizeof(CHARFORMAT2W);
		txSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
		m_pTwh->setCharFormat(cfw);
		return true;
	}
	return false;
}

DWORD richEditUI::getSelectionCharFormat(CHARFORMAT2 &cf) const
{
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
	return (DWORD)lResult;
}

bool richEditUI::setSelectionCharFormat(CHARFORMAT2 &cf)
{
	if( !m_pTwh ) return false;
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
	return (BOOL)lResult == TRUE;
}

bool richEditUI::setWordCharFormat(CHARFORMAT2 &cf)
{
	if( !m_pTwh ) return false;
	cf.cbSize = sizeof(CHARFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
	return (BOOL)lResult == TRUE;
}

DWORD richEditUI::getParaFormat(PARAFORMAT2 &pf) const
{
	pf.cbSize = sizeof(PARAFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
	return (DWORD)lResult;
}

bool richEditUI::setParaFormat(PARAFORMAT2 &pf)
{
	if( !m_pTwh ) return false;
	pf.cbSize = sizeof(PARAFORMAT2);
	LRESULT lResult;
	txSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
	if( (BOOL)lResult == TRUE ) {
		m_pTwh->setParaFormat(pf);
		return true;
	}
	return false;
}

bool richEditUI::redo()
{ 
	if( !m_pTwh ) return false;
	LRESULT lResult;
	txSendMessage(EM_REDO, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}

bool richEditUI::undo()
{ 
	if( !m_pTwh ) return false;
	LRESULT lResult;
	txSendMessage(EM_UNDO, 0, 0, &lResult);
	return (BOOL)lResult == TRUE; 
}

void richEditUI::clear()
{ 
	txSendMessage(WM_CLEAR, 0, 0, 0); 
}

void richEditUI::copy()
{ 
	txSendMessage(WM_COPY, 0, 0, 0); 
}

void richEditUI::cut()
{ 
	txSendMessage(WM_CUT, 0, 0, 0); 
}

void richEditUI::paste()
{ 
	txSendMessage(WM_PASTE, 0, 0, 0); 
}

int richEditUI::getLineCount() const
{ 
	if( !m_pTwh ) return 0;
	LRESULT lResult;
	txSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
	return (int)lResult; 
}

std::wstring richEditUI::getLine(int nIndex, int nMaxLength) const
{
	LPWSTR lpText = NULL;
	lpText = new wchar_t[nMaxLength + 1];
	::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(wchar_t));
	*(LPWORD)lpText = (WORD)nMaxLength;
	txSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
	std::wstring sText;
	sText = (LPCWSTR)lpText;
	delete[] lpText;
	return sText;
}

int richEditUI::lineIndex(int nLine) const
{
	LRESULT lResult;
	txSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
	return (int)lResult;
}

int richEditUI::lineLength(int nLine) const
{
	LRESULT lResult;
	txSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
	return (int)lResult;
}

bool richEditUI::lineScroll(int nLines, int nChars)
{
	LRESULT lResult;
	txSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
	return (BOOL)lResult == TRUE;
}

point richEditUI::getCharPos(long lChar) const
{ 
	point pt; 
	txSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0); 
	return pt;
}

long richEditUI::lineFromChar(long nIndex) const
{ 
	if( !m_pTwh ) return 0L;
	LRESULT lResult;
	txSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
	return (long)lResult;
}

point richEditUI::posFromChar(UINT nChar) const
{ 
	POINTL pt; 
	txSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0); 
	return point(pt.x, pt.y); 
}

int richEditUI::charFromPos(point pt) const
{ 
	POINTL ptl = {pt.x, pt.y}; 
	if( !m_pTwh ) return 0;
	LRESULT lResult;
	txSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
	return (int)lResult; 
}

void richEditUI::emptyUndoBuffer()
{ 
	txSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
}

UINT richEditUI::setUndoLimit(UINT nLimit)
{ 
	if( !m_pTwh ) return 0;
	LRESULT lResult;
	txSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
	return (UINT)lResult; 
}

long richEditUI::streamIn(int nFormat, EDITSTREAM &es)
{ 
	if( !m_pTwh ) return 0L;
	LRESULT lResult;
	txSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
	return (long)lResult;
}

long richEditUI::streamOut(int nFormat, EDITSTREAM &es)
{ 
	if( !m_pTwh ) return 0L;
	LRESULT lResult;
	txSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
	return (long)lResult; 
}

void richEditUI::doInit()
{
	CREATESTRUCT cs;
	cs.style = m_lTwhStyle;
	cs.x = 0;
	cs.y = 0;
	cs.cy = 0;
	cs.cx = 0;
	cs.lpszName = m_sText.data();
	createHost(this, &cs, &m_pTwh);
	if( m_pTwh ) {
		m_pTwh->setTransparent(TRUE);
		LRESULT lResult;
		m_pTwh->getTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
		m_pTwh->OnTxInPlaceActivate(NULL);
		m_pManager->addMessageFilter(this);
	}
}

HRESULT richEditUI::txSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
	if( m_pTwh ) {
		if( msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN ) {
			if( !m_bWantReturn || (::GetKeyState(VK_CONTROL) < 0 && !m_bWantCtrlReturn) ) {
				if( m_pManager != NULL ) m_pManager->sendNotify((controlUI*)this, L"return");
				return S_OK;
			}
		}
		return m_pTwh->getTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
	}
	return S_FALSE;
}

IDropTarget* richEditUI::getTxDropTarget()
{
	IDropTarget *pdt = NULL;
	if( m_pTwh->getTextServices()->TxGetDropTarget(&pdt) == NOERROR ) return pdt;
	return NULL;
}

bool richEditUI::onTxViewChanged()
{
	return true;
}

void richEditUI::onTxNotify(DWORD iNotify, void *pv)
{
}

// 多行非rich格式的richedit有一个滚动条bug，在最后一行是空行时，LineDown和SetScrollPos无法滚动到最后
// 引入iPos就是为了修正这个bug
void richEditUI::setScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		int iLastScrollPos = m_pVerticalScrollBar->getScrollPos();
		m_pVerticalScrollBar->setScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->getScrollPos() - iLastScrollPos;
	}
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		int iLastScrollPos = m_pHorizontalScrollBar->getScrollPos();
		m_pHorizontalScrollBar->setScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->getScrollPos() - iLastScrollPos;
	}
	if( cy != 0 ) {
		int iPos = 0;
		if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) 
			iPos = m_pVerticalScrollBar->getScrollPos();
		WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pVerticalScrollBar->getScrollPos());
		txSendMessage(WM_VSCROLL, wParam, 0L, 0);
		if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
			if( cy > 0 && m_pVerticalScrollBar->getScrollPos() <= iPos )
				m_pVerticalScrollBar->setScrollPos(iPos);
		}
	}
	if( cx != 0 ) {
		WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pHorizontalScrollBar->getScrollPos());
		txSendMessage(WM_HSCROLL, wParam, 0L, 0);
	}
}

void richEditUI::lineUp()
{
	txSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
}

void richEditUI::lineDown()
{
	int iPos = 0;
	if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) 
		iPos = m_pVerticalScrollBar->getScrollPos();
	txSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
	if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		if( m_pVerticalScrollBar->getScrollPos() <= iPos )
			m_pVerticalScrollBar->setScrollPos(m_pVerticalScrollBar->getScrollRange());
	}
}

void richEditUI::pageUp()
{
	txSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
}

void richEditUI::pageDown()
{
	txSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
}

void richEditUI::homeUp()
{
	txSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
}

void richEditUI::endDown()
{
	txSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void richEditUI::lineLeft()
{
	txSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
}

void richEditUI::lineRight()
{
	txSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
}

void richEditUI::pageLeft()
{
	txSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
}

void richEditUI::pageRight()
{
	txSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
}

void richEditUI::homeLeft()
{
	txSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
}

void richEditUI::endRight()
{
	txSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
}

void richEditUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else controlUI::doEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETCURSOR && isEnabled() )
	{
		if( m_pTwh && m_pTwh->doSetCursor(NULL, &event.ptMouse) ) {
			return;
		}
	}
	if( event.Type == UIEVENT_SETFOCUS ) {
		if( m_pTwh ) {
			m_pTwh->OnTxInPlaceActivate(NULL);
			m_pTwh->getTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
		}
	}
	if( event.Type == UIEVENT_KILLFOCUS )  {
		if( m_pTwh ) {
			m_pTwh->OnTxInPlaceActivate(NULL);
			m_pTwh->getTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
		}
	}
	if( event.Type == UIEVENT_TIMER ) {
		if( m_pTwh ) {
			m_pTwh->getTextServices()->TxSendMessage(WM_TIMER, event.wParam, event.lParam, 0);
		} 
	}
	if( event.Type == UIEVENT_SCROLLWHEEL ) {
		if( (event.wKeyState & MK_CONTROL) != 0  ) {
			return;
		}
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) 
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE ) 
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP ) 
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		return;
	}
	if( event.Type > UIEVENT__KEYBEGIN && event.Type < UIEVENT__KEYEND )
	{
		return;
	}
	containerUI::doEvent(event);
}

SIZE richEditUI::estimateSize(SIZE szAvailable)
{
	//return CSize(m_rcItem); // 这种方式在第一次设置大小之后就大小不变了
	return containerUI::estimateSize(szAvailable);
}

void richEditUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	bool bVScrollBarVisiable = false;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		bVScrollBarVisiable = true;
		rc.right -= m_pVerticalScrollBar->getFixedWidth();
	}
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	}

	if( m_pTwh ) {
		m_pTwh->setClientRect(&rc);
		if( bVScrollBarVisiable && (!m_pVerticalScrollBar->isVisible() || m_bVScrollBarFixing) ) {
			LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->getFixedWidth();
			LONG lHeight = 0;
			SIZEL szExtent = { -1, -1 };
			m_pTwh->getTextServices()->TxGetNaturalSize(
				DVASPECT_CONTENT, 
				getManager()->getPaintDC(), 
				NULL,
				NULL,
				TXTNS_FITTOCONTENT,
				&szExtent,
				&lWidth,
				&lHeight);
			if( lHeight > rc.bottom - rc.top ) {
				m_pVerticalScrollBar->setVisible(true);
				m_pVerticalScrollBar->setScrollPos(0);
				m_bVScrollBarFixing = true;
			}
			else {
				if( m_bVScrollBarFixing ) {
					m_pVerticalScrollBar->setVisible(false);
					m_bVScrollBarFixing = false;
				}
			}
		}
	}

	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->isVisible() ) {
		RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->getFixedWidth(), rc.bottom};
		m_pVerticalScrollBar->setPos(rcScrollBarPos);
	}
	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->isVisible() ) {
		RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->getFixedHeight()};
		m_pHorizontalScrollBar->setPos(rcScrollBarPos);
	}

	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
	//	if( !pControl->IsVisible() ) continue;
	//	if( pControl->IsFloat() ) {
	//		SetFloatPos(it);
	//	}
	//	else {
	//		pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
	//	}
	//}
	int it = 0;
	for (std::vector<controlUI *>::const_iterator iter = m_items.cbegin(); iter != m_items.cend(); ++iter, ++it)
	{
		if(!(*iter)->isVisible()) continue;
		if((*iter)->isFloat())
			setFloatPos(it);
		else
			(*iter)->setPos(rc);
	}
}

void richEditUI::doPaint(HDC hDC, const RECT& rcPaint)
{
	RECT rcTemp = { 0 };
	if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

	renderClip clip;
	renderClip::generateClip(hDC, rcTemp, clip);
	controlUI::doPaint(hDC, rcPaint);

	if( m_pTwh ) {
		RECT rc;
		m_pTwh->getControlRect(&rc);
		// Remember wparam is actually the hdc and lparam is the update
		// rect because this message has been preprocessed by the window.
		m_pTwh->getTextServices()->TxDraw(
			DVASPECT_CONTENT,  		// Draw Aspect
			/*-1*/0,				// Lindex
			NULL,					// Info for drawing optimazation
			NULL,					// target device information
			hDC,			        // Draw device HDC
			NULL, 				   	// Target device HDC
			(RECTL*)&rc,			// Bounding client rectangle
			NULL, 		            // Clipping rectangle for metafiles
			(RECT*)&rcPaint,		// Update rectangle
			NULL, 	   				// Call back function
			NULL,					// Call back parameter
			0);				        // What view of the object
		if( m_bVScrollBarFixing ) {
			LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->getFixedWidth();
			LONG lHeight = 0;
			SIZEL szExtent = { -1, -1 };
			m_pTwh->getTextServices()->TxGetNaturalSize(
				DVASPECT_CONTENT, 
				getManager()->getPaintDC(), 
				NULL,
				NULL,
				TXTNS_FITTOCONTENT,
				&szExtent,
				&lWidth,
				&lHeight);
			if( lHeight <= rc.bottom - rc.top ) {
				needUpdate();
			}
		}
	}

	if( m_items.size() > 0 ) {
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

		if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
			for( int it = 0; it < m_items.size(); it++ ) {
				controlUI* pControl = static_cast<controlUI*>(m_items[it]);
				if( !pControl->isVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->getPos()) ) continue;
				if( pControl ->isFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->getPos()) ) continue;
					pControl->doPaint(hDC, rcPaint);
				}
			}
		}
		else {
			renderClip childClip;
			renderClip::generateClip(hDC, rcTemp, childClip);
			for( int it = 0; it < m_items.size(); it++ ) {
				controlUI* pControl = static_cast<controlUI*>(m_items[it]);
				if( !pControl->isVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->getPos()) ) continue;
				if( pControl ->isFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->getPos()) ) continue;
					renderClip::useOldClipBegin(hDC, childClip);
					pControl->doPaint(hDC, rcPaint);
					renderClip::useOldClipEnd(hDC, childClip);
				}
				else {
					if( !::IntersectRect(&rcTemp, &rc, &pControl->getPos()) ) continue;
					pControl->doPaint(hDC, rcPaint);
				}
			}
		}
	}

	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->isVisible() ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->getPos()) ) {
			m_pVerticalScrollBar->doPaint(hDC, rcPaint);
		}
	}

	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->isVisible() ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->getPos()) ) {
			m_pHorizontalScrollBar->doPaint(hDC, rcPaint);
		}
	}
}

void richEditUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"vscrollbar") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
	}
	if( _tcscmp(pstrName, L"autovscroll") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_lTwhStyle |= ES_AUTOVSCROLL;
	}
	else if( _tcscmp(pstrName, L"hscrollbar") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
	}
	if( _tcscmp(pstrName, L"autohscroll") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_lTwhStyle |= ES_AUTOHSCROLL;
	}
	else if( _tcscmp(pstrName, L"wanttab") == 0 ) {
		setWantTab(_tcscmp(pstrValue, L"true") == 0);
	}
	else if( _tcscmp(pstrName, L"wantreturn") == 0 ) {
		setWantReturn(_tcscmp(pstrValue, L"true") == 0);
	}
	else if( _tcscmp(pstrName, L"wantctrlreturn") == 0 ) {
		setWantCtrlReturn(_tcscmp(pstrValue, L"true") == 0);
	}
	else if( _tcscmp(pstrName, L"rich") == 0 ) {
		setRich(_tcscmp(pstrValue, L"true") == 0);
	}
	else if( _tcscmp(pstrName, L"multiline") == 0 ) {
		if( _tcscmp(pstrValue, L"false") == 0 ) m_lTwhStyle &= ~ES_MULTILINE;
	}
	else if( _tcscmp(pstrName, L"readonly") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) { m_lTwhStyle |= ES_READONLY; m_bReadOnly = true; }
	}
	else if( _tcscmp(pstrName, L"numberonly") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_lTwhStyle |= ES_NUMBER;
	}
	else if( _tcscmp(pstrName, L"password") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_lTwhStyle |= ES_PASSWORD;
	}
	else if( _tcscmp(pstrName, L"align") == 0 ) {
		if( _tcsstr(pstrValue, L"left") != NULL ) {
			m_lTwhStyle &= ~(ES_CENTER | ES_RIGHT);
			m_lTwhStyle |= ES_LEFT;
		}
		if( _tcsstr(pstrValue, L"center") != NULL ) {
			m_lTwhStyle &= ~(ES_LEFT | ES_RIGHT);
			m_lTwhStyle |= ES_CENTER;
		}
		if( _tcsstr(pstrValue, L"right") != NULL ) {
			m_lTwhStyle &= ~(ES_LEFT | ES_CENTER);
			m_lTwhStyle |= ES_RIGHT;
		}
	}
	else if( _tcscmp(pstrName, L"font") == 0 ) setFont(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"textcolor") == 0 ) {
		while( *pstrValue > L'\0' && *pstrValue <= L' ' ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setTextColor(clrColor);
	}
	else containerUI::setAttribute(pstrName, pstrValue);
}

LRESULT richEditUI::messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( !isVisible() || !isEnabled() ) return 0;
	if( !isMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
	if( uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0 ) return 0;

	bool bWasHandled = true;
	if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
		switch (uMsg) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				controlUI* pHover = getManager()->findControl(pt);
				if(pHover != this) {
					bWasHandled = false;
					return 0;
				}
			}
			break;
		}
		// Mouse message only go when captured or inside rect
		DWORD dwHitResult = m_pTwh->isCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
		if( dwHitResult == HITRESULT_OUTSIDE ) {
			RECT rc;
			m_pTwh->getControlRect(&rc);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if( uMsg == WM_MOUSEWHEEL ) ::ScreenToClient(getManager()->getPaintWindow(), &pt);
			if( ::PtInRect(&rc, pt) && !getManager()->isCaptured() ) dwHitResult = HITRESULT_HIT;
		}
		if( dwHitResult != HITRESULT_HIT ) return 0;
		if( uMsg == WM_SETCURSOR ) bWasHandled = false;
		else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN ) {
			setFocus();
		}
	}
#ifdef _UNICODE
	else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
#else
	else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR ) {
#endif
		if( !isFocused() ) return 0;
	}
	else
	{
		switch( uMsg ) {
		case WM_HELP:
		case WM_CONTEXTMENU:
			bWasHandled = false;
			break;
		default:
			return 0;
		}
	}
	LRESULT lResult = 0;
	HRESULT Hr = txSendMessage(uMsg, wParam, lParam, &lResult);
	if( Hr == S_OK ) bHandled = bWasHandled;
	return lResult;
}

}