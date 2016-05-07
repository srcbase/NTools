#pragma once

//#include "AC/basedef.h"
#include "AC/drawdef.h"
#include <algorithm>
//#include "D:/NTools/boost/serialization/access.hpp"
//#include "../boost/archive/binary_wiarchive.hpp"
//#include "../boost/archive/binary_woarchive.hpp"
#if defined(BOOST_SERIALIZE)
#include "../boost/serialization/base_object.hpp"

#endif
#if defined(_WIN32)
	#include <GdiPlus.h>
	#include <WinGDI.h>
#endif
namespace gtc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	point 类	2015-1-23
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 点
class point
{
public:
	point()
		: x(0), y(0)
	{}
	point(int32_t _x, int32_t _y)
		: x(_x), y(_y)
	{
	}
	point(const point &_point)
		: x(_point.x), y(_point.y)
	{
	}
#if defined(_AFXDLL)
	point(const CPoint &_point)
		: x(_point.x), y(_point.y)
	{
	}
#endif
	point& operator=(const point &_point)
	{
		if(this != &_point)
		{
			x = _point.x;
			y = _point.y;
		}
		return *this;
	}
	~point() 
	{
	}
public:
	std::wstring toString() const 
	{
		std::wostringstream out;
		out << L"x:" << x << L" y:" << y << std::ends;
		return out.str();
	}
	point operator+(const point& _point) const
	{
		return point(x + _point.x, y + _point.y);
	}

	point operator-(const point& _point) const
	{
		return point(x - _point.x, y - _point.y);
	}
	bool operator==(const point& _point) const 
	{
		return (x == _point.x) && (y == _point.y);
	}
	bool equals(const point& _point)
	{
		return operator==(_point);
	}
public:
#if defined(_AFXDLL)
	CPoint toMSPoint() const {
		return CPoint(x, y);
	}
	Gdiplus::Point toGdiplus() const {
		return Gdiplus::Point(x, y);
	}
	Gdiplus::PointF toGdiplusF() const {
		return Gdiplus::PointF((Gdiplus::REAL)x, (Gdiplus::REAL)y);
	}
	// 偏移对象，vx>0向右，vy>0向下
	void offset(int32_t offx, int32_t offy)
	{
#if defined(WORK_AXIS_CAD)
		x += offx;
		y -= offy;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		x += offx;
		y += offy;
#endif
	}
#endif
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;  
		ar & y;
	}
#endif
public:
	int32_t x;	// 指定点的 X 坐标
	int32_t y;	// 指定点的 Y 坐标

};
class pointf
{
public:
	pointf()
		: x(0.0f), y(0.0f)
	{}
	pointf(float _x, float _y)
		: x(_x), y(_y)
	{
	}
	pointf(const pointf &_point)
		: x(_point.x), y(_point.y)
	{
	}
#if defined(_AFXDLL)
	pointf(const CPoint &_point)
		: x((float)_point.x), y((float)_point.y)
	{
	}
#endif
	pointf& operator=(const pointf &_point)
	{
		if(this != &_point)
		{
			x = _point.x;
			y = _point.y;
		}
		return *this;
	}
	~pointf() 
	{
	}
public:
	std::wstring toString() const
	{
		std::wostringstream out;
		out << L"x:" << x << L" y:" << y << std::ends;
		return out.str();
	}
	pointf operator+(const pointf& _point) const
	{
		return pointf(x + _point.x, y + _point.y);
	}

	pointf operator-(const pointf& _point) const
	{
		return pointf(x - _point.x, y - _point.y);
	}
	bool operator==(const pointf& _point) const 
	{
		return (x == _point.x) && (y == _point.y);
	}
	bool equals(const pointf& _point)
	{
		return operator==(_point);
	}
public:
#if defined(_AFXDLL)
	CPoint toMSPoint() const {
		return CPoint((int)x, (int)y);
	}
	Gdiplus::Point toGdiplus() const {
		return Gdiplus::Point((INT)x, (INT)y);
	}
	Gdiplus::PointF toGdiplusF() const {
		return Gdiplus::PointF(x, y);
	}
	// 偏移对象，vx>0向右，vy>0向下
	void offset(float offx, float offy)
	{
#if defined(WORK_AXIS_CAD)
		x += offx;
		y -= offy;
#elif defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		x += offx;
		y += offy;
#endif
	}
#endif

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;  
		ar & y;
	}
#endif
public:
	float x;	// 指定点的 X 坐标
	float y;	// 指定点的 Y 坐标

};
// 区域大小
class size
{
public:
	size() : width(0), height(0)
	{
	}
	size(int32_t _width, int32_t _height)
		: width(_width), height(_height)
	{
	}
	size(const size &size)
		: width(size.width), height(size.height)
	{
	}
	size& operator=(const size &sz)
	{
		if(this != &sz)
		{
			width = sz.width;
			height = sz.height;
		}
		return *this;
	}
	~size()
	{
	}
public:
	std::wstring toString() const
	{
		std::wostringstream out;
		out << L"width:" << width << L" height:" << height << std::ends;
		return out.str();
	}
	size operator+(const size& sz) const
	{
		return size(width + sz.width, height + sz.height);
	}

	size operator-(const size& sz) const
	{
		return size(width - sz.width, height - sz.height);
	}
	bool operator==(const size &sz) const
	{
		return (width == sz.width) && (height == sz.height);
	}
	bool equals(const size& sz) const
	{
		return operator==(sz);
	}

	bool empty() const
	{
		return (width == 0 && height == 0);
	}
public:
#if defined(_WIN32)
	size(const SIZE &sz) :width(sz.cx), height(sz.cy) {
	}
	size& operator=(const SIZE &sz) {
		width = sz.cx;
		height = sz.cy;
		return *this;
	}
	operator SIZE() const {
		SIZE sz = {width, height};
		return sz;
	}
#endif
#if defined(_AFXDLL)
	CSize toMSSize() const {
		return CSize(width, height);
	}
	Gdiplus::Size toGdiplus() const {
		return Gdiplus::Size(width, height);
	}
	Gdiplus::SizeF toGdiplusF() const {
		return Gdiplus::SizeF((Gdiplus::REAL)width, (Gdiplus::REAL)height);
	}

	size(const CSize &sz)
		: width(sz.cx), height(sz.cy)
	{
	}
	size& operator=(const CSize &sz)
	{
		width = sz.cx;
		height = sz.cy;
		return *this;
	}
#endif

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & width;  
		ar & height;
	}
#endif
public:
	int32_t width;		// 区域宽
	int32_t height;		// 区域高

};
class sizef
{
public:
	sizef() : width(0.0f), height(0.0f)
	{
	}
	sizef(float _width, float _height)
		: width(_width), height(_height)
	{
	}
	sizef(const sizef &sz)
		: width(sz.width), height(sz.height)
	{
	}
	sizef& operator=(const sizef &sz)
	{
		if(this != &sz)
		{
			width = sz.width;
			height = sz.height;
		}
		return *this;
	}
	~sizef()
	{
	}
public:
	std::wstring toString() const
	{
		std::wostringstream out;
		out << L"width:" << width << L" height:" << height << std::ends;
		return out.str();
	}
	sizef operator+(const sizef& sz) const
	{
		return sizef(width + sz.width, height + sz.height);
	}

	sizef operator-(const sizef& sz) const
	{
		return sizef(width - sz.width, height - sz.height);
	}
	bool operator==(const sizef &sz) const
	{
		return (width == sz.width) && (height == sz.height);
	}
	bool equals(const sizef& sz) const
	{
		return operator==(sz);
	}

	bool empty() const
	{
		return (width == 0.0f && height == 0.0f);
	}
public:
#if defined(_AFXDLL)
	CSize toMSSize() const {
		return CSize((int)width, (int)height);
	}
	Gdiplus::Size toGdiplus() const {
		return Gdiplus::Size((INT)width, (INT)height);
	}
	Gdiplus::SizeF toGdiplusF() const {
		return Gdiplus::SizeF(width, height);
	}

	sizef(const CSize &sz)
		: width((float)sz.cx), height((float)sz.cy)
	{
	}
	sizef& operator=(const CSize &sz)
	{
		width = (float)sz.cx;
		height = (float)sz.cy;
		return *this;
	}
#endif

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & width;  
		ar & height;
	}
#endif

public:
	float width;		// 区域宽
	float height;		// 区域高

};

// 定义矩形的左上角的坐标和尺寸大小
// 1、采用WORK_AXIS_CAD坐标系时，绘图区在第一象限，向右，向上为递增
//		 ^
//		 |
// height|
//		 |
//		 ----------->
//	   (0,0)   width
//		
// 2、采用WORK_AXIS_DOC || WORK_AXIS_NORMAL坐标系时
// 
//	   (0,0)   width
//		 ----------->
//		 |
// height|
//		 |
//       V
//	
class rect
{
public:
	rect()
		: x(0), y(0), width(0), height(0)
	{
	}
	rect(point _point, size _size)
		: x(_point.x), y(_point.y), width(_size.width), height(_size.height)
	{
	}
	rect(int32_t _x, int32_t _y, int32_t _width, int32_t _height)
		: x(_x), y(_y), width(_width), height(_height)
	{
	}
	rect(const rect &rt)
		: x(rt.x), y(rt.y), width(rt.width), height(rt.height)
	{
	}
	rect& operator=(const rect &rt)
	{
		if(this != &rt)
		{
			x = rt.x;
			y = rt.y;
			width = rt.width;
			height = rt.height;
		}
		return *this;
	}
	rect* clone() const
	{
		return new rect(*this);
	}
	~rect() {}
public:
	size getSize() const
	{
		return size(width, height);
	}
	size setSize(size szNew)
	{
		size sz = getSize();
		width = szNew.width;
		height = szNew.height;
		return sz;
	}
	size setSize(int32_t cx, int32_t cy) 
	{
		size sz = getSize();
		width = cx;
		height = cy;
		return sz;
	}
	std::wstring toString() const 
	{
		std::wostringstream out;
		out << L"x:" << x << L" y:" << y << L" width:" << width << L" height:" << height << std::ends;
		return out.str();
	}
	void clear()
	{
		x = y = 0;
		width = height = 0;
	}
	void setRect(int32_t _x, int32_t _y, int32_t _width, int32_t _height)
	{
		x = _x;
		y = _y;
		width = _width;
		height = _height;
	}
	void setRect(point pt, size sz)
	{
		x = pt.x;
		y = pt.y;
		width = sz.width;
		height = sz.height;
	}
	int32_t getLeft() const {		
		int32_t left = 0;
#if defined(WORK_AXIS_CAD)
		left = x;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left = x;
#endif
		return left;						
	}
	int32_t getTop() const {		
		int32_t top = 0;
#if defined(WORK_AXIS_CAD)
		top = y + height;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		top = y;
#endif
		return top;						
	}
	int32_t getRight() const {
		int32_t right = 0;
#if defined(WORK_AXIS_CAD)
		right = x + width;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		right = x + width;
#endif
		return right;				
	}
	int32_t getBottom() const {
		int32_t bottom = 0;
#if defined(WORK_AXIS_CAD)
		bottom = y;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		bottom = y + height;
#endif
		return bottom;				

	}
	point topLeft() const
	{
		return point(getLeft(), getTop());
	}
	point bottomRight() const
	{
		return point(getRight(), getBottom());
	}
	point center() const
	{
		return point(x + width/2, y + height/2);
	}
	// 是否为空
	bool isEmptyArea() const 
	{
		return width <= 0 || height <= 0;
	}
	bool operator==(const rect &rt) const
	{
		return x == rt.x && y == rt.y && width == rt.width && height == rt.height;
	}
	bool equals(const rect &rt) const
	{
		return operator==(rt);
	}
	bool contains(int32_t _x, int32_t _y) const
	{
		return x <= _x && _x < x + width 
			&& y <= _y && _y < y + height;
	}
	bool contains(const point &pt) const 
	{
		return contains(pt.x, pt.y);
	}
	bool contains(const rect &rt) const
	{
		return x <= rt.x && rt.getRight() < getRight()
			&& y <= rt.y && rt.getBottom() < getBottom();
	}
	// 查看点是否在对象内
	bool inRect(const point &pt) const
	{
		return pt.x >= x && pt.x < x + width
			&& pt.y >= y && pt.y < y + height;
	}
	bool inRect(rect &rt) const 
	{
		return inRect(rt.topLeft()) && inRect(rt.bottomRight());
	}
	void inflate(int32_t dx, int32_t dy){
		x -= dx;
		y -= dy;
		width += 2*dx;
		height += 2*dy;
	}
	void inflate(const point& pt)
	{
		inflate(pt.x, pt.y);
	}
	void deflate(int32_t dx, int32_t dy){
		x += dx;
		y += dy;
		width -= 2*dx;
		height -= 2*dy;
	}
	void offset(int32_t dx, int32_t dy) {
		x += dx;
		y += dy;
	}
	void offset(const point& pt) {
		offset(pt.x, pt.y);
	}
public:
	// 交集
	// c = a & b
	static bool intersect(rect &c, const rect &a, const rect &b)
	{
		int32_t right = 0;
		int32_t bottom = 0;
		int32_t left = 0;
		int32_t top = 0;
#if defined(WORK_AXIS_CAD)
		left = max(a.getLeft(), b.getLeft());
		top = min(a.getTop(), b.getTop());
		right = min(a.getRight(), b.getRight());
		bottom = max(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = bottom;
		c.width = right - left;
		c.height = top - bottom;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left = max(a.getLeft(), b.getLeft());
		top = max(a.getTop(), b.getTop());
		right = min(a.getRight(), b.getRight());
		bottom = min(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = top;
		c.width = right - left;
		c.height = bottom - top;
#endif
		return !c.isEmptyArea();
	}
	bool intersect(const rect &rt)
	{
		return intersect(*this, *this, rt);
	}
	bool operator&(const rect &rt) 
	{
		return intersect(*this, *this, rt);
	}
	bool operator&=(const rect &rt) 
	{
		return *this & rt;
	}
	// 合集
	static bool Union(rect &c, const rect &a, const rect &b)
	{
		int32_t right = 0;
		int32_t bottom = 0;
		int32_t left = 0;
		int32_t top = 0;
#if defined(WORK_AXIS_CAD)
		left = min(a.getLeft(), b.getLeft());
		top = max(a.getTop(), b.getTop());
		right = max(a.getRight(), b.getRight());
		bottom = min(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = bottom;
		c.width = right - left;
		c.height = top - bottom;
		
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left = min(a.getLeft(), b.getLeft());
		top = min(a.getTop(), b.getTop());
		right = max(a.getRight(), b.getRight());
		bottom = max(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = top;
		c.width = right - left;
		c.height = bottom - top;
		
#endif
		return !c.isEmptyArea();
	}
	bool Union(const rect &rt)
	{
		return Union(*this, *this, rt);
	}

	void join(const rect &rt)
	{
		if(x > rt.x) x = rt.x;
		if(y > rt.y) y = rt.y;
		if(width < rt.width) width = rt.width;
		if(height < rt.height) height = rt.height;
	}
	Gdiplus::Rect toGdiplus() const {
		return *this;// Gdiplus::Rect(x, y, width, height);
	}
	Gdiplus::RectF toGdiplusF() const {
		return *this;//Gdiplus::RectF((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)width, (Gdiplus::REAL)height);
	}

	operator Gdiplus::Rect() const {
		return Gdiplus::Rect(x, y, width, height);
	}
	operator Gdiplus::RectF() const {
		return Gdiplus::RectF((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)width, (Gdiplus::REAL)height);
	}

public:
#if defined(_WIN32)
	operator RECT() const {
		RECT rc = {getLeft(), getTop(), getRight(), getBottom()};
		return rc;
	}
	rect(const RECT &rc) {
		*this = rc;
	//#if defined(WORK_AXIS_CAD)
	//	x = rc.left;
	//	y = rc.bottom;
	//	width = rc.right - rc.left;
	//	height = rc.bottom - rc.top;
	//#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
	//	x = rc.left;
	//	y = rc.top;
	//	width = rc.right - rc.left;
	//	height = rc.bottom - rc.top;
	//#endif
	}
	rect& operator=(const RECT &rt)
	{
	#if defined(WORK_AXIS_CAD)
		x = max(rt.left, rt.right);
		y = min(rt.top, rt.bottom);
		width = rectWidth(rt);
		height = rectHeight(rt);
	#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		x = min(rt.left, rt.right);
		y = min(rt.top, rt.bottom);
		width = rectWidth(rt);
		height = rectHeight(rt);
	#endif
		return *this;
	}

#endif
#if defined(_AFXDLL)
	operator CRect() const {
		CRect rt;
		rt.SetRect(getLeft(), getTop(), getRight(), getBottom());
		return rt;
	}

	rect(const CRect &rt)
	{
		CRect rc(rt);
		rc.NormalizeRect();
#if defined(WORK_AXIS_CAD)
		x = rc.left;
		y = rc.bottom;
		width = rc.Width();
		height = rc.Height();
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		x = rc.left;
		y = rc.top;
		width = rc.Width();
		height = rc.Height();
#endif
	}

#endif

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;  
		ar & y;
		ar & width;
		ar & height;
	}
#endif

public:
	// WORK_AXIS_CAD:左下角(x,y)，向右、向上正增长；否则左上角(x,y)，向右、向下正增长
	int32_t x;			// 
	int32_t y;			// 
	int32_t width;		// 
	int32_t height;		// 

};

class rectf
{
public:
	rectf()
		: x(0.0f), y(0.0f), width(0.0f), height(0.0f)
	{
	}
	rectf(pointf _point, sizef _size)
		: x(_point.x), y(_point.y), width(_size.width), height(_size.height)
	{
	}
	rectf(float _x, float _y, float _width, float _height)
		: x(_x), y(_y), width(_width), height(_height)
	{
	}
	rectf(const rectf &rt)
		: x(rt.x), y(rt.y), width(rt.width), height(rt.height)
	{
	}
	rectf& operator=(const rectf &rt)
	{
		if(this != &rt)
		{
			x = rt.x;
			y = rt.y;
			width = rt.width;
			height = rt.height;
		}
		return *this;
	}

	rectf* clone() const
	{
		return new rectf(*this);
	}
	~rectf() {}
public:
	std::wstring toString() const 
	{
		std::wostringstream out;
		out << L"x:" << x << L" y:" << y << L" width:" << width << L" height:" << height << std::ends;
		return out.str();
	}
	void setRect(float _x, float _y, float _width, float _height)
	{
		x = _x;
		y = _y;
		width = _width;
		height = _height;
	}
	void setRect(pointf pt, sizef sz)
	{
		x = pt.x;
		y = pt.y;
		width = sz.width;
		height = sz.height;
	}
	float getLeft() const {		
		float left = 0.0f;
#if defined(WORK_AXIS_CAD)
		left = x;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left = x;
#endif
		return left;						
	}
	float getTop() const {		
		float top = 0.0f;
#if defined(WORK_AXIS_CAD)
		top = y + height;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		top = y;
#endif
		return top;						
	}
	float getRight() const {
		float right = 0.0f;
#if defined(WORK_AXIS_CAD)
		right = x + width;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		right = x + width;
#endif
		return right;				
	}
	float getBottom() const {
		float bottom = 0.0f;
#if defined(WORK_AXIS_CAD)
		bottom = y;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		bottom = y + height;
#endif
		return bottom;				

	}
	pointf topLeft() const
	{
		return pointf(getLeft(), getTop());
	}
	pointf bottomRight() const
	{
		return pointf(getRight(), getBottom());
	}
	pointf center() const
	{
		return pointf(x + width/2, y + height/2);
	}
	// 是否为空
	bool isEmptyArea() const 
	{
		return width <= 0 || height <= 0;
	}
	bool operator==(const rectf &rt) const
	{
		return x == rt.x && y == rt.y && width == rt.width && height == rt.height;
	}
	bool equals(const rectf &rt) const
	{
		return operator==(rt);
	}
	bool contains(float _x, float _y) const
	{
		return x <= _x && _x < x + width 
			&& y <= _y && _y < y + height;
	}
	bool contains(const pointf &pt) const 
	{
		return contains(pt.x, pt.y);
	}
	bool contains(const rectf &rt) const
	{
		return x <= rt.x && rt.getRight() < getRight()
			&& y <= rt.y && rt.getBottom() < getBottom();
	}
	// 查看点是否在对象内
	bool inRect(const pointf &pt) const
	{
		return pt.x >= x && pt.x < x + width
			&& pt.y >= y && pt.y < y + height;
	}
	bool inRect(rectf &rt) const 
	{
		return inRect(rt.topLeft()) && inRect(rt.bottomRight());
	}
	void inflate(float dx, float dy){
		x -= dx;
		y -= dy;
		width += 2*dx;
		height += 2*dy;
	}
	void inflate(const pointf& pt)
	{
		inflate(pt.x, pt.y);
	}
	void deflate(float dx, float dy){
		x += dx;
		y += dy;
		width -= 2*dx;
		height -= 2*dy;
	}
	void offset(float dx, float dy) {
		x += dx;
		y += dy;
	}
	void offset(const pointf& pt) {
		offset(pt.x, pt.y);
	}
public:
	// 交集
	// c = a & b
	static bool intersect(rectf &c, const rectf &a, const rectf &b)
	{
		float right = 0.0f;
		float bottom = 0.0f;
		float left = 0.0f;
		float top = 0.0f;
#if defined(WORK_AXIS_CAD)
		left = max(a.getLeft(), b.getLeft());
		top = min(a.getTop(), b.getTop());
		right = min(a.getRight(), b.getRight());
		bottom = max(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = bottom;
		c.width = right - left;
		c.height = top - bottom;
#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left = max(a.getLeft(), b.getLeft());
		top = max(a.getTop(), b.getTop());
		right = min(a.getRight(), b.getRight());
		bottom = min(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = top;
		c.width = right - left;
		c.height = bottom - top;
#endif
		return !c.isEmptyArea();
	}
	bool intersect(const rectf &rt)
	{
		return intersect(*this, *this, rt);
	}
	bool operator&(const rectf &rt) 
	{
		return intersect(*this, *this, rt);
	}
	bool operator&=(const rectf &rt) 
	{
		return *this & rt;
	}
	// 合集
	static bool Union(rectf &c, const rectf &a, const rectf &b)
	{
		float right = 0.0f;
		float bottom = 0.0f;
		float left = 0.0f;
		float top = 0.0f;
#if defined(WORK_AXIS_CAD)
		left = min(a.getLeft(), b.getLeft());
		top = max(a.getTop(), b.getTop());
		right = max(a.getRight(), b.getRight());
		bottom = min(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = bottom;
		c.width = right - left;
		c.height = top - bottom;

#else // defined(WORK_AXIS_DOC) || defined(WORK_AXIS_NORMAL)
		left = min(a.getLeft(), b.getLeft());
		top = min(a.getTop(), b.getTop());
		right = max(a.getRight(), b.getRight());
		bottom = max(a.getBottom(), b.getBottom());
		c.x = left;
		c.y = top;
		c.width = right - left;
		c.height = bottom - top;

#endif
		return !c.isEmptyArea();
	}
	bool Union(const rectf &rt)
	{
		return Union(*this, *this, rt);
	}
public:
#if defined(_AFXDLL)
	CRect toMSRect() const {
		CRect rt;
		rt.SetRect((int)getLeft(), (int)getTop(), (int)getRight(), (int)getBottom());
		return rt;
	}
	Gdiplus::Rect toGdiplus() const {
		return Gdiplus::Rect((INT)x, (INT)y, (INT)width, (INT)height);
	}
	Gdiplus::RectF toGdiplusF() const {
		return Gdiplus::RectF(x, y, width, height);
	}
	operator Gdiplus::RectF() const {
		return toGdiplusF();
	}
#endif

#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;  
		ar & y;
		ar & width;
		ar & height;
	}
#endif


public:
	float x;			// 指定矩形的左上角的 x 坐标
	float y;			// 指定矩形的左上角的 y 坐标
	float width;		// 指定矩形的宽度
	float height;		// 指定矩形的高度

};
// 画笔类型
typedef struct BSPen
{
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & style;  
		ar & width;
		ar & color;
	}
#endif
public:
	BEPenStyle	style;	// 画笔样式
	uint32_t	width;	// 逻辑单位,宽度
	uint32_t	color;	// 画笔颜色
public:
	BSPen() : style(BEPenStyleSolid), width(3), color(0xFE000000) {}
	BSPen(BEPenStyle vstyle, uint32_t vwidth, uint32_t vcolor)
		: style(vstyle), width(vwidth), color(vcolor)
	{
	}
	BSPen(const BSPen &pen)
		: style(pen.style), width(pen.width), color(pen.color)
	{
	}
	BSPen& operator=(const BSPen &pen)
	{
		if(this != &pen)
		{
			style = pen.style;
			width = pen.width;
			color = pen.color;
		}
		return *this;
	}
	~BSPen()
	{
	}
public:
#if defined(_WIN32)
	LOGPEN toMSPen() const {
		LOGPEN lgpen;
		lgpen.lopnColor = color;
		lgpen.lopnStyle = style;
		lgpen.lopnWidth.x = lgpen.lopnWidth.y = width;
		return lgpen;
	}
	/*****************************************************************************************
		<< --- BSPen::toGdiplus	2014-01-09 --- >>
		说明：根据自定义画笔结构生成绘制Gdiplus::Pen*
		参数：
		返回值：根据画笔样式生成Gdiplus::Pen*，调用者负责释放
	*****************************************************************************************/
	Gdiplus::Pen* toGdiplus() const {
		Gdiplus::Pen *ppen = new Gdiplus::Pen(color, (Gdiplus::REAL)width);
		if(style == BEPenStyleNull)
			ppen->SetColor(color & 0x00FFFFFF);	// 透明
		ppen->SetDashStyle(Gdiplus::DashStyle(style));
		return ppen;
	}

#endif
}BSPen;

// 定义了一个实际画刷的样式、颜色和模式
typedef struct BSBrush
{
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & style;  
		ar & backColor;
		ar & foreColor;
		ar & hatch;
	}
#endif
public:
	BEBrushStyle	style;		// 画刷样式
	uint32_t		backColor;	// 画刷背景填充颜色
	uint32_t		foreColor;	// 画刷前景色
	BEHatchStyle	hatch;		// 阴影样式
public:
	BSBrush() : style(BEBrushStyleNull), backColor(0xFEFFFF), foreColor(0xFE000000), hatch(BEHatchStyleHorizontal)
	{
	}
	BSBrush(BEBrushStyle vstyle, uint32_t vforeColor, uint32_t vbackColor, BEHatchStyle vhatch)
		: style(vstyle), foreColor(vforeColor), backColor(vbackColor), hatch(vhatch)
	{
	}
	BSBrush(uint32_t vbackColor)
		: style(BEBrushStyleSolid), foreColor(vbackColor), backColor(vbackColor), hatch(BEHatchStyleHorizontal)
	{
	}
	BSBrush(const BSBrush &brush)
		: style(brush.style), foreColor(brush.foreColor), backColor(brush.backColor), hatch(brush.hatch)
	{
	}
	BSBrush& operator=(const BSBrush &brush)
	{
		if(this != &brush)
		{
			style = brush.style;
			foreColor = brush.foreColor;
			backColor = brush.backColor;
			hatch = brush.hatch;
		}
		return *this;
	}
	~BSBrush()
	{
	}
public:
#if defined(_WIN32)
	LOGBRUSH toMSBrush() const {
		LOGBRUSH lgbrush;
		lgbrush.lbColor = foreColor;
		lgbrush.lbHatch = hatch;
		lgbrush.lbStyle = style;
		return lgbrush;
	}
	/*****************************************************************************************
		<< --- BSBrush::toGdiplus	2014-01-09 --- >>
		说明：根据自定义画刷结构生成绘制Gdiplus::Brush
		参数：
		返回值：根据画刷样式生成SolidBrush或HatchBrush形画刷，调用者负责释放
	*****************************************************************************************/
	Gdiplus::Brush* toGdiplus() const {
		Gdiplus::Brush *pbrush = nullptr;
		switch (style)
		{
		case BEBrushStyleSolid:
			pbrush = new Gdiplus::SolidBrush(backColor);
			break;
		case BEBrushStyleNull:
			pbrush = new Gdiplus::SolidBrush(Gdiplus::Color::Transparent);// backColor & 0x00FFFFFF);
			break;
		case BEBrushStyleHatch:
			pbrush = new Gdiplus::HatchBrush(Gdiplus::HatchStyle(hatch), foreColor, backColor);
			break;
		default:
			break;
		}
		return pbrush;
	}

#endif
}BSBrush;
// 字体类别
typedef enum BEFontSize {
	BEFontSizeZero=0,BEFontSizeZeroSmall,BEFontSizeOne,BEFontSizeOneSmall,BEFontSizeTwo,BEFontSizeTwoSmall,
	BEFontSizeThree,BEFontSizeThreeSmall,BEFontSizeFour,BEFontSizeFourSmall,BEFontSizeFive,BEFontSizeFiveSmall,
	BEFontSizeSix,BEFontSizeSixSmall,BEFontSizeSeven,BEFontSizeEight
}BEFontSize;

// 字体结构
typedef struct BSFont
{
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & iHeight;  
		ar & iWidth;
		ar & iEscapement;
		ar & iOrientation;
		ar & iWeight;
		ar & bItalic;
		ar & bUnderline;
		ar & bStrikeOut;
		ar & nCharSet;
		ar & nOutPrecision;
		ar & nClipPrecision;
		ar & nQuality;
		ar & nPitchAndFamily;
		ar & wcFaceName;
	}
#endif
public:
#define FACENAME_MAX 32
	int32_t	iHeight;		// 以逻辑单位指定字体字符元(character cell)或字符的高度
	int32_t	iWidth;			// 以逻辑单位指定字体字符的平均宽度
	int32_t	iEscapement;	// 以十分之一度为单位指定每一行文本输出时相对于页面底端的角度
	int32_t	iOrientation;	// 以十分之一度为单位指定字符基线相对于页面底端的角度
	int32_t	iWeight;		// 指定字体重量。在Windows中，字体重量这个术语用来指代字体的粗细程度。
							//     lfWeight的范围为0到1000，正常情况下的字体重量为400，粗体为700。如果lfWeight为0，则使用默认的字体重量
	bool	bItalic;		// 为TRUE时使用斜体
	bool	bUnderline;	// 为TRUE时给字体添加下划线
	bool	bStrikeOut;	// 为TRUE时给字体添加删除线
	uint8_t nCharSet;		// 指定字符集
	uint8_t nOutPrecision;	// 指定输出精度
	uint8_t nClipPrecision;	// 指定剪辑精度
	uint8_t nQuality;		// 定义输出质量
	uint8_t nPitchAndFamily;// 指定字体的字符间距和族
	wchar_t wcFaceName[FACENAME_MAX];// 字体名称	
public:
	BSFont()
		: iHeight(-20), iWidth(0), iEscapement(0), iOrientation(0), iWeight(400)
		, bItalic(false), bUnderline(false), bStrikeOut(false)
		, nCharSet(0/*ANSI_CHARSET*/), nOutPrecision(0/*OUT_DEFAULT_PRECIS*/), nClipPrecision(0/*CLIP_DEFAULT_PRECIS*/)
		, nQuality(0/*DEFAULT_QUALITY*/), nPitchAndFamily(0/*DEFAULT_PITCH*/)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", L"宋体");
	}
	BSFont(BEFontSize fontSize)
		: iWidth(0), iEscapement(0), iOrientation(0), iWeight(400)
		, bItalic(false), bUnderline(false), bStrikeOut(false)
		, nCharSet(0/*ANSI_CHARSET*/), nOutPrecision(3/*OUT_STROKE_PRECIS*/), nClipPrecision(2/*CLIP_STROKE_PRECIS*/)
		, nQuality(1/*DRAFT_QUALITY*/), nPitchAndFamily(2/*VARIABLE_PITCH*/)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", L"宋体");
		setFontSize(fontSize);
	}

	BSFont(const std::wstring &fontname, BEFontSize fontSize = BEFontSizeFourSmall)
		: iWidth(0), iEscapement(0), iOrientation(0), iWeight(400)
		, bItalic(false), bUnderline(false), bStrikeOut(false)
		, nCharSet(0/*ANSI_CHARSET*/), nOutPrecision(0/*OUT_DEFAULT_PRECIS*/), nClipPrecision(0/*CLIP_DEFAULT_PRECIS*/)
		, nQuality(0/*DEFAULT_QUALITY*/), nPitchAndFamily(0/*DEFAULT_PITCH*/)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", fontname.c_str());
		setFontSize(fontSize);
	}
	BSFont(const BSFont &font)
		: iHeight(font.iHeight), iWidth(font.iWidth), iEscapement(font.iEscapement), iOrientation(font.iOrientation), iWeight(font.iWeight)
		, bItalic(font.bItalic), bUnderline(font.bUnderline), bStrikeOut(font.bStrikeOut)
		, nCharSet(font.nCharSet), nOutPrecision(font.nOutPrecision), nClipPrecision(font.nClipPrecision)
		, nQuality(font.nQuality), nPitchAndFamily(font.nPitchAndFamily)
	{
		std::swprintf(wcFaceName, FACENAME_MAX, L"%s", font.wcFaceName);
	}
	BSFont& operator=(const BSFont &font)
	{
		if(this != &font)
		{
			iHeight = font.iHeight;
			iWidth = font.iWidth;
			iEscapement = font.iEscapement;
			iOrientation = font.iOrientation;
			iWeight = font.iWeight;
			bItalic = font.bItalic;
			bUnderline = font.bUnderline;
			bStrikeOut = font.bStrikeOut;
			nCharSet = font.nCharSet;
			nOutPrecision = font.nOutPrecision;
			nClipPrecision = font.nClipPrecision;
			nQuality = font.nQuality;
			nPitchAndFamily = font.nPitchAndFamily;
			std::swprintf(wcFaceName, FACENAME_MAX, L"%s", font.wcFaceName);
		}
		return *this;
	}
	~BSFont()
	{
	}
public:
	SBDataType toData() {
		uint8_t *pbuf = (uint8_t*)this;
		SBDataType vec(pbuf, pbuf+sizeof(BSFont));
		return vec;
	}
	bool fromData(const SBDataType &vecData) {
		if(vecData.size() != sizeof(BSFont))
			return false;
		BSFont font;
		std::memcpy((void*)&font, vecData.data(), sizeof(BSFont));
		*this = font;
		return true;
	}
	void setFontSize(BEFontSize fontSize) {
		switch (fontSize)
		{
		case BEFontSizeZero:
			iHeight = -70;
			break;
		case BEFontSizeZeroSmall:
			iHeight = -60;
			break;
		case BEFontSizeOne:
			iHeight = -43;
			break;
		case BEFontSizeOneSmall:
			iHeight = -40;
			break;
		case BEFontSizeTwo:
			iHeight = -37;
			break;
		case BEFontSizeTwoSmall:
			iHeight = -30;
			break;
		case BEFontSizeThree:
			iHeight = -27;
			break;
		case BEFontSizeThreeSmall:
			iHeight = -25;
			break;
		case BEFontSizeFour:
			iHeight = -23;
			break;
		case BEFontSizeFourSmall:
			iHeight = -20;
			break;
		case BEFontSizeFive:
			iHeight = -18;
			break;
		case BEFontSizeFiveSmall:
			iHeight = -15;
			break;
		case BEFontSizeSix:
			iHeight = -13;
			break;
		case BEFontSizeSixSmall:
			iHeight = -11;
			break;
		case BEFontSizeSeven:
			iHeight = -9;
			break;
		case BEFontSizeEight:
			iHeight = -8;
			break;
		default:
			iHeight = -20;
			break;
		}
	}

#if defined(_WIN32)
	void toMSFont(LOGFONT *plgfont, HDC hdc = nullptr) const {
		if(plgfont){
			plgfont->lfHeight = -iHeight;
			plgfont->lfWidth = iWidth;
			plgfont->lfEscapement = iEscapement;
			plgfont->lfWeight = iWeight;
			plgfont->lfItalic = bItalic;
			plgfont->lfUnderline = bUnderline;
			plgfont->lfStrikeOut = bStrikeOut;
			plgfont->lfCharSet	= nCharSet;
			plgfont->lfOutPrecision = nOutPrecision;
			plgfont->lfClipPrecision = nClipPrecision;
			plgfont->lfQuality = nQuality;
			plgfont->lfPitchAndFamily = nPitchAndFamily;
			std::swprintf(plgfont->lfFaceName, LF_FACESIZE, L"%s", wcFaceName);
#if defined(WORK_AXIS_CAD) || defined(WORK_AXIS_DOC)	// 往右、上递增；1逻辑单位等于0.1毫米
			plgfont->lfHeight *=254/72; 
#else
			if(hdc)
				plgfont->lfHeight = -MulDiv(plgfont->lfHeight, GetDeviceCaps(hdc, LOGPIXELSY), 72);
#endif

		}

	}

	HFONT createFont() const {
		LOGFONT lf = {0};
		toMSFont(&lf);
		HFONT hfont = CreateFontIndirect(&lf);

		return hfont;
	}
#endif
}BSFont;



}