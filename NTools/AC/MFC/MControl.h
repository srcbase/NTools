#pragma once
#include "db/dbtype.h"
// 列表控件中的额外项
#define DEF_ITEM_TEXT_ALL				L"全部"
#define DEF_ITEM_TEXT_NONE				L"无"
#define DEF_ITEM_TEXT_NULL				L"为空"
// 新增\修改\删除中的常用命令
#define DEF_COMMAND_TEXT_SAVE						L"保存"
#define DEF_COMMAND_TEXT_ADD						L"新增"
#define DEF_COMMAND_TEXT_MODIFY						L"修改"
#define DEF_COMMAND_TEXT_DELETE						L"删除"
// 将控件ID转换成对象
#define DEF_TO_COMMBOX(ctrlID)						(CComboBox *)GetDlgItem(ctrlID)
// 命令按钮
#define DEF_COMMAND_INIT(add,modify,del)			GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_ADD);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_MODIFY);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(FALSE);\
													GetDlgItem(del)->EnableWindow(FALSE);
#define DEF_COMMAND_ADD_READY(add,modify,del)		GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_SAVE);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_MODIFY);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(FALSE);\
													GetDlgItem(del)->EnableWindow(FALSE);
#define DEF_COMMAND_ADD_AFTER(add,modify,del)		GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_ADD);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_MODIFY);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(FALSE);\
													GetDlgItem(del)->EnableWindow(FALSE);
#define DEF_COMMAND_MODIFY_READY(add,modify,del)	GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_ADD);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_SAVE);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(TRUE);\
													GetDlgItem(del)->EnableWindow(TRUE);
#define DEF_COMMAND_MODIFY_AFTER(add,modify,del)	GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_ADD);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_MODIFY);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(TRUE);\
													GetDlgItem(del)->EnableWindow(TRUE);
#define DEF_COMMAND_DELETE_AFTER(add,modify,del)	GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_ADD);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_MODIFY);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(FALSE);\
													GetDlgItem(del)->EnableWindow(FALSE);
#define DEF_COMMAND_SELECT(add,modify,del)			GetDlgItem(add)->SetWindowText(DEF_COMMAND_TEXT_ADD);\
													GetDlgItem(modify)->SetWindowText(DEF_COMMAND_TEXT_MODIFY);\
													GetDlgItem(del)->SetWindowText(DEF_COMMAND_TEXT_DELETE);\
													GetDlgItem(add)->EnableWindow(TRUE);\
													GetDlgItem(modify)->EnableWindow(TRUE);\
													GetDlgItem(del)->EnableWindow(TRUE);
// Ribbon 按钮
#define DEF_RIBBON_INIT(add,modify,del)				((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_ADD);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_MODIFY);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(FALSE);\
													((CMFCRibbonButton*)(del))->SetData(FALSE);
#define DEF_RIBBON_ADD_READY(add,modify,del)		((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_SAVE);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_MODIFY);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(FALSE);\
													((CMFCRibbonButton*)(del))->SetData(FALSE);
#define DEF_RIBBON_ADD_AFTER(add,modify,del)		((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_ADD);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_MODIFY);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(FALSE);\
													((CMFCRibbonButton*)(del))->SetData(FALSE);
#define DEF_RIBBON_MODIFY_READY(add,modify,del)		((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_ADD);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_SAVE);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(TRUE);\
													((CMFCRibbonButton*)(del))->SetData(TRUE);
#define DEF_RIBBON_MODIFY_AFTER(add,modify,del)		((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_ADD);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_MODIFY);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(TRUE);\
													((CMFCRibbonButton*)(del))->SetData(TRUE);
#define DEF_RIBBON_DELETE_AFTER(add,modify,del)		((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_ADD);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_MODIFY);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(FALSE);\
													((CMFCRibbonButton*)(del))->SetData(FALSE);
#define DEF_RIBBON_SELECT(add,modify,del)			((CMFCRibbonButton*)(add))->SetText(DEF_COMMAND_TEXT_ADD);\
													((CMFCRibbonButton*)(modify))->SetText(DEF_COMMAND_TEXT_MODIFY);\
													((CMFCRibbonButton*)(del))->SetText(DEF_COMMAND_TEXT_DELETE);\
													((CMFCRibbonButton*)(add))->SetData(TRUE);\
													((CMFCRibbonButton*)(modify))->SetData(TRUE);\
													((CMFCRibbonButton*)(del))->SetData(TRUE);
namespace acm {

class CNControl
{
private:
	CNControl(void) {}
	virtual ~CNControl(void) {}
public:
	/*****************************************************************************************
		<< --- CNControl::setFocus		2013-11-19 --- >>
		说明：使指定控件得到光标
		参数：
		vpwnd	= 控件所属窗体
		vctrlid	= 得到光标的控件ID
		返回值：
	*****************************************************************************************/
	static void setFocus(CWnd* vpwnd, int vctrlid)
	{
		if(vpwnd != NULL && vctrlid != 0)
		{
			CWnd* pwnd = vpwnd->GetDlgItem(vctrlid);
			if(pwnd != NULL)
			{
				pwnd->PostMessage(EM_SETSEL, 0, -1);
				pwnd->SetFocus();
			}
		}
	}
	/*****************************************************************************************
		<< --- CNControl::setReadOnly		2013-11-19 --- >>
		说明：设定控件的只读属性
		参数：
		vpwnd	= 控件所属窗体
		veditid	= 得到光标的控件ID
		visonlyread	= 是否只读
		返回值：
	*****************************************************************************************/
	static void setReadOnly(CWnd* vpwnd, UINT veditid, bool visonlyread)
	{
		if(vpwnd == NULL || veditid <= 0)
			return;
		vpwnd->GetDlgItem(veditid)->SendMessage(EM_SETREADONLY, visonlyread, 0);
	}
	/*****************************************************************************************
		<< --- CNControl::getIDFromBox		2013-11-19 --- >>
		说明：根据索引项，得到条目的ID数据
		参数：
		pcbo	= ComboBox控件
		vindex	= ComboBox的索引
		返回值：成功=ComboBox的索引对应的ItemData值；失败=0
	*****************************************************************************************/
	static long getIDFromBox(CComboBox *pcbo, long vindex, bool isExtSet = false)
	{
		assert(isExtSet == false);
		assert(pcbo);
		if (!isExtSet)
		{
			return pcbo->GetItemData(vindex);
		}
		gtc::entry *pitem = getDataFromBox(pcbo, vindex);
		if(pitem)
			return pitem->getID();
		return 0;
	}
	static gtc::entry* getDataFromBox(CComboBox *pcbo, long vindex)
	{
		if(pcbo == nullptr || vindex < 0 || vindex >= pcbo->GetCount())
			return nullptr;
		gtc::entry *pitem = nullptr;
		try
		{
			pitem = (gtc::entry*)pcbo->GetItemData(vindex);
		}
		catch(...)
		{
			pitem = nullptr;
			gtc::app::lastError();
		}
		return pitem;
	}

	/*****************************************************************************************
		<< --- CNControl::getIndexFromBox		2013-11-19 --- >>
		说明：根据条目的ID数据，得到项的索引
		参数：
		pcbo	= ComboBox控件
		vid		= ComboBox的ItemData值
		返回值：成功=ComboBox的索引对应的ItemData值；失败=-1
	*****************************************************************************************/
	static long getIndexFromBox(CComboBox *pcbo, long vid, bool isExtSet = false)
	{
		assert(isExtSet == false);
		assert(pcbo);
		if(pcbo == nullptr)
			return -1;
		long index = -1;
		gtc::entry *pitem = nullptr;
		try
		{
			for(int idx = 0; idx < pcbo->GetCount(); idx++)
			{
				if (isExtSet)
				{
					if((pitem = (gtc::entry*)pcbo->GetItemData(idx)) && pitem->getID() == vid)
					{
						index = idx;
						break;
					}
				}
				else
				{
					if (pcbo->GetItemData(idx) == vid)
					{
						index = idx;
						break;
					}
				}
			}
		}
		catch(...)
		{
			index = -1;
			gtc::app::lastError();
		}

		return index;
	}
	static long getIndexFromBox(HWND hWnd, int cboID, long vid, bool isExtSet = false)
	{
		assert(hWnd);
		long idx = -1;
		CComboBox cbo;
		cbo.Attach(GetDlgItem(hWnd, cboID));
		idx = getIndexFromBox(&cbo, vid, isExtSet);
		cbo.Detach();
		return idx;
	}
	/*****************************************************************************************
		<< --- CNControl::setDataComboBox		2013-11-19 --- >>
		说明：设定ComboBox控件的数据
		参数：
		pcbo		= ComboBox控件
		返回值：成功=ComboBox的条目数量；失败=-1
	*****************************************************************************************/
	static void freeDataFromBox(CComboBox *pcbo)
	{
		if(pcbo == nullptr || pcbo->GetCount() <= 0)
			return ;
		//gtc::entry *pitem = nullptr;
		try
		{
			for(int idx = 0; idx < pcbo->GetCount(); idx++)
			{
				delete (gtc::entry*)pcbo->GetItemData(idx);
			}
			pcbo->ResetContent();
		}
		catch(...)
		{
			gtc::app::lastError();
		}
	}
	static void freeDataFromBox(CMFCRibbonComboBox *pcbo)
	{
		if(pcbo == nullptr || pcbo->GetCount() <= 0)
			return ;
		//gtc::entry *pitem = nullptr;
		try
		{
			for(int idx = 0; idx < pcbo->GetCount(); idx++)
			{
				delete (gtc::entry*)pcbo->GetItemData(idx);
			}
			pcbo->RemoveAllItems();
		}
		catch(...)
		{
			gtc::app::lastError();
		}
	}

	/*****************************************************************************************
		<< --- CNControl::setDataBox		2013-11-19 --- >>
		说明：设定ComboBox控件的数据
		参数：
		pcbo		= ComboBox控件
		vdata		= ComboBox的数据
		返回值：成功=ComboBox的条目数量；失败=-1
	*****************************************************************************************/
	static void setDataBox(CComboBox *pcbo, const gtc::entry::PtrArrayType &vdata)
	{
		if(pcbo == nullptr)
			return ;

		try
		{
			// 
			freeDataFromBox(pcbo);

			// 
			gtc::entry::PtrArrayType::const_iterator iter, end = vdata.cend();
			for(iter = vdata.cbegin(); iter != end; ++iter)
			{
				pcbo->SetItemData(pcbo->AddString((*iter)->getTitle().c_str()), DWORD_PTR(*iter));
			}

		}
		catch(...)
		{
			gtc::app::lastError();
		}

	}
	static void setDataBox(CMFCRibbonComboBox *pcbo, const gtc::entry::PtrArrayType &vdata)
	{
		if(!pcbo)
			return ;

		try
		{
			freeDataFromBox(pcbo);

			// 
			gtc::entry::PtrArrayType::const_iterator iter, end = vdata.cend();
			for(iter = vdata.cbegin(); iter != end; ++iter)
			{
				pcbo->AddItem((*iter)->getTitle().c_str(), DWORD_PTR(*iter));
			}

		}
		catch(...)
		{
			gtc::app::lastError();
		}

	}

	/*****************************************************************************************
		<< --- CNControl::setDataBox		2013-11-19 --- >>
		说明：设定ComboBox控件的数据
		参数：
		pcbo		= ComboBox控件ID
		vdata		= ComboBox的数据
		返回值：成功=ComboBox的条目数量；失败=-1
	*****************************************************************************************/
	static void setDataBox(CComboBox *pcbo, const gtc::entry vdata[], const int32_t vdatacount)
	{
		if(!pcbo)
			return ;

		try
		{
			// 
			pcbo->ResetContent();
			// 
			for(int idx = 0; idx < vdatacount; ++idx)
			{
				pcbo->SetItemData(pcbo->AddString(vdata[idx].getTitle().c_str()), DWORD_PTR(&vdata[idx]));
			}
		}
		catch(...)
		{
			gtc::app::lastError();
		}
	}
	/*****************************************************************************************
		<< --- CNControl::isCheckRadioBtn		2013-11-19 --- >>
		说明：检查Radio控件是否选中
		参数：
		vpwnd	= 控件所属窗体
		vchkid	= 控件ID
		返回值：true=选中；false=未选中
	*****************************************************************************************/
	static bool isCheckRadioBtn(CWnd* vpwnd, UINT vchkid)
	{
		if(vpwnd == NULL || vchkid <= 0)
			return false;
		bool ischk = false;
		CButton btn;
		try
		{
			btn.Attach(vpwnd->GetDlgItem(vchkid)->m_hWnd);
			ischk = btn.GetCheck() != 0;
		}
		catch(...)
		{
			ischk = false;
			gtc::app::lastError();
		}
		if(btn.m_hWnd != NULL)
			btn.Detach();

		return ischk;
	}
	/*****************************************************************************************
		<< --- CNControl::setCheckRadioBtn		2013-11-19 --- >>
		说明：设定Radio控件选中标记
		参数：
		vpwnd	= 控件所属窗体
		vchkid	= 控件ID
		vischked= 是否选中
		返回值：
	*****************************************************************************************/
	static void setCheckRadioBtn(CWnd* vpwnd, UINT vchkid, bool vischked)
	{
		if(vpwnd == NULL || vchkid <= 0)
			return ;
		CButton btn;
		try
		{
			btn.Attach(vpwnd->GetDlgItem(vchkid)->m_hWnd);
			btn.SetCheck(vischked?BST_CHECKED:BST_UNCHECKED);
			btn.Detach();
		}
		catch(...)
		{
			gtc::app::lastError();
		}
		if(btn.m_hWnd != NULL)
			btn.Detach();
	}
/*
public:
	// 清空控件数据
	virtual void doClearInputBox(void);
	// 验证用于保存的数据项
	virtual bool doUpdateParams(db::parameter::ArrayType &vparams);
	// 改变控件状态
	virtual void doChangeInputStatus(bool venable);
	// 初始化控件数据项
	virtual void doInitData(void);
	// 刷新网格数据
	virtual void doRefreshData(void);

	// 仅清空必输项控件数据
	virtual void doClearMustReinputBox(void);
	// 设置控件数据
	virtual void doSetBoxAndData(db::dataSet &rst);
	// 根据控件数据得到sql语句的查询条件
	virtual CString doGetQueryWhere(void);
*/
/*
// 清空控件数据
void CDlgHandkey::doClearInputBox(void)
{

	UpdateData(FALSE);
}
// 验证用于保存的数据项
bool CDlgHandkey::doUpdateParams(db::parameter::ArrayType &vparams)
{
	UpdateData(TRUE);

	return true;
}
// 改变控件状态
void CDlgHandkey::doChangeInputStatus(bool venable)
{
}
// 初始化控件数据项
void CDlgHandkey::doInitData(void)
{
}
// 刷新网格数据
void CDlgHandkey::doRefreshData(void)
{
}


////////////////////////////////////
// 根据控件数据得到sql语句的查询条件
CString CDlgHandkey::doGetQueryWhere(void)
{	
	return L"";	
}
// 仅清空必输项控件数据
void CDlgHandkey::doClearMustReinputBox(void)
{

	UpdateData(FALSE);
}
// 设置控件数据
void CDlgHandkey::doSetBoxAndData(db::dataSet &rst)
{
	
	UpdateData(FALSE);
}
*/
};












}