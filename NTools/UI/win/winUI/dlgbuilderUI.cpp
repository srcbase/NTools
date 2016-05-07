#include "StdAfx.h"
#include "dlgbuilderUI.h"

namespace wui{


dialogBuilder::dialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL)
{
	//TRACE_CLASS_BEGIN(this)
}

controlUI* dialogBuilder::create(stringOrID xml, PCWSTR type, IDialogBuilderCallback* pCallback, 
	paintManagerUI* pManager, controlUI* pParent)
{
	if( HIWORD(xml.m_lpstr) != NULL ) {
		if( *(xml.m_lpstr) == L'<' ) {
			if( !m_xml.load(xml.m_lpstr) ) return NULL;
		}
		else {
			if( !m_xml.loadFromFile(xml.m_lpstr) ) return NULL;
		}
	}
	else {
		HRSRC hResource = ::FindResource(paintManagerUI::getResourceDll(), xml.m_lpstr, type);
		if( hResource == NULL ) return NULL;
		HGLOBAL hGlobal = ::LoadResource(paintManagerUI::getResourceDll(), hResource);
		if( hGlobal == NULL ) {
			FreeResource(hResource);
			return NULL;
		}

		m_pCallback = pCallback;
		if( !m_xml.loadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(paintManagerUI::getResourceDll(), hResource) )) return NULL;
		::FreeResource(hResource);
		m_pstrtype = type;
	}

	return create(pCallback, pManager);
}

controlUI* dialogBuilder::create(IDialogBuilderCallback* pCallback, paintManagerUI* pManager, controlUI* pParent)
{
	m_pCallback = pCallback;
	markupNode root = m_xml.getRoot();
	if( !root.isValid() ) return NULL;

	if( pManager ) {
		PCWSTR pstrClass = NULL;
		int nAttributes = 0;
		PCWSTR pstrName = NULL;
		PCWSTR pstrValue = NULL;
		PWSTR pstr = NULL;
		for( markupNode node = root.getChild() ; node.isValid(); node = node.getSibling() ) {
			pstrClass = node.getName();
			if( _tcscmp(pstrClass, L"Image") == 0 ) {
				nAttributes = node.getAttributeCount();
				PCWSTR pImageName = NULL;
				PCWSTR pImageResType = NULL;
				DWORD mask = 0;
				for( int i = 0; i < nAttributes; i++ ) {
					pstrName = node.getAttributeName(i);
					pstrValue = node.getAttributeValue(i);
					if( _tcscmp(pstrName, L"name") == 0 ) {
						pImageName = pstrValue;
					}
					else if( _tcscmp(pstrName, L"restype") == 0 ) {
						pImageResType = pstrValue;
					}
					else if( _tcscmp(pstrName, L"mask") == 0 ) {
						if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
						mask = _tcstoul(pstrValue, &pstr, 16);
					}
				}
				if( pImageName ) pManager->addImage(pImageName, pImageResType, mask);
			}
			else if( _tcscmp(pstrClass, L"Font") == 0 ) {
				nAttributes = node.getAttributeCount();
				PCWSTR pFontName = NULL;
				int size = 12;
				bool bold = false;
				bool underline = false;
				bool italic = false;
				bool defaultfont = false;
				for( int i = 0; i < nAttributes; i++ ) {
					pstrName = node.getAttributeName(i);
					pstrValue = node.getAttributeValue(i);
					if( _tcscmp(pstrName, L"name") == 0 ) {
						pFontName = pstrValue;
					}
					else if( _tcscmp(pstrName, L"size") == 0 ) {
						size = _tcstol(pstrValue, &pstr, 10);
					}
					else if( _tcscmp(pstrName, L"bold") == 0 ) {
						bold = (_tcscmp(pstrValue, L"true") == 0);
					}
					else if( _tcscmp(pstrName, L"underline") == 0 ) {
						underline = (_tcscmp(pstrValue, L"true") == 0);
					}
					else if( _tcscmp(pstrName, L"italic") == 0 ) {
						italic = (_tcscmp(pstrValue, L"true") == 0);
					}
					else if( _tcscmp(pstrName, L"default") == 0 ) {
						defaultfont = (_tcscmp(pstrValue, L"true") == 0);
					}
				}
				if( pFontName ) {
					pManager->addFont(pFontName, size, bold, underline, italic);
					if( defaultfont ) pManager->setDefaultFont(pFontName, size, bold, underline, italic);
				}
			}
			else if( _tcscmp(pstrClass, L"Default") == 0 ) {
				nAttributes = node.getAttributeCount();
				PCWSTR pControlName = NULL;
				PCWSTR pControlValue = NULL;
				for( int i = 0; i < nAttributes; i++ ) {
					pstrName = node.getAttributeName(i);
					pstrValue = node.getAttributeValue(i);
					if( _tcscmp(pstrName, L"name") == 0 ) {
						pControlName = pstrValue;
					}
					else if( _tcscmp(pstrName, L"value") == 0 ) {
						pControlValue = pstrValue;
					}
				}
				if( pControlName ) {
					pManager->addDefaultAttributeList(pControlName, pControlValue);
				}
			}
		}

		pstrClass = root.getName();
		if( _tcscmp(pstrClass, L"Window") == 0 ) {
			if( pManager->getPaintWindow() ) {
				int nAttributes = root.getAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					pstrName = root.getAttributeName(i);
					pstrValue = root.getAttributeValue(i);
					if( _tcscmp(pstrName, L"size") == 0 ) {
						PWSTR pstr = NULL;
						int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
						pManager->setInitSize(cx, cy);
					} 
					else if( _tcscmp(pstrName, L"sizebox") == 0 ) {
						RECT rcSizeBox = { 0 };
						PWSTR pstr = NULL;
						rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
						rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
						rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
						rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
						pManager->setSizeBox(rcSizeBox);
					}
					else if( _tcscmp(pstrName, L"caption") == 0 ) {
						RECT rcCaption = { 0 };
						PWSTR pstr = NULL;
						rcCaption.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
						rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
						rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
						rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
						pManager->setCaptionRect(rcCaption);
					}
					else if( _tcscmp(pstrName, L"roundcorner") == 0 ) {
						PWSTR pstr = NULL;
						int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
						pManager->setRoundCorner(cx, cy);
					} 
					else if( _tcscmp(pstrName, L"mininfo") == 0 ) {
						PWSTR pstr = NULL;
						int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
						pManager->setMinInfo(cx, cy);
					}
					else if( _tcscmp(pstrName, L"maxinfo") == 0 ) {
						PWSTR pstr = NULL;
						int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
						pManager->setMaxInfo(cx, cy);
					}
					else if( _tcscmp(pstrName, L"showdirty") == 0 ) {
						pManager->setShowUpdateRect(_tcscmp(pstrValue, L"true") == 0);
					} 
					else if( _tcscmp(pstrName, L"alpha") == 0 ) {
						pManager->setTransparent(_ttoi(pstrValue));
					} 
					else if( _tcscmp(pstrName, L"bktrans") == 0 ) {
						pManager->setBackgroundTransparent(_tcscmp(pstrValue, L"true") == 0);
					} 
					else if( _tcscmp(pstrName, L"disabledfontcolor") == 0 ) {
						if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
						PWSTR pstr = NULL;
						DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
						pManager->setDefaultDisabledColor(clrColor);
					} 
					else if( _tcscmp(pstrName, L"defaultfontcolor") == 0 ) {
						if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
						PWSTR pstr = NULL;
						DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
						pManager->setDefaultFontColor(clrColor);
					}
					else if( _tcscmp(pstrName, L"linkfontcolor") == 0 ) {
						if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
						PWSTR pstr = NULL;
						DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
						pManager->setDefaultLinkFontColor(clrColor);
					} 
					else if( _tcscmp(pstrName, L"linkhoverfontcolor") == 0 ) {
						if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
						PWSTR pstr = NULL;
						DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
						pManager->setDefaultLinkHoverFontColor(clrColor);
					} 
					else if( _tcscmp(pstrName, L"selectedcolor") == 0 ) {
						if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
						PWSTR pstr = NULL;
						DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
						pManager->setDefaultSelectedBkColor(clrColor);
					} 
				}
			}
		}
	}
	return _parse(&root, NULL, pManager);
}

markup* dialogBuilder::getMarkup()
{
	return &m_xml;
}

void dialogBuilder::getLastErrorMessage(PWSTR pstrMessage, SIZE_T cchMax) const
{
	return m_xml.getLastErrorMessage(pstrMessage, cchMax);
}

void dialogBuilder::getLastErrorLocation(PWSTR pstrSource, SIZE_T cchMax) const
{
	return m_xml.getLastErrorLocation(pstrSource, cchMax);
}

controlUI* dialogBuilder::_parse(markupNode* pRoot, controlUI* pParent, paintManagerUI* pManager)
{
	dialogLayoutUI* pDialogLayout = NULL;
	IContainerUI* pContainer = NULL;
	controlUI* pReturn = NULL;
	for( markupNode node = pRoot->getChild() ; node.isValid(); node = node.getSibling() ) {
		PCWSTR pstrClass = node.getName();
		if( _tcscmp(pstrClass, L"Image") == 0 || _tcscmp(pstrClass, L"Font") == 0 \
			|| _tcscmp(pstrClass, L"Default") == 0 ) continue;

		controlUI* pControl = NULL;
		if( _tcscmp(pstrClass, L"Include") == 0 ) {
			if( !node.hasAttributes() ) continue;
			int count = 1;
			PWSTR pstr = NULL;
			TCHAR szValue[500] = { 0 };
			SIZE_T cchLen = _countof(szValue) - 1;
			if ( node.getAttributeValue(L"count", szValue, cchLen) )
				count = _tcstol(szValue, &pstr, 10);
			cchLen = _countof(szValue) - 1;
			if ( !node.getAttributeValue(L"source", szValue, cchLen) ) continue;
			for ( int i = 0; i < count; i++ ) {
				dialogBuilder builder;
				if( m_pstrtype != NULL ) { // 使用资源dll，从资源中读取
					WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
					pControl = builder.create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
				}
				else {
					pControl = builder.create((PCWSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
				}
			}
			continue;
		}
		else {
			SIZE_T cchLen = _tcslen(pstrClass);
			switch( cchLen ) {
			case 4:
				if( _tcscmp(pstrClass, L"Edit") == 0 )                   pControl = new editUI;
				else if( _tcscmp(pstrClass, L"List") == 0 )              pControl = nullptr;// new listUI;
				else if( _tcscmp(pstrClass, L"Text") == 0 )              pControl = new textUI;
				break;
			case 5:
				if( _tcscmp(pstrClass, L"Combo") == 0 )                  pControl = nullptr;// new comboUI;
				else if( _tcscmp(pstrClass, L"Label") == 0 )             pControl = new labelUI;
				break;
			case 6:
				if( _tcscmp(pstrClass, L"Button") == 0 )                 pControl = new buttonUI;
				else if( _tcscmp(pstrClass, L"Option") == 0 )            pControl = new optionUI;
				else if( _tcscmp(pstrClass, L"Slider") == 0 )            pControl = new sliderUI;
				break;
			case 7:
				if( _tcscmp(pstrClass, L"Control") == 0 )                pControl = new controlUI;
				else if( _tcscmp(pstrClass, L"ActiveX") == 0 )           pControl = nullptr;// new activeXUI;
				break;
			case 8:
				if( _tcscmp(pstrClass, L"Progress") == 0 )               pControl = new progressUI;
				else if(  _tcscmp(pstrClass, L"RichEdit") == 0 )         pControl = new richEditUI;
				break;
			case 9:
				if( _tcscmp(pstrClass, L"Container") == 0 )              pControl = new containerUI;
				else if( _tcscmp(pstrClass, L"TabLayout") == 0 )         pControl = new tabLayoutUI;
				else if( _tcscmp(pstrClass, L"ScrollBar") == 0 )         pControl = new scrollBarUI; 
				break;
			case 10:
				if( _tcscmp(pstrClass, L"ListHeader") == 0 )             pControl = nullptr;// new listHeaderUI;
				else if( _tcscmp(pstrClass, L"TileLayout") == 0 )        pControl = new tileLayoutUI;
				break;
			case 12:
				if( _tcscmp(pstrClass, L"DialogLayout") == 0 )           pControl = new dialogLayoutUI;
				break;
			case 14:
				if( _tcscmp(pstrClass, L"VerticalLayout") == 0 )         pControl = new verticalLayoutUI;
				else if( _tcscmp(pstrClass, L"ListHeaderItem") == 0 )    pControl = nullptr;// new listHeaderItemUI;
				break;
			case 15:
				if( _tcscmp(pstrClass, L"ListTextElement") == 0 )        pControl = nullptr;// new listTextElementUI;
				break;
			case 16:
				if( _tcscmp(pstrClass, L"HorizontalLayout") == 0 )       pControl = new horizontalLayoutUI;
				else if( _tcscmp(pstrClass, L"ListLabelElement") == 0 )  pControl = nullptr;// new listLabelElementUI;
				break;
			case 20:
				if( _tcscmp(pstrClass, L"ListContainerElement") == 0 )   pControl = nullptr;// new listContainerElementUI;
				break;
			}
			// User-supplied control factory
			if( pControl == NULL && m_pCallback != NULL ) {
				pControl = m_pCallback->createControl(pstrClass);
			}
		}

		assert(pControl);
		if( pControl == NULL ) continue;

		// Add children
		if( node.hasChildren() ) {
			_parse(&node, pControl, pManager);
		}
		// Attach to parent
		// 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
		if( pParent != NULL ) {
			if( pContainer == NULL ) pContainer = static_cast<IContainerUI*>(pParent->getInterface(L"IContainer"));
			assert(pContainer);
			if( pContainer == NULL ) return NULL;
			if( !pContainer->add(pControl) ) {
				delete pControl;
				continue;
			}
		}
		// Init default attributes
		if( pManager ) {
			pControl->setManager(pManager, NULL, false);
			PCWSTR pDefaultAttributes = pManager->getDefaultAttributeList(pstrClass);
			if( pDefaultAttributes ) {
				pControl->applyAttributeList(pDefaultAttributes);
			}
		}
		// Process attributes
		if( node.hasAttributes() ) {
			TCHAR szValue[500] = { 0 };
			SIZE_T cchLen = _countof(szValue) - 1;
			// Set ordinary attributes
			int nAttributes = node.getAttributeCount();
			for( int i = 0; i < nAttributes; i++ ) {
				pControl->setAttribute(node.getAttributeName(i), node.getAttributeValue(i));
			}

			// Very custom attributes
			if ( node.getAttributeValue(L"stretch", szValue, cchLen) ) {
				if( pParent == NULL ) continue;

				if( pDialogLayout == NULL ) pDialogLayout = static_cast<dialogLayoutUI*>(pParent->getInterface(L"DialogLayout"));
				assert(pDialogLayout);
				if( pDialogLayout == NULL ) continue;

				UINT uMode = 0;
				if( _tcsstr(szValue, L"move_x") != NULL ) uMode |= UISTRETCH_MOVE_X;
				if( _tcsstr(szValue, L"move_y") != NULL ) uMode |= UISTRETCH_MOVE_Y;
				if( _tcsstr(szValue, L"move_xy")!= NULL ) uMode |= UISTRETCH_MOVE_X | UISTRETCH_MOVE_Y;
				if( _tcsstr(szValue, L"size_x") != NULL ) uMode |= UISTRETCH_SIZE_X;
				if( _tcsstr(szValue, L"size_y") != NULL ) uMode |= UISTRETCH_SIZE_Y;
				if( _tcsstr(szValue, L"size_xy")!= NULL ) uMode |= UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y;
				if( _tcsstr(szValue, L"group")	!= NULL ) uMode |= UISTRETCH_NEWGROUP;
				if( _tcsstr(szValue, L"line")	!= NULL ) uMode |= UISTRETCH_NEWLINE;
				pDialogLayout->setStretchMode(pControl, uMode);
			}
		}
		if( pManager ) {
			pControl->setManager(NULL, NULL, false);
		}
		// Return first item
		if( pReturn == NULL ) pReturn = pControl;
	}
	return pReturn;
}



}