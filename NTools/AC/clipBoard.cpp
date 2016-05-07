#include "clipBoard.h"

namespace acm {


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	clipBoard 类		2015-2-26
//	剪切板操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT clipBoard::enumFormat(FormatETCArrayType &vecFormatETC)
{
	IDataObject *pDataObj = NULL;
	IEnumFORMATETC *pEnumFormat = NULL;
	HRESULT hr = S_OK;
	FORMATETC formatETC = {0};
	ULONG fetched = 0;

	__try
	{
		vecFormatETC.clear();

		if(FAILED(OleGetClipboard(&pDataObj))) __leave;
		if(FAILED(pDataObj->EnumFormatEtc(DATADIR_GET, &pEnumFormat))) __leave;
		while(SUCCEEDED(pEnumFormat->Next(1, &formatETC, &fetched)) && fetched == 1)
		{
			BSFormatETC fmETC = {0};
			fmETC.fetc = formatETC;
			wchar_t pName[MAX_PATH] = {0};
			int len = GetClipboardFormatName(formatETC.cfFormat, fmETC.pFormatName, MAX_PATH);
			fmETC.pFormatName[len] = L'\0';
			vecFormatETC.push_back(fmETC);
			memset(&formatETC, 0, sizeof(formatETC));
		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"clipBoard::enumFormat"))
	{
		WRITE_LOG(L"clipBoard::enumFormat SEH异常 ");
	}

	// 
	if(pEnumFormat) pEnumFormat->Release();
	if(pDataObj) pDataObj->Release();

	return vecFormatETC.size();
}

void clipBoard::free(ItemIDArrayType &vecItemID)
{
	for (ItemIDArrayType::iterator iter = vecItemID.begin(); iter != vecItemID.end(); ++iter)
	{
		if(iter->pIIDL) ILFree(iter->pIIDL);
	}
	vecItemID.clear();
}
UINT clipBoard::enumFile(StringArrayType *pvecFiles /*= NULL*/, HWND hWnd /*= NULL*/)
{
	//IDataObject *pDataObj = NULL;
	//IEnumFORMATETC *pEnumFormat = NULL;
	//HRESULT hr = S_OK;
	//ULONG fetched = 0;
	UINT fileCount = 0;
	HDROP hDrop = NULL;
	wchar_t pFile[MAX_PATH] = {0};
	//__try
	//{
	if(pvecFiles) pvecFiles->clear();

	if(!OpenClipboard(hWnd)) return 0;
	//UINT fm = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	hDrop = (HDROP)GetClipboardData(CF_HDROP);
	if(!hDrop) 
	{
		CloseClipboard();
		return 0;
	}
	fileCount = DragQueryFile(hDrop, -1, NULL, 0);
	// 得到文件列表
	if (pvecFiles)
	{
		for (UINT idx = 0; idx < fileCount; ++idx)
		{
			if (DragQueryFile(hDrop, idx, pFile, _countof(pFile)))
			{
				pvecFiles->push_back(pFile);
			}
		}
	}
	//UINT fm = RegisterClipboardFormat(CFSTR_SHELLIDLIST);
	//if(FAILED(OleGetClipboard(&pDataObj))) __leave;
	//if(FAILED(pDataObj->EnumFormatEtc(DATADIR_GET, &pEnumFormat))) __leave;
	//FORMATETC formatETC = {fm, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	//if(SUCCEEDED(pDataObj->QueryGetData(&formatETC)))
	//{
	//	STGMEDIUM stgm = {0};
	//	if (SUCCEEDED(pDataObj->GetData(&formatETC, &stgm)))
	//	{
	//		if (stgm.tymed == TYMED_HGLOBAL && stgm.hGlobal != NULL)
	//		{
	//			CIDA *pcida = (CIDA *)::GlobalLock(stgm.hGlobal);
	//			fileCount = pcida->cidl;
	//			if(fileCount > 0 && pvecItemID)
	//			{

	//				LPITEMIDLIST pidl = (LPITEMIDLIST)((LPBYTE)pcida + pcida->aoffset[0]);
	//				IShellFolder *pshFolderRoot = NULL;
	//				IShellFolder2 *pshFolder2 = NULL;
	//				hr = SHBindToObject(NULL, pidl, NULL, IID_IShellFolder, (void**)(&pshFolderRoot));
	//				for (int idx = 0; idx < pcida->cidl; ++idx)
	//				{
	//					STRRET strr = {0};
	//					PCUITEMID_CHILD  puidlLast = NULL;
	//					LPCITEMIDLIST pidlChild = (LPCITEMIDLIST)((LPBYTE)pcida + pcida->aoffset[idx+1]);
	//					hr = pshFolderRoot->GetDisplayNameOf(pidlChild, SHGDN_NORMAL | SHGDN_FORPARSING, &strr);
	//					//hr = SHBindToParent(pidlChild, IID_PPV_ARGS(&pshFolder2), &puidlLast);	// 这个地方也能直接解析到
	//					hr = SHBindToFolderIDListParent(pshFolderRoot, pidlChild, IID_IShellFolder2, (void**)&pshFolder2, &puidlLast);	// 这个也可以，XP不支持
	//					if (SUCCEEDED(hr))
	//					{
	//						_BSItemIDList itemID = {0};
	//						itemID.pIIDL = ILClone(puidlLast);
	//						
	//						// 全路径 或 ::{CLSID}
	//						hr = pshFolder2->GetDisplayNameOf(puidlLast, SHGDN_NORMAL | SHGDN_FORPARSING, &strr);
	//						if (SUCCEEDED(hr))
	//						{
	//							hr = StrRetToBuf(&strr, puidlLast, itemID.pFullPath, _countof(itemID.pFullPath));
	//							//if(SUCCEEDED(hr))
	//							//{
	//							//	std::wcsncpy(itemID.pFullPath, pName, _countof(itemID.pFullPath));
	//							//}
	//							if(strr.uType == STRRET_WSTR) CoTaskMemFree(strr.pOleStr);
	//						}
	//						pvecItemID->push_back(itemID);
	//					}
	//				}
	//			}

	//			::GlobalUnlock(stgm.hGlobal);
	//		}
	//		::ReleaseStgMedium(&stgm);
	//	}
	//}
	//}
	//__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"clipBoard::enumFormat"))
	//{
	//	fileCount = 0;
	//	WRITE_LOG(L"clipBoard::enumFormat SEH异常 ");
	//}

	//// 
	//if(pEnumFormat) pEnumFormat->Release();
	//if(pDataObj) pDataObj->Release();
	CloseClipboard();

	return fileCount;
	
	

}


}	// end acm