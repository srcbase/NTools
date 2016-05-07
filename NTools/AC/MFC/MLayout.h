#pragma once
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <atltypes.h>

// 控件距离窗体边界的默认距离
#define CONTROL_INDIALOG_SPACE	15

namespace acm {
// 内盒box在区域area内部如何对齐
enum BEAlignBoxInArea{
		BEAlignBoxInAreaLeftTop			= 0x0011,		// 图片顶部左对齐
		BEAlignBoxInAreaLeftCenter		= 0x0012,		// 图片中间左对齐
		BEAlignBoxInAreaLeftBottom		= 0x0014,		// 图片底部左对齐
		BEAlignBoxInAreaCenterTop		= 0x0021,		// 图片顶部居中
		BEAlignBoxInAreaCenterCenter	= 0x0022,		// 图片中间居中
		BEAlignBoxInAreaCenterBottom	= 0x0024,		// 图片底部居中
		BEAlignBoxInAreaRightTop		= 0x0041,		// 图片顶部右对齐
		BEAlignBoxInAreaRightCenter		= 0x0042,		// 图片中间右对齐
		BEAlignBoxInAreaRightBottom		= 0x0044,		// 图片底部右对齐
		BEAlignBoxInAreaReally			= 0x1000,		// VSP3.1背景图片尺寸不变居中显示		BP_Auto			= 0x1000,		// 自适用
		BEAlignBoxInAreaExtend			= 0x2000,		// VSP3.1图片伸缩全屏显示
		BEAlignBoxInAreaFull			= 0xFFFF		// 平铺
};
/*****************************************************************************************
	CMLayout
	说明：布局窗体或控件
	日期：2013-11-19
*****************************************************************************************/
class CMLayout
{
private:

	CMLayout(void) {	}

	virtual ~CMLayout(void)	{	}
public:
	/*****************************************************************************************
		<< --- CMLayout::reviseArea		2013-11-19 --- >>
		说明：将指定区域varea，按长宽比例vaspectratio进行修正
		参数：
		varea			= 被转换的区域
		vaspectratio	= 长宽比例值
		返回值：修正后的区域
	*****************************************************************************************/
	CRect reviseArea(const CRect& varea, float vaspectratio)
	{
		CRect rect = varea;
		long boxH, boxW, w, h;
		boxH = rect.Height();
		boxW = rect.Width();

		if((float(boxW)/boxH) > vaspectratio)
		{ 
			h = boxH;
			w = long(vaspectratio * h);
			rect.DeflateRect((boxW - w)/2, 0);
		}
		else
		{
			w = boxW;
			h = long(vaspectratio * w);
			rect.DeflateRect(0, (boxH - h)/2);
		}

		return rect;
	}
	/*****************************************************************************************
		<< --- CMLayout::computerImageArea		2013-11-19 --- >>
		说明：将指定区域varea，按长宽比例vaspectratio进行修正
		参数：
		varea			= 被转换的区域
		vaspectratio	= 长宽比例值
		返回值：修正后的区域
	*****************************************************************************************/
	/* 根据图片尺寸以及图片在工作区域摆放的位置，计算一个图片有效区域，并返还这一有效区域 */
	static CRect computerImageArea(const CRect& varea, const CSize& vbox, BEAlignBoxInArea valign)
	{
		int signx = 1, signy = 1;
		if(varea.Width() < 0)
			signx = -1;
		if(varea.Height() < 0)
			signy = -1;
		int imgw = vbox.cx, imgh = vbox.cy;
		CRect area(varea.left, varea.top, varea.left + imgw*signx, varea.top + imgh*signy);
		float fbox, farea; // 宽高比
		fbox = float(varea.Width()*signx)/varea.Height()*signy;
		farea = float(imgw)/imgh;
		if(fbox > farea && imgh > abs(varea.Height()))
		{	// 图片比绘制区域窄，且比绘制区域高
			area.SetRect(area.left, area.top, area.left + int(abs(varea.Height())*farea)*signx, area.top + varea.Height());
		}
		else if(fbox < farea && imgw > abs(varea.Width()))
		{	// 图片比绘制区域宽，且比绘制区域高
			area.SetRect(area.left, area.top, area.left + varea.Width(), area.top + int(abs(varea.Width())/farea)*signy);
		}
		long spw = (abs(varea.Width())-abs(area.Width()))/2;
		long sph = (abs(varea.Height())-abs(area.Height()))/2;
		switch(valign)
		{
		case BEAlignBoxInAreaLeftTop:		//= 0,		// 图片顶部左对齐
			break;
		case BEAlignBoxInAreaLeftCenter:		//= 1,		// 图片中间左对齐
			area.OffsetRect(0, sph*signy);
			break;
		case BEAlignBoxInAreaLeftBottom:		//= 2,		// 图片底部左对齐
			area.OffsetRect(0, sph*2*signy);
			break;
		case BEAlignBoxInAreaCenterTop:		//= 3,		// 图片顶部居中
			area.OffsetRect(spw*signx, 0);
			break;
		case BEAlignBoxInAreaCenterCenter:	//= 4,		// 图片中间居中
			area.OffsetRect(spw*signx, sph*signy);
			break;
		case BEAlignBoxInAreaCenterBottom:	//= 5,		// 图片底部居中
			area.OffsetRect(spw*signx, sph*2*signy);
			break;
		case BEAlignBoxInAreaRightTop:		//= 6,		// 图片顶部右对齐
			area.OffsetRect(spw*2*signx, 0);
			break;
		case BEAlignBoxInAreaRightCenter:	//= 7,		// 图片中间右对齐
			area.OffsetRect(spw*2*signx, sph*signy);
			break;
		case BEAlignBoxInAreaRightBottom:	//= 8,		// 图片底部右对齐
			area.OffsetRect(spw*2*signx, sph*2*signy);
			break;
		case BEAlignBoxInAreaReally:			//			// 
			{
				area = varea;
				long boxH, boxW, w, h;
				boxH = abs(varea.Height());
				boxW = abs(varea.Width());

				if((float(boxW)/boxH) > (float(imgw)/imgh))
				{ 
					h = boxH;
					w = long(float(imgw)/imgh * h);
					area.DeflateRect((boxW - w)/2, 0);
				}
				else
				{
					w = boxW;
					h = long(float(imgh)/imgw * w);
					area.DeflateRect(0, (boxH - h)/2);
				}
			}
			break;
		case BEAlignBoxInAreaExtend:
			{
				if(fbox > farea)
				{	// 图片比绘制区域窄，且比绘制区域高
					area.SetRect(area.left, area.top, area.left + int(abs(varea.Height())*farea)*signx, area.top + varea.Height());
					area.OffsetRect((abs(varea.Width())-abs(area.Width()))/2*signx, 0);
				}
				else if(fbox < farea)
				{	// 图片比绘制区域宽，且比绘制区域高
					area.SetRect(area.left, area.top, area.left + varea.Width(), area.top + int(abs(varea.Width())/farea)*signy);
					area.OffsetRect(0, (abs(varea.Height())-abs(area.Height()))/2*signy);
				}
				else
					area = varea;
				//long boxH, boxW, imgH, imgW, w, h;
				//boxH = abs(varea.Height());
				//boxW = abs(varea.Width());

				//imgH = imgh;
				//imgW = vimgW;
				//if((float(boxW)/boxH) > (float(imgW)/imgH))
				//{ 
				//	h = boxH;
				//	w = long(float(imgW)/imgH * h);
				//	area.DeflateRect((boxW - w)/2, 0);
				//}
				//else
				//{
				//	w = boxW;
				//	h = long(float(imgH)/imgW * w);
				//	area.DeflateRect(0, (boxH - h)/2);
				//}
			}
			break;
		case BEAlignBoxInAreaFull:			//= 10		// 平铺
			area = varea;
			break;
		default:
			{
				long val = valign;
				long offx = 0, offy = 0;
				if(fbox > farea)
				{	// 图片比绘制区域窄，且比绘制区域高
					area.SetRect(area.left, area.top, area.left + int(abs(varea.Height())*farea)*signx, area.top + varea.Height());
				}
				else if(fbox < farea)
				{	// 图片比绘制区域宽，且比绘制区域高
					area.SetRect(area.left, area.top, area.left + varea.Width(), area.top + int(abs(varea.Width())/farea)*signy);
				}
				spw = (abs(varea.Width())-abs(area.Width()))/2;
				sph = (abs(varea.Height())-abs(area.Height()))/2;
				if(val & 0x1000)
				{	// 是 BP_Auto
					if(val & 0x0020)
						offx = spw;
					else if(val & 0x0040)
						offx = spw * 2;
					if(val & 0x0002)
						offy = sph;
					else if(val & 0x0004)
						offy = sph * 2;

					area.OffsetRect(offx*signx, offy*signy);
				}
			}
			break;
		}
		return area;
	}
	/* 控件布局函数 */

	/*****************************************************************************************
		<< --- CMLayout::alignToLeft		2013-11-19 --- >>
		说明：移动控件vmoveid，使其与vtargetid控件左对齐
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect alignToLeft(CWnd* vpwnd, int vmoveid, int vtargetid)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);
		rect.OffsetRect(rt.left - rect.left, 0);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	
	/*****************************************************************************************
		<< --- CMLayout::alignToRight		2013-11-19 --- >>
		说明：移动控件vmoveid，使其与vtargetid控件右对齐
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect alignToRight(CWnd* vpwnd, int vmoveid, int vtargetid)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);
		rect.OffsetRect(rt.right - rect.right, 0);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}

	/*****************************************************************************************
		<< --- CMLayout::alignToTop		2013-11-19 --- >>
		说明：移动控件vmoveid，使其与vtargetid控件上对齐
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect alignToTop(CWnd* vpwnd, int vmoveid, int vtargetid)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);
		rect.OffsetRect(0, rt.top - rect.top);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}

	/*****************************************************************************************
		<< --- CMLayout::alignToBottom		2013-11-19 --- >>
		说明：移动控件vmoveid，使其与vtargetid控件下对齐
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect alignToBottom(CWnd* vpwnd, int vmoveid, int vtargetid)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);
		rect.OffsetRect(0, rt.bottom - rect.bottom);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}

	/*****************************************************************************************
		<< --- CMLayout::moveToLeft		2013-11-19 --- >>
		说明：移动控件vmoveid到vtargetid控件的左边，两者间距vspace
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件间的间距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToLeft(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.OffsetRect(rt.left - rect.left - rect.Width()-vspace, 0);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToRight		2013-11-19 --- >>
		说明：移动控件vmoveid到vtargetid控件的右边，两者间距vspace
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件间的间距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToRight(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.OffsetRect(rt.right - rect.right + rect.Width()+vspace, 0);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToTop		2013-11-19 --- >>
		说明：移动控件vmoveid到vtargetid控件的上边，两者间距vspace
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件间的间距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToTop(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.OffsetRect(0, rt.top - rect.top - rect.Height()-vspace);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToBottom		2013-11-19 --- >>
		说明：移动控件vmoveid到vtargetid控件的下边，两者间距vspace
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件间的间距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToBottom(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.OffsetRect(0, rt.bottom - rect.bottom + rect.Height() + vspace);

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}

	// 
	/*****************************************************************************************
		<< --- CMLayout::expandLeft		2013-11-19 --- >>
		说明：扩展控件vmoveid的左边到距离vtargetid控件vspace处
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件的边界距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandLeft(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.left = rt.right + vspace;

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::expandRight		2013-11-19 --- >>
		说明：扩展控件vmoveid的右边到距离vtargetid控件vspace处
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件的边界距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandRight(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.right = rt.left - vspace;

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::expandTop		2013-11-19 --- >>
		说明：扩展控件vmoveid的上边到距离vtargetid控件vspace处
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件的边界距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandTop(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.top = rt.bottom + vspace;

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::expandBottom		2013-11-19 --- >>
		说明：扩展控件vmoveid的底边到距离vtargetid控件vspace处
		参数：
		vpwnd		= 控件所属窗体
		vmoveid		= 被移动的控件
		vtargetid	= 目标控件
		vspace		= 两控件的边界距
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandBottom(CWnd* vpwnd, int vmoveid, int vtargetid, int vspace)
	{
		CRect rt, rect;
		if(vpwnd == NULL)
			return rect;
		vpwnd->GetDlgItem(vmoveid)->GetWindowRect(rect);
		vpwnd->GetDlgItem(vtargetid)->GetWindowRect(rt);

		rect.bottom = rt.top - vspace;

		vpwnd->ScreenToClient(rect);
		vpwnd->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}

	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToLeftborderInDialog		2013-11-19 --- >>
		说明：将控件vmoveid移动到距离窗体pdialog左边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 距窗体边界距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToLeftborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		int offx = 0, offy = 0;
		pdialog->GetWindowRect(rect);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rt);

		offx = rect.left - rt.left + vspace;
		rt.OffsetRect(offx, offy);
	
		pdialog->ScreenToClient(rt);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rt);
		return rt;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToRightborderInDialog		2013-11-19 --- >>
		说明：将控件vmoveid移动到距离窗体pdialog右边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 距窗体边界距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToRightborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		int offx = 0, offy = 0;
		pdialog->GetWindowRect(rect);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rt);
		offx = rect.right - rt.right - vspace;
		rt.OffsetRect(offx, offy);
	
		pdialog->ScreenToClient(rt);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rt);
		return rt;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToTopborderInDialog		2013-11-19 --- >>
		说明：将控件vmoveid移动到距离窗体pdialog上边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 距窗体边界距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToTopborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		int offx = 0, offy = 0;
		pdialog->GetWindowRect(rect);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rt);

		offy = rect.top - rt.top + vspace + 25;	// 窗体标题栏占25
		rt.OffsetRect(offx, offy);
	
		pdialog->ScreenToClient(rt);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rt);
		return rt;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::moveToBottomborderInDialog		2013-11-19 --- >>
		说明：将控件vmoveid移动到距离窗体pdialog底边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 距窗体边界距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect moveToBottomborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		int offx = 0, offy = 0;
		pdialog->GetWindowRect(rect);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rt);

		offy = rect.bottom - rt.bottom - vspace;
		rt.OffsetRect(offx, offy);
	
		pdialog->ScreenToClient(rt);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rt);
		return rt;
	}

	// 
	/*****************************************************************************************
		<< --- CMLayout::expandToLeftborderInDialog		2013-11-19 --- >>
		说明：扩展vmoveid的左边界到窗体左边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 控件边界与窗体边界间的距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandToLeftborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		pdialog->GetWindowRect(rt);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rect);

		rect.left = rt.left + vspace;
	
		pdialog->ScreenToClient(rect);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::expandToRightborderInDialog		2013-11-19 --- >>
		说明：扩展vmoveid的左边界到窗体右边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 控件边界与窗体边界间的距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandToRightborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		pdialog->GetWindowRect(rt);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rect);

		rect.right = rt.right - vspace;
	
		pdialog->ScreenToClient(rect);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::expandToTopborderInDialog		2013-11-19 --- >>
		说明：扩展vmoveid的左边界到窗体上边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 控件边界与窗体边界间的距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandToTopborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		pdialog->GetWindowRect(rt);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rect);

		rect.top = rt.top + vspace + 25;	// 25是窗体标题栏的高度
	
		pdialog->ScreenToClient(rect);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}
	// 
	/*****************************************************************************************
		<< --- CMLayout::expandToBottomborderInDialog		2013-11-19 --- >>
		说明：扩展vmoveid的左边界到窗体底边界vspace处
		参数：
		pdialog		= 控件所属窗体
		vmoveid		= 被移动的控件
		vspace		= 控件边界与窗体边界间的距离
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect expandToBottomborderInDialog(CWnd* pdialog, int vmoveid, int vspace)
	{
		CRect rect, rt;
		if(pdialog == NULL)
			return rect;
		pdialog->GetWindowRect(rt);
		pdialog->GetDlgItem(vmoveid)->GetWindowRect(rect);

		rect.bottom = rt.bottom - vspace;
	
		pdialog->ScreenToClient(rect);
		pdialog->GetDlgItem(vmoveid)->MoveWindow(rect);
		return rect;
	}

	/*****************************************************************************************
		<< --- CMLayout::fullZoomDialog		2013-11-19 --- >>
		说明：重新设定对话框的大小；vzoomx,vzoomy相对于桌面缩放的百分比
		参数：
		pwnd		= 被改变窗体
		vzoomx		= 水平缩放比例[0--100],0=保持原宽，100=与桌面同宽
		vzoomy		= 垂直缩放比例[0--100],0=保持原高，100=与桌面同高
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect fullZoomDialog(CWnd* pwnd, uint16_t vzoomx, uint16_t vzoomy)
	{
		CRect rect, rtdesk;
		rect.SetRectEmpty();
		if(pwnd == NULL)
			return rect;
		if(vzoomx > 100)
			vzoomx = 100;
		if(vzoomy > 100)
			vzoomy = 100;
		pwnd->GetWindowRect(rect);
		CWnd wnd;
		wnd.Attach(::GetDesktopWindow());
		wnd.GetWindowRect(rtdesk);
		wnd.Detach();
		long x = long(rtdesk.Width()*((100-vzoomx)/100.0f));
		long y = long(rtdesk.Height()*((100-vzoomy)/100.0f));
		if(vzoomx == 0)
			x = rtdesk.Width() - rect.Width();
		if(vzoomy == 0)
			y = rtdesk.Height() - rect.Height();
		rect = rtdesk;
		rect.right = rect.left + rect.right - x;
		rect.bottom = rect.top + rect.bottom - y;
		rect.OffsetRect(x/2, y/2);
		pwnd->MoveWindow(rect);
	
		pwnd->ScreenToClient(rect);
		return rect;
	}

	/*****************************************************************************************
		<< --- CMLayout::fullSizeDialog		2013-11-19 --- >>
		说明：设定对话框的大小
		参数：
		pwnd		= 被改变窗体
		vwidth		= 设定的宽度；<=0时保持原宽度
		vheight		= 设定的高度；<=0时保持原高度
		返回值：移动后的位置
	*****************************************************************************************/
	static CRect fullSizeDialog(CWnd* pwnd, int vwidth, int vheight)
	{
		CRect rect;
		if(pwnd == NULL)
			return rect;
		pwnd->GetWindowRect(rect);
		long width = rect.Width();
		long height = rect.Height();
		if(vwidth > 0)
			width = vwidth;
		if(vheight > 0)
			height = vheight;
		rect.right = rect.left + width;
		rect.bottom = rect.top + height;
		pwnd->MoveWindow(rect);

		pwnd->ScreenToClient(rect);
		return rect;
	}
};

}