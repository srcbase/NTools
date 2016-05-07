#pragma once

class kpdeskNotify : public wuc::windowWnd, public wuc::INotifyUC
{
public:
	kpdeskNotify(void);
	~kpdeskNotify(void);

	virtual PCWSTR getWindowClassName() const { return L"wuc_kpdeskDialog";							}
	virtual UINT getClassStyle() const {		return CS_DBLCLKS;								}
	UINT getWindowStyle() const {				return WS_POPUP|WS_CLIPSIBLINGS;							}
	UINT getWindowExStyle() const {				return WS_EX_LAYERED|WS_EX_LTRREADING/*WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT|WS_EX_TOOLWINDOW*/;							}
	virtual void onFinalMessage(HWND hWnd);
	virtual void notify(wuc::BSNotifyUC& msg);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	wuc::paintManagerUC m_paintManager;

	wuc::buttonUC	m_btnKnow;		// 知道了按钮
	
};

