#include "resourceManager.h"

namespace gtc{

ULONG_PTR	resourceManager::mc_gdiToken = 0;			// GDI+
Gdiplus::GdiplusStartupInput resourceManager::mc_gdiInput;
HMODULE		resourceManager::mc_hModule = NULL;			// 资源所属库句柄
resourceManager::resourceManager()
	: m_procCommand(NULL)
{
	LOGFONTW lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	HFONT hDefaultFont = ::CreateFontIndirectW(&lf);
	m_bsDefaultFont.hFont = hDefaultFont;
	m_bsDefaultFont.wsFontName = lf.lfFaceName;
	m_bsDefaultFont.iSize = -lf.lfHeight;
	m_bsDefaultFont.bBold = (lf.lfWeight >= FW_BOLD);
	m_bsDefaultFont.bUnderline = (lf.lfUnderline == TRUE);
	m_bsDefaultFont.bItalic = (lf.lfItalic == TRUE);
	::ZeroMemory(&m_bsDefaultFont.tm, sizeof(m_bsDefaultFont.tm));
}


resourceManager::~resourceManager(void)
{
	free();
}

bool resourceManager::initEnvironment(HMODULE hModule)
{
	mc_hModule = hModule;
	// GDI+初始化
	return Gdiplus::GdiplusStartup(&mc_gdiToken, &mc_gdiInput, NULL) == Gdiplus::Ok;
}
void resourceManager::releaseEnvironment()
{
	// 退出GDI+
	Gdiplus::GdiplusShutdown(mc_gdiToken);
}

void resourceManager::free()
{
	for (std::map<std::wstring, gtc::image*>::iterator iter = m_mapImageRes.begin(); iter != m_mapImageRes.end(); ++iter)
	{
		//gtc::app::writeVSLogs(L"resourceManager::free %s %08X", iter->first.c_str(), iter->second);
		delete iter->second;
	}
	m_mapImageRes.clear();

	// menu
	for (std::map<std::wstring, HMENU>::iterator iter = m_mapMenuRes.begin(); iter != m_mapMenuRes.end(); ++iter)
	{
		if(iter->second) ::DestroyMenu(iter->second);
	}
	m_mapMenuRes.clear();

	// font
	for (std::vector<BSFontInfoUC *>::iterator iter = m_vecFontRes.begin(); iter != m_vecFontRes.end(); ++iter)
	{
		if((*iter)->hFont) ::DeleteObject((*iter)->hFont);
		delete *iter;
	}
	m_vecFontRes.clear();
	if(m_bsDefaultFont.hFont) ::DeleteObject(m_bsDefaultFont.hFont);
	m_bsDefaultFont.hFont = NULL;
	// attribute
	m_mapAttributeList.clear();
}

gtc::image* resourceManager::addImage(const std::wstring &imgKey, UINT id, LPCTSTR pType /*= RT_RCDATA*/)
{
	imageResource *pimage = new imageResource();
	bool bl = pimage->load(id, pType, mc_hModule);
	if (!bl)
	{
		delete pimage;
		return nullptr;
	}
	std::map<std::wstring, gtc::image*>::iterator iter = m_mapImageRes.find(imgKey);
	if (iter != m_mapImageRes.end())
	{
		delete iter->second;
	}
	m_mapImageRes[imgKey] = pimage;
	return pimage;
}
gtc::image* resourceManager::addImage(const std::wstring &imgKey, PCWSTR pFile)
{
	image *pImage = new image();
	bool bl = pImage->load(pFile, true);
	if (!bl)
	{
		delete pImage;
		return nullptr;
	}

	std::map<std::wstring, gtc::image*>::iterator iter = m_mapImageRes.find(imgKey);
	if (iter != m_mapImageRes.end())
	{
		delete iter->second;
	}
	m_mapImageRes[imgKey] = pImage;
	return pImage;
}

gtc::image* resourceManager::findImage(const std::wstring &imgKey) const
{
	std::map<std::wstring, gtc::image*>::const_iterator iter = m_mapImageRes.find(imgKey);
	if (iter != m_mapImageRes.cend())
	{
		return iter->second;
	}
	
	return nullptr;
}


// 菜单
bool resourceManager::addMenu(const std::wstring &menuKey, UINT id)
{
	HMENU hMenu = ::LoadMenu(mc_hModule, MAKEINTRESOURCE(id));
	if(!hMenu) return false;
	std::map<std::wstring, HMENU>::iterator iter = m_mapMenuRes.find(menuKey);
	if (iter != m_mapMenuRes.end() && iter->second)
	{
		::DestroyMenu(iter->second);
	}
	m_mapMenuRes[menuKey] = hMenu;
	return true;
}

HMENU resourceManager::findMenu(const std::wstring &menuKey) const
{
	std::map<std::wstring, HMENU>::const_iterator iter = m_mapMenuRes.find(menuKey);
	if (iter != m_mapMenuRes.cend())
	{
		return iter->second;
	}
	return NULL;
}
int resourceManager::addFont(PCWSTR pFontName, int nSize, bool bBold /*= false*/, bool bUnderline /*= false*/, bool bItalic /*= false*/)
{
	LOGFONTW lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
	std::wcscpy(lf.lfFaceName, pFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;

	return addFont(&lf);
	//HFONT hFont = ::CreateFontIndirectW(&lf);
	//if( hFont == NULL ) return -1;

	//BSFontInfoUC* pFontInfo = new BSFontInfoUC();
	//if( !pFontInfo ) {
	//	::DeleteObject(hFont);
	//	return -1;
	//}
	//::ZeroMemory(pFontInfo, sizeof(BSFontInfoUC));
	//pFontInfo->hFont = hFont;
	//pFontInfo->wsFontName = pFontName;
	//pFontInfo->iSize = nSize;
	//pFontInfo->bBold = bBold;
	//pFontInfo->bUnderline = bUnderline;
	//pFontInfo->bItalic = bItalic;
	//// 
	//HDC hdc = ::GetDC(NULL);
	//if( hdc ) {
	//	HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);
	//	::GetTextMetrics(hdc, &pFontInfo->tm);
	//	::SelectObject(hdc, hOldFont);
	//	::ReleaseDC(NULL, hdc);
	//}
	//m_vecFontRes.push_back(pFontInfo);
	//return m_vecFontRes.size()-1;
}
int resourceManager::addFont(const BSFontInfo &fontInfo)
{
	return addFont(fontInfo.wsFontName.c_str(), fontInfo.iSize, fontInfo.bBold, fontInfo.bUnderline, fontInfo.bItalic);
}
int resourceManager::addFont(LOGFONTW *plf)
{
	assert(plf);
	if(!plf) return -1;
	HFONT hFont = ::CreateFontIndirectW(plf);
	if( hFont == NULL ) return -1;
	BSFontInfoUC* pFontInfo = new BSFontInfoUC();
	if( !pFontInfo ) {
		::DeleteObject(hFont);
		return -1;
	}
	//::ZeroMemory(pFontInfo, sizeof(BSFontInfoUC));
	pFontInfo->hFont = hFont;
	pFontInfo->wsFontName = plf->lfFaceName;
	pFontInfo->iSize = -plf->lfHeight;
	pFontInfo->bBold = plf->lfWeight >= FW_BOLD;
	pFontInfo->bUnderline = plf->lfUnderline != 0;
	pFontInfo->bItalic = plf->lfItalic != 0;
	// 
	HDC hdc = ::GetDC(NULL);
	if( hdc ) {
		HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);
		::GetTextMetrics(hdc, &pFontInfo->tm);
		::SelectObject(hdc, hOldFont);
		::ReleaseDC(NULL, hdc);
	}
	m_vecFontRes.push_back(pFontInfo);
	return m_vecFontRes.size()-1;
}
HFONT resourceManager::getFont(int fontIndex /*= -1*/)
{
	if(fontIndex < 0 || fontIndex >= (int)m_vecFontRes.size()) return m_bsDefaultFont.hFont;
	return m_vecFontRes[fontIndex]->hFont;
}
const BSFontInfoUC* resourceManager::findFontInfo(int fontIndex) const
{
	if(fontIndex < 0 || fontIndex >= (int)m_vecFontRes.size()) return &m_bsDefaultFont;
	return m_vecFontRes[fontIndex];
}
HFONT resourceManager::setDefaultFont(PCWSTR pFontName, int nSize, bool bBold /*= false*/, bool bUnderline /*= false*/, bool bItalic /*= false*/)
{
	LOGFONTW lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
	std::wcscpy(lf.lfFaceName, pFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;

	return setDefaultFont(&lf);
	//HFONT hFont = ::CreateFontIndirectW(&lf);
	//if( hFont == NULL ) return;

	//::DeleteObject(m_bsDefaultFont.hFont);
	//m_bsDefaultFont.hFont = hFont;
	//m_bsDefaultFont.wsFontName = pFontName;
	//m_bsDefaultFont.iSize = nSize;
	//m_bsDefaultFont.bBold = bBold;
	//m_bsDefaultFont.bUnderline = bUnderline;
	//m_bsDefaultFont.bItalic = bItalic;
	//::ZeroMemory(&m_bsDefaultFont.tm, sizeof(m_bsDefaultFont.tm));
	//// 
	//HDC hdc = ::GetDC(NULL);
	//if( hdc ) {
	//	HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);
	//	::GetTextMetrics(hdc, &m_bsDefaultFont.tm);
	//	::SelectObject(hdc, hOldFont);
	//	::ReleaseDC(NULL, hdc);
	//}
}

HFONT resourceManager::setDefaultFont(const BSFontInfo &fontInfo)
{
	return setDefaultFont(fontInfo.wsFontName.c_str(), fontInfo.iSize, fontInfo.bBold, fontInfo.bUnderline, fontInfo.bItalic);
}

HFONT resourceManager::setDefaultFont(LOGFONTW *plf)
{
	assert(plf);
	if(!plf) return NULL;
	HFONT hFont = ::CreateFontIndirectW(plf);
	if( hFont == NULL ) return NULL;
	::DeleteObject(m_bsDefaultFont.hFont);
	m_bsDefaultFont.hFont = hFont;
	m_bsDefaultFont.wsFontName = plf->lfFaceName;
	m_bsDefaultFont.iSize = -plf->lfHeight;
	m_bsDefaultFont.bBold = plf->lfWeight >= FW_BOLD;
	m_bsDefaultFont.bUnderline = plf->lfUnderline != 0;
	m_bsDefaultFont.bItalic = plf->lfItalic != 0;
	// 
	HDC hdc = ::GetDC(NULL);
	if( hdc ) {
		HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);
		::GetTextMetrics(hdc, &m_bsDefaultFont.tm);
		::SelectObject(hdc, hOldFont);
		::ReleaseDC(NULL, hdc);
	}

	return hFont;
}

bool resourceManager::addAttributeList(const std::wstring &wsControlKey, const std::wstring &wsAttrList)
{
	if(wsControlKey.find(L'=') != std::wstring::npos)
	{
		assert(!L"属性列表的键名中不能有“=”字符！！！");
		return false;
	}
	m_mapAttributeList[wsControlKey] = wsAttrList;
	return true;
}
PCWSTR resourceManager::findAttributeList(const std::wstring &wsControlKey)
{
	StringMapType::const_iterator iter = m_mapAttributeList.find(wsControlKey);
	if (iter != m_mapAttributeList.cend())
	{
		return iter->second.c_str();
	}
	return NULL;
}
bool resourceManager::registerCommandProc(COMMANDPROC procCommand)
{
	m_procCommand = procCommand;
	return m_procCommand != nullptr;
}

INT resourceManager::execCommand(UINT cmdID, void *pData)
{
	if(m_procCommand) return m_procCommand(cmdID, pData);
	return 0;
}

}