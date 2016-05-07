#pragma once
#include "wucdef.h"
#include "wucContainer.h"
namespace wuc {

///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-27	listUC控制信息及状态，背景绘制以背景图优先，当背景色=0时不用绘制背景色
//					本结构将影响listElementUC对象
#define UCLIST_MAX_COLUMNS	32		// 最大列数
typedef struct BSListItemInfo {
	int			iColumns;				// 当前列数
	RECT		rcColumn[UCLIST_MAX_COLUMNS];		// 
	int			iFont;					// 
	UINT		uTextStyle;				// 文本绘制样式*
	RECT		rcTextPadding;			// 元素留边
	DWORD		clrText;				// 常规显示时的文本色
	DWORD		clrBk;					//		的背景色
	bool		bAlternateBk;
	//BYTE		nAlpha;					//		
	DWORD		clrSelectedText;		// 被选中时的文本色
	DWORD		clrSelectedBk;			//		的背景色
	//BYTE		nSelectedAlpha;			// 
	DWORD		clrHotText;				// 处于悬停时的文本色
	DWORD		clrHotBk;				//		的背景色
	//BYTE		nHotAlpha;				// 
	DWORD		clrCutedText;			// 处于剪切时的文本色
	DWORD		clrCutedBk;				//		的背景色
	//BYTE		nCutedAlpha;			// 
	DWORD		clrDisabledText;		// 处于无效或禁用时的文本色
	DWORD		clrDisabledBk;			//		的背景色
	//BYTE		nDisabledAlpha;			// 
	DWORD		clrLine;				// 网格线的颜色，=0时不用绘制网格线
	bool		bShowHtml;				// 
	bool		bMultiExpandable;		// 
	gtc::image		*pImage;			// 图片
	ImageCellType	mapImage;			// key=BEImageState value=RECT，在m_pImage中的位置

}BSListItemInfo;
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-29	列表头的单元格信息
typedef struct BSHeaderItemInfo {
	UINT		uiTextStyle;
	bool		bShadowText;			// 绘制带有阴影的文本
	int			iFont;					// 
	RECT		rcTextPadding;			// 文本留边
	DWORD		clrText;				// 常规显示时的文本色
	DWORD		clrBk;					//		的背景色
	DWORD		clrHotText;				// 处于悬停时的文本色
	DWORD		clrHotBk;				//		的背景色
	DWORD		clrPushedText;			// 处于悬停时的文本色
	DWORD		clrPushedBk;			//		的背景色

	gtc::image		*pImage;			// 图片
	ImageCellType	mapImage;			// key=BEImageState value=RECT，在m_pImage中的位置
}BSHeaderItemInfo;

class listBodyUC;
class listHeaderUC;
class listUC;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IFolderViewCallbackUC	类	2015-1-27
//	节点数据回调函数结合类；listUC的实现者实现这个接口
////////////////////////////////////////////////////////////////////////////////////////////////////////
class IListCallbackUC
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-27	得到节点图标和名称等单元格信息pControl = listElementUC及其子类
	virtual bool getItemData(controlUC *pControl, int iItem, int iSubItem) = 0;
	//	给客户绘制
	virtual bool drawItem(HDC hdc, RECT rcNode, int iItem, int iSubItem, controlUC *pControl) = 0;
	//	得到子项索引；默认：从左到右，从上到下排序
	virtual int getSubItemIndex(int iItem, int iSubItem) = 0;
	virtual int getSubItemIndex(POINT pt) = 0;
	//	2015-2-12	得到本接口共同拥有的控件
	virtual controlUC* getSelf() = 0;
	//	2015-3-2  列表容器大小发生改变
	virtual void scrollRangeChanged(SIZE szOld, SIZE szNew) {}
	virtual void scrollPosChanged(SIZE szOld, SIZE szNew) {}
	virtual int getScrollLineStep()=0;// {	return 0;	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-27	listUC的父基类之一
class IListOwnerUC
{
public:
	virtual BSListItemInfo* getListInfo() = 0;
	virtual int getCurSel() const = 0;
	virtual bool selectItem(int iIndex) = 0;
	//virtual void doEvent(BSEventUC& event) = 0;
	virtual void Event(BSEventUC& event) = 0;
};
class IListUC : public IListOwnerUC
{
public:
	virtual listHeaderUC* getHeader() const = 0;
	virtual containerUC* getList() const = 0;
	virtual IListCallbackUC* getItemCallback() const = 0;
	virtual void setItemCallback(IListCallbackUC* pCallback) = 0;
	virtual bool expandItem(int iIndex, bool bExpand = true) = 0;
	virtual int getExpandedItem() const = 0;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-27	
class IListItemUC
{
public:
	virtual int getIndex() const = 0;
	virtual void setIndex(int iIndex) = 0;
	virtual IListOwnerUC* getOwner() = 0;
	virtual void setOwner(controlUC* pOwner) = 0;
	virtual bool isSelected() const = 0;
	virtual bool select(bool bSelect = true) = 0;
	virtual bool isCuted() const = 0;
	virtual bool cut(bool bCut = true) = 0;
	virtual bool isHot() const = 0;
	virtual bool hot(bool bHot = true) = 0;
	virtual bool isExpanded() const = 0;
	virtual bool expand(bool bExpand = true) = 0;
	virtual void drawItem(HDC hdc, const RECT& rcItem) = 0;
	virtual void drawItemBk(HDC hdc, const RECT& rcItem) = 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listElementUC	类	2015-1-27
//	代表的是一个行数据的伪集合，每个列数据在drawItem中体现出来
//	单个节点元素；本对象默认属性来至BSListItemInfo，特例属性来至listHeaderItemUC
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listElementUC : public controlUC, public IListItemUC
{
public:
	listElementUC(void);
	~listElementUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void Event(BSEventUC &event);
	virtual void paint(HDC hdc, const RECT &rcPaint);

	virtual void setVisible(bool bVisible = true);
	virtual void setEnabled(bool bEnable = true);
	virtual void invalidate();
	virtual bool activate();
	// 绘制行背景
	//virtual void drawBk(HDC hdc, const RECT& rcItem);
public:
	RECT getSubItemPosition(int iSubItem);
public:
	// IListItemUC
	virtual int getIndex() const;
	virtual void setIndex(int iIndex);
	virtual IListOwnerUC* getOwner();
	virtual void setOwner(controlUC* pOwner);
	virtual bool isSelected() const;
	virtual bool select(bool bSelect = true);
	virtual bool isCuted() const;
	virtual bool cut(bool bCut = true);
	virtual bool isHot() const;
	virtual bool hot(bool bHot = true);
	virtual bool isExpanded() const;
	virtual bool expand(bool bExpand = true);
	//virtual void drawItem(HDC hdc, const RECT& rcItem) = 0;
	virtual void drawItemBk(HDC hdc, const RECT& rcItem);
protected:
	int				m_iIndex;		// 可作为行号用
	bool			m_bSelected;
	UINT			m_uButtonState;
	IListOwnerUC	*m_pOwner;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listLabelElementUC  类		2015-8-7
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listLabelElementUC : public listElementUC
{
public:
	listLabelElementUC();
	~listLabelElementUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

	virtual void Event(BSEventUC &event);
	//virtual void paint(HDC hdc, const RECT &rcPaint);
	virtual SIZE estimateSize(SIZE szAvailable);
	// IListItemUC
	virtual void drawItem(HDC hdc, const RECT& rcItem);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listContainerElementUC  类		2015-7-30
//	带有子控件的列表元素基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listContainerElementUC : public containerUC, public IListItemUC
{
public:
	listContainerElementUC();
	~listContainerElementUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual void setVisible(bool bVisible = true);
	virtual void setEnabled(bool bEnable = true);
	virtual bool activate();
	virtual void invalidate();

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void Event(BSEventUC &event);
	virtual void paint(HDC hdc, const RECT &rcPaint);

public:
	// IListItemUC
	virtual int getIndex() const;
	virtual void setIndex(int iIndex);
	virtual IListOwnerUC* getOwner();
	virtual void setOwner(controlUC* pOwner);
	virtual bool isSelected() const;
	virtual bool select(bool bSelect = true);
	virtual bool isCuted() const;
	virtual bool cut(bool bCut = true);
	virtual bool isHot() const;
	virtual bool hot(bool bHot = true);
	virtual bool isExpanded() const;
	virtual bool expand(bool bExpand = true);
	virtual void drawItem(HDC hdc, const RECT& rcItem);
	virtual void drawItemBk(HDC hdc, const RECT& rcItem);



	//void DoPaint(HDC hDC, const RECT& rcPaint);

	//void DrawItemText(HDC hDC, const RECT& rcItem);    
	//void DrawItemBk(HDC hDC, const RECT& rcItem);

protected:
	int				m_iIndex;
	bool			m_bSelected;
	UINT			m_uiButtonState;
	IListOwnerUC	*m_pOwner;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listEmptyElementUC  类		2015-8-3
//	为了满足效果图设计了空行
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listEmptyElementUC : public listContainerElementUC
{
public:
	listEmptyElementUC();
	~listEmptyElementUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;
public:
	// IListItemUC
	virtual void drawItem(HDC hdc, const RECT& rcItem);
	virtual void drawItemBk(HDC hdc, const RECT& rcItem);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listUC类	2015-1-22
//	文件节点显示类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listUC : public horizontalLayoutUC, public IListUC
{
public:
	listUC(void);
	~listUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void setPosition(RECT pos);
	//virtual void Event(BSEventUC &event);
public:
	RECT getSubItemPosition(int iItem, int iSubItem);
public:
	// IListOwnerUC
	virtual BSListItemInfo* getListInfo();
	virtual int getCurSel() const;
	virtual bool selectItem(int iIndex);
	virtual void Event(BSEventUC& event);
	// IListUC
	virtual listHeaderUC* getHeader() const;
	virtual containerUC* getList() const;
	virtual IListCallbackUC* getItemCallback() const;
	virtual void setItemCallback(IListCallbackUC* pCallback);
	virtual bool expandItem(int iIndex, bool bExpand = true);
	virtual int getExpandedItem() const;
	// 
	bool getScrollSelect();
	void setScrollSelect(bool bScrollSelect);
	void setMultiExpanding(bool bMultiExpandable); 
	//virtual void doEvent(BSEventUC& event);
	// containerUC
	virtual int getChildPadding() const;
	virtual void setChildPadding(int iPadding);

	virtual controlUC* getItemAt(int iIndex) const;
	virtual int getItemIndex(controlUC* pControl) const ;
	virtual bool setItemIndex(controlUC* pControl, int iIndex);
	virtual int getCount() const;
	virtual bool add(controlUC* pControl);
	virtual bool addAt(controlUC* pControl, int iIndex);
	virtual bool remove(controlUC* pControl);
	virtual bool removeAt(int iIndex) ;
	virtual void removeAll();

	// 
	void ensureVisible(int iIndex);
	void scroll(int dx, int dy);
	// 
	virtual SIZE getScrollPos() const;
	virtual SIZE getScrollRange() const;
	virtual void setScrollPos(SIZE szPos);
	virtual void lineUp(int step = 0);
	virtual void lineDown(int step = 0);
	virtual void pageUp();
	virtual void pageDown();
	virtual void homeUp();
	virtual void endDown();
	virtual void lineLeft();
	virtual void lineRight();
	virtual void pageLeft();
	virtual void pageRight();
	virtual void homeLeft();
	virtual void endRight();
	//virtual void buildScrollBar(scrollBarUC *pScrollbarVertical, scrollBarUC *pScrollbarHorizontal = nullptr);
	virtual void enableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
	virtual scrollBarUC* getVerticalScrollBar() const;
	virtual scrollBarUC* getHorizontalScrollBar() const;

	// BSFolderViewInfo
	int getColumns() const;
	int getItemFont() const;
	void setItemFont(int iFont);
	DWORD getItemLineColor() const;
	void setItemLineColor(DWORD clrLine);
	bool isItemShowHtml() const;
	void setItemShowHtml(bool bShowHtml = true);
	bool isAlternateBk() const;
	void setAlternateBk(bool bAlternateBk);
	UINT getItemTextStyle() const;
	void setItemTextStyle(UINT style);
	RECT getItemTextPadding() const;
	void setItemTextPadding(RECT rcPadding);
	DWORD getItemTextColor() const;
	void setItemTextColor(DWORD clrText);
	DWORD getItemBkColor() const;
	void setItemBkColor(DWORD clrBk);
	DWORD getItemSelectedTextColor() const;
	void setItemSelectedTextColor(DWORD clrText);
	DWORD getItemSelectedBkColor() const;
	void setItemSelectedBkColor(DWORD clrBk);
	DWORD getItemHotTextColor() const;
	void setItemHotTextColor(DWORD clrText);
	DWORD getItemHotBkColor() const;
	void setItemHotBkColor(DWORD clrBk);
	DWORD getItemCutTextColor() const;
	void setItemCutTextColor(DWORD clrText);
	DWORD getItemCutBkColor() const;
	void setItemCutBkColor(DWORD clrBk);
	DWORD getItemDisabledTextColor() const;
	void setItemDisabledTextColor(DWORD clrText);
	DWORD getItemDisabledBkColor() const;
	void setItemDisabledBkColor(DWORD clrBk);

	gtc::image* getItemImage() const;	// 总图片
	void setItemImage(PCWSTR pwsImage);
	void setItemNormalImage(RECT rcImgCell);
	bool getItemNormalImage(RECT &rc) const;
	void setItemSelectedImage(RECT rcImgCell);
	bool getItemSelectedImage(RECT &rc) const;
	void setItemCutedImage(RECT rcImgCell);
	bool getItemCutedImage(RECT &rc) const;
	void setItemHotImage(RECT rcImgCell);
	bool getItemHotImage(RECT &rc) const;
	void setItemDisabledImage(RECT rcImgCell);
	bool getItemDisabledImage(RECT &rc) const;
protected:
	int 				m_iCurSel;			// 当前选中节点索引
	int					m_iExpandedItem;	// 
	bool				m_bScrollSelect;	// 滚动条是否被选中
	BSListItemInfo		m_bsInfo;			// 节点信息
	listBodyUC			*m_pList;			// 存放节点容器
	listHeaderUC		*m_pHeader;			// 列表头
	IListCallbackUC		*m_pCallback;		// 得到节点数据的回调接口
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listBodyUC 类	2015-1-27
//	存放节点的容器类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listBodyUC : public verticalLayoutUC
{
private:
	listBodyUC();
public:
	explicit listBodyUC(listUC *pOwner);
	~listBodyUC();

	virtual void Event(BSEventUC &event);
	virtual void setPosition(RECT pos);
	virtual void setScrollPos(SIZE szPos);

	listUC* getOwner() const;
protected:
	listUC	*m_pOwner;
};
inline listUC* listBodyUC::getOwner() const
{
	return m_pOwner;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listHeaderUC	类	2015-1-27
//	列表控件的头；本对象中的属性影响listHeaderItemUC单元格
//	可以不可见
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listHeaderUC : public horizontalLayoutUC
{
public:
	listHeaderUC();
	~listHeaderUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual SIZE estimateSize(SIZE szAvailable);
public:
	bool isDragable() const;
	void setDragable(bool bDragable);
	gtc::image* getSliderImage();		// 改变列框的滑块
	void setSliderImage(PCWSTR pImage);

	BSHeaderItemInfo* getHeaderInfo();

	void setItemTextStyle(UINT uStyle);
	UINT getItemTextStyle() const;
	void setItemTextColor(DWORD dwTextColor);
	DWORD getItemTextColor() const;
	void setItemBkColor(DWORD clrBk);
	DWORD getItemBkColor() const;
	void setItemFont(int index);
	int getItemFont() const;
	bool isItemShadowText() const;
	void setItemShadowText(bool bShadow = true);
	void setItemTextPadding(RECT rc);
	RECT getItemTextPadding() const;

	gtc::image* getItemImage() const;
	void setItemImage(PCWSTR pImage);
	bool getItemNormalImage(RECT &rc);
	void setItemNormalImage(RECT rcImgCell);
	bool getItemHotImage(RECT &rc);
	void setItemHotImage(RECT rcImgCell);
	bool getItemPushedImage(RECT &rc);
	void setItemPushedImage(RECT rcImgCell);
protected:
	BSHeaderItemInfo	m_bsInfo;
	//DWORD	m_clrText;
	////DWORD	m_clrBk;	// controlUC
	//UINT	m_uiTextStyle;
	//int		m_iFont;
	//bool	m_bShadowText;	// 绘制带有阴影的文本
	//RECT	m_rcTextPadding;

	bool	m_bDragable;
	gtc::image *m_pimgSlider;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listHeaderItemUC	类	2015-1-27
//	列表控件的头元素单元格；本对象将影响其对应列的listElementUC集合属性
////////////////////////////////////////////////////////////////////////////////////////////////////////
class listHeaderItemUC : public controlUC
{
public:
	listHeaderItemUC();
	~listHeaderItemUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual void Event(BSEventUC &event);
	virtual SIZE estimateSize(SIZE szAvailable);
	virtual void paintStatusImage(HDC hdc);
	virtual void paintText(HDC hdc);
public:
	listHeaderUC* getOwner() const;
	void setOwner(listHeaderUC *pHeader);
	int getSepWidth() const;
	void setSepWidth(int iWidth);

	RECT getThumbRect() const;
protected:
	listHeaderUC	*m_pOwner;

	POINT			m_ptLastMouse;
	UINT			m_uiButtonState;
	int				m_iSepWidth;		// 分隔符宽
};


}