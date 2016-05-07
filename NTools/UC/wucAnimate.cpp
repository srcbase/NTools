#include "wucAnimate.h"

namespace wuc {
animateFrameUC::animateFrameUC(void)
	: controlUC()
	, m_iFrame(0), m_bePlayState(BEPlayStateNone)
	, m_uiFrameElapse(40)

#if defined(GTC_BOOST_THREAD)
	, m_bReadLocked(false), m_bWriteLocked(false)
#endif

{
}


animateFrameUC::~animateFrameUC(void)
{
	//gtc::app::writeVSLogs(L"free animateFrameUC <%s>", m_wsName.c_str());
}

PCWSTR animateFrameUC::getClass() const
{
	return L"animateFrameUC";
}
LPVOID animateFrameUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"FrameAnimate") == 0 ) return static_cast<animateFrameUC*>(this);
	return controlUC::getInterface(pstrName);
}

gtc::image* animateFrameUC::getFrameImage() const
{
	return m_pImage;
}
void animateFrameUC::setFrameImage(PCWSTR pimgKeyName)
{
	//if(m_wsFrameImage == pimgKeyName) return;
	//m_wsFrameImage = pimgKeyName;
	//m_vecFrames.clear();
	bool bl = isPlaying();
	if (bl) playStop();

	__super::setImage(pimgKeyName);
	

	if (bl) playStart();
}
int animateFrameUC::getCurrentFrame() const
{
	return m_iFrame;
}
void animateFrameUC::setCurrentFrame(int frameIndex)
{
	if(m_iFrame == frameIndex) return;
	m_iFrame = frameIndex;
	if (isPlaying())
	{
		playStop();
		playStart();
	}
}

UINT animateFrameUC::getFrameElapse() const
{
	return m_uiFrameElapse;
}
void animateFrameUC::setFrameElapse(UINT elapse)
{
	if(m_uiFrameElapse == elapse) return;
	m_uiFrameElapse = elapse;
	if (isPlaying())
	{
		playStop();
		playStart();
	}
}
void animateFrameUC::Event(BSEventUC& event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else controlUC::Event(event);
		return;
	}
	switch(event.Type)
	{
	case BEUCEventTimer:
		{
			if(event.wParam == DEFAULT_FRAME_TIMERID)
			{
				//SEND_LOG(L"BEUCEventTimer ......");
				invalidate();
				return;
			}
		}
		break;
	}
	__super::Event(event);
}

SIZE animateFrameUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 )
	{
		SIZE sz = {m_szxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 8};
		return sz;
	}
	return controlUC::estimateSize(szAvailable);
}
void animateFrameUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"frameImage") == 0 ) setFrameImage(pstrValue);
	else if( _tcscmp(pstrName, L"frameElapse") == 0 ) setFrameElapse(_tcstol(pstrValue, &pstr, 10));
	else __super::setAttribute(pstrName, pstrValue);
}

void animateFrameUC::paintBkImage(HDC hdc)
{
	// 绘制起始、暂停、停止时的当前帧
}
void animateFrameUC::paintStatusImage(HDC hdc)
{
	assert(hdc && m_pManager);
	if(!m_pImage) return;
	if(m_vecFrames.size() <= 0)
	{
		WRITE_LOCK
		m_vecFrames = calculateAreaCell(m_pImage->getImageSize(), rectSize(m_rcItem));
	}
	Gdiplus::Graphics grap(hdc);
	READ_LOCK
	if(m_vecFrames.size() <= 0 || m_iFrame >= (int)m_vecFrames.size() || m_iFrame < 0) return;
	gtc::drawer::drawImage(grap, m_pImage, m_rcItem, m_vecFrames[m_iFrame]);
	m_iFrame = ++m_iFrame % m_vecFrames.size();

}
bool animateFrameUC::playStart()
{
	assert(m_pManager);
	if(m_bePlayState == BEPlayStatePlaying) return true;
	if(isPlaying()) playStop();

	bool bl = m_pManager->setTimer(this, DEFAULT_FRAME_TIMERID, m_uiFrameElapse);
	if(bl) m_bePlayState = BEPlayStatePlaying;

	return bl;
}
bool animateFrameUC::playFast()
{
	m_bePlayState = BEPlayStateFast;
	return true;
}
bool animateFrameUC::playSlow()
{
	m_bePlayState = BEPlayStateSlow;
	return true;
}
bool animateFrameUC::playPause()
{
	m_bePlayState = BEPlayStatePause;
	return true;
}
bool animateFrameUC::playStop()
{
	assert(m_pManager);
	if(!isPlaying()) return false;
	///
	{
		WRITE_LOCK
		m_vecFrames.clear();
	}
	m_iFrame = 0;
	
	bool bl = m_pManager->killTimer(this, DEFAULT_FRAME_TIMERID);
	if(bl) m_bePlayState = BEPlayStateStop;
	return bl;
}
BEPlayState animateFrameUC::getPlayState() const
{
	return m_bePlayState;
}
bool animateFrameUC::isPlaying() const
{
	return m_bePlayState == BEPlayStatePlaying || 
		m_bePlayState == BEPlayStateFast || 
		m_bePlayState == BEPlayStateSlow;
}
}