#include "wucGif.h"
#include "image/stb_image.c"
namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gifHandler	类 2016-3-4	kp007
//	保留gif数据
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
gifHandler::gifHandler()
	: m_iCurrentFrame(0), m_iFrameCount(0)
	, m_isDeleting(false)
{

}

gifHandler::~gifHandler()
{
	freeImage();
}
void gifHandler::freeImage()
{
	for (std::vector<BSImageInfo *>::iterator iter = m_vecImageInfo.begin(); iter != m_vecImageInfo.end(); ++iter)
	{
		if ((*iter)->hBitmap)
		{
			DeleteObject((*iter)->hBitmap);
		}
		delete *iter;
	}
	m_vecImageInfo.clear();
}

int gifHandler::getFrameCount()
{
	return (int)m_vecImageInfo.size();
}

void gifHandler::addFrameInfo(BSImageInfo* pFrameInfo)
{
	assert(pFrameInfo);
	if (pFrameInfo)
	{
		m_vecImageInfo.push_back(pFrameInfo);
	}
}

BSImageInfo* gifHandler::getNextFrameInfo()
{
	if (!m_isDeleting)
	{
		int n = m_iCurrentFrame++;
		if (m_iCurrentFrame >= (int)m_vecImageInfo.size())
		{
			m_iCurrentFrame = 0;
		}

		return m_vecImageInfo[n];
	}
	return nullptr;
}

BSImageInfo* gifHandler::getCurrentFrameInfo()
{
	if (!m_isDeleting)
	{
		return m_vecImageInfo[m_iCurrentFrame];
	}
	return nullptr;
}

BSImageInfo* gifHandler::getFrameInfoAt(int index)
{
	assert(index >= 0 && index < (int)m_vecImageInfo.size());
	if(index < 0 || index >= (int)m_vecImageInfo.size()) return nullptr;
	if(!m_isDeleting)
	{	
		return m_vecImageInfo[index];
	}

	return nullptr;
}

struct TGifInfo
{
	unsigned char* pData;
	int w;
	int h;
	int delay;
};
/*static*/ gifHandler* gifHandler::load(PCWSTR pfile, DWORD mask /*= 0*/)
{
	assert(pfile);

	gifHandler *pGif = nullptr;
	LPBYTE pData = NULL;
	DWORD dwSize = 0;

	HANDLE hFile = ::CreateFile(pfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
		FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE ) return nullptr;
	dwSize = ::GetFileSize(hFile, NULL);
	if( dwSize > 0 )
	{
		DWORD dwRead = 0;
		pData = new BYTE[ dwSize ];
		::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize ) {
			delete[] pData;
			pData = NULL;
		}

	}
	if(!pData) return nullptr;
	
	// 构建gif句柄
	LPBYTE pImage = NULL;
	int x,y,n;
	int count = 0;
	int delay = 0;
	pGif = new gifHandler();

	TGifInfo* pGifInfos = (TGifInfo*)gif_load_from_memory(pData, dwSize, &count, &n, 4);
	delete[] pData;
	pData = NULL;
	for (int i = 0; i <count; i++ )
	{
		pImage = pGifInfos[i].pData;
		x = pGifInfos[i].w;
		y = pGifInfos[i].h;
		delay = pGifInfos[i].delay;

		if( !pImage ) 
		{
			stbi_image_free(pImage);
			continue;
		}
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
		if( !hBitmap ) 
		{
			stbi_image_free(pImage);
			continue;
		}

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
			bAlphaChannel = true;
		}//end for

		BSImageInfo* data = new BSImageInfo;
		data->hBitmap = hBitmap;
		data->nX = x;
		data->nY = y;
		data->delay = delay;
		data->alphaChannel = bAlphaChannel;
		pGif->addFrameInfo(data);

		stbi_image_free(pImage);
		pImage = NULL;

	}//end for
	stbi_image_free(pGifInfos);


	return pGif;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	buttonGifUC	类 2016-3-4	kp007
//	gif动画按钮
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
buttonGifUC::buttonGifUC()
	: m_hGif(nullptr)
	, m_iPreUpdateDelay(0), m_isUpdateTime(false)
{

}
buttonGifUC::~buttonGifUC()
{
	m_pManager->killTimer(this, GIF_TIMER_ID);

	if (m_hGif)
	{
		delete m_hGif;
		m_hGif = nullptr;
	}
}

PCWSTR buttonGifUC::getClass() const
{
	return L"buttonGifUC";
}
LPVOID buttonGifUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"ButtonGif") == 0) return static_cast<buttonGifUC *>(this);
	return __super::getInterface(pstrName);
}
void buttonGifUC::Event(BSEventUC& event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}
	if( event.Type == BEUCEventTimer && event.wParam == GIF_TIMER_ID )
	{
		m_isUpdateTime = true;
		invalidate();
		return;
	}
	__super::Event(event);
}
void buttonGifUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = nullptr;
	if( _tcscmp(pstrName, L"giffile") == 0 ) setGifFile(pstrValue);
	//else if( _tcscmp(pstrName, L"min") == 0 ) setMinValue(_tcstol(pstrValue, NULL, 10));
	else __super::setAttribute(pstrName, pstrValue);
}

void buttonGifUC::paintStatusImage(HDC hdc)
{
	if(!m_hGif) return __super::paintStatusImage(hdc);
	BSImageInfo* pImageInfo = NULL;
	if (m_isUpdateTime)
	{
		m_isUpdateTime = false;
		pImageInfo = m_hGif->getNextFrameInfo();
	}
	else
	{
		pImageInfo = m_hGif->getCurrentFrameInfo();
	}
	if (pImageInfo)
	{
		RECT rcBmpPart = {0};
		RECT rcCorners = {0};
		rcBmpPart.right = pImageInfo->nX;
		rcBmpPart.bottom = pImageInfo->nY;
		gtc::drawer::drawImage(hdc,pImageInfo->hBitmap,m_rcItem, m_rcPaint,rcBmpPart,rcCorners,pImageInfo->alphaChannel,255);
		if (m_iPreUpdateDelay != pImageInfo->delay)
		{
			m_pManager->killTimer(this, GIF_TIMER_ID);
			m_pManager->setTimer(this, GIF_TIMER_ID, pImageInfo->delay);
			m_iPreUpdateDelay = pImageInfo->delay;
		}

	}
}
void buttonGifUC::setVisible(bool bVisible /*= true*/)
{
	if(!bVisible) m_pManager->killTimer(this, GIF_TIMER_ID);
	__super::setVisible(bVisible);
}

void buttonGifUC::setGifFile(PCWSTR pFile)
{
	assert(pFile);
	if(!pFile) return;

	if (m_hGif)
	{
		m_pManager->killTimer(this, GIF_TIMER_ID);
		m_iPreUpdateDelay = 0;
		delete m_hGif;
		m_hGif = nullptr;
	}
	m_hGif = gifHandler::load(pFile);
	invalidate();
}
}	// wuc