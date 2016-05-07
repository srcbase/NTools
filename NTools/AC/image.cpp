#include "image.h"
#include "AC/sysupport.h"
#include "AC/draw/drawer.h"
#include "AC/file.h"
namespace gtc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	image类	2014-07-10
//	GDI+图像类封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
image::image() 
	: m_pImage(NULL)
	, m_hBuffer(NULL) 
{
	memset(&m_rcPatch, 0, sizeof(m_rcPatch));
	memset(&m_szImage, 0, sizeof(m_szImage));
}
image::image(PCWSTR pFile) 
	: m_pImage(NULL)
	, m_hBuffer(NULL) 
{
	memset(&m_rcPatch, 0, sizeof(m_rcPatch));
	memset(&m_szImage, 0, sizeof(m_szImage));
	load(pFile);
}

image::image(HBITMAP hBitmap)
	: m_pImage(NULL)
	, m_hBuffer(NULL) 
{
	memset(&m_rcPatch, 0, sizeof(m_rcPatch));
	memset(&m_szImage, 0, sizeof(m_szImage));
	load(hBitmap);
}

image::image(HICON hIcon)
	: m_pImage(NULL)
	, m_hBuffer(NULL) 
{
	memset(&m_rcPatch, 0, sizeof(m_rcPatch));
	memset(&m_szImage, 0, sizeof(m_szImage));
	load(hIcon);
}

image::image(Gdiplus::Image *pImage)
	: m_hBuffer(NULL) 
{
	memset(&m_rcPatch, 0, sizeof(m_rcPatch));
	memset(&m_szImage, 0, sizeof(m_szImage));
	operator=(pImage);
}

image::image(const image &img)
	: m_hBuffer(NULL), m_pImage(NULL)
{
	m_rcPatch = img.m_rcPatch;
	m_szImage = img.m_szImage;
	if(img.m_pImage) m_pImage = img.m_pImage->Clone();
	if(img.m_hBuffer) gtc::app::copyGlobalMemory(m_hBuffer, img.m_hBuffer);
}
image::~image() {
	//gtc::app::writeVSLogs(L"image free %08X", m_pImage);
	clear();
}
image& image::operator = (Gdiplus::Image *pImage)
{
	clear();
	m_pImage = pImage->Clone();

	return *this;
}
void image::setPatch(RECT rc)
{
	m_rcPatch = rc;
}

void image::setPatch(LONG left, LONG top, LONG right, LONG bottom)
{
	m_rcPatch.left = left;
	m_rcPatch.top = top;
	m_rcPatch.right = right;
	m_rcPatch.bottom = bottom;
}
RECT image::getPatch() const
{
	return m_rcPatch;
}
bool image::isPatch() const
{
	return m_rcPatch.left != 0 || m_rcPatch.right != 0 || m_rcPatch.top != 0 || m_rcPatch.bottom != 0;
}
SIZE image::getImageSize() const
{
	SIZE sz = {getWidth(), getHeight()};
	return sz;
}
int image::getWidth() const
{
	//return m_pImage->GetWidth();
	if (m_pImage && m_szImage.cx <= 0)
	{
		m_szImage.cx = m_pImage->GetWidth();
		if(m_pImage->GetLastStatus() != Gdiplus::Ok) m_szImage.cx = 0;
	}
	return m_szImage.cx;
}
int image::getHeight() const
{
	//return m_pImage->GetHeight();
	if (m_pImage && m_szImage.cy <= 0)
	{
		m_szImage.cy = m_pImage->GetHeight();
		if(m_pImage->GetLastStatus() != Gdiplus::Ok) m_szImage.cy = 0;
		//return h;
	}
	return m_szImage.cy;
}
bool image::load(PCWSTR pFile, bool bCopy /*= false*/)
{
	assert(pFile);
	clear();
	if(!pFile || !::PathFileExists(pFile)) return false;
	if (!bCopy)
	{
		m_pImage = Gdiplus::Image::FromFile(pFile);
	}
	else
	{
		SBDataType vecData;
		if (gtc::file::read(pFile, vecData))
		{

			return load(vecData);
			/*m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, vecData.size());
			if (m_hBuffer)
			{
			uint8_t* pBuffer = static_cast<uint8_t *>(::GlobalLock(m_hBuffer));
			if (pBuffer)
			{
			memcpy_s(pBuffer, vecData.size(), vecData.data(), vecData.size());
			}
			::GlobalUnlock(m_hBuffer);
			IStream* pStream = nullptr;
			if (SUCCEEDED(::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream)))
			{
			m_pImage = Gdiplus::Image::FromStream(pStream);
			pStream->Release();
			}

			if (!m_pImage)
			{
			::GlobalFree(m_hBuffer);
			m_hBuffer = NULL;
			}
			}*/
		}
	}
	assert(m_pImage);
	return m_pImage && m_pImage->GetLastStatus() == Gdiplus::Ok;
}
bool image::load(HBITMAP hBitmap)
{
	if(!hBitmap) return false;
	clear();
	m_pImage = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
	//CLSID clsidBMP; 
	//if(!getImageCLSID(L"bmp",&clsidBMP)) return false;
	//IStream *pStream = NULL;
	//m_hBuffer = GlobalAlloc(GMEM_MOVEABLE, 0);
	//if (!m_hBuffer) return false;
	//HRESULT hr = CreateStreamOnHGlobal(m_hBuffer, TRUE, &pStream);
	//if (!pStream || FAILED(hr)) return false;
	//// 
	//hr = S_FALSE;
	//Gdiplus::Bitmap bmp(hBitmap, NULL);
	//if(bmp.Save(pStream, &clsidBMP) == Gdiplus::Ok)
	//{
	//	m_pImage = new Gdiplus::Image(pStream);
	//}
	//// 
	//if(pStream) pStream->Release();
	return m_pImage != NULL;
}
bool image::load(HICON hIcon)
{
	assert(hIcon);
	if(!hIcon) return false;
	clear();
	m_pImage = Gdiplus::Bitmap::FromHICON(hIcon);
	//CLSID clsidICON; 
	//if(!getImageCLSID(L"ico",&clsidICON)) return false;
	//IStream *pStream = NULL;
	//m_hBuffer = GlobalAlloc(GMEM_MOVEABLE, 0);
	//if (!m_hBuffer) return false;
	//HRESULT hr = CreateStreamOnHGlobal(m_hBuffer, TRUE, &pStream);
	//if (!pStream || FAILED(hr)) return false;
	//// 
	//hr = S_FALSE;
	//Gdiplus::Bitmap ico(hIcon);
	//if(ico.Save(pStream, &clsidICON) == Gdiplus::Ok)
	//{
	//	m_pImage = new Gdiplus::Image(pStream);
	//}
	//// 
	//if(pStream) pStream->Release();
	return m_pImage != NULL;
}
bool image::load(const SBDataType &imgData)
{
	assert(imgData.size() > 0);
	if(imgData.size() == 0) return false;
	clear();
	m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imgData.size());
	if (m_hBuffer)
	{
		uint8_t* pBuffer = static_cast<uint8_t *>(::GlobalLock(m_hBuffer));
		if (pBuffer)
		{
			memcpy_s(pBuffer, imgData.size(), imgData.data(), imgData.size());
		}
		::GlobalUnlock(m_hBuffer);
		IStream* pStream = nullptr;
		if (SUCCEEDED(::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream)))
		{
			m_pImage = Gdiplus::Image::FromStream(pStream);
			pStream->Release();
			assert(m_pImage && m_pImage->GetLastStatus() == Gdiplus::Ok);
			if (m_pImage && m_pImage->GetLastStatus() != Gdiplus::Ok)
			{
				//gtc::file::write(L"D:\\111\\7.png", imgData);
				delete m_pImage;
				m_pImage = nullptr;
			}
		}

		if (!m_pImage)
		{
			::GlobalFree(m_hBuffer);
			m_hBuffer = NULL;
		}
	}

	return m_pImage != NULL;
}
bool image::save(PCWSTR pfilePath, PCWSTR pfileExt /*= L"jpg"*/)
{
	assert(m_pImage);
	if(!m_pImage) return false;
	assert(!(!pfilePath || !pfileExt || std::wcslen(pfilePath) <= 0 || std::wcslen(pfileExt) <= 0));
	if(!pfilePath || !pfileExt || std::wcslen(pfilePath) <= 0 || std::wcslen(pfileExt) <= 0) return false;
	CLSID clsid; 
	bool bOk = false;
	if (m_pImage && getImageCLSID(pfileExt,&clsid))
	{
		Gdiplus::Status status = m_pImage->Save(pfilePath, &clsid);
		bOk = status == Gdiplus::Ok;
	}
	return bOk;
}

image* image::clone()
{
	return new image(*this);
}
bool image::getImageCLSID(PCWSTR pimgExtName, CLSID *pclsidImage)
{
	//得到格式为format的图像文件的编码值，访问该格式图像的COM组件的GUID值保存在pCLSID中 
	UINT num = 0; 
	UINT size = 0; 
	std::wstring wsExt = pimgExtName, wsFormat(L"image/");
	gtc::convert::toLower(wsExt);
	if(wsExt == L"jpg") wsExt = L"jpeg";
	wsFormat += wsExt;
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL; 
	Gdiplus::GetImageEncodersSize(&num, &size); 
	if(size == 0) 
		return false; // 编码信息不可用 
	//分配内存 
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size)); 
	if(pImageCodecInfo == NULL) 
		return false; // 分配失败 
	//获得系统中可用的编码方式的所有信息 
	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo); 
	//在可用编码信息中查找format格式是否被支持 
	for(UINT i = 0; i < num; ++i) 
	{ //MimeType：编码方式的具体描述 
		if(wsFormat == pImageCodecInfo[i].MimeType ) 
		{
			*pclsidImage = pImageCodecInfo[i].Clsid; 
			free(pImageCodecInfo); 
			return true; 
		}
	}
	free(pImageCodecInfo); 
	return false; 
}

#if defined(_ATL) || defined(_AFXDLL)
bool image::toImage(ATL::CImage &image)
{
	if(!m_pImage) return false;
	CLSID clsidPNG; 
	getImageCLSID(L"png",&clsidPNG);
	IStream *pStream = NULL;
	HGLOBAL hMemImage = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (!hMemImage) return false;
	HRESULT hr = CreateStreamOnHGlobal(hMemImage, TRUE, &pStream);
	if (!pStream || FAILED(hr)) return false;
	// 
	hr = S_FALSE;
	if(m_pImage->Save(pStream, &clsidPNG) == Gdiplus::Ok)
	{
		hr = image.Load(pStream);
	}
	// 
	if(pStream) pStream->Release();
	if(hMemImage) GlobalFree(hMemImage);

	return SUCCEEDED(hr);
}
#endif
HBITMAP image::toHBitmap(COLORREF clrBk /*= 0x0FFFFFFFF*/, SIZE *pszNew /*= NULL*/)
{
	if(!m_pImage) return NULL;
	HBITMAP hBmp = NULL;
	SIZE szImage = {getWidth(), getHeight()};
	//Gdiplus::Bitmap *pBmp = static_cast<Gdiplus::Bitmap *>(m_pImage);
	//if (pBmp)
	//{
	//	SEND_LOG(L"image::toHBitmap bmp");
	//	pBmp->GetHBITMAP(Gdiplus::Color(clrBk), &hBmp);
	//}
	//else
	{
		//SEND_LOG(L"image::toHBitmap png");
		HDC hdc = GetDC(NULL);
		HDC hMem = CreateCompatibleDC(hdc);
		hBmp = CreateCompatibleBitmap(hdc, szImage.cx, szImage.cy);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hMem, hBmp);
		Gdiplus::Graphics grap(hMem);
		////grap.Clear(Gdiplus::Color::Transparent);
		//grap.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(clrBk)), 0, 0, szImage.cx, szImage.cy);
		//grap.DrawImage(m_pImage, 0, 0, szImage.cx, szImage.cy);
		RECT rc = {0, 0, szImage.cx, szImage.cy};
		RECT rcSrc = {0, 0, m_pImage->GetWidth(), m_pImage->GetHeight()};
		gtc::drawer::fillRectangle(grap, clrBk, rc);
		gtc::drawer::drawImage(grap, m_pImage, rc, rcSrc);
		// 
		if(hOldBmp) SelectObject(hMem, hOldBmp);
		if(hMem) DeleteDC(hMem);
		if(hdc) ReleaseDC(NULL, hdc);

	}

	if (hBmp && pszNew && (pszNew->cx != szImage.cx || pszNew->cy != szImage.cy))
	{
		HDC hdc = GetDC(NULL);
		SIZE szSrc = szImage;
		SIZE szDest = *pszNew;
		HDC hMem = CreateCompatibleDC(hdc);
		HGDIOBJ hOldBmpSrc = SelectObject(hMem, hBmp);
		HDC hMem2 = CreateCompatibleDC(hdc);
		HBITMAP hBmp2 = CreateCompatibleBitmap(hdc, szDest.cx, szDest.cy);
		HGDIOBJ hOldBmpDest = SelectObject(hMem2, hBmp2);
		StretchBlt(hMem2, 0, 0, szDest.cx, szDest.cy, hMem, 0, 0, szSrc.cx, szSrc.cy, SRCCOPY);
		// release
		SelectObject(hMem, hOldBmpSrc);
		SelectObject(hMem2, hOldBmpDest);
		DeleteObject(hBmp);
		hBmp = hBmp2;
		DeleteDC(hMem);
		DeleteDC(hMem2);
		ReleaseDC(NULL, hdc);
	}
	return hBmp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	imageResource类	2014-07-10
//	资源图像类封装
////////////////////////////////////////////////////////////////////////////////////////////////////////

void imageResource::clear()
{
	image::clear();
	//if (m_hBuffer)
	//{
	//	//::GlobalUnlock(m_hBuffer);
	//	::GlobalFree(m_hBuffer);
	//	m_hBuffer = NULL;
	//} 
}

bool imageResource::load(PCWSTR pName, PCWSTR pType, HMODULE hInst)
{
	clear();

	HRSRC hResource = NULL;
	if(std::wcscmp(pType, L"bmp") == 0 || std::wcscmp(pType, L"BMP") == 0 || std::wcscmp(pType, L"Bmp") == 0)
	{
		m_pImage = Gdiplus::Bitmap::FromResource(hInst, pName);
	}
	else
	{
		hResource = ::FindResource(hInst, pName, pType);
		if (!hResource)
		{
			SEND_LOG(L"imageResource::load 加载资源失败！！！name:<%s> type:<%s> err:%u", pName, pType, GetLastError());
			return false;
		}
		DWORD imageSize = ::SizeofResource(hInst, hResource);
		if (!imageSize)
			return false;

		const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
		if (!pResourceData)
			return false;

		m_hBuffer  = ::GlobalAlloc(GMEM_FIXED, imageSize);
		if (m_hBuffer)
		{
			void* pBuffer = ::GlobalLock(m_hBuffer);
			if (pBuffer)
			{
				CopyMemory(pBuffer, pResourceData, imageSize);

				IStream* pStream = nullptr;
				if (SUCCEEDED(::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream)))
				{
					m_pImage = Gdiplus::Image::FromStream(pStream);
					//DWORD ls = GetLastError();
					pStream->Release();
					//if (m_pImage)
					//{ 
					//	if (m_pImage->GetLastStatus() == Gdiplus::Ok)
					//		return true;

					//	delete m_pImage;
					//	m_pImage = nullptr;
					//}
				}
				::GlobalUnlock(m_hBuffer);
			}
			::GlobalFree(m_hBuffer);
			m_hBuffer = NULL;
		}
	}
	return m_pImage && m_pImage->GetLastStatus() == Gdiplus::Ok;
}

}