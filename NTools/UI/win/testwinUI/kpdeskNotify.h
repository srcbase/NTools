#pragma once
class kpdeskNotify : public wui::windowWnd, public wui::INotifyUI
{
public:
	kpdeskNotify(void);
	~kpdeskNotify(void);

	virtual PCWSTR getWindowClassName() const { return L"WUIDeskWnd";							}
	virtual UINT getClassStyle() const {		return CS_DBLCLKS;								}
	UINT getWindowStyle() const {				return WS_THICKFRAME;							}
	UINT getWindowExStyle() const {				return WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT|WS_EX_TOOLWINDOW;							}
	virtual void onFinalMessage(HWND hWnd);
	virtual void notify(TNotifyUI& msg);
	virtual LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	paintManagerUI m_paintManager;
};

