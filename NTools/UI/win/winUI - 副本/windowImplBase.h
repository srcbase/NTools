#pragma once

namespace wui {


class WINUI_API windowImplBase : public windowWnd, public INotifyUI, public IMessageFilterUI, public IDialogBuilderCallback
{
public:
	windowImplBase();
	virtual ~windowImplBase();

	virtual void onFinalMessage(HWND hWnd);

	virtual UINT getClassStyle() const;

	virtual void init();

	virtual controlUI* createControl(PCWSTR pstrClass);

	virtual LRESULT messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual LRESULT onClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	virtual LRESULT onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT onMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT handleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	virtual std::wstring getSkinFolder();
	virtual std::wstring getSkinFile() = 0;

	virtual LRESULT responseDefaultKeyEvent(WPARAM wParam);

protected:
	paintManagerUI m_paintManager;
};


}