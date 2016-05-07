#ifndef __WUCACTIVEX_H__
#define __WUCACTIVEX_H__
#pragma once
#include "wucBase.h"
#include "windowWnd.h"
#include "wucContainer.h"
namespace wuc {

class activeXCtrl;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	activeXUC  类		2015-8-10
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class activeXUC : public containerUC, public IMessageFilterUC
{
	friend class activeXCtrl;
public:
	activeXUC();
	virtual ~activeXUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void paint(HDC hdc, const RECT &rcPaint);
	virtual void setPosition(RECT pos);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void setVisible(bool bVisible = true);
	virtual void setInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
	// IMessageFilterUC
	virtual LRESULT messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

public:
	HWND getHostWindow() const;

	bool isDelayCreate() const;
	void setDelayCreate(bool bDelayCreate = true);

	bool createControl(const CLSID clsid);
	bool createControl(PCWSTR pstrCLSID);
	HRESULT getControl(const IID iid, LPVOID* ppRet);
	CLSID getClisd() const;
	std::wstring getModuleName() const;
	void setModuleName(PCWSTR pstrText);
protected:
	virtual void releaseControl();
	virtual bool doCreateControl();
protected:
	CLSID			m_clsid;
	std::wstring	m_wsModuleName;
	bool			m_bCreated;
	bool			m_bDelayCreate;
	IOleObject		*m_pUnk;
	activeXCtrl		*m_pControl;
	HWND			m_hwndHost;
};


}	// wuc

#endif	// __WUCACTIVEX_H__