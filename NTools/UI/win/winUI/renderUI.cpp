#include "StdAfx.h"
#include "renderUI.h"

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
		int *comp, int req_comp);
	extern void     stbi_image_free(void *retval_from_stbi_load);

};

namespace wui {

/////////////////////////////////////////////////////////////////////////////////////
//
//

renderClip::~renderClip()
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	assert(::GetObjectType(hRgn)==OBJ_REGION);
	assert(::GetObjectType(hOldRgn)==OBJ_REGION);
	::SelectClipRgn(hDC, hOldRgn);
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);
}

void renderClip::generateClip(HDC hDC, RECT rc, renderClip& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
}

void renderClip::generateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, renderClip& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
	::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
	::DeleteObject(hRgnItem);
}

void renderClip::useOldClipBegin(HDC hDC, renderClip& clip)
{
	::SelectClipRgn(hDC, clip.hOldRgn);
}

void renderClip::useOldClipEnd(HDC hDC,renderClip& clip)
{
	::SelectClipRgn(hDC, clip.hRgn);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

static const float OneThird = 1.0f / 3;

static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L) {
	const float
		R = (float)GetRValue(ARGB),
		G = (float)GetGValue(ARGB),
		B = (float)GetBValue(ARGB),
		nR = (R<0?0:(R>255?255:R))/255,
		nG = (G<0?0:(G>255?255:G))/255,
		nB = (B<0?0:(B>255?255:B))/255,
		m = min(min(nR,nG),nB),
		M = max(max(nR,nG),nB);
	*L = (m + M)/2;
	if (M==m) *H = *S = 0;
	else {
		const float
			f = (nR==m)?(nG-nB):((nG==m)?(nB-nR):(nR-nG)),
			i = (nR==m)?3.0f:((nG==m)?5.0f:1.0f);
		*H = (i-f/(M-m));
		if (*H>=6) *H-=6;
		*H*=60;
		*S = (2*(*L)<=1)?((M-m)/(M+m)):((M-m)/(2-M-m));
	}
}

static void HSLtoRGB(DWORD* ARGB, float H, float S, float L) {
	const float
		q = 2*L<1?L*(1+S):(L+S-L*S),
		p = 2*L-q,
		h = H/360,
		tr = h + OneThird,
		tg = h,
		tb = h - OneThird,
		ntr = tr<0?tr+1:(tr>1?tr-1:tr),
		ntg = tg<0?tg+1:(tg>1?tg-1:tg),
		ntb = tb<0?tb+1:(tb>1?tb-1:tb),
		R = 255*(6*ntr<1?p+(q-p)*6*ntr:(2*ntr<1?q:(3*ntr<2?p+(q-p)*6*(2.0f*OneThird-ntr):p))),
		G = 255*(6*ntg<1?p+(q-p)*6*ntg:(2*ntg<1?q:(3*ntg<2?p+(q-p)*6*(2.0f*OneThird-ntg):p))),
		B = 255*(6*ntb<1?p+(q-p)*6*ntb:(2*ntb<1?q:(3*ntb<2?p+(q-p)*6*(2.0f*OneThird-ntb):p)));
	*ARGB &= 0xFF000000;
	*ARGB |= RGB( (BYTE)(R<0?0:(R>255?255:R)), (BYTE)(G<0?0:(G>255?255:G)), (BYTE)(B<0?0:(B>255?255:B)) );
}

static COLORREF pixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
		GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
		GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

}

static BOOL WINAPI alphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
	int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
	HDC hTempDC = ::CreateCompatibleDC(hDC);
	if (NULL == hTempDC)
		return FALSE;

	//Creates Source DIB
	LPBITMAPINFO lpbiSrc = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
	{
		::DeleteDC(hTempDC);
		return FALSE;
	}
	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = dwWidth;
	lpbiSrc->bmiHeader.biHeight = dwHeight;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biClrUsed = 0;
	lpbiSrc->bmiHeader.biClrImportant = 0;

	COLORREF* pSrcBits = NULL;
	HBITMAP hSrcDib = CreateDIBSection (
		hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
		NULL, NULL);

	if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
	{
		delete [] lpbiSrc;
		::DeleteDC(hTempDC);
		return FALSE;
	}

	HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
	::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	//Creates Destination DIB
	LPBITMAPINFO lpbiDest = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiDest == NULL)
	{
		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);
		::DeleteDC(hTempDC);
		return FALSE;
	}

	lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiDest->bmiHeader.biWidth = dwWidth;
	lpbiDest->bmiHeader.biHeight = dwHeight;
	lpbiDest->bmiHeader.biPlanes = 1;
	lpbiDest->bmiHeader.biBitCount = 32;
	lpbiDest->bmiHeader.biCompression = BI_RGB;
	lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiDest->bmiHeader.biXPelsPerMeter = 0;
	lpbiDest->bmiHeader.biYPelsPerMeter = 0;
	lpbiDest->bmiHeader.biClrUsed = 0;
	lpbiDest->bmiHeader.biClrImportant = 0;

	COLORREF* pDestBits = NULL;
	HBITMAP hDestDib = CreateDIBSection (
		hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
		NULL, NULL);

	if ((NULL == hDestDib) || (NULL == pDestBits))
	{
		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);
		::DeleteDC(hTempDC);
		return FALSE;
	}

	::SelectObject (hTempDC, hDestDib);
	::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	double src_darken;
	BYTE nAlpha;

	for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
	{
		nAlpha = LOBYTE(*pSrcBits >> 24);
		src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
		if( src_darken < 0.0 ) src_darken = 0.0;
		*pDestBits = pixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
	} //for

	::SelectObject (hTempDC, hDestDib);
	::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	delete [] lpbiDest;
	::DeleteObject(hDestDib);

	delete [] lpbiSrc;
	::DeleteObject(hSrcDib);

	::DeleteDC(hTempDC);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

DWORD renderEngine::adjustColor(DWORD dwColor, short H, short S, short L)
{
	if( H == 180 && S == 100 && L == 100 ) return dwColor;
	float fH, fS, fL;
	float S1 = S / 100.0f;
	float L1 = L / 100.0f;
	RGBtoHSL(dwColor, &fH, &fS, &fL);
	fH += (H - 180);
	fH = fH > 0 ? fH : fH + 360; 
	fS *= S1;
	fL *= L1;
	HSLtoRGB(&dwColor, fH, fS, fL);
	return dwColor;
}

TImageInfo* renderEngine::loadImage(stringOrID bitmap, PCWSTR type, DWORD mask)
{
	LPBYTE pData = NULL;
	DWORD dwSize = 0;

	if( type == NULL ) {
		std::wstring sFile = paintManagerUI::getResourcePath();
		if( paintManagerUI::getResourceZip().empty() ) {
			sFile += bitmap.m_lpstr;
			HANDLE hFile = ::CreateFile(sFile.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) return NULL;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) return NULL;

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				return NULL;
			}
		}
		else {
			sFile += paintManagerUI::getResourceZip();
			HZIP hz = OpenZip((void*)sFile.data(), 0, 2);
			ZIPENTRY ze; 
			int i; 
			if( FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0 ) return NULL;
			dwSize = ze.unc_size;
			if( dwSize == 0 ) return NULL;
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, i, pData, dwSize, 3);
			if( res != 0x00000000 && res != 0x00000600) {
				delete[] pData;
				CloseZip(hz);
				return NULL;
			}
			CloseZip(hz);
		}
	}
	else {
		HRSRC hResource = ::FindResource(paintManagerUI::getResourceDll(), bitmap.m_lpstr, type);
		if( hResource == NULL ) return NULL;
		HGLOBAL hGlobal = ::LoadResource(paintManagerUI::getResourceDll(), hResource);
		if( hGlobal == NULL ) {
			FreeResource(hResource);
			return NULL;
		}

		dwSize = ::SizeofResource(paintManagerUI::getResourceDll(), hResource);
		if( dwSize == 0 ) return NULL;
		pData = new BYTE[ dwSize ];
		::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
		::FreeResource(hResource);
	}

	LPBYTE pImage = NULL;
	int x,y,n;
	pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
	delete[] pData;
	if( !pImage ) return NULL;

	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = x;
	bmi.bmiHeader.biHeight = -y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = x * y * 4;

	bool bAlphaChannel = false;
	LPBYTE pDest = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
	if( !hBitmap ) return NULL;

	for( int i = 0; i < x * y; i++ ) 
	{
		pDest[i*4 + 3] = pImage[i*4 + 3];
		if( pDest[i*4 + 3] < 255 )
		{
			pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
			pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
			pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
			bAlphaChannel = true;
		}
		else
		{
			pDest[i*4] = pImage[i*4 + 2];
			pDest[i*4 + 1] = pImage[i*4 + 1];
			pDest[i*4 + 2] = pImage[i*4]; 
		}

		if( *(DWORD*)(&pDest[i*4]) == mask ) {
			pDest[i*4] = (BYTE)0;
			pDest[i*4 + 1] = (BYTE)0;
			pDest[i*4 + 2] = (BYTE)0; 
			pDest[i*4 + 3] = (BYTE)0;
			bAlphaChannel = true;
		}
	}

	stbi_image_free(pImage);

	TImageInfo* data = new TImageInfo;
	data->hBitmap = hBitmap;
	data->nX = x;
	data->nY = y;
	data->alphaChannel = bAlphaChannel;
	return data;
}

void renderEngine::drawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint,
	const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, 
	BYTE uFade, bool hole, bool xtiled, bool ytiled)
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(L"msimg32.dll"), "AlphaBlend");

	if( lpAlphaBlend == NULL ) lpAlphaBlend = alphaBitBlt;
	if( hBitmap == NULL ) return;

	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
	::SetStretchBltMode(hDC, COLORONCOLOR);

	RECT rcTemp = {0};
	RECT rcDest = {0};
	if( lpAlphaBlend && (alphaChannel || uFade < 255) ) {
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
		// middle
		if( !hole ) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				if( !xtiled && !ytiled ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
				else if( xtiled && ytiled ) {
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int j = 0; j < iTimesY; ++j ) {
						LONG lDestTop = rcDest.top + lHeight * j;
						LONG lDestBottom = rcDest.top + lHeight * (j + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom ) {
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						for( int i = 0; i < iTimesX; ++i ) {
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right ) {
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
								lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
								rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
						}
					}
				}
				else if( xtiled ) {
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					for( int i = 0; i < iTimes; ++i ) {
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if( lDestRight > rcDest.right ) {
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}
						lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
							hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
					}
				}
				else { // ytiled
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int i = 0; i < iTimes; ++i ) {
						LONG lDestTop = rcDest.top + lHeight * i;
						LONG lDestBottom = rcDest.top + lHeight * (i + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom ) {
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
							hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
					}
				}
			}
		}

		// left-top
		if( rcCorners.left > 0 && rcCorners.top > 0 ) {
			rcDest.left = rc.left;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
			}
		}
		// top
		if( rcCorners.top > 0 ) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
					rcCorners.left - rcCorners.right, rcCorners.top, bf);
			}
		}
		// right-top
		if( rcCorners.right > 0 && rcCorners.top > 0 ) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
			}
		}
		// left
		if( rcCorners.left > 0 ) {
			rcDest.left = rc.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
					rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// right
		if( rcCorners.right > 0 ) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
					rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// left-bottom
		if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
			rcDest.left = rc.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
			}
		}
		// bottom
		if( rcCorners.bottom > 0 ) {
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
					rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
			}
		}
		// right-bottom
		if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
					rcCorners.bottom, bf);
			}
		}
	}
	else 
	{
		if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
			&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
			&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
		{
			if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
					hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
			}
		}
		else
		{
			// middle
			if( !hole ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					if( !xtiled && !ytiled ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
					else if( xtiled && ytiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int j = 0; j < iTimesY; ++j ) {
							LONG lDestTop = rcDest.top + lHeight * j;
							LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							for( int i = 0; i < iTimesX; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
									lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
									rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
							}
						}
					}
					else if( xtiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right ) {
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
						}
					}
					else { // ytiled
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestTop = rcDest.top + lHeight * i;
							LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
						}
					}
				}
			}

			// left-top
			if( rcCorners.left > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
				}
			}
			// top
			if( rcCorners.top > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
						rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
				}
			}
			// right-top
			if( rcCorners.right > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
				}
			}
			// left
			if( rcCorners.left > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
						rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
				}
			}
			// right
			if( rcCorners.right > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
				}
			}
			// left-bottom
			if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
				}
			}
			// bottom
			if( rcCorners.bottom > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
				}
			}
			// right-bottom
			if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
						rcCorners.bottom, SRCCOPY);
				}
			}
		}
	}

	::SelectObject(hCloneDC, hOldBitmap);
	::DeleteDC(hCloneDC);
}


bool drawImage(HDC hDC, paintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const std::wstring& sImageName, \
	const std::wstring& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, \
	bool bHole, bool bTiledX, bool bTiledY)
{
	const TImageInfo* data = NULL;
	if( sImageResType.empty() ) {
		data = pManager->getImageEx(sImageName.c_str(), NULL, dwMask);
	}
	else {
		data = pManager->getImageEx(sImageName.c_str(), sImageResType.c_str(), dwMask);
	}
	if( !data ) return false;    

	if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
		rcBmpPart.right = data->nX;
		rcBmpPart.bottom = data->nY;
	}
	if (rcBmpPart.right > data->nX) rcBmpPart.right = data->nX;
	if (rcBmpPart.bottom > data->nY) rcBmpPart.bottom = data->nY;

	RECT rcTemp;
	if( !::IntersectRect(&rcTemp, &rcItem, &rc) ) return true;
	if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) ) return true;

	renderEngine::drawImage(hDC, data->hBitmap, rcItem, rcPaint, rcBmpPart, rcCorner, data->alphaChannel, bFade, bHole, bTiledX, bTiledY);

	return true;
}

bool renderEngine::drawImageString(HDC hDC, paintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, 
	PCWSTR pStrImage, PCWSTR pStrModify)
{
	if ((pManager == NULL) || (hDC == NULL)) return false;

	// 1、aaa.jpg
	// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
	// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

	std::wstring sImageName = pStrImage;
	std::wstring sImageResType;
	RECT rcItem = rc;
	RECT rcBmpPart = {0};
	RECT rcCorner = {0};
	DWORD dwMask = 0;
	BYTE bFade = 0xFF;
	bool bHole = false;
	bool bTiledX = false;
	bool bTiledY = false;

	int image_count = 0;

	std::wstring sItem;
	std::wstring sValue;
	PWSTR pstr = NULL;

	for( int i = 0; i < 2; ++i,image_count = 0 ) {
		if( i == 1)
			pStrImage = pStrModify;

		if( !pStrImage ) continue;

		while( *pStrImage != L'\0' ) {
			sItem.clear();
			sValue.clear();
			while( *pStrImage > L'\0' && *pStrImage <= L' ' ) pStrImage = ::CharNext(pStrImage);
			while( *pStrImage != L'\0' && *pStrImage != L'=' && *pStrImage > L' ' ) {
				PWSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sItem += *pStrImage++;
				}
			}
			while( *pStrImage > L'\0' && *pStrImage <= L' ' ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != L'=' ) break;
			while( *pStrImage > L'\0' && *pStrImage <= L' ' ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != L'\'' ) break;
			while( *pStrImage != L'\0' && *pStrImage != L'\'' ) {
				PWSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sValue += *pStrImage++;
				}
			}
			if( *pStrImage++ != L'\'' ) break;
			if( !sValue.empty() ) {
				if( sItem == L"file" || sItem == L"res" ) {
					if( image_count > 0 )
						wui::drawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
						rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

					sImageName = sValue;
					++image_count;
				}
				else if( sItem == L"restype" ) {
					if( image_count > 0 )
						wui::drawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
						rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

					sImageResType = sValue;
					++image_count;
				}
				else if( sItem == L"dest" ) {
					rcItem.left = rc.left + _tcstol(sValue.data(), &pstr, 10);  assert(pstr);    
					rcItem.top = rc.top + _tcstol(pstr + 1, &pstr, 10);    assert(pstr);
					rcItem.right = rc.left + _tcstol(pstr + 1, &pstr, 10);  assert(pstr);
					if (rcItem.right > rc.right) rcItem.right = rc.right;
					rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, 10); assert(pstr);
					if (rcItem.bottom > rc.bottom) rcItem.bottom = rc.bottom;
				}
				else if( sItem == L"source" ) {
					rcBmpPart.left = _tcstol(sValue.data(), &pstr, 10);  assert(pstr);    
					rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
					rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
					rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);  
				}
				else if( sItem == L"corner" ) {
					rcCorner.left = _tcstol(sValue.data(), &pstr, 10);  assert(pstr);    
					rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
					rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
					rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
				}
				else if( sItem == L"mask" ) {
					if( sValue[0] == L'#') dwMask = _tcstoul(sValue.data() + 1, &pstr, 16);
					else dwMask = _tcstoul(sValue.data(), &pstr, 16);
				}
				else if( sItem == L"fade" ) {
					bFade = (BYTE)_tcstoul(sValue.data(), &pstr, 10);
				}
				else if( sItem == L"hole" ) {
					bHole = (_tcscmp(sValue.data(), L"true") == 0);
				}
				else if( sItem == L"xtiled" ) {
					bTiledX = (_tcscmp(sValue.data(), L"true") == 0);
				}
				else if( sItem == L"ytiled" ) {
					bTiledY = (_tcscmp(sValue.data(), L"true") == 0);
				}
			}
			if( *pStrImage++ != L' ' ) break;
		}
	}

	wui::drawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
		rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

	return true;
}

void renderEngine::drawColor(HDC hDC, const RECT& rc, DWORD color)
{
	if( color <= 0x00FFFFFF ) return;
	if( color >= 0xFF000000 )
	{
		::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}
	else
	{
		// Create a new 32bpp bitmap with room for an alpha channel
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = 1;
		bmi.bmiHeader.biHeight = 1;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
		LPDWORD pDest = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
		if( !hBitmap ) return;

		*pDest = color;

		RECT rcBmpPart = {0, 0, 1, 1};
		RECT rcCorners = {0};
		drawImage(hDC, hBitmap, rc, rc, rcBmpPart, rcCorners, true, 255);
		::DeleteObject(hBitmap);
	}
}

void renderEngine::drawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
{
	typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(L"msimg32.dll"), "AlphaBlend");
	if( lpAlphaBlend == NULL ) lpAlphaBlend = alphaBitBlt;
	typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
	static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(L"msimg32.dll"), "GradientFill");

	BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
	if( bAlpha == 0 ) return;
	int cx = rc.right - rc.left;
	int cy = rc.bottom - rc.top;
	RECT rcPaint = rc;
	HDC hPaintDC = hDC;
	HBITMAP hPaintBitmap = NULL;
	HBITMAP hOldPaintBitmap = NULL;
	if( bAlpha < 255 ) {
		rcPaint.left = rcPaint.top = 0;
		rcPaint.right = cx;
		rcPaint.bottom = cy;
		hPaintDC = ::CreateCompatibleDC(hDC);
		hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
		assert(hPaintDC);
		assert(hPaintBitmap);
		hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
	}
	if( lpGradientFill != NULL ) 
	{
		TRIVERTEX triv[2] = 
		{
			{ rcPaint.left, rcPaint.top, GetBValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetRValue(dwFirst) << 8, 0xFF00 },
			{ rcPaint.right, rcPaint.bottom, GetBValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetRValue(dwSecond) << 8, 0xFF00 }
		};
		GRADIENT_RECT grc = { 0, 1 };
		lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
	}
	else 
	{
		// Determine how many shades
		int nShift = 1;
		if( nSteps >= 64 ) nShift = 6;
		else if( nSteps >= 32 ) nShift = 5;
		else if( nSteps >= 16 ) nShift = 4;
		else if( nSteps >= 8 ) nShift = 3;
		else if( nSteps >= 4 ) nShift = 2;
		int nLines = 1 << nShift;
		for( int i = 0; i < nLines; i++ ) {
			// Do a little alpha blending
			BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
			BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
			BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
			// ... then paint with the resulting color
			HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
			RECT r2 = rcPaint;
			if( bVertical ) {
				r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
				r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
				if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
			}
			else {
				r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
				r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
				if( (r2.right - r2.left) > 0 ) ::FillRect(hPaintDC, &r2, hBrush);
			}
			::DeleteObject(hBrush);
		}
	}
	if( bAlpha < 255 ) {
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
		lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
		::SelectObject(hPaintDC, hOldPaintBitmap);
		::DeleteObject(hPaintBitmap);
		::DeleteDC(hPaintDC);
	}
}

void renderEngine::drawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	POINT ptTemp = { 0 };
	HPEN hPen = ::CreatePen(PS_SOLID, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
	::LineTo(hDC, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

void renderEngine::drawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

void renderEngine::drawRoundRect(HDC hDC, const RECT& rc, int nSize, int width, int height, DWORD dwPenColor)
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

void renderEngine::drawText(HDC hDC, paintManagerUI* pManager, RECT& rc, PCWSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle)
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
	HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->getFont(iFont));
	::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);
}

void renderEngine::drawHtmlText(HDC hDC, paintManagerUI* pManager, RECT& rc, PCWSTR pstrText, DWORD dwTextColor, RECT* prcLinks, std::wstring* sLinks, int& nLinkRects, UINT uStyle)
{
	// 考虑到在xml编辑器中使用<>符号不方便，可以使用{}符号代替
	// 支持标签嵌套（如<l><b>text</b></l>），但是交叉嵌套是应该避免的（如<l><b>text</l></b>）
	// The string formatter supports a kind of "mini-html" that consists of various short tags:
	//
	//   Bold:             <b>text</b>
	//   Color:            <c #xxxxxx>text</c>  where x = RGB in hex
	//   Font:             <f x>text</f>        where x = font id
	//   Italic:           <i>text</i>
	//   Image:            <i x y z>            where x = image name and y = imagelist num and z(optional) = imagelist id
	//   Link:             <a x>text</a>        where x(optional) = link content, normal like app:notepad or http:www.xxx.com
	//   NewLine           <n>                  
	//   Paragraph:        <p x>text</p>        where x = extra pixels indent in p
	//   Raw Text:         <r>text</r>
	//   Selected:         <s>text</s>
	//   Underline:        <u>text</u>
	//   X Indent:         <x i>                where i = hor indent in pixels
	//   Y Indent:         <y i>                where i = ver indent in pixels 

	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	if( pstrText == NULL ) return;
	if( ::IsRectEmpty(&rc) ) return;

	bool bDraw = (uStyle & DT_CALCRECT) == 0;

	std::vector<TFontInfo *> aFontArray;
	std::vector<DWORD> aColorArray;
	std::vector<int> aPIndentArray;

	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	HRGN hRgn = ::CreateRectRgnIndirect(&rc);
	if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

	TEXTMETRIC* pTm = &pManager->getDefaultFontInfo()->tm;
	HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->getDefaultFontInfo()->hFont);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
	DWORD dwBkColor = pManager->getDefaultSelectedBkColor();
	::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));

	// If the drawstyle include a alignment, we'll need to first determine the text-size so
	// we can draw it at the correct position...
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_VCENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		drawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, uStyle | DT_CALCRECT);
		rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
		rc.bottom = rc.top + (rcText.bottom - rcText.top);
	}
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_CENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		drawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, uStyle | DT_CALCRECT);
		rc.left = rc.left + ((rc.right - rc.left) / 2) - ((rcText.right - rcText.left) / 2);
		rc.right = rc.left + (rcText.right - rcText.left);
	}
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_RIGHT) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		drawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, uStyle | DT_CALCRECT);
		rc.left = rc.right - (rcText.right - rcText.left);
	}

	bool bHoverLink = false;
	std::wstring sHoverLink;
	POINT ptMouse = pManager->getMousePos();
	for( int i = 0; !bHoverLink && i < nLinkRects; i++ ) {
		if( ::PtInRect(prcLinks + i, ptMouse) ) {
			sHoverLink = *(std::wstring*)(sLinks + i);
			bHoverLink = true;
		}
	}

	POINT pt = { rc.left, rc.top };
	int iLinkIndex = 0;
	int cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1);
	int cyMinHeight = 0;
	int cxMaxWidth = 0;
	POINT ptLinkStart = { 0 };
	bool bLineEnd = false;
	bool bInRaw = false;
	bool bInLink = false;
	bool bInSelected = false;
	int iLineLinkIndex = 0;

	// 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
	std::vector<TFontInfo*> aLineFontArray;
	std::vector<DWORD> aLineColorArray;
	std::vector<int> aLinePIndentArray;
	PCWSTR pstrLineBegin = pstrText;
	bool bLineInRaw = false;
	bool bLineInLink = false;
	bool bLineInSelected = false;
	int cyLineHeight = 0;
	bool bLineDraw = false; // 行的第二阶段：绘制
	while( *pstrText != L'\0' ) {
		if( pt.x >= rc.right || *pstrText == L'\n' || bLineEnd ) {
			if( *pstrText == L'\n' ) pstrText++;
			if( bLineEnd ) bLineEnd = false;
			if( !bLineDraw ) {
				if( bInLink && iLinkIndex < nLinkRects ) {
					::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, min(pt.x, rc.right), pt.y + cyLine);
					std::wstring *pStr1 = (std::wstring*)(sLinks + iLinkIndex - 1);
					std::wstring *pStr2 = (std::wstring*)(sLinks + iLinkIndex);
					*pStr2 = *pStr1;
				}
				for( int i = iLineLinkIndex; i < iLinkIndex; i++ ) {
					prcLinks[i].bottom = pt.y + cyLine;
				}
				if( bDraw ) {
					bInLink = bLineInLink;
					iLinkIndex = iLineLinkIndex;
				}
			}
			else {
				if( bInLink && iLinkIndex < nLinkRects ) iLinkIndex++;
				bLineInLink = bInLink;
				iLineLinkIndex = iLinkIndex;
			}
			if( (uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw) ) break;
			if( bDraw ) bLineDraw = !bLineDraw; // !
			pt.x = rc.left;
			if( !bLineDraw ) pt.y += cyLine;
			if( pt.y > rc.bottom && bDraw ) break;
			ptLinkStart = pt;
			cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1);
			if( pt.x >= rc.right ) break;
		}
		else if( !bInRaw && ( *pstrText == L'<' || *pstrText == L'{' )
			&& ( pstrText[1] >= L'a' && pstrText[1] <= L'z' )
			&& ( pstrText[2] == L' ' || pstrText[2] == L'>' || pstrText[2] == L'}' ) ) {
				pstrText++;
				PCWSTR pstrNextStart = NULL;
				switch( *pstrText ) {
				case L'a':  // Link
					{
						pstrText++;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						if( iLinkIndex < nLinkRects && !bLineDraw ) {
							std::wstring *pStr = (std::wstring*)(sLinks + iLinkIndex);
							pStr->clear();
							while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'}' ) {
								PCWSTR pstrTemp = ::CharNext(pstrText);
								while( pstrText < pstrTemp) {
									*pStr += *pstrText++;
								}
							}
						}

						DWORD clrColor = pManager->getDefaultLinkFontColor();
						if( bHoverLink && iLinkIndex < nLinkRects ) {
							std::wstring *pStr = (std::wstring*)(sLinks + iLinkIndex);
							if( sHoverLink == *pStr ) clrColor = pManager->getDefaultLinkHoverFontColor();
						}
						//else if( prcLinks == NULL ) {
						//    if( ::PtInRect(&rc, ptMouse) )
						//        clrColor = pManager->GetDefaultLinkHoverFontColor();
						//}
						aColorArray.push_back(clrColor);//.Add((LPVOID)clrColor);
						::SetTextColor(hDC,  RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
						TFontInfo* pFontInfo = pManager->getDefaultFontInfo();
						if( aFontArray.size() > 0 ) pFontInfo = (TFontInfo*)aFontArray.at(aFontArray.size() - 1);
						if( pFontInfo->bUnderline == false ) {
							HFONT hFont = pManager->getFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
							if( hFont == NULL ) hFont = pManager->addFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
							pFontInfo = pManager->getFontInfo(hFont);
							aFontArray.push_back(pFontInfo);//.Add(pFontInfo);
							pTm = &pFontInfo->tm;
							::SelectObject(hDC, pFontInfo->hFont);
							cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
						}
						ptLinkStart = pt;
						bInLink = true;
					}
					break;
				case L'b':  // Bold
					{
						pstrText++;
						TFontInfo* pFontInfo = pManager->getDefaultFontInfo();
						if( aFontArray.size() > 0 ) pFontInfo = (TFontInfo*)aFontArray.at(aFontArray.size() - 1);
						if( pFontInfo->bBold == false ) {
							HFONT hFont = pManager->getFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
							if( hFont == NULL ) hFont = pManager->addFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
							pFontInfo = pManager->getFontInfo(hFont);
							aFontArray.push_back(pFontInfo);//.Add(pFontInfo);
							pTm = &pFontInfo->tm;
							::SelectObject(hDC, pFontInfo->hFont);
							cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
						}
					}
					break;
				case L'c':  // Color
					{
						pstrText++;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						if( *pstrText == L'#') pstrText++;
						DWORD clrColor = _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 16);
						aColorArray.push_back(clrColor);//.Add((LPVOID)clrColor);
						::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
					}
					break;
				case L'f':  // Font
					{
						pstrText++;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						PCWSTR pstrTemp = pstrText;
						int iFont = (int) _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
						//if( isdigit(*pstrText) ) { // debug版本会引起异常
						if( pstrTemp != pstrText ) {
							TFontInfo* pFontInfo = pManager->getFontInfo(iFont);
							aFontArray.push_back(pFontInfo);//.Add(pFontInfo);
							pTm = &pFontInfo->tm;
							::SelectObject(hDC, pFontInfo->hFont);
						}
						else {
							std::wstring sFontName;
							int iFontSize = 10;
							std::wstring sFontAttr;
							bool bBold = false;
							bool bUnderline = false;
							bool bItalic = false;
							while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'}' && *pstrText != L' ' ) {
								pstrTemp = ::CharNext(pstrText);
								while( pstrText < pstrTemp) {
									sFontName += *pstrText++;
								}
							}
							while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
							if( isdigit(*pstrText) ) {
								iFontSize = (int) _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
							}
							while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
							while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'}' ) {
								pstrTemp = ::CharNext(pstrText);
								while( pstrText < pstrTemp) {
									sFontAttr += *pstrText++;
								}
							}
							makeLower(sFontAttr);
							if( sFontAttr.find(L"bold") >= 0 ) bBold = true;
							if( sFontAttr.find(L"underline") >= 0 ) bUnderline = true;
							if( sFontAttr.find(L"italic") >= 0 ) bItalic = true;
							HFONT hFont = pManager->getFont(sFontName.c_str(), iFontSize, bBold, bUnderline, bItalic);
							if( hFont == NULL ) hFont = pManager->addFont(sFontName.c_str(), iFontSize, bBold, bUnderline, bItalic);
							TFontInfo* pFontInfo = pManager->getFontInfo(hFont);
							aFontArray.push_back(pFontInfo);//.Add(pFontInfo);
							pTm = &pFontInfo->tm;
							::SelectObject(hDC, pFontInfo->hFont);
						}
						cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
					}
					break;
				case L'i':  // Italic or Image
					{
						pstrNextStart = pstrText - 1;
						pstrText++;
						int iWidth = 0;
						int iHeight = 0;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						const TImageInfo* pImageInfo = NULL;
						std::wstring sName;
						while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'}' && *pstrText != L' ' ) {
							PCWSTR pstrTemp = ::CharNext(pstrText);
							while( pstrText < pstrTemp) {
								sName += *pstrText++;
							}
						}
						if( sName.empty() ) { // Italic
							pstrNextStart = NULL;
							TFontInfo* pFontInfo = pManager->getDefaultFontInfo();
							if( aFontArray.size() > 0 ) pFontInfo = (TFontInfo*)aFontArray.at(aFontArray.size() - 1);
							if( pFontInfo->bItalic == false ) {
								HFONT hFont = pManager->getFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
								if( hFont == NULL ) hFont = pManager->addFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
								pFontInfo = pManager->getFontInfo(hFont);
								aFontArray.push_back(pFontInfo);//.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
								cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
							}
						}
						else {
							while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
							int iImageListNum = (int) _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
							if( iImageListNum <= 0 ) iImageListNum = 1;
							while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
							int iImageListIndex = (int) _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
							if( iImageListIndex < 0 || iImageListIndex >= iImageListNum ) iImageListIndex = 0;

							pImageInfo = pManager->getImageEx(sName.c_str());
							if( pImageInfo ) {
								iWidth = pImageInfo->nX;
								iHeight = pImageInfo->nY;
								if( iImageListNum > 1 ) iWidth /= iImageListNum;

								if( pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0 ) {
									bLineEnd = true;
								}
								else {
									pstrNextStart = NULL;
									if( bDraw && bLineDraw ) {
										rect rcImage(pt.x, pt.y + cyLineHeight - iHeight, pt.x + iWidth, pt.y + cyLineHeight);
										if( iHeight < cyLineHeight ) { 
											rcImage.bottom -= (cyLineHeight - iHeight) / 2;
											rcImage.top = rcImage.bottom -  iHeight;
										}
										rect rcBmpPart(0, 0, iWidth, iHeight);
										rcBmpPart.left = iWidth * iImageListIndex;
										rcBmpPart.right = iWidth * (iImageListIndex + 1);
										rect rcCorner(0, 0, 0, 0);
										drawImage(hDC, pImageInfo->hBitmap, rcImage, rcImage, rcBmpPart, rcCorner, \
											pImageInfo->alphaChannel, 255);
									}

									cyLine = max(iHeight, cyLine);
									pt.x += iWidth;
									cyMinHeight = pt.y + iHeight;
									cxMaxWidth = max(cxMaxWidth, pt.x);
								}
							}
							else pstrNextStart = NULL;
						}
					}
					break;
				case L'n':  // Newline
					{
						pstrText++;
						if( (uStyle & DT_SINGLELINE) != 0 ) break;
						bLineEnd = true;
					}
					break;
				case L'p':  // Paragraph
					{
						pstrText++;
						if( pt.x > rc.left ) bLineEnd = true;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						int cyLineExtra = (int)_tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
						aPIndentArray.push_back(cyLineExtra);//.Add((LPVOID)cyLineExtra);
						cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra);
					}
					break;
				case L'r':  // Raw Text
					{
						pstrText++;
						bInRaw = true;
					}
					break;
				case L's':  // Selected text background color
					{
						pstrText++;
						bInSelected = !bInSelected;
						if( bDraw && bLineDraw ) {
							if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
							else ::SetBkMode(hDC, TRANSPARENT);
						}
					}
					break;
				case L'u':  // Underline text
					{
						pstrText++;
						TFontInfo* pFontInfo = pManager->getDefaultFontInfo();
						if( aFontArray.size() > 0 ) pFontInfo = (TFontInfo*)aFontArray.at(aFontArray.size() - 1);
						if( pFontInfo->bUnderline == false ) {
							HFONT hFont = pManager->getFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
							if( hFont == NULL ) hFont = pManager->addFont(pFontInfo->sFontName.c_str(), pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
							pFontInfo = pManager->getFontInfo(hFont);
							aFontArray.push_back(pFontInfo);//.Add(pFontInfo);
							pTm = &pFontInfo->tm;
							::SelectObject(hDC, pFontInfo->hFont);
							cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
						}
					}
					break;
				case L'x':  // X Indent
					{
						pstrText++;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						int iWidth = (int) _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
						pt.x += iWidth;
						cxMaxWidth = max(cxMaxWidth, pt.x);
					}
					break;
				case L'y':  // Y Indent
					{
						pstrText++;
						while( *pstrText > L'\0' && *pstrText <= L' ' ) pstrText = ::CharNext(pstrText);
						cyLine = (int) _tcstol(pstrText, const_cast<PWSTR*>(&pstrText), 10);
					}
					break;
				}
				if( pstrNextStart != NULL ) pstrText = pstrNextStart;
				else {
					while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'}' ) pstrText = ::CharNext(pstrText);
					pstrText = ::CharNext(pstrText);
				}
		}
		else if( !bInRaw && ( *pstrText == L'<' || *pstrText == L'{' ) && pstrText[1] == L'/' )
		{
			pstrText++;
			pstrText++;
			switch( *pstrText )
			{
			case L'c':
				{
					pstrText++;
					//aColorArray.Remove(aColorArray.GetSize() - 1);
					aColorArray.pop_back();
					DWORD clrColor = dwTextColor;
					if( aColorArray.size() > 0 ) clrColor = (int)aColorArray.at(aColorArray.size() - 1);
					::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
				}
				break;
			case L'p':
				pstrText++;
				if( pt.x > rc.left ) bLineEnd = true;
				//aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
				aPIndentArray.pop_back();
				cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
				break;
			case L's':
				{
					pstrText++;
					bInSelected = !bInSelected;
					if( bDraw && bLineDraw ) {
						if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
						else ::SetBkMode(hDC, TRANSPARENT);
					}
				}
				break;
			case L'a':
				{
					if( iLinkIndex < nLinkRects ) {
						if( !bLineDraw ) ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, min(pt.x, rc.right), pt.y + pTm->tmHeight + pTm->tmExternalLeading);
						iLinkIndex++;
					}
					//aColorArray.Remove(aColorArray.GetSize() - 1);
					aColorArray.pop_back();
					DWORD clrColor = dwTextColor;
					if( aColorArray.size() > 0 ) clrColor = (int)aColorArray.at(aColorArray.size() - 1);
					::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
					bInLink = false;
				}
			case L'b':
			case L'f':
			case L'i':
			case L'u':
				{
					pstrText++;
					//aFontArray.Remove(aFontArray.GetSize() - 1);
					aFontArray.pop_back();
					TFontInfo* pFontInfo = (TFontInfo*)aFontArray.at(aFontArray.size() - 1);
					if( pFontInfo == NULL ) pFontInfo = pManager->getDefaultFontInfo();
					if( pTm->tmItalic && pFontInfo->bItalic == false ) {
						ABC abc;
						::GetCharABCWidths(hDC, L' ', L' ', &abc);
						pt.x += abc.abcC / 2; // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
					}
					pTm = &pFontInfo->tm;
					::SelectObject(hDC, pFontInfo->hFont);
					cyLine = max(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.at(aPIndentArray.size() - 1));
				}
				break;
			}
			while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'}' ) pstrText = ::CharNext(pstrText);
			pstrText = ::CharNext(pstrText);
		}
		else if( !bInRaw &&  *pstrText == L'<' && pstrText[2] == L'>' && (pstrText[1] == L'{'  || pstrText[1] == L'}') )
		{
			SIZE szSpace = { 0 };
			::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
			if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
			pt.x += szSpace.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText++;pstrText++;pstrText++;
		}
		else if( !bInRaw &&  *pstrText == L'{' && pstrText[2] == L'}' && (pstrText[1] == L'<'  || pstrText[1] == L'>') )
		{
			SIZE szSpace = { 0 };
			::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
			if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
			pt.x += szSpace.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText++;pstrText++;pstrText++;
		}
		else if( !bInRaw &&  *pstrText == L' ' )
		{
			SIZE szSpace = { 0 };
			::GetTextExtentPoint32(hDC, L" ", 1, &szSpace);
			// Still need to paint the space because the font might have
			// underline formatting.
			if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, L" ", 1);
			pt.x += szSpace.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText++;
		}
		else
		{
			POINT ptPos = pt;
			int cchChars = 0;
			int cchSize = 0;
			int cchLastGoodWord = 0;
			int cchLastGoodSize = 0;
			PCWSTR p = pstrText;
			PCWSTR pstrNext;
			SIZE szText = { 0 };
			if( !bInRaw && *p == L'<' || *p == L'{' ) p++, cchChars++, cchSize++;
			while( *p != L'\0' && *p != L'\n' ) {
				// This part makes sure that we're word-wrapping if needed or providing support
				// for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
				// slow when repeated so often.
				// TODO: Rewrite and use GetTextExtentExPoint() instead!
				if( bInRaw ) {
					if( ( *p == L'<' || *p == L'{' ) && p[1] == L'/' 
						&& p[2] == L'r' && ( p[3] == L'>' || p[3] == L'}' ) ) {
							p += 4;
							bInRaw = false;
							break;
					}
				}
				else {
					if( *p == L'<' || *p == L'{' ) break;
				}
				pstrNext = ::CharNext(p);
				cchChars++;
				cchSize += (int)(pstrNext - p);
				szText.cx = cchChars * pTm->tmMaxCharWidth;
				if( pt.x + szText.cx >= rc.right ) {
					::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
				}
				if( pt.x + szText.cx > rc.right ) {
					if( pt.x + szText.cx > rc.right && pt.x != rc.left) {
						cchChars--;
						cchSize -= (int)(pstrNext - p);
					}
					if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
						cchChars = cchLastGoodWord;
						cchSize = cchLastGoodSize;                 
					}
					if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0 ) {
						cchChars -= 1;
						PCWSTR pstrPrev = ::CharPrev(pstrText, p);
						if( cchChars > 0 ) {
							cchChars -= 1;
							pstrPrev = ::CharPrev(pstrText, pstrPrev);
							cchSize -= (int)(p - pstrPrev);
						}
						else 
							cchSize -= (int)(p - pstrPrev);
						pt.x = rc.right;
					}
					bLineEnd = true;
					cxMaxWidth = max(cxMaxWidth, pt.x);
					break;
				}
				if (!( ( p[0] >= L'a' && p[0] <= L'z' ) || ( p[0] >= L'A' && p[0] <= L'Z' ) )) {
					cchLastGoodWord = cchChars;
					cchLastGoodSize = cchSize;
				}
				if( *p == L' ' ) {
					cchLastGoodWord = cchChars;
					cchLastGoodSize = cchSize;
				}
				p = ::CharNext(p);
			}

			::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
			if( bDraw && bLineDraw ) {
				::TextOut(hDC, ptPos.x, ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, pstrText, cchSize);
				if( pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) 
					::TextOut(hDC, ptPos.x + szText.cx, ptPos.y, L"...", 3);
			}
			pt.x += szText.cx;
			cxMaxWidth = max(cxMaxWidth, pt.x);
			pstrText += cchSize;
		}

		if( pt.x >= rc.right || *pstrText == L'\n' || *pstrText == L'\0' ) bLineEnd = true;
		if( bDraw && bLineEnd ) {
			if( !bLineDraw ) {
				aFontArray.resize(aLineFontArray.size());
				::CopyMemory(aFontArray.data(), aLineFontArray.data(), aLineFontArray.size() * sizeof(TFontInfo *));
				aColorArray.resize(aLineColorArray.size());
				::CopyMemory(aColorArray.data(), aLineColorArray.data(), aLineColorArray.size() * sizeof(DWORD));
				aPIndentArray.resize(aLinePIndentArray.size());
				::CopyMemory(aPIndentArray.data(), aLinePIndentArray.data(), aLinePIndentArray.size() * sizeof(int));

				cyLineHeight = cyLine;
				pstrText = pstrLineBegin;
				bInRaw = bLineInRaw;
				bInSelected = bLineInSelected;

				DWORD clrColor = dwTextColor;
				if( aColorArray.size() > 0 ) clrColor = (int)aColorArray.at(aColorArray.size() - 1);
				::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
				TFontInfo* pFontInfo = (TFontInfo*)aFontArray.at(aFontArray.size() - 1);
				if( pFontInfo == NULL ) pFontInfo = pManager->getDefaultFontInfo();
				pTm = &pFontInfo->tm;
				::SelectObject(hDC, pFontInfo->hFont);
				if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
			}
			else {
				aLineFontArray.resize(aFontArray.size());
				::CopyMemory(aLineFontArray.data(), aFontArray.data(), aFontArray.size() * sizeof(TFontInfo*));
				aLineColorArray.resize(aColorArray.size());
				::CopyMemory(aLineColorArray.data(), aColorArray.data(), aColorArray.size() * sizeof(DWORD));
				aLinePIndentArray.resize(aPIndentArray.size());
				::CopyMemory(aLinePIndentArray.data(), aPIndentArray.data(), aPIndentArray.size() * sizeof(int));
				pstrLineBegin = pstrText;
				bLineInSelected = bInSelected;
				bLineInRaw = bInRaw;
			}
		}

		assert(iLinkIndex<=nLinkRects);
	}

	nLinkRects = iLinkIndex;

	// Return size of text when requested
	if( (uStyle & DT_CALCRECT) != 0 ) {
		rc.bottom = max(cyMinHeight, pt.y + cyLine);
		rc.right = min(rc.right, cxMaxWidth);
	}

	if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);

	::SelectObject(hDC, hOldFont);
}

HBITMAP renderEngine::generateBitmap(paintManagerUI* pManager, controlUI* pControl, RECT rc)
{
	int cx = rc.right - rc.left;
	int cy = rc.bottom - rc.top;

	HDC hPaintDC = ::CreateCompatibleDC(pManager->getPaintDC());
	HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->getPaintDC(), rc.right, rc.bottom);
	assert(hPaintDC);
	assert(hPaintBitmap);
	HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
	pControl->doPaint(hPaintDC, rc);

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
	LPDWORD pDest = NULL;
	HDC hCloneDC = ::CreateCompatibleDC(pManager->getPaintDC());
	HBITMAP hBitmap = ::CreateDIBSection(pManager->getPaintDC(), &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
	assert(hCloneDC);
	assert(hBitmap);
	if( hBitmap != NULL )
	{
		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
		::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
		::SelectObject(hCloneDC, hOldBitmap);
		::DeleteDC(hCloneDC);  
		::GdiFlush();
	}

	// Cleanup
	::SelectObject(hPaintDC, hOldPaintBitmap);
	::DeleteObject(hPaintBitmap);
	::DeleteDC(hPaintDC);

	return hBitmap;
}

}