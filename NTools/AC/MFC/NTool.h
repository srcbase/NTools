#ifndef __NTOOL_H__
#define __NTOOL_H__
#pragma once

#include <vector>
#include <memory>

#include <afxwin.h>
#include <afxole.h>

#include "AC/items/items.h"
#include "AC/items/itobjAdmin.h"
// Hints for UpdateAllViews/OnUpdate
#define HINT_UPDATE_WINDOW      0
#define HINT_UPDATE_DRAWOBJ     1
#define HINT_UPDATE_SELECTION   2
#define HINT_DELETE_SELECTION   3
#define HINT_UPDATE_OLE_ITEMS   4
// 自定义事件
/*****************************************************************************************
	<< --- 自定义事件：WM_ME_ITEM_ACTION	2013-12-14 --- >>
	说明：当某个绘图对象发生动作时触发
	参数：
	WPARAM	= 动作类别BSActionSender*
	LPARAM	= 被选中的对象集合指针(itobj**)，接收者负责释放。
	接收者：
	CMainFrame		= 更新浮动窗口内容，如属性窗口，对象树窗口
	CNPainterView	= 刷新绘图对象
	返回值：
*****************************************************************************************/
#define WM_ME_ITEM_ACTION				WM_USER+100	
/*****************************************************************************************
	<< --- 自定义事件：WM_ME_ITEM_ACTIONS	2013-12-14 --- >>
	说明：当某个绘图对象发生动作时触发
	参数：
	WPARAM	= 动作类别BSActionSender*
	LPARAM	= 被选中的对象集合指针(itobj::PtrArrayType*)，接收者负责释放。
	接收者：
	CMainFrame		= 更新浮动窗口内容，如属性窗口，对象树窗口
	CNPainterView	= 刷新绘图对象
	返回值：
*****************************************************************************************/
#define WM_ME_ITEM_ACTIONS				WM_USER+101	
/*****************************************************************************************
	<< --- 自定义事件：WM_ME_ITEM_PROPERTY_CHANGED 	2013-12-23 --- >>
	说明：一般是在对象的属性发生改变时，有属性所属的对象发送给属性编辑窗口
	参数：
	WPARAM	= 发生改变属性的对象
	LPARAM	= 对象属性集合指针(itobjProp::PtrArrayType*)，接收者负责释放。
	接收者：
	CMainFrame		= 更新浮动窗口内容，如属性窗口，对象树窗口
	返回值：
*****************************************************************************************/
#define WM_ME_ITEM_PROPERTY_CHANGED		WM_USER+102

/*****************************************************************************************
	<< --- 自定义事件：WM_ME_ITEM_REDRAW 	2014-03-15 --- >>
	说明：重新绘制对象
	参数：
	WPARAM	= 被绘制的对象(itobj*)
	LPARAM	= 绘制的区域rect*（客户坐标0.1mm），接收者负责释放。
	接收者：
	CNPainterView	= 刷新绘图对象
	返回值：
*****************************************************************************************/
#define WM_ME_ITEM_REDRAW				WM_USER+103

namespace acm {
///////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作对象
typedef enum BEToolShape
{
	BEToolShapeSelection = 0,	// 移动，改变大小等操作
	BEToolShapeLine,			// 线
	BEToolShapeRect,			// 矩形
	BEToolShapeRoundRect,		// 圆角矩形
	BEToolShapeEllipse,			// 椭圆
	BEToolShapePoly,			// 多边形
	BEToolShapeGrid				// 表格
}BEToolShape;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNPainterDoc
//	说明：该类作为自定义绘图文档的公共基类，内部定义了公共方法
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNPainterDoc : public COleDocument
{
protected: // 仅从序列化创建
	CNPainterDoc();
	DECLARE_DYNCREATE(CNPainterDoc)
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	afx_msg void OnFileSave();
// 实现
public:
	virtual ~CNPainterDoc();
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	// 公共方法
	/*****************************************************************************************
		<< --- CNPainterDoc::computePageSize	2013-12-04 --- >>
		说明：根据系统设定的打印机得到itobjPage信息
		参数：
		返回值：
	*****************************************************************************************/
	void computePageSize();
	/*****************************************************************************************
		<< --- CNPainterDoc::getAdminer	2013-12-02 --- >>
		说明：返回对象集合的引用
		参数：
		返回值：返回对象集合的引用 
	*****************************************************************************************/
	gtc::itobjAdmin& getAdminer() {			return m_objAdmin;					}
public:
	// 接口定义
	virtual void draw(CDC *pdc) 
	{
		DEBUG_LOG(L"---> CNPainterDoc::draw ");
	}

	/*****************************************************************************************
		<< --- CNPainterDoc::remove	2013-12-02 --- >>
		说明：移除指定对象
		参数：
		pobj	= 被移除的对象
		返回值：
	*****************************************************************************************/
	virtual void remove(gtc::itobj* pobj)
	{
		BSActionSender *psender = new BSActionSender(BESenderFlagDoc, BEItemActionDelete);
		AfxGetMainWnd()->SendMessage(WM_ME_ITEM_ACTION, (WPARAM)psender, (LPARAM)pobj);
		m_objAdmin.remove(pobj);
	}

	/*****************************************************************************************
		<< --- CNPainterDoc::add	2013-12-02 --- >>
		说明：追加一个绘图对象
		参数：
		pobj	= 被添加的绘图对象
		返回值：
	*****************************************************************************************/
	virtual void add(gtc::itobj* pobj)
	{
		m_objAdmin.add(pobj);
		BSActionSender *psender = new BSActionSender(BESenderFlagDoc, BEItemActionAdd);
		AfxGetMainWnd()->SendMessage(WM_ME_ITEM_ACTION, (WPARAM)psender, (LPARAM)pobj);
	}

	/*****************************************************************************************
		<< --- CNPainterDoc::objectAt	2013-12-02 --- >>
		说明：返回点point处的第一个对象（依赖对象添加的先后顺序）
		参数：
		point	= 参考点
		返回值：有就返回对象共享指针，否则返回空指针
	*****************************************************************************************/
	virtual gtc::itobj* objectAt(const CPoint &point)
	{
		return m_objAdmin.objectAt(point);
	}
protected:
	gtc::itobjAdmin	m_objAdmin;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNPainterView
//	说明：该类作为自定义绘图视图的公共基类，内部定义了公共方法
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNPainterView : public CScrollView
{
public:
	CNPainterView();
public:
	CNPainterDoc* GetDocument()
		{ return (CNPainterDoc*)m_pDocument; }
public:
	///*****************************************************************************************
	//	<< --- CNPainterView::drawBackground	2013-12-02 --- >>
	//	说明：绘制工作区和纸张
	//	参数：
	//	pdc	= CDC*
	//	返回值：
	//*****************************************************************************************/
	//void drawBackground(CDC *pdc);
	/*****************************************************************************************
		<< --- CNPainterView::resetScrollSizes	2013-12-02 --- >>
		说明：根据窗口客户区（设备坐标：像素）设定滚轴区间，使纸保持在窗口客户区中心
		参数：
		clientArea	= 窗口客户区，由GetClientRect或OnSize得到（设备坐标）
		返回值：
	*****************************************************************************************/
	void resetScrollSizes(CSize clientArea = CSize(0,0));
	/*****************************************************************************************
		<< --- CNPainterView::remove	2013-11-30 --- >>
		说明：将指定对象从选中集合中移出
		参数：
		pobj	= 被移出对象
		返回值：
	*****************************************************************************************/
	void remove(gtc::itobj* pobj)
	{
		gtc::itobj::PtrArrayType::iterator iter, end = m_vecSels.end();
		iter = std::find(m_vecSels.begin(), end, pobj);
		if(iter != end)
		{
			//(*iter)->remove();不需要释放
			m_vecSels.erase(iter);
		}
	}
	/*****************************************************************************************
		<< --- CNPainterView::moveHandleTo	2013-11-30 --- >>
		说明：选中指定区域内的对象
		参数：
		rect	= 区域
		bAdd	= true时，往选中对象集中追加对象，否则先清空选中记录集
		返回值：
	*****************************************************************************************/
	void selectWithinRect(CRect rect, bool bAdd = false);
	///*****************************************************************************************
	//	<< --- CNPainterView::moveHandleTo	2013-11-30 --- >>
	//	说明：通过指定定位块改变对象尺寸
	//	参数：
	//	nHandle	= 被操作的定位块索引
	//	point	= 定位块新坐标
	//	pobj	= 被移动的对象
	//	返回值：
	//*****************************************************************************************/
	//void moveHandleTo(int nHandle, CPoint point, gtc::itobj* pobj);
	///*****************************************************************************************
	//	<< --- CNPainterView::moveTo	2013-11-30 --- >>
	//	说明：将对象pobj移到新位置
	//	参数：
	//	position	= 新位置
	//	pobj		= 被移动的对象
	//	返回值：
	//*****************************************************************************************/
	//void moveTo(const CRect &position, gtc::itobj* pobj);
	///*****************************************************************************************
	//	<< --- CNPainterView::getHandleCursor	2013-11-30 --- >>
	//	说明：根据定位块位置得到其对应的光标
	//	参数：
	//	nHandle	= 定位块索引
	//	pobj	= 所属对象；可根据不同对象得到不同光标
	//	返回值：光标
	//*****************************************************************************************/
	//HCURSOR getHandleCursor(int nHandle, gtc::itobj* pobj);
	/*****************************************************************************************
		<< --- CNPainterView::editProperties	2013-11-30 --- >>
		说明：通过属性对话框编辑pobj的属性
		参数：
		pobj	= 被编辑对象
		返回值：
	*****************************************************************************************/
	void editProperties(gtc::itobj* pobj);
	/*****************************************************************************************
		<< --- CNPainterView::cloneSelection	2013-11-30 --- >>
		说明：克隆选中集合列表中的对象，并追加到文档中
		参数：
		返回值：
	*****************************************************************************************/
	void cloneSelection()
	{
		acm::CNPainterDoc *pdoc = GetDocument();
		if(pdoc == nullptr)
			return;
		for(gtc::itobj::PtrArrayType::size_type idx = 0; idx < m_vecSels.size(); ++idx)
			pdoc->add(m_vecSels[idx]->clone());
	}

	/*****************************************************************************************
		<< --- CNPainterView::invalidate	2013-11-30 --- >>
		说明：刷新对象pobj绘制区域
		参数：
		pobj	= 被刷新对象
		返回值：
	*****************************************************************************************/
	void invalidate(gtc::itobj* pobj)
	{
		assert(pobj);
		CRect rect = pobj->getPosition();
		docToClient(rect);
		if(m_bActive && isSelected(pobj))
		{
			int space = gtc::itobj::mc_iTrackerSize/2;
			rect.left -= space;
			rect.top -= space+1;
			rect.right += space+1;
			rect.bottom += space;
		}
		rect.InflateRect(1,1);
		InvalidateRect(rect, FALSE);

	}
	//////////////////////////////////////////////////////////////////////////
	//	2015-1-31	rc采用文档坐标
	void invalidate(RECT rc) {
		docToClient(rc);
		::InflateRect(&rc, 4, 4);
		InvalidateRect(&rc, FALSE);
	}
	//////////////////////////////////////////////////////////////////////////
	//	2015-1-31	rc采用文档坐标
	void invalidate(gtc::rect rc) {
		if(rc.width <= 0) rc.width = 1;
		if(rc.height <= 0) rc.height = 1;
		RECT rcc = rc;
		invalidate(rcc);
	}
	// 全部刷新
	void invalidate()
	{
		Invalidate(FALSE);
	}
	void deselect(gtc::itobj* pobj)
	{
		gtc::itobj::PtrArrayType::const_iterator iter, end = m_vecSels.cend();
		iter = std::find(m_vecSels.cbegin(), end, pobj);
		if(iter != end)
		{
			invalidate(*iter);
			m_vecSels.erase(iter);
		}
		/*POSITION pos = m_selection.Find(pObj);
		if (pos != NULL)
		{
			InvalObj(pObj);
			m_selection.RemoveAt(pos);
		}*/
	}
	/*****************************************************************************************
		<< --- CNPainterView::select	2013-11-30 --- >>
		说明：将对象pobj添加到已选中对象列表中
		参数：
		pobj	= 被添加的对象
		isadd	= true追加到列表中，否则先清空原列表后再添加
		返回值：true存在，否则不存在
	*****************************************************************************************/
	void select(gtc::itobj* pobj, bool isadd = false)
	{
		//if (!bAdd)
		//{
		//	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
		//	m_selection.RemoveAll();
		//}

		//if (pObj == NULL || IsSelected(pObj))
		//	return;

		//m_selection.AddTail(pObj);
		//InvalObj(pObj);
		if(!isadd)
		{
			OnUpdate(nullptr, HINT_UPDATE_SELECTION, nullptr);
			m_vecSels.clear();
		}
		if (pobj == nullptr || isSelected(pobj))
		{
			return;
		}
		m_vecSels.push_back(pobj);
		invalidate(pobj);
		// 
		
	}

	/*****************************************************************************************
		<< --- CNPainterView::isSelected	2013-11-30 --- >>
		说明：查验对象pobj是否在已选中对象列表中
		参数：
		pobj	= 被查验对象
		返回值：true存在，否则不存在
	*****************************************************************************************/
	bool isSelected(const gtc::itobj* pobj) const
	{
		//CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
		//if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
		//	pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
		//return m_selection.Find(pDrawObj) != NULL;
		gtc::itobj::PtrArrayType::const_iterator end = m_vecSels.cend();
		return std::find(m_vecSels.cbegin(), end, pobj)!=end;
	}

	/*****************************************************************************************
		<< --- CNPainterView::clientToDoc	2013-11-29 --- >>
		说明：设备坐标到逻辑坐标的转换
		参数：
		point	= 被转换的点
		返回值：
	*****************************************************************************************/
	void clientToDoc(CPoint& point)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, NULL);
		dc.DPtoLP(&point);
	}
	/*****************************************************************************************
		<< --- CNPainterView::clientToDoc	2013-11-29 --- >>
		说明：设备坐标到逻辑坐标的转换
		参数：
		rect	= 被转换的区域
		返回值：
	*****************************************************************************************/
	void clientToDoc(CRect& rect)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, NULL);
		dc.DPtoLP(rect);
		//ASSERT(rect.left <= rect.right);
		//ASSERT(rect.bottom <= rect.top);
	}
	/*****************************************************************************************
		<< --- CNPainterView::docToClient	2013-11-29 --- >>
		说明：逻辑坐标到设备坐标的转换
		参数：
		point	= 被转换的点
		返回值：
	*****************************************************************************************/
	void docToClient(CPoint& point)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, NULL);
		dc.LPtoDP(&point);
	}
	/*****************************************************************************************
		<< --- CNPainterView::docToClient	2013-11-29 --- >>
		说明：逻辑坐标到设备坐标的转换
		参数：
		point	= 被转换的区域
		返回值：
	*****************************************************************************************/
	void docToClient(RECT& rc)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, NULL);
		dc.LPtoDP(&rc);
		
		//rc.NormalizeRect();
	}
	void docToClient(gtc::rect &rc)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, NULL);
		POINT pt = {rc.x, rc.y};
		dc.LPtoDP(&pt);
		SIZE sz = {rc.width, rc.height};
		dc.LPtoDP(&sz);
		rc.setRect(pt.x, pt.y, sz.cx, sz.cy);
	}
public:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnDraw(CDC* /*pDC*/);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	/*****************************************************************************************
	<< --- 自定义事件：WM_ME_ITEM_PROPERTY_CHANGED 	2013-12-23 --- >>
	说明：一般是在对象的属性发生改变时，有属性所属的对象发送给属性编辑窗口
	参数：
	WPARAM	= 发生改变属性的对象
	LPARAM	= 对象属性集合指针(itobjProp::PtrArrayType*)，接收者负责释放。
	接收者：
	CMainFrame		= 更新浮动窗口内容，如属性窗口，对象树窗口
	返回值：
*****************************************************************************************/
	LRESULT OnMyItemPropertyChanged(WPARAM wParam, LPARAM lParam);
	/*****************************************************************************************
		<< --- 自定义事件：WM_ME_ITEM_ACTION	2013-12-14 --- >>
		说明：刷新绘图对象
		参数：
		WPARAM	= 动作类别BEItemAction
		LPARAM	= 被选中的对象集合指针(itobj::PtrArrayType*)，接收者负责释放。
		返回值：
	*****************************************************************************************/
	LRESULT OnMyItemAction(WPARAM wParam, LPARAM lParam);
	/*****************************************************************************************
		<< --- 自定义事件：WM_ME_ITEM_REDRAW 	2014-03-15 --- >>
		说明：重新绘制对象
		参数：
		WPARAM	= 被绘制的对象(itobj*)
		LPARAM	= 绘制的区域rect*（客户坐标0.1mm），接收者负责释放。
		接收者：
		CNPainterView	= 刷新绘图对象
		返回值：
	*****************************************************************************************/
	LRESULT OnMyItemRedraw(WPARAM wParam, LPARAM lParam);

public:
	// 绘图对象的自定义剪贴板格式
	static CLIPFORMAT mc_cfBItemObjects;
public:
	CSize m_szWorker;		// 工作台尺寸（逻辑单位） > 纸张尺寸 > 窗口客户区
	//CSize m_bsWorker, m_bsPaper;
	bool m_bActive;	// 视图是否处于激活状态
	gtc::itobj::PtrArrayType	m_vecSels;	// 被选中的对象集合
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNTool
//	说明：依赖的公共操作
//	采用CAD制图动作
//		1、鼠标按下弹起后开始绘图操作
//		2、按esc键取消当前绘图操作
//		3、再次按下或space键结束本次绘图操作
//		4、按shift键时可多选对象
//		5、按ctrl键时复制选中对象
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNTool
{
protected:
	CNTool() {}
	CNTool(BEToolShape shape);
	virtual ~CNTool(void) {}
public:
// Overridables
	virtual void OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnKeyDown(CNPainterView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnEditProperties(CNPainterView* pView);
	virtual void OnCancel();
	/*****************************************************************************************
		<< --- CNTool::releaseTool	2013-12-11 --- >>
		说明：释放当前操作。一般发生在选中操作的OnLButtonUp，绘图操作的OnLButtonDown，以及space、esc键
		参数：
		返回值：
	*****************************************************************************************/
	virtual bool releaseTool() {
		if(mc_isDrawing)
		{
			mc_isDrawing = false;
			ReleaseCapture();
			mc_drawShape = BEToolShapeSelection;
			return true;
		}
		return false;
	}

public:
	// 查找指定操作类型的对象
	static CNTool* findTool(BEToolShape drawShape);
public:
// 数据
	BEToolShape	m_beDrawShape;

	static std::vector<CNTool*>	mc_vecTools;	// 每个对象都要“静态实例化”一个操作对象，并加入本数组中
	static CPoint		mc_down;
	static UINT			mc_nDownFlags;
	static CPoint		mc_last;
	static BEToolShape	mc_drawShape;
	static bool			mc_isDrawing;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNToolSelect
//	说明：对象选中后的相关操作；如：移位，改变大小...
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNToolSelect : public CNTool
{
// Constructors
public:
	CNToolSelect();

// Implementation
	virtual void OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnEditProperties(CNPainterView* pView);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNToolRect
//	说明：对象选中后的相关操作；如：移位，改变大小...
//	日期：2013-11-30
////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNToolRect : public CNTool
{
// Constructors
public:
	CNToolRect(BEToolShape drawShape);

// Implementation
	virtual void OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNToolGrid
//	说明：对象选中后的相关操作；如：移位，改变大小...
//	日期：2013-12-17
////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNToolGrid : public CNTool
{
// Constructors
public:
	CNToolGrid();

// Implementation
	virtual void OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point);
};

}
#endif