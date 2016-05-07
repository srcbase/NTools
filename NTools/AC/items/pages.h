#ifndef __PAGES_H__
#define __PAGES_H__
#pragma once

#include "AC/basedef.h"
#include "AC/baseobj.h"
#include "AC/drawdef.h"

#include "AC/items/items.h"
namespace gtc{



//////////////////////////////////////////////////////////////////////////
//	2015-9-17	
typedef wchar_t PAPERNAME[64];	// 纸张类型名；如：A4、信封
typedef wchar_t BINNAME[24];	// 打印机纸槽名；
// 纸张信息（采用逻辑单位：0.1毫米）
typedef struct BSPaperInfo {
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & wsName;  
		ar & iSize;
		ar & bsSize;
		ar & iOrientation;
	}
#endif
public:
	std::wstring	wsName;			// 纸张类型名
	int32_t			iSize;			// 纸张尺寸索引；为0时采用bsSize定义的尺寸
	size			bsSize;			// 纸张尺寸0.1毫米
	int16_t			iOrientation;	// 纸张方向；横向2'DMORIENT_LANDSCAPE'、纵向1'DMORIENT_PORTRAIT'

	BSPaperInfo() : wsName(L"A4"), bsSize(2100,2970), iSize(9/*DMPAPER_A4*/)
		, iOrientation(1/*DMORIENT_PORTRAIT*/)
	{
	}
	BSPaperInfo(bool isLandscape)
		: wsName(L"A4"), bsSize(2100,2970), iSize(9/*DMPAPER_A4*/)
		, iOrientation(isLandscape?2:1)
	{
	}
	BSPaperInfo(const wchar_t *pname, int32_t width, int32_t height, int32_t sizeIndex)
		: wsName(pname?pname:L""), bsSize(width, height), iSize(sizeIndex)
		, iOrientation(1/*DMORIENT_PORTRAIT*/)
	{
	}
	BSPaperInfo(const BSPaperInfo &paper) : wsName(paper.wsName), bsSize(paper.bsSize), iSize(paper.iSize)
		, iOrientation(paper.iOrientation)
	{
	}
	BSPaperInfo& operator=(const BSPaperInfo &paper)
	{
		if(this != &paper)
		{
			wsName = paper.wsName;
			bsSize = paper.bsSize;
			iSize = paper.iSize;
			iOrientation = paper.iOrientation;
		}
		return *this;
	}
	// 描述
	std::wstring description(int indent = 4) const
	{
		std::wostringstream os;
		wchar_t buf[20]={0};

		gtc::outClassBegin(os, L"BSPaperInfo", indent);
		gtc::outClassItem(os, L"纸张", wsName, indent << 1);
		std::swprintf(buf, 20, L"%d %dx%d)", iSize, bsSize.width, bsSize.height);
		gtc::outClassItem(os, L"纸张尺寸", buf, indent << 1);
		std::swprintf(buf, 20, L"%s", iOrientation==1?L"纵向":L"横向");
		gtc::outClassItem(os, L"方向", buf, indent << 1);
		gtc::outClassEnd(os, L"BSPaperInfo", indent);
		return os.str();
	}
public:
	bool isLandscape() const {				return iOrientation == 2;					}



}BSPaperInfo;

//////////////////////////////////////////////////////////////////////////
//	2015-9-17	
// 纸槽信息
typedef struct BSBINInfo {
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & wsName;  
		ar & iSource;
	}
#endif
public:
	std::wstring	wsName;		// 纸槽名
	int32_t			iSource;	// 纸槽索引；see the description of the dmDefaultSource member of the DEVMODE structure

	BSBINInfo() : wsName(L""), iSource(0)
	{
	}
	BSBINInfo(const wchar_t *pname, int32_t binIndex)
		: wsName(pname?pname:L""), iSource(binIndex)
	{
	}
	BSBINInfo(const BSBINInfo &bin) : wsName(bin.wsName), iSource(bin.iSource)
	{
	}
	BSBINInfo& operator=(const BSBINInfo &bin)
	{
		if(this != &bin)
		{
			wsName = bin.wsName;
			iSource = bin.iSource;
		}
		return *this;
	}
	// 描述
	std::wstring description(int indent = 4) const
	{
		std::wostringstream os;

		gtc::outClassBegin(os, L"BSBINInfo", indent);
		gtc::outClassItem(os, L"纸槽", wsName, indent << 1);
		gtc::outClassItem(os, L"索引", iSource, indent << 1);
		gtc::outClassEnd(os, L"BSBINInfo", indent);
		return os.str();
	}


}BSBINInfo;
//////////////////////////////////////////////////////////////////////////
//	2015-9-17	
// 页眉信息；在上下页边距内侧。（采用逻辑单位：0.1毫米）
typedef struct BSHeaderFooterInfo {
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & wsTitle;  
		ar & uiHeight;
		ar & beAlign;
		ar & bsFont;

		ar & iPageStyle;
		ar & bePageAlign;
		ar & bsPageFont;
		ar & vecLogo;
		ar & bsLogo;
	}
#endif
public:
	std::wstring			wsTitle;		// 标题
	uint16_t				uiHeight;		// 高度
	BEAlignMode				beAlign;		// 标题对齐方式
	BSFont					bsFont;			// 标题字体

	int32_t					iPageStyle;		// 页码样式；如：0=空；1=第？页 共x页；2=日期
	BEAlignMode				bePageAlign;	// 尾注对齐方式
	BSFont					bsPageFont;		// 尾注字体
	std::vector<uint8_t>	vecLogo;	// logo图片
	rect					bsLogo;		// logo位置，相对于左上角的偏移位置；采用平铺缩放绘制

	BSHeaderFooterInfo(uint16_t height = 150) 
		: uiHeight(height), wsTitle(L""), beAlign(BEAlignModeAllCenter)
		, iPageStyle(0), bePageAlign(BEAlignModeAllRight)
	{
	}
	BSHeaderFooterInfo(const BSHeaderFooterInfo &info)
		: uiHeight(info.uiHeight), wsTitle(info.wsTitle), beAlign(info.beAlign), bsFont(info.bsFont)
		, iPageStyle(info.iPageStyle), bePageAlign(info.bePageAlign), bsPageFont(info.bsPageFont)
		, vecLogo(info.vecLogo), bsLogo(info.bsLogo)
	{
	}
	BSHeaderFooterInfo& operator=(const BSHeaderFooterInfo &info)
	{
		if(this != &info)
		{
			uiHeight = info.uiHeight;
			wsTitle = info.wsTitle;
			beAlign = info.beAlign;
			bsFont = info.bsFont;

			iPageStyle = info.iPageStyle;
			bePageAlign = info.bePageAlign;
			bsPageFont = info.bsPageFont;

			vecLogo = info.vecLogo;
			bsLogo = info.bsLogo;
		}
		return *this;
	}
	// 描述
	std::wstring description(int indent = 4) const
	{
		std::wostringstream os;

		gtc::outClassBegin(os, L"BSHeaderFooterInfo", indent);
		gtc::outClassItem(os, L"高度", uiHeight, indent << 1);
		gtc::outClassItem(os, L"标题", wsTitle, indent << 1);
		gtc::outClassItem(os, L"页码格式", iPageStyle, indent << 1);
		wchar_t buf[20] = {0};
		std::swprintf(buf, 120, L"%s", vecLogo.size()>0?L"支持":L"不支持");
		gtc::outClassItem(os, L"logo", buf, indent << 1);

		gtc::outClassEnd(os, L"BSHeaderFooterInfo", indent);
		return os.str();
	}


}BSHeaderFooterInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPage	2013-12-02 --- >>
//	绘制：工作区，纸张，纸张背景，页眉页脚等等
////////////////////////////////////////////////////////////////////////////////////////////////////////
//		*：defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)模式下的原点（0，0）
//		x：defined(WORK_AXIS_CAD)模式下的原点（0，0）
//		_________________________________
//		|	|						|	|
//		|	|		MarginTop		|	|
//		|	|						|	|
//		—————————————————————————————————
//		|	|						|	|
//		|	|		 header			|	|
//		|	|						|	|
//		————*————————————————————————————
//		| M	|						| M	|
//		| a	|						| a	|
//		| r	|						| r	|
//		| g	|						| g	|
//		| i	|						| i	|
//		| n	|						| n	|
//		| L	|		PaperWork		| R	|
//		| e	|						| i	|
//		| f	|						| g	|
//		| t	|						| h	|
//		|	|						| t	|
//		|	|						|	|
//		|	|						|	|
//		|	|						|	|
//		————X————————————————————————————
//		|	|						|	|
//		|	|		 footer			|	|
//		|	|						|	|
//		—————————————————————————————————
//		|	|	   MarginBottom		|	|
//		|	|						|	|
//		—————————————————————————————————
// 
// 
class itobjPage : public itobj
{
	_DECLARE_ITOBJ_(itobjPage)
protected:
	typedef enum BEPageProp {
		BEPagePropPaper=1,BEPagePropPaperName,BEPagePropPaperOrientation,
		BEPagePropMargins,BEPagePropMargintop,BEPagePropMarginbottom,BEPagePropMarginleft,BEPagePropMarginright,
		BEPagePropHeaderfooter,
		BEPagePropHeader,BEPagePropHeaderHeight,BEPagePropHeaderTitle,BEPagePropHeaderAlign,BEPagePropHeaderFont,
		BEPagePropHeaderPagestyle,BEPagePropHeaderPagealign,BEPagePropHeaderPagefont,
		BEPagePropHeaderLogo,
		BEPagePropHeaderLogopos,BEPagePropHeaderLogoposX,BEPagePropHeaderLogoposY,BEPagePropHeaderLogoposWidth,BEPagePropHeaderLogoposHeight,
		BEPagePropFooter,BEPagePropFooterHeight,BEPagePropFooterTitle,BEPagePropFooterAlign,BEPagePropFooterFont,
		BEPagePropFooterPagestyle,BEPagePropFooterPagealign,BEPagePropFooterPagefont,
		BEPagePropFooterLogo,
		BEPagePropFooterLogopos,BEPagePropFooterLogoposX,BEPagePropFooterLogoposY,BEPagePropFooterLogoposWidth,BEPagePropFooterLogoposHeight,
		BEPagePropWorkercolor,BEPagePropPapershadow,
		BEPagePropGrid,BEPagePropGridstyle,BEPagePropGridcolor
	}BEPageProp;

public:
	itobjPage(bool isLandscape = false, bool margins = false, bool headerfooter = false, bool grid = false);
	itobjPage(const itobjPage &page);
	itobjPage& operator=(const itobjPage &page);
	virtual ~itobjPage();
	// 描述
	std::wstring description(int indent = 4) const;
	/*****************************************************************************************
		<< --- itobjPage::clone	2013-11-29 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual itobj* clone() {	
		itobj* pitem = new itobjPage(*this);
		return pitem;
	}
public:
	/*****************************************************************************************
		<< --- itobjPage::remove	2013-11-29 --- >>
		说明：释放自己
		参数：
		返回值：
	*****************************************************************************************/
	virtual void remove() {
		delete this;
	}
	/*****************************************************************************************
		<< --- itobjPage::refresh	2013-11-29 --- >>
		说明：刷新信息，如：改变页面尺寸后就需要调用本函数
		参数：
		返回值：
	*****************************************************************************************/
	void refresh();
	/*****************************************************************************************
		<< --- itobjPage::getWindowOrg	2013-12-04 --- >>
		说明：把设备坐标的原点(视口)映射到逻辑坐标的(X, Y)处
		参数：
		bsWorker	= 工作台尺寸
		isprinter	= 是否是打印机
		返回值：
	*****************************************************************************************/
	// _WIN32
	point getWindowOrg(size bsWorker, bool isprinter);
#if defined(_WIN32)
	virtual void draw(CDC *pdc);
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
	virtual int32_t hitTest(CPoint point, acm::CNPainterView *pView, bool bSelected);
	/*****************************************************************************************
		<< --- itobjPage::drawTracker	2013-12-09 --- >>
		说明：绘制定位块所在区域
		参数：
		vpdc		= 绘图环境
		vstate		= 定位块的状态
		vbkcolor	= 块的填充色
		返回值：
	*****************************************************************************************/
	virtual void drawTracker(CDC* pdc, BETrackerState vstate, uint32_t vbkcolor = 0xFF00FF00);
	/*****************************************************************************************
		<< --- static itobjPage::allocPropertySectionArray	2013-12-06 --- >>
		说明：分配对象属性
		参数：
		返回值：
	*****************************************************************************************/
	static std::size_t allocPropertySectionArray(itobj* pitem, bool isShare);
	virtual std::size_t allocPropertySectionArray(bool isShare) {
		return allocPropertySectionArray(this, isShare);
	}
	/*****************************************************************************************
		<< --- itobjPage::updatePropertyValue	2013-12-06 --- >>
		说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
		参数：
		pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
		返回值：
	*****************************************************************************************/
	virtual bool updatePropertyValue(itobjProp *pprop);

#endif
public:
	// 纸张区域，即：m_bsPosition
	void			refreshPagePosition();
	// 打印纸的工作区，x、y轴包含的区域，即:除页边距,页眉页脚外区域
	rect			areaPaperWork() const;
	BSPaperInfo&	getPaperInfo() {								return m_bsPaper;							}
	void			setPaperInfo(const BSPaperInfo &paper) {		m_bsPaper = paper;							}

	bool			supportMargins() const {						return m_bMargins;							}
	void			setMargins(bool isMargins) {					m_bMargins = isMargins;						}
	uint16_t		getMarginTop() const {							return m_uiMarginTop;						}
	void			setMarginTop(uint16_t mTop) {					m_uiMarginTop = mTop;						}
	rect			areaMarginTop() const;

	uint16_t		getMarginBottom() const {						return m_uiMarginBottom;					}
	void			setMarginBottom(uint16_t mBottom) {				m_uiMarginBottom = mBottom;					}
	rect			areaMarginBottom() const;

	uint16_t		getMarginLeft() const {							return m_uiMarginLeft;						}
	void			setMarginLeft(uint16_t mLeft) {					m_uiMarginLeft = mLeft;						}
	rect			areaMarginLeft() const;

	uint16_t		getMarginRight() const {						return m_uiMarginRight;						}
	void			setMarginRight(uint16_t mRight) {				m_uiMarginRight = mRight;					}
	rect			areaMarginRight() const;

	bool			supportHeaderFooter(void) const {				return m_bHeaderFooter;						}
	void			setHeaderFooter(bool isHFer) {					m_bHeaderFooter = isHFer;					}
	BSHeaderFooterInfo& getHeader() {								return m_bsHeader;							}
	void			setHeader(const BSHeaderFooterInfo &info) {		m_bsHeader = info;							}
	rect			areaHeader() const;
	BSHeaderFooterInfo& getFooter() {								return m_bsFooter;							}
	void			setFooter(const BSHeaderFooterInfo &info) {		m_bsFooter = info;							}
	rect			areaFooter() const;

	uint32_t		getWorkerColor() const {						return m_clrWorker;							}
	void			setWorkerColor(uint32_t color) {				m_clrWorker = color;						}
	uint32_t		getPaperShadowColor() const {					return m_clrPaperShadow;					}
	void			setPaperShadowColor(uint32_t color) {			m_clrPaperShadow = color;					}

	bool			supportGrid() const {							return m_bGrid;								}
	void			setGrid(bool grid) {							m_bGrid = grid;								}
	BSPen			getGridColor() const {							return m_bsGrid;							}
	void			setGridColor(const BSPen &pen) {				m_bsGrid = pen;								}


#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<itobj>(*this);
		ar & m_clrWorker;  
		ar & m_clrPaperShadow;

		ar & m_bGrid;
		ar & m_bsGrid;

		ar & m_bsPaper;

		ar & m_bMargins;
		ar & m_uiMarginTop;
		ar & m_uiMarginBottom;
		ar & m_uiMarginLeft;
		ar & m_uiMarginRight;

		ar & m_bHeaderFooter;
		ar & m_bsHeader;
		ar & m_bsFooter;
	}
#endif
protected:
	// 均采用逻辑单位 1=0.1毫米
	uint32_t		m_clrWorker;		// 工作台背景色
	uint32_t		m_clrPaperShadow;	// 纸张阴影

	bool			m_bGrid;			// 是否绘制参考网格线
	BSPen			m_bsGrid;			// 网格线样式

	BSPaperInfo		m_bsPaper;			// 纸张信息，采用逻辑单位 1=0.1毫米

	bool			m_bMargins;			// 是否启用页边距支持
	uint16_t		m_uiMarginTop;		// 上页边距，逻辑单位0.1毫米
	uint16_t		m_uiMarginBottom;	// 下页边距，逻辑单位0.1毫米
	uint16_t		m_uiMarginLeft;		// 左页边距，逻辑单位
	uint16_t		m_uiMarginRight;	// 右页边距，逻辑单位

	bool				m_bHeaderFooter;// 是否启用页眉页脚支持；在页边距内侧
	BSHeaderFooterInfo	m_bsHeader;		// 页眉高，逻辑单位0.1毫米
	BSHeaderFooterInfo	m_bsFooter;		// 页脚高，逻辑单位0.1毫米
	
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPageAxis	2014-01-03 --- >>
//	带坐标系的页
#define _DEFAULT_ITEM_AXISCOLOR_	0xFF000000
////////////////////////////////////////////////////////////////////////////////////////////////////////
class coordinateAxis {

public:
	typedef std::vector<coordinateAxis *> PtrArrayType;
protected:
	coordinateAxis()
		: m_beUnitType(BEUnitTypeWeight), m_iUnitValue(1), m_iOrigin(0), m_iPreScale(1), m_iScales(11)
		, m_iDevRange(1), m_fltDevPreLogic(0.0f), m_fltDevOrigin(0.0f)
	{
	}
public:
	coordinateAxis(BEUnitWeight unit, int32_t origin = 0, int32_t prescale = 1)
		: m_beUnitType(BEUnitTypeWeight), m_iUnitValue(unit), m_iOrigin(origin), m_iPreScale(prescale), m_iScales(11)
		, m_iDevRange(1), m_fltDevPreLogic(0.0f), m_fltDevOrigin(0.0f)
	{
		assert(prescale > 0);
	}
	coordinateAxis(BEUnitTime unit, int32_t origin = 0, int32_t prescale = 1)
		: m_beUnitType(BEUnitTypeTime), m_iUnitValue(unit), m_iOrigin(origin), m_iPreScale(prescale), m_iScales(11)
		, m_iDevRange(1), m_fltDevPreLogic(0.0f), m_fltDevOrigin(0.0f)
	{
		assert(prescale > 0);
	}
	coordinateAxis(const coordinateAxis &axis)
		: m_beUnitType(axis.m_beUnitType), m_iUnitValue(axis.m_iUnitValue), m_iOrigin(axis.m_iOrigin), m_iPreScale(axis.m_iPreScale)
		, m_iScales(axis.m_iScales)
		, m_iDevRange(axis.m_iDevRange), m_fltDevPreLogic(axis.m_fltDevPreLogic), m_fltDevOrigin(axis.m_fltDevOrigin)
	{
	}
	coordinateAxis& operator=(const coordinateAxis &axis)
	{
		if(this != &axis)
		{
			m_beUnitType = axis.m_beUnitType;
			m_iUnitValue = axis.m_iUnitValue;
			m_iOrigin = axis.m_iOrigin;
			m_iPreScale = axis.m_iPreScale;
			m_iDevRange = axis.m_iDevRange;
			m_fltDevPreLogic = axis.m_fltDevPreLogic;
			m_fltDevOrigin = axis.m_fltDevOrigin;
		}
		return *this;
	}
	coordinateAxis* clone() {
		return new coordinateAxis(*this);
	}
	void remove() {
		delete this;
	}

public:
	/*****************************************************************************************
		<< --- coordinateAxis::getUnitName 2014-01-21 --- >>
		说明：坐标轴的单位名称
		参数：
		返回值：坐标轴的单位名称
	*****************************************************************************************/
	std::wstring getUnitName() const {
		switch (m_beUnitType)
		{
		case gtc::BEUnitTypeWeight:
			return toString(BEUnitWeight(m_iUnitValue));
			break;
		case gtc::BEUnitTypeTime:
			return toString(BEUnitTime(m_iUnitValue));
			break;
		default:
			break;
		}
		return L"";
	}
	/*****************************************************************************************
		<< --  setDevRange		2015-1-7 --- >>
		说明：设定坐标轴长度对应的工作区长度
		参数：
			idevRange	= 工作区长度，单位0.1毫米
		返回值：
	*****************************************************************************************/
	void setDevRange(int32_t idevRange)
	{
		assert(idevRange > 0);
		m_iDevRange = idevRange;
		refresh();
	}
	/*****************************************************************************************
		<< --- coordinateAxis::scalingFactor 2014-01-21 --- >>
		说明：得到坐标轴单位相对于标准单位(时间:毫秒；重量:克)的比例
		参数：
		返回值：坐标轴的单位名称
	*****************************************************************************************/
	float scalingFactor() const {
		float scale = 1.0f;
		switch (m_beUnitType)
		{
		case gtc::BEUnitTypeWeight:
			{
				switch (BEUnitWeight(m_iUnitValue))
				{
				case gtc::BEUnitWeightMilligram:
					scale = 1000.0f;
					break;
				case gtc::BEUnitWeightGram:
					scale = 1.0f;
					break;
				case gtc::BEUnitWeightLiang:
					scale = 0.02f;
					break;
				case gtc::BEUnitWeightJing:
					scale = 0.01f;
					break;
				case gtc::BEUnitWeightKilogram:
					scale = 0.001f;
					break;
				case gtc::BEUnitWeightTon:
					scale = 0.000001f;
					break;
				default:
					break;
				}
			}

			break;
		case gtc::BEUnitTypeTime:
			{
				switch (BEUnitTime(m_iUnitValue))
				{
				case gtc::BEUnitTimeMillisecond:
					scale = 1.0f;
					break;
				case gtc::BEUnitTimeSecond:
					scale = 0.001f;
					break;
				case gtc::BEUnitTimeMinute:
					scale = 1.666667e-5f;
					break;
				default:
					break;
				}	
			}

			break;
		default:
			break;
		}
		return scale;
	}
	/*****************************************************************************************
		<< --  toClient		2015-1-7 --- >>
		说明：将逻辑坐标转换成工作坐标（单位0.1毫米）
		参数：
			fltLogic	= 逻辑值
		返回值：工作坐标值0.1毫米
	*****************************************************************************************/
	float toClient(float fltLogic)
	{
		float dev = 0.0f;
		//// 转换到坐标轴值
		//float axisValue = scalingFactor() * fltLogic;	// 毫秒-->x分钟
		// 坐标轴值所占有设备坐标值量
		dev = fltLogic*m_fltDevPreLogic;	// x分钟-->y个0.1mm
		// 相对原点移动
		dev -= m_fltDevOrigin;
		return dev;
	}
protected:
	/*****************************************************************************************
		<< --  replace		2015-1-8 --- >>
		说明：
		参数：
		返回值：
	*****************************************************************************************/
	void refresh() {
		m_fltDevPreLogic = (float)m_iDevRange / (m_iPreScale * m_iScales);
		m_fltDevPreLogic *= scalingFactor();
		m_fltDevOrigin = m_fltDevPreLogic * m_iOrigin;
	}
public:
	BEUnitType getUnitType() const {			return m_beUnitType;						}
	void setUnitType(BEUnitType unitType) {		
		m_beUnitType = unitType;
		refresh();
	}
	int32_t getUnitValue() const {				return m_iUnitValue;						}
	void setUnitValue(int32_t unitValue) {		
		m_iUnitValue = unitValue;
		refresh();
	}
	int32_t getOrigin() const {					return m_iOrigin;							}
	void setOrigin(int32_t origin) {
		m_iOrigin = origin;
		refresh();
	}
	int32_t getPreScale() const {				return m_iPreScale;							}
	void setPreScale(int32_t preScale) {
		m_iPreScale = preScale;
		refresh();
	}
	int32_t getScales() const {					return m_iScales;							}
	void setScales(int32_t scales) {
		m_iScales = scales;
		refresh();
	}

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_beUnitType;  
		ar & m_iUnitValue;
		ar & m_iOrigin;
		ar & m_iPreScale;
		ar & m_iScales;

		ar & m_iDevRange;
		ar & m_fltDevPreLogic;
		ar & m_fltDevOrigin;
	}
#endif

private:
	BEUnitType		m_beUnitType;		// 轴采用的单位类型，如：重量、时间等
	int32_t			m_iUnitValue;		// 轴单位类型对应的值，如：重量（克，两...BEUnitTypeWeight）、时间对应的（秒，分...BEUnitTypeTime）
	int32_t			m_iOrigin;			// 坐标原点值（逻辑单位）
	int32_t			m_iPreScale;		// 每等份对应的值（逻辑单位），坐标轴分为11等份，其中一个等份作为轴箭头用
	int32_t			m_iScales;			// 坐标轴对应的等份数，包含一个等份作为轴箭头用

	int32_t			m_iDevRange;		// 轴长度（工作坐标，单位0.1毫米）
	float			m_fltDevPreLogic;	// 每逻辑单位对应的工作坐标（单位0.1毫米）
	float			m_fltDevOrigin;		// 逻辑原点对应的工作坐标
};

class itobjPageAxis : public itobjPage
{
	_DECLARE_ITOBJ_(itobjPageAxis)
public:
	typedef enum BEAxisFlag {
		BEAxisFlagX	= 0,		// X轴
		BEAxisFlagY				// Y轴
	}BEAxisFlag;

	typedef enum BEAxisInfo {
		BEAxisInfoYName			= 0,	// 信息1，y轴名称
		BEAxisInfoXName			,		// 信息2，x轴名称
		BEAxisInfoYTitle		,		// 信息3，y轴标题
		BEAxisInfoXTitle		,		// 信息4，x轴标题
		BEAxisInfoTitle			,		// 信息5，坐标系标题
		BEAxisInfoRightTitle			// 信息6，坐标系右侧标题
	}BEAxisInfo;
	typedef enum BEAxisProp {
		BEAxisPropTitle=0,BEAxisPropTitleName,BEAxisPropTitleFont,BEAxisPropTitleColor,
		BEAxisPropRightTitle,BEAxisPropRightTitleName,BEAxisPropRightTitleFont,BEAxisPropRightTitleColor,
		BEAxisPropAxisX,BEAxisPropAxisXUnit,BEAxisPropAxisXOrigin,BEAxisPropAxisXPrescale,BEAxisPropAxisXTitle,BEAxisPropAxisXFont,BEAxisPropAxisXColor,
		BEAxisPropAxisY,BEAxisPropAxisYUnit,BEAxisPropAxisYOrigin,BEAxisPropAxisYPrescale,BEAxisPropAxisYTitle,BEAxisPropAxisYFont,BEAxisPropAxisYColor
	}BEAxisProp;

public:
	itobjPageAxis();
	itobjPageAxis(const itobjPageAxis &page);
	itobjPageAxis& operator=(const itobjPageAxis &page);
	virtual ~itobjPageAxis();
	// 描述
	std::wstring description(int indent = 4) const;
	/*****************************************************************************************
		<< --- itobjPageAxis::clone	2013-11-29 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual itobj* clone() {	
		itobj* pitem = new itobjPageAxis(*this);
		return pitem;
	}
public:
	/*****************************************************************************************
		<< --- itobjPageAxis::logicToDoc	2014-01-13 --- >>
		说明：将逻辑坐标映射到工作坐标；(m_iUserScaleOriginX,m_iUserScaleOriginY)对应的工作坐标为(0,0)
		参数：
		pt	= 被映射点的引用
		返回值：
	*****************************************************************************************/
	virtual void logicToDoc(pointf &pt);
	virtual void logicToDoc(rectf &rc);
	virtual void logicToDoc(const float &msvX, const float &msvY, float &devX, float &devY, bool reloadAxis = false);

public:
#if defined(_WIN32)
	virtual void draw(CDC *pdc);
	/*****************************************************************************************
		<< --- static itobjPageAxis::allocPropertySectionArray	2013-12-06 --- >>
		说明：分配对象属性
		参数：
		返回值：
	*****************************************************************************************/
	static std::size_t allocPropertySectionArray(itobj* pitem, bool isShare);
	virtual std::size_t allocPropertySectionArray(bool isShare) {
		return allocPropertySectionArray(this, isShare);
	}
	/*****************************************************************************************
		<< --- itobjPageAxis::updatePropertyValue	2013-12-06 --- >>
		说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
		参数：
		pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
		返回值：
	*****************************************************************************************/
	virtual bool updatePropertyValue(itobjProp *pprop);

#endif
public:
	// 属性

	//int32_t		getMinUserX() const {					return m_iMinUserX;					}
	//void		setMinUserX(int32_t minuserX) {			m_iMinUserX = minuserX;				}
	//int32_t		getMaxUserX() const {					return m_iMaxUserX;					}
	//void		setMaxUserX(int32_t maxuserX) {			m_iMaxUserX = maxuserX;				}
	//int32_t		getMinUserY() const {					return m_iMinUserX;					}
	//void		setMinUserY(int32_t minuserY) {			m_iMinUserY = minuserY;				}
	//int32_t		getMaxUserY() const {					return m_iMaxUserX;					}
	//void		setMaxUserY(int32_t maxuserY) {			m_iMaxUserY = maxuserY;				}
	//
	//int32_t		getMinWorkX() const {					return m_iMinWorkX;					}
	//void		setMinWorkX(int32_t minworkX) {			m_iMinWorkX = minworkX;				}
	//int32_t		getMaxWorkX() const {					return m_iMaxWorkX;					}
	//void		setMaxWorkX(int32_t maxworkX) {			m_iMaxWorkX = maxworkX;				}
	//int32_t		getMinWorkY() const {					return m_iMinWorkX;					}
	//void		setMinWorkY(int32_t minworkY) {			m_iMinWorkY = minworkY;				}
	//int32_t		getMaxWorkY() const {					return m_iMaxWorkX;					}
	//void		setMaxWorkY(int32_t maxworkY) {			m_iMaxWorkY = maxworkY;				}
	//int32_t		getScaleOriginX() const {				return m_iUserScaleOriginX;			}
	//void		setScaleOriginX(int32_t originX) {		m_iUserScaleOriginX = originX;		}
	//int32_t		getScaleOriginY() const {				return m_iUserScaleOriginY;			}
	//void		setScaleOriginY(int32_t originY) {		m_iUserScaleOriginY = originY;		}
	//int32_t		getUserScaleX() const {					return m_iUserScaleX;				}
	//void		setUserScaleX(int32_t scaleX) {			m_iUserScaleX = scaleX;				}
	//int32_t		getUserScaleY() const {					return m_iUserScaleY;				}
	//void		setUserScaleY(int32_t scaleY) {			m_iUserScaleY = scaleY;				}

	coordinateAxis*	getAxis(BEAxisFlag axis) {				return m_arrAxis[axis];		}
	void		setAxisInfo(BEAxisInfo axisInfo, const wchar_t *pinfo);
	gtc::itobj* getAxisInfo(BEAxisInfo axisInfo) {		return m_arrInfo[axisInfo];	}

private:
	static int32_t mc_iLineThick;			// 轴线粗
	static int32_t mc_iScaleLength;			// 轴刻度长
	static int32_t mc_iScaleShortLength;	// 轴刻度长
	static int32_t mc_iAxisArrow;			// 超出轴部分，用来绘制轴的箭头
	static int32_t mc_iScaleInfoSpace;		// 刻度值区域与刻标的间距
	static int32_t mc_iScaleInfoWidth;		// 刻度值区域的宽度
	static int32_t mc_iScaleInfoHeight;		// 刻度值区域的高度
	static int32_t mc_iAxisInfoSpace;		// 轴信息与刻度值区域的间距
	static int32_t mc_iAxisInfoWidth;		// 轴信息的宽度
	static int32_t mc_iAxisInfoHeight;		// 轴信息的高度
	static int32_t mc_iAxisNameWidth;		// 轴名称的宽度
	static int32_t mc_iAxisNameHeight;		// 轴名称的高度

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<itobjPage>(*this);
		ar & m_arrAxis;  
		ar & m_arrInfo;
	}
#endif
protected:
	/*****************************************************************************
	坐标系说明：
		1、y轴：areaPaperWork().cy，分成11等份；每等份映射用户数据为m_iUserScaleY
		2、x轴：areaPaperWork().cx，分成11等份；每等份映射用户数据为m_iUserScaleX
		3、第11个刻度不显示（其被线帽占用）
		4、轴和轴名称颜色采用_DEFAULT_ITEM_AXISCOLOR_，字号：小五

						OriginX+PreScaleX*10	∧y轴			信息5
										信息1	|         /\
												|        /  \
										信		|   /\  /    \						信
										息		|  /  \/      \      /				息
										3		| /            \____/				6
												|/
										OriginY	.-------------------------------->
											OriginX						OriginX+PreScaleX*10
																				信息2
															信息4
	*****************************************************************************/
	//int32_t		m_iMinUserX;	// 用户x轴数据，如：采用开始时间
	//int32_t		m_iMaxUserX;	// 用户x轴数据，如：采用截止时间
	//int32_t		m_iMinUserY;	// 用户y轴数据，如：最小可能称重值0
	//int32_t		m_iMaxUserY;	// 用户y轴数据，如：最大可能称重值500克
	//int32_t		m_iMinWorkX;	// 工作x轴数据，如：最小逻辑值0
	//int32_t		m_iMaxWorkX;	// 工作x轴数据，如：最大逻辑值500毫米
	//int32_t		m_iMinWorkY;	// 工作y轴数据，如：最小逻辑值0
	//int32_t		m_iMaxWorkY;	// 工作y轴数据，如：最大逻辑值500毫米
	//BEUnitType		m_beYUnitType;			// Y轴单位类型
	//BEUnitType		m_beXUnitType;			// X轴单位类型
	//int32_t			m_iYUnitValue;			// Y轴单位
	//int32_t			m_iXUnitValue;			// X轴单位
	//int32_t			m_iUserScaleOriginY;	// 用户数据起点值，如：10克
	//int32_t			m_iUserScaleOriginX;	// 用户数据起点值，如：1分钟或GetTickCount
	//int32_t			m_iUserScaleY;			// 每等份映射用户数据，如：10克，整个坐标系能标示11*10+m_iUserScaleOriginY克
	//int32_t			m_iUserScaleX;			// 每等份映射用户数据，如：1分钟，整个坐标系能标示11+m_iUserScaleOriginX分钟

	coordinateAxis::PtrArrayType	m_arrAxis;
	itobj::PtrArrayType				m_arrInfo;
	//std::array<coordinateAxis*, 2>	m_arrAxis;	// 轴信息
	//std::array<itobj*, 6>	m_arrInfo;	// 信息区
};

}


#endif