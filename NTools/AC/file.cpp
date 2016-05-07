#include "file.h"
#include "shellExt.h"
#include "highLevelAPI.h"	// 加载xp下不支持的API


namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	file类	2014-04-25
//	常用文件操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////


int WINAPI file::_EnumFileMapping(HANDLE hNameMappings, ENUMFILEMAPPROC lpfnFileMapProc, LPVOID pData)
{
	if(!hNameMappings) return -1;
	HANDLETOMAPPINGS *pHM = static_cast<HANDLETOMAPPINGS *>(hNameMappings);
	if(!pHM) return -2;

	int iNumOfNM = pHM->uNumberOfMappings;
	// 检查函数指针; 如果NULL, 直接返回影射数
	if(!lpfnFileMapProc)
		return iNumOfNM;
	// 枚举对象
	LPSHNAMEMAPPING lp = pHM->lpSHNameMapping;
	int i = 0;
	SHNAMEMAPPING shNM;
	while(i < iNumOfNM)
	{
		CopyMemory(&shNM, &lp[i++], sizeof(SHNAMEMAPPING)); 
		if(!lpfnFileMapProc(&shNM, pData))
			break;
	}
	// 返回实际处理的对象数
	return i;
}
bool file::fileAttributeInfo(PCWSTR pFullPath, BSFileAttribute &fileAttribute)
{
	assert(pFullPath && std::wcslen(pFullPath) > 0);
	if(!pFullPath || std::wcslen(pFullPath) <= 0) return false;

	memset(&fileAttribute, 0, sizeof(BSFileAttribute));
	bool bl = false;
#if defined(_WIN32)
	HANDLE hfind = NULL;
	WIN32_FIND_DATAW data = {0};
	try
	{
		hfind = ::FindFirstFileW(pFullPath, &data);
		if(hfind == INVALID_HANDLE_VALUE || hfind == (HANDLE)(LONG)INVALID_FILE_SIZE)
			bl = false;
		else
			bl = true;
		if(hfind != NULL)
		{
			FindClose(hfind);
			hfind = NULL;
		}
		if (bl)
		{
			fileAttribute.uiAttribute = data.dwFileAttributes;
			fileAttribute.dtCreate = convert::toTimet(data.ftCreationTime);
			fileAttribute.dtLastAccess = convert::toTimet(data.ftLastAccessTime);
			fileAttribute.dtLastWrite = convert::toTimet(data.ftLastWriteTime);
			fileAttribute.llSize = (int64_t(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
		}
	}
	catch(...)
	{
		bl =false;
	}
	if(hfind != NULL)
		FindClose(hfind);
#endif
	return bl;
}

std::wstring file::modifyFileNameBaseNumber(const std::wstring &wsFullPath, PCWSTR pSuffix /*= NULL*/)
{
	// 需要建立的文件名不存在，就可以直接用
	wchar_t pPath[MAX_PATH] = {0};
	std::swprintf(pPath, _countof(pPath), L"%s%s", wsFullPath.c_str(), pSuffix?pSuffix:L"");
	if (!PathFileExists(pPath))
		return pPath;

	memset(pPath, 0, sizeof(pPath));
	for (int idx = 2;; ++idx)
	{
		std::swprintf(pPath, _countof(pPath), L"%s (%d)%s", wsFullPath.c_str(), idx, pSuffix?pSuffix:L"");
		if (!PathFileExists(pPath))
			break;
	}

	return pPath;
}

bool file::updateFileTime(const std::wstring &wsFullPath, UINT timeFlag /*= FILE_ATTRIBUTE_ALL_TIME*/)
{
	bool bRet = false;
	HANDLE hFile = NULL;
	__try
	{
		if(!PathFileExists(wsFullPath.c_str())) __leave;
		FILETIME timeCreate = {0};
		FILETIME timeLastAcsess = {0};
		FILETIME timeModify = {0};

		SYSTEMTIME st;
		GetSystemTime(&st);							// gets current time
		SystemTimeToFileTime(&st, &timeCreate);		// converts to file time format
		SystemTimeToFileTime(&st, &timeLastAcsess);	// converts to file time format
		SystemTimeToFileTime(&st, &timeModify);		// converts to file time format

		hFile = ::CreateFile(wsFullPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if(!hFile || hFile == INVALID_HANDLE_VALUE) __leave;

		bRet = ::SetFileTime(hFile, (timeFlag & FILE_ATTRIBUTE_CREATE_TIME)? &timeCreate: NULL, 
			(timeFlag & FILE_ATTRIBUTE_LASTACCESS_TIME)? &timeLastAcsess: NULL,
			(timeFlag & FILE_ATTRIBUTE_LASTWRITE_TIME)? &timeModify : NULL) == TRUE;
		CloseHandle(hFile);
		hFile = NULL;
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::updateFileTime"))
	{
		WRITE_LOG(L"file::updateFileTime SEH异常 ");
	}

	if(hFile) CloseHandle(hFile);

	return bRet;
}

bool file::analysisFilePath(std::wstring wsFullPath, std::wstring &wsName
	, std::wstring *poutExtName /*= NULL*/, std::wstring *poutParentFolder /*= NULL*/)
{
	if(poutExtName) poutExtName->clear();
	if(poutParentFolder) poutParentFolder->clear();
	wsName.clear();
	stringUtil::trim(wsFullPath);
	if (::PathFileExists(wsFullPath.c_str()))
	{	// 采用shell解析
		IShellItem *pshitem = NULL;
		if (acm::shell::createShellItem(wsFullPath.c_str(), IID_PPV_ARGS(&pshitem)))
		{
			PWSTR pName = NULL;
			//std::wstring ws;
			// SIGDN_NORMALDISPLAY 显示名，当开启隐藏已知文件扩展名时，只得到文件名（不含扩展名）
			// SIGDN_PARENTRELATIVEPARSING 文件名，含扩展名
			pshitem->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pName);
			wsName = pName;
			CoTaskMemFree(pName);
			pName = NULL;
			if (poutParentFolder)
			{	// 得到父级目录
				IShellItem *pshiParent = NULL;
				if (SUCCEEDED(pshitem->GetParent(&pshiParent)))
				{
					pshiParent->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pName);
					*poutParentFolder = pName;
					CoTaskMemFree(pName);
					pName = NULL;
				}
				if(pshiParent) pshiParent->Release();
			}
			if (!PathIsDirectory(wsFullPath.c_str()))
			{
				std::wstring::size_type pos = wsName.rfind(L'.');
				if (pos != std::wstring::npos)
				{
					if(poutExtName) *poutExtName = wsName.substr(pos);
					wsName = wsName.substr(0, pos);
				}
			}
			//pshitem->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_PARENTRELATIVEEDITING, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_DESKTOPABSOLUTEEDITING, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_FILESYSPATH, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_URL, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_PARENTRELATIVEFORADDRESSBAR, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_PARENTRELATIVE, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
			//pshitem->GetDisplayName(SIGDN_PARENTRELATIVEFORUI, &pName);
			//ws = pName;
			//CoTaskMemFree(pName);
			//pName = NULL;
		}
		if(pshitem) pshitem->Release();
		return true;
	}
	if(stringUtil::endsWith(wsFullPath, L"\\"))
		wsFullPath = wsFullPath.substr(0, wsFullPath.length()-1);	// 去除最后一个“\”字符
	if(stringUtil::endsWith(wsFullPath, L"/"))
		wsFullPath = wsFullPath.substr(0, wsFullPath.length()-1);	// 去除最后一个“\”字符
	//if(!PathFileExists(wsFullPath.c_str())) return false;
	//bool isDir = ::PathIsDirectory(wsFullPath.c_str());
	if(/*isDir && */stringUtil::endsWith(wsFullPath, L":"))
	{	// 是“c:”这类格式
		if(poutParentFolder) *poutParentFolder = wsFullPath;
		wsName = wsFullPath.substr(0, wsFullPath.length()-1);
		return true;
	}
	std::wstring::size_type pos = wsFullPath.find_last_of(L"\\/");
	if (pos != std::wstring::npos)
	{
		if(poutParentFolder)
			*poutParentFolder = wsFullPath.substr(0, pos);
		wsName = wsFullPath.substr(pos+1);
		pos = wsName.rfind(L'.');
		if (pos != std::wstring::npos)
		{	// 如果名称含有“.”就假定是文件
			if (pos != std::wstring::npos)
			{
				if(poutExtName) *poutExtName = wsName.substr(pos);
				wsName = wsName.substr(0, pos);
			}
		}

		return true;
	}
	return false;
}

HANDLE file::openTemplateFile(PCWSTR pfileName)
{
	assert(pfileName);
	if(!pfileName || std::wcslen(pfileName) <= 0) return NULL;
	HANDLE hFile = NULL;
	wchar_t pPath[MAX_PATH] = {0};
	if (pfileName)
	{
		std::swprintf(pPath, _countof(pPath), L"%%SystemRoot%%\\shellnew\\%s", pfileName);
		std::wstring wstmp = app::expandEnvironmentString(pPath);
		if (PathFileExists(wstmp.c_str()))
		{
			hFile = CreateFile(wstmp.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		if (!hFile || hFile == INVALID_HANDLE_VALUE)
		{
			std::swprintf(pPath, _countof(pPath), L"%%USERPROFILE%%\\Templates\\%s", pfileName);
			wstmp = app::expandEnvironmentString(pPath);
			hFile = CreateFile(wstmp.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		if (!hFile || hFile == INVALID_HANDLE_VALUE)
		{
			std::swprintf(pPath, _countof(pPath), L"%%ALLUSERSPROFILE%%\\Templates\\%s", pfileName);
			wstmp = app::expandEnvironmentString(pPath);
			hFile = CreateFile(wstmp.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
	}
	if(hFile == INVALID_HANDLE_VALUE) hFile = NULL;
	return hFile;
}
bool file::createAndInitializeFileOperation(REFIID riid, void **ppv)
{
	if(ppv) *ppv = NULL;
	assert(ppv);
	if(!ppv) return false;
	// Create the IFileOperation object
	bool bRet = false;
	IFileOperation *pfo = NULL;
	HRESULT hr = CoCreateInstance(__uuidof(FileOperation), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
	if (SUCCEEDED(hr))
	{
		// Set the operation flags.  Turn off  all UI
		// from being shown to the user during the
		// operation.  This includes error, confirmation
		// and progress dialogs.
		//hr = pfo->SetOperationFlags(FOF_NO_UI);
		if (SUCCEEDED(hr))
		{
			if(SUCCEEDED(pfo->QueryInterface(riid, ppv)))
				bRet = true;
		}
		pfo->Release();
	}
	return bRet;
}

std::wstring file::createFolder(PCWSTR pRootName, PCWSTR pfolderName, DWORD nFlag /*= FOF_RENAMEONCOLLISION*/)
{
	assert(pRootName);
	assert(pfolderName);
	if(!pRootName || !pfolderName || std::wcslen(pRootName) <= 0 || std::wcslen(pfolderName) <= 0) return false;
	if(!PathFileExists(pRootName)) return false;
	bool bRet = false;
	std::wstring wsNewPath;
	//if (gtc::app::isWindowsVistaLater())	// 有可能重名，所以此处注释了
	//{
	//	bRet = createFolderEx(pRootName, pfolderName, nFlag);
	//}
	//else
	{
		// 
		wchar_t pPath[MAX_PATH] = {0};
		std::wcsncpy(pPath, pRootName, _countof(pPath));
		::PathAppend(pPath, pfolderName);

		assert(nFlag == FOF_RENAMEONCOLLISION);
		wsNewPath = gtc::file::modifyFileNameBaseNumber(pPath);
		if (wsNewPath.length() > 0 && !::CreateDirectory(wsNewPath.c_str(), NULL))
		{
			wsNewPath.clear();
			//bRet = true;
			//if (poutNew)
			//{
			//	IShellItem *pshitem = NULL;
			//	if (createShellItem(pRootName, pfolderName, IID_PPV_ARGS(&pshitem)))
			//	{
			//		*poutNew = pshitem;
			//		pshitem = NULL;
			//	}
			//	if(pshitem) pshitem->Release();
			//}
		}
	}
	return wsNewPath;
}

bool file::createFolderEx(PCWSTR pRootName, PCWSTR pfolderName, DWORD nFlag /*= FOF_RENAMEONCOLLISION*/, IShellItem **poutNew /*= NULL*/)
{
	//hr = pfo->NewItem(pRoot, FILE_ATTRIBUTE_DIRECTORY, pfolderName, NULL, NULL);
	if(poutNew) *poutNew = NULL;
	assert(pRootName);
	assert(pfolderName);
	if(!_fnSHCreateItemFromParsingName || !_fnSHCreateItemFromRelativeName) return false;
	if(!pRootName || !pfolderName || std::wcslen(pRootName) <= 0 || std::wcslen(pfolderName) <= 0) return false;
	if(!PathFileExists(pRootName)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemRoot = NULL;

	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pRootName, NULL, IID_PPV_ARGS(&pshitemRoot)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->NewItem(pshitemRoot, FILE_ATTRIBUTE_DIRECTORY, pfolderName, NULL, NULL);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		if (bRet && poutNew)
		{
			IShellItem *pshitem = NULL;	// 注意：pfolderName可能重名->怎么处理
			hr = _fnSHCreateItemFromRelativeName(pshitemRoot, pfolderName, NULL, IID_PPV_ARGS(&pshitem));
			if (SUCCEEDED(hr))
			{
				*poutNew = pshitem;
				pshitem = NULL;
			}
			if (pshitem)
			{
				pshitem->Release();
			}
		}
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::createFolderEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::createFolderEx SEH异常 ");
	}

	// 
	if(pshitemRoot) pshitemRoot->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::createFile(PCWSTR pRootName, PCWSTR pfileName, PCWSTR pTemplateName /*= NULL*/
	, DWORD nFlag /*= FOF_RENAMEONCOLLISION*/, IShellItem **poutNew /*= NULL*/)
{
	if(poutNew) *poutNew = NULL;
	assert(pRootName);
	assert(pfileName);
	if(!pRootName || !pfileName || std::wcslen(pRootName) <= 0 || std::wcslen(pfileName) <= 0) return false;
	if(!PathFileExists(pRootName)) return false;
	bool bRet = false;
	if (gtc::app::isWindowsVistaLater())
	{
		bRet = createFileEx(pRootName, pfileName, pTemplateName, nFlag, poutNew);
	}
	else
	{
		// 
		std::wstring wsName(pfileName), wsExt;
		std::wstring::size_type pos = wsName.rfind(L'.');
		if(pos != std::wstring::npos)
		{
			wsExt = wsName.substr(pos);
			wsName = wsName.substr(0, pos);
		}
		wchar_t pPath[MAX_PATH] = {0};
		std::wcsncpy(pPath, pRootName, _countof(pPath));
		::PathAppend(pPath, wsName.c_str());
		std::wstring wsPath = gtc::file::modifyFileNameBaseNumber(pPath, wsExt.c_str());
		if (wsPath.length() > 0)
		{
			HANDLE hTemplate = openTemplateFile(pTemplateName);
			HANDLE hFile = ::CreateFileW(wsPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, hTemplate);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				if (hTemplate && hTemplate != INVALID_HANDLE_VALUE)
				{	// 复制模板文件的数据
					LARGE_INTEGER large = {0};
					if (::GetFileSizeEx(hTemplate, &large) && large.LowPart > 0)
					{
						BYTE *pBuf = new BYTE[large.LowPart];
						DWORD dwRd = 0;
						if (::ReadFile(hTemplate, pBuf, large.LowPart, &dwRd, NULL))
						{
							::WriteFile(hFile, pBuf, large.LowPart, &dwRd, NULL);
						}
						delete pBuf;
					}
				}
				CloseHandle(hFile);
				bRet = true;
			}
			if(hTemplate != INVALID_HANDLE_VALUE && !hTemplate) CloseHandle(hTemplate);
		}
		if (bRet && poutNew)
		{
			IShellItem *pshitem = NULL;
			if (acm::shell::createShellItem(pRootName, (wsName+wsExt).c_str(), IID_PPV_ARGS(&pshitem)))
			{
				*poutNew = pshitem;
				pshitem = NULL;
			}
			if(pshitem) pshitem->Release();
		}
		////// 为了达到系统提示重命名，或同名时自动加序号 newfile(2).txt, newfile(3).txt...
		////// 方法：在app目录下建立一个临时文件，然后再移到目标文件内
		//std::wstring wsappPath = getAppPath();
		//wsappPath += L"\\ling.tmp.fl";
		//HANDLE hFile = ::CreateFileW(wsappPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		//if (hFile != INVALID_HANDLE_VALUE)
		//{
		//	CloseHandle(hFile);
		//	nFlag &= ~FOF_ALLOWUNDO;
		//	nFlag |= FOF_NOCONFIRMATION;
		//	bRet = moveFile(pRootName, wsappPath.c_str(), pfileName, nFlag);
		//	if (bRet && poutNew)
		//	{
		//		IShellItem *pshitem = NULL;
		//		HRESULT hr = createShellItem(pRootName, pfileName, IID_PPV_ARGS(&pshitem));
		//		if (SUCCEEDED(hr))
		//		{
		//			*poutNew = pshitem;
		//			pshitem = NULL;
		//		}
		//		if(pshitem) pshitem->Release();
		//	}
		//}
	}
	return bRet;
}
bool file::createFileEx(PCWSTR pRootName, PCWSTR pfileName, PCWSTR pTemplateName /*= NULL*/
	, DWORD nFlag /*= FOF_RENAMEONCOLLISION*/, IShellItem **poutNew /*= NULL*/)
{
	if(poutNew) *poutNew = NULL;
	assert(pRootName);
	assert(pfileName);
	if(!_fnSHCreateItemFromParsingName || !_fnSHCreateItemFromRelativeName) return false;
	if(!pRootName || !pfileName || std::wcslen(pRootName) <= 0 || std::wcslen(pfileName) <= 0) return false;
	if(!PathFileExists(pRootName)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemRoot = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pRootName, NULL, IID_PPV_ARGS(&pshitemRoot)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->NewItem(pshitemRoot, FILE_ATTRIBUTE_NORMAL, pfileName, pTemplateName, NULL);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		if (bRet && poutNew)
		{
			IShellItem *pshitem = NULL;
			hr = _fnSHCreateItemFromRelativeName(pshitemRoot, pfileName, NULL, IID_PPV_ARGS(&pshitem));
			if (SUCCEEDED(hr))
			{
				*poutNew = pshitem;
				pshitem = NULL;
			}
			if (pshitem)
			{
				pshitem->Release();
			}
		}
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::createFileEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::createFileEx SEH异常 ");
	}

	// 
	if(pshitemRoot) pshitemRoot->Release();
	if(pfo) pfo->Release();

	return bRet;
}
bool file::copyFile(PCWSTR pToFolder, PCWSTR pName, PCWSTR pFrom, DWORD nFlag /*= FOF_ALLOWUNDO|FOF_NOCONFIRMATION*/)
{
	assert(pToFolder && pName);
	assert(pFrom);
	assert(std::wcslen(pToFolder) > 0 && std::wcslen(pName) > 0);
	assert(std::wcslen(pFrom) > 0);
	if(!pToFolder || !pName || !pFrom || std::wcslen(pToFolder) <= 0 || std::wcslen(pName) <= 0 || std::wcslen(pFrom) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;
	if(!PathFileExists(pToFolder)) return false;
	bool bRet = false;
	CoInitialize(NULL);
	if (gtc::app::isWindowsVistaLater())
	{
		bRet = copyFileEx(pToFolder, pName, pFrom, nFlag);
	}
	else
	{
		wchar_t pToFile[MAX_PATH] = {0}, pFromFile[MAX_PATH];
		std::wcsncpy(pToFile, pToFolder, _countof(pToFile));
		std::wcsncpy(pFromFile, pFrom, _countof(pFromFile));
		PathAddBackslash(pToFile);
		std::wcsncat(pToFile, pName, _countof(pToFile));

		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_COPY;
		fileOP.pFrom = pFromFile;
		fileOP.pTo = pToFile;
		fileOP.fFlags = (WORD)nFlag;
		bRet = SHFileOperation(&fileOP) == 0;
	}
	CoUninitialize();
	return bRet;
}
bool file::copyFileEx(PCWSTR pToFolder, PCWSTR pName, PCWSTR pFrom, DWORD nFlag /*= FOF_ALLOWUNDO|FOF_NOCONFIRMATION*/)
{
	assert(pToFolder && pName);
	assert(pFrom);
	assert(std::wcslen(pToFolder) > 0 && std::wcslen(pName) > 0);
	assert(std::wcslen(pFrom) > 0);
	if(!_fnSHCreateItemFromParsingName) return false;
	if(!pToFolder || !pName || !pFrom || std::wcslen(pToFolder) <= 0 || std::wcslen(pName) <= 0 || std::wcslen(pFrom) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;
	if(!PathFileExists(pToFolder)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemFrom = NULL;
	IShellItem *pshitemToFolder = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pFrom, NULL, IID_PPV_ARGS(&pshitemFrom)))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemToFolder)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->CopyItem(pshitemFrom, pshitemToFolder, pName, NULL);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::copyFileEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::copyFileEx SEH异常 ");
	}

	// 
	if(pshitemFrom) pshitemFrom->Release();
	if(pshitemToFolder) pshitemToFolder->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::copyFiles(PCWSTR pToFolder, StringArrayType &vecFiles, 
	DWORD nFlag /*= FOF_ALLOWUNDO*/, ENUMFILEMAPPROC plfnFileMapProc /*= NULL*/, LPVOID pData /*= NULL*/)
{
	assert(pToFolder);
	if(!pToFolder || std::wcslen(pToFolder) <= 0 || vecFiles.size() <= 0) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	bool bRet = false;
	if (app::isWindowsVistaLater())
	{
		bRet = copyFilesEx(pToFolder, vecFiles, nFlag);
	}
	else
	{
		std::wstring wsToFolder(pToFolder);
		std::wstring wsFromFiles;
		for (StringArrayType::const_iterator iter = vecFiles.cbegin(); iter != vecFiles.cend(); ++iter)
		{
			if (::PathFileExists(iter->c_str()))
			{
				wsFromFiles.append(*iter);
				wsFromFiles.push_back(0);
			}
		}
		wsFromFiles.push_back(0);
		wsToFolder.push_back(0);
		wsToFolder.push_back(0);
		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_COPY;
		fileOP.pFrom = wsFromFiles.data();
		fileOP.pTo = wsToFolder.data();
		if(nFlag & FOF_RENAMEONCOLLISION && plfnFileMapProc)
			nFlag |= FOF_WANTMAPPINGHANDLE;
		fileOP.fFlags = (WORD)nFlag;
		int iret = SHFileOperation(&fileOP);

		if (fileOP.hNameMappings)
		{
			_EnumFileMapping(fileOP.hNameMappings, plfnFileMapProc, pData);
			SHFreeNameMappings(fileOP.hNameMappings);
		}
		bRet = iret == 0;
	}

	return bRet;
}
bool file::copyFilesEx(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pToFolder);
	if(!_fnSHCreateItemFromParsingName) return false;
	if(!pToFolder || std::wcslen(pToFolder) <= 0 || vecFiles.size() <= 0) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemTo = NULL;
	IShellItemArray *pshia = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		if(!acm::shell::createShellItemArray(vecFiles, IID_PPV_ARGS(&pshia))) __leave;
		//if(FAILED(SHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemToFolder)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->CopyItems(pshia, pshitemTo);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::copyFilesEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::copyFilesEx SEH异常 ");
	}
	// 
	if(pshitemTo) pshitemTo->Release();
	if(pshia) pshia->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::moveFile(PCWSTR pToFolder, PCWSTR pFrom, PCWSTR pNewName /*= NULL*/, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pToFolder);
	assert(pFrom);
	if(!pToFolder || !pFrom || std::wcslen(pToFolder) <= 0 || std::wcslen(pFrom) <= 0) return false;
	if(!PathIsDirectory(pToFolder) || !PathFileExists(pFrom)) return false;
	bool bRet = false;
	if (app::isWindowsVistaLater())
	{
		bRet = moveFileEx(pToFolder, pFrom, pNewName, nFlag);
	}
	else
	{
		wchar_t pto[MAX_PATH] = {0};
		std::wcsncpy(pto, pToFolder, _countof(pto));
		//PathAddBackslash(pto);

		std::wstring wsFrom(pFrom);
		wsFrom.push_back(0);
		wsFrom.push_back(0);
		if(pNewName && std::wcslen(pNewName) > 0)
		{
			::PathAppend(pto, pNewName);
		}
		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_MOVE;
		fileOP.pFrom = wsFrom.data();
		fileOP.pTo = pto;
		fileOP.fFlags = (WORD)nFlag;
		int iret = SHFileOperation(&fileOP);
		bRet = iret == 0;
	}

	return bRet;

}
bool file::moveFileEx(PCWSTR pToFolder, PCWSTR pFrom, PCWSTR pNewName /*= NULL*/, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pToFolder);
	assert(pFrom);
	assert(std::wcslen(pToFolder) > 0);
	assert(std::wcslen(pFrom) > 0);
	if(!_fnSHCreateItemFromParsingName) return false;
	if(!pToFolder || !pFrom || std::wcslen(pToFolder) <= 0 || std::wcslen(pFrom) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemFrom = NULL;
	IShellItem *pshitemToFolder = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pFrom, NULL, IID_PPV_ARGS(&pshitemFrom)))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemToFolder)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->MoveItem(pshitemFrom, pshitemToFolder, pNewName, NULL);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::moveFileEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::moveFileEx SEH异常 ");
	}

	// 
	if(pshitemFrom) pshitemFrom->Release();
	if(pshitemToFolder) pshitemToFolder->Release();
	if(pfo) pfo->Release();

	return bRet;
}
bool file::moveFiles(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pToFolder);
	if(!pToFolder || std::wcslen(pToFolder) <= 0 || vecFiles.size() <= 0) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	bool bRet = false;
	if (app::isWindowsVistaLater())
	{
		bRet = moveFilesEx(pToFolder, vecFiles, nFlag);
	}
	else
	{
		std::wstring wsToFolder(pToFolder);
		std::wstring wsFromFiles;
		for (StringArrayType::const_iterator iter = vecFiles.cbegin(); iter != vecFiles.cend(); ++iter)
		{
			if (::PathFileExists(iter->c_str()))
			{
				wsFromFiles.append(*iter);
				wsFromFiles.push_back(0);
			}
		}
		wsFromFiles.push_back(0);
		wsToFolder.push_back(0);
		wsToFolder.push_back(0);
		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_MOVE;
		fileOP.pFrom = wsFromFiles.data();
		fileOP.pTo = wsToFolder.data();
		fileOP.fFlags = (WORD)nFlag;
		int iret = SHFileOperation(&fileOP);
		bRet = iret == 0;
	}

	return bRet;
}
bool file::moveFilesEx(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pToFolder);
	if(!_fnSHCreateItemFromParsingName) return false;
	if(!pToFolder || std::wcslen(pToFolder) <= 0 || vecFiles.size() <= 0) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemTo = NULL;
	IShellItemArray *pshia = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		if(!acm::shell::createShellItemArray(vecFiles, IID_PPV_ARGS(&pshia))) __leave;
		//if(FAILED(SHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemToFolder)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->MoveItems(pshia, pshitemTo);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::moveFilesEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::moveFilesEx SEH异常 ");
	}
	// 
	if(pshitemTo) pshitemTo->Release();
	if(pshia) pshia->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::deleteFile(PCWSTR pFrom, DWORD nFlag /*= FOF_ALLOWUNDO|FOF_NOCONFIRMATION*/)
{
	assert(pFrom);
	if(!pFrom || std::wcslen(pFrom) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;
	bool bRet = false;
	if (app::isWindowsVistaLater())
	{
		bRet = deleteFileEx(pFrom, nFlag);
	}
	else
	{
		std::wstring wsFrom(pFrom);
		wsFrom.push_back(0);
		wsFrom.push_back(0);
		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_DELETE;
		fileOP.pFrom = wsFrom.data();
		fileOP.pTo = NULL;
		fileOP.fFlags = (WORD)nFlag;
		int iret = SHFileOperation(&fileOP);
		bRet = iret == 0;
	}

	return bRet;
}
bool file::deleteFileEx(PCWSTR pFrom, DWORD nFlag /*= FOF_ALLOWUNDO|FOF_NOCONFIRMATION*/)
{
	assert(pFrom);
	if(!_fnSHCreateItemFromParsingName) return false;
	if(!pFrom || std::wcslen(pFrom) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;

	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemFrom = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pFrom, NULL, IID_PPV_ARGS(&pshitemFrom)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->DeleteItem(pshitemFrom, NULL);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::deleteFileEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::deleteFileEx SEH异常 ");
	}

	// 
	if(pshitemFrom) pshitemFrom->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::deleteFiles(StringArrayType &vecFiles, DWORD nFlag /*= FOF_ALLOWUNDO|FOF_NOCONFIRMATION*/)
{
	assert(vecFiles.size() > 0);
	if(vecFiles.size() <= 0) return false;
	bool bRet = false;
	if (app::isWindowsVistaLater())
	{
		bRet = deleteFilesEx(vecFiles, nFlag);
	}
	else
	{
		std::wstring wsFromFiles;
		for (StringArrayType::const_iterator iter = vecFiles.cbegin(); iter != vecFiles.cend(); ++iter)
		{
			if (::PathFileExists(iter->c_str()))
			{
				wsFromFiles.append(*iter);
				wsFromFiles.push_back(0);
			}
		}
		wsFromFiles.push_back(0);

		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_DELETE;
		fileOP.pFrom = wsFromFiles.data();
		fileOP.pTo = NULL;
		fileOP.fFlags = (WORD)nFlag;
		int iret = SHFileOperation(&fileOP);
		bRet = iret == 0;
	}

	return bRet;
}
bool file::deleteFilesEx(StringArrayType &vecFiles, DWORD nFlag /*= FOF_ALLOWUNDO|FOF_NOCONFIRMATION*/)
{
	assert(vecFiles.size() > 0);
	if(vecFiles.size() <= 0) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItemArray *pshia = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(!acm::shell::createShellItemArray(vecFiles, IID_PPV_ARGS(&pshia))) __leave;
		//if(FAILED(SHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemToFolder)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->DeleteItems(pshia);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::deleteFilesEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::deleteFilesEx SEH异常 ");
	}
	// 
	if(pshia) pshia->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::renameFile(PCWSTR pFrom, PCWSTR pNewName, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pFrom);
	assert(pNewName);
	if(!pFrom || !pNewName || std::wcslen(pFrom) <= 0 || std::wcslen(pNewName) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;
	bool bRet = false;
	//if (app::isWindowsVistaLater())	// 不知为什么，这种方式系统不能立即显示，刷新才显示
	//{
	//	bRet = renameFileEx(pFrom, pNewName, nFlag);
	//}
	//else
	{
		std::wstring wsFrom(pFrom);
		std::wstring wsName(pNewName);
		//std::wstring wsName = wsFrom.substr(0, wsFrom.find_last_of(L"\\/")+1);
		//wsName += pNewName;

		wsFrom.push_back(0);
		wsFrom.push_back(0);
		wsName.push_back(0);
		wsName.push_back(0);
		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_RENAME;
		fileOP.pFrom = wsFrom.data();
		fileOP.pTo = wsName.data();
		fileOP.fFlags = (WORD)nFlag;
		int iret = SHFileOperation(&fileOP);
		bRet = iret == 0;
	}

	return bRet;
}

bool file::renameFileEx(PCWSTR pFrom, PCWSTR pNewName, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pFrom);
	assert(pNewName);
	if(!_fnSHCreateItemFromParsingName) return false;
	if(!pFrom || !pNewName || std::wcslen(pFrom) <= 0 || std::wcslen(pNewName) <= 0) return false;
	if(!PathFileExists(pFrom)) return false;

	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItem *pshitemFrom = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(FAILED(_fnSHCreateItemFromParsingName(pFrom, NULL, IID_PPV_ARGS(&pshitemFrom)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->RenameItem(pshitemFrom, pNewName, NULL);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::renameFileEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::renameFileEx SEH异常 ");
	}

	// 
	if(pshitemFrom) pshitemFrom->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::renameFiles(StringArrayType &vecFiles, PCWSTR pNewName, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pNewName);
	assert(vecFiles.size() > 0);
	if(!pNewName || std::wcslen(pNewName) <= 0 || vecFiles.size() <= 0) return false;
	bool bRet = false;
	if (app::isWindowsVistaLater())
	{
		bRet = renameFilesEx(vecFiles, pNewName, nFlag);
	}
	else
	{
		std::wstring wsFromFiles, wsToFiles, wstr;
		for (StringArrayType::const_iterator iter = vecFiles.cbegin(); iter != vecFiles.cend(); ++iter)
		{
			if (::PathFileExists(iter->c_str()))
			{
				wsFromFiles.append(*iter);
				wsFromFiles.push_back(0);

				wstr = iter->substr(0, iter->find_last_of(L"\\/")+1);
				wstr += pNewName;
				wsToFiles.append(wstr);
				wsToFiles.push_back(0);
			}
		}
		wsFromFiles.push_back(0);
		wsToFiles.push_back(0);

		SHFILEOPSTRUCT fileOP = {0};
		fileOP.wFunc = FO_MOVE;
		fileOP.pFrom = wsFromFiles.data();
		fileOP.pTo = wsToFiles.data();
		fileOP.fFlags = (WORD)nFlag|FOF_MULTIDESTFILES;
		int iret = SHFileOperation(&fileOP);
		bRet = iret == 0;
	}

	return bRet;
}
bool file::renameFilesEx(StringArrayType &vecFiles, PCWSTR pNewName, DWORD nFlag /*= FOF_ALLOWUNDO*/)
{
	assert(pNewName);
	assert(vecFiles.size() > 0);
	if(!pNewName || std::wcslen(pNewName) <= 0 || vecFiles.size() <= 0) return false;
	bool bRet = false;
	HRESULT hr = S_FALSE;
	IFileOperation *pfo = NULL;
	IShellItemArray *pshia = NULL;
	__try
	{
		if(!createAndInitializeFileOperation(IID_PPV_ARGS(&pfo))) __leave;
		if(!acm::shell::createShellItemArray(vecFiles, IID_PPV_ARGS(&pshia))) __leave;
		//if(FAILED(SHCreateItemFromParsingName(pToFolder, NULL, IID_PPV_ARGS(&pshitemToFolder)))) __leave;
		pfo->SetOperationFlags(nFlag);
		hr = pfo->RenameItems(pshia, pNewName);
		if(SUCCEEDED(hr)) hr = pfo->PerformOperations();

		bRet = SUCCEEDED(hr);
		//if(FAILED(SHCreateItemFromRelativeName(pshitemToFolder, pName, NULL, IID_PPV_ARGS(&pshitemTo)))) __leave;
		//PathAddBackslash(pDesktopPath);PathAppend
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::renameFilesEx"))
	{
		bRet = false;
		WRITE_LOG(L"file::renameFilesEx SEH异常 ");
	}
	// 
	if(pshia) pshia->Release();
	if(pfo) pfo->Release();

	return bRet;
}

bool file::createShortCut(PCWSTR pToFolder, PCWSTR pLnkName, PCWSTR ptargetFilePath
	, PCWSTR pParam /*= NULL*/, PCWSTR pIconFilePath /*= NULL*/, int iconIndex /*= 0*/)
{
	assert(pToFolder && pLnkName && ptargetFilePath);
	if(!pToFolder || !pLnkName || !ptargetFilePath || std::wcslen(pLnkName) <= 0) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	if(!PathFileExists(ptargetFilePath)) return false;
	std::wstring wsName(pLnkName);
	stringUtil::trimRight(wsName);
	if (stringUtil::endsWith(wsName, L".lnk"))
		wsName = wsName.substr(0, wsName.rfind(L'.'));
	wchar_t pPath[MAX_PATH] = {0};
	std::wcsncpy(pPath, pToFolder, _countof(pPath));
	PathAddBackslash(pPath);
	std::wcsncat(pPath, wsName.c_str(), _countof(pPath));
	std::wstring wslnkFile = modifyFileNameBaseNumber(pPath, L".lnk");
	if(wslnkFile.empty()) return false;

	bool bRet = false;
	IShellLink *pshLink = NULL;
	IPersistFile *pshPersist = NULL;
	HRESULT hr = S_FALSE;
	hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pshLink));
	if(SUCCEEDED(hr)) hr = pshLink->QueryInterface(IID_PPV_ARGS(&pshPersist));
	if (SUCCEEDED(hr))
	{
		pshLink->SetPath(ptargetFilePath);
		std::wcsncpy(pPath, ptargetFilePath, _countof(pPath));
		PathRemoveFileSpec(pPath);
		pshLink->SetWorkingDirectory(pPath);
		if(pParam) pshLink->SetArguments(pParam);
		if(pIconFilePath) pshLink->SetIconLocation(pIconFilePath, iconIndex);

		bRet = SUCCEEDED(pshPersist->Save(wslnkFile.c_str(), FALSE));
	}
	// release
	if(pshLink) pshLink->Release();
	if(pshPersist) pshPersist->Release();

	return bRet;
}
bool file::createShortCut(PCWSTR pToFolder, PCWSTR ptargetFilePath)
{
	assert(pToFolder && ptargetFilePath);
	if(!pToFolder || !ptargetFilePath) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	if(!PathFileExists(ptargetFilePath)) return false;
	std::wstring wsName;
	analysisFilePath(ptargetFilePath, wsName);
	if(wsName.empty()) return false;
	return createShortCut(pToFolder, wsName.c_str(), ptargetFilePath);
}

bool file::createShortCuts(PCWSTR pToFolder, StringArrayType &vecTargetFiles)
{
	assert(pToFolder);
	if(!pToFolder || vecTargetFiles.size() <= 0) return false;
	if(!PathIsDirectory(pToFolder)) return false;
	bool bRet = true;
	for (StringArrayType::const_iterator iter = vecTargetFiles.cbegin(); iter != vecTargetFiles.cend(); ++iter)
	{
		if (PathFileExists(iter->c_str()))
		{
			if(!createShortCut(pToFolder, iter->c_str())) bRet = false;
		}
	}
	return bRet;
}

bool file::undoDelete(StringArrayType &vecFiles, DATE dtThisLaterUTC)
{
	bool bRet = false;
	PIDLIST_ABSOLUTE pidlRecycleBin = NULL;
	IShellFolder *pfolderDesktop = NULL;
	IShellFolder2 *pfolderRBin = NULL;
	LPENUMIDLIST penumIdl = NULL;
	ULONG celt = 0;
	ITEMIDLIST *pidl = NULL;
	HRESULT hr = S_OK;
	wchar_t pSource[MAX_PATH] = {0}, pbuf[MAX_PATH];
	STRRET strr = {0};
	IIDLArrayType vecIDLs;

	//datetime dt = datetime::current();
	//COleDateTime dd = COleDateTime::GetCurrentTime()+COleDateTimeSpan(0,0,0,2);
	//CString str = dd.Format(_T("%Y-%m-%d %H:%M:%S"));
	//std::wstring ss = dt.format();
	//SYSTEMTIME st1 = {0}, st2 = {0};
	//DATE ddd = dt;
	//dt.asSystemTime(st1);
	//dd.GetAsSystemTime(st2);

	//dt.changeToUTC();
	//std::wstring ss1 = dt.format();
	//dt.changeToLocal();
	//std::wstring ss2 = dt.format();

	//dt = dtThisLaterUTC;
	//ss = dt.format();
	try
	{
		hr = SHGetDesktopFolder(&pfolderDesktop);
		if(FAILED(hr)) throw(NULL);// __leave;
		hr = SHGetFolderLocation(NULL, CSIDL_BITBUCKET, NULL, 0, &pidlRecycleBin);
		if(FAILED(hr)) throw(NULL);//__leave;
		hr = pfolderDesktop->BindToObject(pidlRecycleBin, NULL, IID_PPV_ARGS(&pfolderRBin));
		if(FAILED(hr)) throw(NULL);//__leave;

		hr = pfolderRBin->EnumObjects(NULL, SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN, &penumIdl);
		if(FAILED(hr)) throw(NULL);//__leave;
		while(SUCCEEDED(penumIdl->Next(1, &pidl, &celt)) && celt == 1)
		{
			VARIANT var;
			SHCOLUMNID column={PSGUID_DISPLACED,PID_DISPLACED_DATE};
			hr = pfolderRBin->GetDetailsEx(pidl, &column, &var);
			if (SUCCEEDED(hr) && var.date >= dtThisLaterUTC)
			{
				//datetime dt = var.date;
				//dt.changeToLocal();
				//SEND_LOG(L">>>>> 删除时间:<%s>", dt.format().c_str());

				//column.pid = PID_DISPLACED_FROM;
				//hr = pfolderRBin->GetDetailsEx(pidl, &column, &var);
				//if (SUCCEEDED(hr))
				//{
				//	CString str = var.bstrVal;
				//	std::wcsncpy(pSource, var.bstrVal, _countof(pSource));
				//	SEND_LOG(L">>>>> 文件原位置:<%s>", pSource);
				//}
				//SHELLDETAILS shds = {0};
				//pfolderRBin->GetDetailsOf(pidl, 0, &shds);

				hr = pfolderRBin->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strr);
				hr = StrRetToBuf(&strr, pidl, pbuf, _countof(pbuf));
				if(SUCCEEDED(hr))
				{
					StringArrayType::const_iterator iter = std::find(vecFiles.cbegin(), vecFiles.cend(), pbuf);
					if (iter != vecFiles.cend())
					{
						vecIDLs.push_back(ILClone(pidl));
					}
				}
			}

			if(pidl) ILFree(pidl);
			pidl = NULL;
		}
		// 准备还原
		if (vecIDLs.size() > 0)
		{
			acm::shell::execSystemMenuCommand(pfolderRBin, vecIDLs, acm::BEFileSyscmdFlagUndoDelete);
		}
	}
	//__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"file::undoDelete"))
	catch(...)
	{
		bRet = false;
		WRITE_LOG(L"file::undoDelete SEH异常 ");
	}
	// 
	if (vecIDLs.size() > 0)
	{
		for (int idx = 0; idx < (int)vecIDLs.size(); ++idx)
		{
			ILFree(vecIDLs[idx]);
		}
	}
	if(pidlRecycleBin) ILFree(pidlRecycleBin);
	if(penumIdl) penumIdl->Release();
	if(pfolderDesktop) pfolderDesktop->Release();
	if(pfolderRBin) pfolderRBin->Release();

	return bRet;
}
}