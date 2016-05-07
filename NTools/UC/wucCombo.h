#ifndef __WUCCOMBO_H__
#define __WUCCOMBO_H__
#include "wucContainer.h"
#include "wucList.h"
namespace wuc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	comboUC  类		2015-8-6
//	对ComboWnd控件的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class comboWnd;
class comboUC : public containerUC, public IListOwnerUC
{
	friend comboWnd;
public:
	comboUC();
	~comboUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual SIZE estimateSize(SIZE szAvailable);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void setPosition(RECT pos);
	virtual void paint(HDC hdc, const RECT &rcPaint);
	virtual void paintText(HDC hdc);
	virtual void paintStatusImage(HDC hdc);

	virtual void setEnabled(bool bEnable = true);
	virtual PCWSTR getText() const;

	PCWSTR getDropBoxAttributeList();
	void setDropBoxAttributeList(PCWSTR pstrList);
	SIZE getDropBoxSize() const;
	void setDropBoxSize(SIZE szDropBox);

	// IListOwnerUC
	virtual BSListItemInfo* getListInfo();
	virtual int getCurSel() const;
	virtual bool selectItem(int iIndex);
	virtual void Event(BSEventUC& event);

	// IContainerUC
	//virtual controlUC* getItemAt(int iIndex) const;
	//virtual int getItemIndex(controlUC* pControl) const ;
	virtual bool setItemIndex(controlUC* pControl, int iIndex);
	//virtual int getCount() const;
	virtual bool add(controlUC* pControl);
	virtual bool addAt(controlUC* pControl, int iIndex);
	virtual bool remove(controlUC* pControl);
	virtual bool removeAt(int iIndex) ;
	virtual void removeAll();

	virtual bool activate();

	RECT getTextPadding() const;
	void setTextPadding(RECT rc);
	bool getNormalImage(RECT &rc);
	void setNormalImage(RECT rcImgCell);
	void setNormalImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getHotImage(RECT &rc);
	void setHotImage(RECT rcImgCell);
	void setHotImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getPushedImage(RECT &rc);
	void setPushedImage(RECT rcImgCell);
	void setPushedImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getFocusedImage(RECT &rc);
	void setFocusedImage(RECT rcImgCell);
	void setFocusedImage(LONG left, LONG top, LONG right, LONG bottom);
	bool getDisabledImage(RECT &rc);
	void setDisabledImage(RECT rcImgCell);
	void setDisabledImage(LONG left, LONG top, LONG right, LONG bottom);

	// BSFolderViewInfo
	int getColumns() const;
	int getItemFont() const;
	void setItemFont(int iFont);
	UINT getItemTextStyle() const;
	void setItemTextStyle(UINT style);
	RECT getItemTextPadding() const;
	void setItemTextPadding(RECT rcPadding);
	DWORD getItemTextColor() const;
	void setItemTextColor(DWORD clrText);
	DWORD getItemBkColor() const;
	void setItemBkColor(DWORD clrBk);
	DWORD getItemSelectedTextColor() const;
	void setItemSelectedTextColor(DWORD clrText);
	DWORD getItemSelectedBkColor() const;
	void setItemSelectedBkColor(DWORD clrBk);
	DWORD getItemHotTextColor() const;
	void setItemHotTextColor(DWORD clrText);
	DWORD getItemHotBkColor() const;
	void setItemHotBkColor(DWORD clrBk);
	DWORD getItemCutTextColor() const;
	void setItemCutTextColor(DWORD clrText);
	DWORD getItemCutBkColor() const;
	void setItemCutBkColor(DWORD clrBk);
	DWORD getItemDisabledTextColor() const;
	void setItemDisabledTextColor(DWORD clrText);
	DWORD getItemDisabledBkColor() const;
	void setItemDisabledBkColor(DWORD clrBk);
	DWORD getItemLineColor() const;
	void setItemLineColor(DWORD clrLine);
	bool isItemShowHtml() const;
	void setItemShowHtml(bool bShowHtml = true);
	bool isAlternateBk() const;
	void setAlternateBk(bool bAlternateBk);

	gtc::image* getItemImage() const;	// 总图片
	void setItemImage(PCWSTR pwsImage);
	void setItemNormalImage(RECT rcImgCell);
	bool getItemNormalImage(RECT &rc) const;
	void setItemSelectedImage(RECT rcImgCell);
	bool getItemSelectedImage(RECT &rc) const;
	void setItemCutedImage(RECT rcImgCell);
	bool getItemCutedImage(RECT &rc) const;
	void setItemHotImage(RECT rcImgCell);
	bool getItemHotImage(RECT &rc) const;
	void setItemDisabledImage(RECT rcImgCell);
	bool getItemDisabledImage(RECT &rc) const;
protected:
	comboWnd		*m_pWindow;

	int				m_iCurSel;
	RECT			m_rcTextPadding;
	std::wstring	m_wsDropBoxAttributes;
	SIZE			m_szDropBox;
	UINT			m_uButtonState;
	BSListItemInfo	m_listInfo;

};

} // wuc

#endif	// __WUCCOMBO_H__