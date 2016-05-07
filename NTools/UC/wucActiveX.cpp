#include "wucActiveX.h"

namespace wuc {
class activeXCtrl;
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-8-10 单位转换	
static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
	int nPixelsPerInchX;    // Pixels per logical inch along width
	int nPixelsPerInchY;    // Pixels per logical inch along height
	HDC hDCScreen = ::GetDC(NULL);
	nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
	nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
	::ReleaseDC(NULL, hDCScreen);
	lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
	lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	activeXWnd  类		2015-8-11
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class activeXWnd : public windowWnd
{
public:
	HWND init(activeXCtrl* pOwner, HWND hWndParent);

	virtual PCWSTR getWindowClassName() const;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);


protected:
	void doVerb(LONG iVerb);

	LRESULT onMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	activeXCtrl* m_pOwner;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	activeXEnum  类		2015-8-11
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class activeXEnum : public IEnumUnknown
{
public:
	activeXEnum(IUnknown* pUnk) 
		: m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
	{
		m_pUnk->AddRef();
	}
	~activeXEnum()
	{
		m_pUnk->Release();
	}


	STDMETHOD_(ULONG,AddRef)()
	{
		return ++m_dwRef;
	}
	STDMETHOD_(ULONG,Release)()
	{
		LONG lRef = --m_dwRef;
		if( lRef == 0 ) delete this;
		return lRef;
	}
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
	{
		*ppvObject = NULL;
		if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
		else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
		if( *ppvObject != NULL ) AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}
	STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
	{
		if( pceltFetched != NULL ) *pceltFetched = 0;
		if( ++m_iPos > 1 ) return S_FALSE;
		*rgelt = m_pUnk;
		(*rgelt)->AddRef();
		if( pceltFetched != NULL ) *pceltFetched = 1;
		return S_OK;
	}
	STDMETHOD(Skip)(ULONG celt)
	{
		m_iPos += celt;
		return S_OK;
	}
	STDMETHOD(Reset)(void)
	{
		m_iPos = 0;
		return S_OK;
	}
	STDMETHOD(Clone)(IEnumUnknown **ppenum)
	{
		return E_NOTIMPL;
	}

protected:
	LONG		m_iPos;
	ULONG		m_dwRef;
	IUnknown	*m_pUnk;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	activeXFrameWnd  类		2015-8-11
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class activeXFrameWnd : public IOleInPlaceFrame
{
public:
	activeXFrameWnd(activeXUC* pOwner) 
		: m_dwRef(1), m_pOwner(pOwner), m_pActiveObject(NULL)
	{
	}
	~activeXFrameWnd()
	{
		if( m_pActiveObject != NULL ) m_pActiveObject->Release();
	}

	// IUnknown
	STDMETHOD_(ULONG,AddRef)()
	{
		return ++m_dwRef;
	}
	STDMETHOD_(ULONG,Release)()
	{
		ULONG lRef = --m_dwRef;
		if( lRef == 0 ) delete this;
		return lRef;
	}
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
	{
		*ppvObject = NULL;
		if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
		else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
		else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
		else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
		if( *ppvObject != NULL ) AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}  
	// IOleInPlaceFrameWindow
	STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
	{
		return S_OK;
	}
	STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
	{
		return S_OK;
	}
	STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
	{
		return S_OK;
	}
	STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
	{
		return S_OK;
	}
	STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
	{
		return S_OK;
	}
	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
	{
		return S_FALSE;
	}
	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd)
	{
		if( m_pOwner == NULL ) return E_UNEXPECTED;
		*phwnd = m_pOwner->getManager()->getPaintWindow();
		return S_OK;
	}
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
	{
		return S_OK;
	}
	// IOleInPlaceUIWindow
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
	{
		return S_OK;
	}
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return S_OK;
	}
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
	{
		if( pActiveObject != NULL ) pActiveObject->AddRef();
		if( m_pActiveObject != NULL ) m_pActiveObject->Release();
		m_pActiveObject = pActiveObject;
		return S_OK;
	}

public:
	ULONG						m_dwRef;
	activeXUC					*m_pOwner;
	IOleInPlaceActiveObject		*m_pActiveObject;


};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	activeXCtrl  类		2015-8-11
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class activeXCtrl : public IOleClientSite, public IOleInPlaceSiteWindowless, public IOleControlSite
	, public IObjectWithSite, public IOleContainer
{
	friend class activeXUC;
	friend class activeXWnd;
public:
	activeXCtrl();
	~activeXCtrl();

	// IUnknown
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

	// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

	// IOleClientSite
	STDMETHOD(SaveObject)(void);       
	STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
	STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
	STDMETHOD(ShowObject)(void);        
	STDMETHOD(OnShowWindow)(BOOL fShow);        
	STDMETHOD(RequestNewObjectLayout)(void);

	// IOleInPlaceSiteWindowless
	STDMETHOD(CanWindowlessActivate)(void);
	STDMETHOD(GetCapture)(void);
	STDMETHOD(SetCapture)(BOOL fCapture);
	STDMETHOD(GetFocus)(void);
	STDMETHOD(SetFocus)(BOOL fFocus);
	STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
	STDMETHOD(ReleaseDC)(HDC hDC);
	STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
	STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
	STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
	STDMETHOD(AdjustRect)(LPRECT prc);
	STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

	// IOleInPlaceSiteEx
	STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
	STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
	STDMETHOD(RequestUIActivate)(void);

	// IOleInPlaceSite
	STDMETHOD(CanInPlaceActivate)(void);       
	STDMETHOD(OnInPlaceActivate)(void);        
	STDMETHOD(OnUIActivate)(void);
	STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHOD(Scroll)(SIZE scrollExtant);
	STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
	STDMETHOD(OnInPlaceDeactivate)(void);
	STDMETHOD(DiscardUndoState)( void);
	STDMETHOD(DeactivateAndUndo)( void);
	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

	// IOleControlSite
	STDMETHOD(OnControlInfoChanged)(void);      
	STDMETHOD(LockInPlaceActive)(BOOL fLock);       
	STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
	STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
	STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)(void);

	// IOleContainer
	STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
	STDMETHOD(LockContainer)(BOOL fLock);

	// IParseDisplayName
	STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

protected:
	HRESULT createActiveXWnd();

protected:
	LONG		m_dwRef;
	activeXUC	*m_pOwner;
	activeXWnd	*m_pWindow;
	IUnknown	*m_pUnkSite;
	IViewObject	*m_pViewObject;
	IOleInPlaceObjectWindowless *m_pInPlaceObject;
	bool		m_bLocked;
	bool		m_bFocused;
	bool		m_bCaptured;
	bool		m_bUIActivated;
	bool		m_bInPlaceActive;
	bool		m_bWindowless;
};

activeXCtrl::activeXCtrl() 
	: m_dwRef(1)
	, m_pOwner(NULL), m_pWindow(NULL), m_pUnkSite(NULL)
	, m_pViewObject(NULL), m_pInPlaceObject(NULL)
	, m_bLocked(false),  m_bFocused(false), m_bCaptured(false)
	, m_bWindowless(true), m_bUIActivated(false), m_bInPlaceActive(false)
{
}

activeXCtrl::~activeXCtrl()
{
	if( m_pWindow != NULL ) {
		::DestroyWindow(*m_pWindow);
		delete m_pWindow;
	}
	if( m_pUnkSite != NULL ) m_pUnkSite->Release();
	if( m_pViewObject != NULL ) m_pViewObject->Release();
	if( m_pInPlaceObject != NULL ) m_pInPlaceObject->Release();
}

STDMETHODIMP activeXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	*ppvObject = NULL;
	if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
	else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
	else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
	else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
	else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
	else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
	else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
	else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
	if( *ppvObject != NULL ) AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) activeXCtrl::AddRef()
{
	return ++m_dwRef;
}

STDMETHODIMP_(ULONG) activeXCtrl::Release()
{
	LONG lRef = --m_dwRef;
	if( lRef == 0 ) delete this;
	return lRef;
}

STDMETHODIMP activeXCtrl::SetSite(IUnknown *pUnkSite)
{
	SEND_LOG(_T("AX: activeXCtrl::SetSite"));
	if( m_pUnkSite != NULL ) {
		m_pUnkSite->Release();
		m_pUnkSite = NULL;
	}
	if( pUnkSite != NULL ) {
		m_pUnkSite = pUnkSite;
		m_pUnkSite->AddRef();
	}
	return S_OK;
}

STDMETHODIMP activeXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
{
	SEND_LOG(_T("AX: activeXCtrl::GetSite"));
	if( ppvSite == NULL ) return E_POINTER;
	*ppvSite = NULL;
	if( m_pUnkSite == NULL ) return E_FAIL;
	return m_pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP activeXCtrl::SaveObject(void)
{
	SEND_LOG(_T("AX: activeXCtrl::SaveObject"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
	SEND_LOG(_T("AX: activeXCtrl::GetMoniker"));
	if( ppmk != NULL ) *ppmk = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::GetContainer(IOleContainer** ppContainer)
{
	SEND_LOG(_T("AX: activeXCtrl::GetContainer"));
	if( ppContainer == NULL ) return E_POINTER;
	*ppContainer = NULL;
	HRESULT Hr = E_NOTIMPL;
	if( m_pUnkSite != NULL ) Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
	if( FAILED(Hr) ) Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
	return Hr;
}

STDMETHODIMP activeXCtrl::ShowObject(void)
{
	SEND_LOG(_T("AX: activeXCtrl::ShowObject"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	HDC hDC = ::GetDC(m_pOwner->m_hwndHost);
	if( hDC == NULL ) return E_FAIL;
	if( m_pViewObject != NULL ) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_pOwner->m_rcItem, (RECTL*) &m_pOwner->m_rcItem, NULL, NULL);
	::ReleaseDC(m_pOwner->m_hwndHost, hDC);
	return S_OK;
}

STDMETHODIMP activeXCtrl::OnShowWindow(BOOL fShow)
{
	SEND_LOG(_T("AX: activeXCtrl::OnShowWindow"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::RequestNewObjectLayout(void)
{
	SEND_LOG(_T("AX: activeXCtrl::RequestNewObjectLayout"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::CanWindowlessActivate(void)
{
	SEND_LOG(_T("AX: activeXCtrl::CanWindowlessActivate"));
	return S_OK;  // Yes, we can!!
}

STDMETHODIMP activeXCtrl::GetCapture(void)
{
	SEND_LOG(_T("AX: activeXCtrl::GetCapture"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	return m_bCaptured ? S_OK : S_FALSE;
}

STDMETHODIMP activeXCtrl::SetCapture(BOOL fCapture)
{
	SEND_LOG(_T("AX: activeXCtrl::SetCapture"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	m_bCaptured = (fCapture == TRUE);
	if( fCapture ) ::SetCapture(m_pOwner->m_hwndHost); else ::ReleaseCapture();
	return S_OK;
}

STDMETHODIMP activeXCtrl::GetFocus(void)
{
	SEND_LOG(_T("AX: activeXCtrl::GetFocus"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	return m_bFocused ? S_OK : S_FALSE;
}

STDMETHODIMP activeXCtrl::SetFocus(BOOL fFocus)
{
	SEND_LOG(_T("AX: activeXCtrl::SetFocus"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if( fFocus ) m_pOwner->setFocus();
	m_bFocused = (fFocus == TRUE);
	return S_OK;
}

STDMETHODIMP activeXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
	SEND_LOG(_T("AX: activeXCtrl::GetDC"));
	if( phDC == NULL ) return E_POINTER;
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	*phDC = ::GetDC(m_pOwner->m_hwndHost);
	if( (grfFlags & OLEDC_PAINTBKGND) != 0 ) {
		RECT rcItem = m_pOwner->getPosition();
		if( !m_bWindowless ) ::OffsetRect(&rcItem, -rcItem.left, -rcItem.top)/*rcItem.ResetOffset()*/;
		::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
	}
	return S_OK;
}

STDMETHODIMP activeXCtrl::ReleaseDC(HDC hDC)
{
	SEND_LOG(_T("AX: activeXCtrl::ReleaseDC"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	::ReleaseDC(m_pOwner->m_hwndHost, hDC);
	return S_OK;
}

STDMETHODIMP activeXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
	SEND_LOG(_T("AX: activeXCtrl::InvalidateRect"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
	return ::InvalidateRect(m_pOwner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP activeXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
	SEND_LOG(_T("AX: activeXCtrl::InvalidateRgn"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	return ::InvalidateRgn(m_pOwner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP activeXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
	SEND_LOG(_T("AX: activeXCtrl::ScrollRect"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::AdjustRect(LPRECT prc)
{
	SEND_LOG(_T("AX: activeXCtrl::AdjustRect"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
	SEND_LOG(_T("AX: activeXCtrl::OnDefWindowMessage"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	*plResult = ::DefWindowProc(m_pOwner->m_hwndHost, msg, wParam, lParam);
	return S_OK;
}

STDMETHODIMP activeXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
	SEND_LOG(_T("AX: activeXCtrl::OnInPlaceActivateEx"));
	assert(m_pInPlaceObject==NULL);
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
	::OleLockRunning(m_pOwner->m_pUnk, TRUE, FALSE);
	HWND hWndFrame = m_pOwner->getManager()->getPaintWindow();
	HRESULT Hr = E_FAIL;
	if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 ) {
		m_bWindowless = true;
		Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
		m_pOwner->m_hwndHost = hWndFrame;
		m_pOwner->getManager()->addMessageFilter(m_pOwner);
	}
	if( FAILED(Hr) ) {
		m_bWindowless = false;
		Hr = createActiveXWnd();
		if( FAILED(Hr) ) return Hr;
		Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
	}
	if( m_pInPlaceObject != NULL ) {
		RECT rcItem = m_pOwner->m_rcItem;
		if( !m_bWindowless ) ::OffsetRect(&rcItem, -rcItem.left, -rcItem.top) /*rcItem.ResetOffset()*/;
		m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
	}
	m_bInPlaceActive = SUCCEEDED(Hr);
	return Hr;
}

STDMETHODIMP activeXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
	SEND_LOG(_T("AX: activeXCtrl::OnInPlaceDeactivateEx"));
	m_bInPlaceActive = false;
	if( m_pInPlaceObject != NULL ) {
		m_pInPlaceObject->Release();
		m_pInPlaceObject = NULL;
	}
	if( m_pWindow != NULL ) {
		::DestroyWindow(*m_pWindow);
		delete m_pWindow;
		m_pWindow = NULL;
	}
	return S_OK;
}

STDMETHODIMP activeXCtrl::RequestUIActivate(void)
{
	SEND_LOG(_T("AX: activeXCtrl::RequestUIActivate"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::CanInPlaceActivate(void)       
{
	SEND_LOG(_T("AX: activeXCtrl::CanInPlaceActivate"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::OnInPlaceActivate(void)
{
	SEND_LOG(_T("AX: activeXCtrl::OnInPlaceActivate"));
	BOOL bDummy = FALSE;
	return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP activeXCtrl::OnUIActivate(void)
{
	SEND_LOG(_T("AX: activeXCtrl::OnUIActivate"));
	m_bUIActivated = true;
	return S_OK;
}

STDMETHODIMP activeXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	SEND_LOG(_T("AX: activeXCtrl::GetWindowContext"));
	if( ppDoc == NULL ) return E_POINTER;
	if( ppFrame == NULL ) return E_POINTER;
	if( lprcPosRect == NULL ) return E_POINTER;
	if( lprcClipRect == NULL ) return E_POINTER;
	if (m_pWindow)
	{
		::GetClientRect(m_pWindow->getHWND(),lprcPosRect);
		::GetClientRect(m_pWindow->getHWND(),lprcClipRect);
	}
	*ppFrame = new activeXFrameWnd(m_pOwner);
	*ppDoc = NULL;
	ACCEL ac = { 0 };
	HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
	lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = m_pOwner->getManager()->getPaintWindow();
	lpFrameInfo->haccel = hac;
	lpFrameInfo->cAccelEntries = 1;
	return S_OK;
}

STDMETHODIMP activeXCtrl::Scroll(SIZE scrollExtant)
{
	SEND_LOG(_T("AX: activeXCtrl::Scroll"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::OnUIDeactivate(BOOL fUndoable)
{
	SEND_LOG(_T("AX: activeXCtrl::OnUIDeactivate"));
	m_bUIActivated = false;
	return S_OK;
}

STDMETHODIMP activeXCtrl::OnInPlaceDeactivate(void)
{
	SEND_LOG(_T("AX: activeXCtrl::OnInPlaceDeactivate"));
	return OnInPlaceDeactivateEx(TRUE);
}

STDMETHODIMP activeXCtrl::DiscardUndoState(void)
{
	SEND_LOG(_T("AX: activeXCtrl::DiscardUndoState"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::DeactivateAndUndo(void)
{
	SEND_LOG(_T("AX: activeXCtrl::DeactivateAndUndo"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
{
	SEND_LOG(_T("AX: activeXCtrl::OnPosRectChange"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::GetWindow(HWND* phwnd)
{
	SEND_LOG(_T("AX: activeXCtrl::GetWindow"));
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if( m_pOwner->m_hwndHost == NULL ) createActiveXWnd();
	if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
	*phwnd = m_pOwner->m_hwndHost;
	return S_OK;
}

STDMETHODIMP activeXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
{
	SEND_LOG(_T("AX: activeXCtrl::ContextSensitiveHelp"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::OnControlInfoChanged(void)      
{
	SEND_LOG(_T("AX: activeXCtrl::OnControlInfoChanged"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::LockInPlaceActive(BOOL fLock)       
{
	SEND_LOG(_T("AX: activeXCtrl::LockInPlaceActive"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::GetExtendedControl(IDispatch** ppDisp)        
{
	SEND_LOG(_T("AX: activeXCtrl::GetExtendedControl"));
	if( ppDisp == NULL ) return E_POINTER;   
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
	return m_pOwner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
}

STDMETHODIMP activeXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
	SEND_LOG(_T("AX: activeXCtrl::TransformCoords"));
	return S_OK;
}

STDMETHODIMP activeXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
	SEND_LOG(_T("AX: activeXCtrl::TranslateAccelerator"));
	return S_FALSE;
}

STDMETHODIMP activeXCtrl::OnFocus(BOOL fGotFocus)
{
	SEND_LOG(_T("AX: activeXCtrl::OnFocus"));
	m_bFocused = (fGotFocus == TRUE);
	return S_OK;
}

STDMETHODIMP activeXCtrl::ShowPropertyFrame(void)
{
	SEND_LOG(_T("AX: activeXCtrl::ShowPropertyFrame"));
	return E_NOTIMPL;
}

STDMETHODIMP activeXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
	SEND_LOG(_T("AX: activeXCtrl::EnumObjects"));
	if( ppenum == NULL ) return E_POINTER;
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	*ppenum = new activeXEnum(m_pOwner->m_pUnk);
	return S_OK;
}

STDMETHODIMP activeXCtrl::LockContainer(BOOL fLock)
{
	SEND_LOG(_T("AX: activeXCtrl::LockContainer"));
	m_bLocked = fLock != FALSE;
	return S_OK;
}

STDMETHODIMP activeXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
	SEND_LOG(_T("AX: activeXCtrl::ParseDisplayName"));
	return E_NOTIMPL;
}

HRESULT activeXCtrl::createActiveXWnd()
{
	if( m_pWindow != NULL ) return S_OK;
	m_pWindow = new activeXWnd;
	if( m_pWindow == NULL ) return E_OUTOFMEMORY;
	//m_pOwner->m_hwndHost = m_pWindow->init(this, NULL);
	m_pOwner->m_hwndHost = m_pWindow->init(this, m_pOwner->getManager()->getPaintWindow());
	return S_OK;
}
HWND activeXWnd::init(activeXCtrl* pOwner, HWND hWndParent)
{
	m_pOwner = pOwner;
	UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	create(hWndParent, L"UCActiveX", uStyle, 0L, 0,0,0,0, NULL);
	return m_hWnd;
}

PCWSTR activeXWnd::getWindowClassName() const
{
	return L"ActiveXWnd";
}
LRESULT activeXWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes=0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_PAINT:         lRes = onPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:      lRes = onSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:     lRes = onKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_ERASEBKGND:    lRes = onEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEACTIVATE: lRes = onMouseActivate(uMsg, wParam, lParam, bHandled); break;
	//case WM_MOUSEWHEEL:
	//	{
	//		SEND_LOG(L"ERROR activeXWnd::handleMessage_WM_MOUSEWHEEL");
	//		if (m_pOwner)
	//		{
	//			m_pOwner->m_pOwner->getManager()->sendNotify(m_pOwner->m_pOwner, L"mousewheel", wParam, lParam);
	//		}
	//		bHandled = FALSE;
	//	}
	//	break;
	default:
		bHandled = FALSE;
	}
	//if(!bHandled && !wuc::windowWnd::isOutMsg(uMsg))
	//	SEND_LOG(L"activeXWnd::handleMessage msg:%s", wuc::windowWnd::stringMsg(uMsg).c_str() );
	if(bHandled) return lRes;
	//if(m_pOwner && m_pOwner->m_pOwner && m_pOwner->m_pOwner->getManager()->messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	if( !bHandled ) return windowWnd::handleMessage(uMsg, wParam, lParam);
	return lRes;
}
void activeXWnd::onFinalMessage(HWND hWnd)
{

}

void activeXWnd::doVerb(LONG iVerb)
{
	if( m_pOwner == NULL ) return;
	if( m_pOwner->m_pOwner == NULL ) return;
	IOleObject* pUnk = NULL;
	m_pOwner->m_pOwner->getControl(IID_IOleObject, (LPVOID*) &pUnk);
	if( pUnk == NULL ) return;
	//CSafeRelease<IOleObject> RefOleObject = pUnk;
	IOleClientSite* pOleClientSite = NULL;
	m_pOwner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
	//CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
	RECT rc = m_pOwner->m_pOwner->getPosition();
	pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &rc);
	if(pUnk) pUnk->Release();
	if(pOleClientSite) pOleClientSite->Release();
}

LRESULT activeXWnd::onMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	IOleObject* pUnk = NULL;
	m_pOwner->m_pOwner->getControl(IID_IOleObject, (LPVOID*) &pUnk);
	if( pUnk == NULL ) return 0;
	//CSafeRelease<IOleObject> RefOleObject = pUnk;
	DWORD dwMiscStatus = 0;
	pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
	if(pUnk) pUnk->Release();
	if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
	if( !m_pOwner->m_bInPlaceActive ) doVerb(OLEIVERB_INPLACEACTIVATE);
	bHandled = FALSE;
	return 0;
}
LRESULT activeXWnd::onSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_pOwner->m_bFocused = true;
	if( !m_pOwner->m_bUIActivated ) doVerb(OLEIVERB_UIACTIVATE);
	return 0;
}
LRESULT activeXWnd::onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_pOwner->m_bFocused = false;
	return 0;
}
LRESULT activeXWnd::onEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_pOwner->m_pViewObject == NULL ) bHandled = FALSE;
	return 1;
}
LRESULT activeXWnd::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	::BeginPaint(m_hWnd, &ps);
	::EndPaint(m_hWnd, &ps);
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	activeXUC  类		2015-8-10
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
activeXUC::activeXUC()
	: m_pUnk(NULL)
	, m_pControl(NULL), m_hwndHost(NULL)
	, m_bCreated(false), m_bDelayCreate(true)
{
	m_clsid = IID_NULL;
}
activeXUC::~activeXUC()
{
	releaseControl();
}

PCWSTR activeXUC::getClass() const
{
	return L"activeXUC";
}
LPVOID activeXUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"ActiveX") == 0 ) return static_cast<activeXUC*>(this);
	return __super::getInterface(pstrName);
}
void activeXUC::paint(HDC hdc, const RECT &rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	//paintBkColor(hdc);
	__super::paint(hdc, rcPaint);
	if( m_pControl != NULL && m_pControl->m_bWindowless && m_pControl->m_pViewObject != NULL )
	{
		m_pControl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (RECTL*) &m_rcItem, (RECTL*) &m_rcItem, NULL, NULL); 
	}
}
void activeXUC::setPosition(RECT pos)
{
	controlUC::setPosition(pos);

	if( !m_bCreated ) doCreateControl();

	if( m_pUnk == NULL ) return;
	if( m_pControl == NULL ) return;

	SIZEL hmSize = { 0 };
	SIZEL pxSize = { 0 };
	pxSize.cx = m_rcItem.right - m_rcItem.left;
	pxSize.cy = m_rcItem.bottom - m_rcItem.top;
	PixelToHiMetric(&pxSize, &hmSize);

	if( m_pUnk != NULL ) {
		m_pUnk->SetExtent(DVASPECT_CONTENT, &hmSize);
	}
	if( m_pControl->m_pInPlaceObject != NULL ) {
		RECT rcItem = m_rcItem;
		if( !m_pControl->m_bWindowless ) ::OffsetRect(&rcItem, -rcItem.left, -rcItem.top);
		m_pControl->m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
	}
	if( !m_pControl->m_bWindowless ) {
		assert(m_pControl->m_pWindow);
		RECT rc = m_rcItem;
		if(m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible())
			rc.right -= m_pVerticalScrollBar->getFixedWidth();
		if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible())
			rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
		HWND hWnd = *m_pControl->m_pWindow;
		::MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		while (GetWindow(hWnd, GW_CHILD))
		{
			hWnd = GetWindow(hWnd, GW_CHILD);
			if(hWnd) ::MoveWindow(hWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		}
	}
}
void activeXUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"clsid") == 0 ) createControl(pstrValue);
	else if( _tcscmp(pstrName, L"modulename") == 0 ) setModuleName(pstrValue);
	else if( _tcscmp(pstrName, L"delaycreate") == 0 ) setDelayCreate(_tcscmp(pstrValue, L"true") == 0);
	else __super::setAttribute(pstrName, pstrValue);
}
void activeXUC::setVisible(bool bVisible /*= true*/)
{
	controlUC::setVisible(bVisible);
	if( m_hwndHost != NULL && !m_pControl->m_bWindowless ) 
		::ShowWindow(m_hwndHost, isVisible() ? SW_SHOW : SW_HIDE);
}
void activeXUC::setInternVisible(bool bVisible /*= true*/)
{
	controlUC::setInternVisible(bVisible);
	if( m_hwndHost != NULL && !m_pControl->m_bWindowless ) 
		::ShowWindow(m_hwndHost, isVisible() ? SW_SHOW : SW_HIDE);
}
LRESULT activeXUC::messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( m_pControl == NULL ) return 0;
	assert(m_pControl->m_bWindowless);
	if( !m_pControl->m_bInPlaceActive ) return 0;
	if( m_pControl->m_pInPlaceObject == NULL ) return 0;
	if( !isMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
	bool bWasHandled = true;

	if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
		// Mouse message only go when captured or inside rect
		DWORD dwHitResult = m_pControl->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
		if( dwHitResult == HITRESULT_OUTSIDE && m_pControl->m_pViewObject != NULL ) {
			IViewObjectEx* pViewEx = NULL;
			m_pControl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
			if( pViewEx != NULL ) {
				POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcItem, ptMouse, 0, &dwHitResult);
				pViewEx->Release();
			}
		}
		if( dwHitResult != HITRESULT_HIT ) return 0;
		if( uMsg == WM_SETCURSOR ) bWasHandled = false;
	}
	else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
		// Keyboard messages just go when we have focus
		if( !isFocused() ) return 0;
	}
	else {
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
	HRESULT Hr = m_pControl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
	if( Hr == S_OK ) bHandled = bWasHandled;
	return lResult;
}

HWND activeXUC::getHostWindow() const
{
	return m_hwndHost;
}

bool activeXUC::isDelayCreate() const
{
	return m_bDelayCreate;
}
void activeXUC::setDelayCreate(bool bDelayCreate /*= true*/)
{
	if( m_bDelayCreate == bDelayCreate ) return;
	if( bDelayCreate == false ) {
		if( m_bCreated == false && m_clsid != IID_NULL ) doCreateControl();
	}
	m_bDelayCreate = bDelayCreate;
}

bool activeXUC::createControl(const CLSID clsid)
{
	assert(clsid!=IID_NULL);
	if( clsid == IID_NULL ) return false;
	m_bCreated = false;
	m_clsid = clsid;
	if( !m_bDelayCreate ) doCreateControl();
	return true;
}
bool activeXUC::createControl(PCWSTR pstrCLSID)
{
	CLSID clsid = { 0 };
	OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
	::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
	_tcsncpy(szCLSID, pstrCLSID, _countof(szCLSID) - 1);
#endif
	if( pstrCLSID[0] == '{' ) ::CLSIDFromString(szCLSID, &clsid);
	else ::CLSIDFromProgID(szCLSID, &clsid);
	return createControl(clsid);

}
HRESULT activeXUC::getControl(const IID iid, LPVOID* ppRet)
{
	assert(ppRet!=NULL);
	assert(*ppRet==NULL);
	if( ppRet == NULL ) return E_POINTER;
	if( m_pUnk == NULL ) return E_PENDING;
	return m_pUnk->QueryInterface(iid, (LPVOID*) ppRet);
}
CLSID activeXUC::getClisd() const
{
	return m_clsid;
}
std::wstring activeXUC::getModuleName() const
{
	return m_wsModuleName;
}
void activeXUC::setModuleName(PCWSTR pstrText)
{
	m_wsModuleName = pstrText;
}

void activeXUC::releaseControl()
{
	m_hwndHost = NULL;
	if( m_pUnk != NULL ) {
		IObjectWithSite* pSite = NULL;
		m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
		if( pSite != NULL ) {
			pSite->SetSite(NULL);
			pSite->Release();
		}
		m_pUnk->Close(OLECLOSE_NOSAVE);
		m_pUnk->SetClientSite(NULL);
		m_pUnk->Release(); 
		m_pUnk = NULL;
	}
	if( m_pControl != NULL ) {
		m_pControl->m_pOwner = NULL;
		m_pControl->Release();
		m_pControl = NULL;
	}
	m_pManager->removeMessageFilter(this);
}

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 

bool activeXUC::doCreateControl()
{
	releaseControl();
	HRESULT hr = S_FALSE;

	// 创建IOleObject*
	LPCLASSFACTORY pClassFactory = NULL;

	if (SUCCEEDED(hr = CoGetClassObject(m_clsid, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL,
		IID_IClassFactory, (void**)&pClassFactory)))
	{
		assert(pClassFactory != NULL);
		hr = pClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_pUnk);
		pClassFactory->Release();
	}
	// 
	assert(m_pUnk);
	if(!m_pUnk) return false;
	m_pControl = new activeXCtrl();
	m_pControl->m_pOwner = this;
	// More control creation stuff
	DWORD dwMiscStatus = 0;
	m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
	//IOleClientSite* pOleClientSite = NULL;
	//m_pControl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
	if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) m_pUnk->SetClientSite(static_cast<IOleClientSite*>(m_pControl));
	IPersistStreamInit* pPersStm = NULL;
	// initialize via IPersistStreamInit
	m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersStm);
	if( pPersStm != NULL ) {
		hr = pPersStm->InitNew();
		pPersStm->Release();
	}
	// set client site last, if appropriate
	if (!(dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
		m_pUnk->SetClientSite(static_cast<IOleClientSite*>(m_pControl));
	// 
	IOleInPlaceObject *pInPlaceObject = NULL;
	if (SUCCEEDED(hr = m_pUnk->QueryInterface(IID_IOleInPlaceObject,
		(LPVOID*)&pInPlaceObject)))
	{
		hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, static_cast<IOleClientSite*>(m_pControl), 0, m_pManager->getPaintWindow(), &m_rcItem);
		hr = pInPlaceObject->SetObjectRects(&m_rcItem, &m_rcItem);
	}
	//// Grab the view...
	hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_pControl->m_pViewObject);
	if( FAILED(hr) ) hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pControl->m_pViewObject);
	if( FAILED(hr) ) hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_pControl->m_pViewObject);
	// Activate and done...
	m_pUnk->SetHostNames(OLESTR("UCActiveX"), NULL);
	if( m_pManager != NULL ) m_pManager->sendNotify((controlUC*)this, L"showactivex", 0, 0, false);
	IObjectWithSite* pSite = NULL;
	m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
	if( pSite != NULL ) {
		pSite->SetSite(static_cast<IOleClientSite*>(m_pControl));
		pSite->Release();
	}

	m_bCreated = SUCCEEDED(hr);
	return m_bCreated;

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-9-15 	
	//releaseControl();
	//// At this point we'll create the ActiveX control
	//m_bCreated = true;
	//IOleControl* pOleControl = NULL;

	//HRESULT Hr = -1;
	//if( !m_wsModuleName.empty() ) {
	//	HMODULE hModule = ::LoadLibrary(m_wsModuleName.c_str());
	//	if( hModule != NULL ) {
	//		IClassFactory* aClassFactory = NULL;
	//		DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc)::GetProcAddress(hModule, "DllGetClassObject");
	//		Hr = aDllGetClassObjectFunc(m_clsid, IID_IClassFactory, (LPVOID*)&aClassFactory);
	//		if( SUCCEEDED(Hr) ) {
	//			Hr = aClassFactory->CreateInstance(NULL, IID_IOleObject, (LPVOID*)&pOleControl);
	//		}
	//		if(aClassFactory) aClassFactory->Release();
	//	}
	//}
	//if( FAILED(Hr) ) {
	//	Hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);
	//}
	//assert(SUCCEEDED(Hr));
	//if( FAILED(Hr) ) return false;
	//pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &m_pUnk);
	//pOleControl->Release();
	//if( m_pUnk == NULL ) return false;
	//// Create the host too
	//m_pControl = new activeXCtrl();
	//m_pControl->m_pOwner = this;
	//// More control creation stuff
	//DWORD dwMiscStatus = 0;
	//m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
	//IOleClientSite* pOleClientSite = NULL;
	//m_pControl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
	////CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
	//// Initialize control
	//if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) m_pUnk->SetClientSite(pOleClientSite);
	//IPersistStreamInit* pPersistStreamInit = NULL;
	//m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
	//if( pPersistStreamInit != NULL ) {
	//	Hr = pPersistStreamInit->InitNew();
	//	pPersistStreamInit->Release();
	//}
	//if( FAILED(Hr) )
	//{
	//	if(pPersistStreamInit) pPersistStreamInit->Release();
	//	if(pOleClientSite) pOleClientSite->Release();
	//	if(m_pUnk) m_pUnk->Release();
	//	return false;
	//}
	//if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 ) m_pUnk->SetClientSite(pOleClientSite);
	//// Grab the view...
	//Hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_pControl->m_pViewObject);
	//if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pControl->m_pViewObject);
	//if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_pControl->m_pViewObject);
	//// Activate and done...
	//m_pUnk->SetHostNames(OLESTR("UCActiveX"), NULL);
	//if( m_pManager != NULL ) m_pManager->sendNotify((controlUC*)this, L"showactivex", 0, 0, false);
	//if( (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 ) {
	//	Hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, m_pManager->getPaintWindow(), &m_rcItem);
	//	//::RedrawWindow(m_pManager->getPaintWindow(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
	//}
	//IObjectWithSite* pSite = NULL;
	//m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
	//if( pSite != NULL ) {
	//	pSite->SetSite(static_cast<IOleClientSite*>(m_pControl));
	//	pSite->Release();
	//}

	//if(pPersistStreamInit) pPersistStreamInit->Release();
	////if(pOleClientSite) pOleClientSite->Release();
	////if(m_pUnk) m_pUnk->Release();

	//return SUCCEEDED(Hr);
}

}