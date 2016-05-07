#pragma once
class kpdeskDialog : public wuc::windowWnd, public wuc::INotifyUC
{
public:
	kpdeskDialog(void);
	~kpdeskDialog(void);

	virtual PCWSTR getWindowClassName() const { return L"wuc_kpdeskDialog";							}
	virtual UINT getClassStyle() const {		return CS_DBLCLKS;								}
	UINT getWindowStyle() const {				return WS_THICKFRAME;							}
	UINT getWindowExStyle() const {				return WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT|WS_EX_TOOLWINDOW;							}
	virtual void onFinalMessage(HWND hWnd);
	virtual void notify(wuc::BSNotifyUC& msg);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	wuc::paintManagerUC m_paintManager;
};

