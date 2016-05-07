#ifndef __WUCWKEWEBKIT_H__
#define __WUCWKEWEBKIT_H__
#pragma once
#include "wucContainer.h"
#include "windowWnd.h"
#include "../wke/wke.h"
namespace wuc {
class wkeWebkitWnd;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	wkeWebkitUC  类		2015-9-16
//	基于wke的封装库
//	文档的加载状态与当前url相关，即url改变后当前状态都要变了
////////////////////////////////////////////////////////////////////////////////////////////////////////

class wkeWebkitUC : public containerUC
{
	friend class wkeWebkitWnd;
public:
	wkeWebkitUC();
	~wkeWebkitUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void Event(BSEventUC &event);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	virtual void setPosition(RECT pos);
	virtual void setScrollPos(SIZE szPos);
	virtual void onInit();
	virtual void setVisible(bool bVisible = true);
	virtual void setInternVisible(bool bVisible = true);

	void setURL(std::wstring strValue);
	void setFile(std::wstring strValue);
	std::wstring runJS(std::wstring strValue);
	void setClientHandler(const wkeClientHandler* handler);
	void stopLoading();

	void goBack();
	void goForward();
	/****************************************************************************************
		<< --- buildJSFunction	2016-1-6	kp007 --- >>         
		说明：将js函数绑定到C++函数上
		参数：
			jsfuncName	= 被绑定的js函数名称
			exeFunc		= 绑定的cpp函数，当浏览器执行jsfuncName函数时调用本函数
			argCount	= js函数的参数个数
		返回值：
	*****************************************************************************************/
	void buildJSFunction(const char *jsfuncName, jsNativeFunction exeFunc, int argCount);

public:
	windowWnd* getWindow();
	std::wstring getURL() const;		// 得到首次setURL时设置的链接
	std::wstring getCurrentURL() const;	// 内部间导航的链接
protected:
	void initScrollBar(SIZE szScroll);
public:
	static void wkeWebkit_Init();
	static void wkeWebkit_Shutdown();

	static wkeWebkitUC* findWebkitUC(const wkeClientHandler *pHandler);
	static void removeClientHandler(const wkeClientHandler *pHandler);
	static std::map<const wkeClientHandler *, wkeWebkitUC *>	mc_mapClientHandler;
protected:
	wkeWebkitWnd	*m_pWindow;
};



}	// wuc
#endif	// __WUCWKEWEBKIT_H__