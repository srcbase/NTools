#include "hflexGrid.h"
#import "D:/NTools/db/msado15.dll" rename("EOF", "ADOEOF")
using namespace ADODB;
namespace acm{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	xxxxx类			2014-10-18
//	类描述
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CNMSHFGrid, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CNMSHFGrid 属性

/////////////////////////////////////////////////////////////////////////////
// CNMSHFGrid 操作

/////////////////////////////////////////////////////////////////////////////////////////////
// CNHFlexGrid
bool CNHFlexGrid::mc_havePicture = true;
CPictureHolder CNHFlexGrid::mc_picOnly;
CPictureHolder CNHFlexGrid::mc_picSelect;
CPictureHolder CNHFlexGrid::mc_picUnSelect;

IMPLEMENT_DYNAMIC(CNHFlexGrid, CNMSHFGrid)

CNHFlexGrid::CNHFlexGrid()
	: m_dwFileVersion(SHESTGRID_DOC_VERSION)
	, m_lOldRow(-1)
	, m_bHaveSN(true)
	, m_euSelectFlag(CNHFlexGrid::BEGridSelectFlagRowSN)
	, m_bIsWrited(false)
	, m_lWriteRows(15)
	, m_strFromValue(L"")
	, m_strFieldsValue(L"")
	, m_strSortValue(L"")
	, m_strWhereValue(L"")
	, m_strIDFldName(L"")
	, m_lIDFldIndex(-1)
	, m_bRefreshFieldIndex(true)
	, m_bIsEmptyData(true)
	, m_lRowHeight(240)
{
	
#ifdef IDB_ONLY
	if(CNHFlexGrid::mc_picOnly.GetPictureDispatch() == NULL)
		CNHFlexGrid::mc_havePicture = CNHFlexGrid::mc_picOnly.CreateFromBitmap(IDB_ONLY);
#endif

#ifdef IDB_SELECTED
	if(CNHFlexGrid::mc_picSelect.GetPictureDispatch() == NULL)
		CNHFlexGrid::mc_havePicture = CNHFlexGrid::mc_picSelect.CreateFromBitmap(IDB_SELECTED);
#endif

#ifdef IDB_UNSELECT
	if(CNHFlexGrid::mc_picUnSelect.GetPictureDispatch() == NULL)
		CNHFlexGrid::mc_havePicture |= CNHFlexGrid::mc_picUnSelect.CreateFromBitmap(IDB_UNSELECT);
#endif
}

CNHFlexGrid::~CNHFlexGrid()
{
}


BEGIN_MESSAGE_MAP(CNHFlexGrid, CNMSHFGrid)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CNHFlexGrid 消息处理程序

BOOL CNHFlexGrid::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(pMsg->message == WM_LBUTTONUP)
	{	// 选中行
		this->doSelectedRow(this->get_Row());
	}
	else if(pMsg->message == WM_MOUSEWHEEL)
	{	// 滚动行数据
		int delta = (int)pMsg->wParam;
		long toprow = this->get_TopRow();
		if(delta > 0)
		{	// 上滚
			toprow--;
			if(toprow < this->get_FixedRows())
				toprow = this->get_FixedRows();
			this->put_TopRow(toprow);
		}
		else
		{
			toprow++;
			this->put_TopRow(toprow);
		}
	}
	return CNMSHFGrid::PreTranslateMessage(pMsg);
}

// 初始化标题及网格信息
bool CNHFlexGrid::doInitHeader(void)
{
	bool bok = false;
	int col = -1;
	gtc::gridColHeader* pheader = NULL;
	// 固定行和列，及合并单元格项
	if(this->haveParentHeader())
	{
		this->put_Rows(3);
		this->put_FixedRows(2);
		this->put_RowHeight(0, SHESTGRID_HEADER_HEIGHT);
		this->put_RowHeight(1, SHESTGRID_HEADER_HEIGHT);
		this->put_MergeRow(0, TRUE);   
	}
	else
	{
		this->put_Rows(2);
		this->put_FixedRows(1);
		this->put_RowHeight(0, long(SHESTGRID_HEADER_HEIGHT*1.5f));
	}
	if(this->haveSN())
	{
		this->put_FixedCols(1);
		this->put_ColWidth(0,0,SHESTGRID_SN_WIDTH);
		this->put_ColAlignmentFixed(0, flexAlignCenterCenter);
		if(this->haveParentHeader())	
		{	// 用于序号列标题的合并
			this->put_TextMatrix(0,0, _T(" "));
			this->put_TextMatrix(1,0, _T(" "));
		}
		this->put_MergeCol(0, TRUE);
	}
	else
		this->put_FixedCols(0);
	this->put_MergeCells(flexMergeFree);
	this->put_RowHeightMin(SHESTGRID_ROW_HEIGHTMIN);
	// 设定列标题和属性
	const long fixedcols = this->get_FixedCols();
	this->put_Cols(0, this->m_arrHeadinfo.size()+fixedcols);
	//for(std::vector<std::shared_ptr<gtc::gridColHeader>>::size_type idx = 0; idx < this->m_arrHeadinfo.size(); idx++)
	gtc::gridColHeader::PtrArrayType::iterator end = m_arrHeadinfo.end(), iter;
	int32_t idx;
	for(iter = m_arrHeadinfo.begin(), idx = 0; iter != end; ++iter, ++idx)
	{
		if((pheader = *iter) != NULL)
		{
			col = idx+fixedcols;
			// 所有列均可合并，如果不需要合并则采取在偶数行给内容加空格的方法
			this->put_MergeCol(col, true);
			// 宽度
			if(pheader->isVisible())
				this->put_ColWidth(col,0,pheader->getWidth());
			else
				this->put_ColWidth(col,0,0);
			// 对齐方式
			if(pheader->getDataType () == BEDataTypeBoolean)
				this->put_ColAlignment(col, flexAlignCenterCenter);
			else
			{
				switch(pheader->getAlign())
				{
				case BEAlignModeAllLeft:			//= DT_LEFT|DT_SINGLELINE|DT_VCENTER,
				case BEAlignModeLeftTop:			//= DT_LEFT|DT_TOP,			// 图片顶部左对齐
				case BEAlignModeLeftCenter:			//= DT_LEFT|DT_VCENTER,		// 图片中间左对齐
				case BEAlignModeLeftBottom:			//= DT_LEFT|DT_BOTTOM,		// 
					this->put_ColAlignment(col, flexAlignLeftCenter);
					break;
				case BEAlignModeAllRight:			//= DT_RIGHT|DT_SINGLELINE|DT_VCENTER,
				case BEAlignModeRightTop:			//= DT_RIGHT|DT_TOP,		// 图片顶部右对齐
				case BEAlignModeRightCenter:		//= DT_RIGHT|DT_VCENTER,	// 图片中间右对齐
				//case BEAlignModeRightBottom:		//= DT_RIGHT				// 图片底部右对齐
					this->put_ColAlignment(col, flexAlignRightCenter);
					break;
				case BEAlignModeAllCenter:			//= DT_CENTER|DT_SINGLELINE|DT_VCENTER,
				case BEAlignModeCenterTop:			//= DT_CENTER|DT_TOP,		// 图片顶部居中
				//case BEAlignModeCenterCenter:		//= DT_CENTER,				// 图片中间居中
				case BEAlignModeCenterBottom:		//= DT_CENTER|DT_BOTTOM,	// 图片底部居中
					this->put_ColAlignment(col, flexAlignCenterCenter);
					break;
				default:
					this->put_ColAlignment(col, flexAlignGeneral);
					break;
				}
			}
			// 设定标题内容
			this->put_Col(col);
			if(this->haveParentHeader())
			{
				if(pheader->getParentName().empty())
					this->put_TextMatrix(0, col, pheader->getName().c_str());
				else
					this->put_TextMatrix(0, col, pheader->getParentName().c_str());
				this->put_TextMatrix(1, col, pheader->getName().c_str());
				this->put_Row(0);
				this->put_CellAlignment(flexAlignCenterCenter);
				this->put_Row(1);
				this->put_CellAlignment(flexAlignCenterCenter);
			}
			else
			{
				this->put_TextMatrix(0, col, pheader->getName().c_str());
				this->put_Row(0);
				this->put_CellAlignment(flexAlignCenterCenter);
			}
		}
		//col = this->InsertColumn(idx, m_arrHeadinfo[idx]->GetName(), LVCFMT_CENTER, m_arrHeadinfo[idx]->GetWidth());
	}
	this->setSelectFlag(m_euSelectFlag);
	// 列宽可调
	this->put_AllowUserResizing(flexResizeColumns);
			
	return m_arrHeadinfo.size() == (col+1);
}
// 清除数据
inline void CNHFlexGrid::doClearData(void)
{
	doSelectedRow(-1);
	m_lOldRow = -1;
	this->put_Rows(this->get_FixedRows()+1);
	long row = this->get_Rows()-1;
	this->put_Row(row);
	for(int col = 0; col < this->get_Cols(0); col++)
	{
		this->put_Col(col);
		this->put_TextMatrix(row, col, _T(""));
		this->putref_CellPicture(NULL);
	}
	gtc::gridColHeader::PtrArrayType::iterator end = m_arrHeadinfo.end(), iter;
	for(iter = m_arrHeadinfo.begin(); iter != end; ++iter)
		(*iter)->getIDValue().clear();
	//for(std::vector<std::shared_ptr<gtc::gridColHeader>>::size_type idx = 0; idx < this->m_arrHeadinfo.size(); idx++)
	//	this->m_arrHeadinfo[idx]->getIDValue().clear();
	m_bIsEmptyData = true;
}
// 清除数据及网格信息
void CNHFlexGrid::doClear(void)
{
	this->doClearData();

	gtc::gridColHeader::free(m_arrHeadinfo);
	//gtc::gridColHeader::FreeArray(m_arrHeadinfo);
	m_mapHeadIndex.clear();
	//m_mapColors.RemoveAll();
	m_arrAdditionalField.clear();
	m_strFromValue = m_strFieldsValue = m_strSortValue = m_strWhereValue = L"";
	m_strIDFldName = L"";
	m_lIDFldIndex = -1;
	m_lOldRow = -1;
	this->ClearStructure();
	this->Clear();
	this->put_RowHeight(this->get_Rows()-1, m_lRowHeight);
}

// 选中行
void CNHFlexGrid::doSelectedRow(long vrow)
{
	if(this->m_lOldRow == vrow || m_bIsEmptyData)
		return;
	CString str;
	const long curcol = this->get_Col();
	const long currow = this->get_Row();
	if(m_lOldRow >= this->get_FixedRows() && m_lOldRow < this->get_Rows())
	{
		if(this->m_euSelectFlag == BEGridSelectFlagRowLight)
		{
			this->put_Row(m_lOldRow);
			for(int col = 0; col < this->get_Cols(0); col++)
			{
				this->put_Col(col);
				this->put_CellForeColor(0xFF000000);
			}
		}
		else
		{
			this->put_Row(m_lOldRow);
			if(this->m_bHaveSN)
			{
				str.Format(_T("%d"), m_lOldRow-this->get_FixedRows()+1);
				this->put_TextMatrix(m_lOldRow, 0, str);
			}
			for(int col = 0; col < this->get_Cols(0); col++)
			{
				this->put_Col(col);
				this->put_CellFontBold(FALSE);
			}
		}
		m_lOldRow = -1;
	}
	if(vrow >= this->get_FixedRows() && vrow < this->get_Rows())
	{
		if(this->m_euSelectFlag == BEGridSelectFlagRowLight)
		{
			this->put_Row(vrow);
			for(int col = this->get_FixedCols(); col < this->get_Cols(0); col++)
			{
				this->put_Col(col);
				//this->put_CellBackColor(0xff);
				this->put_CellForeColor(SHESTGRID_ROWSEL_FORECOLOR);
			}
			/*this->put_RowSel(vrow);
			this->put_Col(this->get_FixedCols());
			this->put_ColSel(this->get_Cols(0)-1);
			this->put_BackColorSel(0xff);
			this->put_ForeColorSel(0xff000);*/
		}
		else
		{
			this->put_Row(vrow);
			if(this->m_bHaveSN)
			{
				str.Format(_T("√%d"), vrow-this->get_FixedRows()+1);
				this->put_TextMatrix(vrow, 0, str);
			}
			for(int col = 0; col < this->get_Cols(0); col++)
			{
				this->put_Col(col);
				this->put_CellFontBold(TRUE);
			}
		}
		m_lOldRow = vrow;
	}

	this->put_Col(curcol);
	this->put_Row(currow);
}
long CNHFlexGrid::doSelectedRowByRowData(long vrowdata)
{
	if(m_bIsEmptyData)
		return -1;
	for(int row = this->get_FixedRows(); row < this->get_Rows(); row++)
	{
		if(vrowdata == this->get_RowData(row))
		{
			this->doSelectedRow(row);
			return row;
		}
	}
	return -1;
}

// 判断是否有网格数据
bool CNHFlexGrid::doIsEmpty(void)
{
	return m_bIsEmptyData;
	/*bool empty = true;
	if(this->get_Rows() == this->get_FixedRows()+1)
	{
		CString str;
		long row = this->get_Rows()-1;
		for(long col = this->get_FixedCols(); col < this->get_Cols(0); col++)
		{
			str = this->get_TextMatrix(row, col);
			if(!str.IsEmpty())
			{
				empty = false;
				break;
			}
		}
	}
	else
		empty = false;

	return empty;*/
}

// 得到查询语句
CString CNHFlexGrid::doGetSqlSelectString(LPCTSTR vwhere, LPCTSTR vsort)
{
	CString strsql, str;
	std::wstring oldwhere = getWhereValue();
	std::wstring oldsort = getSortValue();
	if(this->getFromValue().empty())
		return _T("");
	if(vwhere != NULL)
		this->setWhereValue(vwhere);
	if(vsort != NULL)
		this->setSortValue(vsort);

	long index = 0;
	gtc::gridColHeader* pheader = NULL;
	if(m_bRefreshFieldIndex)
	{
		m_strFieldsValue = L"";
		// key字段
		if(!m_strIDFldName.empty())
		{
			m_strFieldsValue += m_strIDFldName + L",";
			m_lIDFldIndex = index++;
		}
		// 附加字段如：1 AS FSelected等
		// 列字段名
		gtc::gridColHeader::PtrArrayType::iterator end = m_arrHeadinfo.end(), iter;
		int32_t idx;
		for(iter = m_arrHeadinfo.begin(), idx = 0; iter != end; ++iter, ++idx)
		//for(std::vector<std::shared_ptr<gtc::gridColHeader>>::size_type idx = 0; idx < this->m_arrHeadinfo.size(); idx++)
		{
			if((pheader = *iter) != NULL)
			{
				if(!pheader->getFieldName().empty())
				{
					m_strFieldsValue += pheader->getFieldName() + L",";
					pheader->setFieldIndex(index++);
				}
				if(!pheader->getIDFieldName().empty())
				{
					m_strFieldsValue += pheader->getIDFieldName() + L",";
					pheader->setIDFieldIndex(index++);
				}
			}
		}
	}
	// 
	if(this->m_strFieldsValue.empty() || this->m_strFromValue.empty())
		return L"";
	//this->m_strFieldsValue = this->m_strFieldsValue.Left(this->m_strFieldsValue.length()-1);
	m_strFieldsValue.erase(m_strFieldsValue.size()-1);
	strsql.Format(_T("SELECT %s FROM %s "), this->m_strFieldsValue.c_str(), this->m_strFromValue.c_str());
	// where
	if(!this->m_strWhereValue.empty())
	{
		str = this->m_strWhereValue.c_str();
		str.Trim();
		if(str.Left(6).CompareNoCase(_T("WHERE ")) == 0)
			strsql += this->m_strWhereValue.c_str();
		else
			strsql += CString(_T(" WHERE ")) + this->m_strWhereValue.c_str();
	}
	// sort 
	if(!this->m_strSortValue.empty())
	{
		str = this->m_strSortValue.c_str();
		str.Trim();
		str.MakeUpper();
		if(str.Left(6).CompareNoCase(_T("ORDER ")) == 0 && str.Find(_T("BY ")) > 0)
			strsql += this->m_strSortValue.c_str();
		else
			strsql += CString(_T(" ORDER BY ")) + this->m_strSortValue.c_str();
	}

	// 
	setWhereValue(oldwhere);
	setSortValue(oldsort);
	return strsql;
}
// 增加列信息
gtc::gridColHeader* CNHFlexGrid::doAddHeader(const wchar_t *vpname, const wchar_t *vpfldname, const wchar_t *vpidfldname, BEDataType vtype, BEAlignMode valign, int32_t vwidth, const wchar_t *vpparname)
{
	if(vpfldname == nullptr)
		return nullptr;
	long index = -1;
	gtc::gridColHeader* pheader = nullptr;
	std::wstring fld(vpfldname);
	if(fld.empty())
		return nullptr;
	//fld.MakeUpper();
	if((index = doGetHeadIndex(fld)) >= 0)// m_mapHeadIndex.Lookup(fld, index))
	{
		pheader = this->m_arrHeadinfo[index];
	}
	if(pheader == nullptr)
	{
		m_bRefreshFieldIndex = true;
		pheader = new gtc::gridColHeader(vpname, vpfldname, vtype, valign, vwidth);
		pheader->setParentName(vpparname);
		//std::shared_ptr<gtc::gridColHeader> pheader(new CASHESTHeadInfo());//vpname, vpparname, vpfldname, vpidfldname, vtype, valign, vwidth)); //= std::make_shared<gtc::gridColHeader>(vpname, vpparname, vpfldname, vpidfldname, vtype, valign, vwidth);
		m_arrHeadinfo.push_back(pheader);
		fld = pheader->getFieldName();
		gtc::convert::toUpper(fld);
		this->m_mapHeadIndex[fld] = m_arrHeadinfo.size()-1;
		//
		pheader->setIDFieldName(vpidfldname, m_mapHeadIndex);

	}
	else
	{
		pheader->setName(vpname);
		pheader->setDataType(vtype);
		pheader->setAlign(valign);
		pheader->setWidth(vwidth);

		pheader->setIDFieldName(vpidfldname, m_mapHeadIndex);
	}
	return pheader;
}

// 增加列信息（简版）；字段对齐方式根据数据类型来定
gtc::gridColHeader* CNHFlexGrid::doAddHeader(const wchar_t *vpname, const wchar_t *vpfldname, BEDataType vtype, int32_t vwidth)
{
	return doAddHeader(vpname, vpfldname, nullptr, vtype, vwidth);
}
gtc::gridColHeader* CNHFlexGrid::doAddHeader(const wchar_t *vpname, const wchar_t *vpfldname, const wchar_t *vpidfldname, BEDataType vtype, int32_t vwidth)
{
	BEAlignMode align = BEAlignModeAllRight;
	if(vtype == BEDataTypeString)
		align = BEAlignModeAllLeft;
	else if(vtype == BEDataTypeBoolean || vtype == BEDataTypeDate
		|| vtype == BEDataTypeDatetime || vtype == BEDataTypeTime
		|| vtype == BEDataTypeImage)
		align = BEAlignModeAllCenter;
	return doAddHeader(vpname, vpfldname, vpidfldname, vtype, align, vwidth, nullptr);
}


//// 增加颜色列
//CBRangeItemAdmin* CNHFlexGrid::doAddColor(LPCTSTR vpfieldname, long vmin, long vmax, long vcolor)
//{
//	if(vpfieldname == NULL || ::lstrlen(vpfieldname) <= 0)
//		return NULL;
//	CBRangeItemAdmin* pcolor = NULL;
//	gtc::gridColHeader *pheader = NULL;
//	CObject *pobj = NULL;
//	long index = -1;
//	CString fld;
//	// vpfieldname必须存在于头信息中
//	fld = vpfieldname;
//	//fld.MakeUpper();
//	if((index = doGetHeadIndex(fld)) >= 0)//m_mapHeadIndex.Lookup(fld, index))
//	{
//		pheader = this->m_arrHeadinfo[index];
//	}
//	if(pheader == NULL)
//		return NULL;
//	if(m_mapColors.Lookup(vpfieldname, pobj))
//	{
//		pcolor = dynamic_cast<CBRangeItemAdmin*>(pobj);
//	}
//	if(pcolor == NULL)
//	{
//		if((pcolor = new CBRangeItemAdmin(vpfieldname)) != NULL)
//		{
//			pcolor->Add(vmin, vmax, vcolor);
//			m_mapColors[vpfieldname] = pcolor;
//		}
//	}
//	else
//	{
//		pcolor->Add(vmin, vmax, vcolor);
//	}
//	return pcolor;
//}
// 添加附加字段，格式为：“值 AS 字段名”，该字段名必须存在于m_arrHeadinfo中
void CNHFlexGrid::doAddAdditionalField(const wchar_t *vpfldformat)
{
	std::wstring format(vpfldformat),field, str;
	gtc::convert::trim(format);
	//format.Trim();
	if(format.empty())
		return;
	str = format;
	//str.MakeUpper();
	//int pos = -1;
	//if((pos = str.Find(_T("AS"))) > 0)
	//	field = str.Mid(pos+2);
	//else
	//	return;
	//field.Trim();
	gtc::convert::toUpper(str);
	std::wstring::size_type found = str.find(L" AS ");
	if(found != std::wstring::npos)
		field = str.substr(found + 4);
	else
		return;
	gtc::convert::trim(field);
	// 查看字段名是否存在已有的附加字段中，有就退出
	for(std::vector<std::wstring>::size_type idx = 0; idx < this->m_arrAdditionalField.size(); idx++)
	{
		str = this->m_arrAdditionalField[idx];
		gtc::convert::toUpper(str);
		if(str.find(field) != std::wstring::npos)
			return;
	}
	// 该字段名必须存在于m_arrHeadinfo中
	//long index = -1;
	//if(this->m_mapHeadIndex.Lookup(field, index) && index >= 0)
	//{
	//	this->m_arrAdditionalField.Add(format);
	//}
	if(m_mapHeadIndex.find(field) != m_mapHeadIndex.cend())
		m_arrAdditionalField.push_back(format);
}

// 设定网格数据
long CNHFlexGrid::doSetData(LPDISPATCH vpdata)
{
	const long fixrows = this->get_FixedRows();
	const long fixcols = this->get_FixedCols();
	long row=-1, col=-1, fldidx;
	gtc::gridColHeader::PtrArrayType::size_type idx;
	CString str, sfield;
	POSITION pos = NULL;
	_RecordsetPtr prst = (_RecordsetPtr)vpdata;
	//CBRangeItemAdmin* pcolor = NULL;
	CObject* pobj = NULL;
	COleVariant var, varcolor;
	gtc::gridColHeader* phead = NULL;
	if(prst == NULL)
	{
		this->doClearData();
		return 0;
	}
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	ShowWindow(SW_HIDE);
	try
	{
		this->doClearData();
		row = fixrows;
		while(!prst->ADOEOF)
		{
			if(row > fixrows)
			{
				this->AddItem(NULL, _variant_t(this->get_Rows()));
			}
			col = fixcols;
			// 行序号列
			if(this->haveSN() && fixcols > 0)
			{
				str.Format(_T("%d"), row-fixrows+1);
				this->put_TextMatrix(row, 0, str); 
			}
			// 行ID
			if(m_lIDFldIndex >= 0)
			{
				var = prst->GetFields()->GetItem((long)m_lIDFldIndex)->GetValue();
				if(var.vt > 1)
				{
					var.ChangeType(VT_I4);
					this->put_RowData(row, var.lVal);
				}
			}
			else
			{
				if(!m_strIDFldName.empty())
				{
					var = prst->GetFields()->GetItem(_bstr_t(m_strIDFldName.c_str()))->GetValue();
					if(var.vt > 1)
					{
						var.ChangeType(VT_I4);
						this->put_RowData(row, var.lVal);
					}
				}
			}
			// 值<->颜色变化列
			/*if(m_mapColors.GetCount() > 0)
			{
				for(pos = m_mapColors.GetStartPosition(); pos != NULL; )
				{
					m_mapColors.GetNextAssoc(pos, sfield, pobj);
					pcolor = dynamic_cast<CBRangeItemAdmin*>(pobj);
					if(pcolor != NULL && (idx = this->doGetFieldIndex(pcolor->GetFieldName())) >= 0)
					{
						var = prst->GetFields()->GetItem(idx)->GetValue();
						if(pcolor->Find(var, varcolor) && varcolor.vt > 1)
						{
							varcolor.ChangeType(VT_UI4);
							for(col = fixcols; col < get_Cols(0); col++)
							{
								put_Row(row);
								put_Col(col);
								put_CellForeColor(var.ulVal);
							}
						}
					}
				}
			}*/
			// 字段计算
			// 设置网格数据
			for(idx = 0; idx < m_arrHeadinfo.size(); idx++)
			{
				if((phead = m_arrHeadinfo[idx]) != NULL)
				{
					if((fldidx = phead->getFieldIndex()) < 0)
					{
						if(!(sfield = phead->getFieldName().c_str()).IsEmpty())
						{
							var = prst->GetFields()->GetItem(_bstr_t(sfield))->GetValue();
							doSetCellValue(row, idx+fixcols, var);
						}
					}
					else
					{
						var = prst->GetFields()->GetItem(fldidx)->GetValue();
						doSetCellValue(row, idx+fixcols, var);
					}
					// 
					if((fldidx = phead->getIDFieldIndex()) < 0)
					{
						if(!(sfield = phead->getIDFieldName().c_str()).IsEmpty())
						{
							var = prst->GetFields()->GetItem(_bstr_t(sfield))->GetValue();
							if(var.vt > 1)
							{
								var.ChangeType(VT_BSTR);
								phead->setIDFieldValue(row-fixrows, CString(var.bstrVal));
							}
						}
					}
					else
					{
						var = prst->GetFields()->GetItem(fldidx)->GetValue();
						if(var.vt > 1)
						{
							var.ChangeType(VT_BSTR);
							phead->setIDFieldValue(row-fixrows, CString(var.bstrVal));
						}
					}
					
				}
			}
			this->put_RowHeight(row, m_lRowHeight);
			// 
			phead = NULL;
			row++;
			prst->MoveNext();
		}
		
	}
	catch(_com_error& e)
	{
		if(phead != NULL)
			str.Format(_T("%s  [%s][%s][%s]"), (TCHAR*)e.Description(), phead->getName(), phead->getFieldName(), phead->getIDFieldName());
		else
			str.Format(_T("%s"), (TCHAR*)e.Description());
		gtc::app::writeLogs(str);
	}
	catch(...)
	{
		gtc::app::lastError();
	}
	m_bIsEmptyData = row <= fixrows;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	ShowWindow(SW_SHOW);
	/*this->put_Rows(20+fixrows);
	for(INT_PTR row = fixrows; row < this->get_Rows(); row++)
	{
		for(INT_PTR col = fixcols; col < this->get_Cols(0); col++)
		{
			str.Format(_T("data_%02d"), col);
			this->doSetCellValue(row, col, COleVariant(str));
		}
	}*/
	return 0;
}

long CNHFlexGrid::doSetData(db::dataSet *pdataSet)
{
	assert(pdataSet);
	doClearData();
	if(!pdataSet) {
		return -1;
	}
	const long fixrows = this->get_FixedRows();
	const long fixcols = this->get_FixedCols();
	long row=-1, col=-1, fldidx;
	gtc::gridColHeader::PtrArrayType::size_type idx;
	gtc::gridColHeader* phead = NULL;
	std::wstring wsField;
	wchar_t pbuf[40];
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	ShowWindow(SW_HIDE);
	row = fixrows;
	if(!pdataSet->eof()) m_bIsEmptyData = false;
	while(!pdataSet->eof())
	{
		// 空表格有一个空行，所以第一行就不用了
		if(row > fixrows) AddItem(NULL, _variant_t(get_Rows()));
		col = fixcols;
		// 设定行序号
		if (haveSN() && fixcols > 0)
		{
			std::swprintf(pbuf, _countof(pbuf), L"%d", row-fixrows+1);
			put_TextMatrix(row, 0, pbuf);
		}
		// 行ID
		if (m_lIDFldIndex >= 0)
		{
			put_RowData(row, pdataSet->getInt(m_lIDFldIndex));
		}
		else if (!m_strIDFldName.empty())
		{
			put_RowData(row, pdataSet->getInt(m_strIDFldName));
		}
		// 行数据
		for (idx = 0; idx < m_arrHeadinfo.size(); ++idx)
		{
			phead = m_arrHeadinfo[idx];
			assert(phead);
			if(!phead) continue;
			// 数据
			fldidx = phead->getFieldIndex();
			if (fldidx < 0)
			{
				wsField = phead->getFieldName();
				if(!wsField.empty())doSetCellValue(row, idx+fixcols, pdataSet->getValue(wsField));
			}
			else
			{
				doSetCellValue(row, idx+fixcols, pdataSet->getValue(fldidx));
			}
			// 数据对应的ID
			fldidx = phead->getIDFieldIndex();
			if (fldidx < 0)
			{
				wsField = phead->getIDFieldName();
				if(!wsField.empty())
				{
					gtc::variant var = pdataSet->getValue(wsField);
					if(var.dataType != BEDataTypeString) var.changeType(BEDataTypeString);
					wsField = var.pwsVal ? *var.pwsVal : L"";
					phead->setIDFieldValue(row-fixrows, wsField.c_str());
				}
			}
			else
			{
				gtc::variant var = pdataSet->getValue(fldidx);
				if(var.dataType != BEDataTypeString) var.changeType(BEDataTypeString);
				wsField = var.pwsVal ? *var.pwsVal : L"";
				phead->setIDFieldValue(row-fixrows, wsField.c_str());
			}
		}
		// 
		phead = NULL;
		row++;
		pdataSet->nextRow();
	}
	//m_bIsEmptyData = row <= fixrows;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	ShowWindow(SW_SHOW);
	return 0;
}
#ifdef _USED_OFFICE_
/* 导出数据，并生成excel文件
	vfilepath:被保存数据文件全路径
	vphead:标题头
	vpoffal:标题尾
*/
void CNHFlexGrid::doOutData(LPCTSTR vfilepath, LPCTSTR vphead, LPCTSTR vpoffal)
{
	CString cellindex;
	CString path, str;
	Excel::_WorksheetPtr psheet = NULL;
	Excel::_ApplicationPtr pxls = NULL;
	Excel::RangePtr pcell = NULL;
	Excel::FontPtr pfont = NULL;
	gtc::gridColHeader* pheadinfo = nullptr;
	long visiblecols = 0;
	const long headerrows = 3;	// 标题所占行数
	const long footerrows = 3;	// 页尾所占行数
	const long footerdataspacerows = 2;	// 内容与标尾间空的行数
	try
	{
		if(vfilepath == NULL)
		{
			TCHAR szFilter[] = _T("Excel Files (*.xls)|*.xls||");
			CFileDialog dlg(FALSE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,szFilter);
			//CFileDialog dlg(FALSE);
			/*if(vfileext == 0)
			{	// excel文件
				dlg.m_ofn.lpstrFilter = _T("*.xls");
				dlg.m_ofn.lpstrDefExt = _T("xls");
			}*/
			if(dlg.DoModal() == IDOK)
				path = dlg.GetPathName();
			else
				return;
		}
		else
			path = vfilepath;
		BeginWaitCursor();
		pxls.CreateInstance(_T("Excel.Application"));
		/*pxls->Workbooks->Open(_bstr_t("c:\\xxx.xls"));
		psheet = pxls->GetActiveSheet();
		RangePtr cell = psheet->Range["A1"];*/
		pxls->Workbooks->Add((long)xlWorksheet);
		psheet = pxls->GetActiveSheet();
		if(psheet)
			psheet->Name = L"Data01";	// 工作表的名称
		// 得到可见标题列数，不含可能有的序号（固定列）
		visiblecols = 0;
		gtc::gridColHeader::PtrArrayType::iterator end = m_arrHeadinfo.end(), iter;
		for(iter = m_arrHeadinfo.begin(); iter != end; ++iter)
		//for(std::vector<std::shared_ptr<gtc::gridColHeader>>::size_type idx = 0; idx < m_arrHeadinfo.size(); idx++)
		{
			if((pheadinfo = *iter) != nullptr && pheadinfo->isVisible() && pheadinfo->getWidth() > 0)
				visiblecols++;
			//if((pheadinfo = m_arrHeadinfo[idx]) != NULL && pheadinfo->isVisible() && pheadinfo->getWidth() > 0)
			//	visiblecols++;
		}
		/*
		for(POSITION pos = m_objAdminHeads.m_listHeadinfo.GetHeadPosition(); pos != NULL; )
		{
			pheadinfo = (CMSHFHeadInfo*)m_objAdminHeads.m_listHeadinfo.GetNext(pos);
			if(pheadinfo != NULL && pheadinfo->m_bVisible && pheadinfo->m_iWidth > 0)
				visiblecols++;
		}*/
		if(psheet)
		{
			for(long row = 0; row < getRows(); row++)
			{
				for(long col = 0; col < getCols(); col++)
				{
					str = _T("");
					pheadinfo = doGetHeaderinfo(col);
					if(pheadinfo != nullptr)
					{
						if(pheadinfo->isVisible() && pheadinfo->getWidth() > 0)
						{
							if(row >= getFixedRows() && col >= getFixedCols() && (pheadinfo->getDataType() == BEDataTypeBoolean || pheadinfo->getDataType() == BEDataTypeNone))
								str = doGetCellValue(row, col).c_str();
							else
								str = getTextMatrix(row, col).c_str();
						}
					}
					else
						str = getTextMatrix(row, col).c_str();
					cellindex = office::excel::cellIndex(row+headerrows+1, col).c_str();
					//if(pheadinfo != NULL && pheadinfo->GetDataType() == ::BEDataTypeString)
					//	psheet->Range[_bstr_t(cellindex)][vtMissing]->PutNumberFormat(_bstr_t(_T("@")));
					//psheet->Range[_bstr_t(cellindex)][vtMissing]->Value = _bstr_t(str);
					//psheet->GetRange(_bstr_t(cellindex))->PutValue(_bstr_t(str));
					//psheet->GetRange(L"A1")->PutValue(L"dd");//_bstr_t(str));
					pcell = psheet->GetRange(_bstr_t(cellindex));
					if(pcell != NULL)
					{
						if(pheadinfo != NULL && pheadinfo->getDataType() == BEDataTypeString)
							pcell->PutNumberFormat(_bstr_t(_T("@")));
						if(pheadinfo != NULL)
							pcell->HorizontalAlignment = long(pheadinfo->getAlign() == BEAlignModeAllLeft?::Excel::xlLeft:(pheadinfo->getAlign() == BEAlignModeAllRight ? ::Excel::xlRight : ::Excel::xlCenter));
						//pcell->PutValue(_bstr_t(str), 1L);
						pcell->Item[1L][1L] = _bstr_t(str);
						//pcell->Item[1L][0L] = L"123";
					}
				}
			}
			/*
			// 增加序号名
			if(m_bHaveSN)
			{
				visiblecols++;
				cellindex = doGetExcelCellIndex(headerrows+1, 0);
				//cellindex.Format(_T("%c%d"), 65, 3);
				psheet->Range[_bstr_t(cellindex)]->Value = _bstr_t(L"序号");
			}
			// 追加报表标题，第一行和第二行
			if(vphead != NULL && ::lstrlen(vphead) > 0 && visiblecols > 0)
			{
				cellindex.Format(_T("%c%d:%s"), 'A', 1, doGetExcelCellIndex(1,visiblecols - 1));
				psheet->Range[_bstr_t(cellindex)]->MergeCells = true;
				cellindex.Format(_T("%c%d"), 'A', 1);
				psheet->Range[_bstr_t(cellindex)]->Value = _bstr_t(vphead);
				pcell = psheet->Range[_bstr_t(cellindex)];
				pcell->RowHeight = _variant_t(50L);
				pfont = pcell->Font;
				pfont->PutName(_bstr_t(L"黑体"));
				pfont->PutBold(_variant_t(1L));
				pfont->PutSize(_variant_t(16L));
				psheet->Range[_bstr_t(cellindex)]->HorizontalAlignment = _variant_t(long(::Excel::xlCenter));
			}
			// 追加报表尾
			if(vpoffal != NULL && ::lstrlen(vpoffal) > 0 && GetCols() > 0)
			{
				cellindex.Format(_T("%c%d:%c%d"), 'A', GetRows()+headerrows+footerdataspacerows, doGetExcelCellIndex(GetRows()+headerrows+footerdataspacerows, visiblecols - 1));
				psheet->Range[_bstr_t(cellindex)]->MergeCells = true;
				cellindex.Format(_T("%c%d"), 'A', GetRows()+headerrows+footerdataspacerows);
				psheet->Range[_bstr_t(cellindex)]->Value = _bstr_t(vpoffal);
				pcell = psheet->Range[_bstr_t(cellindex)];
				pcell->RowHeight = _variant_t(50L);
				pfont = pcell->Font;
				pfont->PutName(_bstr_t(L"黑体"));
				pfont->PutBold(_variant_t(1L));
				pfont->PutSize(_variant_t(14L));
				psheet->Range[_bstr_t(cellindex)]->HorizontalAlignment = _variant_t(long(::Excel::xlRight));
			}*/
			//psheet->SaveAs(_bstr_t(path));//.AllocSysString());
		}

	}
	catch(_com_error& e)
	{
		gtc::app::writeLogs(e.Description(), L"MSHF Outdata");
	}
	catch(...)
	{
		gtc::app::writeLogs(L"数据导出异常...", L"MSHF Outdata");
	}
	// release
	if(psheet)
	{
		psheet->SaveAs(_bstr_t(path));
		psheet.Release();
	}
	if(pxls)
	{
		pxls->Quit();
		pxls.Release();
	}
}
#endif

// 设定网格内容
inline void CNHFlexGrid::doSetCellValue(int32_t vrow, int32_t vcol, COleVariant& vval)
{
	wchar_t sbuf[40];
	std::wstring txtcell;	// 如是偶数行且m_bIsMerged=false时,要根据对齐方式加空格以防止列合并
	gtc::gridColHeader* pheader = nullptr;
	if(vcol < this->get_Cols(0) && vcol >= this->get_FixedCols() && vrow >= this->get_FixedRows() && vrow < this->get_Rows())
	{
		if(vval.vt <= 1)
		{
			this->put_TextMatrix(vrow, vcol, L"");
			return;
		}
		pheader = this->m_arrHeadinfo[vcol - this->get_FixedCols()];
		if(pheader != nullptr)
		{
			/*if(m_bHaveSN)
			{
				txtcell.Format(_T("%d"), vrow-this->get_FixedRows()+1);
				this->put_TextMatrix(vrow, 0, txtcell);
			}*/
			switch(pheader->getDataType())
			{
			case BEDataTypeBoolean:
				{
					vval.ChangeType(VT_BOOL);
					this->put_Col(vcol);
					if(CNHFlexGrid::mc_havePicture)
					{
						if(pheader->isOnlySelected() && vval.boolVal == VARIANT_TRUE)
						{
							this->put_Row(this->m_lOldRow);
							this->putref_CellPicture(CNHFlexGrid::mc_picUnSelect.GetPictureDispatch());
							this->put_Row(vrow);
							this->putref_CellPicture(CNHFlexGrid::mc_picOnly.GetPictureDispatch());
						}
						else
						{
							this->put_Row(vrow);
							this->putref_CellPicture(vval.boolVal == VARIANT_TRUE ? CNHFlexGrid::mc_picSelect.GetPictureDispatch() : CNHFlexGrid::mc_picUnSelect.GetPictureDispatch());
						}
						this->put_CellPictureAlignment(flexAlignCenterCenter);
					}
					else
					{
						if(pheader->isOnlySelected() && vval.boolVal == VARIANT_TRUE)
						{
							txtcell = CNHFlexGrid::doCorrectCellText(L"", pheader, (vrow%2)!=0);
							this->put_TextMatrix(this->m_lOldRow, vcol, txtcell.c_str());
							txtcell = CNHFlexGrid::doCorrectCellText(DEF_TEXT_SELECT, pheader, (vrow%2)!=0);
							this->put_TextMatrix(vrow, vcol, txtcell.c_str());
						}
						else
						{
							txtcell = CNHFlexGrid::doCorrectCellText((vval.boolVal == VARIANT_TRUE?DEF_TEXT_SELECT:L""), pheader, (vrow%2)!=0);
							this->put_TextMatrix(vrow, vcol, txtcell.c_str());
						}
					}
				}
				break;
			case BEDataTypeDate:
				{
					COleDateTime dt;
					if(vval.vt == VT_DATE)
						dt.m_dt = vval.date;
					else if(vval.vt == VT_BSTR)
						dt.ParseDateTime(CString(vval.bstrVal), VAR_DATEVALUEONLY);
					else if(vval.vt == VT_R8)
						dt.m_dt = vval.dblVal;
					if(dt.GetStatus() == COleDateTime::valid)
					{
						std::swprintf(sbuf, 40, L"%04d-%02d-%02d", dt.GetYear(),dt.GetMonth(),dt.GetDay());
						txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
						this->put_TextMatrix(vrow, vcol, txtcell.c_str());
					}
				}
				break;
			case BEDataTypeTime:
				{
					COleDateTime dt;
					if(vval.vt == VT_DATE)
						dt.m_dt = vval.date;
					else if(vval.vt == VT_BSTR)
						dt.ParseDateTime(CString(vval.bstrVal), VAR_TIMEVALUEONLY);
					else if(vval.vt == VT_R8)
						dt.m_dt = vval.dblVal;
					if(dt.GetStatus() == COleDateTime::valid)
					{
						std::swprintf(sbuf, 40, L"%02d:%02d:%02d", dt.GetHour(),dt.GetMinute(),dt.GetSecond());
						txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
						this->put_TextMatrix(vrow, vcol, txtcell.c_str());
					}
				}
				break;
			case BEDataTypeDatetime:
				{
					COleDateTime dt;
					if(vval.vt == VT_DATE)
						dt.m_dt = vval.date;
					else if(vval.vt == VT_BSTR)
					{
						COleDateTime date, time;
						time.ParseDateTime(CString(vval.bstrVal), VAR_TIMEVALUEONLY);
						date.ParseDateTime(CString(vval.bstrVal), VAR_DATEVALUEONLY);
						dt.SetDateTime(date.GetYear(),date.GetMonth(),date.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
					}
					else if(vval.vt == VT_R8)
						dt.m_dt = vval.dblVal;
					if(dt.GetStatus() == COleDateTime::valid)
					{
						std::swprintf(sbuf, 40, L"%04d-%02d-%02d %02d:%02d:%02d", dt.GetYear(),dt.GetMonth(),dt.GetDay(), dt.GetHour(),dt.GetMinute(),dt.GetSecond());
						txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
						this->put_TextMatrix(vrow, vcol, txtcell.c_str());
					}
				}
				break;
			case BEDataTypeFloat:
			case BEDataTypeDouble:
				{
					wchar_t format[20];
					std::swprintf(format, 20, L"%%0.%df", pheader->getDecimals());
					vval.ChangeType(VT_R4);
					std::swprintf(sbuf, 40, format, vval.fltVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeByte:
			case BEDataTypeInt16:
			case BEDataTypeInt32:
			case BEDataTypeInt64:
				{
					vval.ChangeType(VT_I4);
					std::swprintf(sbuf, 40, L"%d", vval.lVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeImage:
				break;
			default:
				{
					vval.ChangeType(VT_BSTR);
					txtcell = vval.bstrVal;
					txtcell = CNHFlexGrid::doCorrectCellText(txtcell.c_str(), pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			}
			
		}
	}
}

void CNHFlexGrid::doSetCellValue(int32_t vrow, int32_t vcol, gtc::variant var)
{
	wchar_t sbuf[40];
	std::wstring txtcell;	// 如是偶数行且m_bIsMerged=false时,要根据对齐方式加空格以防止列合并
	gtc::gridColHeader* pheader = nullptr;
	if(vcol < this->get_Cols(0) && vcol >= this->get_FixedCols() && vrow >= this->get_FixedRows() && vrow < this->get_Rows())
	{
		if(var.dataType == BEDataTypeNone)
		{
			this->put_TextMatrix(vrow, vcol, L"");
			return;
		}
		pheader = this->m_arrHeadinfo[vcol - this->get_FixedCols()];
		assert(pheader);
		if(pheader != nullptr)
		{
			/*if(m_bHaveSN)
			{
				txtcell.Format(_T("%d"), vrow-this->get_FixedRows()+1);
				this->put_TextMatrix(vrow, 0, txtcell);
			}*/
			switch(pheader->getDataType())
			{
			case BEDataTypeBoolean:
				{
					if(var.dataType != BEDataTypeBoolean) var.changeType(BEDataTypeBoolean);
					this->put_Col(vcol);
					if(CNHFlexGrid::mc_havePicture)
					{
						if(pheader->isOnlySelected() && var.bVal)
						{
							this->put_Row(this->m_lOldRow);
							this->putref_CellPicture(CNHFlexGrid::mc_picUnSelect.GetPictureDispatch());
							this->put_Row(vrow);
							this->putref_CellPicture(CNHFlexGrid::mc_picOnly.GetPictureDispatch());
						}
						else
						{
							this->put_Row(vrow);
							this->putref_CellPicture(var.bVal ? CNHFlexGrid::mc_picSelect.GetPictureDispatch() : CNHFlexGrid::mc_picUnSelect.GetPictureDispatch());
						}
						this->put_CellPictureAlignment(flexAlignCenterCenter);
					}
					else
					{
						if(pheader->isOnlySelected() && var.bVal)
						{
							txtcell = CNHFlexGrid::doCorrectCellText(L"", pheader, (vrow%2)!=0);
							this->put_TextMatrix(this->m_lOldRow, vcol, txtcell.c_str());
							txtcell = CNHFlexGrid::doCorrectCellText(DEF_TEXT_SELECT, pheader, (vrow%2)!=0);
							this->put_TextMatrix(vrow, vcol, txtcell.c_str());
						}
						else
						{
							txtcell = CNHFlexGrid::doCorrectCellText((var.bVal?DEF_TEXT_SELECT:L""), pheader, (vrow%2)!=0);
							this->put_TextMatrix(vrow, vcol, txtcell.c_str());
						}
					}
				}
				break;
			case BEDataTypeDate:
				{
					if(var.dataType != BEDataTypeDate) var.changeType(BEDataTypeDate);
					std::tm *ptm = std::localtime(&var.tmVal);
					if(ptm)
					{
						std::swprintf(sbuf, _countof(sbuf), L"%04d-%02d-%02d", ptm->tm_year, ptm->tm_mon, ptm->tm_mday);// dt.GetYear(),dt.GetMonth(),dt.GetDay());
						txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
						this->put_TextMatrix(vrow, vcol, txtcell.c_str());
					}
				}
				break;
			case BEDataTypeTime:
				{
					if(var.dataType != BEDataTypeTime) var.changeType(BEDataTypeTime);
					std::tm *ptm = std::localtime(&var.tmVal);
					if(ptm)
					{
						std::swprintf(sbuf, _countof(sbuf), L"%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);// dt.GetHour(),dt.GetMinute(),dt.GetSecond());
						txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
						this->put_TextMatrix(vrow, vcol, txtcell.c_str());
					}
				}
				break;
			case BEDataTypeDatetime:
				{
					if(var.dataType != BEDataTypeDatetime) var.changeType(BEDataTypeDatetime);
					std::tm *ptm = std::localtime(&var.tmVal);
					if(ptm)
					{
						std::swprintf(sbuf, _countof(sbuf), L"%04d-%02d-%02d %02d:%02d:%02d", ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
						txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
						this->put_TextMatrix(vrow, vcol, txtcell.c_str());
					}
				}
				break;
			case BEDataTypeFloat:
				{
					if(var.dataType != BEDataTypeFloat) var.changeType(BEDataTypeFloat);
					wchar_t format[20];
					std::swprintf(format, 20, L"%%0.%df", pheader->getDecimals());
					std::swprintf(sbuf, _countof(sbuf), format, var.fltVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeDouble:
				{
					if(var.dataType != BEDataTypeDouble) var.changeType(BEDataTypeDouble);
					wchar_t format[20];
					std::swprintf(format, 20, L"%%0.%df", pheader->getDecimals());
					std::swprintf(sbuf, _countof(sbuf), format, var.dblVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeByte:
				{
					if(var.dataType != BEDataTypeByte) var.changeType(BEDataTypeByte);
					std::swprintf(sbuf, _countof(sbuf), L"%d", var.nVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeInt16:
				{
					if(var.dataType != BEDataTypeInt16) var.changeType(BEDataTypeInt16);
					std::swprintf(sbuf, _countof(sbuf), L"%d", var.iVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeInt32:
				{
					if(var.dataType != BEDataTypeInt32) var.changeType(BEDataTypeInt32);
					std::swprintf(sbuf, _countof(sbuf), L"%d", var.lVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeInt64:
				{
					if(var.dataType != BEDataTypeInt64) var.changeType(BEDataTypeInt64);
					std::swprintf(sbuf, _countof(sbuf), L"%d", var.llVal);
					txtcell = CNHFlexGrid::doCorrectCellText(sbuf, pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			case BEDataTypeImage:
				break;
			default:
				{
					if(var.dataType != BEDataTypeString) var.changeType(BEDataTypeString);
					if(var.pwsVal) txtcell = *var.pwsVal;
					txtcell = CNHFlexGrid::doCorrectCellText(txtcell.c_str(), pheader, (vrow%2)!=0);
					this->put_TextMatrix(vrow, vcol, txtcell.c_str());
				}
				break;
			}
			
		}
	}
}
void CNHFlexGrid::doSetCellValue(int32_t vrow, const wchar_t *vpfldname, COleVariant& vval)
{
	long idx = -1;
	std::wstring fld(vpfldname);
	//fld.MakeUpper();
	if((idx = doGetHeadIndex(fld)) >= 0)//this->m_mapHeadIndex.Lookup(fld, idx) && idx >= 0)
	{
		gtc::gridColHeader* pheader = this->m_arrHeadinfo[idx];
		if(pheader != nullptr)
		{
			if(pheader->getIDFieldName() == fld)
			{
				vval.ChangeType(VT_BSTR);
				pheader->setIDFieldValue(vrow-this->get_FixedRows(), CString(vval.bstrVal)); 
			}
			else if(pheader->getFieldName() == fld)
			{
				this->doSetCellValue(vrow, idx+this->get_FixedCols(), vval);
			}
		}
	}
}
void CNHFlexGrid::doSetCellValue(int32_t vrow, const wchar_t *vpfldname, int vval)
{
	COleVariant var(long(vval), VT_I4);
	this->doSetCellValue(vrow, vpfldname, var);
}
void CNHFlexGrid::doSetCellValue(int32_t vrow, const wchar_t *vpfldname, float vval)
{
	COleVariant var(vval);
	this->doSetCellValue(vrow, vpfldname, var);
}
void CNHFlexGrid::doSetCellValue(int32_t vrow, const wchar_t *vpfldname, LPCTSTR vval)
{
	COleVariant var(vval, VT_BSTR);
	this->doSetCellValue(vrow, vpfldname, var);
}
bool CNHFlexGrid::doGetCellValue(int32_t vrow, int32_t vcol, COleVariant& vvar)
{	
	std::wstring txtcell;	// 如是偶数行且m_bIsMerged=false时,要根据对齐方式加空格以防止列合并
	gtc::gridColHeader* pheader = NULL;
	gtc::convert::toNull(vvar);
	bool bret = false;
	if(vcol < this->get_Cols(0) && vcol >= this->get_FixedCols() && vrow >= this->get_FixedRows() && vrow < this->get_Rows())
	{
		pheader = this->m_arrHeadinfo[vcol - this->get_FixedCols()];
		if(pheader != NULL)
		{
			bret = true;
			txtcell = this->get_TextMatrix(vrow, vcol);
			//txtcell.Trim();
			switch(pheader->getDataType())
			{
			case BEDataTypeBoolean:
				{
					//vvar.ChangeType(VT_BOOL);
					vvar.vt = VT_BOOL;
					if(CNHFlexGrid::mc_havePicture)
					{
						this->put_Col(vcol);
						this->put_Row(vrow);
						if(this->get_CellPicture() == CNHFlexGrid::mc_picUnSelect.GetPictureDispatch())
							vvar.boolVal = VARIANT_FALSE;
						else
							vvar.boolVal = VARIANT_TRUE;
					}
					else
					{
						vvar.boolVal = txtcell.empty()?VARIANT_FALSE:VARIANT_TRUE;
					}
				}
				break;
			case BEDataTypeDate:
				{
					//vvar.ChangeType(VT_DATE);
					vvar.vt = VT_DATE;
					COleDateTime dt;
					dt.ParseDateTime(txtcell.c_str(), VAR_DATEVALUEONLY);
					if(dt.GetStatus() == COleDateTime::valid)
						vvar.date = dt.m_dt;
				}
				break;
			case BEDataTypeTime:
				{
					//vvar.ChangeType(VT_DATE);
					vvar.vt = VT_DATE;
					COleDateTime dt;
					dt.ParseDateTime(txtcell.c_str(), VAR_TIMEVALUEONLY);
					if(dt.GetStatus() == COleDateTime::valid)
						vvar.date = dt.m_dt;
				}
				break;
			case BEDataTypeDatetime:
				{
					//vvar.ChangeType(VT_DATE);
					vvar.vt = VT_DATE;
					COleDateTime dt,dt1,dt2;
					dt1.ParseDateTime(txtcell.c_str(), VAR_DATEVALUEONLY);
					dt2.ParseDateTime(txtcell.c_str(), VAR_TIMEVALUEONLY);
					if(dt1.GetStatus() == COleDateTime::valid && dt2.GetStatus() != COleDateTime::valid)
						vvar.date = dt1.m_dt;
					else if(dt1.GetStatus() != COleDateTime::valid && dt2.GetStatus() == COleDateTime::valid)
						vvar.date = dt2.m_dt;
					else if(dt1.GetStatus() == COleDateTime::valid && dt2.GetStatus() == COleDateTime::valid)
					{
						dt.SetDateTime(dt1.GetYear(), dt1.GetMonth(), dt1.GetDay(), dt2.GetHour(), dt2.GetMinute(), dt2.GetSecond());
						vvar.date = dt.m_dt;
					}
				}
				break;
			case BEDataTypeFloat:
				{
					//vvar.ChangeType(VT_R4);
					vvar.vt = VT_R4;
					vvar.fltVal = (float)std::wcstod(txtcell.c_str(), nullptr);
					//char *pbuf = CMConvert::toCharFromWChar(txtcell.c_str());// CBConvert::ToCharFromWChar(txtcell);
					//if(pbuf != NULL)
					//{
					//	vvar.fltVal = (float)std::atof(pbuf);
					//	delete []pbuf;
					//}
				}
				break;
			case BEDataTypeDouble:
				{
					//vvar.ChangeType(VT_R8);
					vvar.vt = VT_R8;
					vvar.dblVal = std::wcstod(txtcell.c_str(), nullptr);
					//char *pbuf = CMConvert::toCharFromWChar(txtcell.c_str());// CBConvert::ToCharFromWChar(txtcell);
					//if(pbuf != NULL)
					//{
					//	vvar.dblVal = std::atof(pbuf);
					//	delete []pbuf;
					//}
				}
				break;
			case BEDataTypeByte:
				{
					//vvar.ChangeType(VT_UI1);
					vvar.vt = VT_UI1;
					vvar.bVal = (BYTE)std::wcstol(txtcell.c_str(), nullptr, 10);
					//char *pbuf = CMConvert::toCharFromWChar(txtcell.c_str());// CBConvert::ToCharFromWChar(txtcell);
					//if(pbuf != NULL)
					//{
					//	vvar.bVal = (BYTE)std::atoi(pbuf);
					//	delete[] pbuf;
					//}
				}
				break;
			case BEDataTypeInt16:
				{
					vvar.vt = VT_I2;
					vvar.iVal = (short)std::wcstol(txtcell.c_str(), NULL, 10);
				}
				break;
			case BEDataTypeInt32:
				{
					//vvar.ChangeType(VT_I4);
					vvar.vt = VT_I4;
					vvar.intVal = std::wcstol(txtcell.c_str(), NULL, 10);
					//char *pbuf = CMConvert::toCharFromWChar(txtcell.c_str());// CBConvert::ToCharFromWChar(txtcell);
					//if(pbuf != NULL)
					//{
					//	vvar.intVal = std::atoi(pbuf);
					//	delete[] pbuf;
					//}
				}
				break;
			case BEDataTypeInt64:
				{
					//vvar.ChangeType(VT_I8);
					vvar.vt = VT_I8;
					vvar.llVal = std::wcstol(txtcell.c_str(), nullptr, 10);
					//char *pbuf = CMConvert::toCharFromWChar(txtcell.c_str());// CBConvert::ToCharFromWChar(txtcell);
					//if(pbuf != NULL)
					//{
					//	vvar.llVal = std::atoll(pbuf);
					//	delete[] pbuf;
					//}
				}
				break;
			case BEDataTypeImage:
				break;
			default:
				{
					//vvar.ChangeType(VT_BSTR);
					vvar.vt = VT_BSTR;
					vvar.bstrVal = CString(txtcell.c_str()).AllocSysString();
				}
				break;
			}
			
		}
	}
	return bret;
}

bool CNHFlexGrid::doGetCellValue(int32_t vrow, const wchar_t *vpfldname, COleVariant &vvar)
{
	int32_t idx = -1;
	std::wstring fld(vpfldname);
	gtc::convert::toNull(vvar);
	if((idx = doGetHeadIndex(fld)) >= 0)//this->m_mapHeadIndex.Lookup(fld, idx) && idx >= 0)
	{
		gtc::gridColHeader* pheader = this->m_arrHeadinfo[idx];
		if(pheader != nullptr)
		{
			if(pheader->getIDFieldName() == fld)
			{
				//vvar.ChangeType(VT_BSTR);
				vvar.vt = VT_BSTR;
				vvar.bstrVal = CString(pheader->getIDFieldValue(vrow-this->get_FixedRows()).c_str()).AllocSysString();
				return true; 
			}
			else if(pheader->getFieldName() == fld)
			{
				return this->doGetCellValue(vrow, idx+this->get_FixedCols(), vvar);
			}
			else
				return false;
		}
	}
	return false;
}
std::wstring CNHFlexGrid::doGetCellValue(int32_t vrow, int32_t vcol)
{
	std::wstring txtcell;	// 如是偶数行且m_bIsMerged=false时,要根据对齐方式加空格以防止列合并
	if(vcol < this->get_Cols(0) && vcol >= this->get_FixedCols() && vrow >= this->get_FixedRows() && vrow < this->get_Rows())
	{
		COleVariant var;
		if(this->doGetCellValue(vrow, vcol, var) && var.vt > 1)
		{
			var.ChangeType(VT_BSTR);
			txtcell = var.bstrVal;
		}
		//txtcell = this->get_TextMatrix(vrow, vcol);
		//txtcell.Trim();
		gtc::convert::trim(txtcell);
	}
	return txtcell;
}
std::wstring CNHFlexGrid::doGetCellValue(int32_t vrow, LPCTSTR vpfldname)
{
	int32_t idx = -1;
	std::wstring fld(vpfldname), txtcell;
	if((idx = doGetHeadIndex(fld)) >= 0)//this->m_mapHeadIndex.Lookup(fld, idx) && idx >= 0)
	{
		gtc::gridColHeader* pheader = this->m_arrHeadinfo[idx];
		if(pheader != nullptr)
		{
			if(pheader->getIDFieldName() == fld)
			{
				txtcell = pheader->getIDFieldValue(vrow-this->get_FixedRows());
			}
			else if(pheader->getFieldName() == fld)
			{
				txtcell = this->doGetCellValue(vrow, idx+this->get_FixedCols());
			}
		}
	}
	return txtcell;
}
//// 追加移行数据；Key=字段名，Value=字段值
//void CNHFlexGrid::doAppendData(CMap<CString,LPCTSTR,COleVariant*,COleVariant*> vmapdata)
//{
//	CString key;
//	COleVariant* value = NULL;
//	for(POSITION pos = vmapdata.GetStartPosition(); pos != NULL;)
//	{
//		vmapdata.GetNextAssoc(pos, key, value);
//		//this->doSetCellValue(vrow, key, value);
//	}
//}

// 移动数据行，visnext=true下移，false上移，并返回移动后的数据行
int32_t CNHFlexGrid::doMoveRowData(int32_t vrow, bool visnext)
{
	if((visnext && vrow >= (get_Rows()-1)) || (visnext==false && vrow <= get_FixedRows()))
		return -1;
	int32_t newrow = vrow + (visnext?1:-1);
	//CStringArray arr;
	std::vector<std::wstring> arr;
	int32_t id;
	int32_t col;
	//std::wstring str, srow, snewrow;
	// temp old row
	for(col = 0; col < get_Cols(0); col++)
		arr.push_back(this->get_TextMatrix(vrow, col).GetString());
		//arr.Add(this->get_TextMatrix(vrow, col));
	id = this->get_RowData(vrow);
	// switch
	for(col = 0; col < get_Cols(0); col++)
	{
		this->put_TextMatrix(vrow, col, this->get_TextMatrix(newrow, col));
		this->put_TextMatrix(newrow, col, arr[col].c_str());
	}
	this->put_RowData(vrow, this->get_RowData(newrow));
	this->put_RowData(newrow, id);
	// 隐藏的列数据
	int32_t srow = vrow-this->get_FixedRows();
	int32_t snewrow = newrow-this->get_FixedRows();
	std::wstring str;
	gtc::gridColHeader::PtrArrayType::iterator end = m_arrHeadinfo.end(), iter;
	for(iter = m_arrHeadinfo.begin(); iter != end; ++iter)//	for(std::vector<std::shared_ptr<gtc::gridColHeader>>::size_type idx = 0; idx < m_arrHeadinfo.size(); ++idx)
	{
		std::map<int32_t, std::wstring> &map = (*iter)->getIDValue();
		if(map.size() > 0)
		{
			str = map[srow];
			map[srow] = map[snewrow];
			map[snewrow] = str;
		}
	}

	//srow.Format(_T("%d"), vrow-this->get_FixedRows());
	//snewrow.Format(_T("%d"), newrow-this->get_FixedRows());
	//for(col = 0; col < m_arrHeadinfo.size(); col++)
	//{
	//	std::map<int32_t, std::wstring> &map = m_arrHeadinfo[col]->getIDValue();
	//	if(map.size() > 0)
	//	{
	//		str = map[srow];
	//		map[srow] = map[snewrow];
	//		map[snewrow] = str;
	//	}
	//}
	this->doSelectedRow(newrow);
	return newrow;
}

//  将控件vctrlid移到cell（vrow,vcol）处
void CNHFlexGrid::doMoveControlToCell(CWnd* vpwnd, UINT vctrlid, int32_t vrow, int32_t vcol)
{
	if(vpwnd == NULL || vctrlid <= 0)
		return;
	CRect rect, rtme, rt;
	long ctrlcy;
	vpwnd->GetDlgItem(vctrlid)->ShowWindow(SW_HIDE);
	if(vrow >= this->get_FixedRows() && vrow < this->get_Rows() && vcol >= this->get_FixedCols() && vcol < this->get_Cols(0))
	{
		this->put_Row(vrow);
		this->put_Col(vcol);
		// 被移动控件的高
		vpwnd->GetDlgItem(vctrlid)->GetWindowRect(rect);
		ctrlcy = rect.Height();
		// rtme为网格控件的区域
		GetWindowRect(rtme);
		vpwnd->ScreenToClient(rtme);
		// 这时rt的区域就是单元格的区域
		rt.left = gtc::convert::toPixelXFromTwip(this->get_CellLeft());// ::Do_TwipToPixelX(GetCellLeft());
		rt.top = gtc::convert::toPixelYFromTwip(this->get_CellTop());// ::Do_TwipToPixelY(GetCellTop());
		rt.right = rt.left + gtc::convert::toPixelXFromTwip(this->get_CellWidth());// ::Do_TwipToPixelX(GetCellWidth());
		rt.bottom = rt.top + gtc::convert::toPixelYFromTwip(this->get_CellHeight());// ::Do_TwipToPixelY(GetCellHeight());
		rt.OffsetRect(rtme.left-1, rtme.top-1);

		ClientToScreen(rt);

		rect = rt;
		ScreenToClient(rect);
		vpwnd->GetDlgItem(vctrlid)->MoveWindow(rect);

		vpwnd->GetDlgItem(vctrlid)->ShowWindow(SW_SHOW);
		vpwnd->GetDlgItem(vctrlid)->SetFocus();
	}
}

// 查找数据，并返回数据行
int32_t CNHFlexGrid::doFindValue(int32_t vid)
{
	for(int32_t row = this->get_FixedRows(); row < this->get_Rows(); row++)
	{
		if(vid == this->get_RowData(row))
			return row;
	}
	return -1;
}
// 查看指定列中的数据是否已存在
bool CNHFlexGrid::doExistsValue(const wchar_t *pfldname, const wchar_t *pvalue)
{
	int32_t idx = doGetHeadIndex(pfldname);
	long col = idx + get_FixedCols();
	if(col >= 0 && col < get_Cols(0))
	{
		for(long row = get_FixedRows(); row < get_Rows(); ++row)
		{
			if(get_TextMatrix(row, col) == pvalue)
				return true;
		}
	}
	return false;
}


}