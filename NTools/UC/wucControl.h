#pragma once
#include "wucdef.h"
#include "drawManager.h"
namespace wuc{

typedef controlUC* (CALLBACK* FINDCONTROLPROC)(controlUC*, LPVOID);

typedef std::map<UINT, RECT>	ImageCellType;	//	key:UCIMAGE_STATE_xxx value:图片中的部分区域位置坐标

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	controlUC 类	2015-1-23
//	控件基类
//	图片显示方式：不同状态图片合成一张并存在m_pImage中，各个显示部分存在m_mapImage中
//				这样一来九格的定位就需要一个了m_rcPatch
////////////////////////////////////////////////////////////////////////////////////////////////////////
class controlUC
{
public:
	controlUC(void);
	virtual ~controlUC(void);
public:
	virtual const std::wstring& getName() const;
	virtual void setName(PCWSTR pstrName);
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;
	virtual void Event(BSEventUC &event);

	virtual void paint(HDC hdc, const RECT &rcPaint);
	virtual void paintBkColor(HDC hdc);
	virtual void paintBkImage(HDC hdc);
	virtual void paintStatusImage(HDC hdc);
	virtual void paintText(HDC hdc);
	virtual void paintBorder(HDC hdc);

	virtual bool activate();
	virtual drawManager* getManager();
	virtual void onInit();
	virtual void setManager(drawManager *pManager, controlUC *pParent, bool bInit = true);
	virtual controlUC* getParent() const;
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	controlUC* applyAttributeList(PCWSTR pstrList);
	virtual controlUC* findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual bool existsControl(controlUC *pControl);
	// 文本相关
	virtual PCWSTR getText() const;
	virtual void setText(PCWSTR pstrText);
	// 
	DWORD getBkColor() const;
	void setBkColor(DWORD dwBackColor);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-23	
	//void setImagePatch(SIZE szPatch);
	//void setImagePatch(int cx, int cy);
	//SIZE getImagePatch() const;
	void setImage(PCWSTR pImage, bool bRefImg = true);		// 图像绘制区间采用闭区间[from，to]
	void setImage(gtc::image *pImage, bool bRefImg = true);
	void clearImage();
	gtc::image* getImage() const;
	bool getBkImage(RECT &rc) const;
	void setBkImage(RECT rcImgCell);			// rcImgCell：处于getImage中某一部分
	void setBkImage(LONG left, LONG top, LONG right, LONG bottom);
	bool existsImage(UINT keyFlag, RECT *prcCell = NULL) const;	// 查看指定key的图片是否存在
	DWORD getBorderColor() const;
	void setBorderColor(DWORD dwBorderColor);
	DWORD getFocusBorderColor() const;
	void setFocusBorderColor(DWORD dwBorderColor);
	int getBorderSize() const;
	void setBorderSize(int nSize);
	SIZE getBorderRound() const;
	void setBorderRound(SIZE cxyRound);
	int getFont() const;
	void setFont(int fontIndex);
	// 
	virtual SIZE estimateSize(SIZE szAvailable);
	virtual SIZE autoCalculateSize();
	virtual RECT getPadding() const;
	virtual void setPadding(RECT rcPadding);	// 设置外边距，由上层窗口绘制
	virtual void setPadding(LONG left, LONG top, LONG right, LONG bottom);	// 设置外边距，由上层窗口绘制
	virtual SIZE getFixedXY() const;			// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void setFixedXY(SIZE szXY);			// 仅float为true时有效
	virtual void setFixedXY(int fixedX, int fixedY);
	virtual void setFixedSize(SIZE sz);
	virtual void setFixedSize(LONG cx, LONG cy);
	virtual int getFixedWidth() const;			// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void setFixedWidth(int cx);			// 预设的参考值
	virtual int getFixedHeight() const;			// 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void setFixedHeight(int cy);		// 预设的参考值
	virtual int getMinWidth() const;
	virtual void setMinWidth(int cx);
	virtual int getMaxWidth() const;
	virtual void setMaxWidth(int cx);
	virtual int getMinHeight() const;
	virtual void setMinHeight(int cy);
	virtual int getMaxHeight() const;
	virtual void setMaxHeight(int cy);
	// 一些重要的属性
	virtual bool isVisible() const;
	virtual void setVisible(bool bVisible = true);
	virtual void setInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
	virtual bool isEnabled() const;
	virtual void setEnabled(bool bEnable = true);
	virtual bool isMouseEnabled() const;
	virtual void setMouseEnabled(bool bEnable = true);
	virtual bool isFocused() const;
	virtual void setFocus(bool bFocus = true);
	virtual bool isFloat() const;
	virtual void setFloat(bool bFloat = true);
	virtual void setRelativePos(SIZE szMove,SIZE szZoom);
	virtual void setRelativeParentSize(SIZE sz);
	virtual BSRelativePosUC getRelativePos() const;
	virtual bool isRelativePos() const;

	virtual void invalidate();
	bool isUpdateNeeded() const;
	void needUpdate();
	void needParentUpdate();
	// 客户坐标
	RECT getPosition() const;
	virtual void setPosition(RECT pos);
	/*****************************************************************************************
		<< ---  controlUC::ptIn		2014-06-24 --- >>
		说明：查看点是否在控件上
		参数：
		pt = 参考点（客户坐标）
		返回值：是否在
	*****************************************************************************************/
	bool ptIn(const POINT &pt);

	// 用户属性
	virtual const std::wstring& getUserData(); // 辅助函数，供用户使用
	virtual void setUserData(PCWSTR pstrText); // 辅助函数，供用户使用
	virtual UINT_PTR getTag() const; // 辅助函数，供用户使用
	virtual void setTag(UINT_PTR pTag); // 辅助函数，供用户使用

	// 菜单
	virtual bool isContextMenuUsed() const;
	virtual void setContextMenuUsed(bool bMenuUsed);
	// 鼠标提示
	virtual std::wstring getToolTip() const;
	virtual void setToolTip(PCWSTR pstrText);
	// 快捷键
	virtual wchar_t getShortcut() const;
	virtual void setShortcut(wchar_t ch);
public:
	// 解析图像
	static gtc::image* analyzeImage(PCWSTR pstrImage);
	static bool existsImage(const ImageCellType &mapImages, UINT keyFlag, RECT *prcCell = NULL);	// 查看指定key的图片是否存在
	static bool setCellImage(ImageCellType &mapImages, UINT keyFlag, RECT rcCell, gtc::image *pImage = NULL);

protected:
	// 设定图像块在m_pImage中的位置，keyFlag=图块名称
	bool setCellImage(UINT keyFlag, RECT rcCell);
	///*****************************************************************************************
	//	<< ---  controlUC::drawImage		2014-07-31 --- >>
	//	说明：将图片某一区域绘制到目标位置
	//	参数：
	//	grap	= 绘图环境
	//	pImage	= 图像
	//	rcDest	= 目标位置
	//	prcSrc	= 图像源，当不为空时指图像的某一区域，但不能超过图像大小
	//	返回值：执行是否成功
	//*****************************************************************************************/
	//static bool drawImage(Gdiplus::Graphics &grap, gtc::image *pImage, const RECT &rcDest, RECT *prcSrc = nullptr);
protected:
	ImageCellType	m_mapImage;			// key=BEImageState value=RECT，在m_pImage中的位置 // 好像有字节对齐问题，影响了迭代器使用，所以放在顶端
	drawManager		*m_pManager;		// 控件所属窗口
	std::wstring	m_wsName;			// 控件名称，同一窗体内的控件名称必须唯一
	controlUC		*m_pParent;			// 控件的父控件

	RECT			m_rcItem;			// 控件在m_pWnd内的逻辑坐标
	RECT			m_rcPadding;		// 外边距，由上层窗口绘制
	SIZE			m_szXY;				// 控件相对父窗口位置 >= 0相对左边、上边距离，< 0相对右边、下边距离
	SIZE			m_szxyFixed;		// 控件预设的高度、宽度
	SIZE			m_szxyMin;			// 控件最小尺寸
	SIZE			m_szxyMax;			// 控件最大尺寸

	std::wstring	m_wsText;			// 控件内容
	DWORD			m_dwBackColor;		// 
	//std::wstring	m_wsBkImage;		// 背景图key名称
	
	//SIZE			m_szPatch;			// 采用九宫格绘制时四角尺寸，或两端绘制时端宽cx或端高cy
	gtc::image		*m_pImage;			// 图片，绘制区间采用闭区间[from，to]
	bool			m_brefImage;		// m_pImage是否引用资源集中的对象，否：自己需要释放；gdi+会出现ObjectBusy状态！唉
	int				m_iFont;			// 字体索引，基于0，-1时默认字体

	DWORD			m_dwBorderColor;
	DWORD			m_dwFocusBorderColor;
	bool			m_bVisible;			// 
	bool			m_bInternVisible;	// 我的理解是有些容器控件中的部分控件是否可见
	bool			m_bFocused;			// 
	bool			m_bEnabled;			// 
	bool			m_bMouseEnabled;
	bool			m_bUpdateNeeded;	// 是否需要重新绘制
	bool			m_bMenuUsed;		// 支持弹出式菜单		
	bool			m_bFloat;
	BSRelativePosUC m_bsRelativePos;

	std::wstring	m_wsUserData;
	UINT_PTR		m_pTag;
	std::wstring	m_wsToolTip;
	wchar_t			m_chShortcut;

	int				m_iBorderSize;		// 边框大小，如(1)
	SIZE			m_szxyBorderRound;	// 边框圆角半径,如(2,2)

	RECT			m_rcPaint;
};

}