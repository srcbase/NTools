#ifndef __HIGHLEVELAPI_H__
#define __HIGHLEVELAPI_H__
/****************************************************************************************
	说明：本函数只为那些XP下不支持的API提供动态解析过程
*****************************************************************************************/

#include <Shobjidl.h>		// Shell32.dll
#include "singleton.h"

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-4-10 Shell32.dll API 相关声明	
typedef HRESULT (WINAPI* fn_SHCreateItemFromParsingName)(PCWSTR, IBindCtx *, REFIID, void **);
typedef HRESULT (WINAPI* fn_SHCreateItemFromRelativeName)(IShellItem *, PCWSTR, IBindCtx *, REFIID, void **ppv);
typedef HRESULT (WINAPI* fn_SHCreateShellItemArrayFromIDLists)(UINT, PCIDLIST_ABSOLUTE_ARRAY, IShellItemArray **);



extern fn_SHCreateItemFromParsingName			_fnSHCreateItemFromParsingName;
extern fn_SHCreateItemFromRelativeName			_fnSHCreateItemFromRelativeName;
extern fn_SHCreateShellItemArrayFromIDLists		_fnSHCreateShellItemArrayFromIDLists;

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-6-9 User32.dll API 相关声明
typedef BOOL (WINAPI* fn_ChangeWindowMessageFilter)(UINT  /*message*/, DWORD /*dwFlag*/);
typedef BOOL (WINAPI* fn_ChangeWindowMessageFilterEx )(HWND /*hWnd*/, UINT /*message*/, DWORD /*action*/, PCHANGEFILTERSTRUCT /*pChangeFilterStruct*/);

extern fn_ChangeWindowMessageFilter				_fnChangeWindowMessageFilter;
extern fn_ChangeWindowMessageFilterEx			_fnChangeWindowMessageFilterEx;

namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	higtLevelAPI  类		2015-4-10
//	该类仅提供一个加载和卸载dll的作用
////////////////////////////////////////////////////////////////////////////////////////////////////////
class highLevelAPI : public gtc::singleton<highLevelAPI>
{
public:
	highLevelAPI();
	~highLevelAPI();
public:
	static highLevelAPI& getSingleton()
	{
		assert(m_pSingleton);
		return *m_pSingleton;
	}

	static highLevelAPI* getSingletonPtr()
	{
		return m_pSingleton;
	}
private:
	HMODULE	m_hmdlShell32;	// Shell32.dll
	HMODULE m_hmdlUser32;	// User32.dll
};

}	// end acm
#endif	// end __HIGHLEVELAPI_H__