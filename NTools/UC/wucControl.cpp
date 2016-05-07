#include "wucControl.h"

namespace wuc{

controlUC::controlUC(void)
	: m_pManager(nullptr), m_wsName(L""), m_pParent(nullptr)
	, m_wsText(L"")
	, m_bVisible(true), m_bInternVisible(true)
	, m_bFocused(false), m_bEnabled(true), m_bMouseEnabled(true)
	, m_dwBackColor(0), m_dwBorderColor(0x0), m_dwFocusBorderColor(0x0)
	, m_pImage(nullptr), m_brefImage(true)
	, m_bUpdateNeeded(true)
	, m_bMenuUsed(false), m_chShortcut(L'\0')
	, m_iBorderSize(1)
	, m_bFloat(false)
	, m_pTag(NULL)
	, m_iFont(-1)
{
	m_szxyFixed.cx = m_szxyFixed.cy = 0;
	m_szXY.cx = m_szXY.cy = 0;
	m_szxyMin.cx = m_szxyMin.cy = 0;
	m_szxyMax.cx = m_szxyMax.cy = 9999;
	m_szxyBorderRound.cx = m_szxyBorderRound.cy = 0;

	//::ZeroMemory(&m_szPatch, sizeof(RECT));
	::ZeroMemory(&m_rcItem, sizeof(RECT));
	::ZeroMemory(&m_rcPadding, sizeof(RECT));
	::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_bsRelativePos, sizeof(BSRelativePosUC));
}


controlUC::~controlUC(void)
{
	//SEND_LOG(L"wuc::object %08X class:<%s> name:<%s> free ... ", this, getClass(), getName().c_str());
	if(!m_brefImage && m_pImage) 
		delete m_pImage;
	if(m_pManager) m_pManager->reapObject(this);
}

const std::wstring& controlUC::getName() const
{
	return m_wsName;
}
void controlUC::setName(PCWSTR pstrName)
{
	m_wsName = pstrName?pstrName:L"";
}
PCWSTR controlUC::getClass() const
{
	return L"controlUC";
}

LPVOID controlUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Control") == 0) return static_cast<controlUC *>(this);
	return nullptr;
}

UINT controlUC::getControlFlags() const
{
	return 0;
}
bool controlUC::activate()
{
	if( !isVisible() ) return false;
	if( !isEnabled() ) return false;
	return true;
}

drawManager* controlUC::getManager()
{
	return m_pManager;
}
void controlUC::onInit()
{

}
void controlUC::setManager(drawManager *pManager, controlUC *pParent, bool bInit /*= true*/)
{
	m_pManager = pManager;
	m_pParent = pParent;
	
	if(bInit && m_pParent) onInit();
}

controlUC* controlUC::getParent() const
{
	return m_pParent;
}

// 文本相关
PCWSTR controlUC::getText() const
{
	return m_wsText.c_str();
}
void controlUC::setText(PCWSTR pstrText)
{
	if(m_wsText.compare(pstrText) == 0) return;
	m_wsText = pstrText;
	invalidate();
}
void controlUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"pos") == 0 ) {
		RECT rcPos = { 0 };
		rcPos.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcPos.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcPos.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
		setFixedXY(szXY);
		setFixedWidth(rcPos.right - rcPos.left);
		setFixedHeight(rcPos.bottom - rcPos.top);
	}
	else if( _tcscmp(pstrName, L"fixedxy") == 0 ) {
		SIZE szXY = {0};
		szXY.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		szXY.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		setFixedXY(szXY);
	}

	//else if( _tcscmp(pstrName, L"relativepos") == 0 ) {
	//	SIZE szMove,szZoom;
	//	PWSTR pstr = NULL;
	//	szMove.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
	//	szMove.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
	//	szZoom.cx = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
	//	szZoom.cy = _tcstol(pstr + 1, &pstr, 10); assert(pstr); 
	//	setRelativePos(szMove,szZoom);
	//}
	else if( _tcscmp(pstrName, L"padding") == 0 ) {
		RECT rcPadding = { 0 };
		rcPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setPadding(rcPadding);
	}
	else if( _tcscmp(pstrName, L"bkcolor") == 0 || _tcscmp(pstrName, L"bkcolor1") == 0 ) {
		while( *pstrValue > L'\0' && *pstrValue <= L' ')  pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setBkColor(clrColor);
	}
	//else if( _tcscmp(pstrName, L"bkcolor2") == 0 ) {
	//	while( *pstrValue > L'\0' && *pstrValue <= L' ' ) pstrValue = ::CharNext(pstrValue);
	//	if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
	//	PWSTR pstr = NULL;
	//	DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
	//	setBkColor2(clrColor);
	//}
	//else if( _tcscmp(pstrName, L"bkcolor3") == 0 ) {
	//	while( *pstrValue > L'\0' && *pstrValue <= L' ' ) pstrValue = ::CharNext(pstrValue);
	//	if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
	//	PWSTR pstr = NULL;
	//	DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
	//	setBkColor3(clrColor);
	//}
	else if( _tcscmp(pstrName, L"bordercolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setBorderColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"focusbordercolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setFocusBorderColor(clrColor);
	}
	//else if( _tcscmp(pstrName, L"colorhsl") == 0 ) setColorHSL(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"bordersize") == 0 ) setBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"borderround") == 0 ) {
		SIZE cxyRound = { 0 };
		cxyRound.cx = _tcstol(pstrValue, &pstr, 10);	assert(pstr);    
		cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);		assert(pstr);     
		setBorderRound(cxyRound);
	}
	//else if( _tcscmp(pstrName, L"imagepatch") == 0 ) {
	//	SIZE szPatch = { 0 };
	//	szPatch.cx = _tcstol(pstrValue, &pstr, 10);		assert(pstr);    
	//	szPatch.cy = _tcstol(pstr + 1, &pstr, 10);		assert(pstr);     
	//	setImagePatch(szPatch);
	//}
	else if( _tcscmp(pstrName, L"image") == 0 ) setImage(pstrValue);
	else if( _tcscmp(pstrName, L"bkimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setBkImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"font") == 0 ) setFont(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"width") == 0 ) setFixedWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"height") == 0 ) setFixedHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"minwidth") == 0 ) setMinWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"minheight") == 0 ) setMinHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"maxwidth") == 0 ) setMaxWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"maxheight") == 0 ) setMaxHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"name") == 0 ) setName(pstrValue);
	else if( _tcscmp(pstrName, L"text") == 0 ) setText(pstrValue);
	else if( _tcscmp(pstrName, L"tooltip") == 0 ) setToolTip(pstrValue);
	else if( _tcscmp(pstrName, L"userdata") == 0 ) setUserData(pstrValue);
	else if( _tcscmp(pstrName, L"enabled") == 0 ) setEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"mouse") == 0 ) setMouseEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"visible") == 0 ) setVisible(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"float") == 0 ) setFloat(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"shortcut") == 0 ) setShortcut(pstrValue[0]);
	else if( _tcscmp(pstrName, L"menu") == 0 ) setContextMenuUsed(_tcscmp(pstrValue, L"true") == 0);
	else 
	{
		assert(!L"未找到指定属性");
	}
}

controlUC* controlUC::applyAttributeList(PCWSTR pstrList)
{
	std::wstring sItem;
	std::wstring sValue;
	// 格式为 “name1=\"value1\" name2=\"value2\" ”
	// 查验pstrList是否有“=”字符以便确认其是属性列表值或值对应的key
	bool bKeyName = true;
	PCWSTR pstrOffset = pstrList;
	while (*pstrOffset != L'\0')
	{
		if (*pstrOffset == L'=')
		{
			bKeyName = false;
			break;
		}
		++pstrOffset;
	}
	if (bKeyName)
	{
		//assert(m_pManager);
		pstrList = drawManager::getDefaultAttributeList(pstrList);
	}
	while( *pstrList != L'\0' ) {
		sItem.clear();
		sValue.clear();
		while( *pstrList != L'\0' && *pstrList != L'=' ) {
			PWSTR pstrTemp = ::CharNext(pstrList);
			while( pstrList < pstrTemp) {
				sItem += *pstrList++;
			}
		}
		assert( *pstrList == L'=' );
		if( *pstrList++ != L'=' ) return this;
		// 去掉多余空格
		while(*pstrList != L'\0' && *pstrList == L' ') pstrList++;
		assert( *pstrList == L'\"' );
		if( *pstrList++ != L'\"' ) return this;
		while( *pstrList != L'\0' && *pstrList != L'\"' ) {
			PWSTR pstrTemp = ::CharNext(pstrList);
			while( pstrList < pstrTemp) {
				sValue += *pstrList++;
			}
		}
		assert( *pstrList == L'\"' );
		if( *pstrList++ != L'\"' ) return this;
		setAttribute(sItem.c_str(), sValue.c_str());
		//assert( *pstrList == L' ' );
		if( *pstrList++ != L' ' ) return this;
		// 去掉多余空格
		while(*pstrList != L'\0' && *pstrList == L' ') pstrList++;
	}
	return this;
}

SIZE controlUC::estimateSize(SIZE szAvailable)
{
	return m_szxyFixed;
}

SIZE controlUC::autoCalculateSize()
{
	gtc::rect rc;
	rc = gtc::drawer::measureString(m_wsText.c_str(), rc, m_pManager->getFont(getFont()), DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	return rc.getSize();
}
RECT controlUC::getPadding() const
{
	return m_rcPadding;
}
void controlUC::setPadding(RECT rcPadding)
{
	m_rcPadding = rcPadding;
	needParentUpdate();
}
void controlUC::setPadding(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPadding(rc);
}
SIZE controlUC::getFixedXY() const
{
	 return m_szXY;
}
void controlUC::setFixedXY(SIZE szXY)
{
	m_szXY.cx = szXY.cx;
	m_szXY.cy = szXY.cy;
	if(!m_bFloat) needParentUpdate();
	else needUpdate();
}
void controlUC::setFixedXY(int fixedX, int fixedY)
{
	SIZE sz = {fixedX, fixedY};
	setFixedXY(sz);
}
void controlUC::setFixedSize(SIZE sz)
{
	setFixedWidth(sz.cx);
	setFixedHeight(sz.cy);
}

void controlUC::setFixedSize(LONG cx, LONG cy)
{
	setFixedWidth(cx);
	setFixedHeight(cy);

}
int controlUC::getFixedWidth() const
{
	return m_szxyFixed.cx;
}

void controlUC::setFixedWidth(int cx)
{
	if( cx < 0 ) return; 
	m_szxyFixed.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getFixedHeight() const
{
	return m_szxyFixed.cy;
}

void controlUC::setFixedHeight(int cy)
{
	if( cy < 0 ) return; 
	m_szxyFixed.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}
int controlUC::getMinWidth() const
{
	return m_szxyMin.cx;
}

void controlUC::setMinWidth(int cx)
{
	if( m_szxyMin.cx == cx ) return;

	if( cx < 0 ) return; 
	m_szxyMin.cx = cx;
	needUpdate();
}

int controlUC::getMaxWidth() const
{
	return m_szxyMax.cx;
}

void controlUC::setMaxWidth(int cx)
{
	if( m_szxyMax.cx == cx ) return;

	if( cx < 0 ) return; 
	m_szxyMax.cx = cx;
	needUpdate();
}

int controlUC::getMinHeight() const
{
	return m_szxyMin.cy;
}

void controlUC::setMinHeight(int cy)
{
	if( m_szxyMin.cy == cy ) return;

	if( cy < 0 ) return; 
	m_szxyMin.cy = cy;
	needUpdate();
}

int controlUC::getMaxHeight() const
{
	return m_szxyMax.cy;
}

void controlUC::setMaxHeight(int cy)
{
	if( m_szxyMax.cy == cy ) return;

	if( cy < 0 ) return; 
	m_szxyMax.cy = cy;
	needUpdate();
}

inline int controlUC::getBorderSize() const
{
	return m_iBorderSize;
}

inline void controlUC::setBorderSize(int nSize)
{
	if( m_iBorderSize == nSize ) return;

	m_iBorderSize = nSize;
	invalidate();
}

inline SIZE controlUC::getBorderRound() const
{
	return m_szxyBorderRound;
}

inline void controlUC::setBorderRound(SIZE cxyRound)
{
	m_szxyBorderRound = cxyRound;
	invalidate();
}
int controlUC::getFont() const
{
	return m_iFont;
}
void controlUC::setFont(int fontIndex)
{
	if(m_iFont == fontIndex) return;
	m_iFont = fontIndex;
	invalidate();
}
inline DWORD controlUC::getBorderColor() const
{
	return m_dwBorderColor;
}

inline void controlUC::setBorderColor(DWORD dwBorderColor)
{
	if( m_dwBorderColor == dwBorderColor ) return;

	m_dwBorderColor = dwBorderColor;
	invalidate();
}

inline DWORD controlUC::getFocusBorderColor() const
{
	return m_dwFocusBorderColor;
}

inline void controlUC::setFocusBorderColor(DWORD dwBorderColor)
{
	if( m_dwFocusBorderColor == dwBorderColor ) return;

	m_dwFocusBorderColor = dwBorderColor;
	invalidate();
}

inline DWORD controlUC::getBkColor() const
{
	return m_dwBackColor;
}

inline void controlUC::setBkColor(DWORD dwBackColor)
{
	if( m_dwBackColor == dwBackColor ) return;

	m_dwBackColor = dwBackColor;
	invalidate();
}
//void controlUC::setImagePatch(SIZE szPatch)
//{
//	m_szPatch = szPatch;
//}
//void controlUC::setImagePatch(int cx, int cy)
//{
//	m_szPatch.cx = cx;
//	m_szPatch.cy = cy;
//}
//SIZE controlUC::getImagePatch() const
//{
//	return m_szPatch;
//}
void controlUC::setImage(PCWSTR pImage, bool bRefImg /*= true*/)
{
	gtc::image *pimg = analyzeImage(pImage);// drawManager::getResource().findImage(pImage);
	setImage(pimg, bRefImg);
}

void controlUC::setImage(gtc::image *pImage, bool bRefImg /*= true*/)
{
	if (!m_brefImage && m_pImage)
	{
		delete m_pImage;
		m_pImage = nullptr;
	}
	m_brefImage = bRefImg;
	if(m_brefImage && m_pImage == pImage) return;
	if(m_brefImage)
		m_pImage = pImage;
	else
		m_pImage = pImage->clone();
	invalidate();
}

void controlUC::clearImage()
{
	m_pImage = NULL;
	invalidate();
}
gtc::image* controlUC::getImage() const
{
	return m_pImage;
}
bool controlUC::getBkImage(RECT &rc) const
{
	return existsImage(UCIMAGE_STATE_BACKGROUND, &rc);
}
void controlUC::setBkImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setBkImage(rc);
}
void controlUC::setBkImage(RECT rcImgCell)
{
	assert(m_pImage);
	setCellImage(UCIMAGE_STATE_BACKGROUND, rcImgCell);
	invalidate();
	//RECT rc = getBkImage();
	//if(rc == rcImgCell) return;
	//if (m_pImage)
	//{
	//	SIZE sz = m_pImage->getImageSize();
	//	if(rectWidth(rcImgCell) > sz.cx)
	//		rcImgCell.right = rcImgCell.left + sz.cx;	// 是否应该-1
	//	if(rectHeight(rcImgCell) > sz.cy)
	//		rcImgCell.bottom = rcImgCell.top + sz.cy;
	//}
	//m_mapImage[UCIMAGE_STATE_BACKGROUND] = rcImgCell;

	////if(m_wsBkImage == pImage ) return;
	////m_wsBkImage = pImage;
	//invalidate();
}
bool controlUC::setCellImage(UINT keyFlag, RECT rcCell)
{
	ImageCellType::const_iterator iter = m_mapImage.find(keyFlag);
	if (iter != m_mapImage.cend())
	{
		if(rcCell == iter->second) return false;
	}
	if(rcCell.left < 0) rcCell.left = 0;
	if(rcCell.top < 0) rcCell.top = 0;
	if (m_pImage)
	{
		SIZE sz = m_pImage->getImageSize();
		if(rcCell.right > sz.cx) rcCell.right = sz.cx;
		if(rcCell.bottom > sz.cy) rcCell.bottom  = sz.cy;
		//if(rectWidth(rcCell) > sz.cx)
		//	rcCell.right = rcCell.left + sz.cx;
		//if(rectHeight(rcCell) > sz.cy)
		//	rcCell.bottom = rcCell.top + sz.cy;
	}
	m_mapImage[keyFlag] = rcCell;
	return true;
}
bool controlUC::existsImage(UINT keyFlag, RECT *prcCell /*= NULL*/) const
{
	bool bRet = false;
	if (m_pImage)
	{
		ImageCellType::const_iterator iter = m_mapImage.find(keyFlag);
		if (iter != m_mapImage.cend())
		{
			bRet = true;
			if(prcCell) *prcCell = iter->second;
		}
	}
	return bRet;
}
gtc::image* controlUC::analyzeImage(PCWSTR pstrImage)
{
	assert(pstrImage);
	if(!pstrImage) return nullptr;
	// 1、aaa
	// 2、name='aaa' corner='0,0,0,0'
	gtc::image *pImage = nullptr;
	std::wstring wsItem, wsValue;
	bool bCorner = false;
	RECT rcCorner = {0};
	PWSTR pstr = NULL;
	while (*pstrImage != L'\0')
	{
		wsItem.clear();
		wsValue.clear();
		while(*pstrImage > L'\0' && *pstrImage <= L' ') pstrImage = CharNext(pstrImage);	// 跳过前面的无效字符
		while(*pstrImage != L'\0' && *pstrImage != L'=' && *pstrImage > L' ')
		{
			PWSTR pstrTemp = CharNext(pstrImage);
			while(pstrImage < pstrTemp) {
				wsItem += *pstrImage++;
			}
		}
		while(*pstrImage > L'\0' && *pstrImage <= L' ') pstrImage = CharNext(pstrImage);	// 移出=左侧无效字符
		if(*pstrImage++ != L'=' ) break;
		while(*pstrImage > L'\0' && *pstrImage <= L' ') pstrImage = CharNext(pstrImage);	// 移出=右侧无效字符
		if(*pstrImage++ != L'\'' ) break;
		while(*pstrImage != L'\0' && *pstrImage != L'\'')
		{
			PWSTR pstrTemp = CharNext(pstrImage);
			while(pstrImage < pstrTemp) {
				wsValue += *pstrImage++;
			}
		}
		if(*pstrImage++ != L'\'' ) break;
		if (!wsValue.empty())
		{
			if (wsItem == L"name")
			{
				pImage = drawManager::getResource().findImage(wsValue);
				if(!pImage) return nullptr;
			}
			else if (wsItem == L"corner")
			{
				rcCorner.left = _tcstol(wsValue.data(), &pstr, 10);  assert(pstr);    
				rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
				rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
				rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
				bCorner = true;
			}
		}
		if(*pstrImage++ != L' ') break;
	}
	if (!pImage && wsValue.empty() && !wsItem.empty())
	{
		pImage = drawManager::getResource().findImage(wsItem);
	}
	if (pImage && bCorner)
	{
		pImage->setPatch(rcCorner);
	}
	return pImage;
}

bool controlUC::existsImage(const ImageCellType &mapImages, UINT keyFlag, RECT *prcCell /*= NULL*/)
{
	bool bRet = false;
	ImageCellType::const_iterator iter = mapImages.find(keyFlag);
	if (iter != mapImages.cend())
	{
		bRet = true;
		if(prcCell) *prcCell = iter->second;
	}
	
	return bRet;
}
bool controlUC::setCellImage(ImageCellType &mapImages, UINT keyFlag, RECT rcCell, gtc::image *pImage /*= NULL*/)
{
	ImageCellType::const_iterator iter = mapImages.find(keyFlag);
	if (iter != mapImages.cend())
	{
		if(rcCell == iter->second) return false;
	}
	if(rcCell.left < 0) rcCell.left = 0;
	if(rcCell.top < 0) rcCell.top = 0;
	if (pImage)
	{
		SIZE sz = pImage->getImageSize();
		if(rcCell.right > sz.cx) rcCell.right = sz.cx;
		if(rcCell.bottom > sz.cy) rcCell.bottom  = sz.cy;
		//if(rectWidth(rcCell) > sz.cx)
		//	rcCell.right = rcCell.left + sz.cx;
		//if(rectHeight(rcCell) > sz.cy)
		//	rcCell.bottom = rcCell.top + sz.cy;
	}
	mapImages[keyFlag] = rcCell;
	return true;
}

controlUC* controlUC::findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if( (uFlags & UCFIND_VISIBLE) != 0 && !isVisible() ) return NULL;
	if( (uFlags & UCFIND_ENABLED) != 0 && !isEnabled() ) return NULL;
	POINT *ppt = static_cast<LPPOINT>(pData);
	if( (uFlags & UCFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
	return Proc(this, pData);
}

bool controlUC::existsControl(controlUC *pControl)
{
	if(!pControl) return false;
	return (UINT_PTR)this == (UINT_PTR)pControl;
}

// 一些重要的属性
bool controlUC::isVisible() const
{
	return m_bVisible && m_bInternVisible;
}
void controlUC::setVisible(bool bVisible /*= true*/)
{
	if(m_bVisible == bVisible) return;
	bool isv = isVisible();
	m_bVisible = bVisible;
	if( m_bFocused ) m_bFocused = false;
	if(!bVisible && m_pManager && m_pManager->getFocus() == this)
		m_pManager->setFocus(nullptr);
	if( isVisible() != isv ) {
		//SEND_LOG(L"controlUC::setVisible this:%08X ", this);
		needParentUpdate();
	}
}
void controlUC::setInternVisible(bool bVisible /*= true*/)
{
	m_bInternVisible = bVisible;
	if(!bVisible && m_pManager && m_pManager->getFocus() == this)
		m_pManager->setFocus(nullptr);

}
bool controlUC::isEnabled() const
{
	return m_bEnabled;
}
void controlUC::setEnabled(bool bEnabled /*= true*/)
{
	if( m_bEnabled == bEnabled ) return;

	m_bEnabled = bEnabled;
	invalidate();
}

bool controlUC::isMouseEnabled() const
{
	return m_bMouseEnabled;
}

void controlUC::setMouseEnabled(bool bEnable /*= true*/)
{
	m_bMouseEnabled = bEnable;
}

bool controlUC::isFocused() const
{
	return m_bFocused;
}
void controlUC::setFocus(bool bFocus /*= true*/)
{
	if(m_bFocused == bFocus) return;
	m_bFocused = bFocus;
	invalidate();
}
bool controlUC::isFloat() const
{
	return m_bFloat;
}
void controlUC::setFloat(bool bFloat /*= true*/)
{
	if( m_bFloat == bFloat ) return;

	m_bFloat = bFloat;
	needParentUpdate();
}

void controlUC::setRelativePos(SIZE szMove,SIZE szZoom)
{
	m_bsRelativePos.bRelative = TRUE;
	m_bsRelativePos.nMoveXPercent = szMove.cx;
	m_bsRelativePos.nMoveYPercent = szMove.cy;
	m_bsRelativePos.nZoomXPercent = szZoom.cx;
	m_bsRelativePos.nZoomYPercent = szZoom.cy;
}
void controlUC::setRelativeParentSize(SIZE sz)
{
	m_bsRelativePos.szParent = sz;
}
BSRelativePosUC controlUC::getRelativePos() const
{
	return m_bsRelativePos;
}
bool controlUC::isRelativePos() const
{
	return m_bsRelativePos.bRelative;
}

RECT controlUC::getPosition() const
{
	return m_rcItem;
}

void controlUC::setPosition(RECT pos)
{
	if( pos.right < pos.left ) pos.right = pos.left;
	if( pos.bottom < pos.top ) pos.bottom = pos.top;

	RECT invalidateRc = m_rcItem;
	if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = pos;

	m_rcItem = pos;
	if(!m_pManager) return;

	if( m_bFloat ) {
		controlUC* pParent = getParent();
		if( pParent != NULL ) {
			RECT rcParentPos = pParent->getPosition();
			if( m_szXY.cx >= 0 ) m_szXY.cx = m_rcItem.left - rcParentPos.left;
			else m_szXY.cx = m_rcItem.right - rcParentPos.right;

			if( m_szXY.cy >= 0 ) m_szXY.cy = m_rcItem.top - rcParentPos.top;
			else m_szXY.cy = m_rcItem.bottom - rcParentPos.bottom;
		}
	}
	m_bUpdateNeeded = false;

	rectJoin(invalidateRc, m_rcItem);

	//if( m_rcItem.left < invalidateRc.left ) invalidateRc.left = m_rcItem.left;
	//if( m_rcItem.top < invalidateRc.top ) invalidateRc.top = m_rcItem.top;
	//if( m_rcItem.right > invalidateRc.right ) invalidateRc.right = m_rcItem.right;
	//if( m_rcItem.bottom > invalidateRc.bottom ) invalidateRc.bottom = m_rcItem.bottom;

	controlUC* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->getParent() )
	{
		rcTemp = invalidateRc;
		rcParent = pParent->getPosition();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}
	if(m_pManager) m_pManager->invalidate(invalidateRc);

}

bool controlUC::ptIn(const POINT &pt)
{
	RECT rc = getPosition();
	//m_pManager->clientToScreen(&rc);
	return ::PtInRect(&rc, pt) == TRUE;
}

void controlUC::invalidate()
{
	if( !isVisible() ) return;
	
	RECT invalidateRc = m_rcItem;

	controlUC* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->getParent() )
	{
		rcTemp = invalidateRc;
		rcParent = pParent->getPosition();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}

	if( m_pManager != NULL )
	{
		m_pManager->invalidate(invalidateRc);
		//SEND_LOG(L"controlUC::invalidate class:<%s> name:<%s> rc:<%s>", getClass(), getName().c_str(), gtc::format::rect(invalidateRc, true).c_str());
	}
}
bool controlUC::isUpdateNeeded() const
{
	return m_bUpdateNeeded;
}
void controlUC::needUpdate()
{
	if( !isVisible() ) return;
	m_bUpdateNeeded = true;
	invalidate();

	if( m_pManager != NULL ) m_pManager->needUpdate();
}
void controlUC::needParentUpdate()
{
	if( getParent() ) {
		getParent()->needUpdate();
		getParent()->invalidate();
	}
	else {
		needUpdate();
	}

	if( m_pManager != NULL ) m_pManager->needUpdate();
}


void controlUC::paint(HDC hdc, const RECT &rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

	// 绘制循序：背景颜色->背景图->状态图->文本->边框
	if( m_szxyBorderRound.cx > 0 || m_szxyBorderRound.cy > 0 ) {
		renderClip roundClip;
		renderClip::generateRoundClip(hdc, m_rcPaint,  m_rcItem, m_szxyBorderRound.cx, m_szxyBorderRound.cy, roundClip);
		paintBkColor(hdc);
		paintBkImage(hdc);
		paintStatusImage(hdc);
		paintText(hdc);
		paintBorder(hdc);
	}
	else {
		paintBkColor(hdc);
		paintBkImage(hdc);
		paintStatusImage(hdc);
		paintText(hdc);
		paintBorder(hdc);
	}
}

void controlUC::paintBkColor(HDC hdc)
{
	if(m_dwBackColor == 0) return;
	//HBRUSH hBrush = CreateSolidBrush(m_dwBackColor);
	//::FillRect(hdc, &m_rcItem, hBrush);
	//::DeleteObject(hBrush);
	Gdiplus::Graphics grap(hdc);
	gtc::drawer::fillRectangle(grap, m_dwBackColor, gtc::rect(m_rcItem));
}

void controlUC::paintBkImage(HDC hdc)
{
	//if (gtc::stringUtil::startsWith(m_wsName, L"__self__"))
	//{
	//	SEND_LOG(L"NOTE controlUC::paintBkImage name:<%s> class:<%s> ", getName().c_str(), getClass());
	//}
	RECT rc = {0};
	if (existsImage(UCIMAGE_STATE_BACKGROUND, &rc))
	{
		Gdiplus::Graphics grap(hdc);
		gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rc);
		//if (gtc::stringUtil::startsWith(m_wsName, L"__self__"))
		//{
		//	SIZE sz = m_pImage->getImageSize();
		//	int w = m_pImage->getWidth();
		//	if (w == 0)
		//	{
		//		w = m_pImage->getHeight();
		//		Gdiplus::Status st = ((Gdiplus::Image *)m_pImage)->GetLastStatus();
		//		w = 0;
		//	}
		//	SEND_LOG(L"NOTE controlUC::paintBkImage ok:%d name:<%s> class:<%s> sz:<%d,%d> image:%08X rc:<%s>"
		//		, bl, getName().c_str(), getClass(), sz.cx, sz.cy, m_pImage
		//		, gtc::format::rect(rc, true).c_str());
		//}
	}
	////if(!m_pImage )
	//if(m_wsBkImage.empty()) return;
	//if (gtc::image *pImage = m_pManager->findImage(m_wsBkImage.c_str()))
	//{
	//	// 绘制背景图
	//	Gdiplus::Graphics grap(hdc);
	//	gtc::drawer::drawImage(grap, *pImage, gtc::rect(m_rcItem));
	//}
		

}
void controlUC::paintStatusImage(HDC hdc)
{

}
void controlUC::paintText(HDC hdc)
{
	//Gdiplus::Graphics grap(hdc);
	//m_pManager->setDefaultFontColor(0xFFFFFFFF);
	//gtc::drawer::drawString(grap, L"aslfas;fjafweiohweew", gtc::rect(m_rcItem), m_pManager->getDefaultFontInfo()->hFont);
	
}
void controlUC::paintBorder(HDC hdc)
{
	if(m_iBorderSize <= 0) return;
	// 绘制边框
}
void controlUC::Event(BSEventUC &event)
{
	switch(event.Type)
	{
	case BEUCEventSetCursor:
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
		break;
	case BEUCEventSetFocus:
		m_bFocused = true;
		invalidate();
		return;
		break;
	case BEUCEventKillFocus:
		m_bFocused = false;
		invalidate();
		return;
		break;
	case BEUCEventTimer:
		m_pManager->sendNotify(this, L"timer", event.wParam, event.lParam);
		return;
		break;
	case BEUCEventContextMenu:
		{
			if (isContextMenuUsed())
			{
				m_pManager->sendNotify(this, L"menu", event.wParam, event.lParam);
				return;
			}
		}
	default:
		break;
	}
	// 
	if(m_pParent) m_pParent->Event(event);
}

const std::wstring& controlUC::getUserData()
{
	return m_wsUserData;
}
void controlUC::setUserData(PCWSTR pstrText)
{
	m_wsUserData = pstrText ? pstrText : L"";
}
UINT_PTR controlUC::getTag() const
{
	return m_pTag;
}
void controlUC::setTag(UINT_PTR pTag)
{
	m_pTag = pTag;
}

bool controlUC::isContextMenuUsed() const
{
	return m_bMenuUsed;
}
void controlUC::setContextMenuUsed(bool bMenuUsed)
{
	m_bMenuUsed = bMenuUsed;
}

std::wstring controlUC::getToolTip() const
{
	return m_wsToolTip;
}

void controlUC::setToolTip(PCWSTR pstrText)
{
	m_wsToolTip = pstrText;
}


wchar_t controlUC::getShortcut() const
{
	return m_chShortcut;
}

void controlUC::setShortcut(wchar_t ch)
{
	m_chShortcut = ch;
}
//
//bool controlUC::drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const RECT &rcDest, RECT *prcSrc /*= nullptr*/)
//{
//	assert(pImage);
//	int imgW = pImage->getWidth();
//	int imgH = pImage->getHeight();
//	RECT rcSrc = {0,0,imgW,imgH};
//	if(prcSrc) rcSrc = *prcSrc;
//	if(rcSrc.left < 0) rcSrc.left = 0;
//	if(rcSrc.top < 0) rcSrc.top = 0;	
//	if(rcSrc.right > imgW) rcSrc.right = imgW;
//	if(rcSrc.bottom > imgH) rcSrc.bottom = imgH;
//	return grap.DrawImage(*pImage, toGdiplus(rcDest), rcSrc.left, rcSrc.top, rectWidth(rcSrc), rectHeight(rcSrc), Gdiplus::UnitPixel ) == Gdiplus::Ok;
//}


}

