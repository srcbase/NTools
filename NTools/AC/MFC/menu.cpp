#include "menu.h"
#include "KP/KPDeskDef.h"
namespace acm {

UINT	menuItem::mc_uiBaseID = 0;		// 第一个菜单项ID
menuItem::menuItem() 
	: m_wsName(L"")
	, m_uiID(0xFFFFFFFF), m_uiPosition(0xFFFFFFFF)
	, m_uiType(0x0), m_uiState(0x0)
	, m_wsVerb(L""), m_wsHelp(L"")
	, m_hbmpChecked(NULL), m_hbmpUnchecked(NULL)
	, m_beAction(BEMenuActionNone)
	, m_hSubMenu(NULL)
{

}
menuItem::menuItem(PCWSTR pName, BEMenuAction active) 
	: m_wsName(pName ? pName : L"")
	, m_uiID(0xFFFFFFFF), m_uiPosition(0xFFFFFFFF)
	, m_uiType(0x0), m_uiState(0x0)
	, m_wsVerb(L""), m_wsHelp(pName ? pName : L"")
	, m_hbmpChecked(NULL), m_hbmpUnchecked(NULL)
	, m_beAction(active)
	, m_hSubMenu(NULL)
{

}
menuItem::menuItem(PCWSTR pName, UINT itemID, BEMenuAction active)
	: m_wsName(pName ? pName : L"")
	, m_uiID(itemID), m_uiPosition(0xFFFFFFFF)
	, m_uiType(0x0), m_uiState(0x0)
	, m_wsVerb(L""), m_wsHelp(L"")
	, m_hbmpChecked(NULL), m_hbmpUnchecked(NULL)
	, m_beAction(active)
	, m_hSubMenu(NULL)
{

}
menuItem::menuItem(PCWSTR pName, UINT itemID, /*UINT pos, PCWSTR pVerb, PCWSTR pHelp, */HBITMAP hbitmap, BEMenuAction active) 
	: m_wsName(pName ? pName : L"")
	, m_uiID(itemID), m_uiPosition(0xFFFFFFFF)
	, m_uiType(0x0), m_uiState(0x0)
	, m_wsVerb(L""), m_wsHelp(L"")
	, m_hbmpChecked(hbitmap), m_hbmpUnchecked(hbitmap)
	, m_beAction(active)
	, m_hSubMenu(NULL)
{

}

menuItem::~menuItem()
{
	if(m_hbmpChecked != m_hbmpUnchecked)
	{
		if(m_hbmpChecked) DeleteObject(m_hbmpChecked);
		if(m_hbmpUnchecked) DeleteObject(m_hbmpUnchecked);
	}
	else
	{
		if(m_hbmpChecked) DeleteObject(m_hbmpChecked);
	}
	if(m_hSubMenu) ::DestroyMenu(m_hSubMenu);
	free(m_vecSubMenu);
}

void menuItem::init(UINT itemBaseID)
{
	mc_uiBaseID = itemBaseID;
}
UINT menuItem::getItemBaseID()
{
	return mc_uiBaseID;
}
const std::wstring& menuItem::getName() const
{
	return m_wsName;
}
void menuItem::setName(PCWSTR pName)
{
	m_wsName = pName ?pName :L"";
}
BEMenuAction menuItem::getAction() const
{
	return m_beAction;
}
void menuItem::setAction(BEMenuAction action)
{
	m_beAction = action;
}
UINT menuItem::getPosition() const
{
	return m_uiPosition;
}
void menuItem::setPosition(UINT pos)
{
	m_uiPosition = pos;
}
UINT menuItem::getItemID() const
{
	return m_uiID;
}
void menuItem::setItemID(UINT uiID)
{
	m_uiID = uiID;
}
bool menuItem::isEnabled() const
{
	return !(m_uiState & MFS_DISABLED);
}

void menuItem::setEnabled(bool bEnabled)
{
	if(bEnabled) m_uiState &= ~MFS_DISABLED;
	else m_uiState |= MFS_DISABLED;
}

HBITMAP menuItem::getHBMPChecked() const
{
	return m_hbmpChecked;
}
void menuItem::setHBMPChecked(HBITMAP hbitmap)
{
	m_hbmpChecked = hbitmap;
}
HBITMAP menuItem::getHBMPUnchecked() const
{
	return m_hbmpUnchecked;
}
void menuItem::setHBMPUnchecked(HBITMAP hbitmap)
{
	m_hbmpUnchecked = hbitmap;
}
const std::wstring& menuItem::getHelpInfo() const
{
	return m_wsHelp;
}
void menuItem::setHelpInfo(PCWSTR pHelp)
{
	m_wsHelp = pHelp ? pHelp :L"";
}
const std::wstring& menuItem::getVerb() const
{
	return m_wsVerb;
}
void menuItem::setVerb(PCWSTR pVerb)
{
	m_wsVerb = pVerb ? pVerb : L"";
}
bool menuItem::isSeparator() const
{
	return m_wsName.empty() || m_wsName == L"-";
}
bool menuItem::haveSubMenu() const
{
	return m_vecSubMenu.size() > 0;
}
bool menuItem::isCmdMenuItem() const
{
	return !isSeparator() && !haveSubMenu();
}

menuItem::PtrArrayType& menuItem::getSubMenu()
{
	return m_vecSubMenu;
}
bool menuItem::isRadioCheck() const
{
	return (m_uiType & MFT_RADIOCHECK) == MFT_RADIOCHECK;
}
void menuItem::setRadioCheck(bool bRadio /*= true*/)
{
	if(bRadio) m_uiType |= MFT_RADIOCHECK;
	else m_uiType &= ~MFT_RADIOCHECK;
}

bool menuItem::isChecked() const
{
	return (m_uiState & MFS_CHECKED) == MFS_CHECKED;
}
void menuItem::setCheckState(bool bChecked /*= true*/)
{
	m_uiState &= ~(MFS_CHECKED|MFS_UNCHECKED);
	m_uiState |= bChecked ? MFS_CHECKED : MFS_UNCHECKED;
}
std::wstring menuItem::toString() const
{
	std::wostringstream out;
	out << L"名称：" << m_wsName 
		<< L"\t位置：" << m_uiPosition << L"　ID：" << m_uiID 
		<< L"\t类型：" << std::hex << std::uppercase << m_uiType << L"\t状态：" << m_uiState << std::dec << std::nouppercase
		<< L"\t图标：" << (m_hbmpChecked || m_hbmpUnchecked ? L"有" : L"无")
		<< L"\t提示信息：" << m_wsHelp
		<< L"\t动作标记：" << m_wsVerb
		<< L"\t子菜单：" << (m_vecSubMenu.size()>0 ? L"有" : L"无")
		<< std::ends;
	return out.str();
}
void menuItem::free(PtrArrayType &arrayMenuItem)
{
	for (PtrArrayType::iterator iter = arrayMenuItem.begin(); iter != arrayMenuItem.end(); ++iter)
	{
		delete *iter;
	}
	arrayMenuItem.clear();
}

menuItem* menuItem::find(PtrArrayType &arrayMenuItem, UINT cmdID)
{
	for (PtrArrayType::iterator iter = arrayMenuItem.begin(); iter != arrayMenuItem.end(); ++iter)
	{
		if ((*iter)->getItemID() == cmdID) return *iter;
		if ((*iter)->haveSubMenu())
		{
			menuItem *pitem = find((*iter)->getSubMenu(), cmdID);
			if(pitem) return pitem;
		}
		
	}
	return nullptr;
}
void menuItem::buildMenuItemInfo(MENUITEMINFO &mii)
{
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	if (isSeparator())
	{
		mii.fMask = MIIM_TYPE | MIIM_ID;
		mii.fType = MFT_SEPARATOR;
		mii.wID = 0xFFFFFFFF;
		return;
	}
	mii.fMask = MIIM_STRING | MIIM_STATE | MIIM_ID;
	mii.cch = m_wsName.length();
	mii.dwTypeData = (LPWSTR)m_wsName.c_str();
	mii.fState |= isEnabled() ? MFS_ENABLED : MFS_DISABLED;
	mii.fState |= isChecked() ? MFS_CHECKED : MFS_UNCHECKED;
	if(m_vecSubMenu.size() > 0)
	{
		mii.wID = 0xFFFFFFFF;
		mii.fMask |= MIIM_SUBMENU;
		mii.hSubMenu = createSubMenu();
	}
	else
	{
		mii.wID = mc_uiBaseID + m_uiID;
	}
	if (m_hbmpChecked || m_hbmpUnchecked)
	{
		mii.fMask |= MIIM_CHECKMARKS;
		mii.hbmpChecked = m_hbmpChecked;
		mii.hbmpUnchecked = m_hbmpUnchecked;
	}
	if(isRadioCheck())
	{
		mii.fMask |= MIIM_FTYPE;
		mii.fType |= MFT_RADIOCHECK;
	}
	//SEND_LOG(L"MMM buildMii mask:%08X type:%08X state:%08X %s", mii.fMask, mii.fType, mii.fState, m_wsName.c_str());
}

menuItem* menuItem::addSubMenu(PCWSTR pName, UINT itemID/*, PCWSTR pVerb, PCWSTR pHelp, UINT pos = 0xFFFFFFFF*/, HBITMAP hbitmap /*= NULL*/)
{
	menuItem *pmi = new menuItem(pName, itemID, hbitmap, BEMenuActionAdd);
	m_vecSubMenu.push_back(pmi);
	return pmi;
}

HMENU menuItem::createSubMenu()
{
	if(m_hSubMenu) return m_hSubMenu;
	if(m_vecSubMenu.size() <= 0) return NULL;
	MENUITEMINFO mii = {0};
	m_hSubMenu = ::CreatePopupMenu();
	int pos = 0;
	for (PtrArrayType::iterator iter = m_vecSubMenu.begin(); iter != m_vecSubMenu.end(); ++iter)
	{
		(*iter)->buildMenuItemInfo(mii);
		//SEND_LOG(L"createSubMenu %d %d %s", (*iter)->getItemID(), (*iter)->getName().c_str());
		if(::InsertMenuItem(m_hSubMenu, pos, TRUE, &mii))
		{
			(*iter)->setPosition(pos++);
		}
	}
	return m_hSubMenu;
}
bool menuItem::refreshItemInfo(HMENU hMenu)
{
	assert(hMenu);
	if(!hMenu) return false;
	if(m_wsName.empty() || m_wsName == L"-") return false;
	wchar_t pname[MAX_PATH] = {0};
	for (int idx = 0; idx < GetMenuItemCount(hMenu); ++idx)
	{
		GetMenuString(hMenu, idx, pname, _countof(pname), MF_BYPOSITION);
		if (m_wsName == pname)
		{
			MENUITEMINFO mii = {0};
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_ID;
			GetMenuItemInfo(hMenu, idx, TRUE, &mii);
			setPosition(idx);
			setItemID(mii.wID);
			return true;
		}
	}
	return false;
}
}