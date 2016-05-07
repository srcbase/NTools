#pragma once
#include "sysupport.h"
#include "image.h"
namespace gtc{


class resourceManager
{
public:
	// 菜单，按钮等命令的处理函数
	typedef INT  (CALLBACK *COMMANDPROC)(UINT cmdID, void *pData);
public:
	resourceManager();
	~resourceManager();

	void free();
	// 图片 png
	gtc::image* addImage(const std::wstring &imgKey, UINT id, LPCTSTR pType = RT_RCDATA);
	gtc::image* addImage(const std::wstring &imgKey, PCWSTR pFile);
	gtc::image* findImage(const std::wstring &imgKey) const;

	// 菜单
	bool addMenu(const std::wstring &menuKey, UINT id);
	HMENU findMenu(const std::wstring &menuKey) const;
	// 字体
	int addFont(PCWSTR pFontName, int nSize, bool bBold = false, bool bUnderline = false, bool bItalic = false);
	int addFont(const BSFontInfo &fontInfo);
	int addFont(LOGFONTW *plf);
	HFONT getFont(int fontIndex = -1);
	const BSFontInfoUC* findFontInfo(int fontIndex) const;
	HFONT setDefaultFont(PCWSTR pFontName, int nSize, bool bBold = false, bool bUnderline = false, bool bItalic = false);
	HFONT setDefaultFont(const BSFontInfo &fontInfo);
	HFONT setDefaultFont(LOGFONTW *plf);
	// 控件默认属性列表
	bool addAttributeList(const std::wstring &wsControlKey, const std::wstring &wsAttrList);
	PCWSTR findAttributeList(const std::wstring &wsControlKey);
	// 注册命令处理函数
	bool registerCommandProc(COMMANDPROC procCommand);
	INT execCommand(UINT cmdID, void *pData);
public:
	static bool initEnvironment(HMODULE hModule);
	static void releaseEnvironment();
public:
	HMODULE getModule() const {				return mc_hModule;							}
	void setModuble(HMODULE hModule) {		mc_hModule = hModule;						}
	//ULONG_PTR getGDIToken() const {			return m_gdiToken;							}
	//void setGDIToken(ULONG_PTR token) {		m_gdiToken = token;							}
protected:
	static ULONG_PTR	mc_gdiToken;			// GDI+
	static Gdiplus::GdiplusStartupInput mc_gdiInput;
	static HMODULE		mc_hModule;			// 资源所属库句柄
protected:
	std::map<std::wstring, gtc::image*>	m_mapImageRes;		// 图片集和key=用户定义的标记
	std::map<std::wstring, HMENU>		m_mapMenuRes;		// 菜单资源（都是顶级菜单）
	std::vector<BSFontInfoUC *>			m_vecFontRes;		// 字体资源
	BSFontInfoUC						m_bsDefaultFont;	// 默认字体
	StringMapType						m_mapAttributeList;	// 系统默认属性集合；key=用户定义的标记 value=属性列表

	COMMANDPROC		m_procCommand;	// 命令处理函数
};

}