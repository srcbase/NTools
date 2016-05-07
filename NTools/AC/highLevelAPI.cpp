#include "highLevelAPI.h"
#include "sysupport.h"

fn_SHCreateItemFromParsingName			_fnSHCreateItemFromParsingName = NULL;
fn_SHCreateItemFromRelativeName			_fnSHCreateItemFromRelativeName = NULL;
fn_SHCreateShellItemArrayFromIDLists	_fnSHCreateShellItemArrayFromIDLists = NULL;

fn_ChangeWindowMessageFilter			_fnChangeWindowMessageFilter = NULL;
fn_ChangeWindowMessageFilterEx			_fnChangeWindowMessageFilterEx = NULL;

template<> acm::highLevelAPI* gtc::singleton<acm::highLevelAPI>::m_pSingleton = nullptr;
namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	higtLevelAPI  类		2015-4-10
//	该类仅提供一个加载和卸载dll的作用
////////////////////////////////////////////////////////////////////////////////////////////////////////
static highLevelAPI c_highAPI;

highLevelAPI::highLevelAPI()
{
	SEND_LOG(L"highLevelAPI init ... %08X", this);

	m_hmdlShell32 = LoadLibrary(L"Shell32.dll");
	if (m_hmdlShell32)
	{
		_fnSHCreateItemFromParsingName = (fn_SHCreateItemFromParsingName)GetProcAddress(m_hmdlShell32, "SHCreateItemFromParsingName");
		_fnSHCreateItemFromRelativeName = (fn_SHCreateItemFromRelativeName)GetProcAddress(m_hmdlShell32, "SHCreateItemFromRelativeName");
		_fnSHCreateShellItemArrayFromIDLists = (fn_SHCreateShellItemArrayFromIDLists)GetProcAddress(m_hmdlShell32, "SHCreateShellItemArrayFromIDLists");
	}

	m_hmdlUser32 = LoadLibrary(L"User32.dll");
	if (m_hmdlUser32)
	{
		_fnChangeWindowMessageFilter = (fn_ChangeWindowMessageFilter)GetProcAddress(m_hmdlUser32, "ChangeWindowMessageFilter");
		_fnChangeWindowMessageFilterEx = (fn_ChangeWindowMessageFilterEx)GetProcAddress(m_hmdlUser32, "ChangeWindowMessageFilterEx");
	}
}
highLevelAPI::~highLevelAPI()
{
	if(m_hmdlShell32) FreeLibrary(m_hmdlShell32);
	if(m_hmdlUser32) FreeLibrary(m_hmdlUser32);

	SEND_LOG(L"highLevelAPI free ... %08X", this);
}

}	// end acm