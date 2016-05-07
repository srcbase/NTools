#include "shellExt.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include "sysupport.h"
#include "baseReg.h"
#include "AC/file.h"
#include "AC/highLevelAPI.h"	// 加载xp下不支持的API

namespace acm {


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	shell类	2014-12-26
//	windows shell 扩展
////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT shell::mc_uiSmallIconSize = 16;
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-21	静态私有
// 
static BOOL CALLBACK EnumDesktopWindowsProc( HWND hwnd, LPARAM lParam )
{
	HWND* phsyslist = (HWND *)lParam;
	wchar_t szClassName[MAX_PATH] = {0};
	GetClassNameW(hwnd, szClassName, MAX_PATH);
	if ( !_wcsicmp(szClassName, L"WorkerW") )
	{
		*phsyslist = FindWindowExW(hwnd, 0, L"ShellDLL_DefView", 0);
		if(*phsyslist)
		{
			*phsyslist = FindWindowExW(*phsyslist, 0, L"SysListView32", 0);
			if (*phsyslist)
			{
				*phsyslist = hwnd;
				return FALSE;
			}
		}
	}
	return TRUE;
}
bool shell::findDeskHandle(HWND &refProgman, HWND &refShelldll, HWND &refDeskListView)
{
	refProgman = refShelldll = refDeskListView = NULL;
	HWND hwnd = NULL;
	refProgman = FindWindowW(L"Progman", L"Program Manager");
	refShelldll = FindWindowExW(refProgman , 0, L"ShellDLL_DefView", 0);
	refDeskListView = FindWindowExW(refShelldll , 0, L"SysListView32", 0);

	if (!refProgman || !refShelldll || !refDeskListView)
	{
		::EnumWindows(EnumDesktopWindowsProc, (LPARAM)&hwnd);
		if(!refProgman) refProgman = hwnd;
		if(!hwnd) hwnd = refProgman;
		refShelldll = FindWindowExW(hwnd , 0, L"ShellDLL_DefView", 0);
		refDeskListView = FindWindowExW(refShelldll , 0, L"SysListView32", 0);
	}

	bool found = refProgman != NULL && refShelldll != NULL && refDeskListView != NULL;
	if (!found)
	{
		refProgman = refShelldll = refDeskListView = NULL;
	}
	return found;
}
bool shell::createShellItem(PCWSTR pRootName, PCWSTR pName, REFIID riid, void **ppv)
{
	if(ppv) *ppv = NULL;
	assert(pRootName);
	assert(pName);
	assert(ppv);
	if(!pRootName || !pName || !ppv || std::wcslen(pRootName) <= 0 || std::wcslen(pName) <= 0) return false;
	if(!PathFileExists(pRootName)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IShellItem *pshitem = NULL;
	if (_fnSHCreateItemFromRelativeName && createShellItem(pRootName, IID_PPV_ARGS(&pshitem)))
	{
		hr = _fnSHCreateItemFromRelativeName(pshitem, pName, NULL, riid, ppv);
		bRet = SUCCEEDED(hr);
		if(pshitem) pshitem->Release();
		pshitem = NULL;
	}
	else
	{
		wchar_t pFile[MAX_PATH] = {0};
		std::wcsncpy(pFile, pRootName, _countof(pFile));
		::PathAppend(pFile, pName);
		bRet = createShellItem(pFile, riid, ppv);
	}
	//HRESULT hr = S_FALSE;
	//IShellItem *pshitemRoot = NULL;
	//IShellItem *pshitem = NULL;
	//if(SUCCEEDED(hr = SHCreateItemFromParsingName(pRootName, NULL, IID_PPV_ARGS(&pshitemRoot))))
	//{
	//	hr = SHCreateItemFromRelativeName(pshitemRoot, pName, NULL, IID_PPV_ARGS(&pshitem));
	//	if (SUCCEEDED(hr))
	//	{
	//		if(SUCCEEDED(pshitem->QueryInterface(riid, ppv)))
	//			bRet = true;
	//	}
	//}
	//if(pshitem) pshitem->Release();
	//if(pshitemRoot) pshitemRoot->Release();

	return bRet;
}
bool shell::createShellItem(PCWSTR pFullPath, REFIID riid, void **ppv)
{
	if(ppv) *ppv = NULL;
	assert(pFullPath);
	assert(ppv);
	if(!pFullPath || !ppv || std::wcslen(pFullPath) <= 0) return S_FALSE;
	if(!PathFileExists(pFullPath)) return false;
	bool bRet = false;
	if(_fnSHCreateItemFromParsingName)
		bRet = SUCCEEDED(_fnSHCreateItemFromParsingName(pFullPath, NULL, riid, ppv));
	else
	{
		// XP 下
		HRESULT hr = S_FALSE;
		ITEMIDLIST *piidl = NULL;
		IShellItem *pshitem = NULL;
		hr = SHParseDisplayName(pFullPath, NULL, &piidl, 0, NULL);
		hr = SHCreateShellItem(NULL, NULL, piidl, &pshitem);
		hr = pshitem->QueryInterface(riid, ppv);
		bRet = SUCCEEDED(hr);
		if(piidl) ILFree(piidl);
		if(pshitem) pshitem->Release();

	}
	//if(pshfolder) pshfolder->Release();
	//IShellItem *pshitem = NULL;
	//if(SUCCEEDED(SHCreateItemFromParsingName(pFullPath, NULL, IID_PPV_ARGS(&pshitem))))
	//{
	//	if(SUCCEEDED(pshitem->QueryInterface(riid, ppv)))
	//		bRet = true;
	//}
	//if(pshitem) pshitem->Release();
	return bRet;
}

bool shell::createShellItemArray(const StringArrayType &vecFiles, REFIID riid, void **ppv)
{
	if(ppv) *ppv = NULL;
	assert(ppv);
	if(!_fnSHCreateShellItemArrayFromIDLists) return false;
	if(!ppv || vecFiles.size() <= 0) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	std::vector<PIDLIST_ABSOLUTE> vecIDL;
	LPITEMIDLIST pidl = NULL;
	for (StringArrayType::const_iterator iter = vecFiles.cbegin(); iter != vecFiles.cend(); ++iter)
	{
		hr = SHParseDisplayName(iter->c_str(), NULL, &pidl, 0, NULL);
		if(SUCCEEDED(hr)) vecIDL.push_back(pidl);
		pidl = NULL;
	}
	if (vecIDL.size() > 0)
	{
		IShellItemArray *psia = NULL;	// Windows Vista [desktop apps only]
		hr = _fnSHCreateShellItemArrayFromIDLists(vecIDL.size(), (LPCITEMIDLIST *)vecIDL.data(), &psia);
		if (SUCCEEDED(hr))
		{
			if(SUCCEEDED(psia->QueryInterface(riid, ppv)))
				bRet = true;
		}
		if(psia) psia->Release();
		// release
		for (std::vector<PIDLIST_ABSOLUTE>::iterator iter = vecIDL.begin(); iter != vecIDL.end(); ++iter)
		{
			CoTaskMemFree(*iter);
		}
	}
	return bRet;
}

void shell::free(IIDLArrayType &vecIIDLs)
{
	for (IIDLArrayType::iterator iter = vecIIDLs.begin(); iter != vecIIDLs.end(); ++iter)
	{
		ILFree(*iter);	
	}
	vecIIDLs.clear();
}
HBITMAP shell::createThumbnail(PCWSTR pfullPath, int imgCXCY)
{
	// SIIGBF_RESIZETOFIT 保持纵横比，取大的尺寸作为图像尺寸
	// SIIGBF_BIGGERSIZEOK 将图像尺寸调整到32*32 48*48 256*256 
	// SIIGBF_INCACHEONLY 支持异尺寸 如：130*130
	assert(pfullPath && std::wcslen(pfullPath) > 0);
	if(!pfullPath || std::wcslen(pfullPath) <= 0) return NULL;
	if(imgCXCY <= 1) imgCXCY = 1;
	bool bOk = false;
	SIZE szImage = {imgCXCY, imgCXCY};
	HBITMAP hBitmap = NULL;
	HRESULT hr = S_OK;
	IShellItemImageFactory *pImageFactory = NULL;
	IShellFolder * pDesktop = NULL; 
	IExtractImage * pextImage = NULL;
	LPITEMIDLIST pidl = NULL;

	static fnSHCreateItemFromParsingName _SHCreateItemFromParsingName = NULL;
	__try
	{
		if (gtc::app::isWindowsVistaLater())
		{
			if (_SHCreateItemFromParsingName == NULL)
			{
				HMODULE hMdl = LoadLibraryW(L"shell32.dll");
				if (hMdl)
				{
					_SHCreateItemFromParsingName = (fnSHCreateItemFromParsingName)GetProcAddress(hMdl, "SHCreateItemFromParsingName");
					::FreeLibrary(hMdl);
				}
			}
			if (_SHCreateItemFromParsingName)
			{
				hr = _SHCreateItemFromParsingName(pfullPath, NULL, IID_PPV_ARGS(&pImageFactory));
				if (SUCCEEDED(hr))
				{
					hr = pImageFactory->GetImage(szImage, SIIGBF_RESIZETOFIT, &hBitmap);
					if (SUCCEEDED(hr))
					{
						bOk = true;
					}
					else
					{
						SEND_LOG(L"shell::createThumbnail 读取图像失败#%u name:<%s>", GetLastError(), pfullPath);
					}
				}
			}
		}
		else
		{	// xp 只能针对图片文件，如：jpg bmp等等
			hr = SHGetDesktopFolder(&pDesktop); 
			if (SUCCEEDED(hr))
			{ 
				wchar_t ppath[MAX_PATH] = {0};
				std::wcsncpy(ppath, pfullPath, _countof(ppath));
				hr = pDesktop->ParseDisplayName(NULL, 0, PathFindFileNameW(ppath), NULL, &pidl, NULL); 
				if (SUCCEEDED(hr))
				{
					
					hr = pDesktop->GetUIObjectOf(NULL,  1, (LPCITEMIDLIST *)&pidl, IID_IExtractImage, 0, (void **)&pextImage);
					if (SUCCEEDED(hr) && pextImage)
					{
						DWORD dwFlag = IEIFLAG_OFFLINE|IEIFLAG_ASPECT|IEIFLAG_ORIGSIZE| IEIFLAG_QUALITY;
						pextImage->GetLocation(ppath, MAX_PATH, NULL, &szImage, 32, &dwFlag);
						pextImage->Extract(&hBitmap);
					}
				}
			} 
		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::createThumbnail"))
	{
		if(hBitmap)
		{
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}
		WRITE_LOG(L"shell::createThumbnail SEH异常 name:<%s>", pfullPath);
	}
	if(pidl) ILFree(pidl);
	if(pImageFactory) pImageFactory->Release();
	if(pextImage) pextImage->Release();
	if(pDesktop) pDesktop->Release();

	return hBitmap;
}

bool shell::parseLinkTarget(PCWSTR plnkPath, std::wstring &wsTargetPath, std::wstring *pwsTargetParam /*= NULL*/)
{
	if(!plnkPath || std::wcslen(plnkPath) <= 0) return false;
	bool bRet = false;
	IShellLinkW *pshLink = NULL;
	IPersistFile *pPersistFile = NULL;
	wchar_t ppath[MAX_PATH] = {0};
	__try
	{
		wsTargetPath.clear();
		if(pwsTargetParam) pwsTargetParam->clear();
		if(FAILED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pshLink)))
			__leave;
		if(FAILED(pshLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile)))
			__leave;
		if(FAILED(pPersistFile->Load(plnkPath, STGM_READ))) __leave;
		if(FAILED(pshLink->Resolve(NULL, SLR_NO_UI|SLR_NOUPDATE|SLR_NOSEARCH))) __leave;
		if(FAILED(pshLink->GetPath(ppath, MAX_PATH, NULL, SLGP_RAWPATH))) __leave;
		wsTargetPath = ppath;
		if (pwsTargetParam)
		{
			if(FAILED(pshLink->GetArguments(ppath, MAX_PATH))) __leave;
			*pwsTargetParam = ppath;
		}
		// 64位程序快捷方式查找可能会找到32的program files （x86）里
		if (!PathFileExistsW(wsTargetPath.c_str()))
		{
			if (gtc::app::isWindowsOS64())
			{
				std::wstring::size_type pos = wsTargetPath.find(L" (x86)");
				if (pos != std::wstring::npos)
				{
					wsTargetPath.erase(pos, 6);
				}
			}
		}
		bRet = PathFileExistsW(wsTargetPath.c_str()) == TRUE;
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::parseLinkTarget"))
	{
		bRet = false;
		WRITE_LOG(L"shell::parseLinkTarget SEH异常 name:<%s>", plnkPath);
	}

	// 
	if(pPersistFile) pPersistFile->Release();
	if(pshLink) pshLink->Release();
	return bRet;
}
bool shell::parseLinkIconFile(PCWSTR plnkPath, std::wstring &wsIconPath, int &iIconIndex)
{
	if(!plnkPath || std::wcslen(plnkPath) <= 0) return false;
	bool bRet = false;
	IShellLinkW *pshLink = NULL;
	IPersistFile *pPersistFile = NULL;
	wchar_t ppath[MAX_PATH] = {0};
	__try
	{
		wsIconPath.clear();
		iIconIndex = -1;
		if(FAILED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pshLink)))
			__leave;
		if(FAILED(pshLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile)))
			__leave;
		if(FAILED(pPersistFile->Load(plnkPath, 0))) __leave;
		if(FAILED(pshLink->Resolve(NULL, SLR_NO_UI | SLR_NOSEARCH))) __leave;
		if(FAILED(pshLink->GetIconLocation(ppath, MAX_PATH, &iIconIndex))) __leave;
		wsIconPath = ppath;
		bRet = true;
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::parseLinkTarget"))
	{
		bRet = false;
		WRITE_LOG(L"shell::parseLinkTarget SEH异常 name:<%s>", plnkPath);
	}
	// 
	if(pPersistFile) pPersistFile->Release();
	if(pshLink) pshLink->Release();
	return bRet;
}
bool shell::parseAssociateIconFile(PCWSTR pfullPath, std::wstring &wsIconPath, int &iIconIndex)
{
	if(!pfullPath || std::wcslen(pfullPath) <= 0) return false;
	bool bRet = false;
	wchar_t ppath[MAX_PATH] = {0};
	if (::PathIsUNCW(pfullPath))
	{
		GetSystemDirectoryW(ppath, _countof(ppath));
		if (gtc::app::isWindowsVistaLater())
		{
			::PathAppendW(ppath, L"imageres.dll");
			iIconIndex = 137;
		}
		else
		{
			::PathAppendW(ppath, L"Shell32.dll");
			iIconIndex = 85;
		}
		wsIconPath = ppath;
		bRet = true;
	}
	if (!bRet && PathIsDirectoryW(pfullPath))
	{
		//acm::regKey reg;
		std::wstring wsVal;
		std::vector<std::wstring> vecItem;
		acm::regKey::readString(HKEY_CLASSES_ROOT, L"Directory\\DefaultIcon", NULL, wsVal);
		if (gtc::convert::splitString(wsVal, vecItem, L','))
		{
			if (vecItem.size() == 2)
			{
				wsIconPath = vecItem[0];
				iIconIndex = std::wcstol(vecItem[1].c_str(), NULL, 10);
			}
		}
		//if (reg.open(HKEY_CLASSES_ROOT, L"Directory\\DefaultIcon"))
		//{
		//	if(reg.readString(NULL, wsVal) && gtc::convert::splitString(wsVal, vecItem, L','))
		//	{
		//		if (vecItem.size() == 2)
		//		{
		//			wsIconPath = vecItem[0];
		//			iIconIndex = std::wcstol(vecItem[1].c_str(), NULL, 10);
		//		}
		//	}
		//	reg.close();
		//}
		bRet = PathFileExistsW(wsIconPath.c_str()) == TRUE;
	}
	if (!bRet)
	{	// 通过文件扩展名从注册表中找
		std::wstring wsExtName = PathFindExtensionW(pfullPath);
		std::wstring wsType, wstr;
		acm::regKey reg;
		if (reg.open(HKEY_CLASSES_ROOT, wsExtName.c_str()))
		{
			reg.readString(NULL, wsType);
			reg.close();
		}
		wstr = wsType + L"\\DefaultIcon";
		if (reg.open(HKEY_CLASSES_ROOT, wstr.c_str()))
		{
			if (!reg.readString(NULL, wsIconPath))
			{
				reg.close();
				wstr = wsType + L"\\CurVer";
				if (reg.open(HKEY_CLASSES_ROOT, wstr.c_str()))
				{
					if (reg.readString(NULL, wsType))
					{
						reg.close();
						wstr = wsType + L"\\DefaultIcon";
						if (reg.open(HKEY_CLASSES_ROOT, wstr.c_str()))
						{
							reg.readString(NULL, wsIconPath);
							reg.close();
						}
					}
					reg.close();
				}
			}
			reg.close();
		}
		if (!wsIconPath.empty())
		{
			std::wstring wsVal;
			std::vector<std::wstring> vecItem;
			if(gtc::convert::splitString(wsIconPath, vecItem, L','))
			{
				if (vecItem.size() == 2)
				{
					wsIconPath = vecItem[0];
					iIconIndex = std::wcstol(vecItem[1].c_str(), NULL, 10);
				}
			}
			bRet = PathFileExistsW(wsIconPath.c_str()) == TRUE;
		}
		else
		{
			std::wstring wsShellKey = wsType + L"\\shell\\open\\command";
			if (reg.open(HKEY_CLASSES_ROOT, wsShellKey.c_str()))
			{
				if (reg.readString(NULL, wsIconPath))
				{
					std::wstring::size_type pos = wsIconPath.find(L".exe");
					if (pos != std::wstring::npos)
					{
						wsIconPath = wsIconPath.substr(0, pos);
						iIconIndex = 0;
						bRet = true;
					}
				}
			}
		}
	}
	return bRet;
}
bool shell::parseFileForIconFile(PCWSTR pfullPath, std::wstring &wsIconPath, int &iIconIndex)
{
	if(!pfullPath || std::wcslen(pfullPath) <= 0) return false;
	bool bRet = false;
	__try
	{
		if (std::wcscmp(PathFindExtensionW(pfullPath), L".exe") == 0 && !PathIsUNCW(pfullPath))
		{
			wsIconPath = pfullPath;
			iIconIndex = 0;
			bRet = true;
			__leave;
		}
		if (std::wcscmp(PathFindExtensionW(pfullPath), L".lnk") == 0)
		{
			if (parseLinkIconFile(pfullPath, wsIconPath, iIconIndex))
			{
				bRet = true;
				__leave;
			}
			if (parseLinkTarget(pfullPath, wsIconPath, NULL))
			{
				bRet = parseFileForIconFile(wsIconPath.c_str(), wsIconPath, iIconIndex);
				if(bRet) __leave;
			}
		}
		// 
		bRet = parseAssociateIconFile(pfullPath, wsIconPath, iIconIndex);
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::parseFileForIconFile"))
	{
		bRet = false;
		WRITE_LOG(L"shell::parseFileForIconFile SEH异常 name:<%s>", pfullPath);
	}
	return bRet;
}
bool shell::parseFileForIconFile(PCWSTR pfullPath, PWSTR pIconPath, DWORD dwIconPathLen, int &iIconIndex)
{
	if(!pIconPath || dwIconPathLen == 0) return false;
	std::wstring wsIconPath;
	pIconPath[0] = L'\0';
	bool bRet = parseFileForIconFile(pfullPath, wsIconPath, iIconIndex);
	if (bRet)
	{
		std::wcsncpy(pIconPath, wsIconPath.c_str(), dwIconPathLen);
	}
	return bRet;
}
bool shell::extractFileIcon(PCWSTR pfullPath, int iconCXCY, HICON &hIcon, HICON &hSmallIcon)
{
	assert(pfullPath);
	if(!pfullPath || std::wcslen(pfullPath) <= 0) return false;
	wchar_t pIconFile[MAX_PATH] = {0};
	int iIconIndex = -1;
	PIDLIST_ABSOLUTE pidlDir = NULL;
	PIDLIST_RELATIVE pidlFile = NULL;
	IShellFolder *pshFolder = NULL;
	IExtractIconW *pextIcon = NULL;
	HRESULT hr = S_OK;
	hIcon = hSmallIcon = NULL;
	bool bRet = false;
	__try
	{
		if(iconCXCY < (int)mc_uiSmallIconSize) iconCXCY = mc_uiSmallIconSize;
		if(iconCXCY > 256) iconCXCY = 256;
		
		//if (parseFileForIconFile(pfullPath, pIconFile, MAX_PATH, iIconIndex))
		{
			if(FAILED(SHParseDisplayName(pfullPath, NULL, &pidlDir, 0, NULL))) __leave;
			if(FAILED(SHBindToParent(pidlDir, IID_IShellFolder, (void**)&pshFolder, NULL))) __leave;
			pshFolder->ParseDisplayName(NULL, NULL, PathFindFileNameW(pfullPath), NULL, &pidlFile, NULL);
			if(!pidlFile)
				pidlFile = ILClone(pidlDir);
			if(FAILED(pshFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&pidlFile, IID_IExtractIcon, NULL, (void**)&pextIcon))) __leave;
			UINT nFlag = 0;
			hr = pextIcon->GetIconLocation(0, pIconFile, MAX_PATH, &iIconIndex, &nFlag);
			if (SUCCEEDED(hr)/*nFlag & GIL_NOTFILENAME*/)
			{
				//hr = pextIcon->Extract(L"C:\\Windows\\System32\\imageres.dll", -54, &hIcon, &hSmallIcon, MAKELONG(iconCXCY, mc_uiSmallIconSize));
				hr = pextIcon->Extract(pIconFile, iIconIndex, &hIcon, &hSmallIcon, MAKELONG(iconCXCY, mc_uiSmallIconSize));
				bRet = SUCCEEDED(hr);
			}
		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::extractFileIcon"))
	{
		bRet = false;
		if (hIcon)
		{
			DestroyIcon(hIcon);
			hIcon = NULL;
		}
		if (hSmallIcon)
		{
			DestroyIcon(hSmallIcon);
			hSmallIcon = NULL;
		}
		WRITE_LOG(L"shell::extractFileIcon SEH异常 name:<%s>", pfullPath);
	}
	// release
	if(pidlFile) ILFree(pidlFile);
	if(pidlDir) ILFree(pidlDir);
	if(pextIcon) pextIcon->Release();
	if(pshFolder) pshFolder->Release();
	return bRet;
}

bool shell::extractFolderIcon(int iconCXCY, HICON &hIcon, HICON &hSmallIcon)
{
	wchar_t pIconFile[MAX_PATH] = {0};
	int iIconIndex = -1;

	hIcon = hSmallIcon = NULL;
	if(iconCXCY < (int)mc_uiSmallIconSize) iconCXCY = mc_uiSmallIconSize;
	if(iconCXCY > 256) iconCXCY = 256;

	std::wstring wsVal, wsIconFile;
	std::vector<std::wstring> vecItem;
	acm::regKey::readString(HKEY_CLASSES_ROOT, L"Directory\\DefaultIcon", NULL, wsVal);
	if (gtc::convert::splitString(wsVal, vecItem, L','))
	{
		if (vecItem.size() == 2)
		{
			wsIconFile = vecItem[0];
			iIconIndex = std::wcstol(vecItem[1].c_str(), NULL, 10);
			UINT uID = 0;
			if (!wsIconFile.empty())
			{
				PrivateExtractIconsW(wsIconFile.c_str(), iIconIndex, iconCXCY, iconCXCY, &hIcon, &uID, 1, LR_LOADFROMFILE);
				if(hIcon == NULL)
				{
					HICON hIconL = NULL, hIconS = NULL;
					ExtractIconExW(wsIconFile.c_str(), iIconIndex, &hIconL, &hIconS, 1);
					hIcon = hIconL;
					if(hIconS) DestroyIcon(hIconS);
				}
				PrivateExtractIconsW(wsIconFile.c_str(), iIconIndex, mc_uiSmallIconSize, mc_uiSmallIconSize, &hSmallIcon, &uID, 1, LR_LOADFROMFILE);
				if(hSmallIcon == NULL)
				{
					HICON hIconL = NULL, hIconS = NULL;
					ExtractIconExW(wsIconFile.c_str(), iIconIndex, &hIconL, &hIconS, 1);
					hSmallIcon = hIconS;
					if(hIconL) DestroyIcon(hIconL);
				}
			}
		}
	}
	return hIcon != NULL && hSmallIcon != NULL;
}


HMENU shell::parseShellMenu(IShellFolder *pshFolder, IIDLArrayType &vecIIDLs, bool bRename /*= false*/
	, IContextMenu **poutContextMenu /*= NULL*/, HWND hwndOwner /*= NULL*/)
{
	if(vecIIDLs.size() <= 0) return NULL;
	HMENU hMenu = NULL;
	//IShellFolder *pshDesktop = NULL;
	IContextMenu *pshMenu = NULL;
	bool bFree = false;
	IShellExtInit *pShellExtInit = NULL;
	ITEMIDLIST *piidlDesktop = NULL;
	__try
	{
		if (!pshFolder)
		{
			bFree = true;
			if(FAILED(SHGetDesktopFolder(&pshFolder))) __leave;
		}

		if(FAILED(pshFolder->GetUIObjectOf(hwndOwner, vecIIDLs.size(), (LPCITEMIDLIST *)vecIIDLs.data(), 
			IID_IContextMenu, NULL, (void **)&pshMenu)))
			__leave;

			////if(FAILED(SHGetDesktopFolder(&pshDesktop))) __leave;
			//if(FAILED(SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP, &piidlDesktop))) __leave;
			////if(FAILED(pshDesktop->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&piidlDesktop, IID_IDataObject, NULL, (void **)&pDataObject)))
			////	__leave;
			//if(FAILED(pshMenu->QueryInterface(IID_IShellExtInit, (void **)&pShellExtInit)))
			//	__leave;
			//if(FAILED(pShellExtInit->Initialize(piidlDesktop, NULL, NULL))) __leave;

		IContextMenu *pcm = NULL;
		if (SUCCEEDED(pshMenu->QueryInterface(IID_IContextMenu3, (void**)&pcm)))
		{
			pshMenu->Release();
			pshMenu = pcm;
		}
		else if (SUCCEEDED(pshMenu->QueryInterface(IID_IContextMenu2, (void**)&pcm)))
		{
			pshMenu->Release();
			pshMenu = pcm;
		}

		hMenu = CreatePopupMenu();
		UINT uflag = CMF_NORMAL;
		if(bRename) uflag |= CMF_CANRENAME;
		if (FAILED(pshMenu->QueryContextMenu(hMenu, 0, SHELL_MENU_MIN_ID, SHELL_MENU_MAX_ID, uflag)))
		{
			if (hMenu)
			{
				DestroyMenu(hMenu);
				hMenu = NULL;
			}
			__leave;
		}
		if(poutContextMenu)
		{
			*poutContextMenu = pshMenu;
			pshMenu = NULL;
			//pshMenu->QueryInterface(IID_IContextMenu, (void **)poutContextMenu);
		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::parseShellMenu"))
	{
		if (hMenu)
		{
			DestroyMenu(hMenu);
			hMenu = NULL;
		}
		if(poutContextMenu && *poutContextMenu)
		{
			(*poutContextMenu)->Release();
			*poutContextMenu = NULL;
		}


		WRITE_LOG(L"shell::parseShellMenu SEH异常 ");
	}
	// 
	if(pshMenu) pshMenu->Release();
	if(bFree && pshFolder) pshFolder->Release();

	return hMenu;

}

bool shell::extractContextMenuHandle(PCWSTR pregKey, ContextMenuHandleArrayType &vecContextMenu)
{
	assert(pregKey);
	if(!pregKey || std::wcslen(pregKey) <= 0) return false;
	vecContextMenu.clear();

	acm::regKey reg;
	if (reg.open(HKEY_CLASSES_ROOT, pregKey, KEY_READ))
	{
		DWORD idx = 0;
		wchar_t psubKey[512] = {0};
		std::wstring wskeyName, wsCLSID;
		BSContextMenuHandle menuHandle;
		while(reg.enumKey(idx++, wskeyName))
		{
			acm::regKey regSub;
			std::swprintf(psubKey, _countof(psubKey), L"%s\\%s", pregKey, wskeyName.c_str());
			if (regSub.open(HKEY_CLASSES_ROOT, psubKey, KEY_READ))
			{
				if (regSub.readString(NULL, wsCLSID) && wsCLSID.length() > 0)
				{
					if (loadContextMenuHandleFromCLSID(wsCLSID.c_str(), menuHandle))
					{
						menuHandle.wsKeyName = wskeyName;
						menuHandle.wsClsid = wsCLSID;
						vecContextMenu.push_back(menuHandle);
					}
				}
				regSub.close();
			}
		}
	}

	return vecContextMenu.size() > 0;
}
bool shell::parseShellMenuFromContextMenuHandle(const BSContextMenuHandle &contextMenu, IContextMenu **pcontextMenu /*= NULL*/, 
	IContextMenu2 **pcontextMenu2 /*= NULL*/, IContextMenu3 **pcontextMenu3 /*= NULL*/)
{
	if(!contextMenu.pUnknown) return false;
	bool bRet = false;
	//IShellFolder *pshDesktop = NULL;
	//IDataObject *pDataObject = NULL;
	IShellExtInit *pShellExtInit = NULL;
	ITEMIDLIST *piidlDesktop = NULL;
	__try
	{
		//if(FAILED(SHGetDesktopFolder(&pshDesktop))) __leave;
		if(FAILED(SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP, &piidlDesktop))) __leave;
		//if(FAILED(pshDesktop->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&piidlDesktop, IID_IDataObject, NULL, (void **)&pDataObject)))
		//	__leave;
		if(FAILED(contextMenu.pUnknown->QueryInterface(IID_IShellExtInit, (void **)&pShellExtInit)))
			__leave;
		if(FAILED(pShellExtInit->Initialize(piidlDesktop, NULL, NULL))) __leave;
		if (pcontextMenu3) contextMenu.pUnknown->QueryInterface(IID_IContextMenu3, (void **)pcontextMenu3);
		if (pcontextMenu2) contextMenu.pUnknown->QueryInterface(IID_IContextMenu2, (void **)pcontextMenu2);
		if (pcontextMenu) contextMenu.pUnknown->QueryInterface(IID_IContextMenu, (void **)pcontextMenu);
		bRet = true;
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::parseShellMenuFromContextMenuHandle"))
	{
		bRet = false;
		if(pcontextMenu && *pcontextMenu)
		{
			(*pcontextMenu)->Release();
			*pcontextMenu = NULL;
		}
		if(pcontextMenu2 && *pcontextMenu2)
		{
			(*pcontextMenu2)->Release();
			*pcontextMenu2 = NULL;
		}
		if(pcontextMenu3 && *pcontextMenu3)
		{
			(*pcontextMenu3)->Release();
			*pcontextMenu3 = NULL;
		}

		WRITE_LOG(L"shell::parseShellMenuFromContextMenuHandle SEH异常 ");
	}

	if(piidlDesktop) ILFree(piidlDesktop);
	if(pShellExtInit) pShellExtInit->Release();
	//if(pDataObject) pDataObject->Release();
	//if(pshDesktop) pshDesktop->Release();
	return bRet;
}
bool shell::loadContextMenuHandleFromCLSID(PCWSTR pCLSID, BSContextMenuHandle &menuHandle)
{
	bool bRet = false;
	if(!pCLSID || std::wcslen(pCLSID) <= 0) return false;
	wchar_t pkeyCLSID[512] = {0};
	std::swprintf(pkeyCLSID, _countof(pkeyCLSID), L"Software\\Classes\\CLSID\\%s", pCLSID);
	
	acm::regKey reg, regSub;
	std::wstring wsdll;
	// 加载dll
	if (reg.open(HKEY_LOCAL_MACHINE, pkeyCLSID, KEY_READ))
	{
		if (regSub.open(reg, L"InProcServer32", KEY_READ))
		{
			if (regSub.readString(NULL, wsdll))
			{
				menuHandle.hDLL = LoadLibrary(wsdll.c_str());
			}
			regSub.close();
		}
		reg.close();
	}
	// 解析IUnknown*
	CLSID clsid = {0};
	if (SUCCEEDED(CLSIDFromString(pCLSID, &clsid)))
	{
		bRet = SUCCEEDED(CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&menuHandle.pUnknown));
	}
	if (!bRet)
	{
		if (menuHandle.hDLL)
		{
			FreeLibrary(menuHandle.hDLL);
			menuHandle.hDLL = NULL;
		}
		if (menuHandle.pUnknown)
		{
			menuHandle.pUnknown->Release();
			menuHandle.pUnknown = NULL;
		}
	}
	return bRet;
}

bool shell::extractStringFromDLL(PCWSTR pdllpath, UINT uiResID, std::wstring &wsOutValue,
	WORD wLanguageID /*= MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/)
{
	assert(pdllpath);
	if(!pdllpath || std::wcslen(pdllpath) <= 0) return false;
	bool bRet = false;
	HMODULE hMdl = NULL;
	HRSRC hResource = NULL;
	const BSStringResourceImage* pImage = NULL;
	wsOutValue.clear();
	__try
	{
		hMdl = LoadLibraryExW(pdllpath, NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
		if(!hMdl) __leave;
		/*
			The and operation (& static_cast<WORD>(~0)) protects the expression from being greater
			than WORD - this would cause a runtime error when the application is compiled with /RTCc flag.
		*/
		hResource = ::FindResourceExW(hMdl, (LPWSTR) RT_STRING, MAKEINTRESOURCEW( (((uiResID>>4)+1) & static_cast<WORD>(~0)) ), wLanguageID);
		if( hResource == NULL ) __leave;
		wchar_t pbuf[MAX_PATH] = {0};
		LoadString(hMdl, uiResID, pbuf, _countof(pbuf));
		wsOutValue = pbuf;
		//pImage = getStringResouceImage(hMdl, hResource, uiResID);
		//if(!pImage) __leave;
		//wsOutValue = pImage->chString;
		bRet = true;
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::extractStringFromDLL"))
	{
		bRet = false;

		WRITE_LOG(L"shell::extractStringFromDLL SEH异常 ");
	}
	// 
	if(hMdl) FreeLibrary(hMdl);

	return bRet;
}

bool shell::extractStringFromDLL(const std::wstring &wsregValue, std::wstring &wsOutValue, 
	WORD wLanguageID /*= MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/)
{
	if(wsregValue.length() < 2 || wsregValue.front() != L'@') return false;
	std::wstring wsdll;
	UINT uiID = 0;
	std::vector<std::wstring> vec;
	if (gtc::convert::splitString(wsregValue, vec, L',') && vec.size() == 2)
	{
		wsdll = vec[0];
		wsdll.erase(wsdll.begin());
		gtc::stringUtil::trim(wsdll, L"\"");
		wsdll = gtc::app::expandEnvironmentString(wsdll.c_str());
		uiID = std::abs(std::wcstol(vec[1].c_str(), NULL, 10));
		return extractStringFromDLL(wsdll.c_str(), uiID, wsOutValue, wLanguageID);
	}
	return false;
}
// 
//std::wstring shell::createNewFolder(const std::wstring &wsParentPath)
//{
//	if(!::PathFileExists(wsParentPath.c_str())) return L"";
//	wchar_t pPath[MAX_PATH] = {0};
//	std::wcsncpy(pPath, wsParentPath.c_str(), _countof(pPath));
//	::PathAppend(pPath, L"新建文件夹");
//
//	std::wstring wsPath = gtc::file::modifyFileNameBaseNumber(pPath);
//	if(::CreateDirectory(wsPath.c_str(), NULL)) return wsPath;
//	return L"";
//}
void shell::createNewLink(const std::wstring &wsParentPath)
{
	if(!PathFileExists(wsParentPath.c_str())) return;

	wchar_t pPath[MAX_PATH] = {0};
	std::wcsncpy(pPath, wsParentPath.c_str(), _countof(pPath));
	::PathAppend(pPath, L"新快捷方式");
	std::wstring wsPath = gtc::file::modifyFileNameBaseNumber(pPath, L".lnk");
	if (wsPath.length() > 0)
	{
		//wsPath += L".lnk";

		HANDLE hFile = ::CreateFile(wsPath.c_str(), GENERIC_READ | GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile || INVALID_HANDLE_VALUE == hFile)
			return;
		::CloseHandle(hFile);
		// 
		std::swprintf(pPath, _countof(pPath), L"appwiz.cpl,NewLinkHere %s", wsPath.c_str());

		ShellExecute(NULL, NULL, _T("rundll32.exe"), pPath, NULL, SW_SHOW);
	}
}
bool shell::execSystemMenuCommand(const StringArrayType &vecFullPath, BEFileSyscmdFlag cmdFlag, HWND hWnd /*= NULL*/, PCSTR pParameters /*= NULL*/)
{
	assert(vecFullPath.size() > 0);
	if(vecFullPath.size() == 0) return false;
	IIDLArrayType vecIIDLs;
	IShellFolder *pshell = NULL;
	ITEMIDLIST *pidl = ILCreateFromPath(vecFullPath.front().c_str());
	//ITEMIDLIST *pidlParse = NULL;

	if(pidl) SHBindToParent(pidl, IID_PPV_ARGS(&pshell), NULL);
	if(!pshell) return false;
	for (StringArrayType::const_iterator iter = vecFullPath.cbegin(); iter != vecFullPath.cend(); ++iter)
	{
		pidl = NULL;
		pshell->ParseDisplayName(NULL, NULL, PathFindFileName(iter->c_str()), NULL, &pidl, NULL);
		if(!pidl) pidl = ILCreateFromPath(iter->c_str());
		if(pidl) vecIIDLs.push_back(pidl);
	}
	bool bRet = false;
	if (vecIIDLs.size() > 0)
	{
		bRet = execSystemMenuCommand(pshell, vecIIDLs, cmdFlag, hWnd, pParameters);
		free(vecIIDLs);
	}
	// 
	if(pshell) pshell->Release();

	return bRet;
}
bool shell::execSystemMenuCommand(std::wstring wsFullPath, BEFileSyscmdFlag cmdFlag, HWND hWnd /*= NULL*/, PCSTR pParameters /*= NULL*/)
{
	assert(wsFullPath.length() > 0);
	if(wsFullPath.empty()) return false;
	IShellFolder *pshell = NULL;
	ITEMIDLIST *pidl = ILCreateFromPath(wsFullPath.c_str());
	ITEMIDLIST *pidlParse = NULL;
	if (SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pshell), NULL)))
	{
		pshell->ParseDisplayName(NULL, NULL, PathFindFileName(wsFullPath.c_str()), NULL, &pidlParse, NULL);
		if(!pidlParse) pidlParse = ILClone(pidl);
	}

	IIDLArrayType vecIIDLs;
	bool bRet = false;
	if (pidl)
	{
		vecIIDLs.push_back(pidlParse);
		bRet = execSystemMenuCommand(pshell, vecIIDLs, cmdFlag, hWnd, pParameters);
	}
	if(pidl) ILFree(pidl);
	if(pidlParse) ILFree(pidlParse);
	if(pshell) pshell->Release();
	return bRet;
}
bool shell::execSystemMenuCommand(IShellFolder *pshFolder, IIDLArrayType &vecIIDLs, BEFileSyscmdFlag cmdFlag, HWND hWnd /*= NULL*/, PCSTR pParameters /*= NULL*/)
{
	bool bRet = false;
	UINT cmdID = -1;
	IContextMenu *pContextMenu = NULL;
	HRESULT hr = S_FALSE;
	HMENU hMenu = NULL;
	wchar_t pcmdFlag[MAX_PATH] = {0};
	wchar_t pbuf[MAX_PATH] = {0};
	__try
	{
		hMenu = parseShellMenu(pshFolder, vecIIDLs, true, &pContextMenu);
		if(!hMenu) __leave;
		// 命令标记
		switch(cmdFlag)
		{
		case BEFileSyscmdFlagOpen:			// 打开；区分标记：&o &O
			std::wcsncat(pcmdFlag,  L"打开(&O)", _countof(pcmdFlag));
			break;
		case BEFileSyscmdFlagCopy:			// 复制；区分标记：&c &C
			std::wcsncat(pcmdFlag,  L"复制(&C)", _countof(pcmdFlag));
			break;
		case BEFileSyscmdFlagCut:			// 剪切；区分标记：&t &T
			std::wcsncat(pcmdFlag,  L"剪切(&T)", _countof(pcmdFlag));
			break;
		case BEFileSyscmdFlagPaste:			// 粘贴；区分标记：&p &P
			//std::wcsncat(pcmdFlag,  L"&p", _countof(pcmdFlag));
			assert(!L"右键菜单中没有“粘贴”这一项");
			__leave;
			break;
		case BEFileSyscmdFlagDelete:		// 删除；区分标记：&d &D
			std::wcsncat(pcmdFlag,  L"删除(&D)", _countof(pcmdFlag));
			break;
		case BEFileSyscmdFlagUndoDelete:	// 回收站还原；区分标记：&E
			std::wcsncat(pcmdFlag,  L"还原(&E)", _countof(pcmdFlag));
			break;
		default:
			__leave;
			break;
		}
		// 查找“打开”项命令ID
		for (int idx = 0; idx < GetMenuItemCount(hMenu); ++idx)
		{
			::GetMenuString(hMenu, idx, pbuf, MAX_PATH, MF_BYPOSITION);
			if (std::wcsncmp(pbuf, pcmdFlag, _countof(pbuf)) == 0)
			{
				cmdID = ::GetMenuItemID(hMenu, idx);
				break;
			}
			//::GetMenuString(hMenu, idx, pbuf, MAX_PATH, MF_BYPOSITION);
			//cmdID = ::GetMenuItemID(hMenu, idx);
			//SEND_LOG(L"ERROR xxxxxxx ID:%d  name:<%s>", cmdID, pbuf);
		}
		if (cmdID == (UINT)-1 && cmdFlag == BEFileSyscmdFlagOpen)
		{
			for (int idx = 0; idx < GetMenuItemCount(hMenu); ++idx)
			{
				::GetMenuString(hMenu, idx, pbuf, MAX_PATH, MF_BYPOSITION);
				if (std::wcsncmp(pbuf, L"预览(&V)", _countof(pbuf)) == 0)
				{
					cmdID = ::GetMenuItemID(hMenu, idx);
					break;
				}
				//SEND_LOG(L"ERROR xxxxxxx ID:%d  name:<%s>", cmdID, pbuf);
			}
		}
		if (cmdID == -1)
		{
			int idx = GetMenuDefaultItem(hMenu, TRUE, GMDI_USEDISABLED);
			cmdID = GetMenuItemID(hMenu, idx);
		}
		if (cmdID != -1)
		{

			wchar_t szCmd[64];

			hr = pContextMenu->GetCommandString(cmdID - SHELL_MENU_MIN_ID, GCS_VERB,NULL, reinterpret_cast<LPSTR>(szCmd), ARRAYSIZE(szCmd));
			CMINVOKECOMMANDINFO cmi;
			cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
			if(cmdFlag != BEFileSyscmdFlagDelete)
			//	cmi.fMask = CMIC_MASK_CONTROL_DOWN/*|CMIC_MASK_FLAG_NO_UI*/;
			//else
				cmi.fMask = CMIC_MASK_FLAG_NO_UI;
			cmi.hwnd = hWnd;
			cmi.lpVerb = MAKEINTRESOURCEA(cmdID - SHELL_MENU_MIN_ID);
			cmi.lpParameters = pParameters;
			cmi.lpDirectory = NULL;
			cmi.nShow = SW_SHOWNORMAL;
			cmi.dwHotKey = 0;
			cmi.hIcon = NULL;

			hr = pContextMenu->InvokeCommand(&cmi);
			if (SUCCEEDED(hr))
			{
				if (cmdFlag == BEFileSyscmdFlagOpen)
				{
					SHAddToRecentDocs(SHARD_PIDL, vecIIDLs.front());
				}
				bRet = true;
			}
		}

	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::execSystemMenuCommand"))
	{
		bRet = false;

		WRITE_LOG(L"shell::execSystemMenuCommand SEH异常 ");
	}
	if(pContextMenu) pContextMenu->Release();
	if(hMenu) DestroyMenu(hMenu);
	return bRet;
}

bool shell::execSystemCommand(PCWSTR pcmdFile, PCWSTR pCmdVerb /*= L"open"*/, PCWSTR pParameters /*= NULL*/, HWND hWnd /*= NULL*/)
{
	SHELLEXECUTEINFO execInfo = {0};
	execInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	execInfo.hwnd = hWnd;
	execInfo.lpFile = pcmdFile;
	execInfo.lpParameters = pParameters;
	execInfo.lpVerb = pCmdVerb;
	execInfo.nShow = SW_SHOWNORMAL;

	return ShellExecuteEx(&execInfo) == TRUE;
}
bool shell::extractTipInfo(LPCITEMIDLIST pidl, std::wstring &refwsTip)
{
	refwsTip.clear();
	assert(pidl);
	if(!pidl) return false;
	IShellFolder *pshellFolder = NULL;
	IQueryInfo *pQueryInfo = NULL;
	LPITEMIDLIST pidlRelative = NULL;
	PWSTR pTipInfo = NULL;
	HRESULT hr = S_OK;
	bool bRet = false;
	__try
	{
		if(FAILED(::SHBindToParent(pidl, IID_PPV_ARGS(&pshellFolder), (LPCITEMIDLIST *)&pidlRelative))) __leave;
		// 
		STRRET str;
		wchar_t pbuf[MAX_PATH] = {0};
		if (SUCCEEDED(pshellFolder->GetDisplayNameOf(pidlRelative, SHGDN_NORMAL, &str)))
		{
			StrRetToBuf(&str, pidlRelative, pbuf, _countof(pbuf));
		}
		if (SUCCEEDED(pshellFolder->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str)))
		{
			StrRetToBuf(&str, pidl, pbuf, _countof(pbuf));
		}

		if(FAILED(pshellFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&pidlRelative, IID_IQueryInfo, NULL, (void **)&pQueryInfo))) __leave;
		if (SUCCEEDED(pQueryInfo->GetInfoTip(QITIPF_USENAME, &pTipInfo)))
		{
			refwsTip = pTipInfo;
			CoTaskMemFree(pTipInfo);
			bRet = true;
		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"shell::extractTipInfo"))
	{
		bRet = false;

		WRITE_LOG(L"shell::extractTipInfo SEH异常 ");
	}
	// 
	if(pQueryInfo) pQueryInfo->Release();
	if(pshellFolder) pshellFolder->Release();

	return bRet;
}
bool shell::extractTipInfo(std::wstring wsFullPath, std::wstring &refwsTip)
{
	refwsTip.clear();
	ITEMIDLIST *pidl = NULL;
	if(FAILED(SHParseDisplayName(wsFullPath.c_str(), NULL, &pidl, 0, NULL))) return false;
	bool bRet = false;
	if (pidl)
	{
		bRet = extractTipInfo(pidl, refwsTip);
		ILFree(pidl);
	}
	return bRet;
}
const BSStringResourceImage* shell::getStringResouceImage(HINSTANCE hInstance, HRSRC hResource, UINT id)
{
	const BSStringResourceImage* pImage;
	const BSStringResourceImage* pImageEnd;
	ULONG nResourceSize;
	HGLOBAL hGlobal;
	UINT iIndex;

	hGlobal = ::LoadResource( hInstance, hResource );
	if( hGlobal == NULL )
	{
		return NULL;
	}

	pImage = (const BSStringResourceImage*)::LockResource( hGlobal );
	if( pImage == NULL )
	{
		return NULL;
	}

	nResourceSize = ::SizeofResource( hInstance, hResource );
	pImageEnd = (const BSStringResourceImage*)(LPBYTE( pImage )+nResourceSize);
	iIndex = id&0x000f;

	while( (iIndex > 0) && (pImage < pImageEnd) )
	{
		pImage = (const BSStringResourceImage*)(LPBYTE( pImage )+(sizeof( BSStringResourceImage )+(pImage->nLength*sizeof( WCHAR ))));
		iIndex--;
	}
	if( pImage >= pImageEnd )
	{
		return NULL;
	}
	if( pImage->nLength == 0 )
	{
		return NULL;
	}

	return pImage;
}


}	// acm