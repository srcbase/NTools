#ifndef __WUC_WUCGIF__
#define __WUC_WUCGIF__
#pragma once
#include "wucBase.h"

namespace wuc {
//////////////////////////////////////////////////////////////////////////////////////////
// 2016-3-4		kp007	图像信息结构
struct BSImageInfo
{

	HBITMAP hBitmap;
	int		nX;
	int		nY;
	int				delay;
	bool			alphaChannel;
	std::wstring	wsResType;
	DWORD			dwMask;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	gifHandler	类 2016-3-4	kp007
//	保留gif数据
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class gifHandler
{
public:
	gifHandler();

	virtual ~gifHandler();

	int getFrameCount();

	void addFrameInfo(BSImageInfo* pFrameInfo);

	BSImageInfo* getNextFrameInfo();

	BSImageInfo* getCurrentFrameInfo();

	BSImageInfo* getFrameInfoAt(int index);
protected:
	void freeImage();
public:
	static gifHandler* load(PCWSTR pfile, DWORD mask = 0);
private:
	std::vector<BSImageInfo *> m_vecImageInfo;
	int		m_iCurrentFrame;
	int		m_iFrameCount;
	bool	m_isDeleting;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	buttonGifUC	类 2016-3-4	kp007
//	gif动画按钮
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class buttonGifUC : public buttonUC
{
public:
	buttonGifUC();
	~buttonGifUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void Event(BSEventUC& event);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	virtual void paintStatusImage(HDC hdc);
	virtual void setVisible(bool bVisible = true);

public:
	void setGifFile(PCWSTR pFile);

private:
	enum
	{
		GIF_TIMER_ID = 15
	};
private:
	bool		m_isUpdateTime;
	gifHandler	*m_hGif;
	int			m_iPreUpdateDelay;
};

}	// wuc
#endif	// __WUC_WUCGIF__