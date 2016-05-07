#include "itobjAdmin.h"
#include "AC\MFC\NTool.h"
namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjAdmin	2013-12-13 --- >>
//	绘图单元管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////

	
itobjAdmin::itobjAdmin()
	: m_clrTracker(0xFFFF6633)
	, m_pobjPage(new itobjPage())
{
#if defined(WORK_AXIS_CAD)
	m_beWorkaxis = BEWorkAxisCAD;
#elif defined(WORK_AXIS_DOC)
	m_beWorkaxis = BEWorkAxisDOC;
#elif defined(WORK_AXIS_NORMAL)
	m_beWorkaxis = BEWorkAxisNormal;
#endif
	//rect rt(0,0,1000,1000);
	//rt.normalizeRect();
	//mpobj = new itobj(rt);
	//mpobj->setAlignMode(BEAlignModeRightCenter);
	////mpobj->getFont().iEscapement = 900;
	//mpobj->setText(L"dAQq含之332yPqg");
	//mpobj->setBackColor(0xFF0000ff);

}
itobjAdmin::itobjAdmin(const itobjAdmin &admin)
	: m_clrTracker(admin.m_clrTracker), m_beWorkaxis(admin.m_beWorkaxis)
{
	if(m_pobjPage)
		m_pobjPage->remove();
	if(admin.m_pobjPage)
		m_pobjPage = dynamic_cast<itobjPage *>(admin.m_pobjPage->clone());
	copy(admin.m_vecItems);
}
itobjAdmin& itobjAdmin::operator=(const itobjAdmin &admin)
{
	if(this != &admin)
	{
		m_clrTracker = admin.m_clrTracker;
		m_beWorkaxis = admin.m_beWorkaxis;
		if(m_pobjPage)
			m_pobjPage->remove();
		if(admin.m_pobjPage)
			m_pobjPage = dynamic_cast<itobjPage *>(admin.m_pobjPage->clone());

	removeAll();
		copy(admin.m_vecItems);
	}
	return *this;
}
itobjAdmin::~itobjAdmin()
{
	removeAll();
	if(m_pobjPage)
		m_pobjPage->remove();
}
#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjAdmin::draw	2013-12-13 --- >>
	说明：绘制绘图模块
	参数：
	pdc	= 绘图环境
	返回值：
*****************************************************************************************/
void itobjAdmin::draw(CDC *pdc, CView* pView)
{
	if(!pdc)
		return;
	//mpobj->draw(pdc);
	// 绘图单元
	acm::CNPainterView *pview = dynamic_cast<acm::CNPainterView *>(pView);
	itobj *pobj = nullptr;
	for(itobj::PtrArrayType::size_type idx = 0; idx < m_vecItems.size(); ++idx)
	{
		pobj = m_vecItems[idx];
		pobj->draw(pdc);
		if(pview->m_bActive && !pdc->IsPrinting() && pview->isSelected(pobj))
			pobj->drawTracker(pdc, itobj::TSSelected,m_clrTracker);
	}
}
/*****************************************************************************************
	<< --- itobjAdmin::drawBackground	2013-12-02 --- >>
	说明：绘制工作区和纸张
	参数：
	pdc			= 绘图环境
	szWorker	= 工作台尺寸
	返回值：
*****************************************************************************************/
void itobjAdmin::drawBackground(CDC *pdc, const CSize &szWorker)
{
	if(!pdc)
		return;
	size szpaper = m_pobjPage->getPaperInfo().bsSize;
	rect rtworker/*, rtpaper*/;
	if(m_pobjPage->getPaperInfo().isLandscape())	// 默认是纵向
		std::swap(szpaper.width, szpaper.height);
	//rtworker.left = -(szWorker.cx-szpaper.cx)/2;
	//rtworker.right = (szWorker.cx+szpaper.cx)/2;
	//rtworker.top = (szWorker.cy+szpaper.cy)/2;
	//rtworker.bottom = -(szWorker.cy-szpaper.cy)/2;

	rtworker.x = -(szWorker.cx-szpaper.width)/2;
	rtworker.y = -(szWorker.cy-szpaper.height)/2;
	rtworker.width = szWorker.cx;
	rtworker.height = szWorker.cy;

	if(!pdc->IsPrinting())
	{
		if(m_pobjPage->supportMargins())
		{	// 原点(0,0)要内移
#if defined(WORK_AXIS_CAD)
			rtworker.offset(-m_pobjPage->getMarginLeft(), -m_pobjPage->getMarginBottom());
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			rtworker.offset(-m_pobjPage->getMarginLeft(), -m_pobjPage->getMarginTop());
#endif
		}
		if(m_pobjPage->supportHeaderFooter())
		{	// Y轴方向原点(0,0)继续内移
#if defined(WORK_AXIS_CAD)
			rtworker.offset(0, -m_pobjPage->getFooter().uiHeight);
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			rtworker.offset(0, -m_pobjPage->getHeader().uiHeight);
#endif
		}
		// 绘制工作台
		Gdiplus::Graphics grap(pdc->GetSafeHdc());
		rtworker.deflate(10,10);
		grap.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(m_pobjPage->getWorkerColor())), rtworker.toGdiplus());
	}

	// 绘制纸张信息
	m_pobjPage->draw(pdc);

}
/*****************************************************************************************
	<< --- itobjAdmin::prepareDC	2013-12-13 --- >>
	说明：定位绘图坐标系，绘图原点
	参数：
	pdc			= 绘图环境
	szWorker	= 工作台尺寸
	返回值：定位块光标
*****************************************************************************************/
void itobjAdmin::prepareDC(CDC* pdc, const CSize &szWorker)
{
	////1.SetWindowOrg(x, y) 是把设备坐标的原点(视口)映射到逻辑坐标的(X, Y)处 ，查看花布任意处
	////2.SetViewportOrg(x, y) 是把逻辑坐标的原点(窗口)映射到设备坐标的(X, Y)处
	if(!pdc)
		return;
	switch (m_beWorkaxis)
	{
	case BEWorkAxisNormal:
		{
		pdc->SetMapMode(MM_ISOTROPIC);

		long wpin = pdc->GetDeviceCaps(LOGPIXELSX);	// 屏幕水平方向每英寸像素点数	
		long hpin = pdc->GetDeviceCaps(LOGPIXELSY);	// 屏幕垂直方向每英寸像素点数	

		pdc->SetWindowExt(254, -254);		// 窗口的尺寸，逻辑坐标；默认时向右，向下递增，变负时就反方向了
		pdc->SetViewportExt(wpin, hpin);	// 视区的尺寸，设备坐标（像素）
		pdc->SetWindowOrg(m_pobjPage->getWindowOrg(szWorker, pdc->IsPrinting()).toMSPoint());
		}
		break;
	case BEWorkAxisDOC:
		{
		pdc->SetMapMode(MM_ISOTROPIC);

		long wpin = pdc->GetDeviceCaps(LOGPIXELSX);	// 屏幕水平方向每英寸像素点数	
		long hpin = pdc->GetDeviceCaps(LOGPIXELSY);	// 屏幕垂直方向每英寸像素点数	

		pdc->SetWindowExt(254, 254);		// 窗口的尺寸，逻辑坐标；默认时向右，向下递增，变负时就反方向了
		pdc->SetViewportExt(wpin, hpin);	// 视区的尺寸，设备坐标（像素）
		pdc->SetWindowOrg(m_pobjPage->getWindowOrg(szWorker, pdc->IsPrinting()).toMSPoint());
		}
		break;
	case BEWorkAxisCAD:
		pdc->SetMapMode(MM_TEXT);
		break;
	default:
		break;
	}
}

#endif


}