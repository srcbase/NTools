#ifndef __ITOBJADMIN_H__
#define __ITOBJADMIN_H__
#pragma once

#include "AC/items/items.h"
#include "AC/items/pages.h"

namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjAdmin	2013-12-13 --- >>
//	绘图单元管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class itobjAdmin{
public:
	itobjAdmin();
	itobjAdmin(const itobjAdmin &admin);
	itobjAdmin& operator=(const itobjAdmin &admin);
	virtual ~itobjAdmin();
public:
	// 公共方法
	/*****************************************************************************************
		<< --- itobjAdmin::add	2013-12-13 --- >>
		说明：添加一个绘图对象
		参数：
		pobj	= 被添加的绘图对象
		返回值：
	*****************************************************************************************/
	void add(itobj* pobj) {
		if(pobj && std::find(m_vecItems.begin(), m_vecItems.end(), pobj) == m_vecItems.end())
			m_vecItems.push_back(pobj);
	}

	/*****************************************************************************************
		<< --- itobjAdmin::remove	2013-12-13 --- >>
		说明：删除一个绘图对象，并释放内存
		参数：
		pobj	= 被删除的绘图对象
		返回值：
	*****************************************************************************************/
	void remove(itobj* pobj) {
		itobj::PtrArrayType::iterator iter, end = m_vecItems.end();
		iter = std::find(m_vecItems.begin(), end, pobj);
		if(iter != end)
		{
			(*iter)->remove();
			m_vecItems.erase(iter);
		}
	}
	/*****************************************************************************************
		<< --- itobjAdmin::removeAll	2013-12-13 --- >>
		说明：释放所有绘图单元
		参数：
		返回值：
	*****************************************************************************************/
	void removeAll() {
		for(itobj::PtrArrayType::size_type idx = 0; idx < m_vecItems.size(); ++idx)
		{
			m_vecItems[idx]->remove();
		}
		m_vecItems.clear();
	}
	/*****************************************************************************************
		<< --- itobjAdmin::copy	2013-12-13 --- >>
		说明：将绘图单元追加到现有集合中
		参数：
		vecItem	= 被追加的绘图单元
		返回值：
	*****************************************************************************************/
	void copy(const itobj::PtrArrayType &vecItem) {
		for(itobj::PtrArrayType::size_type idx = 0; idx < vecItem.size(); ++idx)
		{
			m_vecItems.push_back(vecItem[idx]->clone());
		}
	}

public:
#if defined(_WIN32)
	/*****************************************************************************************
		<< --- itobjAdmin::draw	2013-12-13 --- >>
		说明：绘制绘图模块
		参数：
		pdc		= 绘图环境
		pView	= 绘图环境所属视图
		返回值：
	*****************************************************************************************/
	void draw(CDC *pdc, CView* pView);
	/*****************************************************************************************
		<< --- itobjAdmin::drawBackground	2013-12-02 --- >>
		说明：绘制工作区和纸张
		参数：
		pdc			= 绘图环境
		szWorker	= 工作台尺寸
		返回值：
	*****************************************************************************************/
	void drawBackground(CDC *pdc, const CSize &szWorker);
	/*****************************************************************************************
		<< --- itobjAdmin::prepareDC	2013-12-13 --- >>
		说明：定位绘图坐标系，绘图原点
		参数：
		pdc			= 绘图环境
		szWorker	= 工作台尺寸
		返回值：定位块光标
	*****************************************************************************************/
	void prepareDC(CDC* pdc, const CSize &szWorker);
	/*****************************************************************************************
		<< --- itobjAdmin::objectAt	2013-12-02 --- >>
		说明：返回点point处的第一个对象（依赖对象添加的先后顺序）
		参数：
		point	= 参考点
		返回值：有就返回对象共享指针，否则返回空指针
	*****************************************************************************************/
	itobj* objectAt(const CPoint &point)
	{
		CRect rect(point, CSize(1, 1));
		for(itobj::PtrArrayType::size_type idx = 0; idx < m_vecItems.size(); ++idx)
		{
			if(m_vecItems[idx]->intersects(gtc::rect(rect.left, rect.top, rect.right, rect.bottom)))
			{
				return m_vecItems[idx];
			}
		}
		return nullptr;
	}
#endif
public:
	// 属性
	itobjPage*				getPager() {				return m_pobjPage;					}
	void					resetPager(itobjPage *ppage) {
		if(m_pobjPage)
			m_pobjPage->remove();
		m_pobjPage = ppage;
	}

	itobj::PtrArrayType&	getItems() {				return m_vecItems;					}
	
#if defined(BOOST_SERIALIZE)
private:
	friend class boost::serialization::access;  
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_clrTracker;  
		ar & m_beWorkaxis;
		ar & m_pobjPage;
		ar & m_vecItems;
	}
#endif

protected:
	//itobj					*mpobj;
	uint32_t				m_clrTracker;	// 跟踪模块填充色
	BEWorkAxis				m_beWorkaxis;	// 根据WORK_AXIS_NORMAL、WORK_AXIS_DOC、WORK_AXIS_CAD的定义得到
	itobjPage				*m_pobjPage;	// 页面信息
	itobj::PtrArrayType		m_vecItems;		// 绘制对象集合
};

}
#endif