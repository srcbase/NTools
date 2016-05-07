#include "pages.h"
#include "AC/drawdata.h"
#include "AC/items/itobjProperty.h"
namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPage	2013-12-02 --- >>
//	绘制：工作区，纸张，纸张背景，页眉页脚等等
////////////////////////////////////////////////////////////////////////////////////////////////////////
_IMPLEMENT_ITOBJ_(itobjPage, itobj, BEDrawShapeTypePage, 0x1)
// itobjPage的属性默认值
#define _DEFAULT_ITEMPAGE_WORKERCOLOR_	0xFFEAEAEA	// 工作台背景色
#define _DEFAULT_ITEMPAGE_SHADOWCOLOR_	0xFF808080	// 页面阴影色
#define _DEFAULT_ITEMPAGE_GRIDCOLOR_	0xFFC0C0C0	// 页面阴影色
itobjPage::itobjPage(bool isLandscape, bool margins, bool headerfooter, bool grid)
	: itobj()
	//, m_wsPrinter(L"")
	, m_bsPaper(isLandscape)
	, m_bMargins(margins)
	, m_uiMarginTop(254),m_uiMarginBottom(254),m_uiMarginLeft(317),m_uiMarginRight(317)
	, m_bHeaderFooter(headerfooter)
	, m_bsHeader(150), m_bsFooter(175)
	, m_clrWorker(_DEFAULT_ITEMPAGE_WORKERCOLOR_), m_clrPaperShadow(_DEFAULT_ITEMPAGE_SHADOWCOLOR_)
	, m_bGrid(grid), m_bsGrid(BEPenStyleDot, 1, _DEFAULT_ITEMPAGE_GRIDCOLOR_)
{
	refreshPagePosition();
}
itobjPage::itobjPage(const itobjPage &page)
	: itobj(page)
	//, m_wsPrinter(L"")
	, m_bsPaper(page.m_bsPaper)
	//, m_bsBIN(page.m_bsBIN)
	, m_bMargins(page.m_bMargins)
	, m_uiMarginTop(page.m_uiMarginTop),m_uiMarginBottom(page.m_uiMarginBottom),m_uiMarginLeft(page.m_uiMarginLeft),m_uiMarginRight(page.m_uiMarginRight)
	, m_bHeaderFooter(page.m_bHeaderFooter)
	, m_bsHeader(page.m_bsHeader), m_bsFooter(page.m_bsFooter)
	, m_clrWorker(page.m_clrWorker), m_clrPaperShadow(page.m_clrPaperShadow)
	, m_bGrid(page.m_bGrid), m_bsGrid(page.m_bsGrid)
{
}
itobjPage& itobjPage::operator=(const itobjPage &page)
{
	if(this != &page)
	{
		itobj::operator=(page);
		//m_wsPrinter = page.m_wsPrinter;
		m_bsPaper = page.m_bsPaper;
		//m_bsBIN = page.m_bsBIN;

		m_bMargins = page.m_bMargins;
		m_uiMarginTop = page.m_uiMarginTop;
		m_uiMarginBottom = page.m_uiMarginBottom;
		m_uiMarginLeft = page.m_uiMarginLeft;
		m_uiMarginRight = page.m_uiMarginRight;

		m_bHeaderFooter = page.m_bHeaderFooter;
		m_bsHeader = page.m_bsHeader;
		m_bsFooter = page.m_bsFooter;

		m_clrWorker = page.m_clrWorker;
		m_clrPaperShadow = page.m_clrPaperShadow;
		m_bGrid = page.m_bGrid;
		m_bsGrid = page.m_bsGrid;

	}
	return *this;
}
itobjPage::~itobjPage()
{
#ifdef DEBUG
	DEBUG_LOG(L"free %s", description().c_str());
#endif
}
// 描述
inline std::wstring itobjPage::description(int indent) const
{
	std::wostringstream os;
	wchar_t buf[128] = {0};

	gtc::outClassBegin(os, L"itobjPage", indent);
	//gtc::outClassItem(os, L"打印机", m_wsPrinter, indent << 1);
	gtc::outClassItem(os, L"纸张", m_bsPaper.description(indent << 1), indent << 1);
	//gtc::outClassItem(os, L"纸槽", m_bsBIN.description(indent << 1), indent << 1);
	std::swprintf(buf, 128, L"%s (上：%d,下：%d,左：%d,右：%d)", m_bMargins?L"支持":L"不支持", 
		m_uiMarginTop, m_uiMarginBottom, m_uiMarginLeft, m_uiMarginRight);
	gtc::outClassItem(os, L"页边距", buf, indent << 1);
	std::swprintf(buf, 128, L"%s", m_bHeaderFooter?L"支持":L"不支持");
	gtc::outClassItem(os, L"页眉页脚", buf, indent << 1);
	gtc::outBaseClass(os, *(itobj*)this, indent << 1);
	gtc::outClassEnd(os, L"itobjPage", indent);
	return os.str();
}
/*****************************************************************************************
	<< --- itobj::clone	2013-11-29 --- >>
	说明：刷新信息，如：改变页面尺寸后就需要调用本函数
	参数：
	返回值：
*****************************************************************************************/
void itobjPage::refresh()
{
	size szPaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())	// 尺寸用纵向表示的。2=横向
		std::swap(szPaper.width, szPaper.height);

	m_bsPosition.setRect(point(0,0), szPaper);
	//m_bsPosition.normalizeRect();
}

/*****************************************************************************************
	<< --- itobj::getWindowOrg	2013-12-04 --- >>
	说明：把设备坐标的原点(视口)映射到逻辑坐标的(X, Y)处
	参数：
	bsWorker	= 工作台尺寸
	isprinter	= 是否是打印机
	返回值：
*****************************************************************************************/
point itobjPage::getWindowOrg(size bsWorker, bool isprinter)
{
	point ptorg(0,0);
	size szPaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())	// 尺寸用纵向表示的。2=横向
		std::swap(szPaper.width, szPaper.height);
	if(isprinter)
	{
		//rect rt(0,0,szPaper.width,szPaper.height);// = m_bsPosition;
		//ptorg.x = min(rt.left, rt.right);
		//ptorg.y = max(rt.top, rt.bottom);
		if(supportMargins())
		{	// 原点(0,0)要内移
#if defined(WORK_AXIS_CAD)
			ptorg.x -= m_uiMarginLeft;
			ptorg.y -= m_uiMarginBottom;
#else	//defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			ptorg.x -= m_uiMarginLeft;
			ptorg.y -= m_uiMarginBottom;
#endif
		}
		if(supportHeaderFooter())
		{	// Y轴方向原点(0,0)继续内移
#if defined(WORK_AXIS_CAD)
			ptorg.y -= m_bsFooter.uiHeight;
#else	//defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			ptorg.y -= m_bsHeader.uiHeight;
#endif
		}
	}
	else
	{
#if defined(WORK_AXIS_CAD)	// 往右、上递增；1逻辑单位等于0.1毫米
		ptorg.x = -(bsWorker.width-szPaper.width)/2;
		ptorg.y = (bsWorker.height+szPaper.height)/2;
#elif defined(WORK_AXIS_DOC)	// 往右、下递增；1逻辑单位等于0.1毫米
		ptorg.x = -(bsWorker.width-szPaper.width)/2;
		ptorg.y = -(bsWorker.height-szPaper.height)/2;
#endif
		if(supportMargins())
		{	// 原点(0,0)要内移
#if defined(WORK_AXIS_CAD)
			ptorg.x -= m_uiMarginLeft;
			ptorg.y -= m_uiMarginBottom;
#else	//defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			ptorg.x -= m_uiMarginLeft;
			ptorg.y -= m_uiMarginBottom;
#endif
		}
		if(supportHeaderFooter())
		{	// Y轴方向原点(0,0)继续内移
#if defined(WORK_AXIS_CAD)
			ptorg.y -= m_bsFooter.uiHeight;
#else	//defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			ptorg.y -= m_bsHeader.uiHeight;
#endif
		}
	}

	return ptorg;
}
// 纸张区域，即：m_bsPosition
inline void itobjPage::refreshPagePosition()
{
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	m_bsPosition.setRect(point(0,0), szpaper);
	//m_bsPosition.normalizeRect();
	if(m_bMargins)
	{
		szpaper.width -= m_uiMarginLeft + m_uiMarginRight;
		szpaper.height -= m_uiMarginTop + m_uiMarginBottom;
	}

	if(m_bHeaderFooter)
	{
		szpaper.height -= m_bsHeader.uiHeight + m_bsFooter.uiHeight;
	}
	rect area;
#if defined(WORK_AXIS_CAD)
	if(m_bMargins)
		m_bsPosition.offset(-m_uiMarginLeft, m_uiMarginBottom);
	if(m_bHeaderFooter)
		m_bsPosition.offset(0, m_bsFooter.uiHeight);
	//area.left = 0;
	//area.right = szpaper.cx;
	//area.top = szpaper.cy;
	//area.bottom = 0;
	
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	if(m_bMargins)
		m_bsPosition.offset(-m_uiMarginLeft, -m_uiMarginBottom);
	if(m_bHeaderFooter)
		m_bsPosition.offset(0, -m_bsFooter.uiHeight);
	//area.left = 0;
	//area.right = szpaper.cx;
	//area.top = 0;
	//area.bottom = szpaper.cy;
#endif
}


inline rect itobjPage::areaPaperWork() const
{
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	if(m_bMargins)
	{
		szpaper.width -= m_uiMarginLeft + m_uiMarginRight;
		szpaper.height -= m_uiMarginTop + m_uiMarginBottom;
	}

	if(m_bHeaderFooter)
	{
		szpaper.height -= m_bsHeader.uiHeight + m_bsFooter.uiHeight;
	}
	rect area;

	area.setRect(point(0,0), szpaper);

	return area;
}

inline rect itobjPage::areaMarginTop() const 
{
	rect area;
	if(!m_bMargins)
		return area;
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	int32_t header = m_bHeaderFooter?m_bsHeader.uiHeight:0;
	int32_t footer = m_bHeaderFooter?m_bsFooter.uiHeight:0;
#if defined(WORK_AXIS_CAD)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = szpaper.cy - m_uiMarginBottom - footer;
	//area.bottom = area.top - m_uiMarginTop;
	area.x =0;
	area.y = szpaper.height - m_uiMarginBottom - footer;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_uiMarginTop;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = -m_uiMarginTop;
	//area.bottom = 0;
	area.x = 0;
	area.y = -m_uiMarginTop;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_uiMarginTop;
#endif
	return area;
}
inline rect itobjPage::areaMarginBottom() const
{
	rect area;
	if(!m_bMargins)
		return area;
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	int32_t header = m_bHeaderFooter?m_bsHeader.uiHeight:0;
	int32_t footer = m_bHeaderFooter?m_bsFooter.uiHeight:0;
#if defined(WORK_AXIS_CAD)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = -footer;
	//area.bottom = area.top-m_uiMarginBottom;
	area.x = 0;
	area.y = -footer-m_uiMarginBottom;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_uiMarginBottom;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = szpaper.cy - m_uiMarginTop - m_uiMarginBottom;
	//area.bottom = szpaper.cy - m_uiMarginTop;
	area.x = 0;
	area.y = szpaper.height - m_uiMarginTop - m_uiMarginBottom;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_uiMarginBottom;
#endif
	return area;
}
rect itobjPage::areaMarginLeft() const
{
	rect area;
	if(!m_bMargins)
		return area;
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	int32_t header = m_bHeaderFooter?m_bsHeader.uiHeight:0;
	int32_t footer = m_bHeaderFooter?m_bsFooter.uiHeight:0;
#if defined(WORK_AXIS_CAD)
	//area.left = -m_uiMarginLeft;
	//area.right = 0;
	//area.top = szpaper.cy - m_uiMarginBottom - m_uiMarginTop - footer - header;
	//area.bottom = 0;
	area.x = -m_uiMarginRight;
	area.y = 0;
	area.width = m_uiMarginLeft;
	area.height = szpaper.height - m_uiMarginBottom - m_uiMarginTop - footer - header;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//area.left = -m_uiMarginLeft;
	//area.right = 0;
	//area.top = 0;
	//area.bottom = szpaper.cy - m_uiMarginTop - m_uiMarginBottom - footer - header;
	area.x = -m_uiMarginRight;
	area.y = 0;
	area.width = m_uiMarginLeft;
	area.height = szpaper.height - m_uiMarginBottom - m_uiMarginTop - footer - header;
#endif
	return area;
}

inline rect itobjPage::areaMarginRight() const
{
	rect area;
	if(!m_bMargins)
		return area;
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	int32_t header = m_bHeaderFooter?m_bsHeader.uiHeight:0;
	int32_t footer = m_bHeaderFooter?m_bsFooter.uiHeight:0;
#if defined(WORK_AXIS_CAD)
	//area.left = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.right = szpaper.cx - m_uiMarginLeft;
	//area.top = szpaper.cy - m_uiMarginBottom - m_uiMarginTop - footer - header;
	//area.bottom = 0;
	area.x = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.y = 0;
	area.width = m_uiMarginRight;
	area.height = szpaper.height - m_uiMarginBottom - m_uiMarginTop - footer - header;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//area.left = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.right = szpaper.cx - m_uiMarginLeft;
	//area.top = 0;
	//area.bottom = szpaper.cy - m_uiMarginTop - m_uiMarginBottom - footer - header;
	area.x = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.y = 0;
	area.width = m_uiMarginRight;
	area.height = szpaper.height - m_uiMarginBottom - m_uiMarginTop - footer - header;
#endif
	return area;
}
rect itobjPage::areaHeader() const
{
	rect area;
	if(!m_bHeaderFooter)
		return area;
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	int32_t margtop = m_bMargins?m_uiMarginTop:0;
	int32_t margbottom = m_bMargins?m_uiMarginBottom:0;
#if defined(WORK_AXIS_CAD)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = szpaper.cy - m_uiMarginBottom - m_bsFooter.uiHeight-margtop;
	//area.bottom = area.top - m_bsHeader.uiHeight;
	area.x = 0;
	area.y = szpaper.height - m_uiMarginBottom - m_bsFooter.uiHeight-margtop;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_bsHeader.uiHeight;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = -m_uiMarginTop;
	//area.bottom = 0;
	area.x = 0;
	area.y = -m_bsHeader.uiHeight;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_bsHeader.uiHeight;
#endif
	return area;
}

rect itobjPage::areaFooter() const
{
	rect area;
	if(!m_bHeaderFooter)
		return area;
	size szpaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())
		std::swap(szpaper.width, szpaper.height);
	int32_t margtop = m_bMargins?m_uiMarginTop:0;
	int32_t margbottom = m_bMargins?m_uiMarginBottom:0;
#if defined(WORK_AXIS_CAD)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = 0;
	//area.bottom = area.top-m_bsFooter.uiHeight;
	area.x = 0;
	area.y = -m_bsFooter.uiHeight;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_bsFooter.uiHeight;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//area.left = 0;
	//area.right = szpaper.cx - m_uiMarginLeft - m_uiMarginRight;
	//area.top = szpaper.cy - m_uiMarginTop - m_uiMarginBottom;
	//area.bottom = szpaper.cy - m_uiMarginTop;
	area.x = 0;
	area.y = -m_bsFooter.uiHeight;
	area.width = szpaper.width - m_uiMarginLeft - m_uiMarginRight;
	area.height = m_bsFooter.uiHeight;
#endif
	return area;
}


#if defined(_WIN32)
void itobjPage::draw(CDC *pdc)
{
	if(!pdc)
		return;
	Gdiplus::Graphics grap(pdc->GetSafeHdc());
	// 绘制纸张、纸轮廓、参考线等
	size szPaper = m_bsPaper.bsSize;
	if(m_bsPaper.isLandscape())	// 尺寸用纵向表示的。2=横向
		std::swap(szPaper.width, szPaper.height);
	rect rtPaper(0,0,szPaper.width, szPaper.height);
	if(!pdc->IsPrinting())
	{
		if(supportMargins())
		{	// 原点(0,0)要内移
#if defined(WORK_AXIS_CAD)
			rtPaper.offset(-m_uiMarginLeft, -m_uiMarginBottom);
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			rtPaper.offset(-m_uiMarginLeft, -m_uiMarginTop);
#endif
		}
		if(supportHeaderFooter())
		{	// Y轴方向原点(0,0)继续内移
#if defined(WORK_AXIS_CAD)
			rtPaper.offset(0, -m_bsFooter.uiHeight);
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
			rtPaper.offset(0, -m_bsHeader.uiHeight);
#endif
		}
		// 画纸轮廓
		int32_t offy = 0;
#if defined(WORK_AXIS_CAD)
		offy = -10;
#else	// defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		offy = 10;
#endif
		rtPaper.offset(10, offy);
		grap.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(m_clrPaperShadow)), rtPaper.toGdiplus());
		// 画纸
		rtPaper.offset(-10, -offy);
		grap.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::White), rtPaper.toGdiplus());
		// 画参考线
		if(supportGrid())
		{
		}
		// 绘制页眉、页脚
		if(supportHeaderFooter())
		{
		}
	}
	else
	{

	}

}
/*****************************************************************************************
	<< --- itobjPage::hitTest	2013-12-09 --- >>
	说明：测试点是否在对象或定位块上
	参数：
	point		= 被测试点（逻辑坐标）
	pView		= 绘图对象所在视图
	bSelected	= true时定位块，否则测试对象
	返回值：
		= 0；未选中
		> 0；定位块索引
*****************************************************************************************/
int32_t itobjPage::hitTest(CPoint point, acm::CNPainterView *pView, bool bSelected)
{
	return itobj::hitTest(point, pView, bSelected);
}
/*****************************************************************************************
	<< --- itobjPage::drawTracker	2013-12-09 --- >>
	说明：绘制定位块所在区域
	参数：
	vpdc		= 绘图环境
	vstate		= 定位块的状态
	vbkcolor	= 块的填充色
	返回值：
*****************************************************************************************/
void itobjPage::drawTracker(CDC* pdc, BETrackerState vstate, uint32_t vbkcolor)
{
	itobj::drawTracker(pdc, vstate, vbkcolor);
}
/*****************************************************************************************
	<< --- itobjPage::allocPropertySectionArray	2013-12-06 --- >>
	说明：分配对象属性
	参数：
	返回值：
*****************************************************************************************/
std::size_t itobjPage::allocPropertySectionArray(itobj* pitem, bool isShare)
{
	itobjPropSection::PtrArrayType &vecSection = pitem->getPropertys();
	if(vecSection.size() > 0) pitem->freePropertys();
	// 
	itobjPropSection *psec = nullptr, *psubsec = nullptr, *psubsec1 = nullptr;
	entry::PtrArrayType vec;
	itobjPage *ppage = dynamic_cast<itobjPage *>(pitem);
	if(ppage && (psec = new itobjPropSection(L"页面样式")) != nullptr) 
	{
		psec->addProperty(new itobjPropColor(L"工作台背景色", ppage->m_clrWorker, _DEFAULT_ITEMPAGE_WORKERCOLOR_, L"工作台背景色。", BEDrawShapeTypePage,BEPagePropWorkercolor)); 
		psec->addProperty(new itobjPropColor(L"页面阴影", ppage->m_clrPaperShadow, _DEFAULT_ITEMPAGE_SHADOWCOLOR_, L"页面阴影颜色，用来突出显示页面。", BEDrawShapeTypePage,BEPagePropPapershadow)); 
		// 
		psec->addProperty(new itobjPropBoolean(L"网格", ppage->m_bGrid, L"是否显示网格，可作为绘图的参考线，便于绘制操作。", BEDrawShapeTypePage, BEPagePropGrid));
		psubsec = new itobjPropSection(L"网格样式", nullptr, true);
		psubsec->addProperty(new itobjPropColor(L"颜色", ppage->m_bsGrid.color, _DEFAULT_ITEMPAGE_GRIDCOLOR_, L"", BEDrawShapeTypePage, BEPagePropGridcolor));
		vec.assign(std::begin(gc_tbPenStyle), std::end(gc_tbPenStyle));
		psubsec->addProperty(new itobjPropCombo(L"样式", ppage->m_bsGrid.style, vec, false, L"", BEDrawShapeTypePage, BEPagePropGridstyle));
		psec->addProperty(psubsec);
		//
		psubsec = new itobjPropSection(L"纸张", nullptr, true);
		psubsec->addProperty(new itobjPropEdit(L"尺寸", ppage->m_bsPaper.wsName.c_str(), false, L"该大小可以在“页面设置”对话框中更改。", BEDrawShapeTypePage, BEPagePropPaperName));
		vec.assign(std::begin(gc_tbPaperOrientation), std::end(gc_tbPaperOrientation));
		psubsec->addProperty(new itobjPropCombo(L"方向", ppage->m_bsPaper.iOrientation, vec, false, L"", BEDrawShapeTypePage, BEPagePropPaperOrientation));
		psec->addProperty(psubsec);
		// 
		int32_t maxw = ppage->m_bsPaper.iOrientation == 1?ppage->m_bsPaper.bsSize.width/2:ppage->m_bsPaper.bsSize.height/2;
		int32_t maxh = ppage->m_bsPaper.iOrientation == 1?ppage->m_bsPaper.bsSize.height/2:ppage->m_bsPaper.bsSize.width/2;
		psec->addProperty(new itobjPropBoolean(L"页边距", ppage->m_bMargins, L"是否启用页边距，启用页边距后会改变绘图起点(0,0)位置。", BEDrawShapeTypePage, BEPagePropMargins));
		psubsec = new itobjPropSection(L"页边距尺寸", nullptr, true);
		psubsec->addProperty(new itobjPropSpin(L"左边距", ppage->m_uiMarginLeft, 0, maxw, L"",		BEDrawShapeTypePage,BEPagePropMarginleft));
		psubsec->addProperty(new itobjPropSpin(L"右边距", ppage->m_uiMarginRight, 0, maxw, L"",		BEDrawShapeTypePage,BEPagePropMarginright));
		psubsec->addProperty(new itobjPropSpin(L"上边距", ppage->m_uiMarginTop, 0, maxh, L"",		BEDrawShapeTypePage,BEPagePropMargintop));
		psubsec->addProperty(new itobjPropSpin(L"下边距", ppage->m_uiMarginBottom, 0, maxh, L"",		BEDrawShapeTypePage,BEPagePropMarginbottom));
		psec->addProperty(psubsec);
		// 
		maxh = ppage->m_bsPaper.iOrientation == 1?ppage->m_bsPaper.bsSize.height/4:ppage->m_bsPaper.bsSize.width/4;
		psec->addProperty(new itobjPropBoolean(L"页眉页脚", ppage->m_bHeaderFooter, L"是否启用页眉页脚，启用后会改变绘图起点(0,0)位置。", BEDrawShapeTypePage, BEPagePropHeaderfooter));
		psubsec = new itobjPropSection(L"页眉", nullptr, true);
		psubsec->addProperty(new itobjPropSpin(L"高度", ppage->m_bsHeader.uiHeight, 0, maxh, L"",		BEDrawShapeTypePage, BEPagePropHeaderHeight));
		psubsec->addProperty(new itobjPropEdit(L"标题内容", ppage->m_bsHeader.wsTitle.c_str(), true, L"",	BEDrawShapeTypePage, BEPagePropHeaderTitle));
		vec.assign(std::begin(gc_tbAlignMode), std::end(gc_tbAlignMode));
		psubsec->addProperty(new itobjPropCombo(L"标题对齐", ppage->m_bsHeader.beAlign, vec, false, L"",			BEDrawShapeTypePage, BEPagePropHeaderAlign));
		psubsec->addProperty(new itobjPropFont(L"标题字体", ppage->m_bsHeader.bsFont, L"",				BEDrawShapeTypePage, BEPagePropHeaderFont));
		vec.assign(std::begin(gc_tbPageNoStyle), std::end(gc_tbPageNoStyle));
		psubsec->addProperty(new itobjPropCombo(L"页码样式", ppage->m_bsHeader.iPageStyle, vec, false, L"",		BEDrawShapeTypePage, BEPagePropHeaderPagestyle));
		vec.assign(std::begin(gc_tbAlignMode), std::end(gc_tbAlignMode));
		psubsec->addProperty(new itobjPropCombo(L"页码对齐", ppage->m_bsHeader.bePageAlign, vec, false, L"",		BEDrawShapeTypePage, BEPagePropHeaderPagealign));
		psubsec->addProperty(new itobjPropFont(L"页码字体", ppage->m_bsHeader.bsPageFont, L"",			BEDrawShapeTypePage, BEPagePropHeaderPagefont));
		psubsec->addProperty(new itobjPropFile(L"图标", nullptr, L".jpeg", gc_wsImageFilter, true, L"",	BEDrawShapeTypePage, BEPagePropHeaderLogo));
		psubsec1 = new itobjPropSection(L"图标位置", nullptr, true);
		maxw = (ppage->m_bsPaper.iOrientation == 1?ppage->m_bsPaper.bsSize.width:ppage->m_bsPaper.bsSize.height)-ppage->m_uiMarginLeft-ppage->m_uiMarginRight;
		psubsec1->addProperty(new itobjPropSpin(L"左", ppage->m_bsHeader.bsLogo.x, 0, ppage->m_bsHeader.uiHeight, L"",	BEDrawShapeTypePage,BEPagePropHeaderLogoposX));
		psubsec1->addProperty(new itobjPropSpin(L"上", ppage->m_bsHeader.bsLogo.y, 0, ppage->m_bsHeader.uiHeight, L"",	BEDrawShapeTypePage,BEPagePropHeaderLogoposY));
		psubsec1->addProperty(new itobjPropSpin(L"宽", ppage->m_bsHeader.bsLogo.width, 0, maxw, L"",						BEDrawShapeTypePage,BEPagePropHeaderLogoposWidth));
		psubsec1->addProperty(new itobjPropSpin(L"高", ppage->m_bsHeader.bsLogo.height, 0, maxw, L"",					BEDrawShapeTypePage,BEPagePropHeaderLogoposHeight));
		psubsec->addProperty(psubsec1);
		psec->addProperty(psubsec);
		//////////////
		psubsec = new itobjPropSection(L"页脚", nullptr, true);
		psubsec->addProperty(new itobjPropSpin(L"高度", ppage->m_bsFooter.uiHeight, 0, maxh, L"",		BEDrawShapeTypePage, BEPagePropFooterHeight));
		psubsec->addProperty(new itobjPropEdit(L"标题内容", ppage->m_bsFooter.wsTitle.c_str(), true, L"",	BEDrawShapeTypePage, BEPagePropFooterTitle));
		vec.assign(std::begin(gc_tbAlignMode), std::end(gc_tbAlignMode));
		psubsec->addProperty(new itobjPropCombo(L"标题对齐", ppage->m_bsFooter.beAlign, vec, false, L"",			BEDrawShapeTypePage, BEPagePropFooterAlign));
		psubsec->addProperty(new itobjPropFont(L"标题字体", ppage->m_bsFooter.bsFont, L"",				BEDrawShapeTypePage, BEPagePropFooterFont));
		vec.assign(std::begin(gc_tbPageNoStyle), std::end(gc_tbPageNoStyle));
		psubsec->addProperty(new itobjPropCombo(L"页码样式", ppage->m_bsFooter.iPageStyle, vec, false, L"",		BEDrawShapeTypePage, BEPagePropFooterPagestyle));
		vec.assign(std::begin(gc_tbAlignMode), std::end(gc_tbAlignMode));
		psubsec->addProperty(new itobjPropCombo(L"页码对齐", ppage->m_bsFooter.bePageAlign, vec, false, L"",		BEDrawShapeTypePage, BEPagePropFooterPagealign));
		psubsec->addProperty(new itobjPropFont(L"页码字体", ppage->m_bsFooter.bsPageFont, L"",			BEDrawShapeTypePage, BEPagePropFooterPagefont));
		psubsec->addProperty(new itobjPropFile(L"图标", nullptr, L".jpeg", gc_wsImageFilter, true, L"",	BEDrawShapeTypePage, BEPagePropFooterLogo));
		psubsec1 = new itobjPropSection(L"图标位置", nullptr, true);
		maxw = (ppage->m_bsPaper.iOrientation == 1?ppage->m_bsPaper.bsSize.width:ppage->m_bsPaper.bsSize.height)-ppage->m_uiMarginLeft-ppage->m_uiMarginRight;
		psubsec1->addProperty(new itobjPropSpin(L"左", ppage->m_bsFooter.bsLogo.x, 0, ppage->m_bsFooter.uiHeight, L"",	BEDrawShapeTypePage,BEPagePropFooterLogoposX));
		psubsec1->addProperty(new itobjPropSpin(L"上", ppage->m_bsFooter.bsLogo.y, 0, ppage->m_bsFooter.uiHeight, L"",	BEDrawShapeTypePage,BEPagePropFooterLogoposY));
		psubsec1->addProperty(new itobjPropSpin(L"宽", ppage->m_bsFooter.bsLogo.width, 0, maxw, L"",						BEDrawShapeTypePage,BEPagePropFooterLogoposWidth));
		psubsec1->addProperty(new itobjPropSpin(L"高", ppage->m_bsFooter.bsLogo.height, 0, maxw, L"",					BEDrawShapeTypePage,BEPagePropFooterLogoposHeight));
		psubsec->addProperty(psubsec1);
		psec->addProperty(psubsec);

		// 
		vecSection.push_back(psec);
	}
	return vecSection.size();
}

/*****************************************************************************************
	<< --- itobjPage::updatePropertyValue	2013-12-06 --- >>
	说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
	参数：
	pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
	返回值：
*****************************************************************************************/
bool itobjPage::updatePropertyValue(itobjProp *pprop)
{
	if(pprop && __super::updatePropertyValue(pprop))
		return true;
	if(!pprop || pprop->getTypekey() != getType())
		return false;
	bool exists = true;


	return exists;
}
#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPageAxis	2014-01-03 --- >>
//	带坐标系的页
////////////////////////////////////////////////////////////////////////////////////////////////////////
_IMPLEMENT_ITOBJ_(itobjPageAxis, itobjPage, BEDrawShapeTypePageAxis, 0x1)
// 以下均为逻辑单位
int32_t itobjPageAxis::mc_iLineThick			= 6;		// 轴线粗
int32_t itobjPageAxis::mc_iScaleLength			= 15;		// 轴刻度长
int32_t itobjPageAxis::mc_iScaleShortLength		= 10;		// 轴刻度长
int32_t itobjPageAxis::mc_iAxisArrow			= 6;		// 超出轴部分，用来绘制轴的箭头大小；线帽单位：像素
int32_t itobjPageAxis::mc_iScaleInfoSpace		= 15;		// 刻度值区域与刻标的间距
int32_t itobjPageAxis::mc_iScaleInfoWidth		= 200;		// 刻度值区域的宽度
int32_t itobjPageAxis::mc_iScaleInfoHeight		= 100;		// 刻度值区域的高度
int32_t itobjPageAxis::mc_iAxisInfoSpace		= 80;		// 轴信息与刻度值区域的间距
int32_t itobjPageAxis::mc_iAxisInfoWidth		= 600;		// 轴信息的宽度
int32_t itobjPageAxis::mc_iAxisInfoHeight		= 60;		// 轴信息的高度
int32_t itobjPageAxis::mc_iAxisNameWidth		= 200;		// 轴名称的宽度
int32_t itobjPageAxis::mc_iAxisNameHeight		= 100;		// 轴名称的高度


itobjPageAxis::itobjPageAxis()
	: itobjPage(true, true, true, false)
	, m_arrAxis(2, nullptr)/*二维坐标x,y*/
	, m_arrInfo(6, nullptr)/*纸面分为6个信息区和1个工作区*/
{
	// 轴信息
	// x轴，分钟，每刻度=1分钟
	m_arrAxis[BEAxisFlagX] = new coordinateAxis(BEUnitTimeSecond, 0, 20);
	// y轴，克，每刻度=5克
	m_arrAxis[BEAxisFlagY] = new coordinateAxis(BEUnitWeightGram, 0, 5);
	int32_t offy = 0, offx = 0;
	// 信息1，y轴名称
	itobj* pobj = nullptr;
	//const rect rtwork = areaPaperWork();
	rect rt, rc = areaMarginLeft();
	rt = rc;
	rt.width = mc_iAxisNameWidth;
	rt.height = mc_iAxisNameHeight;
#if defined(WORK_AXIS_CAD)
	offy = rc.height - mc_iAxisNameHeight;
	offx = rc.width-mc_iScaleInfoSpace-mc_iAxisArrow-mc_iAxisNameWidth;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	offy = mc_iAxisNameHeight;
	offx = rc.width-mc_iScaleInfoSpace-mc_iAxisArrow-mc_iAxisNameWidth;
#endif
	rt.offset(offx, offy);
	pobj = new itobj(rt);
	pobj->setAlignMode(BEAlignModeRightTop);
	pobj->setText(m_arrAxis[BEAxisFlagY]->getUnitName());
	pobj->getFont().setFontSize(BEFontSizeFiveSmall);
	pobj->setBorderStyle(BEBorderStyleNone);
	//pobj->setBackColor(0xFF0000ff);
	//pobj->getBrush().style = BEBrushStyleSolid;
	m_arrInfo[BEAxisInfoYName] = pobj;
	// 信息2，x轴名称
	rt = rc = areaFooter();//areaMarginBottom();
	rt.width = mc_iAxisNameWidth;
	rt.height = mc_iAxisNameHeight;
#if defined(WORK_AXIS_CAD)
	offx = rc.width-mc_iAxisNameWidth;
	offy = rc.height -mc_iScaleInfoSpace-mc_iAxisArrow - mc_iAxisNameHeight;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	offx = rc.width-mc_iAxisNameWidth;
	offy = mc_iScaleInfoSpace+mc_iAxisArrow;
#endif
	rt.offset(offx, offy);
	pobj = new itobj(rt);
	pobj->setAlignMode(BEAlignModeRightTop);
	pobj->setText(m_arrAxis[BEAxisFlagX]->getUnitName());
	pobj->getFont().setFontSize(BEFontSizeFiveSmall);
	pobj->setBorderStyle(BEBorderStyleNone);
	pobj->getPen().style = gtc::BEPenStyleDashDotDot;
	pobj->setBorderStyle(BEBorderStyleRectangle);
	//pobj->setBackColor(0xFF0000ff);
	//pobj->getBrush().style = BEBrushStyleSolid;
	m_arrInfo[BEAxisInfoXName] = pobj;
	// 信息3，y轴标题
	rt = rc = areaMarginLeft();
	rt.width = rc.width - mc_iAxisInfoSpace*2;
	rt.height = rc.height - mc_iAxisNameHeight*2;
#if defined(WORK_AXIS_CAD)
	offx = mc_iAxisInfoSpace;
	offy = mc_iAxisNameHeight;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	offx = mc_iAxisInfoSpace;
	offy = mc_iAxisNameHeight;
#endif
	rt.offset(offx, offy);
	pobj = new itobj(rt);
	pobj->setText(L"");//称重测量值
	pobj->setAlignMode(BEAlignModeCenterCenter);
	pobj->getFont().iEscapement = 900;
	pobj->setBorderStyle(BEBorderStyleNone);
	//pobj->setBackColor(0xFF00ff00);
	//pobj->getBrush().style = BEBrushStyleSolid;
	m_arrInfo[BEAxisInfoYTitle] = pobj;
	// 信息4，x轴标题
	rt = rc = areaFooter();// areaMarginBottom();
	rt.width = rc.width - mc_iScaleInfoWidth*2;
	rt.height = rc.height - mc_iAxisInfoSpace;
#if defined(WORK_AXIS_CAD)
	offx = mc_iScaleInfoWidth;
	offy = 0;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	offx = mc_iScaleInfoWidth;
	offy = 0;
#endif
	rt.offset(offx, offy);
	pobj = new itobj(rt);
	pobj->setAlignMode(BEAlignModeCenterCenter);
	pobj->setText(L"");//称重测试
	pobj->setBorderStyle(BEBorderStyleNone);
	//pobj->setBackColor(0xFF00ff00);
	//pobj->getBrush().style = BEBrushStyleSolid;
	m_arrInfo[BEAxisInfoXTitle] = pobj;
	// 信息5，坐标系标题
	rt = areaHeader();// areaMarginTop();
	pobj = new itobj(rt);
	pobj->setAlignMode(BEAlignModeCenterCenter);
	pobj->setText(L"标题");
	pobj->setBorderStyle(BEBorderStyleNone);
	//pobj->setBackColor(0xFFff0000);
	//pobj->getBrush().style = BEBrushStyleSolid;
	m_arrInfo[BEAxisInfoTitle] = pobj;
	// 信息6，坐标系右侧标题
	rt = areaMarginRight();
	rt.width -= mc_iAxisNameHeight;
	pobj = new itobj(rt);
	pobj->setAlignMode(BEAlignModeCenterTop);
	pobj->setText(L"");//图例
	pobj->setBorderStyle(BEBorderStyleNone);
	pobj->getFont().iEscapement = 900;
	//pobj->setBackColor(0xFFff0000);
	//pobj->getBrush().style = BEBrushStyleSolid;
	m_arrInfo[BEAxisInfoRightTitle] = pobj;

	// 设定坐标轴对应的范围
	rt = areaPaperWork();
	m_arrAxis[BEAxisFlagX]->setDevRange(rt.width);
	m_arrAxis[BEAxisFlagY]->setDevRange(rt.height);
}

itobjPageAxis::itobjPageAxis(const itobjPageAxis &page)
	: itobjPage(page)
{

}

itobjPageAxis& itobjPageAxis::operator=(const itobjPageAxis &page)
{
	if(this != &page)
	{
		itobjPage::operator=(page);
	}
	return *this;
}

itobjPageAxis::~itobjPageAxis()
{
	DEBUG_LOG(L"free %s", description().c_str());
}
void itobjPageAxis::logicToDoc(pointf &pt) {
	float devX = 0.0f, devY = 0.0f;
	logicToDoc(pt.x, pt.y, devX, devY);
	pt.x = devX;
	pt.y = devY;
}
void itobjPageAxis::logicToDoc(rectf &rc)
{
	pointf ptLT(rc.getLeft(), rc.getTop());
	pointf ptRB(rc.getRight(), rc.getBottom());
	logicToDoc(ptLT);
	logicToDoc(ptRB);
	rc.x = min(ptLT.x, ptRB.x);
	rc.y = min(ptLT.y, ptRB.y);
	rc.width = abs(ptLT.x - ptRB.x);
	rc.height = abs(ptLT.y - ptRB.y);
}
void itobjPageAxis::logicToDoc(const float &msvX, const float &msvY, float &devX, float &devY, bool reloadAxis /*= false*/) {
	if (reloadAxis)
	{
		rect rcWorker = areaPaperWork();
		m_arrAxis[BEAxisFlagX]->setDevRange(rcWorker.width);
		m_arrAxis[BEAxisFlagY]->setDevRange(rcWorker.height);
	}
	devX = m_arrAxis[BEAxisFlagX]->toClient(msvX);
	devY = m_arrAxis[BEAxisFlagY]->toClient(msvY);
	//static coordinateAxis *paxisX = nullptr;
	//static coordinateAxis *paxisY = nullptr;
	//static float devPreLogicX = 0.0f;		// 1分钟-->b设备坐标值
	//static float devPreLogicY = 0.0f;
	//static float originOffsetX = 0.0f;
	//static float originOffsetY = 0.0f;

	//if (reloadAxis || paxisX == nullptr || paxisY == nullptr)
	//{
	//	paxisX = getAxis(itobjPageAxis::BEAxisFlagX);
	//	paxisY = getAxis(itobjPageAxis::BEAxisFlagY);
	//	// 
	//	rect rcWorker = areaPaperWork();
	//	devPreLogicX = rcWorker.width/(paxisX->iPreScale*11);		// 1分钟-->b设备坐标值
	//	devPreLogicY = rcWorker.height/(paxisY->iPreScale*11);
	//	// 
	//	originOffsetX = paxisX->iOrigin*devPreLogicX;
	//	originOffsetY = paxisY->iOrigin*devPreLogicY;
	//}
	//// 转换到坐标轴值
	//float axisValueX = paxisX->scalingFactor() * msvX;		// 毫秒-->x分钟
	//float axisValueY = paxisY->scalingFactor() * msvY;
	//// 坐标轴值所占有设备坐标值量
	//devX = axisValueX*devPreLogicX;	// x分钟-->y个像素
	//devY = axisValueY*devPreLogicY;
	//// 相对原点移动
	//devX -= originOffsetX;
	//devY -= originOffsetY;
}
// 描述
std::wstring itobjPageAxis::description(int indent) const
{
	std::wostringstream os;
	wchar_t buf[128] = {0};

	gtc::outClassBegin(os, L"itobjPageAxis", indent);
	////gtc::outClassItem(os, L"打印机", m_wsPrinter, indent << 1);
	//gtc::outClassItem(os, L"纸张", m_bsPaper.description(indent << 1), indent << 1);
	////gtc::outClassItem(os, L"纸槽", m_bsBIN.description(indent << 1), indent << 1);
	//std::swprintf(buf, 128, L"%s (上：%d,下：%d,左：%d,右：%d)", m_bMargins?L"支持":L"不支持", 
	//	m_uiMarginTop, m_uiMarginBottom, m_uiMarginLeft, m_uiMarginRight);
	//gtc::outClassItem(os, L"页边距", buf, indent << 1);
	//std::swprintf(buf, 128, L"%s", m_bHeaderFooter?L"支持":L"不支持");
	//gtc::outClassItem(os, L"页眉页脚", buf, indent << 1);
	gtc::outBaseClass(os, *(itobjPage*)this, indent << 1);
	gtc::outClassEnd(os, L"itobjPageAxis", indent);
	return os.str();
}
#if defined(_WIN32)
void itobjPageAxis::draw(CDC *pdc)
{
	if(!pdc)
		return;
	itobjPage::draw(pdc);
	// 绘制坐标轴
	rect rtworker = areaPaperWork();
	if(!rtworker.isEmptyArea())
	{
		Gdiplus::Graphics grap(pdc->GetSafeHdc());
		grap.SetPageUnit(Gdiplus::UnitPixel);
		Gdiplus::Pen pen(0xff000000, mc_iLineThick);
		Gdiplus::GraphicsPath grapPath;
		grapPath.AddLine(0,0, mc_iAxisArrow/3, -mc_iAxisArrow);
		grapPath.AddLine(0,0, -mc_iAxisArrow/3, -mc_iAxisArrow);
		Gdiplus::CustomLineCap cap(nullptr, &grapPath);
		pen.SetCustomEndCap(&cap);
		// x,y轴的分割数，如：11
		const int32_t iScalesX = getAxis(BEAxisFlagX)->getScales();
		const int32_t iScalesY = getAxis(BEAxisFlagY)->getScales();
		// Y轴
		grap.DrawLine(&pen, rtworker.x, rtworker.y, rtworker.x, rtworker.y+rtworker.height);
		// X轴
		grap.DrawLine(&pen, rtworker.x, rtworker.y, rtworker.x+rtworker.width, rtworker.y);
		// Y轴刻度
		Gdiplus::Pen penScale(0xff000000, mc_iLineThick);
		const int32_t infoLen = 10;
		wchar_t info[infoLen] = {0};
		rect rtinfo;
		const int32_t scaleY = rtworker.height/iScalesY;
		int32_t scale = scaleY;
		//LOGFONT lgfont = {0};
		//m_arrInfo[BEAxisInfoYName]->getFont().toMSFont(&lgfont);
		//Gdiplus::Font fonty(pdc->GetSafeHdc(), &lgfont);
		//m_arrInfo[BEAxisInfoXName]->getFont().toMSFont(&lgfont);
		//Gdiplus::Font fontx(pdc->GetSafeHdc(), &lgfont);
		// Y原点刻值
		//rtinfo.setRect(-mc_iScaleInfoWidth, mc_iScaleInfoHeight/2, 0, -mc_iScaleInfoHeight/2);
		rtinfo.setRect(-mc_iScaleInfoWidth, -mc_iScaleInfoHeight/2, mc_iScaleInfoWidth, mc_iScaleInfoHeight);
		rtinfo.offset(-mc_iScaleInfoSpace, 0);
		std::swprintf(info, infoLen, L"%d", m_arrAxis[BEAxisFlagY]->getOrigin());
	    drawer::drawString(grap, info, rtinfo, m_arrInfo[BEAxisInfoYName]->getFont(), BEAlignModeAllRight);
		for(int32_t idx = 0; idx < iScalesY-1; ++idx)
		{
			// 刻标
			grap.DrawLine(&penScale, 0, scale, mc_iScaleLength, scale);
			// 刻值
			rtinfo.offset(0, scaleY);
			std::swprintf(info, infoLen, L"%d", m_arrAxis[BEAxisFlagY]->getOrigin()+m_arrAxis[BEAxisFlagY]->getPreScale()*(idx+1));
			drawer::drawString(grap, info, rtinfo, m_arrInfo[BEAxisInfoYName]->getFont(), BEAlignModeAllRight);
			// 
			scale += scaleY;
		}
		// X轴刻度
		const int32_t scaleX = rtworker.width/iScalesX;
		scale = scaleX;
		// X原点刻值
		//rtinfo.setRect(-mc_iScaleInfoWidth/2, 0, mc_iScaleInfoWidth/2, -mc_iScaleInfoHeight);
		rtinfo.setRect(-mc_iScaleInfoWidth/2, -mc_iScaleInfoHeight, mc_iScaleInfoWidth, mc_iScaleInfoHeight);
		rtinfo.offset(0, -mc_iScaleInfoSpace);
		std::swprintf(info, infoLen, L"%d", m_arrAxis[BEAxisFlagX]->getOrigin());
		drawer::drawString(grap, info, rtinfo, m_arrInfo[BEAxisInfoXName]->getFont(), BEAlignModeCenterTop);
		for(int32_t idx = 0; idx < iScalesX-1; ++idx)
		{
			// 刻标
			grap.DrawLine(&penScale, scale, 0, scale, mc_iScaleLength);
			// 刻值
			rtinfo.offset(scaleX, 0);
			std::swprintf(info, infoLen, L"%d", m_arrAxis[BEAxisFlagX]->getOrigin()+m_arrAxis[BEAxisFlagX]->getPreScale()*(idx+1));
			drawer::drawString(grap, info, rtinfo, m_arrInfo[BEAxisInfoXName]->getFont(), BEAlignModeCenterTop);
			// 
			scale += scaleX;
		}

	}
	// 绘制坐标信息
	for(itobj::PtrArrayType::iterator iter = m_arrInfo.begin(), end = m_arrInfo.end(); iter != end; ++iter)
		(*iter)->draw(pdc);
}

/*****************************************************************************************
	<< --- static itobjPageAxis::allocPropertySectionArray	2013-12-06 --- >>
	说明：分配对象属性
	参数：
	返回值：
*****************************************************************************************/
std::size_t itobjPageAxis::allocPropertySectionArray(itobj* pitem, bool isShare)
{
	itobjPropSection::PtrArrayType &vecSection = pitem->getPropertys();
	if(vecSection.size() > 0) pitem->freePropertys();
	//vecSection = itobj::allocPropertySectionArray(pitem, isShare);
	itobjPageAxis *paxis = dynamic_cast<itobjPageAxis *>(pitem);
	itobjPropSection *psec = nullptr, *psubsec = nullptr;
	entry::PtrArrayType vec;
	if(paxis && (psec = new itobjPropSection(L"页面信息")) != nullptr)
	{
		// 
		psubsec = new itobjPropSection(L"标题", nullptr, true);
		psubsec->addProperty(new itobjPropEdit(L"内容", paxis->getAxisInfo(BEAxisInfoTitle)->getText().c_str(), true, L"标题内容。", BEDrawShapeTypePageAxis, BEAxisPropTitleName));
		psubsec->addProperty(new itobjPropFont(L"字体", paxis->getAxisInfo(BEAxisInfoTitle)->getFont(), L"标题内容字体。", BEDrawShapeTypePageAxis, BEAxisPropTitleFont));
		psubsec->addProperty(new itobjPropColor(L"颜色", paxis->getAxisInfo(BEAxisInfoTitle)->getTextColor(), _DEFAULT_ITEM_TEXTCOLOR_, L"标题颜色。", BEDrawShapeTypePageAxis, BEAxisPropTitleColor)); 
		psec->addProperty(psubsec);
		// 
		psubsec = new itobjPropSection(L"副标题", nullptr, true);
		psubsec->addProperty(new itobjPropEdit(L"内容", paxis->getAxisInfo(BEAxisInfoRightTitle)->getText().c_str(), true, L"副标题内容。", BEDrawShapeTypePageAxis, BEAxisPropRightTitleName));
		psubsec->addProperty(new itobjPropFont(L"字体", paxis->getAxisInfo(BEAxisInfoRightTitle)->getFont(), L"副标题内容字体。", BEDrawShapeTypePageAxis, BEAxisPropRightTitleFont));
		psubsec->addProperty(new itobjPropColor(L"颜色", paxis->getAxisInfo(BEAxisInfoRightTitle)->getTextColor(), _DEFAULT_ITEM_TEXTCOLOR_, L"副标题颜色。", BEDrawShapeTypePageAxis, BEAxisPropRightTitleColor)); 
		psec->addProperty(psubsec);
		// 轴和轴名称颜色采用_DEFAULT_ITEM_AXISCOLOR_
		psubsec = new itobjPropSection(L"X轴", nullptr, true);
		vec.assign(std::begin(gc_tbUnitTime), std::end(gc_tbUnitTime));
		psubsec->addProperty(new itobjPropCombo(L"单位", paxis->getAxis(BEAxisFlagX)->getUnitValue(), vec, false, L"", BEDrawShapeTypePageAxis, BEAxisPropAxisXUnit));
		psubsec->addProperty(new itobjPropSpin(L"原点值", paxis->getAxis(BEAxisFlagX)->getOrigin(), INT32_MIN, INT32_MAX, L"坐标轴原点。",	BEDrawShapeTypePageAxis, BEAxisPropAxisXOrigin));
		psubsec->addProperty(new itobjPropSpin(L"单位刻度值", paxis->getAxis(BEAxisFlagX)->getPreScale(), INT32_MIN, INT32_MAX, L"坐标轴每刻度对应的逻辑值。",	BEDrawShapeTypePageAxis, BEAxisPropAxisXPrescale));
		psubsec->addProperty(new itobjPropEdit(L"标题", paxis->getAxisInfo(BEAxisInfoXTitle)->getText().c_str(), true, L"副标题内容。", BEDrawShapeTypePageAxis, BEAxisPropAxisXTitle));
		psubsec->addProperty(new itobjPropFont(L"字体", paxis->getAxisInfo(BEAxisInfoXTitle)->getFont(), L"副标题内容字体。", BEDrawShapeTypePageAxis, BEAxisPropAxisXFont));
		psubsec->addProperty(new itobjPropColor(L"颜色", paxis->getAxisInfo(BEAxisInfoXTitle)->getTextColor(), _DEFAULT_ITEM_TEXTCOLOR_, L"副标题颜色。", BEDrawShapeTypePageAxis, BEAxisPropAxisXColor)); 
		psec->addProperty(psubsec);
		// 轴和轴名称颜色采用_DEFAULT_ITEM_AXISCOLOR_
		psubsec = new itobjPropSection(L"Y轴", nullptr, true);
		vec.assign(std::begin(gc_tbUnitWeight), std::end(gc_tbUnitWeight));
		psubsec->addProperty(new itobjPropCombo(L"单位", paxis->getAxis(BEAxisFlagY)->getUnitValue(), vec, false, L"", BEDrawShapeTypePageAxis, BEAxisPropAxisYUnit));
		psubsec->addProperty(new itobjPropSpin(L"原点值", paxis->getAxis(BEAxisFlagY)->getOrigin(), INT32_MIN, INT32_MAX, L"坐标轴原点。",	BEDrawShapeTypePageAxis, BEAxisPropAxisYOrigin));
		psubsec->addProperty(new itobjPropSpin(L"单位刻度值", paxis->getAxis(BEAxisFlagY)->getPreScale(), INT32_MIN, INT32_MAX, L"坐标轴每刻度对应的逻辑值。",	BEDrawShapeTypePageAxis, BEAxisPropAxisYPrescale));
		psubsec->addProperty(new itobjPropEdit(L"标题", paxis->getAxisInfo(BEAxisInfoYTitle)->getText().c_str(), true, L"副标题内容。", BEDrawShapeTypePageAxis, BEAxisPropAxisYTitle));
		psubsec->addProperty(new itobjPropFont(L"字体", paxis->getAxisInfo(BEAxisInfoYTitle)->getFont(), L"副标题内容字体。", BEDrawShapeTypePageAxis, BEAxisPropAxisYFont));
		psubsec->addProperty(new itobjPropColor(L"颜色", paxis->getAxisInfo(BEAxisInfoYTitle)->getTextColor(), _DEFAULT_ITEM_TEXTCOLOR_, L"副标题颜色。", BEDrawShapeTypePageAxis, BEAxisPropAxisYColor)); 
		psec->addProperty(psubsec);
		// 
		vecSection.push_back(psec);
	}

	// 
	return vecSection.size();
}

/*****************************************************************************************
	<< --- itobjPageAxis::updatePropertyValue	2013-12-06 --- >>
	说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
	参数：
	pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
	返回值：
*****************************************************************************************/
bool itobjPageAxis::updatePropertyValue(itobjProp *pprop)
{
	if(pprop && itobj::updatePropertyValue(pprop))
		return true;
	if(!pprop || pprop->getTypekey() != BEDrawShapeTypePageAxis)
		return false;
	bool exists = true;
	COleVariant var = pprop->getValue();
	switch (BEAxisProp(pprop->getItemkey()))
	{
	case gtc::itobjPageAxis::BEAxisPropTitle:
		break;
	case gtc::itobjPageAxis::BEAxisPropTitleName:
		var.ChangeType(VT_BSTR);
		m_arrInfo[BEAxisInfoTitle]->setText(_bstr_t(var.bstrVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropTitleFont:
		m_arrInfo[BEAxisInfoTitle]->getFont().fromData(convert::toData(var));
		break;
	case gtc::itobjPageAxis::BEAxisPropTitleColor:
		var.ChangeType(VT_UI4);
		m_arrInfo[BEAxisInfoTitle]->setTextColor(RGB_FLIP(var.ulVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropRightTitle:
		break;
	case gtc::itobjPageAxis::BEAxisPropRightTitleName:
		var.ChangeType(VT_BSTR);
		m_arrInfo[BEAxisInfoRightTitle]->setText(_bstr_t(var.bstrVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropRightTitleFont:
		m_arrInfo[BEAxisInfoRightTitle]->getFont().fromData(convert::toData(var));
		break;
	case gtc::itobjPageAxis::BEAxisPropRightTitleColor:
		var.ChangeType(VT_UI4);
		m_arrInfo[BEAxisInfoRightTitle]->setTextColor(RGB_FLIP(var.ulVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisX:
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisXUnit:
		var.ChangeType(VT_I4);
		m_arrAxis[BEAxisFlagX]->setUnitValue(var.lVal);
		m_arrInfo[BEAxisInfoXName]->setText(m_arrAxis[BEAxisFlagX]->getUnitName());
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisXOrigin:
		var.ChangeType(VT_I4);
		m_arrAxis[BEAxisFlagX]->setOrigin(var.lVal);
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisXPrescale:
		var.ChangeType(VT_I4);
		m_arrAxis[BEAxisFlagX]->setPreScale(var.lVal);
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisXTitle:
		var.ChangeType(VT_BSTR);
		m_arrInfo[BEAxisInfoXTitle]->setText(_bstr_t(var.bstrVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisXFont:
		m_arrInfo[BEAxisInfoXTitle]->getFont().fromData(convert::toData(var));
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisXColor:
		var.ChangeType(VT_UI4);
		m_arrInfo[BEAxisInfoXTitle]->setTextColor(RGB_FLIP(var.ulVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisY:
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisYUnit:
		var.ChangeType(VT_I4);
		m_arrAxis[BEAxisFlagY]->setUnitValue(var.lVal);
		m_arrInfo[BEAxisInfoYName]->setText(m_arrAxis[BEAxisFlagY]->getUnitName());
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisYOrigin:
		var.ChangeType(VT_I4);
		m_arrAxis[BEAxisFlagY]->setOrigin(var.lVal);
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisYPrescale:
		var.ChangeType(VT_I4);
		m_arrAxis[BEAxisFlagY]->setPreScale(var.lVal);
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisYTitle:
		var.ChangeType(VT_BSTR);
		m_arrInfo[BEAxisInfoYTitle]->setText(_bstr_t(var.bstrVal));
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisYFont:
		m_arrInfo[BEAxisInfoYTitle]->getFont().fromData(convert::toData(var));
		break;
	case gtc::itobjPageAxis::BEAxisPropAxisYColor:
		var.ChangeType(VT_UI4);
		m_arrInfo[BEAxisInfoYTitle]->setTextColor(RGB_FLIP(var.ulVal));
		break;
	default:
		exists = false;
		break;
	}
	return exists;
}


#endif
}