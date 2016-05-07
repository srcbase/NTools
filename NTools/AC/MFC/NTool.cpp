#include "NTool.h"
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类
#include <afxole.h>
//#include "d:/VSTOOLS/AC/ATools.h"
//#include "d:/VSTOOLS/AC/BDevice/BDevice.h"
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/serialization/version.hpp>
//#include <boost/serialization/split_member.hpp>
//#include <boost/serialization/export.hpp>
//
//BOOST_CLASS_EXPORT(gtc::CAEntryItem)
//BOOST_CLASS_EXPORT(gtc::CAEntryItemValue)
//BOOST_CLASS_EXPORT(gtc::itobj)
//BOOST_CLASS_EXPORT(gtc::itobjRect)
//BOOST_CLASS_EXPORT(gtc::itobjGrid)
//BOOST_CLASS_EXPORT(gtc::itobjPage)
//BOOST_CLASS_EXPORT(gtc::itobjPageAxis)

namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNPainterDoc
//	说明：该类作为自定义绘图文档的公共基类，内部定义了公共方法
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CNPainterDoc, COleDocument)

BEGIN_MESSAGE_MAP(CNPainterDoc, COleDocument)
	//ON_COMMAND(ID_FILE_SAVE, &CNPainterDoc::OnFileSave)
END_MESSAGE_MAP()


// CMPainterDoc 构造/析构

CNPainterDoc::CNPainterDoc()
	: COleDocument()
{
	computePageSize();
}

CNPainterDoc::~CNPainterDoc()
{
}

BOOL CNPainterDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


void CNPainterDoc::OnFileSave()
{
	//////COleDocument::OnFileSave();
	CString filepath;
	filepath.Empty();
	//static wchar_t filter[] = L"MPainter 文件 (*.mpn)|*.mpn|所有文件*.*|*.*||";
	//DWORD flag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | AFX_IDS_SAVEFILE | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	//CFileDialog dlgFile(FALSE, L"*.mpn", NULL, flag, filter, NULL, 0);
	//if(dlgFile.DoModal() == IDOK)
	//{
	//	filepath = dlgFile.GetPathName();
	//	std::ofstream ofs(filepath.GetBuffer());
	//	boost::archive::binary_oarchive ar(ofs);
	//	ar & m_objAdmin;
	//	ofs.close();
	//}
}


// CNPainterDoc 序列化

void CNPainterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
///*****************************************************************************************
//	<< --- CNPainterDoc::draw	2013-12-08 --- >>
//	说明：绘制绘图模块
//	参数：
//	pdc	= 绘图环境
//	返回值：
//*****************************************************************************************/
//void CNPainterDoc::draw(CDC *pdc)
//{
//	CNPainterView *pview = nullptr;
//	gtc::itobj *pobj = nullptr;
//	POSITION pos = GetFirstViewPosition();
//	if(pos)
//		pview = dynamic_cast<CNPainterView*>(GetNextView(pos));
//	for(gtc::itobj::PtrArrayType::size_type idx = 0; idx < m_vecObjects.size(); ++idx)
//	{
//		pobj = m_vecObjects[idx];
//		pobj->draw(pdc);
//		if(pview->m_bActive && !pdc->IsPrinting() && pview->isSelected(pobj))
//			pobj->drawTracker(pdc, gtc::itobj::TSSelected,m_clrTracker);
//	}
//}

/*****************************************************************************************
	<< --- CNPainterDoc::computePageSize	2013-12-04 --- >>
	说明：根据系统设定的打印机得到itobjPage信息
	参数：
	返回值：
*****************************************************************************************/
void CNPainterDoc::computePageSize()
{
	PRINTDLG pd = {0};
	CSize oldSize = m_objAdmin.getPager()->getPaperInfo().bsSize.toMSSize();// m_objPaper.getPaperInfo().bsSize.toMSSize();
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd))
	{
		DEVMODE *pdev = (DEVMODE*)GlobalLock(pd.hDevMode);
		if(pdev)
		{
			gtc::BSPaperInfo &paper = m_objAdmin.getPager()->getPaperInfo();
			paper.iOrientation = pdev->dmOrientation;
			paper.iSize = pdev->dmPaperSize;
			paper.bsSize.width = pdev->dmPaperWidth;
			paper.bsSize.height = pdev->dmPaperLength;
			paper.wsName = pdev->dmFormName;
			GlobalUnlock(pd.hDevMode);

			m_objAdmin.getPager()->refresh();
		}
	}
	//if(oldSize != m_objPaper.getPaperInfo().bsSize.toMSSize())
	{	// 刷新工作区
		POSITION pos = GetFirstViewPosition();
		if(pos != NULL)
			((CNPainterView*)GetNextView(pos))->OnUpdate(nullptr, HINT_UPDATE_WINDOW, nullptr);
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNPainterView
//	说明：该类作为自定义绘图视图的公共基类，内部定义了公共方法
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 绘图对象的自定义剪贴板格式
CLIPFORMAT CNPainterView::mc_cfBItemObjects = (CLIPFORMAT)::RegisterClipboardFormat(L"BItem Objects");
BEGIN_MESSAGE_MAP(CNPainterView, CScrollView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_ME_ITEM_PROPERTY_CHANGED, &CNPainterView::OnMyItemPropertyChanged)
	ON_MESSAGE(WM_ME_ITEM_ACTION, &CNPainterView::OnMyItemAction)
	ON_MESSAGE(WM_ME_ITEM_REDRAW, &CNPainterView::OnMyItemRedraw)
END_MESSAGE_MAP()



CNPainterView::CNPainterView()
	:m_bActive(false)
	, m_szWorker(3500,3500)/*2970+400, 2100+400)*/
{

}
///*****************************************************************************************
//	<< --- CNPainterView::drawBackground	2013-12-02 --- >>
//	说明：绘制工作区和纸张
//	参数：
//	pdc	= CDC*
//	返回值：
//*****************************************************************************************/
//void CNPainterView::drawBackground(CDC *pdc)
//{
//	CNPainterDoc *pdoc = GetDocument();
//	if(!pdoc || !pdc || pdc->IsPrinting())
//		return;
//	gtc::itobjPage &page = pdoc->getPaper();
//	CSize szworker = m_szWorker;
//	CSize szpaper = page.getPaperInfo().bsSize.toMSSize();
//	CRect rtworker, rtpaper;
//	rtworker.left = -(szworker.cx-szpaper.cx)/2;
//	rtworker.right = (szworker.cx+szpaper.cx)/2;
//	rtworker.top = (szworker.cy+szpaper.cy)/2;
//	rtworker.bottom = -(szworker.cy-szpaper.cy)/2;
//	if(page.supportMargins())
//	{
//#if defined(WORK_AXIS_CAD)
//		rtworker.OffsetRect(-page.getMarginLeft(), -page.getMarginBottom());
//#else	//defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
//		rtworker.OffsetRect(-page.getMarginLeft(), -page.getMarginTop());
//#endif
//	}
//	if(page.supportHeaderFooter())
//	{	// Y轴方向原点(0,0)继续内移
//#if defined(WORK_AXIS_CAD)
//		rtworker.OffsetRect(0, -page.getFooter().uiHeight);
//#else	//defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
//		rtworker.OffsetRect(0, -page.getHeader().uiHeight);
//#endif
//	}
//	rtworker.NormalizeRect();
//	rtworker.DeflateRect(10,10);
//	pdc->FillRect(rtworker, &CBrush(0xFFEAEAEA));
//	pdoc->getPaper().draw(pdc);
////#if defined(WORK_AXIS_CAD)
////	rtpaper.NormalizeRect();
////	rtpaper.OffsetRect(10, -10);
////	pdc->FillRect(rtpaper, &CBrush(0xFF808080));
////	rtpaper.OffsetRect(-10, 10);
////	pdc->FillRect(rtpaper, &CBrush(0xFFFFFFFF));
////#elif defined(WORK_AXIS_DOC)
////	rtpaper.NormalizeRect();
////	rtpaper.OffsetRect(10, 10);
////	pdc->FillRect(rtpaper, &CBrush(0xFF808080));
////	rtpaper.OffsetRect(-10, -10);
////	pdc->FillRect(rtpaper, &CBrush(0xFFFFFFFF));
////#else	// defined(WORK_AXIS_NORMAL)
////	rtpaper.NormalizeRect();
////	rtpaper.OffsetRect(10, 10);
////	pdc->FillRect(rtpaper, &CBrush(0xFF808080));
////	rtpaper.OffsetRect(-10, -10);
////	pdc->FillRect(rtpaper, &CBrush(0xFFFFFFFF));
////#endif
//}
/*****************************************************************************************
	<< --- CNPainterView::resetScrollSizes	2013-12-02 --- >>
	说明：根据窗口客户区（设备坐标：像素）设定滚轴区间，使纸保持在窗口客户区中心
	参数：
	clientArea	= 窗口客户区，由GetClientRect或OnSize得到（设备坐标）
	返回值：
*****************************************************************************************/
void CNPainterView::resetScrollSizes(CSize clientArea)
{
	CNPainterDoc *pdoc = GetDocument();
	if(!pdoc)
		return;
	CRect rtclient;
	const CSize client = clientArea;
	if(clientArea.cx <= 0 || clientArea.cy <= 0)
	{
		GetClientRect(rtclient);
		rtclient.NormalizeRect();
		clientArea.SetSize(rtclient.Width(), rtclient.Height());
	}
	CClientDC dc(NULL);
	// 转换到逻辑坐标
	clientArea.cx = MulDiv(clientArea.cx, 254, dc.GetDeviceCaps(LOGPIXELSX));
	clientArea.cy = MulDiv(clientArea.cy, 254, dc.GetDeviceCaps(LOGPIXELSY));
	// 工作台尺寸（逻辑单位） > 纸张尺寸 > 窗口客户区
	int32_t cx = 0;// max(clientArea.cx, pdoc->getAdminer().getPager().getPaperInfo().bsSize.cx)+20;
	int32_t cy = 0;// max(clientArea.cy, pdoc->getAdminer().getPager().getPaperInfo().bsSize.cy)+20;
	gtc::BSPaperInfo &paper = pdoc->getAdminer().getPager()->getPaperInfo();
	if(paper.isLandscape())	// 默认是纵向1 横向2
	{
		cx = max(clientArea.cx, paper.bsSize.height)+20;
		cy = max(clientArea.cy, paper.bsSize.width)+20;
		//std::swap(cx, cy);
	}
	else
	{
		cx = max(clientArea.cx, paper.bsSize.width)+20;
		cy = max(clientArea.cy, paper.bsSize.height)+20;
	}
	if(m_szWorker.cx < cx)
		m_szWorker.cx = cx;
	if(m_szWorker.cy < cy)
		m_szWorker.cy = cy;
	// 
	CSize szWorker = m_szWorker;
	szWorker.cx = MulDiv(szWorker.cx, dc.GetDeviceCaps(LOGPIXELSX), 254);
	szWorker.cy = MulDiv(szWorker.cy, dc.GetDeviceCaps(LOGPIXELSY), 254);
	SetScrollSizes(MM_TEXT, szWorker);
	// 水平滚动条居中
	SCROLLINFO scrinfo = {0};
	scrinfo.cbSize = sizeof(SCROLLINFO);
	GetScrollInfo(SB_HORZ, &scrinfo);
	SetScrollPos(SB_HORZ, (scrinfo.nMax-scrinfo.nMin-scrinfo.nPage)/2);
	// 垂直显示纸边
	GetScrollInfo(SB_VERT, &scrinfo);
	CSize szPaper = paper.bsSize.toMSSize();
	if(paper.isLandscape())	// 默认是纵向
		std::swap(szPaper.cx, szPaper.cy);
	szPaper.cx = MulDiv(szPaper.cx, dc.GetDeviceCaps(LOGPIXELSX), 254);
	szPaper.cy = MulDiv(szPaper.cy, dc.GetDeviceCaps(LOGPIXELSY), 254);
	SetScrollPos(SB_VERT, (scrinfo.nMax-scrinfo.nMin-scrinfo.nPage)*(szPaper.cy+50)/szWorker.cy);

}

/*****************************************************************************************
	<< --- CNPainterView::moveHandleTo	2013-11-30 --- >>
	说明：选中指定区域内的对象
	参数：
	rect	= 区域
	bAdd	= true时，往选中对象集中追加对象，否则先清空选中记录集
	返回值：
*****************************************************************************************/
void CNPainterView::selectWithinRect(CRect rect, bool bAdd)
{
	if(!bAdd)
		select(nullptr);
	clientToDoc(rect);
	gtc::rect bsrt(rect.left, rect.top, rect.right, rect.bottom);
	gtc::itobj::PtrArrayType &objs = GetDocument()->getAdminer().getItems();
	for(gtc::itobj::PtrArrayType::size_type idx = 0; idx < objs.size(); ++idx)
	{
		if(objs[idx]->intersects(bsrt))
			select(objs[idx], true);
	}

}
/*****************************************************************************************
	<< --- CNPainterView::editProperties	2013-11-30 --- >>
	说明：通过属性对话框编辑pobj的属性
	参数：
	pobj	= 被编辑对象
	返回值：
*****************************************************************************************/
void CNPainterView::editProperties(gtc::itobj* pobj)
{
}

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
LRESULT CNPainterView::OnMyItemPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	gtc::itobj* pobj = (gtc::itobj*)lParam;
	if(pobj)
	{
		invalidate(pobj);
	}
	return 0;
}

/*****************************************************************************************
	<< --- 自定义事件：WM_ME_ITEM_ACTION	2013-12-14 --- >>
	说明：刷新绘图对象
	参数：
	WPARAM	= 动作类别BEItemAction
	LPARAM	= 被选中的对象集合指针(itobj::PtrArrayType*)，接收者负责释放。
	返回值：
*****************************************************************************************/
LRESULT CNPainterView::OnMyItemAction(WPARAM wParam, LPARAM lParam)
{
	BSActionSender *psender = (BSActionSender *)wParam;
	gtc::itobj* pobj = (gtc::itobj*)lParam;
	return 0;
}

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
LRESULT CNPainterView::OnMyItemRedraw(WPARAM wParam, LPARAM lParam)
{
	gtc::itobj *pobj = (gtc::itobj*)wParam;
	gtc::rect *prect = (gtc::rect*)lParam;
	if (pobj)
	{
		if (prect)
		{
			invalidate(*prect);
			//CClientDC dc(this);
			//OnPrepareDC(&dc, NULL);
			//RECT rc = *prect;
			//dc.Rectangle(&rc);
			//RECT rc = prect->operator RECT();
			//docToClient(rc);
			//::InflateRect(&rc, 1, 1);
			//invalidate(rc);
			//docToClient(*prect);
			//prect->inflate(1,1);
			//invalidate(*prect);
		}
		else
		{
			invalidate(pobj);
		}
	}
	delete prect;

	return 0;
}


void CNPainterView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
	case HINT_UPDATE_WINDOW:    // redraw entire window
		Invalidate(FALSE);
		break;

	case HINT_UPDATE_DRAWOBJ:   // a single object has changed
		//invalidate((CDrawObj*)pHint);
		break;

	case HINT_UPDATE_SELECTION: // an entire selection has changed
		{
			for(gtc::itobj::PtrArrayType::size_type idx = 0; idx < m_vecSels.size(); ++idx)
				invalidate(m_vecSels[idx]);
		}
		break;

	case HINT_DELETE_SELECTION: // an entire selection has been removed
		break;
	case HINT_UPDATE_OLE_ITEMS:
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CNPainterView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	// invalidate selections when active status changes
	if(m_bActive != (bActivate == TRUE))
	{
		if(bActivate)
			m_bActive = true;
		if(!m_vecSels.empty())
			OnUpdate(nullptr, HINT_UPDATE_SELECTION, nullptr);
		m_bActive = bActivate == TRUE;
	}

}


void CNPainterView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	resetScrollSizes();

}


void CNPainterView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo);

	CNPainterDoc *pdoc = GetDocument();
	if(!pdoc)
		return;
	////1.SetWindowOrg(x, y) 是把设备坐标的原点(视口)映射到逻辑坐标的(X, Y)处 ，查看花布任意处
	////2.SetViewportOrg(x, y) 是把逻辑坐标的原点(窗口)映射到设备坐标的(X, Y)处
	// 
#if defined(WORK_AXIS_CAD)	// 往右、上递增；1逻辑单位等于0.1毫米
	pDC->SetMapMode(MM_ISOTROPIC);

	long wpin = pDC->GetDeviceCaps(LOGPIXELSX);	// 屏幕水平方向每英寸像素点数	
	long hpin = pDC->GetDeviceCaps(LOGPIXELSY);	// 屏幕垂直方向每英寸像素点数	

	pDC->SetWindowExt(254, -254);		// 窗口的尺寸，逻辑坐标；默认时向右，向下递增，变负时就反方向了
	pDC->SetViewportExt(wpin, hpin);	// 视区的尺寸，设备坐标（像素）
	pDC->SetWindowOrg(pdoc->getAdminer().getPager()->getWindowOrg(gtc::size(m_szWorker.cx, m_szWorker.cy), pDC->IsPrinting()).toMSPoint());
	//pDC->OffsetWindowOrg(300, 300);
#elif defined(WORK_AXIS_DOC)	// 往右、下递增；1逻辑单位等于0.1毫米
	pDC->SetMapMode(MM_ISOTROPIC);

	long wpin = pDC->GetDeviceCaps(LOGPIXELSX);	// 屏幕水平方向每英寸像素点数	
	long hpin = pDC->GetDeviceCaps(LOGPIXELSY);	// 屏幕垂直方向每英寸像素点数	

	pDC->SetWindowExt(254, 254);		// 窗口的尺寸，逻辑坐标；默认时向右，向下递增，变负时就反方向了
	pDC->SetViewportExt(wpin, hpin);	// 视区的尺寸，设备坐标（像素）
	pDC->SetWindowOrg(pdoc->getAdminer().getPager().getWindowOrg(gtc::size(m_szWorker.cx, m_szWorker.cy), pDC->IsPrinting()).toMSPoint());

	//CSize worker, paper;
	//// 将工作布大小（逻辑坐标尺寸）映射到设备坐标（像素）
	//worker.cx = MulDiv(wpin, m_bsWorker.cx, 254);
	//worker.cy = MulDiv(hpin, m_bsWorker.cy, 254);
	//// 将打印纸大小（逻辑坐标尺寸）映射到设备坐标（像素）
	//paper.cx = MulDiv(wpin, m_bsPaper.cx, 254);
	//paper.cy = MulDiv(hpin, m_bsPaper.cy, 254);
	////1.SetWindowOrg(x, y) 是把设备坐标的原点(视口)映射到逻辑坐标的(X, Y)处 ，查看花布任意处
	////2.SetViewportOrg(x, y) 是把逻辑坐标的原点(窗口)映射到设备坐标的(X, Y)处
	//CPoint ptorg;
	//ptorg.x = (m_bsWorker.cx-m_bsPaper.cx)/2;
	//ptorg.y = (m_bsWorker.cy-m_bsPaper.cy)/2;
	//pDC->SetWindowOrg(-ptorg.x, -ptorg.y);
	
#else //defined(WORK_AXIS_NORMAL)
	pDC->SetMapMode(MM_TEXT);
#endif

}



void CNPainterView::OnDraw(CDC* pDC)
{
	CNPainterDoc* pdoc = GetDocument();
	ASSERT_VALID(pdoc);
	if (!pdoc)
		return;

	CRect client, rect;
	
	CDC dc;
	CDC *pdrawdc = pDC;
	CBitmap bitmap;
	CBitmap* poldbmp = nullptr;
	pDC->GetClipBox(client);
	rect = client;
	docToClient(rect);
	CString str;
	str.Format(_T("CNPainterView::OnDraw clipRect:<%d,%d,%d,%d %d,%d>"), rect.left, rect.top, rect.right, rect.bottom, rect.Width(), rect.Height());
	AfxGetMainWnd()->SetWindowText(str);
	if(!pDC->IsPrinting())
	{
		if(dc.CreateCompatibleDC(pDC))
		{
			if(bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
			{
				OnPrepareDC(&dc, NULL);
				pdrawdc = &dc;
				dc.OffsetViewportOrg(-rect.left, -rect.top);
				poldbmp = dc.SelectObject(&bitmap);
				dc.SetBrushOrg(rect.left % 8, rect.top % 8);

				// might as well clip to the same rectangle
				//dc.SelectClipRgn(NULL);
				dc.IntersectClipRect(client);
			}
		}
	}
	//// 更新背景
	//if(!pdrawdc->IsPrinting())
	//	DoDrawBackground(pdrawdc);
	//// 背景图片
	//pdoc->m_adminObjs.DrawPaperBack(pdrawdc);
	////// 画网格 VSP3.1 网格线已经集成到itobjPage::Draw()
	////if(!pdrawdc->IsPrinting() && pdoc->m_adminObjs.IsVisibleGrid())
	////	DoDrawGrid(pdrawdc);
	//pdoc->m_adminObjs.DrawObjects(pdrawdc);
	//// 绘制选中标识
	//DoDrawSelectItems(pdrawdc);
	// 
	pdoc->getAdminer().drawBackground(pdrawdc, m_szWorker);
	//// 画对象
	pdoc->getAdminer().draw(pdrawdc, this);
	pdoc->draw(pdrawdc);
	// 刷新客户区
	
	//if(pdrawdc != pDC)
	//{	
	//	pDC->BitBlt(client.left, client.top, client.Width(), client.Height(), pdrawdc, client.left, client.top, SRCCOPY);
	//	if(poldbmp)
	//		pdrawdc->SelectObject(poldbmp);
	//}
	if (pdrawdc != pDC)
	{
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0,0);
		pDC->SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0,0);
		dc.SetMapMode(MM_TEXT);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&dc, 0, 0, SRCCOPY);
		dc.SelectObject(poldbmp);
	}

	

}


void CNPainterView::OnSize(UINT nType, int cx, int cy)
{
//#ifdef DEBUG
//	DEBUG_LOG(L"exec CNPainterView::OnSize ......");
//#endif
	CScrollView::OnSize(nType, cx, cy);
	if(m_bActive)
		resetScrollSizes(CSize(cx, cy));
}

void CNPainterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bActive)
		return;
	CNTool *ptool = CNTool::findTool(CNTool::mc_drawShape);
	if(ptool)
		ptool->OnLButtonDown(this, nFlags, point);
	//CScrollView::OnLButtonDown(nFlags, point);
}



void CNPainterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bActive)
		return;
	CNTool *ptool = CNTool::findTool(CNTool::mc_drawShape);
	if(ptool)
		ptool->OnLButtonUp(this, nFlags, point);
	//CScrollView::OnLButtonUp(nFlags, point);
}



void CNPainterView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bActive)
		return;
	CNTool *ptool = CNTool::findTool(CNTool::mc_drawShape);
	if(ptool)
		ptool->OnMouseMove(this, nFlags, point);
	//CScrollView::OnMouseMove(nFlags, point);
}
void CNPainterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!m_bActive)
		return;
	CNTool *ptool = CNTool::findTool(CNTool::mc_drawShape);
	if(ptool)
		ptool->OnKeyDown(this, nChar, nRepCnt, nFlags);
	//CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CNPainterView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bActive)
		return;
	CNTool *ptool = CNTool::findTool(CNTool::mc_drawShape);
	if(ptool)
		ptool->OnLButtonDblClk(this, nFlags, point);
	//CScrollView::OnLButtonDblClk(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNTool
//	说明：依赖的公共操作
//	日期：2013-11-29
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 静态变量初始化
std::vector<CNTool*>	CNTool::mc_vecTools;
CPoint CNTool::mc_down = CPoint(0,0);
UINT CNTool::mc_nDownFlags;
CPoint CNTool::mc_last;
BEToolShape CNTool::mc_drawShape = BEToolShapeSelection;
bool CNTool::mc_isDrawing = false;
// 实例化对象
static CNToolSelect selectTool;
static CNToolRect lineTool(BEToolShapeLine);
static CNToolRect rectTool(BEToolShapeRect);
static CNToolRect roundRectTool(BEToolShapeRoundRect);
static CNToolRect ellipseTool(BEToolShapeEllipse);
static CNToolGrid gridTool;
//static CNToolRect polyTool;

inline CNTool::CNTool(BEToolShape shape)
	: m_beDrawShape(shape)
{
	mc_vecTools.push_back(this);
}

inline CNTool* CNTool::findTool(BEToolShape drawShape)
{
	for(std::vector<CNTool*>::size_type idx = 0; idx < mc_vecTools.size(); ++idx)
	{
		if(mc_vecTools[idx]->m_beDrawShape == drawShape)
			return mc_vecTools[idx];
	}
	return nullptr;
}

//
void CNTool::OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	// deactivate any in-place active item on this view!
	COleClientItem* pActiveItem = pView->GetDocument()->GetInPlaceActiveItem(pView);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
		ASSERT(pView->GetDocument()->GetInPlaceActiveItem(pView) == NULL);
	}	
	pView->SetCapture();
	mc_nDownFlags = nFlags;
	mc_down = point;
	mc_last = point;
	mc_isDrawing = true;

}

void CNTool::OnLButtonDblClk(CNPainterView* /*pView*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
}
void CNTool::OnMouseMove(CNPainterView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
	mc_last = point;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CNTool::OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	//ReleaseCapture();

	//if (point == mc_down)
	//	mc_drawShape = BEToolShapeSelection;
}

void CNTool::OnKeyDown(CNPainterView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!pView)
		return;
	// 选中对象的上下左右移动默认为1mm/次，如按下Ctrl则是微移0.2mm/次
	bool isMove = false;
	int32_t off = 10, offx = 0, offy = 0;
	if((nFlags & MK_CONTROL) != 0)
		off = 2;

	switch(nChar)
	{
	case VK_SPACE:	// 结束本次绘图操作
		releaseTool();
		break;
	case VK_ESCAPE:	// 取消所有选中对象的选中标记
		{
			releaseTool();
			gtc::itobj::PtrArrayType vecSels = pView->m_vecSels;
			pView->m_vecSels.clear();
			for(gtc::itobj::PtrArrayType::size_type idx = 0; idx < vecSels.size(); ++idx)
			{
				CRect rt = vecSels[idx]->getPosition();
				pView->docToClient(rt);
				int space = gtc::itobj::mc_iTrackerSize/2;
				//if(m_bActive && isSelected(pobj))
				{
					rt.left -= space;
					rt.top -= space+1;
					rt.right += space+1;
					rt.bottom += space;
				}
				rt.InflateRect(1,1);
				pView->InvalidateRect(rt, FALSE);
			}
		}
		break;
	case VK_UP:		// 上移
		isMove = true;
		offy = -off;
		break;
	case VK_DOWN:	// 下移
		isMove = true;
		offy = off;
		break;
	case VK_LEFT:	// 左移
		isMove = true;
		offx = -off;
		break;
	case VK_RIGHT:	// 右移
		isMove = true;
		offx = off;
		break;
	}
	if(isMove)
	{
		gtc::rect rt;
		for(gtc::itobj::PtrArrayType::iterator iter = pView->m_vecSels.begin(), end = pView->m_vecSels.end(); iter != end; ++iter)
		{
			rt = (*iter)->getPosition();
			rt.offset(offx, offy);
			(*iter)->moveTo(rt, pView);
			std::vector<int32_t> vecProp;
			vecProp.push_back(gtc::itobj::BEItemPropPositionX);
			vecProp.push_back(gtc::itobj::BEItemPropPositionY);
			vecProp.push_back(gtc::itobj::BEItemPropPositionWidth);
			vecProp.push_back(gtc::itobj::BEItemPropPositionHeight);
			(*iter)->messageProperty(gtc::BEDrawShapeTypeItem, vecProp);
		}
	}
}

void CNTool::OnEditProperties(CNPainterView* /*pView*/)
{
}

void CNTool::OnCancel()
{
	mc_drawShape = BEToolShapeSelection;
}

/*****************************************************************************************
	CNToolSelect
	说明：对象选中后的相关操作；如：移位，改变大小...
	日期：2013-11-29
*****************************************************************************************/

enum BESelectMode
{
	BESelectModeNone,
	BESelectModeNetSelect,
	BESelectModeMove,
	BESelectModeSize		// 通过定位块改变对象尺寸
};

BESelectMode g_beSelectMode = BESelectModeNone;
int g_iDragHandle;

CPoint lastPoint;

CNToolSelect::CNToolSelect()
	: CNTool(BEToolShapeSelection)
{
}

void CNToolSelect::OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point)
{

	CPoint local = point;
	pView->clientToDoc(local);

	gtc::itobj* pobj = nullptr;
	g_beSelectMode = BESelectModeNone;

	// Check for resizing (only allowed on single selections)
	if (pView->m_vecSels.size() == 1)
	{
		//pObj = pView->m_selection.GetHead();
		//nDragHandle = pObj->HitTest(local, pView, TRUE);
		//if (nDragHandle != 0)
		//	selectMode = size;
		pobj = pView->m_vecSels[0];
		g_iDragHandle = pobj->hitTest(local, pView, true);// CNTool::hitTest(pobj, local, pView, true);
		if(g_iDragHandle > 0)
			g_beSelectMode = BESelectModeSize;
	}

	// See if the click was on an object, select and start move if so
	if (g_beSelectMode == BESelectModeNone)
	{
		pobj = pView->GetDocument()->objectAt(local);

		if (pobj != nullptr)
		{
			if(pobj->getType() == gtc::BEDrawShapeTypeGrid)
			{	// 选中单元格
				pView->invalidate(pobj);
			}
			g_beSelectMode = BESelectModeMove;

			if (!pView->isSelected(pobj))
				pView->select(pobj, (nFlags & MK_SHIFT) != 0);

			// Ctrl+Click clones the selection...
			if ((nFlags & MK_CONTROL) != 0)
				pView->cloneSelection();
		}
	}

	// Click on background, start a net-selection
	if (g_beSelectMode == BESelectModeNone)
	{
		if ((nFlags & MK_SHIFT) == 0)
			pView->select(nullptr);

		g_beSelectMode = BESelectModeNetSelect;// netSelect;

		CClientDC dc(pView);
		CRect rt(point.x, point.y, point.x, point.y);
		rt.NormalizeRect();
		dc.DrawFocusRect(rt);
	}

	lastPoint = local;
	CNTool::OnLButtonDown(pView, nFlags, point);
}

void CNToolSelect::OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() != pView)
	{
		if (mc_drawShape == BEToolShapeSelection && pView->m_vecSels.size() == 1)
		{	// 如果只选中了一个对象，就通过定位块改变尺寸
			//CDrawObj* pObj = pView->m_selection.GetHead();
			//CPoint local = point;
			//pView->ClientToDoc(local);
			//int nHandle = pObj->HitTest(local, pView, TRUE);
			//if (nHandle != 0)
			//{
			//	SetCursor(pObj->GetHandleCursor(nHandle));
			//	return; // bypass CDrawTool
			//}
			gtc::itobj* pobj = pView->m_vecSels[0];
			CPoint local = point;
			pView->clientToDoc(local);
			int nHandle = pobj->hitTest(local, pView, true);// hitTest(pobj, local, pView, true);
			if(nHandle > 0)
			{
				SetCursor(pobj->getHandleCursor(nHandle));
				return;
			}

		}
		if (mc_drawShape == BEToolShapeSelection)
			CNTool::OnMouseMove(pView, nFlags, point);
		return;
	}

	if (g_beSelectMode == BESelectModeNetSelect)
	{
		CClientDC dc(pView);
		CRect rt(mc_down.x, mc_down.y, mc_last.x, mc_last.y);
		rt.NormalizeRect();
		dc.DrawFocusRect(rt);
		rt.SetRect(mc_down.x, mc_down.y, point.x, point.y);
		rt.NormalizeRect();
		dc.DrawFocusRect(rt);

		CNTool::OnMouseMove(pView, nFlags, point);
		return;
	}

	CPoint local = point;
	pView->clientToDoc(local);
	CPoint delta = (CPoint)(local - lastPoint);
	for(gtc::itobj::PtrArrayType::size_type idx = 0; idx < pView->m_vecSels.size(); ++idx)
	{
		gtc::itobj* pobj = pView->m_vecSels[idx];
		CRect pos = pobj->getPosition();
		if(g_beSelectMode == BESelectModeMove)
		{	// 整体移动
			pos += delta;
			pobj->moveTo(pos, pView);
			//pView->moveTo(pos, pobj);
		}
		else if(g_iDragHandle != 0)
		{	// 改变尺寸
			pobj->moveHandleTo(g_iDragHandle, local, pView);
			//pView->moveHandleTo(g_iDragHandle, local, pobj);
		}
	}

	/*POSITION pos = pView->m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = pView->m_selection.GetNext(pos);
		CRect position = pObj->m_position;

		if (selectMode == move)
		{
			position += delta;
			pObj->MoveTo(position, pView);
		}
		else if (nDragHandle != 0)
		{
			pObj->MoveHandleTo(nDragHandle, local, pView);
		}
	}*/

	lastPoint = local;

	if (g_beSelectMode == BESelectModeSize && mc_drawShape == BEToolShapeSelection)
	{
		mc_last = point;
		SetCursor(pView->m_vecSels[0]->getHandleCursor(g_iDragHandle));
		//SetCursor(pView->m_selection.GetHead()->GetHandleCursor(nDragHandle));
		return; // bypass CDrawTool
	}

	mc_last = point;

	if (mc_drawShape == BEToolShapeSelection)
		CNTool::OnMouseMove(pView, nFlags, point);
}

void CNToolSelect::OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() == pView)
	{
		if (g_beSelectMode == BESelectModeNetSelect)
		{
			CClientDC dc(pView);
			CRect rt(mc_down.x, mc_down.y, mc_last.x, mc_last.y);
			rt.NormalizeRect();
			dc.DrawFocusRect(rt);

			pView->selectWithinRect(rt, true);
		}
		else if (g_beSelectMode != BESelectModeNone)
		{
			pView->GetDocument()->UpdateAllViews(pView);
		}
		// 
		BSActionSender *psender = new BSActionSender(BESenderFlagView, BEItemActionSelect);
		AfxGetMainWnd()->SendMessage(WM_ME_ITEM_ACTION, (WPARAM)psender, 0L);
	}
	if(mc_drawShape == BEToolShapeSelection)
	{
		releaseTool();
	}

	CNTool::OnLButtonUp(pView, nFlags, point);
}

void CNToolSelect::OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	if ((nFlags & MK_SHIFT) != 0)
	{
		// Shift+DblClk deselects object...
		CPoint local = point;
		pView->clientToDoc(local);
		gtc::itobj* pobj = pView->GetDocument()->objectAt(local);
		if (pobj != nullptr)
			pView->deselect(pobj);
	}
	else
	{
		// "Normal" DblClk opens properties, or OLE server...
		if(pView->m_vecSels.size() == 1)
		{
			OnEditProperties(pView);
		}
/*		if (pView->m_selection.GetCount() == 1)
			pView->m_selection.GetHead()->OnOpen(pView);*/
	}

	CNTool::OnLButtonDblClk(pView, nFlags, point);
}

void CNToolSelect::OnEditProperties(CNPainterView* pView)
{
	//if (pView->m_selection.GetCount() == 1)
	//	pView->m_selection.GetHead()->OnEditProperties();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNToolRect
//	说明：对象选中后的相关操作；如：移位，改变大小...
//	日期：2013-11-30
////////////////////////////////////////////////////////////////////////////////////////////////////////
CNToolRect::CNToolRect(BEToolShape drawShape)
	: CNTool(drawShape)
{
}

void CNToolRect::OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	if(releaseTool() && pView->m_vecSels.size() == 1)
	{
		//pView->m_vecSels[0]->getPosition().normalizeRect();
		return;
	}
	CNTool::OnLButtonDown(pView, nFlags, point);
	pView->select(nullptr);
	CPoint local = point;
	pView->clientToDoc(local);
	gtc::itobjRect *pobj = new gtc::itobjRect(gtc::rect(local.x, local.y, 1, 1));//local.x, local.y));
	switch (m_beDrawShape)
	{
	default:
		ASSERT(FALSE); // unsuported shape!

	case BEToolShapeRect:
		pobj->setShape(gtc::itobjRect::BEShapeRectangle);
		break;

	case BEToolShapeRoundRect:
		pobj->setShape(gtc::itobjRect::BEShapeRoundRectangle);
		break;

	case BEToolShapeEllipse:
		pobj->setShape(gtc::itobjRect::BEShapeEllipse);
		break;

	case BEToolShapeLine:
		pobj->setShape(gtc::itobjRect::BEShapeLine);
		break;
	}
	pView->GetDocument()->add(pobj);
	pView->select(pobj);

	g_beSelectMode = BESelectModeSize;
	g_iDragHandle = 1;
	lastPoint = local;
}

void CNToolRect::OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	selectTool.OnMouseMove(pView, nFlags, point);
}

void CNToolRect::OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	//if (point == mc_down && pView->m_vecSels.size() > 0)
	//{
	//	// Don't create empty objects...
	//	gtc::itobj* pobj = pView->m_vecSels[pView->m_vecSels.size()-1];
	//	pView->remove(pobj);
	//	pView->GetDocument()->remove(pobj);
	//	//pobj->remove();
	//	//CDrawObj *pObj = pView->m_selection.GetTail();
	//	//pView->GetDocument()->Remove(pObj);
	//	//pObj->Remove();
	//	selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
	//}

	selectTool.OnLButtonUp(pView, nFlags, point);
}

//
void CNToolRect::OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	CNTool::OnLButtonDblClk(pView, nFlags, point);
}
//

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CNToolGrid
//	说明：对象选中后的相关操作；如：移位，改变大小...
//	日期：2013-12-17
////////////////////////////////////////////////////////////////////////////////////////////////////////

CNToolGrid::CNToolGrid()
	: CNTool(BEToolShapeGrid)
{
}

// Implementation
void CNToolGrid::OnLButtonDown(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	//if(releaseTool() && pView->m_vecSels.size() == 1)
	//{
	//	pView->m_vecSels[0]->getPosition().normalizeRect();
	//	return;
	//}
	//CNTool::OnLButtonDown(pView, nFlags, point);
	//pView->select(nullptr);
	//CPoint local = point;
	//pView->clientToDoc(local);
	//gtc::itobjGrid *pgrid = new gtc::itobjGrid(gtc::rect(local.x, local.y, local.x, local.y));
	//
	//pView->GetDocument()->add(pgrid);
	//pView->select(pgrid);

	//g_beSelectMode = BESelectModeSize;
	//g_iDragHandle = 1;
	//lastPoint = local;
}
void CNToolGrid::OnMouseMove(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	selectTool.OnMouseMove(pView, nFlags, point);
}
void CNToolGrid::OnLButtonUp(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	selectTool.OnLButtonUp(pView, nFlags, point);
}
void CNToolGrid::OnLButtonDblClk(CNPainterView* pView, UINT nFlags, const CPoint& point)
{
	CNTool::OnLButtonDblClk(pView, nFlags, point);
}

}
