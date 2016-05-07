#include "itobjProperty.h"

//BOOST_SERIALIZATION_ASSUME_ABSTRACT(gtc::itobjProp)
//BOOST_CLASS_EXPORT(gtc::itobjPropSection)
//BOOST_CLASS_EXPORT(gtc::itobjPropBoolean)
//BOOST_CLASS_EXPORT(gtc::itobjPropSpin)


//BOOST_CLASS_EXPORT_GUID(gtc::itobjPropSection, "gtc::itobjPropSection")
//BOOST_CLASS_EXPORT_GUID(gtc::itobjPropSpin, "gtc::itobjPropSpin")
//BOOST_CLASS_EXPORT_GUID(gtc::itobjPropBoolean, "gtc::itobjPropBoolean")

namespace gtc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjProp	2013-12-05 --- >>
//	属性条目基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<int64_t, itobjProp *>	itobjProp::mc_mapProperty;
#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjProp::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjProp::allocProperty()
{
	return nullptr;
}

#endif

void itobjProp::addMapProperty(itobjProp *pprop)
{
	if(pprop)
	{
		int64_t key = pprop->getTypekey();
		key = key << 32 | pprop->getItemkey();
		mc_mapProperty[key] = pprop;
	}
}

void itobjProp::removeMapProperty(itobjProp *pprop)
{
	if(pprop)
	{
		int64_t key = pprop->getTypekey();
		key = key << 32 | pprop->getItemkey();
		mc_mapProperty.erase(key);
	}
}

itobjProp* itobjProp::findPropertyItem(int32_t typeKey, int32_t itemKey)
{
	int64_t key = typeKey;
	key = key << 32 | itemKey;
	std::map<int64_t, itobjProp *>::iterator iter = mc_mapProperty.find(key);
	if(iter != mc_mapProperty.end())
		return iter->second;
	return nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropSection	2013-12-05 --- >>
//	属性分组
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool itobjPropSection::existsProperty(const itobjProp *pProp) const
{
	bool bExists = std::find(m_vecPropertys.cbegin(), m_vecPropertys.cend(), pProp) != m_vecPropertys.cend();
	if(bExists) return true;

	itobjPropSection *pSection = nullptr;
	for (itobjProp::PtrArrayType::const_iterator iter = m_vecPropertys.cbegin(); iter != m_vecPropertys.cend(); ++iter)
	{
		 if (pSection = dynamic_cast<itobjPropSection *>(*iter))
		 {
			 bExists = pSection->existsProperty(pProp);
			 if(bExists) return bExists;
		 }
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropSpin	2013-12-05 --- >>
//	属性条目：具有取值范围的int型属性
////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjPropSpin::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropSpin::allocProperty()
{
	CMFCPropertyGridProperty *ppro = new CMFCPropertyGridProperty(m_wsCaption.c_str(), _variant_t(m_iValue), m_wsDescription.c_str());
	ppro->EnableSpinControl(true, m_iMin, m_iMax);
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropBoolean	2013-12-05 --- >>
//	属性条目：bool型属性
////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjPropBoolean::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropBoolean::allocProperty()
{
	CMFCPropertyGridProperty *ppro = new CMFCPropertyGridProperty(m_wsCaption.c_str(), _variant_t(m_bValue), m_wsDescription.c_str());
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropEdit	2013-12-05 --- >>
//	属性条目：文本型属性
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjPropEdit::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropEdit::allocProperty()
{
	CMFCPropertyGridProperty *ppro = new CMFCPropertyGridProperty(m_wsCaption.c_str(), m_wsText.c_str(), m_wsDescription.c_str());
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropColor	2013-12-05 --- >>
//	属性条目：颜色属性
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjPropColor::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropColor::allocProperty()
{
	CMFCPropertyGridColorProperty* ppro = new CMFCPropertyGridColorProperty(m_wsCaption.c_str(), m_uiColor, nullptr, m_wsCaption.c_str());
	ppro->EnableOtherButton(_T("其他..."));
	ppro->EnableAutomaticButton(_T("默认"), m_uiDefault);
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropCombo	2013-12-05 --- >>
//	属性条目：列表框属性
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjPropCombo::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropCombo::allocProperty()
{
	const entry *pentry = entry::findEntry(m_iID, m_vecItems);

	CMFCPropertyGridProperty *ppro = new CMFCPropertyGridProperty(m_wsCaption.c_str(), pentry?pentry->getName().c_str():L"", m_wsDescription.c_str());
	for(entry::PtrArrayType::size_type idx = 0; idx < m_vecItems.size(); ++idx)
		ppro->AddOption(m_vecItems[idx]->getName().c_str());
	ppro->AllowEdit(FALSE);
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropFont	2013-12-06 --- >>
//	属性条目：列表框属性
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
/*****************************************************************************************
	<< --- itobjPropFont::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropFont::allocProperty()
{
	LOGFONT lf;
	m_bsFont.toMSFont(&lf);
	CMFCPropertyGridFontProperty *ppro = new CMFCPropertyGridFontProperty(m_wsCaption.c_str(), lf, CF_EFFECTS|CF_SCREENFONTS, m_wsDescription.c_str());
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropFolder	2013-12-06 --- >>
//	属性条目：文件夹
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)

/*****************************************************************************************
	<< --- itobjPropFolder::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropFolder::allocProperty()
{
	CMFCPropertyGridFileProperty *ppro = new CMFCPropertyGridFileProperty(m_wsCaption.c_str(), m_wsFolder.c_str(), 0, m_wsDescription.c_str());
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- itobjPropFile	2013-12-06 --- >>
//	属性条目：文件选择
////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)

/*****************************************************************************************
	<< --- itobjPropFile::allocProperty	2013-12-05 --- >>
	说明：生产特定类型的属性条目；如：文本编辑型、数字型、列表型、布尔型、字体型、文件型...
	参数：
	返回值：特定类型的属性条目
*****************************************************************************************/
CMFCPropertyGridProperty* itobjPropFile::allocProperty()
{
	CMFCPropertyGridFileProperty *ppro = new CMFCPropertyGridFileProperty(m_wsCaption.c_str(), true, m_wsFile.c_str(), m_wsDefaultExt.c_str(), 0, m_wsFilter.c_str(), m_wsDescription.c_str());
	ppro->Enable(m_bEnable);
	ppro->SetData((DWORD_PTR)this);
	return ppro;
}

#endif


}	// gtc


