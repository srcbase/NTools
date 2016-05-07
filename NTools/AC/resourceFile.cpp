#include "resourceFile.h"

namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	resourceFile  类		2015-5-27
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
resourceFile::resourceFile(PCWSTR presFile /*= NULL*/)
	: m_wsResfile(presFile ? presFile : L"")
	, m_hmdlRes(NULL)
{
	if (!m_wsResfile.empty())
	{
		m_hmdlRes = ::LoadLibraryEx(m_wsResfile.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
		assert(m_hmdlRes);
	}
}
resourceFile::~resourceFile()
{
	if(m_hmdlRes) ::FreeLibrary(m_hmdlRes);
}

void resourceFile::setResourceFile(PCWSTR presFile)
{
	if(m_wsResfile == presFile) return;
	if(m_hmdlRes) 
	{
		::FreeLibrary(m_hmdlRes);
		m_hmdlRes = NULL;
	}
	if (!m_wsResfile.empty())
	{
		m_hmdlRes = ::LoadLibraryW(m_wsResfile.c_str());
		assert(m_hmdlRes);
	}
}
std::wstring resourceFile::getResourceFile() const
{
	return m_wsResfile;
}
HICON resourceFile::findIcon(UINT iconID)
{
	return LoadIcon(m_hmdlRes, MAKEINTRESOURCEW(iconID));
}
HICON resourceFile::findIcon(UINT iconID, int cxIcon, int cyIcon)
{
	HICON hIcon = (HICON)LoadImage(m_hmdlRes, MAKEINTRESOURCEW(iconID), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
	return hIcon;
	////bool bRet = false;
	//HRSRC hRsrc = NULL;
	//DWORD dwSize = 0;
	//HGLOBAL hgblRsrc = NULL;
	//LPVOID pDataRsrc = NULL;
	////HGLOBAL hgData = NULL;
	////LPVOID pData = NULL;
	////IStream* pStream = NULL;
	//__try
	//{
	//	hRsrc = FindResource(m_hmdlRes, MAKEINTRESOURCEW(iconID), RT_GROUP_ICON);
	//	if(hRsrc == NULL) __leave;
	//	dwSize = SizeofResource(m_hmdlRes, hRsrc);
	//	if(dwSize == 0) __leave;
	//	hgblRsrc = LoadResource(m_hmdlRes, hRsrc);
	//	if(hgblRsrc == NULL) __leave;
	//	pDataRsrc = LockResource(hgblRsrc);
	//	if(pDataRsrc == NULL) __leave;

	//	int subID = LookupIconIdFromDirectoryEx((PBYTE)pDataRsrc, TRUE, cxIcon, cyIcon, LR_DEFAULTCOLOR);
	//	if (subID > 0)
	//	{
	//		hRsrc = FindResource(m_hmdlRes, MAKEINTRESOURCEW(subID), RT_ICON);
	//		if(hRsrc == NULL) __leave;
	//		hgblRsrc = LoadResource(m_hmdlRes, hRsrc);
	//		if(hgblRsrc == NULL) __leave;
	//		pDataRsrc = LockResource(hgblRsrc);
	//		if(pDataRsrc == NULL) __leave;
	//		dwSize = SizeofResource(m_hmdlRes, hRsrc);
	//		if(dwSize == 0) __leave;
	//		hIcon = CreateIconFromResourceEx((PBYTE)pDataRsrc, dwSize, TRUE, 0x00030000, cxIcon, cyIcon, LR_DEFAULTCOLOR);
	//	}
	//	//// 
	//	//hgData = ::GlobalAlloc(GMEM_FIXED, dwSize);
	//	//if(hgData == NULL) __leave;
	//	//pData = ::GlobalLock(hgData);
	//	//if(pData == NULL) __leave;
	//	//CopyMemory(pData, pDataRsrc, dwSize);
	//	//if (::CreateStreamOnHGlobal(hgData, FALSE, &pStream) == S_OK)
	//	//{
	//	//	Gdiplus::Bitmap *pBmp = Gdiplus::Bitmap::FromStream(pStream);
	//	//	bRet = pBmp->GetHICON(&refHIcon) == Gdiplus::Ok;
	//	//	//refImage = Gdiplus::Image::FromStream(pStream);
	//	//	//DWORD ls = GetLastError();
	//	//	pStream->Release();
	//	//	pStream = NULL;
	//	//}
	//	//// 
	//	//if(::GlobalUnlock(hgData)) pData = NULL;
	//	//::GlobalFree(hgData);
	//	//hgData = NULL;
	//	//bRet = true;
	//}
	//__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"resourceFile::findIcon"))
	//{
	//	WRITE_LOG(L"resourceFile::findIcon SEH异常 ");
	//	//bRet = false;
	//	if (hIcon)
	//	{
	//		::DestroyIcon(hIcon);
	//		hIcon = NULL;
	//	}
	//}
	////if(pStream) pStream->Release();
	////if(pData) ::GlobalUnlock(hgData);
	////if(hgData) ::GlobalFree(hgData);
	//if(hgblRsrc)
	//{
	//	UnlockResource(hgblRsrc);
	//	FreeResource(hgblRsrc);
	//}
	//return hIcon;
}

bool resourceFile::findPNG(UINT pngID, gtc::image &imgPng)
{
	bool bRet = false;
	HRSRC hRsrc = NULL;
	DWORD dwSize = 0;
	HGLOBAL hgblRsrc = NULL;
	LPVOID pDataRsrc = NULL;
	HGLOBAL hgData = NULL;
	LPVOID pData = NULL;
	IStream* pStream = NULL;
	__try
	{
		hRsrc = FindResource(m_hmdlRes, MAKEINTRESOURCEW(pngID), L"PNG");
		if(hRsrc == NULL) __leave;
		dwSize = SizeofResource(m_hmdlRes, hRsrc);
		if(dwSize == 0) __leave;
		hgblRsrc = LoadResource(m_hmdlRes, hRsrc);
		if(hgblRsrc == NULL) __leave;
		pDataRsrc = LockResource(hgblRsrc);
		if(pDataRsrc == NULL) __leave;

		// 
		hgData = ::GlobalAlloc(GMEM_FIXED, dwSize);
		if(hgData == NULL) __leave;
		pData = ::GlobalLock(hgData);
		if(pData == NULL) __leave;
		CopyMemory(pData, pDataRsrc, dwSize);
		if (SUCCEEDED(::CreateStreamOnHGlobal(hgData, FALSE, &pStream)))
		{
			imgPng = Gdiplus::Image::FromStream(pStream);
			bRet = true;
			//DWORD ls = GetLastError();
			pStream->Release();
			pStream = NULL;
		}
		// 
		if(::GlobalUnlock(hgData)) pData = NULL;
		::GlobalFree(hgData);
		hgData = NULL;
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"resourceFile::findPNG"))
	{
		WRITE_LOG(L"resourceFile::findPNG SEH异常 ");
		bRet = false;
	}
	if(pStream) pStream->Release();
	if(pData) ::GlobalUnlock(hgData);
	if(hgData) ::GlobalFree(hgData);
	if(hgblRsrc)
	{
		UnlockResource(hgblRsrc);
		FreeResource(hgblRsrc);
	}
	return bRet;
}

HBITMAP resourceFile::findBitmap(UINT bmpID)
{
	HBITMAP hBitmap = LoadBitmap(m_hmdlRes, MAKEINTRESOURCEW(bmpID));
	////bool bRet = false;
	//HRSRC hRsrc = NULL;
	//DWORD dwSize = 0;
	//HGLOBAL hgblRsrc = NULL;
	//LPVOID pDataRsrc = NULL;
	////HGLOBAL hgData = NULL;
	////LPVOID pData = NULL;
	////IStream* pStream = NULL;
	//__try
	//{
	//	hRsrc = FindResource(m_hmdlRes, MAKEINTRESOURCEW(iconID), RT_GROUP_ICON);
	//	if(hRsrc == NULL) __leave;
	//	dwSize = SizeofResource(m_hmdlRes, hRsrc);
	//	if(dwSize == 0) __leave;
	//	hgblRsrc = LoadResource(m_hmdlRes, hRsrc);
	//	if(hgblRsrc == NULL) __leave;
	//	pDataRsrc = LockResource(hgblRsrc);
	//	if(pDataRsrc == NULL) __leave;

	//	int subID = LookupIconIdFromDirectoryEx((PBYTE)pDataRsrc, TRUE, cxIcon, cyIcon, LR_DEFAULTCOLOR);
	//	if (subID > 0)
	//	{
	//		hRsrc = FindResource(m_hmdlRes, MAKEINTRESOURCEW(subID), RT_ICON);
	//		if(hRsrc == NULL) __leave;
	//		hgblRsrc = LoadResource(m_hmdlRes, hRsrc);
	//		if(hgblRsrc == NULL) __leave;
	//		pDataRsrc = LockResource(hgblRsrc);
	//		if(pDataRsrc == NULL) __leave;
	//		dwSize = SizeofResource(m_hmdlRes, hRsrc);
	//		if(dwSize == 0) __leave;
	//		hIcon = CreateIconFromResourceEx((PBYTE)pDataRsrc, dwSize, TRUE, 0x00030000, cxIcon, cyIcon, LR_DEFAULTCOLOR);
	//	}
	//	//// 
	//	//hgData = ::GlobalAlloc(GMEM_FIXED, dwSize);
	//	//if(hgData == NULL) __leave;
	//	//pData = ::GlobalLock(hgData);
	//	//if(pData == NULL) __leave;
	//	//CopyMemory(pData, pDataRsrc, dwSize);
	//	//if (::CreateStreamOnHGlobal(hgData, FALSE, &pStream) == S_OK)
	//	//{
	//	//	Gdiplus::Bitmap *pBmp = Gdiplus::Bitmap::FromStream(pStream);
	//	//	bRet = pBmp->GetHICON(&refHIcon) == Gdiplus::Ok;
	//	//	//refImage = Gdiplus::Image::FromStream(pStream);
	//	//	//DWORD ls = GetLastError();
	//	//	pStream->Release();
	//	//	pStream = NULL;
	//	//}
	//	//// 
	//	//if(::GlobalUnlock(hgData)) pData = NULL;
	//	//::GlobalFree(hgData);
	//	//hgData = NULL;
	//	//bRet = true;
	//}
	//__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"resourceFile::findIcon"))
	//{
	//	WRITE_LOG(L"resourceFile::findIcon SEH异常 ");
	//	//bRet = false;
	//	if (hBitmap)
	//	{
	//		::DeleteObject(hBitmap);
	//		hBitmap = NULL;
	//	}
	//}
	////if(pStream) pStream->Release();
	////if(pData) ::GlobalUnlock(hgData);
	////if(hgData) ::GlobalFree(hgData);
	//if(hgblRsrc)
	//{
	//	UnlockResource(hgblRsrc);
	//	FreeResource(hgblRsrc);
	//}
	return hBitmap;
}
}	// end gtc