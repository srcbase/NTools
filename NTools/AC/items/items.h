#ifndef __ITEMS_H__
#define __ITEMS_H__
#pragma once
#include <cstdint>
#include <string>
#include <deque>
#include <sstream>
#include <algorithm>
#include "AC/basedef.h"
#include "AC/drawdef.h"
#include "AC/baseobj.h"
#include "AC/draw/drawer.h"
#include "AC/items/itobjProperty.h"
#if defined(_AFXDLL)
	#include <afxwin.h>
	#include <atltypes.h>
	namespace acm {
		class CNPainterView;
	}
#endif

//// itobj序列化
//#if defined(BOOST_SERIALIZE)
//	BOOST_CLASS_EXPORT(gtc::itobjRect)
//	BOOST_CLASS_EXPORT(gtc::itobjPropSpin)
//	BOOST_CLASS_EXPORT(gtc::itobjPropBoolean)
//	BOOST_CLASS_EXPORT(gtc::itobjPropEdit)
//	BOOST_CLASS_EXPORT(gtc::itobjPropColor)
//	BOOST_CLASS_EXPORT(gtc::itobjPropCombo)
//	BOOST_CLASS_EXPORT(gtc::itobjPropFont)
//	BOOST_CLASS_EXPORT(gtc::itobjPropFolder)
//	BOOST_CLASS_EXPORT(gtc::itobjPropFile)
//#endif

namespace gtc{

class itobj;

///////////////////////////////////////////////////////////////////////////////////////////////////
// 绘图对象类型；每新增一个绘图对象类型时此处也要更新
typedef enum BEDrawShapeType {
	BEDrawShapeTypeItem		= 0x0,			// 根对象
	BEDrawShapeTypePage		,				// 纸张，打印页
	BEDrawShapeTypePageAxis	,				// 纸张，带坐标系
	BEDrawShapeTypeRect		,				// 矩形、椭圆、线
	BEDrawShapeTypeGrid		,				// 数据表格

	BEDrawShapeTypeDevice					= 0x8000,		// 设备类型基类SerialPort
	BEDrawShapeTypeDeviceWeigherBase		= 0x8001,
	BEDrawShapeTypeDeviceSerialPort			= 0x8101,		// 串口通讯设备
	BEDrawShapeTypeDeviceWeigher			= 0x8102,		// 称重数据采集
	BEDrawShapeTypeDeviceWeigherSando		= 0x8103,		// 称重数据采集，山东AD-S321
	BEDrawShapeTypeDeviceSCR				= 0x8200,		// 可控硅
	BEDrawShapeTypeDeviceMotor				= 0x8300,		// 步进电机
	BEDrawShapeTypeDeviceScale				= 0x8F00		// 电子秤
}BEDrawShapeType;
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- BSItobjSchema	2013-12-12 --- >>
//	定义一个类继承关系的B树。如下：
//		depth:0				classA
//							/    \
//		depth:1			classB	 classC
//						/		  /	  |	\	\
//		depth:2		classD	   classE F  G   H
//						|		|
//		depth:3		classI	   classJ
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

// itobj类及其子类的结构信息
typedef struct BSItobjSchema  {
	//typedef gtc::itobjPropSection::PtrArrayType (*PFuncAllocPropertySection)(itobj *pitem, bool isShare);
	std::string						className;
	int32_t							classType;		// 类类型，在整个树中唯一key;=BEDrawShapeType
	int32_t							classDepth;		// 类继承深度
	uint32_t						classVersion;	// 类版本号
	BSItobjSchema					*pparentClass;	// 父类
	std::vector<BSItobjSchema *>	vecSubClass;	// 子类
	//PFuncAllocPropertySection		pfuncAllocPropertySection;	// 分配属性静态类函数
	BSItobjSchema(const char *pname, int32_t type, int32_t depth, BSItobjSchema *pitobj, BSItobjSchema *pparentItobj, uint32_t version/*, PFuncAllocPropertySection func*/)
		: className(pname?pname:""), classType(type), classDepth(depth)
		, pparentClass(pparentItobj)/*, pfuncAllocPropertySection(func)*/
		, classVersion(version)
	{
		assert(className.length() > 0);
		//assert(depth != 0 && pitobj && pparentItobj);
		if(pitobj && pparentClass)
			pparentClass->vecSubClass.push_back(pitobj);
	}
	// 公共方法
	BSItobjSchema *getNodeByDepth(int32_t depth) {
		if(depth > classDepth)
			return nullptr;
		BSItobjSchema *pnode = this;
		while(pnode)
		{
			if(pnode->classDepth == depth)
				return pnode;
			pnode = pnode->pparentClass;
		}
		return pnode;
	}
	/*****************************************************************************************
		<< --- static findCommunityBasecalss	2013-12-12 --- >>
		说明：查找指定对象组的公共基类类型
		参数：
		pnode1	= 被比较节点1
		pnode2	= 被比较节点2
		返回值：公共基对象类型
	*****************************************************************************************/
	static BSItobjSchema* findCommunityBasecalss(BSItobjSchema* pnode1, BSItobjSchema *pnode2)
	{
		// 节点是类静态生产的，所以同一个类的节点地址只有一个
		if(pnode1 == pnode2)
			return pnode1;
		if(pnode1 && pnode2)
		{
			// 定位到同一深度
			if(pnode1->classDepth < pnode2->classDepth)
				pnode2 = pnode2->getNodeByDepth(pnode1->classDepth);
			else if(pnode1->classDepth > pnode2->classDepth)
				pnode1 = pnode1->getNodeByDepth(pnode2->classDepth);
			while(pnode1 && pnode2)
			{
				if(pnode1 == pnode2)
					return pnode1;
				pnode1 = pnode1->pparentClass;
				pnode2 = pnode2->pparentClass;
			}
		}
		return nullptr;
	}
}BSItobjSchema;
// 类定义文件中使用
#define _DECLARE_ITOBJ_(class_name) \
public: \
	static BSItobjSchema class##class_name; \
public: \
	virtual BSItobjSchema* getSchemaNode() const { \
		return &class##class_name; \
	} \
	virtual int32_t getType(int32_t depth) const { /*根据深度得到基类的类型*/\
		BSItobjSchema *pnode = &class##class_name; \
		while(pnode) \
		{ \
			if(pnode->classDepth == depth) \
				return pnode->classType; \
			pnode = pnode->pparentClass; \
		} \
		return -1; \
	} \
	virtual int32_t getType() const { \
		return class##class_name.classType; \
	}
// 类实现文件中使用，根类中使用
#define _IMPLEMENT_ROOT_ITOBJ_(class_root, class_type, class_version) \
	BSItobjSchema class_root::class##class_root = BSItobjSchema( \
	#class_root, class_type, 0, nullptr, nullptr, class_version/*, class_root::allocPropertySectionArray*/); 
// 类实现文件中使用，派生类中使用
#define _IMPLEMENT_ITOBJ_(class_name, base_class_name, class_type, class_version) \
	BSItobjSchema class_name::class##class_name = BSItobjSchema( \
	#class_name, class_type, base_class_name::class##base_class_name.classDepth+1, \
	&class_name::class##class_name, &base_class_name::class##base_class_name, class_version/*, class_name::allocPropertySectionArray*/); 



////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobj	2014-06-07 --- >>
//	绘图单元通用类
////////////////////////////////////////////////////////////////////////////////////////////////////////
// itobj的属性默认值
#define _DEFAULT_ITEM_BACKCOLOR_	0xFFFFFFFF	// 默认背景色：白色
#define _DEFAULT_ITEM_FORECOLOR_	0xFF000000	// 默认前景色：黑色
//#define _DEFAULT_ITEM_PENCOLOR_		0xFF000000	// 默认画笔色：黑色
#define _DEFAULT_ITEM_TEXTCOLOR_	0xFF000000	// 默认文本色：黑色

class itobj
{
	_DECLARE_ITOBJ_(itobj)
public:
	//typedef gtc::itobj	*PtrType;
	typedef std::vector<itobj *> PtrArrayType;
public:
	enum BETrackerState { TSNormal, TSSelected, TSActive };
	// 
	typedef enum BEItemProp{
		BEItemPropText=1,BEItemPropTextColor,BEItemPropBackColor,BEItemPropForeColor,BEItemPropAutoline,BEItemPropVisible,BEItemPropAlign,
		BEItemPropBorderStyle,
		BEItemPropPen,BEItemPropPenWidth/*,BEItemPropPenColor*/,BEItemPropPenStyle,
		BEItemPropBrush/*,BEItemPropBrushColor*/,BEItemPropBrushStyle,BEItemPropBrushHatch,
		BEItemPropFont,
		BEItemPropPosition,BEItemPropPositionX,BEItemPropPositionY,BEItemPropPositionWidth,BEItemPropPositionHeight,
		BEItemPropGroupflag
	}BEItemProp;
public:
	itobj();
	itobj(rect position);
	itobj(const itobj &item);
	itobj& operator=(const itobj &item);
	virtual ~itobj();
	// 描述
	std::wstring description(int indent = 4) const;
public:
	// 静态的static
	/*****************************************************************************************
		<< --- static itobj::exist	2014-03-02 --- >>
		说明：查看指定对象在集合中是否存在
		参数：
		datas	= 对象集合
		pobj	= 被查找的对象
		返回值：是否存在
	*****************************************************************************************/
	static bool exist(PtrArrayType &datas, itobj* pobj)
	{
		for(PtrArrayType::iterator iter = datas.begin(), end = datas.end(); iter != end; ++iter)
			if(*iter == pobj)
				return true;
		return false;
	}

public:
	/*****************************************************************************************
		<< --- itobj::remove	2013-11-29 --- >>
		说明：释放自己
		参数：
		返回值：
	*****************************************************************************************/
	virtual void remove() {
		delete this;
	}

	/*****************************************************************************************
		<< --- itobj::clone	2013-11-29 --- >>
		说明：克隆自己
		参数：
		返回值：克隆对象
	*****************************************************************************************/
	virtual itobj* clone() {	
		itobj* pitem = new itobj(*this);
		return pitem;
	}
	/*****************************************************************************************
		<< --- itobj::getHandleCount	2013-11-29 --- >>
		说明：本对象选中时显示的定位块数；可通过定位点调整对象位置、大小...
		参数：
		返回值：定位点数
	*****************************************************************************************/
	virtual int32_t getHandleCount();
	/*****************************************************************************************
		<< --- itobj::getHandle	2013-11-29 --- >>
		说明：得到指定定位块的中心点（逻辑单位）
		参数：
		vhandle	= 定位块索引           
				1-----2-----3
				|			|
				8			4
				|			|
				7-----6-----5
		返回值：定位块的中心点
	*****************************************************************************************/
	virtual point getHandle(int32_t vhandle);
	// 对象与vrect的交集算法, 如果vpsubobj不为空就返回对子对象的引用
	/*****************************************************************************************
		<< --- itobj::intersects	2013-11-29 --- >>
		说明：对象与vrect的是否有交集
		参数：
		vrect	= 被查验区域
		返回值：是否有交集
	*****************************************************************************************/
	virtual bool intersects(const rect& vrect)
	{
		rect fixed = m_bsPosition;
		rect rt = vrect;
		//fixed.normalizeRect();
		//rt.normalizeRect();
		return !(rt & fixed);//.isEmptyArea();//.isEmpty();
	}
public:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// WIN32 的虚继承
#if defined(_WIN32)
	// 
	virtual void draw(CDC *pdc);
	/*****************************************************************************************
		<< --- itobj::getHandleCursor	2013-12-11 --- >>
		说明：得到指定定位块光标
		参数：
		nHandle		= 定位块索引
		返回值：定位块光标
	*****************************************************************************************/
	virtual HCURSOR getHandleCursor(int nHandle);
	/*****************************************************************************************
		<< --- itobj::drawTracker	2013-12-09 --- >>
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
		<< --- itobj::drawTracker	2013-12-09 --- >>
		说明：绘制定位块所在区域
		参数：
		vpdc		= 绘图环境
		vstate		= 定位块的状态
		vbkcolor	= 块的填充色
		返回值：
	*****************************************************************************************/
	virtual void drawTracker(CDC* pdc, BETrackerState vstate, uint32_t vbkcolor = 0xFF00FF00);
	/*****************************************************************************************
		<< --- itobj::moveHandleTo	2013-11-30 --- >>
		说明：通过指定定位块改变对象尺寸
		参数：
		nHandle	= 被操作的定位块索引
		point	= 定位块新坐标
		pView		= 被移动对象所属视图
		返回值：
	*****************************************************************************************/
	virtual void moveHandleTo(int nHandle, CPoint point, acm::CNPainterView *pView);
	/*****************************************************************************************
		<< --- itobj::moveTo	2013-11-30 --- >>
		说明：将对象pobj移到新位置
		参数：
		position	= 新位置
		pView		= 被移动对象所属视图
		返回值：
	*****************************************************************************************/
	virtual void moveTo(const CRect &position, acm::CNPainterView *pView);
	/*****************************************************************************************
		<< --- static itobj::allocPropertySectionArray	2013-12-06 --- >>
		说明：分配对象属性
		参数：
		返回值：
	*****************************************************************************************/
	static std::size_t allocPropertySectionArray(itobj* pitem, bool isShare);
	virtual std::size_t allocPropertySectionArray(bool isShare) {
		return allocPropertySectionArray(this, isShare);
	}
	void freePropertys();
	/*****************************************************************************************
		<< --- itobj::updatePropertyValue	2013-12-06 --- >>
		说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
		参数：
		pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
		返回值：更改成功
	*****************************************************************************************/
	virtual bool updatePropertyValue(itobjProp *pprop);
	/*****************************************************************************************
		<< --- itobj::messageProperty	2013-12-24 --- >>
		说明：更新指定属性标记的属性值，并将消息WM_ME_ITEM_PROPERTY_CHANGED发送给属性编辑窗口
		参数：
		typeKey		= 对象类型；如：BEDrawShapeType
		vecItemKey	= 需要更新的属性标记
		返回值：
	*****************************************************************************************/
	virtual void messageProperty(int32_t typeKey, std::vector<int32_t> &vecItemKey);

	/*****************************************************************************************
		<< --- itobj::findProperty	2014-07-26 --- >>
		说明：查找指定属性
		参数：
		typeKey		= 对象类型；如：BEDrawShapeType
		itemKey		= 属性标记
		返回值：
			!=null 属性对象,否则为空
	*****************************************************************************************/
	itobjProp* findProperty(int32_t typeKey, int32_t itemKey);
	/*****************************************************************************************
		<< --- itobj::existsProperty	2014-09-06 --- >>
		说明：查找指定属性是否存在
		参数：
		pProp		= 被查找的属性
		返回值：是否存在
	*****************************************************************************************/
	bool existsProperty(const itobjProp *pProp) const;
#endif
	//// 得到标示点的逻辑坐标范围????????????
	//virtual rect GetHandleRect(int vhandle, CDC* vpdc);
	//// 绘制条目??????????????????????
	//virtual void Draw(CDC* vpdc);
	//// 绘制选中标记????????????????
	//virtual void DrawTracker(CDC* vpdc, BETrackerState vstate, uint32_t vbkcolor = 0xFF00FF00);
	//// 返回对象选中标记点的光标形状??????????????
	//virtual HCURSOR GetHandleCursor(int vhandle);
	//// 通过移动标示点改变对象的尺寸??????????????
	//virtual void MoveHandleTo(int vhandle, point vpoint, CView* vpview);
	//// 改变对象的位置??????????????
	//virtual void MoveTo(const rect& vposition, CView* vpview);
	//// 检测点vpoint所在的对象标示点，并返回该点索引??????????????
	//virtual int HitTest(point vpoint, bool visselected, CDC* vpdc);
public:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// WIN32 公共方法
#if defined(_WIN32)
	/*****************************************************************************************
		<< --- itobj::getHandleRect	2013-12-06 --- >>
		说明：得到定位块所在区域
		参数：
		handle	= 定位块索引，基于索引1
		pview	= 本绘图对象所在视图
		返回值：定位块区域（逻辑坐标）
	*****************************************************************************************/
	rect getHandleRect(int32_t handle, CDC *pdc);
	rect getHandleRect(int32_t handle, acm::CNPainterView *pview);
#endif

protected:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// WIN32 protected
//#if defined(_WIN32)
//	// 
//	/*****************************************************************************************
//		<< --- itobj::drawBorder	2013-12-23 --- >>
//		说明：根据边界样式和画笔绘制边框
//		参数：
//		pdc	= 绘图环境
//		返回值：定位块区域（逻辑坐标）
//	*****************************************************************************************/
//	void drawBorder(CDC *pdc);
//
//#endif
public:
	// static
	/*****************************************************************************************
		<< --- static itobj::findCommunityBasecalss	2013-12-12 --- >>
		说明：查找指定对象组的公共基类类型标示
		参数：
		vecItems	= 被筛选的对象数组
		返回值：绘图对象类型
	*****************************************************************************************/
	static BSItobjSchema* findCommunityBasecalss(PtrArrayType &vecItems);
public:
	std::wstring	getText() const {						return m_wsText;						}
	void			setText(const wchar_t *ptext) {			m_wsText = ptext==nullptr?L"":ptext;	}
	void			setText(const std::wstring &text) {		m_wsText = text;						}
	uint32_t		getBackColor() const {					return m_bsBrush.backColor;				}
	void			setBackColor(uint32_t color) {			m_bsBrush.backColor = color;			}
	uint32_t		getForeColor() const {					return m_bsPen.color;					}
	void			setForeColor(uint32_t color) {			
		m_bsPen.color = color;
		m_bsBrush.foreColor = color;
	}
	uint32_t		getTextColor() const {					return m_clrTextColor;					}
	void			setTextColor(uint32_t color) {			m_clrTextColor = color;					}
	bool			isAutoLine() const {					return m_bAutoLine;						}
	void			setAutoLine(bool isauto) {				m_bAutoLine = isauto;					}
	//int32_t			getFontSize() const {					return m_iFontSize;						}
	//void			setFontSize(int32_t fontsize) {			m_iFontSize = fontsize;					}
	bool			isVisible() const {						return m_bVisible;						}
	void			setVisible(bool visible) {				m_bVisible = visible;					}
	BEAlignMode getAlignMode() const {					return m_beAlign;						}
	void			setAlignMode(BEAlignMode align) {	m_beAlign = align;						}

	BEBorderStyle	getBorderStyle() const {				return m_beBorderStyle;					}
	void			setBorderStyle(BEBorderStyle borderStyle) {
		m_beBorderStyle = borderStyle;
		if(m_beBorderStyle==BEBorderStyleNone)
			m_bsPen.style = BEPenStyleNull; /*NULL*/
	}
	BSPen&			getPen() {								return m_bsPen;							}
	void			setPen(const BSPen &pen) {				m_bsPen = pen;							}
	BSBrush&		getBrush() {							return m_bsBrush;						}
	void			setBrush(const BSBrush &brush) {		m_bsBrush = brush;						}
	BSFont&			getFont() {								return m_bsFont;						}
	void			setFont(const BSFont &font) {			m_bsFont = font;						}
	rect&			getPosition() {							return m_bsPosition;					}
	void			setPosition(const rect &position) {		m_bsPosition = position;				}

	uint32_t		getGroupFlag() const {					return m_uiGroupFlag;					}
	void			setGroupFlag(uint32_t group) {			m_uiGroupFlag = group;					}
	//uint32_t		getZoom() const {						return m_uiZoom;						}
	//void			setZoom(uint32_t zoom) {				m_uiZoom = zoom;						}
	//bool			isVisibleBorder() const {				return m_bBorder;						}
	//void			setBorderVisible(bool visible) {		m_bBorder = visible;					}
	itobjPropSection::PtrArrayType& getPropertys() {		return m_vecPropertySection;			}

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_wsText;  
		ar & m_clrTextColor;
		ar & m_bAutoLine;
		ar & m_bVisible;
		ar & m_beAlign;
		ar & m_beBorderStyle;
		ar & m_bsPen;
		ar & m_bsBrush;
		ar & m_bsFont;
		ar & m_bsPosition;
		ar & m_uiGroupFlag;
		//ar & m_vecPropertySection;
	}
#endif	
public:
	static int32_t		mc_iTrackerSize;	// 定位块的宽和高7（单位：像素）
protected:
	std::wstring	m_wsText;		// 显示文本
	//uint32_t		m_clrBackColor;	// 背景色=画刷填充色（定义取消了，但属性方法保留）
	//uint32_t		m_clrForeColor;	// 前景色=画笔色和画刷线条色（定义取消了，但属性方法保留）
	uint32_t		m_clrTextColor;	// 文本色
	bool			m_bAutoLine;	// 文本超出范围后是否自动换行
	//int32_t			m_iFontSize;	// 字体的尺寸
	bool			m_bVisible;		// 对象是否可见
	BEAlignMode		m_beAlign;	// 对其方式
	
	BEBorderStyle	m_beBorderStyle;// 边框样式
	BSPen			m_bsPen;		// 画笔，对应于边框样式的颜色、线形、及其宽度

	BSBrush			m_bsBrush;		// 画刷，只存在两种情况：1、白色背景（空刷）2、有色背景
	BSFont			m_bsFont;		// 字体
	rect			m_bsPosition;	// 外围尺寸范围

	uint32_t		m_uiGroupFlag;	// 对象分组标志; 0=无分组，否则为分组标记
	//uint32_t		m_uiZoom;		// 缩放比例，原始比例为100
	//rect			m_bsOldPosition;// 次值不需要序列化
	//bool			m_bBorder;		// 是否有边框

	itobjPropSection::PtrArrayType	m_vecPropertySection;	// 属性分组;在initPropertySectionArray()中初始化,需要释放

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjRect	2013-11-24 --- >>
//	绘制：矩形，圆角矩形，椭圆，线
////////////////////////////////////////////////////////////////////////////////////////////////////////
class itobjRect : public itobj
{
	_DECLARE_ITOBJ_(itobjRect)
protected:
	itobjRect();
	typedef enum BERectProp {
		BERectPropShape=1,BERectPropRoundness
	}BERectProp;
public:
	enum BEShape { BEShapeRectangle, BEShapeRoundRectangle, BEShapeEllipse, BEShapeLine };	// 矩形，圆角矩形，椭圆，线
	std::wstring toStringBEShape(BEShape shape) const {
		switch (shape)
		{
		case BEShapeRectangle:
			return L"BEShapeRectangle";
			break;
		case BEShapeRoundRectangle:
			return L"BEShapeRoundRectangle";
			break;
		case BEShapeEllipse:
			return L"BEShapeEllipse";
			break;
		case BEShapeLine:
			return L"BEShapeLine";
			break;
		default:
			break;
		}
		return L"";
	}

public:
	itobjRect(const rect& vrect);
	itobjRect(const rect& vrect, BEShape vshape, point vround = point(16, 16 ));

	itobjRect(const itobjRect& vipsql);
	itobjRect& operator =(const itobjRect& vipsql);
	virtual ~itobjRect();
	// 描述
	std::wstring description(int indent = 4) const;
	virtual itobj* clone(void){					
		itobj* prect = new itobjRect(*this);
		return prect;
	}

public:
#if defined(_WIN32)
	virtual void draw(CDC *pdc);
	/*****************************************************************************************
		<< --- itobjRect::getHandleCursor	2013-12-11 --- >>
		说明：得到指定定位块光标
		参数：
		nHandle		= 定位块索引
		返回值：定位块光标
	*****************************************************************************************/
	virtual HCURSOR getHandleCursor(int nHandle);
	/*****************************************************************************************
		<< --- itobjRect::moveHandleTo	2013-11-30 --- >>
		说明：通过指定定位块改变对象尺寸
		参数：
		nHandle	= 被操作的定位块索引
		point	= 定位块新坐标
		pView		= 被移动对象所属视图
		返回值：
	*****************************************************************************************/
	virtual void moveHandleTo(int nHandle, CPoint point, acm::CNPainterView *pView);
	/*****************************************************************************************
		<< --- itobjRect::drawTracker	2013-12-09 --- >>
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
		<< --- itobjRect::drawTracker	2013-12-09 --- >>
		说明：绘制定位块所在区域
		参数：
		vpdc		= 绘图环境
		vstate		= 定位块的状态
		vbkcolor	= 块的填充色
		返回值：
	*****************************************************************************************/
	virtual void drawTracker(CDC* pdc, BETrackerState vstate, uint32_t vbkcolor = 0xFF00FF00);
	/*****************************************************************************************
		<< --- static itobjRect::allocPropertySectionArray	2013-12-06 --- >>
		说明：分配对象属性
		参数：
		返回值：
	*****************************************************************************************/
	static std::size_t allocPropertySectionArray(itobj* pitem, bool isShare);
	virtual std::size_t allocPropertySectionArray(bool isShare) {
		return allocPropertySectionArray(this, isShare);
	}
	/*****************************************************************************************
		<< --- itobjRect::updatePropertyValue	2013-12-06 --- >>
		说明：根据属性对话框值更新对象值，具体哪个对象值通过typeKey和itemKey确定
		参数：
		pprop	= 属性值，被更新的属性根据pprop的类型键和条目键确定
		返回值：
	*****************************************************************************************/
	virtual bool updatePropertyValue(itobjProp *pprop);
#endif
public:
	void						setShape(BEShape shape)						{	m_beShape = shape;				}
	BEShape						getShape(void)								{	return m_beShape;				}
	void						setRoundCorners(point vpoint)				{	m_roundness = vpoint;			}
	point						getRoundCorners(void)						{	return m_roundness;				}

	virtual int32_t getHandleCount();
	virtual point getHandle(int32_t vhandle);
	//virtual HCURSOR GetHandleCursor(int vhandle);
	//virtual void MoveHandleTo(int vhandle, CPoint vpoint, CView* vpview);
	virtual bool intersects(const rect& vrect);

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<itobj>(*this);
		ar & m_beShape;  
		ar & m_roundness;
	}
#endif
protected:
	BEShape	m_beShape;
	point m_roundness; // for roundRect corners

	//friend class CNToolRect;

};


}
#endif