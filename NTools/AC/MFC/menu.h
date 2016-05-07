#pragma once
#include "AC/basedef.h"
//////////////////////////////////////////////////////////////////////////
// 菜单项对应的操作
typedef enum BEMenuAction {
	BEMenuActionNone	= 0,
	BEMenuActionAdd		,		// 新增菜单
	BEMenuActionReplace	,		// 按名称替换菜单项
	BEMenuActionDelete			// 仅移除菜单项（按名称查找）
}BEMenuAction;

namespace acm {
// 替换系统菜单项 “查看”“排序方式”“刷新”
class menuItem {
public:
	typedef std::vector<menuItem *> PtrArrayType;
protected:
	menuItem();
public:
	menuItem(PCWSTR pName, BEMenuAction active);
	//menuItem(PCWSTR pName, bool isEnabled);
	menuItem(PCWSTR pName, UINT itemID, BEMenuAction active);
	menuItem(PCWSTR pName, UINT itemID, /*UINT pos, PCWSTR pVerb, PCWSTR pHelp, */HBITMAP hbitmap, BEMenuAction active);
	virtual ~menuItem();
public:
	// 构建菜单信息
	void buildMenuItemInfo(MENUITEMINFO &mii);
	// 添加子菜单
	menuItem* addSubMenu(PCWSTR pName, UINT itemID = 0xFFFFFFFF, /*PCWSTR pVerb, PCWSTR pHelp, UINT pos = 0xFFFFFFFF, */HBITMAP hbitmap = NULL);
	//menuItem* addSubMenu(PCWSTR pName, UINT pos = 0xFFFFFFFF, HBITMAP hbitmap = NULL);
	// 根据子菜单项创建子菜单
	HMENU createSubMenu();
	// 按名匹配查找菜单项，并更新相关信息
	bool refreshItemInfo(HMENU hMenu);

	std::wstring toString() const;
public:
	const std::wstring& getName() const;
	void setName(PCWSTR pName);
	BEMenuAction getAction() const;
	void setAction(BEMenuAction action);
	UINT getPosition() const;
	void setPosition(UINT pos);
	UINT getItemID() const;
	void setItemID(UINT uiID);
	bool isEnabled() const;
	void setEnabled(bool bEnabled);
	HBITMAP getHBMPChecked() const;
	void setHBMPChecked(HBITMAP hbitmap);
	HBITMAP getHBMPUnchecked() const;
	void setHBMPUnchecked(HBITMAP hbitmap);
	const std::wstring& getHelpInfo() const;
	void setHelpInfo(PCWSTR pHelp);
	const std::wstring& getVerb() const;
	void setVerb(PCWSTR pVerb);
	bool isSeparator() const;
	bool haveSubMenu() const;
	bool isCmdMenuItem() const;
	PtrArrayType& getSubMenu();
	bool isRadioCheck() const;
	void setRadioCheck(bool bRadio = true);
	bool isChecked() const;
	void setCheckState(bool bChecked = true);
public:
	static void init(UINT itemBaseID);
	static UINT getItemBaseID();

	static void free(PtrArrayType &arrayMenuItem);
	static menuItem* find(PtrArrayType &arrayMenuItem, UINT cmdID);
protected:
	static UINT		mc_uiBaseID;		// 第一个菜单项ID
	std::wstring	m_wsName;			// 菜单名称；为""或"-"时为菜单中的分隔线
	BEMenuAction	m_beAction;			// 对菜单项的动作
	UINT			m_uiPosition;		// 菜单位置索引
	UINT			m_uiID;				// 菜单ID，基于0；SEPARATOR和含有子菜单的项无需设置ID
	HBITMAP			m_hbmpChecked;		// 选中时的图标
	HBITMAP			m_hbmpUnchecked;	// 未选中时的图标
	std::wstring	m_wsHelp;			// 提示信息
	std::wstring	m_wsVerb;			// 动作标记

	HMENU			m_hSubMenu;			// 
	PtrArrayType	m_vecSubMenu;		// 子菜单项

	UINT			m_uiType;			// 类型；目前支持有MFT_SEPARATOR、MFT_RADIOCHECK
	UINT			m_uiState;			// 状态MFS_CHECKED、MFS_UNCHECKED、MFS_ENABLED

};
}