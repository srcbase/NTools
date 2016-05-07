#include "windowWTL.h"
#include "drawManager.h"
#include <cassert>
namespace wuc
{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowWTL 类	2015-1-19
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
windowWTL::windowWTL()
	: m_beAnswerState(BEAnswerStateNone)
{

}
windowWTL::~windowWTL()
{

}

HWND windowWTL::getHWND() const
{
	return m_hWnd;
}
//windowWTL::operator HWND() const
//{
//	return m_hWnd;
//}

ATL::CWndClassInfo windowWTL::GetWndClassInfo()
{
	ATL::CWndClassInfo wc = 
	{ 
		{ sizeof(WNDCLASSEX), getClassStyle(), StartWindowProc, 
		0, 0, drawManager::getInstance(), NULL, NULL, (HBRUSH)GetStockObject(NULL_BRUSH)/*否则会影响背景透明*/, NULL, getWindowClassName(), NULL }, 
		GetWndClassName(), NULL, NULL/*IDC_ARROW 会影响wuc对BEUCEventSetCursor的捕获*/, TRUE, 0, _T("") 
	}; 
	//ATL::CWndClassInfo wc = 
	//{ 
	//	{ sizeof(WNDCLASSEX), getClassStyle(), StartWindowProc, 
	//	0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_BACKGROUND + 1), NULL, getWindowClassName(), NULL }, 
	//	GetWndClassName(), NULL, IDC_ARROW, TRUE, 0, _T("") 
	//}; 

	return wc; 
}

HWND windowWTL::create(HWND hWndParent, RECT rc, PCWSTR pWindowName /*= NULL*/, DWORD dwStyle /*= 0*/, DWORD dwExStyle /*= 0*/, 
	_U_MENUorID MenuOrID /*= 0U*/, LPVOID lpCreateParam /*= NULL*/)
{
	ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);
	if (pWindowName == NULL)
		pWindowName = getWindowCaption();
	if(dwStyle == 0) dwStyle = getWindowStyle();
	if(dwExStyle == 0) dwExStyle = getWindowExStyle();

	//HWND hWnd = Create(hWndParent, rc, pWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
	HWND hWnd = CWindowImplBaseT<CWindow, CControlWinTraits>::
		Create(hWndParent, rc, pWindowName, dwStyle, dwExStyle, MenuOrID, atom, lpCreateParam);

	//std::assert(hWnd);

	return hWnd;
}

//BOOL windowWTL::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
//{
//	if (hWnd == m_hWnd)
//	{
//		lResult = handleMessage(uMsg, wParam, lParam);
//	}
//	else
//		__super::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
//	return TRUE;
//}

LRESULT windowWTL::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	if(uMsg == WM_SETCURSOR)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}
	//WNDPROC pfnWndProc = (WNDPROC)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);
	return ::CallWindowProc(m_pfnSuperWindowProc, m_hWnd, uMsg, wParam, lParam);
	return S_OK;
}
void windowWTL::onFinalMessage(HWND hWnd)
{
	m_hWnd = NULL;	// 
}
void windowWTL::close()
{
	assert(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	
	if(DestroyWindow()) m_hWnd = NULL;
	//SendMessage(WM_DESTROY);
	//m_hWnd = NULL;
	::PostQuitMessage(0);
}

BEAnswerState windowWTL::getAnswerState() const
{
	return m_beAnswerState;
}
void windowWTL::setAnswerState(BEAnswerState state)
{
	m_beAnswerState = state;
}

}	// end wuc