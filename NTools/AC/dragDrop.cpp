#include "dragDrop.h"

namespace acm {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dataObject  类		2015-3-6
//	托拽动作的数据封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
dataObject::dataObject()
	: m_lRefCount(1), m_iCurDataIndex(-1)
{
	
}
dataObject::~dataObject()
{
	m_lRefCount = 0;
	clear();
}

STDMETHODIMP dataObject::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =  
	{  
		QITABENT(dataObject, IDataObject),  
		{ 0 },  
	};
	return QISearch(this, qit, riid, ppv);  
}
STDMETHODIMP_(ULONG) dataObject::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}
STDMETHODIMP_(ULONG) dataObject::Release()
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);  
	if (0 == lRef)  
	{  
		delete this;  
	}  
	return m_lRefCount;  
}

STDMETHODIMP dataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	if(!pformatetcIn || !pmedium) return E_INVALIDARG;
	memset(pmedium, 0, sizeof(STGMEDIUM));

	if(SUCCEEDED(QueryGetData(pformatetcIn)) && m_iCurDataIndex >= 0 && m_iCurDataIndex < (int)m_vecData.size())
	{
		if (m_vecData[m_iCurDataIndex].pstgMedium->tymed != TYMED_NULL)
		{
			return copyMedium(pmedium, m_vecData[m_iCurDataIndex].pstgMedium, pformatetcIn->cfFormat);
		}
	}
	//for (DataStorageArrayType::iterator iter = m_vecData.begin(); iter != m_vecData.end(); ++iter)
	//{  
	//	if( (pformatetcIn->tymed & iter->pformatETC->tymed) &&  
	//		(pformatetcIn->dwAspect == iter->pformatETC->dwAspect) &&  
	//		(pformatetcIn->cfFormat == iter->pformatETC->cfFormat) )  
	//	{
	//		return copyMedium(pmedium, iter->pstgMedium, iter->pformatETC);
	//	}
	//}

	return DV_E_FORMATETC;
}
STDMETHODIMP dataObject::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	if(!pformatetc || !pmedium) return E_INVALIDARG;
	if(pformatetc->tymed != pmedium->tymed) return E_FAIL;

	FORMATETC* fetc = new FORMATETC;  
	STGMEDIUM* pStgMed = new STGMEDIUM;  
	ZeroMemory(fetc, sizeof(FORMATETC));  
	ZeroMemory(pStgMed, sizeof(STGMEDIUM));  

	*fetc = *pformatetc;  

	if ( TRUE == fRelease )  
	{  
		*pStgMed = *pmedium;  
	}  
	else  
	{  
		copyMedium(pStgMed, pmedium, pformatetc->cfFormat);  
	}  

	BSDataStorage dataEntry = { fetc, pStgMed };
	if (SUCCEEDED(QueryGetData(pformatetc)) && m_iCurDataIndex >= 0 && m_iCurDataIndex < (int)m_vecData.size())
	{
		if(m_vecData[m_iCurDataIndex].pformatETC) delete m_vecData[m_iCurDataIndex].pformatETC;
		if(m_vecData[m_iCurDataIndex].pstgMedium)
		{
			ReleaseStgMedium(m_vecData[m_iCurDataIndex].pstgMedium);
			delete m_vecData[m_iCurDataIndex].pstgMedium;
		}
		m_vecData[m_iCurDataIndex] = dataEntry;
	}
	else
	{
		m_vecData.push_back(dataEntry); 
	}
	return S_OK;
}
STDMETHODIMP dataObject::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
	UNREFERENCED_PARAMETER(pformatetc);  
	UNREFERENCED_PARAMETER(pmedium);  
	return E_NOTIMPL;
}
STDMETHODIMP dataObject::QueryGetData(FORMATETC *pformatetc)
{
	m_iCurDataIndex = -1;
	if(!pformatetc) return E_INVALIDARG;
	if(!(pformatetc->dwAspect & DVASPECT_CONTENT)) return DV_E_DVASPECT;
	HRESULT hr = DV_E_TYMED;
	for (DataStorageArrayType::iterator iter = m_vecData.begin(); iter != m_vecData.end(); ++iter)
	{
		if (iter->pformatETC->tymed & pformatetc->tymed)
		{
			if (iter->pformatETC->cfFormat == pformatetc->cfFormat)
			{
				m_iCurDataIndex = iter - m_vecData.begin();
				return S_OK;
			}
			else hr = DV_E_CLIPFORMAT;
		}
		else hr = DV_E_TYMED;
	}
	return hr;
}
STDMETHODIMP dataObject::GetCanonicalFormatEtc(FORMATETC *pformatetcIn, FORMATETC *pformatetcOut)
{
	if(pformatetcOut) pformatetcOut->ptd = NULL;
	return E_NOTIMPL;
}
STDMETHODIMP dataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	if(!ppenumFormatEtc) return E_INVALIDARG;
	*ppenumFormatEtc = NULL;  
	HRESULT hr = E_NOTIMPL;

	if ( DATADIR_GET == dwDirection )  
	{  
		std::vector<FORMATETC> vecFormatETC;
		for (DataStorageArrayType::iterator iter = m_vecData.begin(); iter != m_vecData.end(); ++iter)
		{
			if (iter->pformatETC)
			{
				vecFormatETC.push_back(*iter->pformatETC);
			}
		}
		hr = SHCreateStdEnumFmtEtc(vecFormatETC.size(), vecFormatETC.data(), ppenumFormatEtc);  
	}  
	return hr;
}
STDMETHODIMP dataObject::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSnk , DWORD *pdwConnection)
{
	UNREFERENCED_PARAMETER(pformatetc);  
	UNREFERENCED_PARAMETER(advf);  
	UNREFERENCED_PARAMETER(pAdvSnk);  
	UNREFERENCED_PARAMETER(pdwConnection);  
	return E_NOTIMPL;
}
STDMETHODIMP dataObject::DUnadvise(DWORD dwConnection)
{
	UNREFERENCED_PARAMETER(dwConnection);  
	return E_NOTIMPL;
}
STDMETHODIMP dataObject::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
	UNREFERENCED_PARAMETER(ppenumAdvise);  
	return E_NOTIMPL;
}

bool dataObject::copyMedium(STGMEDIUM* lpDest, STGMEDIUM* lpSource, CLIPFORMAT cfFormat)
{
	if (lpDest->tymed == TYMED_NULL)
	{
		assert(lpSource->tymed != TYMED_NULL);
		switch (lpSource->tymed)
		{
		case TYMED_ENHMF:
		case TYMED_HGLOBAL:
			assert(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));
			lpDest->tymed = lpSource->tymed;
			lpDest->hGlobal = NULL;
			break;  // fall through to CopyGlobalMemory case

		case TYMED_ISTREAM:
			lpDest->pstm = lpSource->pstm;
			lpDest->pstm->AddRef();
			lpDest->tymed = TYMED_ISTREAM;
			return TRUE;

		case TYMED_ISTORAGE:
			lpDest->pstg = lpSource->pstg;
			lpDest->pstg->AddRef();
			lpDest->tymed = TYMED_ISTORAGE;
			return TRUE;

		case TYMED_MFPICT:
			{
				// copy LPMETAFILEPICT struct + embedded HMETAFILE
				HGLOBAL hDest = gtc::app::copyGlobalMemory(NULL, lpSource->hGlobal);
				if (hDest == NULL)
					return FALSE;
				LPMETAFILEPICT lpPict = (LPMETAFILEPICT)::GlobalLock(hDest);
				assert(lpPict != NULL);
				lpPict->hMF = ::CopyMetaFile(lpPict->hMF, NULL);
				if (lpPict->hMF == NULL)
				{
					::GlobalUnlock(hDest);
					::GlobalFree(hDest);
					return FALSE;
				}
				::GlobalUnlock(hDest);

				// fill STGMEDIUM struct
				lpDest->hGlobal = hDest;
				lpDest->tymed = TYMED_MFPICT;
			}
			return TRUE;

		case TYMED_GDI:
			lpDest->tymed = TYMED_GDI;
			lpDest->hGlobal = NULL;
			break;

		case TYMED_FILE:
			{
				lpDest->tymed = TYMED_FILE;
				assert(lpSource->lpszFileName != NULL);
				if (lpSource->lpszFileName == NULL)
				{
					GTC_EXCEPT(gtc::baseException::BSExceptionCodeInvalidParams, L"文件名为空");
				}
				UINT cbSrc = static_cast<UINT>(wcslen(lpSource->lpszFileName));
				LPOLESTR szFileName = (LPOLESTR)::CoTaskMemAlloc((cbSrc+1)*sizeof(OLECHAR));
				lpDest->lpszFileName = szFileName;
				if (szFileName == NULL)
					return FALSE;

				::memcpy_s(szFileName, (cbSrc+1)*sizeof(OLECHAR), 
					lpSource->lpszFileName,  (cbSrc+1)*sizeof(OLECHAR));
				return true;
			}

			// unable to create + copy other TYMEDs
		default:
			return false;
		}
	}
	assert(lpDest->tymed == lpSource->tymed);

	switch (lpSource->tymed)
	{
	case TYMED_HGLOBAL:
		{
			HGLOBAL hDest = gtc::app::copyGlobalMemory(lpDest->hGlobal,	lpSource->hGlobal);
			if (hDest == NULL)
				return false;

			lpDest->hGlobal = hDest;
		}
		return true;

	case TYMED_ISTREAM:
		{
			assert(lpDest->pstm != NULL);
			assert(lpSource->pstm != NULL);

			// get the size of the source stream
			STATSTG stat;
			if (lpSource->pstm->Stat(&stat, STATFLAG_NONAME) != S_OK)
			{
				// unable to get size of source stream
				return false;
			}
			assert(stat.pwcsName == NULL);

			// always seek to zero before copy
			LARGE_INTEGER zero = { 0, 0 };
			lpDest->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
			lpSource->pstm->Seek(zero, STREAM_SEEK_SET, NULL);

			// copy source to destination
			if (lpSource->pstm->CopyTo(lpDest->pstm, stat.cbSize,
				NULL, NULL) != NULL)
			{
				// copy from source to dest failed
				return false;
			}

			// always seek to zero after copy
			lpDest->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
			lpSource->pstm->Seek(zero, STREAM_SEEK_SET, NULL);
		}
		return true;

	case TYMED_ISTORAGE:
		{
			assert(lpDest->pstg != NULL);
			assert(lpSource->pstg != NULL);

			// just copy source to destination
			if (lpSource->pstg->CopyTo(0, NULL, NULL, lpDest->pstg) != S_OK)
				return false;
		}
		return true;

	case TYMED_FILE:
		{
			assert(lpSource->lpszFileName != NULL);
			assert(lpDest->lpszFileName != NULL);
			std::wstring strSource(lpSource->lpszFileName ?lpSource->lpszFileName :L"");
			std::wstring strDest(lpDest->lpszFileName ?lpDest->lpszFileName :L"");
			return CopyFile(lpSource->lpszFileName ? strSource.c_str() : NULL,
				lpDest->lpszFileName ? strDest.c_str() : NULL, FALSE) == TRUE;
		}


	case TYMED_ENHMF:
	case TYMED_GDI:
		{
			assert(sizeof(HGLOBAL) == sizeof(HENHMETAFILE));

			// with TYMED_GDI cannot copy into existing HANDLE
			if (lpDest->hGlobal != NULL)
				return false;

			// otherwise, use OleDuplicateData for the copy
			lpDest->hGlobal = OleDuplicateData(lpSource->hGlobal, cfFormat, 0);
			if (lpDest->hGlobal == NULL)
				return false;
		}
		return true;

		// other TYMEDs cannot be copied
	default:
		return false;
	}
	//if(!pMedDest || !pMedSrc || !pFmtSrc) return E_INVALIDARG;
	//
	//switch(pMedSrc->tymed)  
	//{  
	//case TYMED_HGLOBAL:  
	//	pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);  
	//	break;  
	//case TYMED_GDI:  
	//	pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);  
	//	break;  
	//case TYMED_MFPICT:  
	//	pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);  
	//	break;  
	//case TYMED_ENHMF:  
	//	pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);  
	//	break;  
	//case TYMED_FILE:  
	//	pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);  
	//	break;  
	//case TYMED_ISTREAM:  
	//	pMedDest->pstm = pMedSrc->pstm;  
	//	pMedSrc->pstm->AddRef();  
	//	break;  
	//case TYMED_ISTORAGE:  
	//	pMedDest->pstg = pMedSrc->pstg;  
	//	pMedSrc->pstg->AddRef();  
	//	break;  
	//case TYMED_NULL:  
	//default:  
	//	break;  
	//}
	//pMedDest->tymed = pMedSrc->tymed;  
	//pMedDest->pUnkForRelease = NULL;  
	//if(pMedSrc->pUnkForRelease != NULL)  
	//{  
	//	pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;  
	//	pMedSrc->pUnkForRelease->AddRef();  
	//}  
	//return S_OK;
}

//bool dataObject::setClipboard(IDataObject *pdataObject)
//{
//	if(!pdataObject) return false;
//	OleInitialize(NULL);
//	HRESULT hr  = OleSetClipboard(pdataObject);
//	if (SUCCEEDED(hr))
//	{
//		hr = OleFlushClipboard();
//	}
//	OleUninitialize();
//
//	return SUCCEEDED(hr);
//}
bool dataObject::parseDropFile(IDataObject *pdataObject, std::vector<std::wstring> &vecFile)
{
	assert(pdataObject);
	vecFile.clear();
	if(!pdataObject) return false;

	FORMATETC formatEtc = {0};
	STGMEDIUM stgMedium = {0};
	if (getGlobalData(pdataObject, CF_HDROP, formatEtc, stgMedium))
	{
		HDROP hDrop = (HDROP)GlobalLock(stgMedium.hGlobal);
		if (hDrop)
		{
			wchar_t szFile[MAX_PATH] = {0};
			UINT nCount = ::DragQueryFile(hDrop, -1, NULL, 0);
			for(UINT idx = 0; idx < nCount; idx++ )
			{
				if(::DragQueryFile(hDrop, idx, szFile, _countof(szFile)))
					vecFile.push_back(szFile);
			}
		}
		GlobalUnlock(stgMedium.hGlobal);
		ReleaseStgMedium(&stgMedium);
	}
	//FORMATETC fmtetc = {CF_HDROP,0,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
	//STGMEDIUM stgmed = {0};
	//if (SUCCEEDED(pdataObject->QueryGetData(&fmtetc)))
	//{
	//	if (SUCCEEDED(pdataObject->GetData(&fmtetc, &stgmed)))
	//	{
	//		HDROP hDrop = (HDROP)GlobalLock(stgmed.hGlobal);
	//		if( hDrop!=NULL )
	//		{
	//			//UINT nFiles=0xFFFFFFFF;
	//			wchar_t szFile[MAX_PATH] = {0};
	//			INT nCount = ::DragQueryFile(hDrop,-1,NULL,0);
	//			for( INT i=0; i<nCount; i++ )
	//			{
	//				if(::DragQueryFile(hDrop,i,szFile,ARRAYSIZE(szFile)))
	//					vecFile.push_back(szFile);
	//			}
	//		}

	//		GlobalUnlock(stgmed.hGlobal); 
	//		ReleaseStgMedium(&stgmed);
	//	}
	//}
	return vecFile.size() > 0;
}

//bool dataObject::setTargetData(IDataObject *pDataObject)
//{
//	if(!pDataObject) return false;
//	UINT len = sizeof(CLSID_MyDocuments);
//	HGLOBAL hgDrop = GlobalAlloc(GMEM_MOVEABLE, len);
//	void *pData = static_cast<LPVOID>(GlobalLock(hgDrop));
//	memcpy(pData, &CLSID_MyDocuments, len);
//	GlobalUnlock(hgDrop);
//
//	FORMATETC fmtETC = {RegisterClipboardFormat(CFSTR_TARGETCLSID), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
//	STGMEDIUM stgMed = {0};
//	HRESULT hr = pDataObject->QueryGetData(&fmtETC);
//	if (SUCCEEDED(hr))
//	{
//		hr = pDataObject->GetData(&fmtETC, &stgMed);
//		if (SUCCEEDED(hr))
//		{
//			GlobalFree(stgMed.hGlobal);
//		}
//	}
//	//fmtETC.cfFormat = CF_LOCALE;
//	//hr = pDataObject->QueryGetData(&fmtETC);
//	//if (SUCCEEDED(hr))
//	//{
//	//	hr = pDataObject->GetData(&fmtETC, &stgMed);
//	//	if (SUCCEEDED(hr))
//	//	{
//	//		GlobalFree(stgMed.hGlobal);
//	//	}
//	//}
//
//	stgMed.tymed = fmtETC.tymed;
//	stgMed.hGlobal = hgDrop;
//	stgMed.pUnkForRelease = NULL;
//	hr = pDataObject->SetData(&fmtETC, &stgMed, TRUE);
//	return SUCCEEDED(hr);
//	//return setGlobalData(RegisterClipboardFormat(CFSTR_TARGETCLSID), &hgDrop);
//}

bool dataObject::setDropDescription(DROPIMAGETYPE dimgType, PCWSTR pMessage, PCWSTR pInsert /*= NULL*/)
{
	DROPDESCRIPTION dropDesc = {dimgType};
	if(pMessage) std::wcsncpy(dropDesc.szMessage, pMessage, _countof(dropDesc.szMessage));
	else dropDesc.szMessage[0] = L'\0';
	if(pInsert) std::wcsncpy(dropDesc.szInsert, pInsert, _countof(dropDesc.szInsert));
	else dropDesc.szInsert[0] = L'\0';
	HGLOBAL hGlobal = gtc::app::createGlobalHandle(&dropDesc, sizeof(dropDesc));

	bool bRet = false;
	//bRet = updateData(RegisterClipboardFormat(CFSTR_DROPDESCRIPTION), hGlobal);
	bRet = setGlobalData(this, CFSTR_DROPDESCRIPTION, hGlobal);
	if (!bRet)
	{
		GlobalFree(hGlobal);
		hGlobal = NULL;
	}
	return bRet;
}
bool dataObject::setDropDescription(DWORD dwEffect, const std::wstring &wsTarget)
{
	bool bl = false;
	DROPIMAGETYPE dimgType = DROPIMAGE_NONE;
	if(dwEffect == DROPEFFECT_MOVE)
	{
		dimgType = DROPIMAGE_MOVE;
		bl = setDropDescription(dimgType, L"移动到v %1", wsTarget.c_str());
		SEND_LOG(L"WARNING dataObject::setDropDescription 移动到:%d", bl);
	}
	else if(dwEffect == DROPEFFECT_COPY)
	{
		dimgType = DROPIMAGE_COPY;
		bl = setDropDescription(dimgType, L"复制到v %1", wsTarget.c_str());
		SEND_LOG(L"WARNING dataObject::setDropDescription 复制到:%d", bl);
	}
	else if(dwEffect == DROPEFFECT_LINK)
	{
		dimgType = DROPIMAGE_LINK;
		bl = setDropDescription(dimgType, L"在v %1 创建链接", wsTarget.c_str());
		SEND_LOG(L"WARNING dataObject::setDropDescription 在:%d", bl);
	}
	return bl;
}
std::size_t dataObject::enumFormat(IDataObject *pDataObject, FormatETCArrayType &vecFormatETC)
{
	if(!pDataObject) return 0;
	IEnumFORMATETC *pEnumFormat = NULL;
	HRESULT hr = S_OK;
	FORMATETC formatETC = {0};
	ULONG fetched = 0;

	__try
	{
		vecFormatETC.clear();

		if(FAILED(pDataObject->EnumFormatEtc(DATADIR_GET, &pEnumFormat))) __leave;
		while(SUCCEEDED(pEnumFormat->Next(1, &formatETC, &fetched)) && fetched == 1)
		{
			BSFormatETC fmETC = {0};
			fmETC.fetc = formatETC;
			wchar_t pName[MAX_PATH] = {0};
			int len = GetClipboardFormatName(formatETC.cfFormat, fmETC.pFormatName, MAX_PATH);
			fmETC.pFormatName[len] = L'\0';
			if (len == 0)
			{
				toStringCF(fmETC.fetc.cfFormat, fmETC.pFormatName, _countof(fmETC.pFormatName));
				//std::swprintf(fmETC.pFormatName, _countof(fmETC.pFormatName), L"%s", toStringCF(formatETC.cfFormat).c_str());
			}
			vecFormatETC.push_back(fmETC);
			memset(&formatETC, 0, sizeof(formatETC));
		}
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"dataObject::enumFormat"))
	{
		WRITE_LOG(L"dataObject::enumFormat SEH异常 ");
	}

	// 
	if(pEnumFormat) pEnumFormat->Release();

	return vecFormatETC.size();
}
bool dataObject::getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, FORMATETC& formatEtc, STGMEDIUM& stgMedium)
{
	assert(pDataObject);
	assert(pwsFormat);
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) <= 0) return false;
	return getGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), formatEtc, stgMedium);

}
bool dataObject::getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, FORMATETC& formatEtc, STGMEDIUM& stgMedium)
{
	assert(pDataObject);
	if(!pDataObject) return false;
	bool bRet = false;
	formatEtc.cfFormat = cfFormat;
	formatEtc.ptd = NULL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_HGLOBAL;
	if (SUCCEEDED(pDataObject->QueryGetData(&formatEtc)))
	{
		if (SUCCEEDED(pDataObject->GetData(&formatEtc, &stgMedium)))
		{
			bRet = (TYMED_HGLOBAL & stgMedium.tymed) != 0;
			if(!bRet)
				::ReleaseStgMedium(&stgMedium);
		}
	}
	return bRet;
}
bool dataObject::setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, HGLOBAL hgData, bool bForce /*= true*/)
{
	assert(pDataObject);
	assert(hgData);
	if(!pDataObject || !hgData) return false;
	bool bSet = false;
	FORMATETC formatEtc = {0};
	STGMEDIUM stgMedium = {0};
	if (getGlobalData(pDataObject, cfFormat, formatEtc, stgMedium))
	{
		if(stgMedium.hGlobal) ::GlobalFree(stgMedium.hGlobal);
		stgMedium.hGlobal = hgData;
		bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
		if (!bSet)									// not changed or setting failed
			::ReleaseStgMedium(&stgMedium);
	}
	else if (bForce || hgData)
	{
		stgMedium.hGlobal = hgData;
		stgMedium.tymed = TYMED_HGLOBAL;
		stgMedium.pUnkForRelease = NULL;
		bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
	}
	return bSet;
}

bool dataObject::setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, HGLOBAL hgData, bool bForce /*= true*/)
{
	assert(pDataObject);
	assert(pwsFormat);
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return setGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), hgData, bForce);
}
bool dataObject::getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, std::wstring &wsData)
{
	assert(pDataObject);
	wsData.clear();
	FORMATETC fmtetc = {0};
	STGMEDIUM stgmed = {0};
	if (getGlobalData(pDataObject, cfFormat, fmtetc, stgmed))
	{
		if (::GlobalSize(stgmed.hGlobal) > 0)
		{
			wsData = *(static_cast<PCWSTR>(::GlobalLock(stgmed.hGlobal)));
			::GlobalUnlock(stgmed.hGlobal);
			::ReleaseStgMedium(&stgmed);
			return true;
		}
	}
	return false;
	//if(!pDataObject) return false;
	//bool bRet = false;
	//FORMATETC fmtetc = {cfFormat,0,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
	//STGMEDIUM stgmed = {0};
	//if (SUCCEEDED(pDataObject->QueryGetData(&fmtetc)))
	//{
	//	if (SUCCEEDED(pDataObject->GetData(&fmtetc, &stgmed)))
	//	{
	//		PCWSTR pstr = (PCWSTR)GlobalLock(stgmed.hGlobal);
	//		if( pstr!=NULL )
	//		{
	//			wsData = pstr;
	//			bRet = true;
	//		}

	//		GlobalUnlock(stgmed.hGlobal); 
	//		ReleaseStgMedium(&stgmed);
	//	}
	//}
	//return bRet;
}
bool dataObject::getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, std::string &csData)
{
	assert(pDataObject);
	csData.clear();
	if(!pDataObject) return false;
	FORMATETC fmtetc = {0};
	STGMEDIUM stgmed = {0};
	if (getGlobalData(pDataObject, cfFormat, fmtetc, stgmed))
	{
		if (::GlobalSize(stgmed.hGlobal) > 0)
		{
			csData = *(static_cast<PCSTR>(::GlobalLock(stgmed.hGlobal)));
			::GlobalUnlock(stgmed.hGlobal);
			::ReleaseStgMedium(&stgmed);
			return true;
		}
	}
	return false;
	//bool bRet = false;
	//FORMATETC fmtetc = {cfFormat,0,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
	//STGMEDIUM stgmed = {0};
	//if (SUCCEEDED(pDataObject->QueryGetData(&fmtetc)))
	//{
	//	if (SUCCEEDED(pDataObject->GetData(&fmtetc, &stgmed)))
	//	{
	//		PCSTR pstr = (PCSTR)GlobalLock(stgmed.hGlobal);
	//		if( pstr!=NULL )
	//		{
	//			csData = pstr;
	//			bRet = true;
	//		}

	//		GlobalUnlock(stgmed.hGlobal); 
	//		ReleaseStgMedium(&stgmed);
	//	}
	//}
	//return bRet;
}
bool dataObject::getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, std::wstring &wsData)
{
	assert(pDataObject);
	assert(pwsFormat);
	wsData.clear();
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return getGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), wsData);
}
bool dataObject::getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, std::string &csData)
{
	assert(pDataObject);
	assert(pwsFormat);
	csData.clear();
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return getGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), csData);
}

bool dataObject::setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, const std::wstring &wsData, bool bForce /*= true*/)
{
	assert(pDataObject);
	if(!pDataObject) return false;
	bool bSet = false;
	FORMATETC formatEtc = {0};
	STGMEDIUM stgMedium = {0};
	if (getGlobalData(pDataObject, cfFormat, formatEtc, stgMedium))
	{
		PCWSTR pData = static_cast<PCWSTR>(::GlobalLock(stgMedium.hGlobal));
		bSet = (pData != wsData);
		::GlobalUnlock(stgMedium.hGlobal);
		if (bSet)
		{
			::GlobalFree(stgMedium.hGlobal);
			stgMedium.hGlobal = gtc::app::createGlobalHandle((const void*)wsData.data(), wsData.size()*sizeof(wchar_t));
			bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
		}
		if (!bSet)									// not changed or setting failed
			::ReleaseStgMedium(&stgMedium);
	}
	else if (bForce || !wsData.empty())
	{
		stgMedium.hGlobal = gtc::app::createGlobalHandle((const void*)wsData.data(), wsData.size()*sizeof(wchar_t));
		if (stgMedium.hGlobal)
		{
			stgMedium.tymed = TYMED_HGLOBAL;
			stgMedium.pUnkForRelease = NULL;
			bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
			if(!bSet)
				::GlobalFree(stgMedium.hGlobal);
		}
	}
	return bSet;
}
bool dataObject::setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, const std::string &csData, bool bForce /*= true*/)
{
	assert(pDataObject);
	if(!pDataObject) return false;
	bool bSet = false;
	FORMATETC formatEtc = {0};
	STGMEDIUM stgMedium = {0};
	if (getGlobalData(pDataObject, cfFormat, formatEtc, stgMedium))
	{
		PCSTR pData = static_cast<PCSTR>(::GlobalLock(stgMedium.hGlobal));
		bSet = (pData != csData);
		::GlobalUnlock(stgMedium.hGlobal);
		if (bSet)
		{
			::GlobalFree(stgMedium.hGlobal);
			stgMedium.hGlobal = gtc::app::createGlobalHandle((const void*)csData.data(), csData.size());
			bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
		}
		if (!bSet)									// not changed or setting failed
			::ReleaseStgMedium(&stgMedium);
	}
	else if (bForce || !csData.empty())
	{
		stgMedium.hGlobal = gtc::app::createGlobalHandle((const void*)csData.data(), csData.size());
		if (stgMedium.hGlobal)
		{
			stgMedium.tymed = TYMED_HGLOBAL;
			stgMedium.pUnkForRelease = NULL;
			bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
			if(!bSet)
				::GlobalFree(stgMedium.hGlobal);
		}
	}
	return bSet;
}
bool dataObject::setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, const std::wstring &wsData, bool bForce /*= true*/)
{
	assert(pDataObject);
	assert(pwsFormat);
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return setGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), wsData, bForce);
}
bool dataObject::setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, const std::string &csData, bool bForce /*= true*/)
{
	assert(pDataObject);
	assert(pwsFormat);
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return setGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), csData, bForce);

}

bool dataObject::getGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, DWORD &dwData)
{
	assert(pDataObject);
	dwData = 0;
	if(!pDataObject) return false;
	FORMATETC fmtetc = {0};
	STGMEDIUM stgmed = {0};
	if (getGlobalData(pDataObject, cfFormat, fmtetc, stgmed))
	{
		if (::GlobalSize(stgmed.hGlobal) > 0)
		{
			dwData = *(static_cast<DWORD *>(::GlobalLock(stgmed.hGlobal)));
			::GlobalUnlock(stgmed.hGlobal);
			::ReleaseStgMedium(&stgmed);
			return true;
		}
	}
	return false;
}
bool dataObject::getGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, DWORD &dwData)
{
	assert(pDataObject);
	assert(pwsFormat);
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return getGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), dwData);

}

bool dataObject::setGlobalData(IDataObject *pDataObject, CLIPFORMAT cfFormat, DWORD dwData, bool bForce /*= true*/)
{
	assert(pDataObject);
	if(!pDataObject) return false;
	bool bSet = false;
	FORMATETC formatEtc = {0};
	STGMEDIUM stgMedium = {0};
	if (getGlobalData(pDataObject, cfFormat, formatEtc, stgMedium))
	{
		DWORD *pData = static_cast<DWORD *>(::GlobalLock(stgMedium.hGlobal));
		bSet = (*pData != dwData);
		*pData = dwData;
		::GlobalUnlock(stgMedium.hGlobal);
		if (bSet)
		{
			bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
		}
		if (!bSet)									// not changed or setting failed
			::ReleaseStgMedium(&stgMedium);
	}
	else if (bForce || dwData)
	{
		stgMedium.hGlobal = gtc::app::createGlobalHandle((const void*)&dwData, sizeof(dwData));
		if (stgMedium.hGlobal)
		{
			stgMedium.tymed = TYMED_HGLOBAL;
			stgMedium.pUnkForRelease = NULL;
			bSet = SUCCEEDED(pDataObject->SetData(&formatEtc, &stgMedium, TRUE));
			if(!bSet)
				::GlobalFree(stgMedium.hGlobal);
		}
	}
	return bSet;
}
bool dataObject::setGlobalData(IDataObject *pDataObject, PCWSTR pwsFormat, DWORD dwData, bool bForce /*= true*/)
{
	assert(pDataObject);
	assert(pwsFormat);
	if(!pDataObject || !pwsFormat || std::wcslen(pwsFormat) == 0) return false;
	return setGlobalData(pDataObject, static_cast<CLIPFORMAT>(::RegisterClipboardFormat(pwsFormat)), dwData, bForce);

}
//bool dataObject::updateData(CLIPFORMAT cfFormat, HGLOBAL globalData)
//{
//	bool bRet = false;
//	FORMATETC fmtetc = {cfFormat,0,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
//	if (SUCCEEDED(QueryGetData(&fmtetc)) && (m_iCurDataIndex >= 0 && m_iCurDataIndex < (int)m_vecData.size()))
//	{
//		STGMEDIUM *pstgm = m_vecData[m_iCurDataIndex].pstgMedium;
//		if (pstgm)
//		{
//			GlobalFree(pstgm->hGlobal);
//			pstgm->hGlobal = globalData;
//			bRet = true;
//		}
//	}
//	else
//	{
//		bRet = setGlobalData(cfFormat, globalData);
//	}
//	return bRet;
//}

void dataObject::clear()
{
	for (DataStorageArrayType::iterator iter = m_vecData.begin(); iter != m_vecData.end(); ++iter)
	{
		if(iter->pstgMedium)
		{
			ReleaseStgMedium(iter->pstgMedium);
			delete iter->pstgMedium;
		}
		if(iter->pformatETC) delete iter->pformatETC;
	}
	m_vecData.clear();
}

//bool dataObject::isDataAvailable(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc /*= NULL*/)
//{
//	FORMATETC fmtETC = { cfFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
//	HRESULT hr = QueryGetData(&fmtETC);
//	if (SUCCEEDED(hr) && lpFormatEtc)
//	{
//		*lpFormatEtc = fmtETC;
//	}
//	return SUCCEEDED(hr);
//}

//bool dataObject::setClipboard()
//{
//	return setClipboard(this);
//}
//bool dataObject::setGlobalData(PCSTR pstr)
//{
//	if(!pstr) return false;
//	std::size_t len = std::strlen(pstr);
//	if(len <= 0) return false;
//	bool bRet = false;
//	HGLOBAL hGlobal = gtc::app::createGlobalHandle((void *)pstr, len+1);
//	if (hGlobal)
//	{
//		bRet = setGlobalData(CF_TEXT, hGlobal);
//		if (!bRet)
//		{
//			GlobalFree(hGlobal);
//			hGlobal = NULL;
//		}
//	}
//
//	return bRet;
//}
//
//bool dataObject::setGlobalData(PCWSTR pwstr)
//{
//	if(!pwstr) return false;
//	std::size_t len = std::wcslen(pwstr);
//	if(len <= 0) return false;
//	bool bRet = false;
//	HGLOBAL hGlobal = gtc::app::createGlobalHandle((void *)pwstr, (len+1)*sizeof(wchar_t));
//	if (hGlobal)
//	{
//		bRet = setGlobalData(CF_UNICODETEXT, hGlobal);
//		if (!bRet)
//		{
//			GlobalFree(hGlobal);
//			hGlobal = NULL;
//		}
//	}
//
//	return bRet;
//
//}
//bool dataObject::setGlobalData(CLIPFORMAT cfFormat, HGLOBAL globalData)
//{
//	if(!globalData) return false;
//	FORMATETC fmtETC = {cfFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
//	STGMEDIUM stgMed = {0};
//	stgMed.tymed = fmtETC.tymed;
//	stgMed.hGlobal = globalData;
//	stgMed.pUnkForRelease = NULL;
//
//	return SUCCEEDED(SetData(&fmtETC, &stgMed, TRUE));
//}

bool dataObject::setFileData(const std::vector<std::wstring> &vecFile)
{
	if(vecFile.size() <= 0) return false;
	std::size_t uSize = sizeof(DROPFILES);
	for (std::vector<std::wstring>::const_iterator iter = vecFile.cbegin(); iter != vecFile.cend(); ++iter)
	{
		uSize += (iter->length()+1)*sizeof(wchar_t);
	}
	// The last string is double-null terminated.
	uSize += sizeof(wchar_t);
	HGLOBAL hgDrop = GlobalAlloc(GMEM_MOVEABLE,uSize);
	if(!hgDrop) return false;
	void *pData = static_cast<LPVOID>(GlobalLock(hgDrop));

	DROPFILES *pdfile = static_cast<DROPFILES *>(pData);
	pdfile->pFiles = sizeof(DROPFILES);
	pdfile->fNC = FALSE;
	pdfile->pt.x = 0;
	pdfile->pt.y	 = 0;
	pdfile->fWide = TRUE;

	BYTE *pFileStart = NULL;
	std::size_t uOffset = sizeof(DROPFILES);

	for (std::vector<std::wstring>::const_iterator iter = vecFile.cbegin(); iter != vecFile.cend(); ++iter)
	{
		pFileStart = static_cast<BYTE *>(pData) + uOffset;
		memcpy(pFileStart, iter->data(), (iter->length()+1)*sizeof(wchar_t));
		uOffset += (iter->length()+1)*sizeof(wchar_t);
	}
	// Copy the last null byte.
	pFileStart = static_cast<BYTE *>(pData) + uOffset;
	memcpy(pFileStart, L"\0", sizeof(wchar_t));

	GlobalUnlock(hgDrop);

	//bool bRet = setGlobalData(CF_HDROP, hgDrop);
	bool bRet = setGlobalData(this, CF_HDROP, hgDrop);
	if (!bRet)
	{
		GlobalFree(hgDrop);
		hgDrop = NULL;
	}
	return bRet;

}

//bool dataObject::setTargetData(const ITEMIDLIST *pidlTarget)
//{
//	//if(!pidlTarget) return false;
//	UINT len = sizeof(CLSID_RecycleBin);
//	HGLOBAL hgDrop = GlobalAlloc(GMEM_MOVEABLE, len);
//	void *pData = static_cast<LPVOID>(GlobalLock(hgDrop));
//	memcpy(pData, &CLSID_RecycleBin, len);
//	GlobalUnlock(hgDrop);
//	
//	bool bRet = setGlobalData(RegisterClipboardFormat(CFSTR_TARGETCLSID), hgDrop);
//	if (!bRet)
//	{
//		GlobalFree(hgDrop);
//		hgDrop = NULL;
//	}
//	return bRet;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dropSource  类		2015-3-6
//	拖放源的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
dropSource::dropSource()
	: m_lRefCount(1), m_pDragSourceHelper(NULL), m_pDragSourceHelper2(NULL)
	, m_pDataObject(nullptr)
	, m_bSetCursor(true)
{
	CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDragSourceHelper, (void**)&m_pDragSourceHelper);
	if (m_pDragSourceHelper)
	{
		m_pDragSourceHelper->QueryInterface(IID_IDragSourceHelper2, (void**)&m_pDragSourceHelper2);
		if (m_pDragSourceHelper2)
		{
			m_pDragSourceHelper2->SetFlags(DSH_ALLOWDROPDESCRIPTIONTEXT);
			m_pDragSourceHelper->Release();
			m_pDragSourceHelper = static_cast<IDragSourceHelper*>(m_pDragSourceHelper2);
		}
	}
}
dropSource::~dropSource()
{
	m_lRefCount = 0;
	if (m_pDragSourceHelper)
	{
		m_pDragSourceHelper->Release();
		m_pDragSourceHelper = NULL;
	}

}

STDMETHODIMP dropSource::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =  
	{  
		QITABENT(dropSource, IDropSource),  
		{ 0 },  
	};
	return QISearch(this, qit, riid, ppv);  
}
STDMETHODIMP_(ULONG) dropSource::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}
STDMETHODIMP_(ULONG) dropSource::Release()
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);  
	if (0 == lRef)  
	{  
		delete this;  
	}  
	return m_lRefCount;  
}
STDMETHODIMP dropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	if(fEscapePressed)
		return DRAGDROP_S_CANCEL;  
	//SEND_LOG(L"dropSource::QueryContinueDrag ketState:%08X", grfKeyState);
	// If the left button of mouse is released  
	if( 0 == (grfKeyState & (MK_LBUTTON) ))
		return DRAGDROP_S_DROP;
	return S_OK;
}
STDMETHODIMP dropSource::GiveFeedback(DWORD dwEffect)
{
	SCODE sc = DRAGDROP_S_USEDEFAULTCURSORS;
	// Drag must be started, Windows version must be Vista or later
	//  and visual styles must be enabled.
	if (m_pDataObject)
	{
		// IsShowingLayered is true when the target window shows the drag image
		// (target window calls the IDropTargetHelper handler functions).
		DWORD dwData = 0;
		bool bOldStyle = false;
		if(m_pDataObject->getGlobalData(m_pDataObject, L"IsShowingLayered", dwData))
			bOldStyle = dwData == 0;
		// Check if the drop description data object must be updated:
		// - When entering a window that does not show drag images while the previous
		//   target has shown the image, the drop description should be set to invalid.
		//   Condition: bOldStyle && !m_bSetCursor
		// - When setting drop description text here is enabled and the target
		//   shows the drag image, the description should be updated if not done by the target.
		//   Condition: m_pDropDescription && !bOldStyle
		if ((bOldStyle && !m_bSetCursor) || !bOldStyle)
		{
			// Get DropDescription data if present.
			// If not present, cursor and optional text are shown like within the Windows Explorer.
			FORMATETC formatEtc = {0};
			STGMEDIUM stgMedium = {0};
			if (m_pDataObject->getGlobalData(m_pDataObject, CFSTR_DROPDESCRIPTION, formatEtc, stgMedium))
			{
				bool bChangeDescription = false;		// Set when DropDescription will be changed here
				DROPDESCRIPTION *pDropDescription = static_cast<DROPDESCRIPTION*>(::GlobalLock(stgMedium.hGlobal));
				if (bOldStyle)
				{
					bChangeDescription = clearDescription(pDropDescription);
				}
				// The drop target is showing the drag image and new cursors and may have changed the description.
				// Drop targets that change the description should clear it when leaving the window
				// (set the image type to DROPIMAGE_INVALID from within OnDragLeave()).
				// The target may have set a special image type (LABEL, WARNING, NOIMAGE).
				// Then use it and do not change anything here.
				else if (pDropDescription->type <= DROPIMAGE_LINK)
				{
					DROPIMAGETYPE nImageType = dropEffectToDropImage(dwEffect);
					//SEND_LOG(L"AAAAA 3 imgType:%u effect:%u dropImgType:%u", nImageType, dwEffect, pDropDescription->type);
					// When the target has changed the description, it usually has set the correct type.
					// If the image type does not match the drop effect, set the text here.
					if (DROPIMAGE_INVALID != nImageType &&
						(pDropDescription->type != nImageType || m_wsInsertTarget != pDropDescription->szInsert))
					{
						bChangeDescription = true;
						if(m_wsInsertTarget.empty()) clearDescription(pDropDescription);
						else
						{
							pDropDescription->type = nImageType;
							if (pDropDescription->type == DROPIMAGE_MOVE)
							{
								std::wcsncpy(pDropDescription->szMessage, L"移动到 %1", _countof(pDropDescription->szMessage));
							}
							else if (pDropDescription->type == DROPIMAGE_COPY)
							{
								std::wcsncpy(pDropDescription->szMessage, L"复制到 %1", _countof(pDropDescription->szMessage));
							}
							else if (pDropDescription->type == DROPIMAGE_LINK)
							{
								std::wcsncpy(pDropDescription->szMessage, L"在 %1 创建链接", _countof(pDropDescription->szMessage));
							}
							std::wcsncpy(pDropDescription->szInsert, m_wsInsertTarget.c_str(), _countof(pDropDescription->szInsert));
						}
						
						//// When text is available, set drop description image type and text.
						//if (m_pDropDescription->HasText(nImageType))
						//{
						//	bChangeDescription = true;
						//	pDropDescription->type = nImageType;
						//	m_pDropDescription->SetDescription(pDropDescription, nImageType);
						//}
						//// Set image type to invalid. Cursor is shown according to drop effect
						////  using system default text.
						//else
						//{
						//	bChangeDescription = clearDescription(pDropDescription);
						//}
					}
				}	
				::GlobalUnlock(stgMedium.hGlobal);
				if (bChangeDescription)				// Update the DropDescription data object when 
				{									//  image type or description text has been changed here.
					//SEND_LOG(L"AAAAA 5");
					if (FAILED(m_pDataObject->SetData(&formatEtc, &stgMedium, TRUE)))
						bChangeDescription = false;
				}
				if (!bChangeDescription)			// Description not changed or setting it failed
					::ReleaseStgMedium(&stgMedium);
			}
		}
		if (!bOldStyle)								// Show new style drag cursors.
		{
			if (m_bSetCursor)
			{
				// Set the default cursor.
				// Otherwise, the old style drag cursor remains when leaving a window
				//  that does not support drag images.
				// NOTE: 
				//  Add '#define OEMRESOURCE' on top of stdafx.h.
				//  This is necessary to include the OCR_ definitions from winuser.h.
				HCURSOR hCursor = (HCURSOR)LoadImage(
					NULL,							// hInst must be NULL for OEM images
					MAKEINTRESOURCE(32512),	// default cursor OCR_NORMAL
					IMAGE_CURSOR,					// image type is cursor
					0, 0,							// use system metrics values with LR_DEFAULTSIZE
					LR_DEFAULTSIZE | LR_SHARED);
				::SetCursor(hCursor);
			}
			// Update of the cursor may be suppressed if "IsShowingText" is false.
			// But this results in invisible cursors for very short moments.
			//			if (0 != CDragDropHelper::GetGlobalDataDWord(m_pIDataObj, _T("IsShowingText")))
			// If a drop description object exists and the image type is not invalid,
			//  it is used. Otherwise use the default cursor and text according to the drop effect. 
			setDragImageCursor(dwEffect);
			sc = S_OK;								// Don't show default (old style) drag cursor
		}
		// When the old style drag cursor is actually used, the Windows cursor must be set
		//  to the default arrow cursor when showing new style drag cursors the next time.
		// If a new style drag cursor is actually shown, the cursor has been set above.
		// By using this flag, the cursor must be set only once when entering a window that
		//  supports new style drag cursors and not with each call when moving over it.
		m_bSetCursor = bOldStyle;
	}
	return sc;
	//UNREFERENCED_PARAMETER(dwEffect);  
	//return DRAGDROP_S_USEDEFAULTCURSORS;
}
bool dropSource::startDragDrop(dataObject *pDataObject, DWORD dwOKEffects, LPDWORD pdwEffect)
{
	assert(pDataObject);
	if(!pDataObject) return false;
	OleInitialize(NULL);
	m_pDataObject = pDataObject;
	bool bUseDescription = (m_pDragSourceHelper2 != NULL) && gtc::app::isWindowsVistaLater()/* && ::IsAppThemed()*/;
	if (bUseDescription)
	{
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DROPDESCRIPTION));
		if (hGlobal)
		{
			DROPDESCRIPTION *pDropDescription = static_cast<DROPDESCRIPTION*>(::GlobalLock(hGlobal));
			pDropDescription->type = DROPIMAGE_INVALID;
			::GlobalUnlock(hGlobal);
			m_pDataObject->setGlobalData(pDataObject, CFSTR_DROPDESCRIPTION, hGlobal);
		}
	}
	//HRESULT hr = ::SHDoDragDrop(NULL, pDataObject, this, dwOKEffects, pdwEffect);
	HRESULT hr = ::DoDragDrop(pDataObject, this, dwOKEffects, pdwEffect);
	OleUninitialize();
	return SUCCEEDED(hr);
}
bool dropSource::initFromWindow(HWND hWnd, POINT pt,IDataObject* pDataObject)
{
	if(!m_pDragSourceHelper) return false;

	return SUCCEEDED(m_pDragSourceHelper->InitializeFromWindow(hWnd, &pt, pDataObject));
}
bool dropSource::initFromBitmap(HBITMAP hBitmap, POINT ptMouse, RECT rcDrawScreen, IDataObject* pDataObject, 
	COLORREF crColorKey /*= 0xFFFFFFFF*/)
{
	if(!m_pDragSourceHelper) return false;
	SHDRAGIMAGE di = {0};
	BITMAP      bm;
	GetObject(hBitmap, sizeof(bm), &bm);
	di.sizeDragImage.cx = bm.bmWidth;
	di.sizeDragImage.cy = bm.bmHeight;
	di.hbmpDragImage = hBitmap;
	di.crColorKey = crColorKey; 
	di.ptOffset.x = ptMouse.x - rcDrawScreen.left;
	di.ptOffset.y = ptMouse.y - rcDrawScreen.top;
	//di.ptOffset = ptMouse;
	//di.ptOffset.x = di.ptOffset.y = 50;
	return SUCCEEDED(m_pDragSourceHelper->InitializeFromBitmap(&di, pDataObject));
}

dataObject* dropSource::getDataObject()
{
	return m_pDataObject;
}

#define DDWM_SETCURSOR		(WM_USER + 2)

bool dropSource::setDragImageCursor(DWORD dwEffect)
{
	// Stored data is always a DWORD even with 64-bit apps.
	DWORD dwData = 0;
	HWND hWnd = NULL;
	if (m_pDataObject->getGlobalData(m_pDataObject, L"DragWindow", dwData))
	{
		hWnd = (HWND)ULongToHandle(dwData);
	}
	if (hWnd)
	{
		WPARAM wParam = 0;								// Use DropDescription to get type and optional text
		switch (dwEffect & ~DROPEFFECT_SCROLL)
		{
		case DROPEFFECT_NONE : wParam = 1; break;
		case DROPEFFECT_COPY : wParam = 3; break;		// The order is not as for DROPEEFECT values!
		case DROPEFFECT_MOVE : wParam = 2; break;
		case DROPEFFECT_LINK : wParam = 4; break;
		}
		::SendMessage(hWnd, DDWM_SETCURSOR, wParam, 0);
	}
	return NULL != hWnd;
}

bool dropSource::clearDescription(DROPDESCRIPTION *pDropDescription)
{
	assert(pDropDescription);
	if(!pDropDescription) return false;
	bool bChanged = 
		pDropDescription->type != DROPIMAGE_INVALID ||
		pDropDescription->szMessage[0] != L'\0' || 
		pDropDescription->szInsert[0] != L'\0'; 
	pDropDescription->type = DROPIMAGE_INVALID;
	pDropDescription->szMessage[0] = pDropDescription->szInsert[0] = L'\0';
	return bChanged;
}

DROPIMAGETYPE dropSource::dropEffectToDropImage(DWORD dwEffect)
{
	DROPIMAGETYPE nImageType = DROPIMAGE_INVALID;
	dwEffect &= ~DROPEFFECT_SCROLL;
	if (DROPEFFECT_NONE == dwEffect)
		nImageType = DROPIMAGE_NONE;
	else if (dwEffect & DROPEFFECT_MOVE)
		nImageType = DROPIMAGE_MOVE;
	else if (dwEffect & DROPEFFECT_COPY)
		nImageType = DROPIMAGE_COPY;
	else if (dwEffect & DROPEFFECT_LINK)
		nImageType = DROPIMAGE_LINK;
	return nImageType;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dropTarget  类		2015-3-9
//	拖放目标封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////

dropTarget::dropTarget()
	: m_lRefCount(1), m_pDropTargetHelper(NULL)
	, m_pFormatETC(NULL)
	, m_bAllowDrop(false)
	, m_pDTargetCallback(NULL)
{
	if(FAILED(CoCreateInstance(CLSID_DragDropHelper,NULL,CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper,(LPVOID*)&m_pDropTargetHelper)))
		m_pDropTargetHelper = NULL;
}
dropTarget::~dropTarget()
{
	m_lRefCount = 0;
	if(m_pDropTargetHelper)
	{
		m_pDropTargetHelper->Release();
		m_pDropTargetHelper = NULL;
	}
}
IDropTargetCallback* dropTarget::getOwner() const
{
	return m_pDTargetCallback;
}
void dropTarget::setOwner(IDropTargetCallback *pOwner)
{
	m_pDTargetCallback = pOwner;
}
STDMETHODIMP dropTarget::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =  
	{  
		QITABENT(dropTarget, IUnknown),  
		QITABENT(dropTarget, IDropTarget),  
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}
STDMETHODIMP_(ULONG) dropTarget::AddRef(void)
{
	return InterlockedIncrement(&m_lRefCount);
}
STDMETHODIMP_(ULONG) dropTarget::Release(void)
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);  
	if (0 == lRef)  
	{  
		delete this;  
	}  
	return m_lRefCount;  
}

STDMETHODIMP dropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if(!pDataObj) return E_INVALIDARG;
	m_pFormatETC = NULL;
	if(m_pDropTargetHelper) m_pDropTargetHelper->DragEnter(m_hTargetWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	// 
	for (std::vector<FORMATETC>::iterator iter = m_vecFormatETC.begin(); iter != m_vecFormatETC.end(); ++iter)
	{
		m_bAllowDrop = SUCCEEDED(pDataObj->QueryGetData(&(*iter)));
		if (m_bAllowDrop)
		{
			m_pFormatETC = &(*iter);
			break;
		}
	}

	queryDrop(grfKeyState, pdwEffect);
	if(m_pDTargetCallback)
	{
		POINT ptt = {pt.x, pt.y};
		m_pDTargetCallback->onDragEnter(pDataObj, grfKeyState, ptt, pdwEffect);
	}
	return S_OK;
}
STDMETHODIMP dropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if(m_pDropTargetHelper) m_pDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);
	queryDrop(grfKeyState, pdwEffect);
	if(m_pDTargetCallback)
	{
		POINT ptt = {pt.x, pt.y};
		m_pDTargetCallback->onDragOver(grfKeyState, ptt, pdwEffect);
	}
	return S_OK;
}
STDMETHODIMP dropTarget::DragLeave(void)
{
	if(m_pDropTargetHelper) m_pDropTargetHelper->DragLeave();
	if(m_pDTargetCallback)
	{
		m_pDTargetCallback->onDragLeave();
	}

	m_bAllowDrop = false;
	m_pFormatETC = NULL;
	return S_OK;
}
STDMETHODIMP dropTarget::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if(!pDataObj) return E_INVALIDARG;

	if(m_pDropTargetHelper) m_pDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);

	if (queryDrop(grfKeyState, pdwEffect))
	{
		if (m_pDTargetCallback)
		{
			POINT ptt = {pt.x, pt.y};
			m_pDTargetCallback->onDrop(pDataObj, grfKeyState, ptt, pdwEffect);
		}	
	}

	m_bAllowDrop = false;
	m_pFormatETC = NULL;
	if(pdwEffect) *pdwEffect = DROPEFFECT_NONE;
	return S_OK;
}

bool dropTarget::registerDropTarget(HWND hWnd)
{
	if(!hWnd) return false;
	m_hTargetWnd = hWnd;
	HRESULT hr = RegisterDragDrop(m_hTargetWnd, this);
	return SUCCEEDED(hr);
}
void dropTarget::revokeDropTarget()
{
	HRESULT hr = S_OK;
	if (m_hTargetWnd)
	{
		hr = RevokeDragDrop(m_hTargetWnd);
		m_hTargetWnd = NULL;
	}
}

void dropTarget::addSupportedFormat(CLIPFORMAT fmt)
{
	FORMATETC fmtETC = {fmt, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	addSupportedFormat(fmtETC);
}

void dropTarget::addSupportedFormat(FORMATETC &fmtETC)
{
	m_vecFormatETC.push_back(fmtETC);
}

bool dropTarget::queryDrop(DWORD grfKeyState, LPDWORD pdwEffect)
{
	assert(pdwEffect);
	if(!pdwEffect) return false;
	DWORD dwOKEffects = *pdwEffect; 

	if(!m_bAllowDrop)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return false;
	}
	//CTRL+SHIFT  -- DROPEFFECT_LINK
	//CTRL        -- DROPEFFECT_COPY
	//SHIFT       -- DROPEFFECT_MOVE
	//ALT		  -- DROPEFFECT_LINK
	//no modifier -- DROPEFFECT_MOVE or whatever is allowed by src
	*pdwEffect = (grfKeyState & MK_CONTROL) ?
		( (grfKeyState & MK_SHIFT) ? DROPEFFECT_LINK : DROPEFFECT_COPY )/* 按下MK_CONTROL */:
		( (grfKeyState & MK_SHIFT) ? DROPEFFECT_MOVE : ( (grfKeyState & MK_ALT) ? DROPEFFECT_LINK : DROPEFFECT_MOVE));
	if(*pdwEffect == 0) 
	{
		// No modifier keys used by user while dragging. 
		if (DROPEFFECT_COPY & dwOKEffects)
			*pdwEffect = DROPEFFECT_COPY;
		else if (DROPEFFECT_MOVE & dwOKEffects)
			*pdwEffect = DROPEFFECT_MOVE; 
		else if (DROPEFFECT_LINK & dwOKEffects)
			*pdwEffect = DROPEFFECT_LINK; 
		else 
		{
			*pdwEffect = DROPEFFECT_NONE;
		}
	} 
	else
	{
		// Check if the drag source application allows the drop effect desired by user.
		// The drag source specifies this in DoDragDrop
		if(!(*pdwEffect & dwOKEffects))
			*pdwEffect = DROPEFFECT_NONE;
	}  

	return (DROPEFFECT_NONE == *pdwEffect)?false:true;
}

}	// end gtc